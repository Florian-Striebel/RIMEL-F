



























#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(EVENT__HAVE_STDARG_H)
#include <stdarg.h>
#endif
#if defined(EVENT__HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <sys/queue.h>

#include "event2/util.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/bufferevent_struct.h"
#include "event2/event.h"
#include "event2/util.h"
#include "event-internal.h"
#include "log-internal.h"
#include "mm-internal.h"
#include "bufferevent-internal.h"
#include "util-internal.h"
#include "iocp-internal.h"

#if !defined(SO_UPDATE_CONNECT_CONTEXT)

#define SO_UPDATE_CONNECT_CONTEXT 0x7010
#endif


static int be_async_enable(struct bufferevent *, short);
static int be_async_disable(struct bufferevent *, short);
static void be_async_destruct(struct bufferevent *);
static int be_async_flush(struct bufferevent *, short, enum bufferevent_flush_mode);
static int be_async_ctrl(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);

struct bufferevent_async {
struct bufferevent_private bev;
struct event_overlapped connect_overlapped;
struct event_overlapped read_overlapped;
struct event_overlapped write_overlapped;
size_t read_in_progress;
size_t write_in_progress;
unsigned ok : 1;
unsigned read_added : 1;
unsigned write_added : 1;
};

const struct bufferevent_ops bufferevent_ops_async = {
"socket_async",
evutil_offsetof(struct bufferevent_async, bev.bev),
be_async_enable,
be_async_disable,
NULL,
be_async_destruct,
bufferevent_generic_adj_timeouts_,
be_async_flush,
be_async_ctrl,
};

static inline struct bufferevent_async *
upcast(struct bufferevent *bev)
{
struct bufferevent_async *bev_a;
if (bev->be_ops != &bufferevent_ops_async)
return NULL;
bev_a = EVUTIL_UPCAST(bev, struct bufferevent_async, bev.bev);
return bev_a;
}

static inline struct bufferevent_async *
upcast_connect(struct event_overlapped *eo)
{
struct bufferevent_async *bev_a;
bev_a = EVUTIL_UPCAST(eo, struct bufferevent_async, connect_overlapped);
EVUTIL_ASSERT(BEV_IS_ASYNC(&bev_a->bev.bev));
return bev_a;
}

static inline struct bufferevent_async *
upcast_read(struct event_overlapped *eo)
{
struct bufferevent_async *bev_a;
bev_a = EVUTIL_UPCAST(eo, struct bufferevent_async, read_overlapped);
EVUTIL_ASSERT(BEV_IS_ASYNC(&bev_a->bev.bev));
return bev_a;
}

static inline struct bufferevent_async *
upcast_write(struct event_overlapped *eo)
{
struct bufferevent_async *bev_a;
bev_a = EVUTIL_UPCAST(eo, struct bufferevent_async, write_overlapped);
EVUTIL_ASSERT(BEV_IS_ASYNC(&bev_a->bev.bev));
return bev_a;
}

static void
bev_async_del_write(struct bufferevent_async *beva)
{
struct bufferevent *bev = &beva->bev.bev;

if (beva->write_added) {
beva->write_added = 0;
event_base_del_virtual_(bev->ev_base);
}
}

static void
bev_async_del_read(struct bufferevent_async *beva)
{
struct bufferevent *bev = &beva->bev.bev;

if (beva->read_added) {
beva->read_added = 0;
event_base_del_virtual_(bev->ev_base);
}
}

static void
bev_async_add_write(struct bufferevent_async *beva)
{
struct bufferevent *bev = &beva->bev.bev;

if (!beva->write_added) {
beva->write_added = 1;
event_base_add_virtual_(bev->ev_base);
}
}

static void
bev_async_add_read(struct bufferevent_async *beva)
{
struct bufferevent *bev = &beva->bev.bev;

if (!beva->read_added) {
beva->read_added = 1;
event_base_add_virtual_(bev->ev_base);
}
}

static void
bev_async_consider_writing(struct bufferevent_async *beva)
{
size_t at_most;
int limit;
struct bufferevent *bev = &beva->bev.bev;



if (beva->write_in_progress || beva->bev.connecting)
return;
if (!beva->ok || !(bev->enabled&EV_WRITE) ||
!evbuffer_get_length(bev->output)) {
bev_async_del_write(beva);
return;
}

at_most = evbuffer_get_length(bev->output);



limit = (int)bufferevent_get_write_max_(&beva->bev);
if (at_most >= (size_t)limit && limit >= 0)
at_most = limit;

if (beva->bev.write_suspended) {
bev_async_del_write(beva);
return;
}


bufferevent_incref_(bev);
if (evbuffer_launch_write_(bev->output, at_most,
&beva->write_overlapped)) {
bufferevent_decref_(bev);
beva->ok = 0;
bufferevent_run_eventcb_(bev, BEV_EVENT_ERROR, 0);
} else {
beva->write_in_progress = at_most;
bufferevent_decrement_write_buckets_(&beva->bev, at_most);
bev_async_add_write(beva);
}
}

static void
bev_async_consider_reading(struct bufferevent_async *beva)
{
size_t cur_size;
size_t read_high;
size_t at_most;
int limit;
struct bufferevent *bev = &beva->bev.bev;



if (beva->read_in_progress || beva->bev.connecting)
return;
if (!beva->ok || !(bev->enabled&EV_READ)) {
bev_async_del_read(beva);
return;
}


cur_size = evbuffer_get_length(bev->input);
read_high = bev->wm_read.high;
if (read_high) {
if (cur_size >= read_high) {
bev_async_del_read(beva);
return;
}
at_most = read_high - cur_size;
} else {
at_most = 16384;
}



limit = (int)bufferevent_get_read_max_(&beva->bev);
if (at_most >= (size_t)limit && limit >= 0)
at_most = limit;

if (beva->bev.read_suspended) {
bev_async_del_read(beva);
return;
}

bufferevent_incref_(bev);
if (evbuffer_launch_read_(bev->input, at_most, &beva->read_overlapped)) {
beva->ok = 0;
bufferevent_run_eventcb_(bev, BEV_EVENT_ERROR, 0);
bufferevent_decref_(bev);
} else {
beva->read_in_progress = at_most;
bufferevent_decrement_read_buckets_(&beva->bev, at_most);
bev_async_add_read(beva);
}

return;
}

static void
be_async_outbuf_callback(struct evbuffer *buf,
const struct evbuffer_cb_info *cbinfo,
void *arg)
{
struct bufferevent *bev = arg;
struct bufferevent_async *bev_async = upcast(bev);




bufferevent_incref_and_lock_(bev);

if (cbinfo->n_added)
bev_async_consider_writing(bev_async);

bufferevent_decref_and_unlock_(bev);
}

static void
be_async_inbuf_callback(struct evbuffer *buf,
const struct evbuffer_cb_info *cbinfo,
void *arg)
{
struct bufferevent *bev = arg;
struct bufferevent_async *bev_async = upcast(bev);




bufferevent_incref_and_lock_(bev);

if (cbinfo->n_deleted)
bev_async_consider_reading(bev_async);

bufferevent_decref_and_unlock_(bev);
}

static int
be_async_enable(struct bufferevent *buf, short what)
{
struct bufferevent_async *bev_async = upcast(buf);

if (!bev_async->ok)
return -1;

if (bev_async->bev.connecting) {

return 0;
}

if (what & EV_READ)
BEV_RESET_GENERIC_READ_TIMEOUT(buf);
if (what & EV_WRITE)
BEV_RESET_GENERIC_WRITE_TIMEOUT(buf);




if (what & EV_READ)
bev_async_consider_reading(bev_async);
if (what & EV_WRITE)
bev_async_consider_writing(bev_async);
return 0;
}

static int
be_async_disable(struct bufferevent *bev, short what)
{
struct bufferevent_async *bev_async = upcast(bev);




if (what & EV_READ) {
BEV_DEL_GENERIC_READ_TIMEOUT(bev);
bev_async_del_read(bev_async);
}
if (what & EV_WRITE) {
BEV_DEL_GENERIC_WRITE_TIMEOUT(bev);
bev_async_del_write(bev_async);
}

return 0;
}

static void
be_async_destruct(struct bufferevent *bev)
{
struct bufferevent_async *bev_async = upcast(bev);
struct bufferevent_private *bev_p = BEV_UPCAST(bev);
evutil_socket_t fd;

EVUTIL_ASSERT(!upcast(bev)->write_in_progress &&
!upcast(bev)->read_in_progress);

bev_async_del_read(bev_async);
bev_async_del_write(bev_async);

fd = evbuffer_overlapped_get_fd_(bev->input);
if (fd != (evutil_socket_t)INVALID_SOCKET &&
(bev_p->options & BEV_OPT_CLOSE_ON_FREE)) {
evutil_closesocket(fd);
evbuffer_overlapped_set_fd_(bev->input, INVALID_SOCKET);
}
}



static void
bev_async_set_wsa_error(struct bufferevent *bev, struct event_overlapped *eo)
{
DWORD bytes, flags;
evutil_socket_t fd;

fd = evbuffer_overlapped_get_fd_(bev->input);
WSAGetOverlappedResult(fd, &eo->overlapped, &bytes, FALSE, &flags);
}

static int
be_async_flush(struct bufferevent *bev, short what,
enum bufferevent_flush_mode mode)
{
return 0;
}

static void
connect_complete(struct event_overlapped *eo, ev_uintptr_t key,
ev_ssize_t nbytes, int ok)
{
struct bufferevent_async *bev_a = upcast_connect(eo);
struct bufferevent *bev = &bev_a->bev.bev;
evutil_socket_t sock;

BEV_LOCK(bev);

EVUTIL_ASSERT(bev_a->bev.connecting);
bev_a->bev.connecting = 0;
sock = evbuffer_overlapped_get_fd_(bev_a->bev.bev.input);

setsockopt(sock, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);

if (ok)
bufferevent_async_set_connected_(bev);
else
bev_async_set_wsa_error(bev, eo);

bufferevent_run_eventcb_(bev,
ok? BEV_EVENT_CONNECTED : BEV_EVENT_ERROR, 0);

event_base_del_virtual_(bev->ev_base);

bufferevent_decref_and_unlock_(bev);
}

static void
read_complete(struct event_overlapped *eo, ev_uintptr_t key,
ev_ssize_t nbytes, int ok)
{
struct bufferevent_async *bev_a = upcast_read(eo);
struct bufferevent *bev = &bev_a->bev.bev;
short what = BEV_EVENT_READING;
ev_ssize_t amount_unread;
BEV_LOCK(bev);
EVUTIL_ASSERT(bev_a->read_in_progress);

amount_unread = bev_a->read_in_progress - nbytes;
evbuffer_commit_read_(bev->input, nbytes);
bev_a->read_in_progress = 0;
if (amount_unread)
bufferevent_decrement_read_buckets_(&bev_a->bev, -amount_unread);

if (!ok)
bev_async_set_wsa_error(bev, eo);

if (bev_a->ok) {
if (ok && nbytes) {
BEV_RESET_GENERIC_READ_TIMEOUT(bev);
bufferevent_trigger_nolock_(bev, EV_READ, 0);
bev_async_consider_reading(bev_a);
} else if (!ok) {
what |= BEV_EVENT_ERROR;
bev_a->ok = 0;
bufferevent_run_eventcb_(bev, what, 0);
} else if (!nbytes) {
what |= BEV_EVENT_EOF;
bev_a->ok = 0;
bufferevent_run_eventcb_(bev, what, 0);
}
}

bufferevent_decref_and_unlock_(bev);
}

static void
write_complete(struct event_overlapped *eo, ev_uintptr_t key,
ev_ssize_t nbytes, int ok)
{
struct bufferevent_async *bev_a = upcast_write(eo);
struct bufferevent *bev = &bev_a->bev.bev;
short what = BEV_EVENT_WRITING;
ev_ssize_t amount_unwritten;

BEV_LOCK(bev);
EVUTIL_ASSERT(bev_a->write_in_progress);

amount_unwritten = bev_a->write_in_progress - nbytes;
evbuffer_commit_write_(bev->output, nbytes);
bev_a->write_in_progress = 0;

if (amount_unwritten)
bufferevent_decrement_write_buckets_(&bev_a->bev,
-amount_unwritten);


if (!ok)
bev_async_set_wsa_error(bev, eo);

if (bev_a->ok) {
if (ok && nbytes) {
BEV_RESET_GENERIC_WRITE_TIMEOUT(bev);
bufferevent_trigger_nolock_(bev, EV_WRITE, 0);
bev_async_consider_writing(bev_a);
} else if (!ok) {
what |= BEV_EVENT_ERROR;
bev_a->ok = 0;
bufferevent_run_eventcb_(bev, what, 0);
} else if (!nbytes) {
what |= BEV_EVENT_EOF;
bev_a->ok = 0;
bufferevent_run_eventcb_(bev, what, 0);
}
}

bufferevent_decref_and_unlock_(bev);
}

struct bufferevent *
bufferevent_async_new_(struct event_base *base,
evutil_socket_t fd, int options)
{
struct bufferevent_async *bev_a;
struct bufferevent *bev;
struct event_iocp_port *iocp;

options |= BEV_OPT_THREADSAFE;

if (!(iocp = event_base_get_iocp_(base)))
return NULL;

if (fd >= 0 && event_iocp_port_associate_(iocp, fd, 1)<0) {
int err = GetLastError();



if (err != ERROR_INVALID_PARAMETER)
return NULL;
}

if (!(bev_a = mm_calloc(1, sizeof(struct bufferevent_async))))
return NULL;

bev = &bev_a->bev.bev;
if (!(bev->input = evbuffer_overlapped_new_(fd))) {
mm_free(bev_a);
return NULL;
}
if (!(bev->output = evbuffer_overlapped_new_(fd))) {
evbuffer_free(bev->input);
mm_free(bev_a);
return NULL;
}

if (bufferevent_init_common_(&bev_a->bev, base, &bufferevent_ops_async,
options)<0)
goto err;

evbuffer_add_cb(bev->input, be_async_inbuf_callback, bev);
evbuffer_add_cb(bev->output, be_async_outbuf_callback, bev);

event_overlapped_init_(&bev_a->connect_overlapped, connect_complete);
event_overlapped_init_(&bev_a->read_overlapped, read_complete);
event_overlapped_init_(&bev_a->write_overlapped, write_complete);

bufferevent_init_generic_timeout_cbs_(bev);

bev_a->ok = fd >= 0;

return bev;
err:
bufferevent_free(&bev_a->bev.bev);
return NULL;
}

void
bufferevent_async_set_connected_(struct bufferevent *bev)
{
struct bufferevent_async *bev_async = upcast(bev);
bev_async->ok = 1;
bufferevent_init_generic_timeout_cbs_(bev);

be_async_enable(bev, bev->enabled);
}

int
bufferevent_async_can_connect_(struct bufferevent *bev)
{
const struct win32_extension_fns *ext =
event_get_win32_extension_fns_();

if (BEV_IS_ASYNC(bev) &&
event_base_get_iocp_(bev->ev_base) &&
ext && ext->ConnectEx)
return 1;

return 0;
}

int
bufferevent_async_connect_(struct bufferevent *bev, evutil_socket_t fd,
const struct sockaddr *sa, int socklen)
{
BOOL rc;
struct bufferevent_async *bev_async = upcast(bev);
struct sockaddr_storage ss;
const struct win32_extension_fns *ext =
event_get_win32_extension_fns_();

EVUTIL_ASSERT(ext && ext->ConnectEx && fd >= 0 && sa != NULL);





memset(&ss, 0, sizeof(ss));
if (sa->sa_family == AF_INET) {
struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
sin->sin_family = AF_INET;
sin->sin_addr.s_addr = INADDR_ANY;
} else if (sa->sa_family == AF_INET6) {
struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
sin6->sin6_family = AF_INET6;
sin6->sin6_addr = in6addr_any;
} else {

return -1;
}
if (bind(fd, (struct sockaddr *)&ss, sizeof(ss)) < 0 &&
WSAGetLastError() != WSAEINVAL)
return -1;

event_base_add_virtual_(bev->ev_base);
bufferevent_incref_(bev);
rc = ext->ConnectEx(fd, sa, socklen, NULL, 0, NULL,
&bev_async->connect_overlapped.overlapped);
if (rc || WSAGetLastError() == ERROR_IO_PENDING)
return 0;

event_base_del_virtual_(bev->ev_base);
bufferevent_decref_(bev);

return -1;
}

static int
be_async_ctrl(struct bufferevent *bev, enum bufferevent_ctrl_op op,
union bufferevent_ctrl_data *data)
{
switch (op) {
case BEV_CTRL_GET_FD:
data->fd = evbuffer_overlapped_get_fd_(bev->input);
return 0;
case BEV_CTRL_SET_FD: {
struct event_iocp_port *iocp;

if (data->fd == evbuffer_overlapped_get_fd_(bev->input))
return 0;
if (!(iocp = event_base_get_iocp_(bev->ev_base)))
return -1;
if (event_iocp_port_associate_(iocp, data->fd, 1) < 0)
return -1;
evbuffer_overlapped_set_fd_(bev->input, data->fd);
evbuffer_overlapped_set_fd_(bev->output, data->fd);
return 0;
}
case BEV_CTRL_CANCEL_ALL: {
struct bufferevent_async *bev_a = upcast(bev);
evutil_socket_t fd = evbuffer_overlapped_get_fd_(bev->input);
if (fd != (evutil_socket_t)INVALID_SOCKET &&
(bev_a->bev.options & BEV_OPT_CLOSE_ON_FREE)) {
closesocket(fd);
evbuffer_overlapped_set_fd_(bev->input, INVALID_SOCKET);
}
bev_a->ok = 0;
return 0;
}
case BEV_CTRL_GET_UNDERLYING:
default:
return -1;
}
}


