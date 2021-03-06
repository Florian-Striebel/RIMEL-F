
























#if !defined(BUFFEREVENT_INTERNAL_H_INCLUDED_)
#define BUFFEREVENT_INTERNAL_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

#include "event2/event-config.h"
#include "event2/event_struct.h"
#include "evconfig-private.h"
#include "event2/util.h"
#include "defer-internal.h"
#include "evthread-internal.h"
#include "event2/thread.h"
#include "ratelim-internal.h"
#include "event2/bufferevent_struct.h"

#include "ipv6-internal.h"
#if defined(_WIN32)
#include <ws2tcpip.h>
#endif
#if defined(EVENT__HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif
#if defined(EVENT__HAVE_NETINET_IN6_H)
#include <netinet/in6.h>
#endif












#define BEV_SUSPEND_WM 0x01

#define BEV_SUSPEND_BW 0x02

#define BEV_SUSPEND_BW_GROUP 0x04


#define BEV_SUSPEND_LOOKUP 0x08


#define BEV_SUSPEND_FILT_READ 0x10

typedef ev_uint16_t bufferevent_suspend_flags;

struct bufferevent_rate_limit_group {

LIST_HEAD(rlim_group_member_list, bufferevent_private) members;

struct ev_token_bucket rate_limit;
struct ev_token_bucket_cfg rate_limit_cfg;



unsigned read_suspended : 1;


unsigned write_suspended : 1;



unsigned pending_unsuspend_read : 1;



unsigned pending_unsuspend_write : 1;




ev_uint64_t total_read;
ev_uint64_t total_written;



int n_members;



ev_ssize_t min_share;
ev_ssize_t configured_min_share;



struct event master_refill_event;


struct evutil_weakrand_state weakrand_seed;




void *lock;
};


struct bufferevent_rate_limit {





LIST_ENTRY(bufferevent_private) next_in_group;


struct bufferevent_rate_limit_group *group;


struct ev_token_bucket limit;


struct ev_token_bucket_cfg *cfg;



struct event refill_bucket_event;
};



struct bufferevent_private {

struct bufferevent bev;


struct evbuffer_cb_entry *read_watermarks_cb;


unsigned own_lock : 1;



unsigned readcb_pending : 1;


unsigned writecb_pending : 1;

unsigned connecting : 1;


unsigned connection_refused : 1;


short eventcb_pending;




bufferevent_suspend_flags read_suspended;




bufferevent_suspend_flags write_suspended;



int errno_pending;


int dns_error;


struct event_callback deferred;


enum bufferevent_options options;


int refcnt;



void *lock;



ev_ssize_t max_single_read;



ev_ssize_t max_single_write;


struct bufferevent_rate_limit *rate_limiting;








union {
struct sockaddr_in6 in6;
struct sockaddr_in in;
} conn_address;

struct evdns_getaddrinfo_request *dns_request;
};


enum bufferevent_ctrl_op {
BEV_CTRL_SET_FD,
BEV_CTRL_GET_FD,
BEV_CTRL_GET_UNDERLYING,
BEV_CTRL_CANCEL_ALL
};


union bufferevent_ctrl_data {
void *ptr;
evutil_socket_t fd;
};





struct bufferevent_ops {

const char *type;










off_t mem_offset;





int (*enable)(struct bufferevent *, short);





int (*disable)(struct bufferevent *, short);



void (*unlink)(struct bufferevent *);





void (*destruct)(struct bufferevent *);


int (*adj_timeouts)(struct bufferevent *);


int (*flush)(struct bufferevent *, short, enum bufferevent_flush_mode);


int (*ctrl)(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);

};

extern const struct bufferevent_ops bufferevent_ops_socket;
extern const struct bufferevent_ops bufferevent_ops_filter;
extern const struct bufferevent_ops bufferevent_ops_pair;

#define BEV_IS_SOCKET(bevp) ((bevp)->be_ops == &bufferevent_ops_socket)
#define BEV_IS_FILTER(bevp) ((bevp)->be_ops == &bufferevent_ops_filter)
#define BEV_IS_PAIR(bevp) ((bevp)->be_ops == &bufferevent_ops_pair)

#if defined(_WIN32)
extern const struct bufferevent_ops bufferevent_ops_async;
#define BEV_IS_ASYNC(bevp) ((bevp)->be_ops == &bufferevent_ops_async)
#else
#define BEV_IS_ASYNC(bevp) 0
#endif


int bufferevent_init_common_(struct bufferevent_private *, struct event_base *, const struct bufferevent_ops *, enum bufferevent_options options);



void bufferevent_suspend_read_(struct bufferevent *bufev, bufferevent_suspend_flags what);


void bufferevent_unsuspend_read_(struct bufferevent *bufev, bufferevent_suspend_flags what);



void bufferevent_suspend_write_(struct bufferevent *bufev, bufferevent_suspend_flags what);


void bufferevent_unsuspend_write_(struct bufferevent *bufev, bufferevent_suspend_flags what);

#define bufferevent_wm_suspend_read(b) bufferevent_suspend_read_((b), BEV_SUSPEND_WM)

#define bufferevent_wm_unsuspend_read(b) bufferevent_unsuspend_read_((b), BEV_SUSPEND_WM)














int bufferevent_disable_hard_(struct bufferevent *bufev, short event);



int bufferevent_enable_locking_(struct bufferevent *bufev, void *lock);


#define bufferevent_incref_(bufev) bufferevent_incref(bufev)


void bufferevent_incref_and_lock_(struct bufferevent *bufev);



#define bufferevent_decref_(bufev) bufferevent_decref(bufev)



int bufferevent_decref_and_unlock_(struct bufferevent *bufev);



void bufferevent_run_readcb_(struct bufferevent *bufev, int options);


void bufferevent_run_writecb_(struct bufferevent *bufev, int options);



void bufferevent_run_eventcb_(struct bufferevent *bufev, short what, int options);





static inline void bufferevent_trigger_nolock_(struct bufferevent *bufev, short iotype, int options);



static inline void
bufferevent_trigger_nolock_(struct bufferevent *bufev, short iotype, int options)
{
if ((iotype & EV_READ) && ((options & BEV_TRIG_IGNORE_WATERMARKS) ||
evbuffer_get_length(bufev->input) >= bufev->wm_read.low))
bufferevent_run_readcb_(bufev, options);
if ((iotype & EV_WRITE) && ((options & BEV_TRIG_IGNORE_WATERMARKS) ||
evbuffer_get_length(bufev->output) <= bufev->wm_write.low))
bufferevent_run_writecb_(bufev, options);
}



int bufferevent_add_event_(struct event *ev, const struct timeval *tv);








void bufferevent_init_generic_timeout_cbs_(struct bufferevent *bev);




int bufferevent_generic_adj_timeouts_(struct bufferevent *bev);
int bufferevent_generic_adj_existing_timeouts_(struct bufferevent *bev);

enum bufferevent_options bufferevent_get_options_(struct bufferevent *bev);

const struct sockaddr*
bufferevent_socket_get_conn_address_(struct bufferevent *bev);



#define BEV_RESET_GENERIC_READ_TIMEOUT(bev) do { if (evutil_timerisset(&(bev)->timeout_read)) event_add(&(bev)->ev_read, &(bev)->timeout_read); } while (0)






#define BEV_RESET_GENERIC_WRITE_TIMEOUT(bev) do { if (evutil_timerisset(&(bev)->timeout_write)) event_add(&(bev)->ev_write, &(bev)->timeout_write); } while (0)




#define BEV_DEL_GENERIC_READ_TIMEOUT(bev) event_del(&(bev)->ev_read)

#define BEV_DEL_GENERIC_WRITE_TIMEOUT(bev) event_del(&(bev)->ev_write)





#define BEV_UPCAST(b) EVUTIL_UPCAST((b), struct bufferevent_private, bev)

#if defined(EVENT__DISABLE_THREAD_SUPPORT)
#define BEV_LOCK(b) EVUTIL_NIL_STMT_
#define BEV_UNLOCK(b) EVUTIL_NIL_STMT_
#else

#define BEV_LOCK(b) do { struct bufferevent_private *locking = BEV_UPCAST(b); EVLOCK_LOCK(locking->lock, 0); } while (0)





#define BEV_UNLOCK(b) do { struct bufferevent_private *locking = BEV_UPCAST(b); EVLOCK_UNLOCK(locking->lock, 0); } while (0)



#endif




int bufferevent_decrement_write_buckets_(struct bufferevent_private *bev,
ev_ssize_t bytes);
int bufferevent_decrement_read_buckets_(struct bufferevent_private *bev,
ev_ssize_t bytes);
ev_ssize_t bufferevent_get_read_max_(struct bufferevent_private *bev);
ev_ssize_t bufferevent_get_write_max_(struct bufferevent_private *bev);

int bufferevent_ratelim_init_(struct bufferevent_private *bev);

#if defined(__cplusplus)
}
#endif


#endif
