






















































#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(EVENT__HAVE_EVENT_PORTS)

#include <sys/time.h>
#include <sys/queue.h>
#include <errno.h>
#include <poll.h>
#include <port.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "event2/thread.h"

#include "evthread-internal.h"
#include "event-internal.h"
#include "log-internal.h"
#include "evsignal-internal.h"
#include "evmap-internal.h"

#define INITIAL_EVENTS_PER_GETN 8
#define MAX_EVENTS_PER_GETN 4096






struct fd_info {

short fdi_what;



int pending_idx_plus_1;
};

#define FDI_HAS_READ(fdi) ((fdi)->fdi_what & EV_READ)
#define FDI_HAS_WRITE(fdi) ((fdi)->fdi_what & EV_WRITE)
#define FDI_HAS_EVENTS(fdi) (FDI_HAS_READ(fdi) || FDI_HAS_WRITE(fdi))
#define FDI_TO_SYSEVENTS(fdi) (FDI_HAS_READ(fdi) ? POLLIN : 0) | (FDI_HAS_WRITE(fdi) ? POLLOUT : 0)


struct evport_data {
int ed_port;

int ed_npending;

int ed_maxevents;

int *ed_pending;

port_event_t *ed_pevtlist;

};

static void* evport_init(struct event_base *);
static int evport_add(struct event_base *, int fd, short old, short events, void *);
static int evport_del(struct event_base *, int fd, short old, short events, void *);
static int evport_dispatch(struct event_base *, struct timeval *);
static void evport_dealloc(struct event_base *);
static int grow(struct evport_data *, int min_events);

const struct eventop evportops = {
"evport",
evport_init,
evport_add,
evport_del,
evport_dispatch,
evport_dealloc,
1,
0,
sizeof(struct fd_info),
};





static void*
evport_init(struct event_base *base)
{
struct evport_data *evpd;

if (!(evpd = mm_calloc(1, sizeof(struct evport_data))))
return (NULL);

if ((evpd->ed_port = port_create()) == -1) {
mm_free(evpd);
return (NULL);
}

if (grow(evpd, INITIAL_EVENTS_PER_GETN) < 0) {
close(evpd->ed_port);
mm_free(evpd);
return NULL;
}

evpd->ed_npending = 0;

evsig_init_(base);

return (evpd);
}

static int
grow(struct evport_data *data, int min_events)
{
int newsize;
int *new_pending;
port_event_t *new_pevtlist;
if (data->ed_maxevents) {
newsize = data->ed_maxevents;
do {
newsize *= 2;
} while (newsize < min_events);
} else {
newsize = min_events;
}

new_pending = mm_realloc(data->ed_pending, sizeof(int)*newsize);
if (new_pending == NULL)
return -1;
data->ed_pending = new_pending;
new_pevtlist = mm_realloc(data->ed_pevtlist, sizeof(port_event_t)*newsize);
if (new_pevtlist == NULL)
return -1;
data->ed_pevtlist = new_pevtlist;

data->ed_maxevents = newsize;
return 0;
}

#if defined(CHECK_INVARIANTS)






static void
check_evportop(struct evport_data *evpd)
{
EVUTIL_ASSERT(evpd);
EVUTIL_ASSERT(evpd->ed_port > 0);
}




static void
check_event(port_event_t* pevt)
{






EVUTIL_ASSERT(pevt->portev_source == PORT_SOURCE_FD);
}

#else
#define check_evportop(epop)
#define check_event(pevt)
#endif





static int
reassociate(struct evport_data *epdp, struct fd_info *fdip, int fd)
{
int sysevents = FDI_TO_SYSEVENTS(fdip);

if (sysevents != 0) {
if (port_associate(epdp->ed_port, PORT_SOURCE_FD,
fd, sysevents, fdip) == -1) {
event_warn("port_associate");
return (-1);
}
}

check_evportop(epdp);

return (0);
}






static int
evport_dispatch(struct event_base *base, struct timeval *tv)
{
int i, res;
struct evport_data *epdp = base->evbase;
port_event_t *pevtlist = epdp->ed_pevtlist;







int nevents = 1;






struct timespec ts;
struct timespec *ts_p = NULL;
if (tv != NULL) {
ts.tv_sec = tv->tv_sec;
ts.tv_nsec = tv->tv_usec * 1000;
ts_p = &ts;
}






for (i = 0; i < epdp->ed_npending; ++i) {
struct fd_info *fdi = NULL;
const int fd = epdp->ed_pending[i];
if (fd != -1) {


fdi = evmap_io_get_fdinfo_(&base->io, fd);
}

if (fdi != NULL && FDI_HAS_EVENTS(fdi)) {
reassociate(epdp, fdi, fd);

fdi->pending_idx_plus_1 = 0;
}
}

EVBASE_RELEASE_LOCK(base, th_base_lock);

res = port_getn(epdp->ed_port, pevtlist, epdp->ed_maxevents,
(unsigned int *) &nevents, ts_p);

EVBASE_ACQUIRE_LOCK(base, th_base_lock);

if (res == -1) {
if (errno == EINTR || errno == EAGAIN) {
return (0);
} else if (errno == ETIME) {
if (nevents == 0)
return (0);
} else {
event_warn("port_getn");
return (-1);
}
}

event_debug(("%s: port_getn reports %d events", __func__, nevents));

for (i = 0; i < nevents; ++i) {
port_event_t *pevt = &pevtlist[i];
int fd = (int) pevt->portev_object;
struct fd_info *fdi = pevt->portev_user;


check_evportop(epdp);
check_event(pevt);
epdp->ed_pending[i] = fd;
fdi->pending_idx_plus_1 = i + 1;





res = 0;
if (pevt->portev_events & (POLLERR|POLLHUP)) {
res = EV_READ | EV_WRITE;
} else {
if (pevt->portev_events & POLLIN)
res |= EV_READ;
if (pevt->portev_events & POLLOUT)
res |= EV_WRITE;
}




if (pevt->portev_events & (POLLERR|POLLHUP|POLLNVAL))
res |= EV_READ|EV_WRITE;

evmap_io_active_(base, fd, res);
}
epdp->ed_npending = nevents;

if (nevents == epdp->ed_maxevents &&
epdp->ed_maxevents < MAX_EVENTS_PER_GETN) {


grow(epdp, epdp->ed_maxevents * 2);
}

check_evportop(epdp);

return (0);
}







static int
evport_add(struct event_base *base, int fd, short old, short events, void *p)
{
struct evport_data *evpd = base->evbase;
struct fd_info *fdi = p;

check_evportop(evpd);

fdi->fdi_what |= events;

return reassociate(evpd, fdi, fd);
}





static int
evport_del(struct event_base *base, int fd, short old, short events, void *p)
{
struct evport_data *evpd = base->evbase;
struct fd_info *fdi = p;
int associated = ! fdi->pending_idx_plus_1;

check_evportop(evpd);

fdi->fdi_what &= ~(events &(EV_READ|EV_WRITE));

if (associated) {
if (!FDI_HAS_EVENTS(fdi) &&
port_dissociate(evpd->ed_port, PORT_SOURCE_FD, fd) == -1) {




if (errno != EBADFD) {
event_warn("port_dissociate");
return (-1);
}
} else {
if (FDI_HAS_EVENTS(fdi)) {
return (reassociate(evpd, fdi, fd));
}
}
} else {
if ((fdi->fdi_what & (EV_READ|EV_WRITE)) == 0) {
const int i = fdi->pending_idx_plus_1 - 1;
EVUTIL_ASSERT(evpd->ed_pending[i] == fd);
evpd->ed_pending[i] = -1;
fdi->pending_idx_plus_1 = 0;
}
}
return 0;
}


static void
evport_dealloc(struct event_base *base)
{
struct evport_data *evpd = base->evbase;

evsig_dealloc_(base);

close(evpd->ed_port);

if (evpd->ed_pending)
mm_free(evpd->ed_pending);
if (evpd->ed_pevtlist)
mm_free(evpd->ed_pevtlist);

mm_free(evpd);
}

#endif
