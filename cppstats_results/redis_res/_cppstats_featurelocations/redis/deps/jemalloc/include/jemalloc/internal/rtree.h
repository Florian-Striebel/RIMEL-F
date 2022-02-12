#if !defined(JEMALLOC_INTERNAL_RTREE_H)
#define JEMALLOC_INTERNAL_RTREE_H

#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/rtree_tsd.h"
#include "jemalloc/internal/sc.h"
#include "jemalloc/internal/tsd.h"









#define RTREE_NHIB ((1U << (LG_SIZEOF_PTR+3)) - LG_VADDR)

#define RTREE_NLIB LG_PAGE

#define RTREE_NSB (LG_VADDR - RTREE_NLIB)

#if RTREE_NSB <= 10
#define RTREE_HEIGHT 1
#elif RTREE_NSB <= 36
#define RTREE_HEIGHT 2
#elif RTREE_NSB <= 52
#define RTREE_HEIGHT 3
#else
#error Unsupported number of significant virtual address bits
#endif

#if RTREE_NHIB >= LG_CEIL(SC_NSIZES)
#define RTREE_LEAF_COMPACT
#endif


#define RTREE_LEAFKEY_INVALID ((uintptr_t)1)

typedef struct rtree_node_elm_s rtree_node_elm_t;
struct rtree_node_elm_s {
atomic_p_t child;
};

struct rtree_leaf_elm_s {
#if defined(RTREE_LEAF_COMPACT)












atomic_p_t le_bits;
#else
atomic_p_t le_extent;
atomic_u_t le_szind;
atomic_b_t le_slab;
#endif
};

typedef struct rtree_level_s rtree_level_t;
struct rtree_level_s {

unsigned bits;




unsigned cumbits;
};

typedef struct rtree_s rtree_t;
struct rtree_s {
malloc_mutex_t init_lock;

#if RTREE_HEIGHT > 1
rtree_node_elm_t root[1U << (RTREE_NSB/RTREE_HEIGHT)];
#else
rtree_leaf_elm_t root[1U << (RTREE_NSB/RTREE_HEIGHT)];
#endif
};







static const rtree_level_t rtree_levels[] = {
#if RTREE_HEIGHT == 1
{RTREE_NSB, RTREE_NHIB + RTREE_NSB}
#elif RTREE_HEIGHT == 2
{RTREE_NSB/2, RTREE_NHIB + RTREE_NSB/2},
{RTREE_NSB/2 + RTREE_NSB%2, RTREE_NHIB + RTREE_NSB}
#elif RTREE_HEIGHT == 3
{RTREE_NSB/3, RTREE_NHIB + RTREE_NSB/3},
{RTREE_NSB/3 + RTREE_NSB%3/2,
RTREE_NHIB + RTREE_NSB/3*2 + RTREE_NSB%3/2},
{RTREE_NSB/3 + RTREE_NSB%3 - RTREE_NSB%3/2, RTREE_NHIB + RTREE_NSB}
#else
#error Unsupported rtree height
#endif
};

bool rtree_new(rtree_t *rtree, bool zeroed);

typedef rtree_node_elm_t *(rtree_node_alloc_t)(tsdn_t *, rtree_t *, size_t);
extern rtree_node_alloc_t *JET_MUTABLE rtree_node_alloc;

typedef rtree_leaf_elm_t *(rtree_leaf_alloc_t)(tsdn_t *, rtree_t *, size_t);
extern rtree_leaf_alloc_t *JET_MUTABLE rtree_leaf_alloc;

typedef void (rtree_node_dalloc_t)(tsdn_t *, rtree_t *, rtree_node_elm_t *);
extern rtree_node_dalloc_t *JET_MUTABLE rtree_node_dalloc;

typedef void (rtree_leaf_dalloc_t)(tsdn_t *, rtree_t *, rtree_leaf_elm_t *);
extern rtree_leaf_dalloc_t *JET_MUTABLE rtree_leaf_dalloc;
#if defined(JEMALLOC_JET)
void rtree_delete(tsdn_t *tsdn, rtree_t *rtree);
#endif
rtree_leaf_elm_t *rtree_leaf_elm_lookup_hard(tsdn_t *tsdn, rtree_t *rtree,
rtree_ctx_t *rtree_ctx, uintptr_t key, bool dependent, bool init_missing);

JEMALLOC_ALWAYS_INLINE uintptr_t
rtree_leafkey(uintptr_t key) {
unsigned ptrbits = ZU(1) << (LG_SIZEOF_PTR+3);
unsigned cumbits = (rtree_levels[RTREE_HEIGHT-1].cumbits -
rtree_levels[RTREE_HEIGHT-1].bits);
unsigned maskbits = ptrbits - cumbits;
uintptr_t mask = ~((ZU(1) << maskbits) - 1);
return (key & mask);
}

JEMALLOC_ALWAYS_INLINE size_t
rtree_cache_direct_map(uintptr_t key) {
unsigned ptrbits = ZU(1) << (LG_SIZEOF_PTR+3);
unsigned cumbits = (rtree_levels[RTREE_HEIGHT-1].cumbits -
rtree_levels[RTREE_HEIGHT-1].bits);
unsigned maskbits = ptrbits - cumbits;
return (size_t)((key >> maskbits) & (RTREE_CTX_NCACHE - 1));
}

JEMALLOC_ALWAYS_INLINE uintptr_t
rtree_subkey(uintptr_t key, unsigned level) {
unsigned ptrbits = ZU(1) << (LG_SIZEOF_PTR+3);
unsigned cumbits = rtree_levels[level].cumbits;
unsigned shiftbits = ptrbits - cumbits;
unsigned maskbits = rtree_levels[level].bits;
uintptr_t mask = (ZU(1) << maskbits) - 1;
return ((key >> shiftbits) & mask);
}












#if defined(RTREE_LEAF_COMPACT)
JEMALLOC_ALWAYS_INLINE uintptr_t
rtree_leaf_elm_bits_read(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, bool dependent) {
return (uintptr_t)atomic_load_p(&elm->le_bits, dependent
? ATOMIC_RELAXED : ATOMIC_ACQUIRE);
}

JEMALLOC_ALWAYS_INLINE extent_t *
rtree_leaf_elm_bits_extent_get(uintptr_t bits) {
#if defined(__aarch64__)




uintptr_t high_bit_mask = ((uintptr_t)1 << LG_VADDR) - 1;

uintptr_t low_bit_mask = ~(uintptr_t)1;
uintptr_t mask = high_bit_mask & low_bit_mask;
return (extent_t *)(bits & mask);
#else

return (extent_t *)((uintptr_t)((intptr_t)(bits << RTREE_NHIB) >>
RTREE_NHIB) & ~((uintptr_t)0x1));
#endif
}

JEMALLOC_ALWAYS_INLINE szind_t
rtree_leaf_elm_bits_szind_get(uintptr_t bits) {
return (szind_t)(bits >> LG_VADDR);
}

JEMALLOC_ALWAYS_INLINE bool
rtree_leaf_elm_bits_slab_get(uintptr_t bits) {
return (bool)(bits & (uintptr_t)0x1);
}

#endif

JEMALLOC_ALWAYS_INLINE extent_t *
rtree_leaf_elm_extent_read(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, bool dependent) {
#if defined(RTREE_LEAF_COMPACT)
uintptr_t bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm, dependent);
return rtree_leaf_elm_bits_extent_get(bits);
#else
extent_t *extent = (extent_t *)atomic_load_p(&elm->le_extent, dependent
? ATOMIC_RELAXED : ATOMIC_ACQUIRE);
return extent;
#endif
}

JEMALLOC_ALWAYS_INLINE szind_t
rtree_leaf_elm_szind_read(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, bool dependent) {
#if defined(RTREE_LEAF_COMPACT)
uintptr_t bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm, dependent);
return rtree_leaf_elm_bits_szind_get(bits);
#else
return (szind_t)atomic_load_u(&elm->le_szind, dependent ? ATOMIC_RELAXED
: ATOMIC_ACQUIRE);
#endif
}

JEMALLOC_ALWAYS_INLINE bool
rtree_leaf_elm_slab_read(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, bool dependent) {
#if defined(RTREE_LEAF_COMPACT)
uintptr_t bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm, dependent);
return rtree_leaf_elm_bits_slab_get(bits);
#else
return atomic_load_b(&elm->le_slab, dependent ? ATOMIC_RELAXED :
ATOMIC_ACQUIRE);
#endif
}

static inline void
rtree_leaf_elm_extent_write(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, extent_t *extent) {
#if defined(RTREE_LEAF_COMPACT)
uintptr_t old_bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm, true);
uintptr_t bits = ((uintptr_t)rtree_leaf_elm_bits_szind_get(old_bits) <<
LG_VADDR) | ((uintptr_t)extent & (((uintptr_t)0x1 << LG_VADDR) - 1))
| ((uintptr_t)rtree_leaf_elm_bits_slab_get(old_bits));
atomic_store_p(&elm->le_bits, (void *)bits, ATOMIC_RELEASE);
#else
atomic_store_p(&elm->le_extent, extent, ATOMIC_RELEASE);
#endif
}

static inline void
rtree_leaf_elm_szind_write(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, szind_t szind) {
assert(szind <= SC_NSIZES);

#if defined(RTREE_LEAF_COMPACT)
uintptr_t old_bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm,
true);
uintptr_t bits = ((uintptr_t)szind << LG_VADDR) |
((uintptr_t)rtree_leaf_elm_bits_extent_get(old_bits) &
(((uintptr_t)0x1 << LG_VADDR) - 1)) |
((uintptr_t)rtree_leaf_elm_bits_slab_get(old_bits));
atomic_store_p(&elm->le_bits, (void *)bits, ATOMIC_RELEASE);
#else
atomic_store_u(&elm->le_szind, szind, ATOMIC_RELEASE);
#endif
}

static inline void
rtree_leaf_elm_slab_write(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, bool slab) {
#if defined(RTREE_LEAF_COMPACT)
uintptr_t old_bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm,
true);
uintptr_t bits = ((uintptr_t)rtree_leaf_elm_bits_szind_get(old_bits) <<
LG_VADDR) | ((uintptr_t)rtree_leaf_elm_bits_extent_get(old_bits) &
(((uintptr_t)0x1 << LG_VADDR) - 1)) | ((uintptr_t)slab);
atomic_store_p(&elm->le_bits, (void *)bits, ATOMIC_RELEASE);
#else
atomic_store_b(&elm->le_slab, slab, ATOMIC_RELEASE);
#endif
}

static inline void
rtree_leaf_elm_write(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, extent_t *extent, szind_t szind, bool slab) {
#if defined(RTREE_LEAF_COMPACT)
uintptr_t bits = ((uintptr_t)szind << LG_VADDR) |
((uintptr_t)extent & (((uintptr_t)0x1 << LG_VADDR) - 1)) |
((uintptr_t)slab);
atomic_store_p(&elm->le_bits, (void *)bits, ATOMIC_RELEASE);
#else
rtree_leaf_elm_slab_write(tsdn, rtree, elm, slab);
rtree_leaf_elm_szind_write(tsdn, rtree, elm, szind);




rtree_leaf_elm_extent_write(tsdn, rtree, elm, extent);
#endif
}

static inline void
rtree_leaf_elm_szind_slab_update(tsdn_t *tsdn, rtree_t *rtree,
rtree_leaf_elm_t *elm, szind_t szind, bool slab) {
assert(!slab || szind < SC_NBINS);





rtree_leaf_elm_slab_write(tsdn, rtree, elm, slab);
rtree_leaf_elm_szind_write(tsdn, rtree, elm, szind);
}

JEMALLOC_ALWAYS_INLINE rtree_leaf_elm_t *
rtree_leaf_elm_lookup(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key, bool dependent, bool init_missing) {
assert(key != 0);
assert(!dependent || !init_missing);

size_t slot = rtree_cache_direct_map(key);
uintptr_t leafkey = rtree_leafkey(key);
assert(leafkey != RTREE_LEAFKEY_INVALID);


if (likely(rtree_ctx->cache[slot].leafkey == leafkey)) {
rtree_leaf_elm_t *leaf = rtree_ctx->cache[slot].leaf;
assert(leaf != NULL);
uintptr_t subkey = rtree_subkey(key, RTREE_HEIGHT-1);
return &leaf[subkey];
}




#define RTREE_CACHE_CHECK_L2(i) do { if (likely(rtree_ctx->l2_cache[i].leafkey == leafkey)) { rtree_leaf_elm_t *leaf = rtree_ctx->l2_cache[i].leaf; assert(leaf != NULL); if (i > 0) { rtree_ctx->l2_cache[i].leafkey = rtree_ctx->l2_cache[i - 1].leafkey; rtree_ctx->l2_cache[i].leaf = rtree_ctx->l2_cache[i - 1].leaf; rtree_ctx->l2_cache[i - 1].leafkey = rtree_ctx->cache[slot].leafkey; rtree_ctx->l2_cache[i - 1].leaf = rtree_ctx->cache[slot].leaf; } else { rtree_ctx->l2_cache[0].leafkey = rtree_ctx->cache[slot].leafkey; rtree_ctx->l2_cache[0].leaf = rtree_ctx->cache[slot].leaf; } rtree_ctx->cache[slot].leafkey = leafkey; rtree_ctx->cache[slot].leaf = leaf; uintptr_t subkey = rtree_subkey(key, RTREE_HEIGHT-1); return &leaf[subkey]; } } while (0)


























RTREE_CACHE_CHECK_L2(0);

for (unsigned i = 1; i < RTREE_CTX_NCACHE_L2; i++) {
RTREE_CACHE_CHECK_L2(i);
}
#undef RTREE_CACHE_CHECK_L2

return rtree_leaf_elm_lookup_hard(tsdn, rtree, rtree_ctx, key,
dependent, init_missing);
}

static inline bool
rtree_write(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx, uintptr_t key,
extent_t *extent, szind_t szind, bool slab) {

assert(extent != NULL);

rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup(tsdn, rtree, rtree_ctx,
key, false, true);
if (elm == NULL) {
return true;
}

assert(rtree_leaf_elm_extent_read(tsdn, rtree, elm, false) == NULL);
rtree_leaf_elm_write(tsdn, rtree, elm, extent, szind, slab);

return false;
}

JEMALLOC_ALWAYS_INLINE rtree_leaf_elm_t *
rtree_read(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx, uintptr_t key,
bool dependent) {
rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup(tsdn, rtree, rtree_ctx,
key, dependent, false);
if (!dependent && elm == NULL) {
return NULL;
}
assert(elm != NULL);
return elm;
}

JEMALLOC_ALWAYS_INLINE extent_t *
rtree_extent_read(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key, bool dependent) {
rtree_leaf_elm_t *elm = rtree_read(tsdn, rtree, rtree_ctx, key,
dependent);
if (!dependent && elm == NULL) {
return NULL;
}
return rtree_leaf_elm_extent_read(tsdn, rtree, elm, dependent);
}

JEMALLOC_ALWAYS_INLINE szind_t
rtree_szind_read(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key, bool dependent) {
rtree_leaf_elm_t *elm = rtree_read(tsdn, rtree, rtree_ctx, key,
dependent);
if (!dependent && elm == NULL) {
return SC_NSIZES;
}
return rtree_leaf_elm_szind_read(tsdn, rtree, elm, dependent);
}






JEMALLOC_ALWAYS_INLINE bool
rtree_extent_szind_read(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key, bool dependent, extent_t **r_extent, szind_t *r_szind) {
rtree_leaf_elm_t *elm = rtree_read(tsdn, rtree, rtree_ctx, key,
dependent);
if (!dependent && elm == NULL) {
return true;
}
*r_extent = rtree_leaf_elm_extent_read(tsdn, rtree, elm, dependent);
*r_szind = rtree_leaf_elm_szind_read(tsdn, rtree, elm, dependent);
return false;
}








JEMALLOC_ALWAYS_INLINE bool
rtree_szind_slab_read_fast(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key, szind_t *r_szind, bool *r_slab) {
rtree_leaf_elm_t *elm;

size_t slot = rtree_cache_direct_map(key);
uintptr_t leafkey = rtree_leafkey(key);
assert(leafkey != RTREE_LEAFKEY_INVALID);

if (likely(rtree_ctx->cache[slot].leafkey == leafkey)) {
rtree_leaf_elm_t *leaf = rtree_ctx->cache[slot].leaf;
assert(leaf != NULL);
uintptr_t subkey = rtree_subkey(key, RTREE_HEIGHT-1);
elm = &leaf[subkey];

#if defined(RTREE_LEAF_COMPACT)
uintptr_t bits = rtree_leaf_elm_bits_read(tsdn, rtree,
elm, true);
*r_szind = rtree_leaf_elm_bits_szind_get(bits);
*r_slab = rtree_leaf_elm_bits_slab_get(bits);
#else
*r_szind = rtree_leaf_elm_szind_read(tsdn, rtree, elm, true);
*r_slab = rtree_leaf_elm_slab_read(tsdn, rtree, elm, true);
#endif
return true;
} else {
return false;
}
}
JEMALLOC_ALWAYS_INLINE bool
rtree_szind_slab_read(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key, bool dependent, szind_t *r_szind, bool *r_slab) {
rtree_leaf_elm_t *elm = rtree_read(tsdn, rtree, rtree_ctx, key,
dependent);
if (!dependent && elm == NULL) {
return true;
}
#if defined(RTREE_LEAF_COMPACT)
uintptr_t bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm, dependent);
*r_szind = rtree_leaf_elm_bits_szind_get(bits);
*r_slab = rtree_leaf_elm_bits_slab_get(bits);
#else
*r_szind = rtree_leaf_elm_szind_read(tsdn, rtree, elm, dependent);
*r_slab = rtree_leaf_elm_slab_read(tsdn, rtree, elm, dependent);
#endif
return false;
}

static inline void
rtree_szind_slab_update(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key, szind_t szind, bool slab) {
assert(!slab || szind < SC_NBINS);

rtree_leaf_elm_t *elm = rtree_read(tsdn, rtree, rtree_ctx, key, true);
rtree_leaf_elm_szind_slab_update(tsdn, rtree, elm, szind, slab);
}

static inline void
rtree_clear(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
uintptr_t key) {
rtree_leaf_elm_t *elm = rtree_read(tsdn, rtree, rtree_ctx, key, true);
assert(rtree_leaf_elm_extent_read(tsdn, rtree, elm, false) !=
NULL);
rtree_leaf_elm_write(tsdn, rtree, elm, NULL, SC_NSIZES, false);
}

#endif
