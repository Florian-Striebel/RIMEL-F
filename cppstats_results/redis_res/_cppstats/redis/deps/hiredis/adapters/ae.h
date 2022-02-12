#include <sys/types.h>
#include <ae.h>
#include "../hiredis.h"
#include "../async.h"
typedef struct redisAeEvents {
redisAsyncContext *context;
aeEventLoop *loop;
int fd;
int reading, writing;
} redisAeEvents;
static void redisAeReadEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
((void)el); ((void)fd); ((void)mask);
redisAeEvents *e = (redisAeEvents*)privdata;
redisAsyncHandleRead(e->context);
}
static void redisAeWriteEvent(aeEventLoop *el, int fd, void *privdata, int mask) {
((void)el); ((void)fd); ((void)mask);
redisAeEvents *e = (redisAeEvents*)privdata;
redisAsyncHandleWrite(e->context);
}
static void redisAeAddRead(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (!e->reading) {
e->reading = 1;
aeCreateFileEvent(loop,e->fd,AE_READABLE,redisAeReadEvent,e);
}
}
static void redisAeDelRead(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (e->reading) {
e->reading = 0;
aeDeleteFileEvent(loop,e->fd,AE_READABLE);
}
}
static void redisAeAddWrite(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (!e->writing) {
e->writing = 1;
aeCreateFileEvent(loop,e->fd,AE_WRITABLE,redisAeWriteEvent,e);
}
}
static void redisAeDelWrite(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
aeEventLoop *loop = e->loop;
if (e->writing) {
e->writing = 0;
aeDeleteFileEvent(loop,e->fd,AE_WRITABLE);
}
}
static void redisAeCleanup(void *privdata) {
redisAeEvents *e = (redisAeEvents*)privdata;
redisAeDelRead(privdata);
redisAeDelWrite(privdata);
hi_free(e);
}
static int redisAeAttach(aeEventLoop *loop, redisAsyncContext *ac) {
redisContext *c = &(ac->c);
redisAeEvents *e;
if (ac->ev.data != NULL)
return REDIS_ERR;
e = (redisAeEvents*)hi_malloc(sizeof(*e));
if (e == NULL)
return REDIS_ERR;
e->context = ac;
e->loop = loop;
e->fd = c->fd;
e->reading = e->writing = 0;
ac->ev.addRead = redisAeAddRead;
ac->ev.delRead = redisAeDelRead;
ac->ev.addWrite = redisAeAddWrite;
ac->ev.delWrite = redisAeDelWrite;
ac->ev.cleanup = redisAeCleanup;
ac->ev.data = e;
return REDIS_OK;
}
