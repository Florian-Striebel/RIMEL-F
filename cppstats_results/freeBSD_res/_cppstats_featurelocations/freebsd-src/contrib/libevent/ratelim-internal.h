
























#if !defined(RATELIM_INTERNAL_H_INCLUDED_)
#define RATELIM_INTERNAL_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

#include "event2/util.h"




struct ev_token_bucket {


ev_ssize_t read_limit, write_limit;


ev_uint32_t last_updated;
};


struct ev_token_bucket_cfg {

size_t read_rate;

size_t read_maximum;

size_t write_rate;

size_t write_maximum;



struct timeval tick_timeout;


unsigned msec_per_tick;
};



int ev_token_bucket_update_(struct ev_token_bucket *bucket,
const struct ev_token_bucket_cfg *cfg,
ev_uint32_t current_tick);



ev_uint32_t ev_token_bucket_get_tick_(const struct timeval *tv,
const struct ev_token_bucket_cfg *cfg);






int ev_token_bucket_init_(struct ev_token_bucket *bucket,
const struct ev_token_bucket_cfg *cfg,
ev_uint32_t current_tick,
int reinitialize);

int bufferevent_remove_from_rate_limit_group_internal_(struct bufferevent *bev,
int unsuspend);


#define ev_token_bucket_decrement_read(b,n) do { (b)->read_limit -= (n); } while (0)




#define ev_token_bucket_decrement_write(b,n) do { (b)->write_limit -= (n); } while (0)




#if defined(__cplusplus)
}
#endif

#endif
