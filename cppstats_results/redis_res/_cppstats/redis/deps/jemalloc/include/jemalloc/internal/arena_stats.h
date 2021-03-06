#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/mutex_prof.h"
#include "jemalloc/internal/sc.h"
JEMALLOC_DIAGNOSTIC_DISABLE_SPURIOUS
#if defined(JEMALLOC_ATOMIC_U64)
typedef atomic_u64_t arena_stats_u64_t;
#else
typedef uint64_t arena_stats_u64_t;
#endif
typedef struct arena_stats_large_s arena_stats_large_t;
struct arena_stats_large_s {
arena_stats_u64_t nmalloc;
arena_stats_u64_t ndalloc;
arena_stats_u64_t nrequests;
arena_stats_u64_t nfills;
arena_stats_u64_t nflushes;
size_t curlextents;
};
typedef struct arena_stats_decay_s arena_stats_decay_t;
struct arena_stats_decay_s {
arena_stats_u64_t npurge;
arena_stats_u64_t nmadvise;
arena_stats_u64_t purged;
};
typedef struct arena_stats_extents_s arena_stats_extents_t;
struct arena_stats_extents_s {
atomic_zu_t ndirty;
atomic_zu_t dirty_bytes;
atomic_zu_t nmuzzy;
atomic_zu_t muzzy_bytes;
atomic_zu_t nretained;
atomic_zu_t retained_bytes;
};
typedef struct arena_stats_s arena_stats_t;
struct arena_stats_s {
#if !defined(JEMALLOC_ATOMIC_U64)
malloc_mutex_t mtx;
#endif
atomic_zu_t mapped;
atomic_zu_t retained;
atomic_zu_t extent_avail;
arena_stats_decay_t decay_dirty;
arena_stats_decay_t decay_muzzy;
atomic_zu_t base;
atomic_zu_t internal;
atomic_zu_t resident;
atomic_zu_t metadata_thp;
atomic_zu_t allocated_large;
arena_stats_u64_t nmalloc_large;
arena_stats_u64_t ndalloc_large;
arena_stats_u64_t nfills_large;
arena_stats_u64_t nflushes_large;
arena_stats_u64_t nrequests_large;
atomic_zu_t abandoned_vm;
atomic_zu_t tcache_bytes;
mutex_prof_data_t mutex_prof_data[mutex_prof_num_arena_mutexes];
arena_stats_large_t lstats[SC_NSIZES - SC_NBINS];
nstime_t uptime;
};
static inline bool
arena_stats_init(tsdn_t *tsdn, arena_stats_t *arena_stats) {
if (config_debug) {
for (size_t i = 0; i < sizeof(arena_stats_t); i++) {
assert(((char *)arena_stats)[i] == 0);
}
}
#if !defined(JEMALLOC_ATOMIC_U64)
if (malloc_mutex_init(&arena_stats->mtx, "arena_stats",
WITNESS_RANK_ARENA_STATS, malloc_mutex_rank_exclusive)) {
return true;
}
#endif
return false;
}
static inline void
arena_stats_lock(tsdn_t *tsdn, arena_stats_t *arena_stats) {
#if !defined(JEMALLOC_ATOMIC_U64)
malloc_mutex_lock(tsdn, &arena_stats->mtx);
#endif
}
static inline void
arena_stats_unlock(tsdn_t *tsdn, arena_stats_t *arena_stats) {
#if !defined(JEMALLOC_ATOMIC_U64)
malloc_mutex_unlock(tsdn, &arena_stats->mtx);
#endif
}
static inline uint64_t
arena_stats_read_u64(tsdn_t *tsdn, arena_stats_t *arena_stats,
arena_stats_u64_t *p) {
#if defined(JEMALLOC_ATOMIC_U64)
return atomic_load_u64(p, ATOMIC_RELAXED);
#else
malloc_mutex_assert_owner(tsdn, &arena_stats->mtx);
return *p;
#endif
}
static inline void
arena_stats_add_u64(tsdn_t *tsdn, arena_stats_t *arena_stats,
arena_stats_u64_t *p, uint64_t x) {
#if defined(JEMALLOC_ATOMIC_U64)
atomic_fetch_add_u64(p, x, ATOMIC_RELAXED);
#else
malloc_mutex_assert_owner(tsdn, &arena_stats->mtx);
*p += x;
#endif
}
static inline void
arena_stats_sub_u64(tsdn_t *tsdn, arena_stats_t *arena_stats,
arena_stats_u64_t *p, uint64_t x) {
#if defined(JEMALLOC_ATOMIC_U64)
uint64_t r = atomic_fetch_sub_u64(p, x, ATOMIC_RELAXED);
assert(r - x <= r);
#else
malloc_mutex_assert_owner(tsdn, &arena_stats->mtx);
*p -= x;
assert(*p + x >= *p);
#endif
}
static inline void
arena_stats_accum_u64(arena_stats_u64_t *dst, uint64_t src) {
#if defined(JEMALLOC_ATOMIC_U64)
uint64_t cur_dst = atomic_load_u64(dst, ATOMIC_RELAXED);
atomic_store_u64(dst, src + cur_dst, ATOMIC_RELAXED);
#else
*dst += src;
#endif
}
static inline size_t
arena_stats_read_zu(tsdn_t *tsdn, arena_stats_t *arena_stats,
atomic_zu_t *p) {
#if defined(JEMALLOC_ATOMIC_U64)
return atomic_load_zu(p, ATOMIC_RELAXED);
#else
malloc_mutex_assert_owner(tsdn, &arena_stats->mtx);
return atomic_load_zu(p, ATOMIC_RELAXED);
#endif
}
static inline void
arena_stats_add_zu(tsdn_t *tsdn, arena_stats_t *arena_stats,
atomic_zu_t *p, size_t x) {
#if defined(JEMALLOC_ATOMIC_U64)
atomic_fetch_add_zu(p, x, ATOMIC_RELAXED);
#else
malloc_mutex_assert_owner(tsdn, &arena_stats->mtx);
size_t cur = atomic_load_zu(p, ATOMIC_RELAXED);
atomic_store_zu(p, cur + x, ATOMIC_RELAXED);
#endif
}
static inline void
arena_stats_sub_zu(tsdn_t *tsdn, arena_stats_t *arena_stats,
atomic_zu_t *p, size_t x) {
#if defined(JEMALLOC_ATOMIC_U64)
size_t r = atomic_fetch_sub_zu(p, x, ATOMIC_RELAXED);
assert(r - x <= r);
#else
malloc_mutex_assert_owner(tsdn, &arena_stats->mtx);
size_t cur = atomic_load_zu(p, ATOMIC_RELAXED);
atomic_store_zu(p, cur - x, ATOMIC_RELAXED);
#endif
}
static inline void
arena_stats_accum_zu(atomic_zu_t *dst, size_t src) {
size_t cur_dst = atomic_load_zu(dst, ATOMIC_RELAXED);
atomic_store_zu(dst, src + cur_dst, ATOMIC_RELAXED);
}
static inline void
arena_stats_large_flush_nrequests_add(tsdn_t *tsdn, arena_stats_t *arena_stats,
szind_t szind, uint64_t nrequests) {
arena_stats_lock(tsdn, arena_stats);
arena_stats_large_t *lstats = &arena_stats->lstats[szind - SC_NBINS];
arena_stats_add_u64(tsdn, arena_stats, &lstats->nrequests, nrequests);
arena_stats_add_u64(tsdn, arena_stats, &lstats->nflushes, 1);
arena_stats_unlock(tsdn, arena_stats);
}
static inline void
arena_stats_mapped_add(tsdn_t *tsdn, arena_stats_t *arena_stats, size_t size) {
arena_stats_lock(tsdn, arena_stats);
arena_stats_add_zu(tsdn, arena_stats, &arena_stats->mapped, size);
arena_stats_unlock(tsdn, arena_stats);
}
