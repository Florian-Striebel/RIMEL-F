#if !defined(JEMALLOC_INTERNAL_ARENA_STRUCTS_B_H)
#define JEMALLOC_INTERNAL_ARENA_STRUCTS_B_H

#include "jemalloc/internal/arena_stats.h"
#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/bin.h"
#include "jemalloc/internal/bitmap.h"
#include "jemalloc/internal/extent_dss.h"
#include "jemalloc/internal/jemalloc_internal_types.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/nstime.h"
#include "jemalloc/internal/ql.h"
#include "jemalloc/internal/sc.h"
#include "jemalloc/internal/smoothstep.h"
#include "jemalloc/internal/ticker.h"

struct arena_decay_s {

malloc_mutex_t mtx;




bool purging;





atomic_zd_t time_ms;

nstime_t interval;







nstime_t epoch;

uint64_t jitter_state;







nstime_t deadline;






size_t nunpurged;






size_t backlog[SMOOTHSTEP_NSTEPS];







arena_stats_decay_t *stats;

uint64_t ceil_npages;
};

struct arena_s {













atomic_u_t nthreads[2];


atomic_u_t binshard_next;






tsdn_t *last_thd;


arena_stats_t stats;








ql_head(tcache_t) tcache_ql;
ql_head(cache_bin_array_descriptor_t) cache_bin_array_descriptor_ql;
malloc_mutex_t tcache_ql_mtx;


prof_accum_t prof_accum;







atomic_zu_t offset_state;






atomic_zu_t extent_sn_next;






atomic_u_t dss_prec;






atomic_zu_t nactive;






extent_list_t large;

malloc_mutex_t large_mtx;







extents_t extents_dirty;
extents_t extents_muzzy;
extents_t extents_retained;







arena_decay_t decay_dirty;
arena_decay_t decay_muzzy;














pszind_t extent_grow_next;
pszind_t retain_grow_limit;
malloc_mutex_t extent_grow_mtx;







extent_tree_t extent_avail;
atomic_zu_t extent_avail_cnt;
malloc_mutex_t extent_avail_mtx;






bins_t bins[SC_NBINS];






base_t *base;

nstime_t create_time;
};


struct arena_tdata_s {
ticker_t decay_ticker;
};


struct alloc_ctx_s {
szind_t szind;
bool slab;
};

#endif
