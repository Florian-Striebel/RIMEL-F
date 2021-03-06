#include "jemalloc/internal/jemalloc_internal_types.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/sc.h"
struct base_block_s {
size_t size;
base_block_t *next;
extent_t extent;
};
struct base_s {
unsigned ind;
atomic_p_t extent_hooks;
malloc_mutex_t mtx;
bool auto_thp_switched;
pszind_t pind_last;
size_t extent_sn_next;
base_block_t *blocks;
extent_heap_t avail[SC_NSIZES];
size_t allocated;
size_t resident;
size_t mapped;
size_t n_thp;
};
