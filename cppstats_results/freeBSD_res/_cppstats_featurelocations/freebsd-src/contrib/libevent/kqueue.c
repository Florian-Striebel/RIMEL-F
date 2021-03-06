



























#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(EVENT__HAVE_KQUEUE)

#include <sys/types.h>
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#include <sys/queue.h>
#include <sys/event.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#if defined(EVENT__HAVE_INTTYPES_H)
#include <inttypes.h>
#endif




#if defined(EVENT__HAVE_INTTYPES_H) && !defined(__OpenBSD__) && !defined(__FreeBSD__) && !defined(__darwin__) && !defined(__APPLE__) && !defined(__CloudABI__)
#define PTR_TO_UDATA(x) ((intptr_t)(x))
#define INT_TO_UDATA(x) ((intptr_t)(x))
#else
#define PTR_TO_UDATA(x) (x)
#define INT_TO_UDATA(x) ((void*)(x))
#endif

#include "event-internal.h"
#include "log-internal.h"
#include "evmap-internal.h"
#include "event2/thread.h"
#include "evthread-internal.h"
#include "changelist-internal.h"

#include "kqueue-internal.h"

#define NEVENT 64

struct kqop {
struct kevent *changes;
int changes_size;

struct kevent *events;
int events_size;
int kq;
int notify_event_added;
pid_t pid;
};

static void kqop_free(struct kqop *kqop);

static void *kq_init(struct event_base *);
static int kq_sig_add(struct event_base *, int, short, short, void *);
static int kq_sig_del(struct event_base *, int, short, short, void *);
static int kq_dispatch(struct event_base *, struct timeval *);
static void kq_dealloc(struct event_base *);

const struct eventop kqops = {
"kqueue",
kq_init,
event_changelist_add_,
event_changelist_del_,
kq_dispatch,
kq_dealloc,
1 ,
EV_FEATURE_ET|EV_FEATURE_O1|EV_FEATURE_FDS,
EVENT_CHANGELIST_FDINFO_SIZE
};

static const struct eventop kqsigops = {
"kqueue_signal",
NULL,
kq_sig_add,
kq_sig_del,
NULL,
NULL,
1 ,
0,
0
};

static void *
kq_init(struct event_base *base)
{
int kq = -1;
struct kqop *kqueueop = NULL;

if (!(kqueueop = mm_calloc(1, sizeof(struct kqop))))
return (NULL);



if ((kq = kqueue()) == -1) {
event_warn("kqueue");
goto err;
}

kqueueop->kq = kq;

kqueueop->pid = getpid();


kqueueop->changes = mm_calloc(NEVENT, sizeof(struct kevent));
if (kqueueop->changes == NULL)
goto err;
kqueueop->events = mm_calloc(NEVENT, sizeof(struct kevent));
if (kqueueop->events == NULL)
goto err;
kqueueop->events_size = kqueueop->changes_size = NEVENT;


memset(&kqueueop->changes[0], 0, sizeof kqueueop->changes[0]);
kqueueop->changes[0].ident = -1;
kqueueop->changes[0].filter = EVFILT_READ;
kqueueop->changes[0].flags = EV_ADD;





if (kevent(kq,
kqueueop->changes, 1, kqueueop->events, NEVENT, NULL) != 1 ||
(int)kqueueop->events[0].ident != -1 ||
!(kqueueop->events[0].flags & EV_ERROR)) {
event_warn("%s: detected broken kqueue; not using.", __func__);
goto err;
}

base->evsigsel = &kqsigops;

return (kqueueop);
err:
if (kqueueop)
kqop_free(kqueueop);

return (NULL);
}

#define ADD_UDATA 0x30303

static void
kq_setup_kevent(struct kevent *out, evutil_socket_t fd, int filter, short change)
{
memset(out, 0, sizeof(struct kevent));
out->ident = fd;
out->filter = filter;

if (change & EV_CHANGE_ADD) {
out->flags = EV_ADD;


out->udata = INT_TO_UDATA(ADD_UDATA);
if (change & EV_ET)
out->flags |= EV_CLEAR;
#if defined(NOTE_EOF)

if (filter == EVFILT_READ)
out->fflags = NOTE_EOF;
#endif
} else {
EVUTIL_ASSERT(change & EV_CHANGE_DEL);
out->flags = EV_DELETE;
}
}

static int
kq_build_changes_list(const struct event_changelist *changelist,
struct kqop *kqop)
{
int i;
int n_changes = 0;

for (i = 0; i < changelist->n_changes; ++i) {
struct event_change *in_ch = &changelist->changes[i];
struct kevent *out_ch;
if (n_changes >= kqop->changes_size - 1) {
int newsize = kqop->changes_size * 2;
struct kevent *newchanges;

newchanges = mm_realloc(kqop->changes,
newsize * sizeof(struct kevent));
if (newchanges == NULL) {
event_warn("%s: realloc", __func__);
return (-1);
}
kqop->changes = newchanges;
kqop->changes_size = newsize;
}
if (in_ch->read_change) {
out_ch = &kqop->changes[n_changes++];
kq_setup_kevent(out_ch, in_ch->fd, EVFILT_READ,
in_ch->read_change);
}
if (in_ch->write_change) {
out_ch = &kqop->changes[n_changes++];
kq_setup_kevent(out_ch, in_ch->fd, EVFILT_WRITE,
in_ch->write_change);
}
}
return n_changes;
}

static int
kq_grow_events(struct kqop *kqop, size_t new_size)
{
struct kevent *newresult;

newresult = mm_realloc(kqop->events,
new_size * sizeof(struct kevent));

if (newresult) {
kqop->events = newresult;
kqop->events_size = new_size;
return 0;
} else {
return -1;
}
}

static int
kq_dispatch(struct event_base *base, struct timeval *tv)
{
struct kqop *kqop = base->evbase;
struct kevent *events = kqop->events;
struct kevent *changes;
struct timespec ts, *ts_p = NULL;
int i, n_changes, res;

if (tv != NULL) {
ts.tv_sec = tv->tv_sec;
ts.tv_nsec = tv->tv_usec * 1000;
ts_p = &ts;
}


EVUTIL_ASSERT(kqop->changes);
n_changes = kq_build_changes_list(&base->changelist, kqop);
if (n_changes < 0)
return -1;

event_changelist_remove_all_(&base->changelist, base);



changes = kqop->changes;
kqop->changes = NULL;










if (kqop->events_size < n_changes) {
int new_size = kqop->events_size;
do {
new_size *= 2;
} while (new_size < n_changes);

kq_grow_events(kqop, new_size);
events = kqop->events;
}

EVBASE_RELEASE_LOCK(base, th_base_lock);

res = kevent(kqop->kq, changes, n_changes,
events, kqop->events_size, ts_p);

EVBASE_ACQUIRE_LOCK(base, th_base_lock);

EVUTIL_ASSERT(kqop->changes == NULL);
kqop->changes = changes;

if (res == -1) {
if (errno != EINTR) {
event_warn("kevent");
return (-1);
}

return (0);
}

event_debug(("%s: kevent reports %d", __func__, res));

for (i = 0; i < res; i++) {
int which = 0;

if (events[i].flags & EV_ERROR) {
switch (events[i].data) {





case ENOENT:


case EINVAL:
continue;
#if defined(__FreeBSD__)













case ENOTCAPABLE:
continue;
#endif


case EBADF:






continue;


case EPERM:
case EPIPE:




if (events[i].udata) {


which |= EV_READ;
break;
} else {


continue;
}


default:
errno = events[i].data;
return (-1);
}
} else if (events[i].filter == EVFILT_READ) {
which |= EV_READ;
} else if (events[i].filter == EVFILT_WRITE) {
which |= EV_WRITE;
} else if (events[i].filter == EVFILT_SIGNAL) {
which |= EV_SIGNAL;
#if defined(EVFILT_USER)
} else if (events[i].filter == EVFILT_USER) {
base->is_notify_pending = 0;
#endif
}

if (!which)
continue;

if (events[i].filter == EVFILT_SIGNAL) {
evmap_signal_active_(base, events[i].ident, 1);
} else {
evmap_io_active_(base, events[i].ident, which | EV_ET);
}
}

if (res == kqop->events_size) {


kq_grow_events(kqop, kqop->events_size * 2);
}

return (0);
}

static void
kqop_free(struct kqop *kqop)
{
if (kqop->changes)
mm_free(kqop->changes);
if (kqop->events)
mm_free(kqop->events);
if (kqop->kq >= 0 && kqop->pid == getpid())
close(kqop->kq);
memset(kqop, 0, sizeof(struct kqop));
mm_free(kqop);
}

static void
kq_dealloc(struct event_base *base)
{
struct kqop *kqop = base->evbase;
evsig_dealloc_(base);
kqop_free(kqop);
}


static int
kq_sig_add(struct event_base *base, int nsignal, short old, short events, void *p)
{
struct kqop *kqop = base->evbase;
struct kevent kev;
struct timespec timeout = { 0, 0 };
(void)p;

EVUTIL_ASSERT(nsignal >= 0 && nsignal < NSIG);

memset(&kev, 0, sizeof(kev));
kev.ident = nsignal;
kev.filter = EVFILT_SIGNAL;
kev.flags = EV_ADD;




if (kevent(kqop->kq, &kev, 1, NULL, 0, &timeout) == -1)
return (-1);






if (evsig_set_handler_(base, nsignal,
nsignal == SIGCHLD ? SIG_DFL : SIG_IGN) == -1)
return (-1);

return (0);
}

static int
kq_sig_del(struct event_base *base, int nsignal, short old, short events, void *p)
{
struct kqop *kqop = base->evbase;
struct kevent kev;

struct timespec timeout = { 0, 0 };
(void)p;

EVUTIL_ASSERT(nsignal >= 0 && nsignal < NSIG);

memset(&kev, 0, sizeof(kev));
kev.ident = nsignal;
kev.filter = EVFILT_SIGNAL;
kev.flags = EV_DELETE;




if (kevent(kqop->kq, &kev, 1, NULL, 0, &timeout) == -1)
return (-1);

if (evsig_restore_handler_(base, nsignal) == -1)
return (-1);

return (0);
}






#define NOTIFY_IDENT 42

int
event_kq_add_notify_event_(struct event_base *base)
{
struct kqop *kqop = base->evbase;
#if defined(EVFILT_USER) && defined(NOTE_TRIGGER)
struct kevent kev;
struct timespec timeout = { 0, 0 };
#endif

if (kqop->notify_event_added)
return 0;

#if defined(EVFILT_USER) && defined(NOTE_TRIGGER)
memset(&kev, 0, sizeof(kev));
kev.ident = NOTIFY_IDENT;
kev.filter = EVFILT_USER;
kev.flags = EV_ADD | EV_CLEAR;

if (kevent(kqop->kq, &kev, 1, NULL, 0, &timeout) == -1) {
event_warn("kevent: adding EVFILT_USER event");
return -1;
}

kqop->notify_event_added = 1;

return 0;
#else
return -1;
#endif
}

int
event_kq_notify_base_(struct event_base *base)
{
struct kqop *kqop = base->evbase;
#if defined(EVFILT_USER) && defined(NOTE_TRIGGER)
struct kevent kev;
struct timespec timeout = { 0, 0 };
#endif
if (! kqop->notify_event_added)
return -1;

#if defined(EVFILT_USER) && defined(NOTE_TRIGGER)
memset(&kev, 0, sizeof(kev));
kev.ident = NOTIFY_IDENT;
kev.filter = EVFILT_USER;
kev.fflags = NOTE_TRIGGER;

if (kevent(kqop->kq, &kev, 1, NULL, 0, &timeout) == -1) {
event_warn("kevent: triggering EVFILT_USER event");
return -1;
}

return 0;
#else
return -1;
#endif
}

#endif
