



























#include "evconfig-private.h"

#include <sys/types.h>

#include "event2/event-config.h"

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

#if defined(_WIN32)
#include <winsock2.h>
#endif

#include "event2/util.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/bufferevent_struct.h"
#include "event2/event.h"
#include "log-internal.h"
#include "mm-internal.h"
#include "bufferevent-internal.h"
#include "util-internal.h"


static int be_filter_enable(struct bufferevent *, short);
static int be_filter_disable(struct bufferevent *, short);
static void be_filter_unlink(struct bufferevent *);
static void be_filter_destruct(struct bufferevent *);

static void be_filter_readcb(struct bufferevent *, void *);
static void be_filter_writecb(struct bufferevent *, void *);
static void be_filter_eventcb(struct bufferevent *, short, void *);
static int be_filter_flush(struct bufferevent *bufev,
short iotype, enum bufferevent_flush_mode mode);
static int be_filter_ctrl(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);

static void bufferevent_filtered_inbuf_cb(struct evbuffer *buf,
const struct evbuffer_cb_info *cbinfo, void *arg);

static void bufferevent_filtered_outbuf_cb(struct evbuffer *buf,
const struct evbuffer_cb_info *info, void *arg);

struct bufferevent_filtered {
struct bufferevent_private bev;


struct bufferevent *underlying;

struct evbuffer_cb_entry *inbuf_cb;

struct evbuffer_cb_entry *outbuf_cb;


unsigned got_eof;


void (*free_context)(void *);

bufferevent_filter_cb process_in;

bufferevent_filter_cb process_out;

void *context;
};

const struct bufferevent_ops bufferevent_ops_filter = {
"filter",
evutil_offsetof(struct bufferevent_filtered, bev.bev),
be_filter_enable,
be_filter_disable,
be_filter_unlink,
be_filter_destruct,
bufferevent_generic_adj_timeouts_,
be_filter_flush,
be_filter_ctrl,
};



static inline struct bufferevent_filtered *
upcast(struct bufferevent *bev)
{
struct bufferevent_filtered *bev_f;
if (bev->be_ops != &bufferevent_ops_filter)
return NULL;
bev_f = (void*)( ((char*)bev) -
evutil_offsetof(struct bufferevent_filtered, bev.bev));
EVUTIL_ASSERT(bev_f->bev.bev.be_ops == &bufferevent_ops_filter);
return bev_f;
}

#define downcast(bev_f) (&(bev_f)->bev.bev)




static int
be_underlying_writebuf_full(struct bufferevent_filtered *bevf,
enum bufferevent_flush_mode state)
{
struct bufferevent *u = bevf->underlying;
return state == BEV_NORMAL &&
u->wm_write.high &&
evbuffer_get_length(u->output) >= u->wm_write.high;
}



static int
be_readbuf_full(struct bufferevent_filtered *bevf,
enum bufferevent_flush_mode state)
{
struct bufferevent *bufev = downcast(bevf);
return state == BEV_NORMAL &&
bufev->wm_read.high &&
evbuffer_get_length(bufev->input) >= bufev->wm_read.high;
}



static enum bufferevent_filter_result
be_null_filter(struct evbuffer *src, struct evbuffer *dst, ev_ssize_t lim,
enum bufferevent_flush_mode state, void *ctx)
{
(void)state;
if (evbuffer_remove_buffer(src, dst, lim) == 0)
return BEV_OK;
else
return BEV_ERROR;
}

struct bufferevent *
bufferevent_filter_new(struct bufferevent *underlying,
bufferevent_filter_cb input_filter,
bufferevent_filter_cb output_filter,
int options,
void (*free_context)(void *),
void *ctx)
{
struct bufferevent_filtered *bufev_f;
int tmp_options = options & ~BEV_OPT_THREADSAFE;

if (!underlying)
return NULL;

if (!input_filter)
input_filter = be_null_filter;
if (!output_filter)
output_filter = be_null_filter;

bufev_f = mm_calloc(1, sizeof(struct bufferevent_filtered));
if (!bufev_f)
return NULL;

if (bufferevent_init_common_(&bufev_f->bev, underlying->ev_base,
&bufferevent_ops_filter, tmp_options) < 0) {
mm_free(bufev_f);
return NULL;
}
if (options & BEV_OPT_THREADSAFE) {
bufferevent_enable_locking_(downcast(bufev_f), NULL);
}

bufev_f->underlying = underlying;

bufev_f->process_in = input_filter;
bufev_f->process_out = output_filter;
bufev_f->free_context = free_context;
bufev_f->context = ctx;

bufferevent_setcb(bufev_f->underlying,
be_filter_readcb, be_filter_writecb, be_filter_eventcb, bufev_f);

bufev_f->inbuf_cb = evbuffer_add_cb(downcast(bufev_f)->input,
bufferevent_filtered_inbuf_cb, bufev_f);
evbuffer_cb_clear_flags(downcast(bufev_f)->input, bufev_f->inbuf_cb,
EVBUFFER_CB_ENABLED);

bufev_f->outbuf_cb = evbuffer_add_cb(downcast(bufev_f)->output,
bufferevent_filtered_outbuf_cb, bufev_f);

bufferevent_init_generic_timeout_cbs_(downcast(bufev_f));
bufferevent_incref_(underlying);

bufferevent_enable(underlying, EV_READ|EV_WRITE);
bufferevent_suspend_read_(underlying, BEV_SUSPEND_FILT_READ);

return downcast(bufev_f);
}

static void
be_filter_unlink(struct bufferevent *bev)
{
struct bufferevent_filtered *bevf = upcast(bev);
EVUTIL_ASSERT(bevf);

if (bevf->bev.options & BEV_OPT_CLOSE_ON_FREE) {





if (BEV_UPCAST(bevf->underlying)->refcnt < 2) {
event_warnx("BEV_OPT_CLOSE_ON_FREE set on an "
"bufferevent with too few references");
} else {
bufferevent_free(bevf->underlying);
}
} else {
if (bevf->underlying) {
if (bevf->underlying->errorcb == be_filter_eventcb)
bufferevent_setcb(bevf->underlying,
NULL, NULL, NULL, NULL);
bufferevent_unsuspend_read_(bevf->underlying,
BEV_SUSPEND_FILT_READ);
}
}
}

static void
be_filter_destruct(struct bufferevent *bev)
{
struct bufferevent_filtered *bevf = upcast(bev);
EVUTIL_ASSERT(bevf);
if (bevf->free_context)
bevf->free_context(bevf->context);

if (bevf->inbuf_cb)
evbuffer_remove_cb_entry(bev->input, bevf->inbuf_cb);

if (bevf->outbuf_cb)
evbuffer_remove_cb_entry(bev->output, bevf->outbuf_cb);
}

static int
be_filter_enable(struct bufferevent *bev, short event)
{
struct bufferevent_filtered *bevf = upcast(bev);
if (event & EV_WRITE)
BEV_RESET_GENERIC_WRITE_TIMEOUT(bev);

if (event & EV_READ) {
BEV_RESET_GENERIC_READ_TIMEOUT(bev);
bufferevent_unsuspend_read_(bevf->underlying,
BEV_SUSPEND_FILT_READ);
}
return 0;
}

static int
be_filter_disable(struct bufferevent *bev, short event)
{
struct bufferevent_filtered *bevf = upcast(bev);
if (event & EV_WRITE)
BEV_DEL_GENERIC_WRITE_TIMEOUT(bev);
if (event & EV_READ) {
BEV_DEL_GENERIC_READ_TIMEOUT(bev);
bufferevent_suspend_read_(bevf->underlying,
BEV_SUSPEND_FILT_READ);
}
return 0;
}

static enum bufferevent_filter_result
be_filter_process_input(struct bufferevent_filtered *bevf,
enum bufferevent_flush_mode state,
int *processed_out)
{
enum bufferevent_filter_result res;
struct bufferevent *bev = downcast(bevf);

if (state == BEV_NORMAL) {


if (!(bev->enabled & EV_READ) ||
be_readbuf_full(bevf, state))
return BEV_OK;
}

do {
ev_ssize_t limit = -1;
if (state == BEV_NORMAL && bev->wm_read.high)
limit = bev->wm_read.high -
evbuffer_get_length(bev->input);

res = bevf->process_in(bevf->underlying->input,
bev->input, limit, state, bevf->context);

if (res == BEV_OK)
*processed_out = 1;
} while (res == BEV_OK &&
(bev->enabled & EV_READ) &&
evbuffer_get_length(bevf->underlying->input) &&
!be_readbuf_full(bevf, state));

if (*processed_out)
BEV_RESET_GENERIC_READ_TIMEOUT(bev);

return res;
}


static enum bufferevent_filter_result
be_filter_process_output(struct bufferevent_filtered *bevf,
enum bufferevent_flush_mode state,
int *processed_out)
{

enum bufferevent_filter_result res = BEV_OK;
struct bufferevent *bufev = downcast(bevf);
int again = 0;

if (state == BEV_NORMAL) {





if (!(bufev->enabled & EV_WRITE) ||
be_underlying_writebuf_full(bevf, state) ||
!evbuffer_get_length(bufev->output))
return BEV_OK;
}



evbuffer_cb_clear_flags(bufev->output, bevf->outbuf_cb,
EVBUFFER_CB_ENABLED);

do {
int processed = 0;
again = 0;

do {
ev_ssize_t limit = -1;
if (state == BEV_NORMAL &&
bevf->underlying->wm_write.high)
limit = bevf->underlying->wm_write.high -
evbuffer_get_length(bevf->underlying->output);

res = bevf->process_out(downcast(bevf)->output,
bevf->underlying->output,
limit,
state,
bevf->context);

if (res == BEV_OK)
processed = *processed_out = 1;
} while (
res == BEV_OK &&

(bufev->enabled & EV_WRITE) &&


evbuffer_get_length(bufev->output) &&

!be_underlying_writebuf_full(bevf,state));

if (processed) {

bufferevent_trigger_nolock_(bufev, EV_WRITE, 0);

if (res == BEV_OK &&
(bufev->enabled & EV_WRITE) &&
evbuffer_get_length(bufev->output) &&
!be_underlying_writebuf_full(bevf, state)) {
again = 1;
}
}
} while (again);


evbuffer_cb_set_flags(bufev->output,bevf->outbuf_cb,
EVBUFFER_CB_ENABLED);

if (*processed_out)
BEV_RESET_GENERIC_WRITE_TIMEOUT(bufev);

return res;
}


static void
bufferevent_filtered_outbuf_cb(struct evbuffer *buf,
const struct evbuffer_cb_info *cbinfo, void *arg)
{
struct bufferevent_filtered *bevf = arg;
struct bufferevent *bev = downcast(bevf);

if (cbinfo->n_added) {
int processed_any = 0;


bufferevent_incref_and_lock_(bev);
be_filter_process_output(bevf, BEV_NORMAL, &processed_any);
bufferevent_decref_and_unlock_(bev);
}
}

static void
be_filter_read_nolock_(struct bufferevent *underlying, void *me_)
{
struct bufferevent_filtered *bevf = me_;
enum bufferevent_filter_result res;
enum bufferevent_flush_mode state;
struct bufferevent *bufev = downcast(bevf);
struct bufferevent_private *bufev_private = BEV_UPCAST(bufev);
int processed_any = 0;


EVUTIL_ASSERT(bufev_private->refcnt >= 0);


if (bufev_private->refcnt > 0) {

if (bevf->got_eof)
state = BEV_FINISHED;
else
state = BEV_NORMAL;


res = be_filter_process_input(bevf, state, &processed_any);
(void)res;




if (processed_any) {
bufferevent_trigger_nolock_(bufev, EV_READ, 0);
if (evbuffer_get_length(underlying->input) > 0 &&
be_readbuf_full(bevf, state)) {





evbuffer_cb_set_flags(bufev->input, bevf->inbuf_cb,
EVBUFFER_CB_ENABLED);
}
}
}
}


static void
bufferevent_filtered_inbuf_cb(struct evbuffer *buf,
const struct evbuffer_cb_info *cbinfo, void *arg)
{
struct bufferevent_filtered *bevf = arg;
enum bufferevent_flush_mode state;
struct bufferevent *bev = downcast(bevf);

BEV_LOCK(bev);

if (bevf->got_eof)
state = BEV_FINISHED;
else
state = BEV_NORMAL;


if (!be_readbuf_full(bevf, state)) {




evbuffer_cb_clear_flags(bev->input, bevf->inbuf_cb,
EVBUFFER_CB_ENABLED);
if (evbuffer_get_length(bevf->underlying->input) > 0)
be_filter_read_nolock_(bevf->underlying, bevf);
}

BEV_UNLOCK(bev);
}


static void
be_filter_readcb(struct bufferevent *underlying, void *me_)
{
struct bufferevent_filtered *bevf = me_;
struct bufferevent *bev = downcast(bevf);

BEV_LOCK(bev);

be_filter_read_nolock_(underlying, me_);

BEV_UNLOCK(bev);
}



static void
be_filter_writecb(struct bufferevent *underlying, void *me_)
{
struct bufferevent_filtered *bevf = me_;
struct bufferevent *bev = downcast(bevf);
struct bufferevent_private *bufev_private = BEV_UPCAST(bev);
int processed_any = 0;

BEV_LOCK(bev);


EVUTIL_ASSERT(bufev_private->refcnt >= 0);


if (bufev_private->refcnt > 0) {
be_filter_process_output(bevf, BEV_NORMAL, &processed_any);
}

BEV_UNLOCK(bev);
}


static void
be_filter_eventcb(struct bufferevent *underlying, short what, void *me_)
{
struct bufferevent_filtered *bevf = me_;
struct bufferevent *bev = downcast(bevf);
struct bufferevent_private *bufev_private = BEV_UPCAST(bev);

BEV_LOCK(bev);


EVUTIL_ASSERT(bufev_private->refcnt >= 0);


if (bufev_private->refcnt > 0) {


bufferevent_run_eventcb_(bev, what, 0);
}

BEV_UNLOCK(bev);
}

static int
be_filter_flush(struct bufferevent *bufev,
short iotype, enum bufferevent_flush_mode mode)
{
struct bufferevent_filtered *bevf = upcast(bufev);
int processed_any = 0;
EVUTIL_ASSERT(bevf);

bufferevent_incref_and_lock_(bufev);

if (iotype & EV_READ) {
be_filter_process_input(bevf, mode, &processed_any);
}
if (iotype & EV_WRITE) {
be_filter_process_output(bevf, mode, &processed_any);
}


bufferevent_flush(bevf->underlying, iotype, mode);

bufferevent_decref_and_unlock_(bufev);

return processed_any;
}

static int
be_filter_ctrl(struct bufferevent *bev, enum bufferevent_ctrl_op op,
union bufferevent_ctrl_data *data)
{
struct bufferevent_filtered *bevf;
switch (op) {
case BEV_CTRL_GET_UNDERLYING:
bevf = upcast(bev);
data->ptr = bevf->underlying;
return 0;
case BEV_CTRL_SET_FD:
bevf = upcast(bev);

if (bevf->underlying &&
bevf->underlying->be_ops &&
bevf->underlying->be_ops->ctrl) {
return (bevf->underlying->be_ops->ctrl)(bevf->underlying, op, data);
}

case BEV_CTRL_GET_FD:
case BEV_CTRL_CANCEL_ALL:
default:
return -1;
}

return -1;
}
