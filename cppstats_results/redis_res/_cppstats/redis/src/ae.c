#include "ae.h"
#include "anet.h"
#include "redisassert.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "zmalloc.h"
#include "config.h"
#if defined(HAVE_EVPORT)
#include "ae_evport.c"
#else
#if defined(HAVE_EPOLL)
#include "ae_epoll.c"
#else
#if defined(HAVE_KQUEUE)
#include "ae_kqueue.c"
#else
#include "ae_select.c"
#endif
#endif
#endif
aeEventLoop *aeCreateEventLoop(int setsize) {
aeEventLoop *eventLoop;
int i;
monotonicInit();
if ((eventLoop = zmalloc(sizeof(*eventLoop))) == NULL) goto err;
eventLoop->events = zmalloc(sizeof(aeFileEvent)*setsize);
eventLoop->fired = zmalloc(sizeof(aeFiredEvent)*setsize);
if (eventLoop->events == NULL || eventLoop->fired == NULL) goto err;
eventLoop->setsize = setsize;
eventLoop->timeEventHead = NULL;
eventLoop->timeEventNextId = 0;
eventLoop->stop = 0;
eventLoop->maxfd = -1;
eventLoop->beforesleep = NULL;
eventLoop->aftersleep = NULL;
eventLoop->flags = 0;
if (aeApiCreate(eventLoop) == -1) goto err;
for (i = 0; i < setsize; i++)
eventLoop->events[i].mask = AE_NONE;
return eventLoop;
err:
if (eventLoop) {
zfree(eventLoop->events);
zfree(eventLoop->fired);
zfree(eventLoop);
}
return NULL;
}
int aeGetSetSize(aeEventLoop *eventLoop) {
return eventLoop->setsize;
}
void aeSetDontWait(aeEventLoop *eventLoop, int noWait) {
if (noWait)
eventLoop->flags |= AE_DONT_WAIT;
else
eventLoop->flags &= ~AE_DONT_WAIT;
}
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize) {
int i;
if (setsize == eventLoop->setsize) return AE_OK;
if (eventLoop->maxfd >= setsize) return AE_ERR;
if (aeApiResize(eventLoop,setsize) == -1) return AE_ERR;
eventLoop->events = zrealloc(eventLoop->events,sizeof(aeFileEvent)*setsize);
eventLoop->fired = zrealloc(eventLoop->fired,sizeof(aeFiredEvent)*setsize);
eventLoop->setsize = setsize;
for (i = eventLoop->maxfd+1; i < setsize; i++)
eventLoop->events[i].mask = AE_NONE;
return AE_OK;
}
void aeDeleteEventLoop(aeEventLoop *eventLoop) {
aeApiFree(eventLoop);
zfree(eventLoop->events);
zfree(eventLoop->fired);
aeTimeEvent *next_te, *te = eventLoop->timeEventHead;
while (te) {
next_te = te->next;
zfree(te);
te = next_te;
}
zfree(eventLoop);
}
void aeStop(aeEventLoop *eventLoop) {
eventLoop->stop = 1;
}
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
aeFileProc *proc, void *clientData)
{
if (fd >= eventLoop->setsize) {
errno = ERANGE;
return AE_ERR;
}
aeFileEvent *fe = &eventLoop->events[fd];
if (aeApiAddEvent(eventLoop, fd, mask) == -1)
return AE_ERR;
fe->mask |= mask;
if (mask & AE_READABLE) fe->rfileProc = proc;
if (mask & AE_WRITABLE) fe->wfileProc = proc;
fe->clientData = clientData;
if (fd > eventLoop->maxfd)
eventLoop->maxfd = fd;
return AE_OK;
}
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask)
{
if (fd >= eventLoop->setsize) return;
aeFileEvent *fe = &eventLoop->events[fd];
if (fe->mask == AE_NONE) return;
if (mask & AE_WRITABLE) mask |= AE_BARRIER;
aeApiDelEvent(eventLoop, fd, mask);
fe->mask = fe->mask & (~mask);
if (fd == eventLoop->maxfd && fe->mask == AE_NONE) {
int j;
for (j = eventLoop->maxfd-1; j >= 0; j--)
if (eventLoop->events[j].mask != AE_NONE) break;
eventLoop->maxfd = j;
}
}
void *aeGetFileClientData(aeEventLoop *eventLoop, int fd) {
if (fd >= eventLoop->setsize) return NULL;
aeFileEvent *fe = &eventLoop->events[fd];
if (fe->mask == AE_NONE) return NULL;
return fe->clientData;
}
int aeGetFileEvents(aeEventLoop *eventLoop, int fd) {
if (fd >= eventLoop->setsize) return 0;
aeFileEvent *fe = &eventLoop->events[fd];
return fe->mask;
}
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
aeTimeProc *proc, void *clientData,
aeEventFinalizerProc *finalizerProc)
{
long long id = eventLoop->timeEventNextId++;
aeTimeEvent *te;
te = zmalloc(sizeof(*te));
if (te == NULL) return AE_ERR;
te->id = id;
te->when = getMonotonicUs() + milliseconds * 1000;
te->timeProc = proc;
te->finalizerProc = finalizerProc;
te->clientData = clientData;
te->prev = NULL;
te->next = eventLoop->timeEventHead;
te->refcount = 0;
if (te->next)
te->next->prev = te;
eventLoop->timeEventHead = te;
return id;
}
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id)
{
aeTimeEvent *te = eventLoop->timeEventHead;
while(te) {
if (te->id == id) {
te->id = AE_DELETED_EVENT_ID;
return AE_OK;
}
te = te->next;
}
return AE_ERR;
}
static int64_t usUntilEarliestTimer(aeEventLoop *eventLoop) {
aeTimeEvent *te = eventLoop->timeEventHead;
if (te == NULL) return -1;
aeTimeEvent *earliest = NULL;
while (te) {
if (!earliest || te->when < earliest->when)
earliest = te;
te = te->next;
}
monotime now = getMonotonicUs();
return (now >= earliest->when) ? 0 : earliest->when - now;
}
static int processTimeEvents(aeEventLoop *eventLoop) {
int processed = 0;
aeTimeEvent *te;
long long maxId;
te = eventLoop->timeEventHead;
maxId = eventLoop->timeEventNextId-1;
monotime now = getMonotonicUs();
while(te) {
long long id;
if (te->id == AE_DELETED_EVENT_ID) {
aeTimeEvent *next = te->next;
if (te->refcount) {
te = next;
continue;
}
if (te->prev)
te->prev->next = te->next;
else
eventLoop->timeEventHead = te->next;
if (te->next)
te->next->prev = te->prev;
if (te->finalizerProc) {
te->finalizerProc(eventLoop, te->clientData);
now = getMonotonicUs();
}
zfree(te);
te = next;
continue;
}
if (te->id > maxId) {
te = te->next;
continue;
}
if (te->when <= now) {
int retval;
id = te->id;
te->refcount++;
retval = te->timeProc(eventLoop, id, te->clientData);
te->refcount--;
processed++;
now = getMonotonicUs();
if (retval != AE_NOMORE) {
te->when = now + retval * 1000;
} else {
te->id = AE_DELETED_EVENT_ID;
}
}
te = te->next;
}
return processed;
}
int aeProcessEvents(aeEventLoop *eventLoop, int flags)
{
int processed = 0, numevents;
if (!(flags & AE_TIME_EVENTS) && !(flags & AE_FILE_EVENTS)) return 0;
if (eventLoop->maxfd != -1 ||
((flags & AE_TIME_EVENTS) && !(flags & AE_DONT_WAIT))) {
int j;
struct timeval tv, *tvp;
int64_t usUntilTimer = -1;
if (flags & AE_TIME_EVENTS && !(flags & AE_DONT_WAIT))
usUntilTimer = usUntilEarliestTimer(eventLoop);
if (usUntilTimer >= 0) {
tv.tv_sec = usUntilTimer / 1000000;
tv.tv_usec = usUntilTimer % 1000000;
tvp = &tv;
} else {
if (flags & AE_DONT_WAIT) {
tv.tv_sec = tv.tv_usec = 0;
tvp = &tv;
} else {
tvp = NULL;
}
}
if (eventLoop->flags & AE_DONT_WAIT) {
tv.tv_sec = tv.tv_usec = 0;
tvp = &tv;
}
if (eventLoop->beforesleep != NULL && flags & AE_CALL_BEFORE_SLEEP)
eventLoop->beforesleep(eventLoop);
numevents = aeApiPoll(eventLoop, tvp);
if (eventLoop->aftersleep != NULL && flags & AE_CALL_AFTER_SLEEP)
eventLoop->aftersleep(eventLoop);
for (j = 0; j < numevents; j++) {
int fd = eventLoop->fired[j].fd;
aeFileEvent *fe = &eventLoop->events[fd];
int mask = eventLoop->fired[j].mask;
int fired = 0;
int invert = fe->mask & AE_BARRIER;
if (!invert && fe->mask & mask & AE_READABLE) {
fe->rfileProc(eventLoop,fd,fe->clientData,mask);
fired++;
fe = &eventLoop->events[fd];
}
if (fe->mask & mask & AE_WRITABLE) {
if (!fired || fe->wfileProc != fe->rfileProc) {
fe->wfileProc(eventLoop,fd,fe->clientData,mask);
fired++;
}
}
if (invert) {
fe = &eventLoop->events[fd];
if ((fe->mask & mask & AE_READABLE) &&
(!fired || fe->wfileProc != fe->rfileProc))
{
fe->rfileProc(eventLoop,fd,fe->clientData,mask);
fired++;
}
}
processed++;
}
}
if (flags & AE_TIME_EVENTS)
processed += processTimeEvents(eventLoop);
return processed;
}
int aeWait(int fd, int mask, long long milliseconds) {
struct pollfd pfd;
int retmask = 0, retval;
memset(&pfd, 0, sizeof(pfd));
pfd.fd = fd;
if (mask & AE_READABLE) pfd.events |= POLLIN;
if (mask & AE_WRITABLE) pfd.events |= POLLOUT;
if ((retval = poll(&pfd, 1, milliseconds))== 1) {
if (pfd.revents & POLLIN) retmask |= AE_READABLE;
if (pfd.revents & POLLOUT) retmask |= AE_WRITABLE;
if (pfd.revents & POLLERR) retmask |= AE_WRITABLE;
if (pfd.revents & POLLHUP) retmask |= AE_WRITABLE;
return retmask;
} else {
return retval;
}
}
void aeMain(aeEventLoop *eventLoop) {
eventLoop->stop = 0;
while (!eventLoop->stop) {
aeProcessEvents(eventLoop, AE_ALL_EVENTS|
AE_CALL_BEFORE_SLEEP|
AE_CALL_AFTER_SLEEP);
}
}
char *aeGetApiName(void) {
return aeApiName();
}
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep) {
eventLoop->beforesleep = beforesleep;
}
void aeSetAfterSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *aftersleep) {
eventLoop->aftersleep = aftersleep;
}
