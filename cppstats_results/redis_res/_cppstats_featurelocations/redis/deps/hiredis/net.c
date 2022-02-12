

































#include "fmacros.h"
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "net.h"
#include "sds.h"
#include "sockcompat.h"
#include "win32.h"


void __redisSetError(redisContext *c, int type, const char *str);

void redisNetClose(redisContext *c) {
if (c && c->fd != REDIS_INVALID_FD) {
close(c->fd);
c->fd = REDIS_INVALID_FD;
}
}

ssize_t redisNetRead(redisContext *c, char *buf, size_t bufcap) {
ssize_t nread = recv(c->fd, buf, bufcap, 0);
if (nread == -1) {
if ((errno == EWOULDBLOCK && !(c->flags & REDIS_BLOCK)) || (errno == EINTR)) {

return 0;
} else if(errno == ETIMEDOUT && (c->flags & REDIS_BLOCK)) {

__redisSetError(c, REDIS_ERR_TIMEOUT, "recv timeout");
return -1;
} else {
__redisSetError(c, REDIS_ERR_IO, NULL);
return -1;
}
} else if (nread == 0) {
__redisSetError(c, REDIS_ERR_EOF, "Server closed the connection");
return -1;
} else {
return nread;
}
}

ssize_t redisNetWrite(redisContext *c) {
ssize_t nwritten = send(c->fd, c->obuf, hi_sdslen(c->obuf), 0);
if (nwritten < 0) {
if ((errno == EWOULDBLOCK && !(c->flags & REDIS_BLOCK)) || (errno == EINTR)) {

} else {
__redisSetError(c, REDIS_ERR_IO, NULL);
return -1;
}
}
return nwritten;
}

static void __redisSetErrorFromErrno(redisContext *c, int type, const char *prefix) {
int errorno = errno;
char buf[128] = { 0 };
size_t len = 0;

if (prefix != NULL)
len = snprintf(buf,sizeof(buf),"%s: ",prefix);
strerror_r(errorno, (char *)(buf + len), sizeof(buf) - len);
__redisSetError(c,type,buf);
}

static int redisSetReuseAddr(redisContext *c) {
int on = 1;
if (setsockopt(c->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
redisNetClose(c);
return REDIS_ERR;
}
return REDIS_OK;
}

static int redisCreateSocket(redisContext *c, int type) {
redisFD s;
if ((s = socket(type, SOCK_STREAM, 0)) == REDIS_INVALID_FD) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
return REDIS_ERR;
}
c->fd = s;
if (type == AF_INET) {
if (redisSetReuseAddr(c) == REDIS_ERR) {
return REDIS_ERR;
}
}
return REDIS_OK;
}

static int redisSetBlocking(redisContext *c, int blocking) {
#if !defined(_WIN32)
int flags;




if ((flags = fcntl(c->fd, F_GETFL)) == -1) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_GETFL)");
redisNetClose(c);
return REDIS_ERR;
}

if (blocking)
flags &= ~O_NONBLOCK;
else
flags |= O_NONBLOCK;

if (fcntl(c->fd, F_SETFL, flags) == -1) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_SETFL)");
redisNetClose(c);
return REDIS_ERR;
}
#else
u_long mode = blocking ? 0 : 1;
if (ioctl(c->fd, FIONBIO, &mode) == -1) {
__redisSetErrorFromErrno(c, REDIS_ERR_IO, "ioctl(FIONBIO)");
redisNetClose(c);
return REDIS_ERR;
}
#endif
return REDIS_OK;
}

int redisKeepAlive(redisContext *c, int interval) {
int val = 1;
redisFD fd = c->fd;

if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1){
__redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
return REDIS_ERR;
}

val = interval;

#if defined(__APPLE__) && defined(__MACH__)
if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &val, sizeof(val)) < 0) {
__redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
return REDIS_ERR;
}
#else
#if defined(__GLIBC__) && !defined(__FreeBSD_kernel__)
if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
__redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
return REDIS_ERR;
}

val = interval/3;
if (val == 0) val = 1;
if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
__redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
return REDIS_ERR;
}

val = 3;
if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
__redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
return REDIS_ERR;
}
#endif
#endif

return REDIS_OK;
}

int redisSetTcpNoDelay(redisContext *c) {
int yes = 1;
if (setsockopt(c->fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,"setsockopt(TCP_NODELAY)");
redisNetClose(c);
return REDIS_ERR;
}
return REDIS_OK;
}

#define __MAX_MSEC (((LONG_MAX) - 999) / 1000)

static int redisContextTimeoutMsec(redisContext *c, long *result)
{
const struct timeval *timeout = c->connect_timeout;
long msec = -1;


if (timeout != NULL) {
if (timeout->tv_usec > 1000000 || timeout->tv_sec > __MAX_MSEC) {
*result = msec;
return REDIS_ERR;
}

msec = (timeout->tv_sec * 1000) + ((timeout->tv_usec + 999) / 1000);

if (msec < 0 || msec > INT_MAX) {
msec = INT_MAX;
}
}

*result = msec;
return REDIS_OK;
}

static int redisContextWaitReady(redisContext *c, long msec) {
struct pollfd wfd[1];

wfd[0].fd = c->fd;
wfd[0].events = POLLOUT;

if (errno == EINPROGRESS) {
int res;

if ((res = poll(wfd, 1, msec)) == -1) {
__redisSetErrorFromErrno(c, REDIS_ERR_IO, "poll(2)");
redisNetClose(c);
return REDIS_ERR;
} else if (res == 0) {
errno = ETIMEDOUT;
__redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
redisNetClose(c);
return REDIS_ERR;
}

if (redisCheckConnectDone(c, &res) != REDIS_OK || res == 0) {
redisCheckSocketError(c);
return REDIS_ERR;
}

return REDIS_OK;
}

__redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
redisNetClose(c);
return REDIS_ERR;
}

int redisCheckConnectDone(redisContext *c, int *completed) {
int rc = connect(c->fd, (const struct sockaddr *)c->saddr, c->addrlen);
if (rc == 0) {
*completed = 1;
return REDIS_OK;
}
switch (errno) {
case EISCONN:
*completed = 1;
return REDIS_OK;
case EALREADY:
case EINPROGRESS:
case EWOULDBLOCK:
*completed = 0;
return REDIS_OK;
default:
return REDIS_ERR;
}
}

int redisCheckSocketError(redisContext *c) {
int err = 0, errno_saved = errno;
socklen_t errlen = sizeof(err);

if (getsockopt(c->fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,"getsockopt(SO_ERROR)");
return REDIS_ERR;
}

if (err == 0) {
err = errno_saved;
}

if (err) {
errno = err;
__redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
return REDIS_ERR;
}

return REDIS_OK;
}

int redisContextSetTimeout(redisContext *c, const struct timeval tv) {
const void *to_ptr = &tv;
size_t to_sz = sizeof(tv);

if (setsockopt(c->fd,SOL_SOCKET,SO_RCVTIMEO,to_ptr,to_sz) == -1) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,"setsockopt(SO_RCVTIMEO)");
return REDIS_ERR;
}
if (setsockopt(c->fd,SOL_SOCKET,SO_SNDTIMEO,to_ptr,to_sz) == -1) {
__redisSetErrorFromErrno(c,REDIS_ERR_IO,"setsockopt(SO_SNDTIMEO)");
return REDIS_ERR;
}
return REDIS_OK;
}

int redisContextUpdateConnectTimeout(redisContext *c, const struct timeval *timeout) {

if (c->connect_timeout == timeout)
return REDIS_OK;


if (c->connect_timeout == NULL) {
c->connect_timeout = hi_malloc(sizeof(*c->connect_timeout));
if (c->connect_timeout == NULL)
return REDIS_ERR;
}

memcpy(c->connect_timeout, timeout, sizeof(*c->connect_timeout));
return REDIS_OK;
}

int redisContextUpdateCommandTimeout(redisContext *c, const struct timeval *timeout) {

if (c->command_timeout == timeout)
return REDIS_OK;


if (c->command_timeout == NULL) {
c->command_timeout = hi_malloc(sizeof(*c->command_timeout));
if (c->command_timeout == NULL)
return REDIS_ERR;
}

memcpy(c->command_timeout, timeout, sizeof(*c->command_timeout));
return REDIS_OK;
}

static int _redisContextConnectTcp(redisContext *c, const char *addr, int port,
const struct timeval *timeout,
const char *source_addr) {
redisFD s;
int rv, n;
char _port[6];
struct addrinfo hints, *servinfo, *bservinfo, *p, *b;
int blocking = (c->flags & REDIS_BLOCK);
int reuseaddr = (c->flags & REDIS_REUSEADDR);
int reuses = 0;
long timeout_msec = -1;

servinfo = NULL;
c->connection_type = REDIS_CONN_TCP;
c->tcp.port = port;








if (c->tcp.host != addr) {
hi_free(c->tcp.host);

c->tcp.host = hi_strdup(addr);
if (c->tcp.host == NULL)
goto oom;
}

if (timeout) {
if (redisContextUpdateConnectTimeout(c, timeout) == REDIS_ERR)
goto oom;
} else {
hi_free(c->connect_timeout);
c->connect_timeout = NULL;
}

if (redisContextTimeoutMsec(c, &timeout_msec) != REDIS_OK) {
__redisSetError(c, REDIS_ERR_IO, "Invalid timeout specified");
goto error;
}

if (source_addr == NULL) {
hi_free(c->tcp.source_addr);
c->tcp.source_addr = NULL;
} else if (c->tcp.source_addr != source_addr) {
hi_free(c->tcp.source_addr);
c->tcp.source_addr = hi_strdup(source_addr);
}

snprintf(_port, 6, "%d", port);
memset(&hints,0,sizeof(hints));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;






if ((rv = getaddrinfo(c->tcp.host,_port,&hints,&servinfo)) != 0) {
hints.ai_family = AF_INET6;
if ((rv = getaddrinfo(addr,_port,&hints,&servinfo)) != 0) {
__redisSetError(c,REDIS_ERR_OTHER,gai_strerror(rv));
return REDIS_ERR;
}
}
for (p = servinfo; p != NULL; p = p->ai_next) {
addrretry:
if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == REDIS_INVALID_FD)
continue;

c->fd = s;
if (redisSetBlocking(c,0) != REDIS_OK)
goto error;
if (c->tcp.source_addr) {
int bound = 0;

if ((rv = getaddrinfo(c->tcp.source_addr, NULL, &hints, &bservinfo)) != 0) {
char buf[128];
snprintf(buf,sizeof(buf),"Can't get addr: %s",gai_strerror(rv));
__redisSetError(c,REDIS_ERR_OTHER,buf);
goto error;
}

if (reuseaddr) {
n = 1;
if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*) &n,
sizeof(n)) < 0) {
freeaddrinfo(bservinfo);
goto error;
}
}

for (b = bservinfo; b != NULL; b = b->ai_next) {
if (bind(s,b->ai_addr,b->ai_addrlen) != -1) {
bound = 1;
break;
}
}
freeaddrinfo(bservinfo);
if (!bound) {
char buf[128];
snprintf(buf,sizeof(buf),"Can't bind socket: %s",strerror(errno));
__redisSetError(c,REDIS_ERR_OTHER,buf);
goto error;
}
}


hi_free(c->saddr);
c->saddr = hi_malloc(p->ai_addrlen);
if (c->saddr == NULL)
goto oom;

memcpy(c->saddr, p->ai_addr, p->ai_addrlen);
c->addrlen = p->ai_addrlen;

if (connect(s,p->ai_addr,p->ai_addrlen) == -1) {
if (errno == EHOSTUNREACH) {
redisNetClose(c);
continue;
} else if (errno == EINPROGRESS) {
if (blocking) {
goto wait_for_ready;
}




} else if (errno == EADDRNOTAVAIL && reuseaddr) {
if (++reuses >= REDIS_CONNECT_RETRIES) {
goto error;
} else {
redisNetClose(c);
goto addrretry;
}
} else {
wait_for_ready:
if (redisContextWaitReady(c,timeout_msec) != REDIS_OK)
goto error;
if (redisSetTcpNoDelay(c) != REDIS_OK)
goto error;
}
}
if (blocking && redisSetBlocking(c,1) != REDIS_OK)
goto error;

c->flags |= REDIS_CONNECTED;
rv = REDIS_OK;
goto end;
}
if (p == NULL) {
char buf[128];
snprintf(buf,sizeof(buf),"Can't create socket: %s",strerror(errno));
__redisSetError(c,REDIS_ERR_OTHER,buf);
goto error;
}

oom:
__redisSetError(c, REDIS_ERR_OOM, "Out of memory");
error:
rv = REDIS_ERR;
end:
if(servinfo) {
freeaddrinfo(servinfo);
}

return rv;
}

int redisContextConnectTcp(redisContext *c, const char *addr, int port,
const struct timeval *timeout) {
return _redisContextConnectTcp(c, addr, port, timeout, NULL);
}

int redisContextConnectBindTcp(redisContext *c, const char *addr, int port,
const struct timeval *timeout,
const char *source_addr) {
return _redisContextConnectTcp(c, addr, port, timeout, source_addr);
}

int redisContextConnectUnix(redisContext *c, const char *path, const struct timeval *timeout) {
#if !defined(_WIN32)
int blocking = (c->flags & REDIS_BLOCK);
struct sockaddr_un *sa;
long timeout_msec = -1;

if (redisCreateSocket(c,AF_UNIX) < 0)
return REDIS_ERR;
if (redisSetBlocking(c,0) != REDIS_OK)
return REDIS_ERR;

c->connection_type = REDIS_CONN_UNIX;
if (c->unix_sock.path != path) {
hi_free(c->unix_sock.path);

c->unix_sock.path = hi_strdup(path);
if (c->unix_sock.path == NULL)
goto oom;
}

if (timeout) {
if (redisContextUpdateConnectTimeout(c, timeout) == REDIS_ERR)
goto oom;
} else {
hi_free(c->connect_timeout);
c->connect_timeout = NULL;
}

if (redisContextTimeoutMsec(c,&timeout_msec) != REDIS_OK)
return REDIS_ERR;


if (c->saddr) hi_free(c->saddr);

sa = (struct sockaddr_un*)(c->saddr = hi_malloc(sizeof(struct sockaddr_un)));
if (sa == NULL)
goto oom;

c->addrlen = sizeof(struct sockaddr_un);
sa->sun_family = AF_UNIX;
strncpy(sa->sun_path, path, sizeof(sa->sun_path) - 1);
if (connect(c->fd, (struct sockaddr*)sa, sizeof(*sa)) == -1) {
if (errno == EINPROGRESS && !blocking) {

} else {
if (redisContextWaitReady(c,timeout_msec) != REDIS_OK)
return REDIS_ERR;
}
}


if (blocking && redisSetBlocking(c,1) != REDIS_OK)
return REDIS_ERR;

c->flags |= REDIS_CONNECTED;
return REDIS_OK;
#else


errno = EPROTONOSUPPORT;
return REDIS_ERR;
#endif
oom:
__redisSetError(c, REDIS_ERR_OOM, "Out of memory");
return REDIS_ERR;
}
