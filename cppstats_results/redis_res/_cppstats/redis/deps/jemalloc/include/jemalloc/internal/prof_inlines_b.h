#include "jemalloc/internal/safety_check.h"
#include "jemalloc/internal/sz.h"
JEMALLOC_ALWAYS_INLINE bool
prof_gdump_get_unlocked(void) {
return prof_gdump_val;
}
JEMALLOC_ALWAYS_INLINE prof_tdata_t *
prof_tdata_get(tsd_t *tsd, bool create) {
prof_tdata_t *tdata;
cassert(config_prof);
tdata = tsd_prof_tdata_get(tsd);
if (create) {
if (unlikely(tdata == NULL)) {
if (tsd_nominal(tsd)) {
tdata = prof_tdata_init(tsd);
tsd_prof_tdata_set(tsd, tdata);
}
} else if (unlikely(tdata->expired)) {
tdata = prof_tdata_reinit(tsd, tdata);
tsd_prof_tdata_set(tsd, tdata);
}
assert(tdata == NULL || tdata->attached);
}
return tdata;
}
JEMALLOC_ALWAYS_INLINE prof_tctx_t *
prof_tctx_get(tsdn_t *tsdn, const void *ptr, alloc_ctx_t *alloc_ctx) {
cassert(config_prof);
assert(ptr != NULL);
return arena_prof_tctx_get(tsdn, ptr, alloc_ctx);
}
JEMALLOC_ALWAYS_INLINE void
prof_tctx_set(tsdn_t *tsdn, const void *ptr, size_t usize,
alloc_ctx_t *alloc_ctx, prof_tctx_t *tctx) {
cassert(config_prof);
assert(ptr != NULL);
arena_prof_tctx_set(tsdn, ptr, usize, alloc_ctx, tctx);
}
JEMALLOC_ALWAYS_INLINE void
prof_tctx_reset(tsdn_t *tsdn, const void *ptr, prof_tctx_t *tctx) {
cassert(config_prof);
assert(ptr != NULL);
arena_prof_tctx_reset(tsdn, ptr, tctx);
}
JEMALLOC_ALWAYS_INLINE nstime_t
prof_alloc_time_get(tsdn_t *tsdn, const void *ptr, alloc_ctx_t *alloc_ctx) {
cassert(config_prof);
assert(ptr != NULL);
return arena_prof_alloc_time_get(tsdn, ptr, alloc_ctx);
}
JEMALLOC_ALWAYS_INLINE void
prof_alloc_time_set(tsdn_t *tsdn, const void *ptr, alloc_ctx_t *alloc_ctx,
nstime_t t) {
cassert(config_prof);
assert(ptr != NULL);
arena_prof_alloc_time_set(tsdn, ptr, alloc_ctx, t);
}
JEMALLOC_ALWAYS_INLINE bool
prof_sample_check(tsd_t *tsd, size_t usize, bool update) {
ssize_t check = update ? 0 : usize;
int64_t bytes_until_sample = tsd_bytes_until_sample_get(tsd);
if (update) {
bytes_until_sample -= usize;
if (tsd_nominal(tsd)) {
tsd_bytes_until_sample_set(tsd, bytes_until_sample);
}
}
if (likely(bytes_until_sample >= check)) {
return true;
}
return false;
}
JEMALLOC_ALWAYS_INLINE bool
prof_sample_accum_update(tsd_t *tsd, size_t usize, bool update,
prof_tdata_t **tdata_out) {
prof_tdata_t *tdata;
cassert(config_prof);
if (likely(prof_sample_check(tsd, usize, update))) {
return true;
}
bool booted = tsd_prof_tdata_get(tsd);
tdata = prof_tdata_get(tsd, true);
if (unlikely((uintptr_t)tdata <= (uintptr_t)PROF_TDATA_STATE_MAX)) {
tdata = NULL;
}
if (tdata_out != NULL) {
*tdata_out = tdata;
}
if (unlikely(tdata == NULL)) {
return true;
}
if (!booted && prof_sample_check(tsd, usize, update)) {
return true;
}
if (tsd_reentrancy_level_get(tsd) > 0) {
return true;
}
if (update) {
prof_sample_threshold_update(tdata);
}
return !tdata->active;
}
JEMALLOC_ALWAYS_INLINE prof_tctx_t *
prof_alloc_prep(tsd_t *tsd, size_t usize, bool prof_active, bool update) {
prof_tctx_t *ret;
prof_tdata_t *tdata;
prof_bt_t bt;
assert(usize == sz_s2u(usize));
if (!prof_active || likely(prof_sample_accum_update(tsd, usize, update,
&tdata))) {
ret = (prof_tctx_t *)(uintptr_t)1U;
} else {
bt_init(&bt, tdata->vec);
prof_backtrace(&bt);
ret = prof_lookup(tsd, &bt);
}
return ret;
}
JEMALLOC_ALWAYS_INLINE void
prof_malloc(tsdn_t *tsdn, const void *ptr, size_t usize, alloc_ctx_t *alloc_ctx,
prof_tctx_t *tctx) {
cassert(config_prof);
assert(ptr != NULL);
assert(usize == isalloc(tsdn, ptr));
if (unlikely((uintptr_t)tctx > (uintptr_t)1U)) {
prof_malloc_sample_object(tsdn, ptr, usize, tctx);
} else {
prof_tctx_set(tsdn, ptr, usize, alloc_ctx,
(prof_tctx_t *)(uintptr_t)1U);
}
}
JEMALLOC_ALWAYS_INLINE void
prof_realloc(tsd_t *tsd, const void *ptr, size_t usize, prof_tctx_t *tctx,
bool prof_active, bool updated, const void *old_ptr, size_t old_usize,
prof_tctx_t *old_tctx) {
bool sampled, old_sampled, moved;
cassert(config_prof);
assert(ptr != NULL || (uintptr_t)tctx <= (uintptr_t)1U);
if (prof_active && !updated && ptr != NULL) {
assert(usize == isalloc(tsd_tsdn(tsd), ptr));
if (prof_sample_accum_update(tsd, usize, true, NULL)) {
prof_alloc_rollback(tsd, tctx, true);
tctx = (prof_tctx_t *)(uintptr_t)1U;
}
}
sampled = ((uintptr_t)tctx > (uintptr_t)1U);
old_sampled = ((uintptr_t)old_tctx > (uintptr_t)1U);
moved = (ptr != old_ptr);
if (unlikely(sampled)) {
prof_malloc_sample_object(tsd_tsdn(tsd), ptr, usize, tctx);
} else if (moved) {
prof_tctx_set(tsd_tsdn(tsd), ptr, usize, NULL,
(prof_tctx_t *)(uintptr_t)1U);
} else if (unlikely(old_sampled)) {
prof_tctx_reset(tsd_tsdn(tsd), ptr, tctx);
} else {
assert((uintptr_t)prof_tctx_get(tsd_tsdn(tsd), ptr, NULL) ==
(uintptr_t)1U);
}
if (unlikely(old_sampled)) {
prof_free_sampled_object(tsd, ptr, old_usize, old_tctx);
}
}
JEMALLOC_ALWAYS_INLINE void
prof_free(tsd_t *tsd, const void *ptr, size_t usize, alloc_ctx_t *alloc_ctx) {
prof_tctx_t *tctx = prof_tctx_get(tsd_tsdn(tsd), ptr, alloc_ctx);
cassert(config_prof);
assert(usize == isalloc(tsd_tsdn(tsd), ptr));
if (unlikely((uintptr_t)tctx > (uintptr_t)1U)) {
prof_free_sampled_object(tsd, ptr, usize, tctx);
}
}
