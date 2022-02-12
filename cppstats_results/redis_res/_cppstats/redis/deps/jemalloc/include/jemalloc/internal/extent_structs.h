#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/bit_util.h"
#include "jemalloc/internal/bitmap.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/ql.h"
#include "jemalloc/internal/ph.h"
#include "jemalloc/internal/sc.h"
typedef enum {
extent_state_active = 0,
extent_state_dirty = 1,
extent_state_muzzy = 2,
extent_state_retained = 3
} extent_state_t;
struct extent_s {
uint64_t e_bits;
#define MASK(CURRENT_FIELD_WIDTH, CURRENT_FIELD_SHIFT) ((((((uint64_t)0x1U) << (CURRENT_FIELD_WIDTH)) - 1)) << (CURRENT_FIELD_SHIFT))
#define EXTENT_BITS_ARENA_WIDTH MALLOCX_ARENA_BITS
#define EXTENT_BITS_ARENA_SHIFT 0
#define EXTENT_BITS_ARENA_MASK MASK(EXTENT_BITS_ARENA_WIDTH, EXTENT_BITS_ARENA_SHIFT)
#define EXTENT_BITS_SLAB_WIDTH 1
#define EXTENT_BITS_SLAB_SHIFT (EXTENT_BITS_ARENA_WIDTH + EXTENT_BITS_ARENA_SHIFT)
#define EXTENT_BITS_SLAB_MASK MASK(EXTENT_BITS_SLAB_WIDTH, EXTENT_BITS_SLAB_SHIFT)
#define EXTENT_BITS_COMMITTED_WIDTH 1
#define EXTENT_BITS_COMMITTED_SHIFT (EXTENT_BITS_SLAB_WIDTH + EXTENT_BITS_SLAB_SHIFT)
#define EXTENT_BITS_COMMITTED_MASK MASK(EXTENT_BITS_COMMITTED_WIDTH, EXTENT_BITS_COMMITTED_SHIFT)
#define EXTENT_BITS_DUMPABLE_WIDTH 1
#define EXTENT_BITS_DUMPABLE_SHIFT (EXTENT_BITS_COMMITTED_WIDTH + EXTENT_BITS_COMMITTED_SHIFT)
#define EXTENT_BITS_DUMPABLE_MASK MASK(EXTENT_BITS_DUMPABLE_WIDTH, EXTENT_BITS_DUMPABLE_SHIFT)
#define EXTENT_BITS_ZEROED_WIDTH 1
#define EXTENT_BITS_ZEROED_SHIFT (EXTENT_BITS_DUMPABLE_WIDTH + EXTENT_BITS_DUMPABLE_SHIFT)
#define EXTENT_BITS_ZEROED_MASK MASK(EXTENT_BITS_ZEROED_WIDTH, EXTENT_BITS_ZEROED_SHIFT)
#define EXTENT_BITS_STATE_WIDTH 2
#define EXTENT_BITS_STATE_SHIFT (EXTENT_BITS_ZEROED_WIDTH + EXTENT_BITS_ZEROED_SHIFT)
#define EXTENT_BITS_STATE_MASK MASK(EXTENT_BITS_STATE_WIDTH, EXTENT_BITS_STATE_SHIFT)
#define EXTENT_BITS_SZIND_WIDTH LG_CEIL(SC_NSIZES)
#define EXTENT_BITS_SZIND_SHIFT (EXTENT_BITS_STATE_WIDTH + EXTENT_BITS_STATE_SHIFT)
#define EXTENT_BITS_SZIND_MASK MASK(EXTENT_BITS_SZIND_WIDTH, EXTENT_BITS_SZIND_SHIFT)
#define EXTENT_BITS_NFREE_WIDTH (LG_SLAB_MAXREGS + 1)
#define EXTENT_BITS_NFREE_SHIFT (EXTENT_BITS_SZIND_WIDTH + EXTENT_BITS_SZIND_SHIFT)
#define EXTENT_BITS_NFREE_MASK MASK(EXTENT_BITS_NFREE_WIDTH, EXTENT_BITS_NFREE_SHIFT)
#define EXTENT_BITS_BINSHARD_WIDTH 6
#define EXTENT_BITS_BINSHARD_SHIFT (EXTENT_BITS_NFREE_WIDTH + EXTENT_BITS_NFREE_SHIFT)
#define EXTENT_BITS_BINSHARD_MASK MASK(EXTENT_BITS_BINSHARD_WIDTH, EXTENT_BITS_BINSHARD_SHIFT)
#define EXTENT_BITS_IS_HEAD_WIDTH 1
#define EXTENT_BITS_IS_HEAD_SHIFT (EXTENT_BITS_BINSHARD_WIDTH + EXTENT_BITS_BINSHARD_SHIFT)
#define EXTENT_BITS_IS_HEAD_MASK MASK(EXTENT_BITS_IS_HEAD_WIDTH, EXTENT_BITS_IS_HEAD_SHIFT)
#define EXTENT_BITS_SN_SHIFT (EXTENT_BITS_IS_HEAD_WIDTH + EXTENT_BITS_IS_HEAD_SHIFT)
#define EXTENT_BITS_SN_MASK (UINT64_MAX << EXTENT_BITS_SN_SHIFT)
void *e_addr;
union {
size_t e_size_esn;
#define EXTENT_SIZE_MASK ((size_t)~(PAGE-1))
#define EXTENT_ESN_MASK ((size_t)PAGE-1)
size_t e_bsize;
};
ql_elm(extent_t) ql_link;
phn(extent_t) ph_link;
union {
arena_slab_data_t e_slab_data;
struct {
nstime_t e_alloc_time;
atomic_p_t e_prof_tctx;
};
};
};
typedef ql_head(extent_t) extent_list_t;
typedef ph(extent_t) extent_tree_t;
typedef ph(extent_t) extent_heap_t;
struct extents_s {
malloc_mutex_t mtx;
extent_heap_t heaps[SC_NPSIZES + 1];
atomic_zu_t nextents[SC_NPSIZES + 1];
atomic_zu_t nbytes[SC_NPSIZES + 1];
bitmap_t bitmap[BITMAP_GROUPS(SC_NPSIZES + 1)];
extent_list_t lru;
atomic_zu_t npages;
extent_state_t state;
bool delay_coalesce;
};
struct extent_util_stats_s {
size_t nfree;
size_t nregs;
size_t size;
};
struct extent_util_stats_verbose_s {
void *slabcur_addr;
size_t nfree;
size_t nregs;
size_t size;
size_t bin_nfree;
size_t bin_nregs;
};
