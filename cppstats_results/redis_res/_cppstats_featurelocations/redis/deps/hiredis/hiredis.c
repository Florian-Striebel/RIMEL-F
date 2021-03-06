
































#include "fmacros.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#include "hiredis.h"
#include "net.h"
#include "sds.h"
#include "async.h"
#include "win32.h"

extern int redisContextUpdateConnectTimeout(redisContext *c, const struct timeval *timeout);
extern int redisContextUpdateCommandTimeout(redisContext *c, const struct timeval *timeout);

static redisContextFuncs redisContextDefaultFuncs = {
.free_privctx = NULL,
.async_read = redisAsyncRead,
.async_write = redisAsyncWrite,
.read = redisNetRead,
.write = redisNetWrite
};

static redisReply *createReplyObject(int type);
static void *createStringObject(const redisReadTask *task, char *str, size_t len);
static void *createArrayObject(const redisReadTask *task, size_t elements);
static void *createIntegerObject(const redisReadTask *task, long long value);
static void *createDoubleObject(const redisReadTask *task, double value, char *str, size_t len);
static void *createNilObject(const redisReadTask *task);
static void *createBoolObject(const redisReadTask *task, int bval);



static redisReplyObjectFunctions defaultFunctions = {
createStringObject,
createArrayObject,
createIntegerObject,
createDoubleObject,
createNilObject,
createBoolObject,
freeReplyObject
};


static redisReply *createReplyObject(int type) {
redisReply *r = hi_calloc(1,sizeof(*r));

if (r == NULL)
return NULL;

r->type = type;
return r;
}


void freeReplyObject(void *reply) {
redisReply *r = reply;
size_t j;

if (r == NULL)
return;

switch(r->type) {
case REDIS_REPLY_INTEGER:
break;
case REDIS_REPLY_ARRAY:
case REDIS_REPLY_MAP:
case REDIS_REPLY_SET:
case REDIS_REPLY_PUSH:
if (r->element != NULL) {
for (j = 0; j < r->elements; j++)
freeReplyObject(r->element[j]);
hi_free(r->element);
}
break;
case REDIS_REPLY_ERROR:
case REDIS_REPLY_STATUS:
case REDIS_REPLY_STRING:
case REDIS_REPLY_DOUBLE:
case REDIS_REPLY_VERB:
hi_free(r->str);
break;
}
hi_free(r);
}

static void *createStringObject(const redisReadTask *task, char *str, size_t len) {
redisReply *r, *parent;
char *buf;

r = createReplyObject(task->type);
if (r == NULL)
return NULL;

assert(task->type == REDIS_REPLY_ERROR ||
task->type == REDIS_REPLY_STATUS ||
task->type == REDIS_REPLY_STRING ||
task->type == REDIS_REPLY_VERB);


if (task->type == REDIS_REPLY_VERB) {
buf = hi_malloc(len-4+1);
if (buf == NULL) goto oom;

memcpy(r->vtype,str,3);
r->vtype[3] = '\0';
memcpy(buf,str+4,len-4);
buf[len-4] = '\0';
r->len = len - 4;
} else {
buf = hi_malloc(len+1);
if (buf == NULL) goto oom;

memcpy(buf,str,len);
buf[len] = '\0';
r->len = len;
}
r->str = buf;

if (task->parent) {
parent = task->parent->obj;
assert(parent->type == REDIS_REPLY_ARRAY ||
parent->type == REDIS_REPLY_MAP ||
parent->type == REDIS_REPLY_SET ||
parent->type == REDIS_REPLY_PUSH);
parent->element[task->idx] = r;
}
return r;

oom:
freeReplyObject(r);
return NULL;
}

static void *createArrayObject(const redisReadTask *task, size_t elements) {
redisReply *r, *parent;

r = createReplyObject(task->type);
if (r == NULL)
return NULL;

if (elements > 0) {
r->element = hi_calloc(elements,sizeof(redisReply*));
if (r->element == NULL) {
freeReplyObject(r);
return NULL;
}
}

r->elements = elements;

if (task->parent) {
parent = task->parent->obj;
assert(parent->type == REDIS_REPLY_ARRAY ||
parent->type == REDIS_REPLY_MAP ||
parent->type == REDIS_REPLY_SET ||
parent->type == REDIS_REPLY_PUSH);
parent->element[task->idx] = r;
}
return r;
}

static void *createIntegerObject(const redisReadTask *task, long long value) {
redisReply *r, *parent;

r = createReplyObject(REDIS_REPLY_INTEGER);
if (r == NULL)
return NULL;

r->integer = value;

if (task->parent) {
parent = task->parent->obj;
assert(parent->type == REDIS_REPLY_ARRAY ||
parent->type == REDIS_REPLY_MAP ||
parent->type == REDIS_REPLY_SET ||
parent->type == REDIS_REPLY_PUSH);
parent->element[task->idx] = r;
}
return r;
}

static void *createDoubleObject(const redisReadTask *task, double value, char *str, size_t len) {
redisReply *r, *parent;

r = createReplyObject(REDIS_REPLY_DOUBLE);
if (r == NULL)
return NULL;

r->dval = value;
r->str = hi_malloc(len+1);
if (r->str == NULL) {
freeReplyObject(r);
return NULL;
}






memcpy(r->str, str, len);
r->str[len] = '\0';

if (task->parent) {
parent = task->parent->obj;
assert(parent->type == REDIS_REPLY_ARRAY ||
parent->type == REDIS_REPLY_MAP ||
parent->type == REDIS_REPLY_SET);
parent->element[task->idx] = r;
}
return r;
}

static void *createNilObject(const redisReadTask *task) {
redisReply *r, *parent;

r = createReplyObject(REDIS_REPLY_NIL);
if (r == NULL)
return NULL;

if (task->parent) {
parent = task->parent->obj;
assert(parent->type == REDIS_REPLY_ARRAY ||
parent->type == REDIS_REPLY_MAP ||
parent->type == REDIS_REPLY_SET ||
parent->type == REDIS_REPLY_PUSH);
parent->element[task->idx] = r;
}
return r;
}

static void *createBoolObject(const redisReadTask *task, int bval) {
redisReply *r, *parent;

r = createReplyObject(REDIS_REPLY_BOOL);
if (r == NULL)
return NULL;

r->integer = bval != 0;

if (task->parent) {
parent = task->parent->obj;
assert(parent->type == REDIS_REPLY_ARRAY ||
parent->type == REDIS_REPLY_MAP ||
parent->type == REDIS_REPLY_SET);
parent->element[task->idx] = r;
}
return r;
}



static uint32_t countDigits(uint64_t v) {
uint32_t result = 1;
for (;;) {
if (v < 10) return result;
if (v < 100) return result + 1;
if (v < 1000) return result + 2;
if (v < 10000) return result + 3;
v /= 10000U;
result += 4;
}
}


static size_t bulklen(size_t len) {
return 1+countDigits(len)+2+len+2;
}

int redisvFormatCommand(char **target, const char *format, va_list ap) {
const char *c = format;
char *cmd = NULL;
int pos;
hisds curarg, newarg;
int touched = 0;
char **curargv = NULL, **newargv = NULL;
int argc = 0;
int totlen = 0;
int error_type = 0;
int j;


if (target == NULL)
return -1;


curarg = hi_sdsempty();
if (curarg == NULL)
return -1;

while(*c != '\0') {
if (*c != '%' || c[1] == '\0') {
if (*c == ' ') {
if (touched) {
newargv = hi_realloc(curargv,sizeof(char*)*(argc+1));
if (newargv == NULL) goto memory_err;
curargv = newargv;
curargv[argc++] = curarg;
totlen += bulklen(hi_sdslen(curarg));


curarg = hi_sdsempty();
if (curarg == NULL) goto memory_err;
touched = 0;
}
} else {
newarg = hi_sdscatlen(curarg,c,1);
if (newarg == NULL) goto memory_err;
curarg = newarg;
touched = 1;
}
} else {
char *arg;
size_t size;


newarg = curarg;

switch(c[1]) {
case 's':
arg = va_arg(ap,char*);
size = strlen(arg);
if (size > 0)
newarg = hi_sdscatlen(curarg,arg,size);
break;
case 'b':
arg = va_arg(ap,char*);
size = va_arg(ap,size_t);
if (size > 0)
newarg = hi_sdscatlen(curarg,arg,size);
break;
case '%':
newarg = hi_sdscat(curarg,"%");
break;
default:

{
static const char intfmts[] = "diouxX";
static const char flags[] = "#0-+ ";
char _format[16];
const char *_p = c+1;
size_t _l = 0;
va_list _cpy;


while (*_p != '\0' && strchr(flags,*_p) != NULL) _p++;


while (*_p != '\0' && isdigit(*_p)) _p++;


if (*_p == '.') {
_p++;
while (*_p != '\0' && isdigit(*_p)) _p++;
}


va_copy(_cpy,ap);


if (strchr(intfmts,*_p) != NULL) {
va_arg(ap,int);
goto fmt_valid;
}


if (strchr("eEfFgGaA",*_p) != NULL) {
va_arg(ap,double);
goto fmt_valid;
}


if (_p[0] == 'h' && _p[1] == 'h') {
_p += 2;
if (*_p != '\0' && strchr(intfmts,*_p) != NULL) {
va_arg(ap,int);
goto fmt_valid;
}
goto fmt_invalid;
}


if (_p[0] == 'h') {
_p += 1;
if (*_p != '\0' && strchr(intfmts,*_p) != NULL) {
va_arg(ap,int);
goto fmt_valid;
}
goto fmt_invalid;
}


if (_p[0] == 'l' && _p[1] == 'l') {
_p += 2;
if (*_p != '\0' && strchr(intfmts,*_p) != NULL) {
va_arg(ap,long long);
goto fmt_valid;
}
goto fmt_invalid;
}


if (_p[0] == 'l') {
_p += 1;
if (*_p != '\0' && strchr(intfmts,*_p) != NULL) {
va_arg(ap,long);
goto fmt_valid;
}
goto fmt_invalid;
}

fmt_invalid:
va_end(_cpy);
goto format_err;

fmt_valid:
_l = (_p+1)-c;
if (_l < sizeof(_format)-2) {
memcpy(_format,c,_l);
_format[_l] = '\0';
newarg = hi_sdscatvprintf(curarg,_format,_cpy);



c = _p-1;
}

va_end(_cpy);
break;
}
}

if (newarg == NULL) goto memory_err;
curarg = newarg;

touched = 1;
c++;
}
c++;
}


if (touched) {
newargv = hi_realloc(curargv,sizeof(char*)*(argc+1));
if (newargv == NULL) goto memory_err;
curargv = newargv;
curargv[argc++] = curarg;
totlen += bulklen(hi_sdslen(curarg));
} else {
hi_sdsfree(curarg);
}


curarg = NULL;


totlen += 1+countDigits(argc)+2;


cmd = hi_malloc(totlen+1);
if (cmd == NULL) goto memory_err;

pos = sprintf(cmd,"*%d\r\n",argc);
for (j = 0; j < argc; j++) {
pos += sprintf(cmd+pos,"$%zu\r\n",hi_sdslen(curargv[j]));
memcpy(cmd+pos,curargv[j],hi_sdslen(curargv[j]));
pos += hi_sdslen(curargv[j]);
hi_sdsfree(curargv[j]);
cmd[pos++] = '\r';
cmd[pos++] = '\n';
}
assert(pos == totlen);
cmd[pos] = '\0';

hi_free(curargv);
*target = cmd;
return totlen;

format_err:
error_type = -2;
goto cleanup;

memory_err:
error_type = -1;
goto cleanup;

cleanup:
if (curargv) {
while(argc--)
hi_sdsfree(curargv[argc]);
hi_free(curargv);
}

hi_sdsfree(curarg);
hi_free(cmd);

return error_type;
}













int redisFormatCommand(char **target, const char *format, ...) {
va_list ap;
int len;
va_start(ap,format);
len = redisvFormatCommand(target,format,ap);
va_end(ap);



if (len < 0)
len = -1;

return len;
}







int redisFormatSdsCommandArgv(hisds *target, int argc, const char **argv,
const size_t *argvlen)
{
hisds cmd, aux;
unsigned long long totlen;
int j;
size_t len;


if (target == NULL)
return -1;


totlen = 1+countDigits(argc)+2;
for (j = 0; j < argc; j++) {
len = argvlen ? argvlen[j] : strlen(argv[j]);
totlen += bulklen(len);
}


cmd = hi_sdsempty();
if (cmd == NULL)
return -1;


aux = hi_sdsMakeRoomFor(cmd, totlen);
if (aux == NULL) {
hi_sdsfree(cmd);
return -1;
}

cmd = aux;


cmd = hi_sdscatfmt(cmd, "*%i\r\n", argc);
for (j=0; j < argc; j++) {
len = argvlen ? argvlen[j] : strlen(argv[j]);
cmd = hi_sdscatfmt(cmd, "$%u\r\n", len);
cmd = hi_sdscatlen(cmd, argv[j], len);
cmd = hi_sdscatlen(cmd, "\r\n", sizeof("\r\n")-1);
}

assert(hi_sdslen(cmd)==totlen);

*target = cmd;
return totlen;
}

void redisFreeSdsCommand(hisds cmd) {
hi_sdsfree(cmd);
}






int redisFormatCommandArgv(char **target, int argc, const char **argv, const size_t *argvlen) {
char *cmd = NULL;
int pos;
size_t len;
int totlen, j;


if (target == NULL)
return -1;


totlen = 1+countDigits(argc)+2;
for (j = 0; j < argc; j++) {
len = argvlen ? argvlen[j] : strlen(argv[j]);
totlen += bulklen(len);
}


cmd = hi_malloc(totlen+1);
if (cmd == NULL)
return -1;

pos = sprintf(cmd,"*%d\r\n",argc);
for (j = 0; j < argc; j++) {
len = argvlen ? argvlen[j] : strlen(argv[j]);
pos += sprintf(cmd+pos,"$%zu\r\n",len);
memcpy(cmd+pos,argv[j],len);
pos += len;
cmd[pos++] = '\r';
cmd[pos++] = '\n';
}
assert(pos == totlen);
cmd[pos] = '\0';

*target = cmd;
return totlen;
}

void redisFreeCommand(char *cmd) {
hi_free(cmd);
}

void __redisSetError(redisContext *c, int type, const char *str) {
size_t len;

c->err = type;
if (str != NULL) {
len = strlen(str);
len = len < (sizeof(c->errstr)-1) ? len : (sizeof(c->errstr)-1);
memcpy(c->errstr,str,len);
c->errstr[len] = '\0';
} else {

assert(type == REDIS_ERR_IO);
strerror_r(errno, c->errstr, sizeof(c->errstr));
}
}

redisReader *redisReaderCreate(void) {
return redisReaderCreateWithFunctions(&defaultFunctions);
}

static void redisPushAutoFree(void *privdata, void *reply) {
(void)privdata;
freeReplyObject(reply);
}

static redisContext *redisContextInit(void) {
redisContext *c;

c = hi_calloc(1, sizeof(*c));
if (c == NULL)
return NULL;

c->funcs = &redisContextDefaultFuncs;

c->obuf = hi_sdsempty();
c->reader = redisReaderCreate();
c->fd = REDIS_INVALID_FD;

if (c->obuf == NULL || c->reader == NULL) {
redisFree(c);
return NULL;
}

return c;
}

void redisFree(redisContext *c) {
if (c == NULL)
return;
redisNetClose(c);

hi_sdsfree(c->obuf);
redisReaderFree(c->reader);
hi_free(c->tcp.host);
hi_free(c->tcp.source_addr);
hi_free(c->unix_sock.path);
hi_free(c->connect_timeout);
hi_free(c->command_timeout);
hi_free(c->saddr);

if (c->privdata && c->free_privdata)
c->free_privdata(c->privdata);

if (c->funcs->free_privctx)
c->funcs->free_privctx(c->privctx);

memset(c, 0xff, sizeof(*c));
hi_free(c);
}

redisFD redisFreeKeepFd(redisContext *c) {
redisFD fd = c->fd;
c->fd = REDIS_INVALID_FD;
redisFree(c);
return fd;
}

int redisReconnect(redisContext *c) {
c->err = 0;
memset(c->errstr, '\0', strlen(c->errstr));

if (c->privctx && c->funcs->free_privctx) {
c->funcs->free_privctx(c->privctx);
c->privctx = NULL;
}

redisNetClose(c);

hi_sdsfree(c->obuf);
redisReaderFree(c->reader);

c->obuf = hi_sdsempty();
c->reader = redisReaderCreate();

if (c->obuf == NULL || c->reader == NULL) {
__redisSetError(c, REDIS_ERR_OOM, "Out of memory");
return REDIS_ERR;
}

int ret = REDIS_ERR;
if (c->connection_type == REDIS_CONN_TCP) {
ret = redisContextConnectBindTcp(c, c->tcp.host, c->tcp.port,
c->connect_timeout, c->tcp.source_addr);
} else if (c->connection_type == REDIS_CONN_UNIX) {
ret = redisContextConnectUnix(c, c->unix_sock.path, c->connect_timeout);
} else {


__redisSetError(c,REDIS_ERR_OTHER,"Not enough information to reconnect");
ret = REDIS_ERR;
}

if (c->command_timeout != NULL && (c->flags & REDIS_BLOCK) && c->fd != REDIS_INVALID_FD) {
redisContextSetTimeout(c, *c->command_timeout);
}

return ret;
}

redisContext *redisConnectWithOptions(const redisOptions *options) {
redisContext *c = redisContextInit();
if (c == NULL) {
return NULL;
}
if (!(options->options & REDIS_OPT_NONBLOCK)) {
c->flags |= REDIS_BLOCK;
}
if (options->options & REDIS_OPT_REUSEADDR) {
c->flags |= REDIS_REUSEADDR;
}
if (options->options & REDIS_OPT_NOAUTOFREE) {
c->flags |= REDIS_NO_AUTO_FREE;
}



if (options->push_cb != NULL)
redisSetPushCallback(c, options->push_cb);
else if (!(options->options & REDIS_OPT_NO_PUSH_AUTOFREE))
redisSetPushCallback(c, redisPushAutoFree);

c->privdata = options->privdata;
c->free_privdata = options->free_privdata;

if (redisContextUpdateConnectTimeout(c, options->connect_timeout) != REDIS_OK ||
redisContextUpdateCommandTimeout(c, options->command_timeout) != REDIS_OK) {
__redisSetError(c, REDIS_ERR_OOM, "Out of memory");
return c;
}

if (options->type == REDIS_CONN_TCP) {
redisContextConnectBindTcp(c, options->endpoint.tcp.ip,
options->endpoint.tcp.port, options->connect_timeout,
options->endpoint.tcp.source_addr);
} else if (options->type == REDIS_CONN_UNIX) {
redisContextConnectUnix(c, options->endpoint.unix_socket,
options->connect_timeout);
} else if (options->type == REDIS_CONN_USERFD) {
c->fd = options->endpoint.fd;
c->flags |= REDIS_CONNECTED;
} else {

return NULL;
}

if (options->command_timeout != NULL && (c->flags & REDIS_BLOCK) && c->fd != REDIS_INVALID_FD) {
redisContextSetTimeout(c, *options->command_timeout);
}

return c;
}




redisContext *redisConnect(const char *ip, int port) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
return redisConnectWithOptions(&options);
}

redisContext *redisConnectWithTimeout(const char *ip, int port, const struct timeval tv) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
options.connect_timeout = &tv;
return redisConnectWithOptions(&options);
}

redisContext *redisConnectNonBlock(const char *ip, int port) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
options.options |= REDIS_OPT_NONBLOCK;
return redisConnectWithOptions(&options);
}

redisContext *redisConnectBindNonBlock(const char *ip, int port,
const char *source_addr) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
options.endpoint.tcp.source_addr = source_addr;
options.options |= REDIS_OPT_NONBLOCK;
return redisConnectWithOptions(&options);
}

redisContext *redisConnectBindNonBlockWithReuse(const char *ip, int port,
const char *source_addr) {
redisOptions options = {0};
REDIS_OPTIONS_SET_TCP(&options, ip, port);
options.endpoint.tcp.source_addr = source_addr;
options.options |= REDIS_OPT_NONBLOCK|REDIS_OPT_REUSEADDR;
return redisConnectWithOptions(&options);
}

redisContext *redisConnectUnix(const char *path) {
redisOptions options = {0};
REDIS_OPTIONS_SET_UNIX(&options, path);
return redisConnectWithOptions(&options);
}

redisContext *redisConnectUnixWithTimeout(const char *path, const struct timeval tv) {
redisOptions options = {0};
REDIS_OPTIONS_SET_UNIX(&options, path);
options.connect_timeout = &tv;
return redisConnectWithOptions(&options);
}

redisContext *redisConnectUnixNonBlock(const char *path) {
redisOptions options = {0};
REDIS_OPTIONS_SET_UNIX(&options, path);
options.options |= REDIS_OPT_NONBLOCK;
return redisConnectWithOptions(&options);
}

redisContext *redisConnectFd(redisFD fd) {
redisOptions options = {0};
options.type = REDIS_CONN_USERFD;
options.endpoint.fd = fd;
return redisConnectWithOptions(&options);
}


int redisSetTimeout(redisContext *c, const struct timeval tv) {
if (c->flags & REDIS_BLOCK)
return redisContextSetTimeout(c,tv);
return REDIS_ERR;
}


int redisEnableKeepAlive(redisContext *c) {
if (redisKeepAlive(c, REDIS_KEEPALIVE_INTERVAL) != REDIS_OK)
return REDIS_ERR;
return REDIS_OK;
}


redisPushFn *redisSetPushCallback(redisContext *c, redisPushFn *fn) {
redisPushFn *old = c->push_cb;
c->push_cb = fn;
return old;
}






int redisBufferRead(redisContext *c) {
char buf[1024*16];
int nread;


if (c->err)
return REDIS_ERR;

nread = c->funcs->read(c, buf, sizeof(buf));
if (nread > 0) {
if (redisReaderFeed(c->reader, buf, nread) != REDIS_OK) {
__redisSetError(c, c->reader->err, c->reader->errstr);
return REDIS_ERR;
} else {
}
} else if (nread < 0) {
return REDIS_ERR;
}
return REDIS_OK;
}










int redisBufferWrite(redisContext *c, int *done) {


if (c->err)
return REDIS_ERR;

if (hi_sdslen(c->obuf) > 0) {
ssize_t nwritten = c->funcs->write(c);
if (nwritten < 0) {
return REDIS_ERR;
} else if (nwritten > 0) {
if (nwritten == (ssize_t)hi_sdslen(c->obuf)) {
hi_sdsfree(c->obuf);
c->obuf = hi_sdsempty();
if (c->obuf == NULL)
goto oom;
} else {
if (hi_sdsrange(c->obuf,nwritten,-1) < 0) goto oom;
}
}
}
if (done != NULL) *done = (hi_sdslen(c->obuf) == 0);
return REDIS_OK;

oom:
__redisSetError(c, REDIS_ERR_OOM, "Out of memory");
return REDIS_ERR;
}



int redisGetReplyFromReader(redisContext *c, void **reply) {
if (redisReaderGetReply(c->reader,reply) == REDIS_ERR) {
__redisSetError(c,c->reader->err,c->reader->errstr);
return REDIS_ERR;
}

return REDIS_OK;
}



static int redisHandledPushReply(redisContext *c, void *reply) {
if (reply && c->push_cb && redisIsPushReply(reply)) {
c->push_cb(c->privdata, reply);
return 1;
}

return 0;
}

int redisGetReply(redisContext *c, void **reply) {
int wdone = 0;
void *aux = NULL;


if (redisGetReplyFromReader(c,&aux) == REDIS_ERR)
return REDIS_ERR;


if (aux == NULL && c->flags & REDIS_BLOCK) {

do {
if (redisBufferWrite(c,&wdone) == REDIS_ERR)
return REDIS_ERR;
} while (!wdone);


do {
if (redisBufferRead(c) == REDIS_ERR)
return REDIS_ERR;



do {
if (redisGetReplyFromReader(c,&aux) == REDIS_ERR)
return REDIS_ERR;
} while (redisHandledPushReply(c, aux));
} while (aux == NULL);
}


if (reply != NULL) {
*reply = aux;
} else {
freeReplyObject(aux);
}

return REDIS_OK;
}








int __redisAppendCommand(redisContext *c, const char *cmd, size_t len) {
hisds newbuf;

newbuf = hi_sdscatlen(c->obuf,cmd,len);
if (newbuf == NULL) {
__redisSetError(c,REDIS_ERR_OOM,"Out of memory");
return REDIS_ERR;
}

c->obuf = newbuf;
return REDIS_OK;
}

int redisAppendFormattedCommand(redisContext *c, const char *cmd, size_t len) {

if (__redisAppendCommand(c, cmd, len) != REDIS_OK) {
return REDIS_ERR;
}

return REDIS_OK;
}

int redisvAppendCommand(redisContext *c, const char *format, va_list ap) {
char *cmd;
int len;

len = redisvFormatCommand(&cmd,format,ap);
if (len == -1) {
__redisSetError(c,REDIS_ERR_OOM,"Out of memory");
return REDIS_ERR;
} else if (len == -2) {
__redisSetError(c,REDIS_ERR_OTHER,"Invalid format string");
return REDIS_ERR;
}

if (__redisAppendCommand(c,cmd,len) != REDIS_OK) {
hi_free(cmd);
return REDIS_ERR;
}

hi_free(cmd);
return REDIS_OK;
}

int redisAppendCommand(redisContext *c, const char *format, ...) {
va_list ap;
int ret;

va_start(ap,format);
ret = redisvAppendCommand(c,format,ap);
va_end(ap);
return ret;
}

int redisAppendCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen) {
hisds cmd;
int len;

len = redisFormatSdsCommandArgv(&cmd,argc,argv,argvlen);
if (len == -1) {
__redisSetError(c,REDIS_ERR_OOM,"Out of memory");
return REDIS_ERR;
}

if (__redisAppendCommand(c,cmd,len) != REDIS_OK) {
hi_sdsfree(cmd);
return REDIS_ERR;
}

hi_sdsfree(cmd);
return REDIS_OK;
}












static void *__redisBlockForReply(redisContext *c) {
void *reply;

if (c->flags & REDIS_BLOCK) {
if (redisGetReply(c,&reply) != REDIS_OK)
return NULL;
return reply;
}
return NULL;
}

void *redisvCommand(redisContext *c, const char *format, va_list ap) {
if (redisvAppendCommand(c,format,ap) != REDIS_OK)
return NULL;
return __redisBlockForReply(c);
}

void *redisCommand(redisContext *c, const char *format, ...) {
va_list ap;
va_start(ap,format);
void *reply = redisvCommand(c,format,ap);
va_end(ap);
return reply;
}

void *redisCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen) {
if (redisAppendCommandArgv(c,argc,argv,argvlen) != REDIS_OK)
return NULL;
return __redisBlockForReply(c);
}
