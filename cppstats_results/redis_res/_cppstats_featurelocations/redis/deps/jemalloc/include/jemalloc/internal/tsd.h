#if !defined(JEMALLOC_INTERNAL_TSD_H)
#define JEMALLOC_INTERNAL_TSD_H

#include "jemalloc/internal/arena_types.h"
#include "jemalloc/internal/assert.h"
#include "jemalloc/internal/bin_types.h"
#include "jemalloc/internal/jemalloc_internal_externs.h"
#include "jemalloc/internal/prof_types.h"
#include "jemalloc/internal/ql.h"
#include "jemalloc/internal/rtree_tsd.h"
#include "jemalloc/internal/tcache_types.h"
#include "jemalloc/internal/tcache_structs.h"
#include "jemalloc/internal/util.h"
#include "jemalloc/internal/witness.h"




































#if defined(JEMALLOC_JET)
typedef void (*test_callback_t)(int *);
#define MALLOC_TSD_TEST_DATA_INIT 0x72b65c10
#define MALLOC_TEST_TSD O(test_data, int, int) O(test_callback, test_callback_t, int)


#define MALLOC_TEST_TSD_INITIALIZER , MALLOC_TSD_TEST_DATA_INIT, NULL
#else
#define MALLOC_TEST_TSD
#define MALLOC_TEST_TSD_INITIALIZER
#endif


#define MALLOC_TSD O(tcache_enabled, bool, bool) O(arenas_tdata_bypass, bool, bool) O(reentrancy_level, int8_t, int8_t) O(narenas_tdata, uint32_t, uint32_t) O(offset_state, uint64_t, uint64_t) O(thread_allocated, uint64_t, uint64_t) O(thread_deallocated, uint64_t, uint64_t) O(bytes_until_sample, int64_t, int64_t) O(prof_tdata, prof_tdata_t *, prof_tdata_t *) O(rtree_ctx, rtree_ctx_t, rtree_ctx_t) O(iarena, arena_t *, arena_t *) O(arena, arena_t *, arena_t *) O(arenas_tdata, arena_tdata_t *, arena_tdata_t *)O(binshards, tsd_binshards_t, tsd_binshards_t)O(tcache, tcache_t, tcache_t) O(witness_tsd, witness_tsd_t, witness_tsdn_t) MALLOC_TEST_TSD


















#define TSD_INITIALIZER { ATOMIC_INIT(tsd_state_uninitialized), TCACHE_ENABLED_ZERO_INITIALIZER, false, 0, 0, 0, 0, 0, 0, NULL, RTREE_CTX_ZERO_INITIALIZER, NULL, NULL, NULL, TSD_BINSHARDS_ZERO_INITIALIZER, TCACHE_ZERO_INITIALIZER, WITNESS_TSD_INITIALIZER MALLOC_TEST_TSD_INITIALIZER }




















void *malloc_tsd_malloc(size_t size);
void malloc_tsd_dalloc(void *wrapper);
void malloc_tsd_cleanup_register(bool (*f)(void));
tsd_t *malloc_tsd_boot0(void);
void malloc_tsd_boot1(void);
void tsd_cleanup(void *arg);
tsd_t *tsd_fetch_slow(tsd_t *tsd, bool internal);
void tsd_state_set(tsd_t *tsd, uint8_t new_state);
void tsd_slow_update(tsd_t *tsd);
void tsd_prefork(tsd_t *tsd);
void tsd_postfork_parent(tsd_t *tsd);
void tsd_postfork_child(tsd_t *tsd);





void tsd_global_slow_inc(tsdn_t *tsdn);
void tsd_global_slow_dec(tsdn_t *tsdn);
bool tsd_global_slow();

enum {

tsd_state_nominal = 0,

tsd_state_nominal_slow = 1,









tsd_state_nominal_recompute = 2,





tsd_state_nominal_max = 2,






tsd_state_minimal_initialized = 3,

tsd_state_purgatory = 4,
tsd_state_reincarnated = 5,






tsd_state_uninitialized = 6
};






#define TSD_MANGLE(n) cant_access_tsd_items_directly_use_a_getter_or_setter_##n

#if defined(JEMALLOC_U8_ATOMICS)
#define tsd_state_t atomic_u8_t
#define tsd_atomic_load atomic_load_u8
#define tsd_atomic_store atomic_store_u8
#define tsd_atomic_exchange atomic_exchange_u8
#else
#define tsd_state_t atomic_u32_t
#define tsd_atomic_load atomic_load_u32
#define tsd_atomic_store atomic_store_u32
#define tsd_atomic_exchange atomic_exchange_u32
#endif


struct tsd_s {










tsd_state_t state;
#define O(n, t, nt) t TSD_MANGLE(n);

MALLOC_TSD
#undef O
};

JEMALLOC_ALWAYS_INLINE uint8_t
tsd_state_get(tsd_t *tsd) {






return *(uint8_t *)&tsd->state;
}






struct tsdn_s {
tsd_t tsd;
};
#define TSDN_NULL ((tsdn_t *)0)
JEMALLOC_ALWAYS_INLINE tsdn_t *
tsd_tsdn(tsd_t *tsd) {
return (tsdn_t *)tsd;
}

JEMALLOC_ALWAYS_INLINE bool
tsdn_null(const tsdn_t *tsdn) {
return tsdn == NULL;
}

JEMALLOC_ALWAYS_INLINE tsd_t *
tsdn_tsd(tsdn_t *tsdn) {
assert(!tsdn_null(tsdn));

return &tsdn->tsd;
}






#if defined(JEMALLOC_MALLOC_THREAD_CLEANUP)
#include "jemalloc/internal/tsd_malloc_thread_cleanup.h"
#elif (defined(JEMALLOC_TLS))
#include "jemalloc/internal/tsd_tls.h"
#elif (defined(_WIN32))
#include "jemalloc/internal/tsd_win.h"
#else
#include "jemalloc/internal/tsd_generic.h"
#endif






#define O(n, t, nt) JEMALLOC_ALWAYS_INLINE t * tsd_##n##p_get_unsafe(tsd_t *tsd) { return &tsd->TSD_MANGLE(n); }




MALLOC_TSD
#undef O


#define O(n, t, nt) JEMALLOC_ALWAYS_INLINE t * tsd_##n##p_get(tsd_t *tsd) { uint8_t state = tsd_state_get(tsd); assert(state == tsd_state_nominal || state == tsd_state_nominal_slow || state == tsd_state_nominal_recompute || state == tsd_state_reincarnated || state == tsd_state_minimal_initialized); return tsd_##n##p_get_unsafe(tsd); }














MALLOC_TSD
#undef O





#define O(n, t, nt) JEMALLOC_ALWAYS_INLINE nt * tsdn_##n##p_get(tsdn_t *tsdn) { if (tsdn_null(tsdn)) { return NULL; } tsd_t *tsd = tsdn_tsd(tsdn); return (nt *)tsd_##n##p_get(tsd); }








MALLOC_TSD
#undef O


#define O(n, t, nt) JEMALLOC_ALWAYS_INLINE t tsd_##n##_get(tsd_t *tsd) { return *tsd_##n##p_get(tsd); }




MALLOC_TSD
#undef O


#define O(n, t, nt) JEMALLOC_ALWAYS_INLINE void tsd_##n##_set(tsd_t *tsd, t val) { assert(tsd_state_get(tsd) != tsd_state_reincarnated && tsd_state_get(tsd) != tsd_state_minimal_initialized); *tsd_##n##p_get(tsd) = val; }






MALLOC_TSD
#undef O

JEMALLOC_ALWAYS_INLINE void
tsd_assert_fast(tsd_t *tsd) {





assert(!malloc_slow && tsd_tcache_enabled_get(tsd) &&
tsd_reentrancy_level_get(tsd) == 0);
}

JEMALLOC_ALWAYS_INLINE bool
tsd_fast(tsd_t *tsd) {
bool fast = (tsd_state_get(tsd) == tsd_state_nominal);
if (fast) {
tsd_assert_fast(tsd);
}

return fast;
}

JEMALLOC_ALWAYS_INLINE tsd_t *
tsd_fetch_impl(bool init, bool minimal) {
tsd_t *tsd = tsd_get(init);

if (!init && tsd_get_allocates() && tsd == NULL) {
return NULL;
}
assert(tsd != NULL);

if (unlikely(tsd_state_get(tsd) != tsd_state_nominal)) {
return tsd_fetch_slow(tsd, minimal);
}
assert(tsd_fast(tsd));
tsd_assert_fast(tsd);

return tsd;
}


JEMALLOC_ALWAYS_INLINE tsd_t *
tsd_fetch_min(void) {
return tsd_fetch_impl(true, true);
}


JEMALLOC_ALWAYS_INLINE tsd_t *
tsd_internal_fetch(void) {
tsd_t *tsd = tsd_fetch_min();

tsd_state_set(tsd, tsd_state_reincarnated);

return tsd;
}

JEMALLOC_ALWAYS_INLINE tsd_t *
tsd_fetch(void) {
return tsd_fetch_impl(true, false);
}

static inline bool
tsd_nominal(tsd_t *tsd) {
return (tsd_state_get(tsd) <= tsd_state_nominal_max);
}

JEMALLOC_ALWAYS_INLINE tsdn_t *
tsdn_fetch(void) {
if (!tsd_booted_get()) {
return NULL;
}

return tsd_tsdn(tsd_fetch_impl(false, false));
}

JEMALLOC_ALWAYS_INLINE rtree_ctx_t *
tsd_rtree_ctx(tsd_t *tsd) {
return tsd_rtree_ctxp_get(tsd);
}

JEMALLOC_ALWAYS_INLINE rtree_ctx_t *
tsdn_rtree_ctx(tsdn_t *tsdn, rtree_ctx_t *fallback) {




if (unlikely(tsdn_null(tsdn))) {
rtree_ctx_data_init(fallback);
return fallback;
}
return tsd_rtree_ctx(tsdn_tsd(tsdn));
}

#endif
