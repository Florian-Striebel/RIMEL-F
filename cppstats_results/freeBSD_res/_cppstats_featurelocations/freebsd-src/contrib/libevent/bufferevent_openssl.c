


























#if defined(__APPLE__) && defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "event2/event-config.h"
#include "evconfig-private.h"

#include <sys/types.h>

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
#endif

#include "event2/bufferevent.h"
#include "event2/bufferevent_struct.h"
#include "event2/bufferevent_ssl.h"
#include "event2/buffer.h"
#include "event2/event.h"

#include "mm-internal.h"
#include "bufferevent-internal.h"
#include "log-internal.h"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "openssl-compat.h"
















#define BIO_TYPE_LIBEVENT 57



#if 0
static void
print_err(int val)
{
int err;
printf("Error was %d\n", val);

while ((err = ERR_get_error())) {
const char *msg = (const char*)ERR_reason_error_string(err);
const char *lib = (const char*)ERR_lib_error_string(err);
const char *func = (const char*)ERR_func_error_string(err);

printf("%s in %s %s\n", msg, lib, func);
}
}
#else
#define print_err(v) ((void)0)
#endif


static int
bio_bufferevent_new(BIO *b)
{
BIO_set_init(b, 0);
BIO_set_data(b, NULL);
return 1;
}


static int
bio_bufferevent_free(BIO *b)
{
if (!b)
return 0;
if (BIO_get_shutdown(b)) {
if (BIO_get_init(b) && BIO_get_data(b))
bufferevent_free(BIO_get_data(b));
BIO_free(b);
}
return 1;
}


static int
bio_bufferevent_read(BIO *b, char *out, int outlen)
{
int r = 0;
struct evbuffer *input;

BIO_clear_retry_flags(b);

if (!out)
return 0;
if (!BIO_get_data(b))
return -1;

input = bufferevent_get_input(BIO_get_data(b));
if (evbuffer_get_length(input) == 0) {

BIO_set_retry_read(b);
return -1;
} else {
r = evbuffer_remove(input, out, outlen);
}

return r;
}


static int
bio_bufferevent_write(BIO *b, const char *in, int inlen)
{
struct bufferevent *bufev = BIO_get_data(b);
struct evbuffer *output;
size_t outlen;

BIO_clear_retry_flags(b);

if (!BIO_get_data(b))
return -1;

output = bufferevent_get_output(bufev);
outlen = evbuffer_get_length(output);



if (bufev->wm_write.high && bufev->wm_write.high <= (outlen+inlen)) {
if (bufev->wm_write.high <= outlen) {

BIO_set_retry_write(b);
return -1;
}
inlen = bufev->wm_write.high - outlen;
}

EVUTIL_ASSERT(inlen > 0);
evbuffer_add(output, in, inlen);
return inlen;
}


static long
bio_bufferevent_ctrl(BIO *b, int cmd, long num, void *ptr)
{
struct bufferevent *bufev = BIO_get_data(b);
long ret = 1;

switch (cmd) {
case BIO_CTRL_GET_CLOSE:
ret = BIO_get_shutdown(b);
break;
case BIO_CTRL_SET_CLOSE:
BIO_set_shutdown(b, (int)num);
break;
case BIO_CTRL_PENDING:
ret = evbuffer_get_length(bufferevent_get_input(bufev)) != 0;
break;
case BIO_CTRL_WPENDING:
ret = evbuffer_get_length(bufferevent_get_output(bufev)) != 0;
break;


case BIO_CTRL_DUP:
case BIO_CTRL_FLUSH:
ret = 1;
break;
default:
ret = 0;
break;
}
return ret;
}


static int
bio_bufferevent_puts(BIO *b, const char *s)
{
return bio_bufferevent_write(b, s, strlen(s));
}


static BIO_METHOD *methods_bufferevent;


static BIO_METHOD *
BIO_s_bufferevent(void)
{
if (methods_bufferevent == NULL) {
methods_bufferevent = BIO_meth_new(BIO_TYPE_LIBEVENT, "bufferevent");
if (methods_bufferevent == NULL)
return NULL;
BIO_meth_set_write(methods_bufferevent, bio_bufferevent_write);
BIO_meth_set_read(methods_bufferevent, bio_bufferevent_read);
BIO_meth_set_puts(methods_bufferevent, bio_bufferevent_puts);
BIO_meth_set_ctrl(methods_bufferevent, bio_bufferevent_ctrl);
BIO_meth_set_create(methods_bufferevent, bio_bufferevent_new);
BIO_meth_set_destroy(methods_bufferevent, bio_bufferevent_free);
}
return methods_bufferevent;
}



static BIO *
BIO_new_bufferevent(struct bufferevent *bufferevent, int close_flag)
{
BIO *result;
if (!bufferevent)
return NULL;
if (!(result = BIO_new(BIO_s_bufferevent())))
return NULL;
BIO_set_init(result, 1);
BIO_set_data(result, bufferevent);
BIO_set_shutdown(result, close_flag ? 1 : 0);
return result;
}














struct bio_data_counts {
unsigned long n_written;
unsigned long n_read;
};

struct bufferevent_openssl {





struct bufferevent_private bev;


struct bufferevent *underlying;

SSL *ssl;



struct evbuffer_cb_entry *outbuf_cb;



struct bio_data_counts counts;



ev_ssize_t last_write;

#define NUM_ERRORS 3
ev_uint32_t errors[NUM_ERRORS];




unsigned read_blocked_on_write : 1;

unsigned write_blocked_on_read : 1;

unsigned allow_dirty_shutdown : 1;

unsigned n_errors : 2;


unsigned state : 2;

unsigned old_state : 2;
};

static int be_openssl_enable(struct bufferevent *, short);
static int be_openssl_disable(struct bufferevent *, short);
static void be_openssl_unlink(struct bufferevent *);
static void be_openssl_destruct(struct bufferevent *);
static int be_openssl_adj_timeouts(struct bufferevent *);
static int be_openssl_flush(struct bufferevent *bufev,
short iotype, enum bufferevent_flush_mode mode);
static int be_openssl_ctrl(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);

const struct bufferevent_ops bufferevent_ops_openssl = {
"ssl",
evutil_offsetof(struct bufferevent_openssl, bev.bev),
be_openssl_enable,
be_openssl_disable,
be_openssl_unlink,
be_openssl_destruct,
be_openssl_adj_timeouts,
be_openssl_flush,
be_openssl_ctrl,
};



static inline struct bufferevent_openssl *
upcast(struct bufferevent *bev)
{
struct bufferevent_openssl *bev_o;
if (bev->be_ops != &bufferevent_ops_openssl)
return NULL;
bev_o = (void*)( ((char*)bev) -
evutil_offsetof(struct bufferevent_openssl, bev.bev));
EVUTIL_ASSERT(bev_o->bev.bev.be_ops == &bufferevent_ops_openssl);
return bev_o;
}

static inline void
put_error(struct bufferevent_openssl *bev_ssl, unsigned long err)
{
if (bev_ssl->n_errors == NUM_ERRORS)
return;





bev_ssl->errors[bev_ssl->n_errors++] = (ev_uint32_t) err;
}




static int
start_reading(struct bufferevent_openssl *bev_ssl)
{
if (bev_ssl->underlying) {
bufferevent_unsuspend_read_(bev_ssl->underlying,
BEV_SUSPEND_FILT_READ);
return 0;
} else {
struct bufferevent *bev = &bev_ssl->bev.bev;
int r;
r = bufferevent_add_event_(&bev->ev_read, &bev->timeout_read);
if (r == 0 && bev_ssl->read_blocked_on_write)
r = bufferevent_add_event_(&bev->ev_write,
&bev->timeout_write);
return r;
}
}




static int
start_writing(struct bufferevent_openssl *bev_ssl)
{
int r = 0;
if (bev_ssl->underlying) {
if (bev_ssl->write_blocked_on_read) {
bufferevent_unsuspend_read_(bev_ssl->underlying,
BEV_SUSPEND_FILT_READ);
}
} else {
struct bufferevent *bev = &bev_ssl->bev.bev;
r = bufferevent_add_event_(&bev->ev_write, &bev->timeout_write);
if (!r && bev_ssl->write_blocked_on_read)
r = bufferevent_add_event_(&bev->ev_read,
&bev->timeout_read);
}
return r;
}

static void
stop_reading(struct bufferevent_openssl *bev_ssl)
{
if (bev_ssl->write_blocked_on_read)
return;
if (bev_ssl->underlying) {
bufferevent_suspend_read_(bev_ssl->underlying,
BEV_SUSPEND_FILT_READ);
} else {
struct bufferevent *bev = &bev_ssl->bev.bev;
event_del(&bev->ev_read);
}
}

static void
stop_writing(struct bufferevent_openssl *bev_ssl)
{
if (bev_ssl->read_blocked_on_write)
return;
if (bev_ssl->underlying) {
bufferevent_unsuspend_read_(bev_ssl->underlying,
BEV_SUSPEND_FILT_READ);
} else {
struct bufferevent *bev = &bev_ssl->bev.bev;
event_del(&bev->ev_write);
}
}

static int
set_rbow(struct bufferevent_openssl *bev_ssl)
{
if (!bev_ssl->underlying)
stop_reading(bev_ssl);
bev_ssl->read_blocked_on_write = 1;
return start_writing(bev_ssl);
}

static int
set_wbor(struct bufferevent_openssl *bev_ssl)
{
if (!bev_ssl->underlying)
stop_writing(bev_ssl);
bev_ssl->write_blocked_on_read = 1;
return start_reading(bev_ssl);
}

static int
clear_rbow(struct bufferevent_openssl *bev_ssl)
{
struct bufferevent *bev = &bev_ssl->bev.bev;
int r = 0;
bev_ssl->read_blocked_on_write = 0;
if (!(bev->enabled & EV_WRITE))
stop_writing(bev_ssl);
if (bev->enabled & EV_READ)
r = start_reading(bev_ssl);
return r;
}


static int
clear_wbor(struct bufferevent_openssl *bev_ssl)
{
struct bufferevent *bev = &bev_ssl->bev.bev;
int r = 0;
bev_ssl->write_blocked_on_read = 0;
if (!(bev->enabled & EV_READ))
stop_reading(bev_ssl);
if (bev->enabled & EV_WRITE)
r = start_writing(bev_ssl);
return r;
}

static void
conn_closed(struct bufferevent_openssl *bev_ssl, int when, int errcode, int ret)
{
int event = BEV_EVENT_ERROR;
int dirty_shutdown = 0;
unsigned long err;

switch (errcode) {
case SSL_ERROR_ZERO_RETURN:

if (SSL_get_shutdown(bev_ssl->ssl) & SSL_RECEIVED_SHUTDOWN)
event = BEV_EVENT_EOF;
else
dirty_shutdown = 1;
break;
case SSL_ERROR_SYSCALL:

if ((ret == 0 || ret == -1) && ERR_peek_error() == 0)
dirty_shutdown = 1;
break;
case SSL_ERROR_SSL:

break;
case SSL_ERROR_WANT_X509_LOOKUP:

break;
case SSL_ERROR_NONE:
case SSL_ERROR_WANT_READ:
case SSL_ERROR_WANT_WRITE:
case SSL_ERROR_WANT_CONNECT:
case SSL_ERROR_WANT_ACCEPT:
default:

event_warnx("BUG: Unexpected OpenSSL error code %d", errcode);
break;
}

while ((err = ERR_get_error())) {
put_error(bev_ssl, err);
}

if (dirty_shutdown && bev_ssl->allow_dirty_shutdown)
event = BEV_EVENT_EOF;

stop_reading(bev_ssl);
stop_writing(bev_ssl);


event = when | event;
bufferevent_run_eventcb_(&bev_ssl->bev.bev, event, 0);
}

static void
init_bio_counts(struct bufferevent_openssl *bev_ssl)
{
BIO *rbio, *wbio;

wbio = SSL_get_wbio(bev_ssl->ssl);
bev_ssl->counts.n_written = wbio ? BIO_number_written(wbio) : 0;
rbio = SSL_get_rbio(bev_ssl->ssl);
bev_ssl->counts.n_read = rbio ? BIO_number_read(rbio) : 0;
}

static inline void
decrement_buckets(struct bufferevent_openssl *bev_ssl)
{
unsigned long num_w = BIO_number_written(SSL_get_wbio(bev_ssl->ssl));
unsigned long num_r = BIO_number_read(SSL_get_rbio(bev_ssl->ssl));

unsigned long w = num_w - bev_ssl->counts.n_written;
unsigned long r = num_r - bev_ssl->counts.n_read;
if (w)
bufferevent_decrement_write_buckets_(&bev_ssl->bev, w);
if (r)
bufferevent_decrement_read_buckets_(&bev_ssl->bev, r);
bev_ssl->counts.n_written = num_w;
bev_ssl->counts.n_read = num_r;
}

#define OP_MADE_PROGRESS 1
#define OP_BLOCKED 2
#define OP_ERR 4



static int
do_read(struct bufferevent_openssl *bev_ssl, int n_to_read) {

struct bufferevent *bev = &bev_ssl->bev.bev;
struct evbuffer *input = bev->input;
int r, n, i, n_used = 0, atmost;
struct evbuffer_iovec space[2];
int result = 0;

if (bev_ssl->bev.read_suspended)
return 0;

atmost = bufferevent_get_read_max_(&bev_ssl->bev);
if (n_to_read > atmost)
n_to_read = atmost;

n = evbuffer_reserve_space(input, n_to_read, space, 2);
if (n < 0)
return OP_ERR;

for (i=0; i<n; ++i) {
if (bev_ssl->bev.read_suspended)
break;
ERR_clear_error();
r = SSL_read(bev_ssl->ssl, space[i].iov_base, space[i].iov_len);
if (r>0) {
result |= OP_MADE_PROGRESS;
if (bev_ssl->read_blocked_on_write)
if (clear_rbow(bev_ssl) < 0)
return OP_ERR | result;
++n_used;
space[i].iov_len = r;
decrement_buckets(bev_ssl);
} else {
int err = SSL_get_error(bev_ssl->ssl, r);
print_err(err);
switch (err) {
case SSL_ERROR_WANT_READ:

if (bev_ssl->read_blocked_on_write)
if (clear_rbow(bev_ssl) < 0)
return OP_ERR | result;
break;
case SSL_ERROR_WANT_WRITE:


if (!bev_ssl->read_blocked_on_write)
if (set_rbow(bev_ssl) < 0)
return OP_ERR | result;
break;
default:
conn_closed(bev_ssl, BEV_EVENT_READING, err, r);
break;
}
result |= OP_BLOCKED;
break;
}
}

if (n_used) {
evbuffer_commit_space(input, space, n_used);
if (bev_ssl->underlying)
BEV_RESET_GENERIC_READ_TIMEOUT(bev);
}

return result;
}



static int
do_write(struct bufferevent_openssl *bev_ssl, int atmost)
{
int i, r, n, n_written = 0;
struct bufferevent *bev = &bev_ssl->bev.bev;
struct evbuffer *output = bev->output;
struct evbuffer_iovec space[8];
int result = 0;

if (bev_ssl->last_write > 0)
atmost = bev_ssl->last_write;
else
atmost = bufferevent_get_write_max_(&bev_ssl->bev);

n = evbuffer_peek(output, atmost, NULL, space, 8);
if (n < 0)
return OP_ERR | result;

if (n > 8)
n = 8;
for (i=0; i < n; ++i) {
if (bev_ssl->bev.write_suspended)
break;




if (space[i].iov_len == 0)
continue;

ERR_clear_error();
r = SSL_write(bev_ssl->ssl, space[i].iov_base,
space[i].iov_len);
if (r > 0) {
result |= OP_MADE_PROGRESS;
if (bev_ssl->write_blocked_on_read)
if (clear_wbor(bev_ssl) < 0)
return OP_ERR | result;
n_written += r;
bev_ssl->last_write = -1;
decrement_buckets(bev_ssl);
} else {
int err = SSL_get_error(bev_ssl->ssl, r);
print_err(err);
switch (err) {
case SSL_ERROR_WANT_WRITE:

if (bev_ssl->write_blocked_on_read)
if (clear_wbor(bev_ssl) < 0)
return OP_ERR | result;
bev_ssl->last_write = space[i].iov_len;
break;
case SSL_ERROR_WANT_READ:


if (!bev_ssl->write_blocked_on_read)
if (set_wbor(bev_ssl) < 0)
return OP_ERR | result;
bev_ssl->last_write = space[i].iov_len;
break;
default:
conn_closed(bev_ssl, BEV_EVENT_WRITING, err, r);
bev_ssl->last_write = -1;
break;
}
result |= OP_BLOCKED;
break;
}
}
if (n_written) {
evbuffer_drain(output, n_written);
if (bev_ssl->underlying)
BEV_RESET_GENERIC_WRITE_TIMEOUT(bev);

bufferevent_trigger_nolock_(bev, EV_WRITE, BEV_OPT_DEFER_CALLBACKS);
}
return result;
}

#define WRITE_FRAME 15000

#define READ_DEFAULT 4096



static int
bytes_to_read(struct bufferevent_openssl *bev)
{
struct evbuffer *input = bev->bev.bev.input;
struct event_watermark *wm = &bev->bev.bev.wm_read;
int result = READ_DEFAULT;
ev_ssize_t limit;



if (bev->write_blocked_on_read) {
return 0;
}

if (! (bev->bev.bev.enabled & EV_READ)) {
return 0;
}

if (bev->bev.read_suspended) {
return 0;
}

if (wm->high) {
if (evbuffer_get_length(input) >= wm->high) {
return 0;
}

result = wm->high - evbuffer_get_length(input);
} else {
result = READ_DEFAULT;
}


limit = bufferevent_get_read_max_(&bev->bev);
if (result > limit) {
result = limit;
}

return result;
}






static void
consider_reading(struct bufferevent_openssl *bev_ssl)
{
int r;
int n_to_read;
int all_result_flags = 0;

while (bev_ssl->write_blocked_on_read) {
r = do_write(bev_ssl, WRITE_FRAME);
if (r & (OP_BLOCKED|OP_ERR))
break;
}
if (bev_ssl->write_blocked_on_read)
return;

n_to_read = bytes_to_read(bev_ssl);

while (n_to_read) {
r = do_read(bev_ssl, n_to_read);
all_result_flags |= r;

if (r & (OP_BLOCKED|OP_ERR))
break;

if (bev_ssl->bev.read_suspended)
break;









n_to_read = SSL_pending(bev_ssl->ssl);















if (!n_to_read && bev_ssl->underlying)
n_to_read = bytes_to_read(bev_ssl);
}

if (all_result_flags & OP_MADE_PROGRESS) {
struct bufferevent *bev = &bev_ssl->bev.bev;

bufferevent_trigger_nolock_(bev, EV_READ, 0);
}

if (!bev_ssl->underlying) {

if (bev_ssl->bev.read_suspended ||
!(bev_ssl->bev.bev.enabled & EV_READ)) {
event_del(&bev_ssl->bev.bev.ev_read);
}
}
}

static void
consider_writing(struct bufferevent_openssl *bev_ssl)
{
int r;
struct evbuffer *output = bev_ssl->bev.bev.output;
struct evbuffer *target = NULL;
struct event_watermark *wm = NULL;

while (bev_ssl->read_blocked_on_write) {
r = do_read(bev_ssl, 1024);
if (r & OP_MADE_PROGRESS) {
struct bufferevent *bev = &bev_ssl->bev.bev;

bufferevent_trigger_nolock_(bev, EV_READ, 0);
}
if (r & (OP_ERR|OP_BLOCKED))
break;
}
if (bev_ssl->read_blocked_on_write)
return;
if (bev_ssl->underlying) {
target = bev_ssl->underlying->output;
wm = &bev_ssl->underlying->wm_write;
}
while ((bev_ssl->bev.bev.enabled & EV_WRITE) &&
(! bev_ssl->bev.write_suspended) &&
evbuffer_get_length(output) &&
(!target || (! wm->high || evbuffer_get_length(target) < wm->high))) {
int n_to_write;
if (wm && wm->high)
n_to_write = wm->high - evbuffer_get_length(target);
else
n_to_write = WRITE_FRAME;
r = do_write(bev_ssl, n_to_write);
if (r & (OP_BLOCKED|OP_ERR))
break;
}

if (!bev_ssl->underlying) {
if (evbuffer_get_length(output) == 0) {
event_del(&bev_ssl->bev.bev.ev_write);
} else if (bev_ssl->bev.write_suspended ||
!(bev_ssl->bev.bev.enabled & EV_WRITE)) {

event_del(&bev_ssl->bev.bev.ev_write);
}
}
}

static void
be_openssl_readcb(struct bufferevent *bev_base, void *ctx)
{
struct bufferevent_openssl *bev_ssl = ctx;
consider_reading(bev_ssl);
}

static void
be_openssl_writecb(struct bufferevent *bev_base, void *ctx)
{
struct bufferevent_openssl *bev_ssl = ctx;
consider_writing(bev_ssl);
}

static void
be_openssl_eventcb(struct bufferevent *bev_base, short what, void *ctx)
{
struct bufferevent_openssl *bev_ssl = ctx;
int event = 0;

if (what & BEV_EVENT_EOF) {
if (bev_ssl->allow_dirty_shutdown)
event = BEV_EVENT_EOF;
else
event = BEV_EVENT_ERROR;
} else if (what & BEV_EVENT_TIMEOUT) {

event = what;
} else if (what & BEV_EVENT_ERROR) {

event = what;
} else if (what & BEV_EVENT_CONNECTED) {


}
if (event)
bufferevent_run_eventcb_(&bev_ssl->bev.bev, event, 0);
}

static void
be_openssl_readeventcb(evutil_socket_t fd, short what, void *ptr)
{
struct bufferevent_openssl *bev_ssl = ptr;
bufferevent_incref_and_lock_(&bev_ssl->bev.bev);
if (what == EV_TIMEOUT) {
bufferevent_run_eventcb_(&bev_ssl->bev.bev,
BEV_EVENT_TIMEOUT|BEV_EVENT_READING, 0);
} else {
consider_reading(bev_ssl);
}
bufferevent_decref_and_unlock_(&bev_ssl->bev.bev);
}

static void
be_openssl_writeeventcb(evutil_socket_t fd, short what, void *ptr)
{
struct bufferevent_openssl *bev_ssl = ptr;
bufferevent_incref_and_lock_(&bev_ssl->bev.bev);
if (what == EV_TIMEOUT) {
bufferevent_run_eventcb_(&bev_ssl->bev.bev,
BEV_EVENT_TIMEOUT|BEV_EVENT_WRITING, 0);
} else {
consider_writing(bev_ssl);
}
bufferevent_decref_and_unlock_(&bev_ssl->bev.bev);
}

static int
be_openssl_auto_fd(struct bufferevent_openssl *bev_ssl, int fd)
{
if (!bev_ssl->underlying) {
struct bufferevent *bev = &bev_ssl->bev.bev;
if (event_initialized(&bev->ev_read) && fd < 0) {
fd = event_get_fd(&bev->ev_read);
}
}
return fd;
}

static int
set_open_callbacks(struct bufferevent_openssl *bev_ssl, evutil_socket_t fd)
{
if (bev_ssl->underlying) {
bufferevent_setcb(bev_ssl->underlying,
be_openssl_readcb, be_openssl_writecb, be_openssl_eventcb,
bev_ssl);
return 0;
} else {
struct bufferevent *bev = &bev_ssl->bev.bev;
int rpending=0, wpending=0, r1=0, r2=0;

if (event_initialized(&bev->ev_read)) {
rpending = event_pending(&bev->ev_read, EV_READ, NULL);
wpending = event_pending(&bev->ev_write, EV_WRITE, NULL);

event_del(&bev->ev_read);
event_del(&bev->ev_write);
}

event_assign(&bev->ev_read, bev->ev_base, fd,
EV_READ|EV_PERSIST|EV_FINALIZE,
be_openssl_readeventcb, bev_ssl);
event_assign(&bev->ev_write, bev->ev_base, fd,
EV_WRITE|EV_PERSIST|EV_FINALIZE,
be_openssl_writeeventcb, bev_ssl);

if (rpending)
r1 = bufferevent_add_event_(&bev->ev_read, &bev->timeout_read);
if (wpending)
r2 = bufferevent_add_event_(&bev->ev_write, &bev->timeout_write);

return (r1<0 || r2<0) ? -1 : 0;
}
}

static int
do_handshake(struct bufferevent_openssl *bev_ssl)
{
int r;

switch (bev_ssl->state) {
default:
case BUFFEREVENT_SSL_OPEN:
EVUTIL_ASSERT(0);
return -1;
case BUFFEREVENT_SSL_CONNECTING:
case BUFFEREVENT_SSL_ACCEPTING:
ERR_clear_error();
r = SSL_do_handshake(bev_ssl->ssl);
break;
}
decrement_buckets(bev_ssl);

if (r==1) {
int fd = event_get_fd(&bev_ssl->bev.bev.ev_read);

bev_ssl->state = BUFFEREVENT_SSL_OPEN;
set_open_callbacks(bev_ssl, fd);

bufferevent_enable(&bev_ssl->bev.bev, bev_ssl->bev.bev.enabled);
bufferevent_run_eventcb_(&bev_ssl->bev.bev,
BEV_EVENT_CONNECTED, 0);
return 1;
} else {
int err = SSL_get_error(bev_ssl->ssl, r);
print_err(err);
switch (err) {
case SSL_ERROR_WANT_WRITE:
stop_reading(bev_ssl);
return start_writing(bev_ssl);
case SSL_ERROR_WANT_READ:
stop_writing(bev_ssl);
return start_reading(bev_ssl);
default:
conn_closed(bev_ssl, BEV_EVENT_READING, err, r);
return -1;
}
}
}

static void
be_openssl_handshakecb(struct bufferevent *bev_base, void *ctx)
{
struct bufferevent_openssl *bev_ssl = ctx;
do_handshake(bev_ssl);
}

static void
be_openssl_handshakeeventcb(evutil_socket_t fd, short what, void *ptr)
{
struct bufferevent_openssl *bev_ssl = ptr;

bufferevent_incref_and_lock_(&bev_ssl->bev.bev);
if (what & EV_TIMEOUT) {
bufferevent_run_eventcb_(&bev_ssl->bev.bev, BEV_EVENT_TIMEOUT, 0);
} else
do_handshake(bev_ssl);
bufferevent_decref_and_unlock_(&bev_ssl->bev.bev);
}

static int
set_handshake_callbacks(struct bufferevent_openssl *bev_ssl, evutil_socket_t fd)
{
if (bev_ssl->underlying) {
bufferevent_setcb(bev_ssl->underlying,
be_openssl_handshakecb, be_openssl_handshakecb,
be_openssl_eventcb,
bev_ssl);

if (fd < 0)
return 0;

if (bufferevent_setfd(bev_ssl->underlying, fd))
return 1;

return do_handshake(bev_ssl);
} else {
struct bufferevent *bev = &bev_ssl->bev.bev;

if (event_initialized(&bev->ev_read)) {
event_del(&bev->ev_read);
event_del(&bev->ev_write);
}

event_assign(&bev->ev_read, bev->ev_base, fd,
EV_READ|EV_PERSIST|EV_FINALIZE,
be_openssl_handshakeeventcb, bev_ssl);
event_assign(&bev->ev_write, bev->ev_base, fd,
EV_WRITE|EV_PERSIST|EV_FINALIZE,
be_openssl_handshakeeventcb, bev_ssl);
if (fd >= 0)
bufferevent_enable(bev, bev->enabled);
return 0;
}
}

int
bufferevent_ssl_renegotiate(struct bufferevent *bev)
{
struct bufferevent_openssl *bev_ssl = upcast(bev);
if (!bev_ssl)
return -1;
if (SSL_renegotiate(bev_ssl->ssl) < 0)
return -1;
bev_ssl->state = BUFFEREVENT_SSL_CONNECTING;
if (set_handshake_callbacks(bev_ssl, be_openssl_auto_fd(bev_ssl, -1)) < 0)
return -1;
if (!bev_ssl->underlying)
return do_handshake(bev_ssl);
return 0;
}

static void
be_openssl_outbuf_cb(struct evbuffer *buf,
const struct evbuffer_cb_info *cbinfo, void *arg)
{
struct bufferevent_openssl *bev_ssl = arg;
int r = 0;


if (cbinfo->n_added && bev_ssl->state == BUFFEREVENT_SSL_OPEN) {
if (cbinfo->orig_size == 0)
r = bufferevent_add_event_(&bev_ssl->bev.bev.ev_write,
&bev_ssl->bev.bev.timeout_write);

if (bev_ssl->underlying)
consider_writing(bev_ssl);
}

(void)r;
}


static int
be_openssl_enable(struct bufferevent *bev, short events)
{
struct bufferevent_openssl *bev_ssl = upcast(bev);
int r1 = 0, r2 = 0;

if (events & EV_READ)
r1 = start_reading(bev_ssl);
if (events & EV_WRITE)
r2 = start_writing(bev_ssl);

if (bev_ssl->underlying) {
if (events & EV_READ)
BEV_RESET_GENERIC_READ_TIMEOUT(bev);
if (events & EV_WRITE)
BEV_RESET_GENERIC_WRITE_TIMEOUT(bev);

if (events & EV_READ)
consider_reading(bev_ssl);
if (events & EV_WRITE)
consider_writing(bev_ssl);
}
return (r1 < 0 || r2 < 0) ? -1 : 0;
}

static int
be_openssl_disable(struct bufferevent *bev, short events)
{
struct bufferevent_openssl *bev_ssl = upcast(bev);

if (events & EV_READ)
stop_reading(bev_ssl);
if (events & EV_WRITE)
stop_writing(bev_ssl);

if (bev_ssl->underlying) {
if (events & EV_READ)
BEV_DEL_GENERIC_READ_TIMEOUT(bev);
if (events & EV_WRITE)
BEV_DEL_GENERIC_WRITE_TIMEOUT(bev);
}
return 0;
}

static void
be_openssl_unlink(struct bufferevent *bev)
{
struct bufferevent_openssl *bev_ssl = upcast(bev);

if (bev_ssl->bev.options & BEV_OPT_CLOSE_ON_FREE) {
if (bev_ssl->underlying) {
if (BEV_UPCAST(bev_ssl->underlying)->refcnt < 2) {
event_warnx("BEV_OPT_CLOSE_ON_FREE set on an "
"bufferevent with too few references");
} else {
bufferevent_free(bev_ssl->underlying);



}
}
} else {
if (bev_ssl->underlying) {
if (bev_ssl->underlying->errorcb == be_openssl_eventcb)
bufferevent_setcb(bev_ssl->underlying,
NULL,NULL,NULL,NULL);
bufferevent_unsuspend_read_(bev_ssl->underlying,
BEV_SUSPEND_FILT_READ);
}
}
}

static void
be_openssl_destruct(struct bufferevent *bev)
{
struct bufferevent_openssl *bev_ssl = upcast(bev);

if (bev_ssl->bev.options & BEV_OPT_CLOSE_ON_FREE) {
if (! bev_ssl->underlying) {
evutil_socket_t fd = -1;
BIO *bio = SSL_get_wbio(bev_ssl->ssl);
if (bio)
fd = BIO_get_fd(bio, NULL);
if (fd >= 0)
evutil_closesocket(fd);
}
SSL_free(bev_ssl->ssl);
}
}

static int
be_openssl_adj_timeouts(struct bufferevent *bev)
{
struct bufferevent_openssl *bev_ssl = upcast(bev);

if (bev_ssl->underlying) {
return bufferevent_generic_adj_timeouts_(bev);
} else {
return bufferevent_generic_adj_existing_timeouts_(bev);
}
}

static int
be_openssl_flush(struct bufferevent *bufev,
short iotype, enum bufferevent_flush_mode mode)
{

return 0;
}

static int
be_openssl_set_fd(struct bufferevent_openssl *bev_ssl,
enum bufferevent_ssl_state state, int fd)
{
bev_ssl->state = state;

switch (state) {
case BUFFEREVENT_SSL_ACCEPTING:
SSL_set_accept_state(bev_ssl->ssl);
if (set_handshake_callbacks(bev_ssl, fd) < 0)
return -1;
break;
case BUFFEREVENT_SSL_CONNECTING:
SSL_set_connect_state(bev_ssl->ssl);
if (set_handshake_callbacks(bev_ssl, fd) < 0)
return -1;
break;
case BUFFEREVENT_SSL_OPEN:
if (set_open_callbacks(bev_ssl, fd) < 0)
return -1;
break;
default:
return -1;
}

return 0;
}

static int
be_openssl_ctrl(struct bufferevent *bev,
enum bufferevent_ctrl_op op, union bufferevent_ctrl_data *data)
{
struct bufferevent_openssl *bev_ssl = upcast(bev);
switch (op) {
case BEV_CTRL_SET_FD:
if (!bev_ssl->underlying) {
BIO *bio;
bio = BIO_new_socket(data->fd, 0);
SSL_set_bio(bev_ssl->ssl, bio, bio);
} else {
BIO *bio;
if (!(bio = BIO_new_bufferevent(bev_ssl->underlying, 0)))
return -1;
SSL_set_bio(bev_ssl->ssl, bio, bio);
}

return be_openssl_set_fd(bev_ssl, bev_ssl->old_state, data->fd);
case BEV_CTRL_GET_FD:
if (bev_ssl->underlying) {
data->fd = event_get_fd(&bev_ssl->underlying->ev_read);
} else {
data->fd = event_get_fd(&bev->ev_read);
}
return 0;
case BEV_CTRL_GET_UNDERLYING:
data->ptr = bev_ssl->underlying;
return 0;
case BEV_CTRL_CANCEL_ALL:
default:
return -1;
}
}

SSL *
bufferevent_openssl_get_ssl(struct bufferevent *bufev)
{
struct bufferevent_openssl *bev_ssl = upcast(bufev);
if (!bev_ssl)
return NULL;
return bev_ssl->ssl;
}

static struct bufferevent *
bufferevent_openssl_new_impl(struct event_base *base,
struct bufferevent *underlying,
evutil_socket_t fd,
SSL *ssl,
enum bufferevent_ssl_state state,
int options)
{
struct bufferevent_openssl *bev_ssl = NULL;
struct bufferevent_private *bev_p = NULL;
int tmp_options = options & ~BEV_OPT_THREADSAFE;

if (underlying != NULL && fd >= 0)
return NULL;

if (!(bev_ssl = mm_calloc(1, sizeof(struct bufferevent_openssl))))
goto err;

bev_p = &bev_ssl->bev;

if (bufferevent_init_common_(bev_p, base,
&bufferevent_ops_openssl, tmp_options) < 0)
goto err;



SSL_set_mode(ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

bev_ssl->underlying = underlying;
bev_ssl->ssl = ssl;

bev_ssl->outbuf_cb = evbuffer_add_cb(bev_p->bev.output,
be_openssl_outbuf_cb, bev_ssl);

if (options & BEV_OPT_THREADSAFE)
bufferevent_enable_locking_(&bev_ssl->bev.bev, NULL);

if (underlying) {
bufferevent_init_generic_timeout_cbs_(&bev_ssl->bev.bev);
bufferevent_incref_(underlying);
}

bev_ssl->old_state = state;
bev_ssl->last_write = -1;

init_bio_counts(bev_ssl);

fd = be_openssl_auto_fd(bev_ssl, fd);
if (be_openssl_set_fd(bev_ssl, state, fd))
goto err;

if (underlying) {
bufferevent_setwatermark(underlying, EV_READ, 0, 0);
bufferevent_enable(underlying, EV_READ|EV_WRITE);
if (state == BUFFEREVENT_SSL_OPEN)
bufferevent_suspend_read_(underlying,
BEV_SUSPEND_FILT_READ);
}

return &bev_ssl->bev.bev;
err:
if (bev_ssl)
bufferevent_free(&bev_ssl->bev.bev);
return NULL;
}

struct bufferevent *
bufferevent_openssl_filter_new(struct event_base *base,
struct bufferevent *underlying,
SSL *ssl,
enum bufferevent_ssl_state state,
int options)
{


int close_flag = 0;
BIO *bio;
if (!underlying)
return NULL;
if (!(bio = BIO_new_bufferevent(underlying, close_flag)))
return NULL;

SSL_set_bio(ssl, bio, bio);

return bufferevent_openssl_new_impl(
base, underlying, -1, ssl, state, options);
}

struct bufferevent *
bufferevent_openssl_socket_new(struct event_base *base,
evutil_socket_t fd,
SSL *ssl,
enum bufferevent_ssl_state state,
int options)
{

BIO *bio = SSL_get_wbio(ssl);
long have_fd = -1;

if (bio)
have_fd = BIO_get_fd(bio, NULL);

if (have_fd >= 0) {

if (fd < 0) {

fd = (evutil_socket_t) have_fd;
} else if (have_fd == (long)fd) {

} else {


return NULL;
}
(void) BIO_set_close(bio, 0);
} else {

if (fd >= 0) {

bio = BIO_new_socket(fd, 0);
SSL_set_bio(ssl, bio, bio);
} else {

}
}

return bufferevent_openssl_new_impl(
base, NULL, fd, ssl, state, options);
}

int
bufferevent_openssl_get_allow_dirty_shutdown(struct bufferevent *bev)
{
int allow_dirty_shutdown = -1;
struct bufferevent_openssl *bev_ssl;
BEV_LOCK(bev);
bev_ssl = upcast(bev);
if (bev_ssl)
allow_dirty_shutdown = bev_ssl->allow_dirty_shutdown;
BEV_UNLOCK(bev);
return allow_dirty_shutdown;
}

void
bufferevent_openssl_set_allow_dirty_shutdown(struct bufferevent *bev,
int allow_dirty_shutdown)
{
struct bufferevent_openssl *bev_ssl;
BEV_LOCK(bev);
bev_ssl = upcast(bev);
if (bev_ssl)
bev_ssl->allow_dirty_shutdown = !!allow_dirty_shutdown;
BEV_UNLOCK(bev);
}

unsigned long
bufferevent_get_openssl_error(struct bufferevent *bev)
{
unsigned long err = 0;
struct bufferevent_openssl *bev_ssl;
BEV_LOCK(bev);
bev_ssl = upcast(bev);
if (bev_ssl && bev_ssl->n_errors) {
err = bev_ssl->errors[--bev_ssl->n_errors];
}
BEV_UNLOCK(bev);
return err;
}
