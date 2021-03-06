#if !defined(JEMALLOC_INTERNAL_CACHE_BIN_H)
#define JEMALLOC_INTERNAL_CACHE_BIN_H

#include "jemalloc/internal/ql.h"

















typedef int32_t cache_bin_sz_t;

typedef struct cache_bin_stats_s cache_bin_stats_t;
struct cache_bin_stats_s {




uint64_t nrequests;
};





typedef struct cache_bin_info_s cache_bin_info_t;
struct cache_bin_info_s {

cache_bin_sz_t ncached_max;
};

typedef struct cache_bin_s cache_bin_t;
struct cache_bin_s {

cache_bin_sz_t low_water;

cache_bin_sz_t ncached;





cache_bin_stats_t tstats;









void **avail;
};

typedef struct cache_bin_array_descriptor_s cache_bin_array_descriptor_t;
struct cache_bin_array_descriptor_s {




ql_elm(cache_bin_array_descriptor_t) link;

cache_bin_t *bins_small;
cache_bin_t *bins_large;
};

static inline void
cache_bin_array_descriptor_init(cache_bin_array_descriptor_t *descriptor,
cache_bin_t *bins_small, cache_bin_t *bins_large) {
ql_elm_new(descriptor, link);
descriptor->bins_small = bins_small;
descriptor->bins_large = bins_large;
}

JEMALLOC_ALWAYS_INLINE void *
cache_bin_alloc_easy(cache_bin_t *bin, bool *success) {
void *ret;

bin->ncached--;





if (unlikely(bin->ncached <= bin->low_water)) {
bin->low_water = bin->ncached;
if (bin->ncached == -1) {
bin->ncached = 0;
*success = false;
return NULL;
}
}








*success = true;
ret = *(bin->avail - (bin->ncached + 1));

return ret;
}

JEMALLOC_ALWAYS_INLINE bool
cache_bin_dalloc_easy(cache_bin_t *bin, cache_bin_info_t *bin_info, void *ptr) {
if (unlikely(bin->ncached == bin_info->ncached_max)) {
return false;
}
assert(bin->ncached < bin_info->ncached_max);
bin->ncached++;
*(bin->avail - bin->ncached) = ptr;

return true;
}

#endif
