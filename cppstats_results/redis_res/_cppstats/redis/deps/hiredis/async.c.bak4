






























#include "fmacros.h"
#include "alloc.h"
#include <stdlib.h>
#include <string.h>
#if !defined(_MSC_VER)
#include <strings.h>
#endif
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include "async.h"
#include "net.h"
#include "dict.c"
#include "sds.h"
#include "win32.h"

#include "async_private.h"


int __redisAppendCommand(redisContext *c, const char *cmd, size_t len);
void __redisSetError(redisContext *c, int type, const char *str);


static unsigned int callbackHash(const void *key) {
return dictGenHashFunction((const unsigned char *)key,
hi_sdslen((const hisds)key));
}

static void *callbackValDup(void *privdata, const void *src) {
((void) privdata);
redisCallback *dup;

dup = hi_malloc(sizeof(*dup));
if (dup == NULL)
return NULL;

memcpy(dup,src,sizeof(*dup));
return dup;
}

static int callbackKeyCompare(void *privdata, const void *key1, const void *key2) {
int l1, l2;
((void) privdata);

l1 = hi_sdslen((const hisds)key1);
l2 = hi_sdslen((const hisds)key2);
if (l1 != l2) return 0;
return memcmp(key1,key2,l1) == 0;
}

static void callbackKeyDestructor(void *privdata, void *key) {
((void) privdata);
hi_sdsfree((hisds)key);
}

static void callbackValDestructor(void *privdata, void *val) {
((void) privdata);
hi_free(val);
}

static dictType callbackDict = {
callbackHash,
NULL,
callbackValDup,
callbackKeyCompare,
callbackKeyDestructor,
callbackValDestructor
};

static redisAsyncContext *redisAsyncInitialize(redisContext *c) {
redisAsyncContext *ac;
dict *channels = NULL, *patterns = NULL;

channels = dictCreate(&callbackDict,NULL);
if (channels == NULL)
goto oom;

patterns = dictCreate(&callbackDict,NULL);
if (patterns == NULL)
goto oom;

ac = hi_realloc(c,sizeof(redisAsyncContext));
if (ac == NULL)
goto oom;

c = &(ac->c);




c->flags &= ~REDIS_CONNECTED;

ac->err = 0;
ac->errstr = NULL;
ac->data = NULL;
ac->dataCleanup = NULL;

ac->ev.data = NULL;
ac->ev.addRead = NULL;
ac->ev.delRead = NULL;
ac->ev.addWrite = NULL;
ac->ev.delWrite = NULL;
ac->ev.cleanup = NULL;
ac->ev.scheduleTimer = NULL;

ac->onConnect = NULL;
ac->onDisconnect = NULL;

ac->replies.head = NULL;
ac->replies.tail = NULL;
ac->sub.invalid.head = NULL;
ac->sub.invalid.tail = NULL;
ac->sub.channels = channels;
ac->sub.patterns = patterns;

return ac;
oom:
if (channels) dictRelease(channels);
if (patterns) dictRelease(patterns);
return NULL;
}



static void __redisAsyncCopyError(redisAsyncContext *ac) {
if (!ac)
return;

redisContext *c = &(ac->c);
ac->err = c->err;
ac->errstr = c->errstr;
}

redisAsyncContext *redisAsyncConnectWithOptions(const redisOptions *options) {
redisOptions myOptions = *options;
redisContext *c;
redisAsyncContext *ac;



myOptions.push_cb = NULL;
myOptions.options |= REDIS_OPT_NO_PUSH_AUTOFREE;

myOptions.options |= REDIS_OPT_NONBLOCK;
c = redisConnectWithOptions(&myOptions);
if (c == NULL) {
return NULL;
}

ac = redisAsyncInitialize(c);
if (ac == NULL) {
redisFree(c);
return NULL;
}


redisAsyncSetPushCallback(ac, myOptions.async_push_cb);

__redisAsyncCopyError(ac);
return ac;
}

redisAsyncContext *redisAsyncConnect(const char *ip, int port) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
return redisAsyncConnectWithOptions(&options);
}

redisAsyncContext *redisAsyncConnectBind(const char *ip, int port,
const char *source_addr) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
options.endpoint.tcp.source_addr = source_addr;
return redisAsyncConnectWithOptions(&options);
}

redisAsyncContext *redisAsyncConnectBindWithReuse(const char *ip, int port,
const char *source_addr) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
options.options |= REDIS_OPT_REUSEADDR;
options.endpoint.tcp.source_addr = source_addr;
return redisAsyncConnectWithOptions(&options);
}

redisAsyncContext *redisAsyncConnectUnix(const char *path) {
redisOptions options = {0};
REDIS_OPTIONS_SET_UNIX(&options, path);
return redisAsyncConnectWithOptions(&options);
}

int redisAsyncSetConnectCallback(redisAsyncContext *ac, redisConnectCallback *fn) {
if (ac->onConnect == NULL) {
ac->onConnect = fn;




_EL_ADD_WRITE(ac);
return REDIS_OK;
}
return REDIS_ERR;
}

int redisAsyncSetDisconnectCallback(redisAsyncContext *ac, redisDisconnectCallback *fn) {
if (ac->onDisconnect == NULL) {
ac->onDisconnect = fn;
return REDIS_OK;
}
return REDIS_ERR;
}


static int __redisPushCallback(redisCallbackList *list, redisCallback *source) {
redisCallback *cb;


cb = hi_malloc(sizeof(*cb));
if (cb == NULL)
return REDIS_ERR_OOM;

if (source != NULL) {
memcpy(cb,source,sizeof(*cb));
cb->next = NULL;
}


if (list->head == NULL)
list->head = cb;
if (list->tail != NULL)
list->tail->next = cb;
list->tail = cb;
return REDIS_OK;
}

static int __redisShiftCallback(redisCallbackList *list, redisCallback *target) {
redisCallback *cb = list->head;
if (cb != NULL) {
list->head = cb->next;
if (cb == list->tail)
list->tail = NULL;


if (target != NULL)
memcpy(target,cb,sizeof(*cb));
hi_free(cb);
return REDIS_OK;
}
return REDIS_ERR;
}

static void __redisRunCallback(redisAsyncContext *ac, redisCallback *cb, redisReply *reply) {
redisContext *c = &(ac->c);
if (cb->fn != NULL) {
c->flags |= REDIS_IN_CALLBACK;
cb->fn(ac,reply,cb->privdata);
c->flags &= ~REDIS_IN_CALLBACK;
}
}

static void __redisRunPushCallback(redisAsyncContext *ac, redisReply *reply) {
if (ac->push_cb != NULL) {
ac->c.flags |= REDIS_IN_CALLBACK;
ac->push_cb(ac, reply);
ac->c.flags &= ~REDIS_IN_CALLBACK;
}
}


static void __redisAsyncFree(redisAsyncContext *ac) {
redisContext *c = &(ac->c);
redisCallback cb;
dictIterator *it;
dictEntry *de;


while (__redisShiftCallback(&ac->replies,&cb) == REDIS_OK)
__redisRunCallback(ac,&cb,NULL);


while (__redisShiftCallback(&ac->sub.invalid,&cb) == REDIS_OK)
__redisRunCallback(ac,&cb,NULL);


if (ac->sub.channels) {
it = dictGetIterator(ac->sub.channels);
if (it != NULL) {
while ((de = dictNext(it)) != NULL)
__redisRunCallback(ac,dictGetEntryVal(de),NULL);
dictReleaseIterator(it);
}

dictRelease(ac->sub.channels);
}

if (ac->sub.patterns) {
it = dictGetIterator(ac->sub.patterns);
if (it != NULL) {
while ((de = dictNext(it)) != NULL)
__redisRunCallback(ac,dictGetEntryVal(de),NULL);
dictReleaseIterator(it);
}

dictRelease(ac->sub.patterns);
}


_EL_CLEANUP(ac);



if (ac->onDisconnect && (c->flags & REDIS_CONNECTED)) {
if (c->flags & REDIS_FREEING) {
ac->onDisconnect(ac,REDIS_OK);
} else {
ac->onDisconnect(ac,(ac->err == 0) ? REDIS_OK : REDIS_ERR);
}
}

if (ac->dataCleanup) {
ac->dataCleanup(ac->data);
}


redisFree(c);
}





void redisAsyncFree(redisAsyncContext *ac) {
redisContext *c = &(ac->c);
c->flags |= REDIS_FREEING;
if (!(c->flags & REDIS_IN_CALLBACK))
__redisAsyncFree(ac);
}


void __redisAsyncDisconnect(redisAsyncContext *ac) {
redisContext *c = &(ac->c);


__redisAsyncCopyError(ac);

if (ac->err == 0) {

int ret = __redisShiftCallback(&ac->replies,NULL);
assert(ret == REDIS_ERR);
} else {


c->flags |= REDIS_DISCONNECTING;
}



_EL_CLEANUP(ac);



if (!(c->flags & REDIS_NO_AUTO_FREE)) {
__redisAsyncFree(ac);
}
}







void redisAsyncDisconnect(redisAsyncContext *ac) {
redisContext *c = &(ac->c);
c->flags |= REDIS_DISCONNECTING;


c->flags &= ~REDIS_NO_AUTO_FREE;
if (!(c->flags & REDIS_IN_CALLBACK) && ac->replies.head == NULL)
__redisAsyncDisconnect(ac);
}

static int __redisGetSubscribeCallback(redisAsyncContext *ac, redisReply *reply, redisCallback *dstcb) {
redisContext *c = &(ac->c);
dict *callbacks;
redisCallback *cb;
dictEntry *de;
int pvariant;
char *stype;
hisds sname;



if (reply->type == REDIS_REPLY_ARRAY || reply->type == REDIS_REPLY_PUSH) {
assert(reply->elements >= 2);
assert(reply->element[0]->type == REDIS_REPLY_STRING);
stype = reply->element[0]->str;
pvariant = (tolower(stype[0]) == 'p') ? 1 : 0;

if (pvariant)
callbacks = ac->sub.patterns;
else
callbacks = ac->sub.channels;


assert(reply->element[1]->type == REDIS_REPLY_STRING);
sname = hi_sdsnewlen(reply->element[1]->str,reply->element[1]->len);
if (sname == NULL)
goto oom;

de = dictFind(callbacks,sname);
if (de != NULL) {
cb = dictGetEntryVal(de);


if (strcasecmp(stype+pvariant,"subscribe") == 0) {
cb->pending_subs -= 1;
}

memcpy(dstcb,cb,sizeof(*dstcb));


if (strcasecmp(stype+pvariant,"unsubscribe") == 0) {
if (cb->pending_subs == 0)
dictDelete(callbacks,sname);



assert(reply->element[2]->type == REDIS_REPLY_INTEGER);


if (reply->element[2]->integer == 0
&& dictSize(ac->sub.channels) == 0
&& dictSize(ac->sub.patterns) == 0)
c->flags &= ~REDIS_SUBSCRIBED;
}
}
hi_sdsfree(sname);
} else {

__redisShiftCallback(&ac->sub.invalid,dstcb);
}
return REDIS_OK;
oom:
__redisSetError(&(ac->c), REDIS_ERR_OOM, "Out of memory");
return REDIS_ERR;
}

#define redisIsSpontaneousPushReply(r) (redisIsPushReply(r) && !redisIsSubscribeReply(r))


static int redisIsSubscribeReply(redisReply *reply) {
char *str;
size_t len, off;


if (reply->elements < 1 || reply->element[0]->type != REDIS_REPLY_STRING ||
reply->element[0]->len < sizeof("message") - 1)
{
return 0;
}


off = tolower(reply->element[0]->str[0]) == 'p';
str = reply->element[0]->str + off;
len = reply->element[0]->len - off;

return !strncasecmp(str, "subscribe", len) ||
!strncasecmp(str, "message", len);

}

void redisProcessCallbacks(redisAsyncContext *ac) {
redisContext *c = &(ac->c);
redisCallback cb = {NULL, NULL, 0, NULL};
void *reply = NULL;
int status;

while((status = redisGetReply(c,&reply)) == REDIS_OK) {
if (reply == NULL) {


if (c->flags & REDIS_DISCONNECTING && hi_sdslen(c->obuf) == 0
&& ac->replies.head == NULL) {
__redisAsyncDisconnect(ac);
return;
}


if(c->flags & REDIS_MONITORING) {
__redisPushCallback(&ac->replies,&cb);
}



break;
}





if (redisIsSpontaneousPushReply(reply)) {
__redisRunPushCallback(ac, reply);
c->reader->fn->freeObject(reply);
continue;
}



if (__redisShiftCallback(&ac->replies,&cb) != REDIS_OK) {















if (((redisReply*)reply)->type == REDIS_REPLY_ERROR) {
c->err = REDIS_ERR_OTHER;
snprintf(c->errstr,sizeof(c->errstr),"%s",((redisReply*)reply)->str);
c->reader->fn->freeObject(reply);
__redisAsyncDisconnect(ac);
return;
}

assert((c->flags & REDIS_SUBSCRIBED || c->flags & REDIS_MONITORING));
if(c->flags & REDIS_SUBSCRIBED)
__redisGetSubscribeCallback(ac,reply,&cb);
}

if (cb.fn != NULL) {
__redisRunCallback(ac,&cb,reply);
c->reader->fn->freeObject(reply);


if (c->flags & REDIS_FREEING) {
__redisAsyncFree(ac);
return;
}
} else {




c->reader->fn->freeObject(reply);
}
}


if (status != REDIS_OK)
__redisAsyncDisconnect(ac);
}

static void __redisAsyncHandleConnectFailure(redisAsyncContext *ac) {
if (ac->onConnect) ac->onConnect(ac, REDIS_ERR);
__redisAsyncDisconnect(ac);
}




static int __redisAsyncHandleConnect(redisAsyncContext *ac) {
int completed = 0;
redisContext *c = &(ac->c);

if (redisCheckConnectDone(c, &completed) == REDIS_ERR) {

redisCheckSocketError(c);
__redisAsyncHandleConnectFailure(ac);
return REDIS_ERR;
} else if (completed == 1) {

if (c->connection_type == REDIS_CONN_TCP &&
redisSetTcpNoDelay(c) == REDIS_ERR) {
__redisAsyncHandleConnectFailure(ac);
return REDIS_ERR;
}

if (ac->onConnect) ac->onConnect(ac, REDIS_OK);
c->flags |= REDIS_CONNECTED;
return REDIS_OK;
} else {
return REDIS_OK;
}
}

void redisAsyncRead(redisAsyncContext *ac) {
redisContext *c = &(ac->c);

if (redisBufferRead(c) == REDIS_ERR) {
__redisAsyncDisconnect(ac);
} else {

_EL_ADD_READ(ac);
redisProcessCallbacks(ac);
}
}




void redisAsyncHandleRead(redisAsyncContext *ac) {
redisContext *c = &(ac->c);

if (!(c->flags & REDIS_CONNECTED)) {

if (__redisAsyncHandleConnect(ac) != REDIS_OK)
return;

if (!(c->flags & REDIS_CONNECTED))
return;
}

c->funcs->async_read(ac);
}

void redisAsyncWrite(redisAsyncContext *ac) {
redisContext *c = &(ac->c);
int done = 0;

if (redisBufferWrite(c,&done) == REDIS_ERR) {
__redisAsyncDisconnect(ac);
} else {

if (!done)
_EL_ADD_WRITE(ac);
else
_EL_DEL_WRITE(ac);


_EL_ADD_READ(ac);
}
}

void redisAsyncHandleWrite(redisAsyncContext *ac) {
redisContext *c = &(ac->c);

if (!(c->flags & REDIS_CONNECTED)) {

if (__redisAsyncHandleConnect(ac) != REDIS_OK)
return;

if (!(c->flags & REDIS_CONNECTED))
return;
}

c->funcs->async_write(ac);
}

void redisAsyncHandleTimeout(redisAsyncContext *ac) {
redisContext *c = &(ac->c);
redisCallback cb;

if ((c->flags & REDIS_CONNECTED) && ac->replies.head == NULL) {

return;
}

if (!c->err) {
__redisSetError(c, REDIS_ERR_TIMEOUT, "Timeout");
}

if (!(c->flags & REDIS_CONNECTED) && ac->onConnect) {
ac->onConnect(ac, REDIS_ERR);
}

while (__redisShiftCallback(&ac->replies, &cb) == REDIS_OK) {
__redisRunCallback(ac, &cb, NULL);
}





__redisAsyncDisconnect(ac);
}



static const char *nextArgument(const char *start, const char **str, size_t *len) {
const char *p = start;
if (p[0] != '$') {
p = strchr(p,'$');
if (p == NULL) return NULL;
}

*len = (int)strtol(p+1,NULL,10);
p = strchr(p,'\r');
assert(p);
*str = p+2;
return p+2+(*len)+2;
}




static int __redisAsyncCommand(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata, const char *cmd, size_t len) {
redisContext *c = &(ac->c);
redisCallback cb;
struct dict *cbdict;
dictEntry *de;
redisCallback *existcb;
int pvariant, hasnext;
const char *cstr, *astr;
size_t clen, alen;
const char *p;
hisds sname;
int ret;


if (c->flags & (REDIS_DISCONNECTING | REDIS_FREEING)) return REDIS_ERR;


cb.fn = fn;
cb.privdata = privdata;
cb.pending_subs = 1;


p = nextArgument(cmd,&cstr,&clen);
assert(p != NULL);
hasnext = (p[0] == '$');
pvariant = (tolower(cstr[0]) == 'p') ? 1 : 0;
cstr += pvariant;
clen -= pvariant;

if (hasnext && strncasecmp(cstr,"subscribe\r\n",11) == 0) {
c->flags |= REDIS_SUBSCRIBED;


while ((p = nextArgument(p,&astr,&alen)) != NULL) {
sname = hi_sdsnewlen(astr,alen);
if (sname == NULL)
goto oom;

if (pvariant)
cbdict = ac->sub.patterns;
else
cbdict = ac->sub.channels;

de = dictFind(cbdict,sname);

if (de != NULL) {
existcb = dictGetEntryVal(de);
cb.pending_subs = existcb->pending_subs + 1;
}

ret = dictReplace(cbdict,sname,&cb);

if (ret == 0) hi_sdsfree(sname);
}
} else if (strncasecmp(cstr,"unsubscribe\r\n",13) == 0) {


if (!(c->flags & REDIS_SUBSCRIBED)) return REDIS_ERR;




} else if(strncasecmp(cstr,"monitor\r\n",9) == 0) {

c->flags |= REDIS_MONITORING;
__redisPushCallback(&ac->replies,&cb);
} else {
if (c->flags & REDIS_SUBSCRIBED)


__redisPushCallback(&ac->sub.invalid,&cb);
else
__redisPushCallback(&ac->replies,&cb);
}

__redisAppendCommand(c,cmd,len);


_EL_ADD_WRITE(ac);

return REDIS_OK;
oom:
__redisSetError(&(ac->c), REDIS_ERR_OOM, "Out of memory");
return REDIS_ERR;
}

int redisvAsyncCommand(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata, const char *format, va_list ap) {
char *cmd;
int len;
int status;
len = redisvFormatCommand(&cmd,format,ap);


if (len < 0)
return REDIS_ERR;

status = __redisAsyncCommand(ac,fn,privdata,cmd,len);
hi_free(cmd);
return status;
}

int redisAsyncCommand(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata, const char *format, ...) {
va_list ap;
int status;
va_start(ap,format);
status = redisvAsyncCommand(ac,fn,privdata,format,ap);
va_end(ap);
return status;
}

int redisAsyncCommandArgv(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata, int argc, const char **argv, const size_t *argvlen) {
hisds cmd;
int len;
int status;
len = redisFormatSdsCommandArgv(&cmd,argc,argv,argvlen);
if (len < 0)
return REDIS_ERR;
status = __redisAsyncCommand(ac,fn,privdata,cmd,len);
hi_sdsfree(cmd);
return status;
}

int redisAsyncFormattedCommand(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata, const char *cmd, size_t len) {
int status = __redisAsyncCommand(ac,fn,privdata,cmd,len);
return status;
}

redisAsyncPushFn *redisAsyncSetPushCallback(redisAsyncContext *ac, redisAsyncPushFn *fn) {
redisAsyncPushFn *old = ac->push_cb;
ac->push_cb = fn;
return old;
}

int redisAsyncSetTimeout(redisAsyncContext *ac, struct timeval tv) {
if (!ac->c.command_timeout) {
ac->c.command_timeout = hi_calloc(1, sizeof(tv));
if (ac->c.command_timeout == NULL) {
__redisSetError(&ac->c, REDIS_ERR_OOM, "Out of memory");
__redisAsyncCopyError(ac);
return REDIS_ERR;
}
}

if (tv.tv_sec != ac->c.command_timeout->tv_sec ||
tv.tv_usec != ac->c.command_timeout->tv_usec)
{
*ac->c.command_timeout = tv;
}

return REDIS_OK;
}
