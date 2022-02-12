#include "jemalloc/internal/cache_bin.h"
#include "jemalloc/internal/ql.h"
#include "jemalloc/internal/sc.h"
#include "jemalloc/internal/ticker.h"
#include "jemalloc/internal/tsd_types.h"
typedef ql_elm(tsd_t) tsd_link_t;
struct tcache_s {
uint64_t prof_accumbytes;
ticker_t gc_ticker;
cache_bin_t bins_small[SC_NBINS];
ql_elm(tcache_t) link;
ql_elm(tsd_t) tsd_link;
bool in_hook;
cache_bin_array_descriptor_t cache_bin_array_descriptor;
arena_t *arena;
szind_t next_gc_bin;
uint8_t lg_fill_div[SC_NBINS];
cache_bin_t bins_large[SC_NSIZES-SC_NBINS];
};
struct tcaches_s {
union {
tcache_t *tcache;
tcaches_t *next;
};
};
