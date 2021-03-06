#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
typedef struct aeApiState {
int kqfd;
struct kevent *events;
char *eventsMask;
} aeApiState;
#define EVENT_MASK_MALLOC_SIZE(sz) (((sz) + 3) / 4)
#define EVENT_MASK_OFFSET(fd) ((fd) % 4 * 2)
#define EVENT_MASK_ENCODE(fd, mask) (((mask) & 0x3) << EVENT_MASK_OFFSET(fd))
static inline int getEventMask(const char *eventsMask, int fd) {
return (eventsMask[fd/4] >> EVENT_MASK_OFFSET(fd)) & 0x3;
}
static inline void addEventMask(char *eventsMask, int fd, int mask) {
eventsMask[fd/4] |= EVENT_MASK_ENCODE(fd, mask);
}
static inline void resetEventMask(char *eventsMask, int fd) {
eventsMask[fd/4] &= ~EVENT_MASK_ENCODE(fd, 0x3);
}
static int aeApiCreate(aeEventLoop *eventLoop) {
aeApiState *state = zmalloc(sizeof(aeApiState));
if (!state) return -1;
state->events = zmalloc(sizeof(struct kevent)*eventLoop->setsize);
if (!state->events) {
zfree(state);
return -1;
}
state->kqfd = kqueue();
if (state->kqfd == -1) {
zfree(state->events);
zfree(state);
return -1;
}
anetCloexec(state->kqfd);
state->eventsMask = zmalloc(EVENT_MASK_MALLOC_SIZE(eventLoop->setsize));
memset(state->eventsMask, 0, EVENT_MASK_MALLOC_SIZE(eventLoop->setsize));
eventLoop->apidata = state;
return 0;
}
static int aeApiResize(aeEventLoop *eventLoop, int setsize) {
aeApiState *state = eventLoop->apidata;
state->events = zrealloc(state->events, sizeof(struct kevent)*setsize);
state->eventsMask = zrealloc(state->eventsMask, EVENT_MASK_MALLOC_SIZE(setsize));
memset(state->eventsMask, 0, EVENT_MASK_MALLOC_SIZE(setsize));
return 0;
}
static void aeApiFree(aeEventLoop *eventLoop) {
aeApiState *state = eventLoop->apidata;
close(state->kqfd);
zfree(state->events);
zfree(state->eventsMask);
zfree(state);
}
static int aeApiAddEvent(aeEventLoop *eventLoop, int fd, int mask) {
aeApiState *state = eventLoop->apidata;
struct kevent ke;
if (mask & AE_READABLE) {
EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
if (kevent(state->kqfd, &ke, 1, NULL, 0, NULL) == -1) return -1;
}
if (mask & AE_WRITABLE) {
EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
if (kevent(state->kqfd, &ke, 1, NULL, 0, NULL) == -1) return -1;
}
return 0;
}
static void aeApiDelEvent(aeEventLoop *eventLoop, int fd, int mask) {
aeApiState *state = eventLoop->apidata;
struct kevent ke;
if (mask & AE_READABLE) {
EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
kevent(state->kqfd, &ke, 1, NULL, 0, NULL);
}
if (mask & AE_WRITABLE) {
EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
kevent(state->kqfd, &ke, 1, NULL, 0, NULL);
}
}
static int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp) {
aeApiState *state = eventLoop->apidata;
int retval, numevents = 0;
if (tvp != NULL) {
struct timespec timeout;
timeout.tv_sec = tvp->tv_sec;
timeout.tv_nsec = tvp->tv_usec * 1000;
retval = kevent(state->kqfd, NULL, 0, state->events, eventLoop->setsize,
&timeout);
} else {
retval = kevent(state->kqfd, NULL, 0, state->events, eventLoop->setsize,
NULL);
}
if (retval > 0) {
int j;
for (j = 0; j < retval; j++) {
struct kevent *e = state->events+j;
int fd = e->ident;
int mask = 0;
if (e->filter == EVFILT_READ) mask = AE_READABLE;
else if (e->filter == EVFILT_WRITE) mask = AE_WRITABLE;
addEventMask(state->eventsMask, fd, mask);
}
numevents = 0;
for (j = 0; j < retval; j++) {
struct kevent *e = state->events+j;
int fd = e->ident;
int mask = getEventMask(state->eventsMask, fd);
if (mask) {
eventLoop->fired[numevents].fd = fd;
eventLoop->fired[numevents].mask = mask;
resetEventMask(state->eventsMask, fd);
numevents++;
}
}
} else if (retval == -1 && errno != EINTR) {
panic("aeApiPoll: kevent, %s", strerror(errno));
}
return numevents;
}
static char *aeApiName(void) {
return "kqueue";
}
