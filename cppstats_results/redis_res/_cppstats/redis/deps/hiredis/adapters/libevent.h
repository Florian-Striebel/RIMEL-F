#include <event2/event.h>
#include "../hiredis.h"
#include "../async.h"
#define REDIS_LIBEVENT_DELETED 0x01
#define REDIS_LIBEVENT_ENTERED 0x02
typedef struct redisLibeventEvents {
redisAsyncContext *context;
struct event *ev;
struct event_base *base;
struct timeval tv;
short flags;
short state;
} redisLibeventEvents;
static void redisLibeventDestroy(redisLibeventEvents *e) {
hi_free(e);
}
static void redisLibeventHandler(int fd, short event, void *arg) {
((void)fd);
redisLibeventEvents *e = (redisLibeventEvents*)arg;
e->state |= REDIS_LIBEVENT_ENTERED;
#define CHECK_DELETED() if (e->state & REDIS_LIBEVENT_DELETED) {redisLibeventDestroy(e);return; }
if ((event & EV_TIMEOUT) && (e->state & REDIS_LIBEVENT_DELETED) == 0) {
redisAsyncHandleTimeout(e->context);
CHECK_DELETED();
}
if ((event & EV_READ) && e->context && (e->state & REDIS_LIBEVENT_DELETED) == 0) {
redisAsyncHandleRead(e->context);
CHECK_DELETED();
}
if ((event & EV_WRITE) && e->context && (e->state & REDIS_LIBEVENT_DELETED) == 0) {
redisAsyncHandleWrite(e->context);
CHECK_DELETED();
}
e->state &= ~REDIS_LIBEVENT_ENTERED;
#undef CHECK_DELETED
}
static void redisLibeventUpdate(void *privdata, short flag, int isRemove) {
redisLibeventEvents *e = (redisLibeventEvents *)privdata;
const struct timeval *tv = e->tv.tv_sec || e->tv.tv_usec ? &e->tv : NULL;
if (isRemove) {
if ((e->flags & flag) == 0) {
return;
} else {
e->flags &= ~flag;
}
} else {
if (e->flags & flag) {
return;
} else {
e->flags |= flag;
}
}
event_del(e->ev);
event_assign(e->ev, e->base, e->context->c.fd, e->flags | EV_PERSIST,
redisLibeventHandler, privdata);
event_add(e->ev, tv);
}
static void redisLibeventAddRead(void *privdata) {
redisLibeventUpdate(privdata, EV_READ, 0);
}
static void redisLibeventDelRead(void *privdata) {
redisLibeventUpdate(privdata, EV_READ, 1);
}
static void redisLibeventAddWrite(void *privdata) {
redisLibeventUpdate(privdata, EV_WRITE, 0);
}
static void redisLibeventDelWrite(void *privdata) {
redisLibeventUpdate(privdata, EV_WRITE, 1);
}
static void redisLibeventCleanup(void *privdata) {
redisLibeventEvents *e = (redisLibeventEvents*)privdata;
if (!e) {
return;
}
event_del(e->ev);
event_free(e->ev);
e->ev = NULL;
if (e->state & REDIS_LIBEVENT_ENTERED) {
e->state |= REDIS_LIBEVENT_DELETED;
} else {
redisLibeventDestroy(e);
}
}
static void redisLibeventSetTimeout(void *privdata, struct timeval tv) {
redisLibeventEvents *e = (redisLibeventEvents *)privdata;
short flags = e->flags;
e->flags = 0;
e->tv = tv;
redisLibeventUpdate(e, flags, 0);
}
static int redisLibeventAttach(redisAsyncContext *ac, struct event_base *base) {
redisContext *c = &(ac->c);
redisLibeventEvents *e;
if (ac->ev.data != NULL)
return REDIS_ERR;
e = (redisLibeventEvents*)hi_calloc(1, sizeof(*e));
if (e == NULL)
return REDIS_ERR;
e->context = ac;
ac->ev.addRead = redisLibeventAddRead;
ac->ev.delRead = redisLibeventDelRead;
ac->ev.addWrite = redisLibeventAddWrite;
ac->ev.delWrite = redisLibeventDelWrite;
ac->ev.cleanup = redisLibeventCleanup;
ac->ev.scheduleTimer = redisLibeventSetTimeout;
ac->ev.data = e;
e->ev = event_new(base, c->fd, EV_READ | EV_WRITE, redisLibeventHandler, e);
e->base = base;
return REDIS_OK;
}
