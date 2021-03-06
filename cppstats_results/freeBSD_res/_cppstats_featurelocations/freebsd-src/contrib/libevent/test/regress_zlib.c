


























#undef NDEBUG

#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include "event2/event-config.h"

#include <sys/types.h>
#if !defined(_WIN32)
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>

#include "event2/util.h"
#include "event2/event.h"
#include "event2/event_compat.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"

#include "regress.h"
#include "mm-internal.h"








#if !defined(_LARGEFILE64_SOURCE)
#define _LARGEFILE64_SOURCE 0
#endif
#if !defined(_LFS64_LARGEFILE)
#define _LFS64_LARGEFILE 0
#endif
#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 0
#endif
#if !defined(off64_t)
#define off64_t ev_int64_t
#endif

#include <zlib.h>

static int infilter_calls;
static int outfilter_calls;
static int readcb_finished;
static int writecb_finished;
static int errorcb_invoked;





static void
zlib_deflate_free(void *ctx)
{
z_streamp p = ctx;

assert(deflateEnd(p) == Z_OK);
mm_free(p);
}

static void
zlib_inflate_free(void *ctx)
{
z_streamp p = ctx;

assert(inflateEnd(p) == Z_OK);
mm_free(p);
}

static int
getstate(enum bufferevent_flush_mode state)
{
switch (state) {
case BEV_FINISHED:
return Z_FINISH;
case BEV_FLUSH:
return Z_SYNC_FLUSH;
case BEV_NORMAL:
default:
return Z_NO_FLUSH;
}
}






static enum bufferevent_filter_result
zlib_input_filter(struct evbuffer *src, struct evbuffer *dst,
ev_ssize_t lim, enum bufferevent_flush_mode state, void *ctx)
{
struct evbuffer_iovec v_in[1];
struct evbuffer_iovec v_out[1];
int nread, nwrite;
int res, n;

z_streamp p = ctx;

do {

n = evbuffer_peek(src, -1, NULL, v_in, 1);
if (n) {
p->avail_in = v_in[0].iov_len;
p->next_in = (unsigned char *)v_in[0].iov_base;
} else {
p->avail_in = 0;
p->next_in = 0;
}

evbuffer_reserve_space(dst, 4096, v_out, 1);
p->next_out = (unsigned char *)v_out[0].iov_base;
p->avail_out = v_out[0].iov_len;


res = inflate(p, getstate(state));


nread = v_in[0].iov_len - p->avail_in;
nwrite = v_out[0].iov_len - p->avail_out;

evbuffer_drain(src, nread);
v_out[0].iov_len = nwrite;
evbuffer_commit_space(dst, v_out, 1);

if (res==Z_BUF_ERROR) {



if (nwrite == 0)
return BEV_NEED_MORE;
} else {
assert(res == Z_OK || res == Z_STREAM_END);
}

} while (evbuffer_get_length(src) > 0);

++infilter_calls;

return (BEV_OK);
}

static enum bufferevent_filter_result
zlib_output_filter(struct evbuffer *src, struct evbuffer *dst,
ev_ssize_t lim, enum bufferevent_flush_mode state, void *ctx)
{
struct evbuffer_iovec v_in[1];
struct evbuffer_iovec v_out[1];
int nread, nwrite;
int res, n;

z_streamp p = ctx;

do {

n = evbuffer_peek(src, -1, NULL, v_in, 1);
if (n) {
p->avail_in = v_in[0].iov_len;
p->next_in = (unsigned char *)v_in[0].iov_base;
} else {
p->avail_in = 0;
p->next_in = 0;
}

evbuffer_reserve_space(dst, 4096, v_out, 1);
p->next_out = (unsigned char *)v_out[0].iov_base;
p->avail_out = v_out[0].iov_len;


res = deflate(p, getstate(state));


nread = v_in[0].iov_len - p->avail_in;
nwrite = v_out[0].iov_len - p->avail_out;

evbuffer_drain(src, nread);
v_out[0].iov_len = nwrite;
evbuffer_commit_space(dst, v_out, 1);

if (res==Z_BUF_ERROR) {



if (nwrite == 0)
return BEV_NEED_MORE;
} else {
assert(res == Z_OK || res == Z_STREAM_END);
}

} while (evbuffer_get_length(src) > 0);

++outfilter_calls;

return (BEV_OK);
}





static void
readcb(struct bufferevent *bev, void *arg)
{
if (evbuffer_get_length(bufferevent_get_input(bev)) == 8333) {
struct evbuffer *evbuf = evbuffer_new();
assert(evbuf != NULL);


bufferevent_read_buffer(bev, evbuf);

bufferevent_disable(bev, EV_READ);

if (evbuffer_get_length(evbuf) == 8333) {
++readcb_finished;
}

evbuffer_free(evbuf);
}
}

static void
writecb(struct bufferevent *bev, void *arg)
{
if (evbuffer_get_length(bufferevent_get_output(bev)) == 0) {
++writecb_finished;
}
}

static void
errorcb(struct bufferevent *bev, short what, void *arg)
{
errorcb_invoked = 1;
}

void
test_bufferevent_zlib(void *arg)
{
struct bufferevent *bev1=NULL, *bev2=NULL;
char buffer[8333];
z_stream *z_input, *z_output;
int i, r;
evutil_socket_t pair[2] = {-1, -1};
(void)arg;

infilter_calls = outfilter_calls = readcb_finished = writecb_finished
= errorcb_invoked = 0;

if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1) {
tt_abort_perror("socketpair");
}

evutil_make_socket_nonblocking(pair[0]);
evutil_make_socket_nonblocking(pair[1]);

bev1 = bufferevent_socket_new(NULL, pair[0], 0);
bev2 = bufferevent_socket_new(NULL, pair[1], 0);

z_output = mm_calloc(sizeof(*z_output), 1);
r = deflateInit(z_output, Z_DEFAULT_COMPRESSION);
tt_int_op(r, ==, Z_OK);
z_input = mm_calloc(sizeof(*z_input), 1);
r = inflateInit(z_input);
tt_int_op(r, ==, Z_OK);


bev1 = bufferevent_filter_new(bev1, NULL, zlib_output_filter,
BEV_OPT_CLOSE_ON_FREE, zlib_deflate_free, z_output);
bev2 = bufferevent_filter_new(bev2, zlib_input_filter,
NULL, BEV_OPT_CLOSE_ON_FREE, zlib_inflate_free, z_input);
bufferevent_setcb(bev1, readcb, writecb, errorcb, NULL);
bufferevent_setcb(bev2, readcb, writecb, errorcb, NULL);

bufferevent_disable(bev1, EV_READ);
bufferevent_enable(bev1, EV_WRITE);

bufferevent_enable(bev2, EV_READ);

for (i = 0; i < (int)sizeof(buffer); i++)
buffer[i] = i;


bufferevent_write(bev1, buffer, 1800);
bufferevent_write(bev1, buffer + 1800, sizeof(buffer) - 1800);


bufferevent_flush(bev1, EV_WRITE, BEV_FINISHED);

event_dispatch();

tt_want(infilter_calls);
tt_want(outfilter_calls);
tt_want(readcb_finished);
tt_want(writecb_finished);
tt_want(!errorcb_invoked);

test_ok = 1;
end:
if (bev1)
bufferevent_free(bev1);
if (bev2)
bufferevent_free(bev2);

if (pair[0] >= 0)
evutil_closesocket(pair[0]);
if (pair[1] >= 0)
evutil_closesocket(pair[1]);
}
