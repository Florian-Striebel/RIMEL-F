



































#define JEMALLOC_CKH_C_
#include "jemalloc/internal/jemalloc_preamble.h"

#include "jemalloc/internal/ckh.h"

#include "jemalloc/internal/jemalloc_internal_includes.h"

#include "jemalloc/internal/assert.h"
#include "jemalloc/internal/hash.h"
#include "jemalloc/internal/malloc_io.h"
#include "jemalloc/internal/prng.h"
#include "jemalloc/internal/util.h"




static bool ckh_grow(tsd_t *tsd, ckh_t *ckh);
static void ckh_shrink(tsd_t *tsd, ckh_t *ckh);







static size_t
ckh_bucket_search(ckh_t *ckh, size_t bucket, const void *key) {
ckhc_t *cell;
unsigned i;

for (i = 0; i < (ZU(1) << LG_CKH_BUCKET_CELLS); i++) {
cell = &ckh->tab[(bucket << LG_CKH_BUCKET_CELLS) + i];
if (cell->key != NULL && ckh->keycomp(key, cell->key)) {
return (bucket << LG_CKH_BUCKET_CELLS) + i;
}
}

return SIZE_T_MAX;
}




static size_t
ckh_isearch(ckh_t *ckh, const void *key) {
size_t hashes[2], bucket, cell;

assert(ckh != NULL);

ckh->hash(key, hashes);


bucket = hashes[0] & ((ZU(1) << ckh->lg_curbuckets) - 1);
cell = ckh_bucket_search(ckh, bucket, key);
if (cell != SIZE_T_MAX) {
return cell;
}


bucket = hashes[1] & ((ZU(1) << ckh->lg_curbuckets) - 1);
cell = ckh_bucket_search(ckh, bucket, key);
return cell;
}

static bool
ckh_try_bucket_insert(ckh_t *ckh, size_t bucket, const void *key,
const void *data) {
ckhc_t *cell;
unsigned offset, i;





offset = (unsigned)prng_lg_range_u64(&ckh->prng_state,
LG_CKH_BUCKET_CELLS);
for (i = 0; i < (ZU(1) << LG_CKH_BUCKET_CELLS); i++) {
cell = &ckh->tab[(bucket << LG_CKH_BUCKET_CELLS) +
((i + offset) & ((ZU(1) << LG_CKH_BUCKET_CELLS) - 1))];
if (cell->key == NULL) {
cell->key = key;
cell->data = data;
ckh->count++;
return false;
}
}

return true;
}







static bool
ckh_evict_reloc_insert(ckh_t *ckh, size_t argbucket, void const **argkey,
void const **argdata) {
const void *key, *data, *tkey, *tdata;
ckhc_t *cell;
size_t hashes[2], bucket, tbucket;
unsigned i;

bucket = argbucket;
key = *argkey;
data = *argdata;
while (true) {








i = (unsigned)prng_lg_range_u64(&ckh->prng_state,
LG_CKH_BUCKET_CELLS);
cell = &ckh->tab[(bucket << LG_CKH_BUCKET_CELLS) + i];
assert(cell->key != NULL);


tkey = cell->key; tdata = cell->data;
cell->key = key; cell->data = data;
key = tkey; data = tdata;

#if defined(CKH_COUNT)
ckh->nrelocs++;
#endif


ckh->hash(key, hashes);
tbucket = hashes[1] & ((ZU(1) << ckh->lg_curbuckets) - 1);
if (tbucket == bucket) {
tbucket = hashes[0] & ((ZU(1) << ckh->lg_curbuckets)
- 1);
















}

if (tbucket == argbucket) {
*argkey = key;
*argdata = data;
return true;
}

bucket = tbucket;
if (!ckh_try_bucket_insert(ckh, bucket, key, data)) {
return false;
}
}
}

static bool
ckh_try_insert(ckh_t *ckh, void const**argkey, void const**argdata) {
size_t hashes[2], bucket;
const void *key = *argkey;
const void *data = *argdata;

ckh->hash(key, hashes);


bucket = hashes[0] & ((ZU(1) << ckh->lg_curbuckets) - 1);
if (!ckh_try_bucket_insert(ckh, bucket, key, data)) {
return false;
}


bucket = hashes[1] & ((ZU(1) << ckh->lg_curbuckets) - 1);
if (!ckh_try_bucket_insert(ckh, bucket, key, data)) {
return false;
}




return ckh_evict_reloc_insert(ckh, bucket, argkey, argdata);
}





static bool
ckh_rebuild(ckh_t *ckh, ckhc_t *aTab) {
size_t count, i, nins;
const void *key, *data;

count = ckh->count;
ckh->count = 0;
for (i = nins = 0; nins < count; i++) {
if (aTab[i].key != NULL) {
key = aTab[i].key;
data = aTab[i].data;
if (ckh_try_insert(ckh, &key, &data)) {
ckh->count = count;
return true;
}
nins++;
}
}

return false;
}

static bool
ckh_grow(tsd_t *tsd, ckh_t *ckh) {
bool ret;
ckhc_t *tab, *ttab;
unsigned lg_prevbuckets, lg_curcells;

#if defined(CKH_COUNT)
ckh->ngrows++;
#endif






lg_prevbuckets = ckh->lg_curbuckets;
lg_curcells = ckh->lg_curbuckets + LG_CKH_BUCKET_CELLS;
while (true) {
size_t usize;

lg_curcells++;
usize = sz_sa2u(sizeof(ckhc_t) << lg_curcells, CACHELINE);
if (unlikely(usize == 0
|| usize > SC_LARGE_MAXCLASS)) {
ret = true;
goto label_return;
}
tab = (ckhc_t *)ipallocztm(tsd_tsdn(tsd), usize, CACHELINE,
true, NULL, true, arena_ichoose(tsd, NULL));
if (tab == NULL) {
ret = true;
goto label_return;
}

ttab = ckh->tab;
ckh->tab = tab;
tab = ttab;
ckh->lg_curbuckets = lg_curcells - LG_CKH_BUCKET_CELLS;

if (!ckh_rebuild(ckh, tab)) {
idalloctm(tsd_tsdn(tsd), tab, NULL, NULL, true, true);
break;
}


idalloctm(tsd_tsdn(tsd), ckh->tab, NULL, NULL, true, true);
ckh->tab = tab;
ckh->lg_curbuckets = lg_prevbuckets;
}

ret = false;
label_return:
return ret;
}

static void
ckh_shrink(tsd_t *tsd, ckh_t *ckh) {
ckhc_t *tab, *ttab;
size_t usize;
unsigned lg_prevbuckets, lg_curcells;





lg_prevbuckets = ckh->lg_curbuckets;
lg_curcells = ckh->lg_curbuckets + LG_CKH_BUCKET_CELLS - 1;
usize = sz_sa2u(sizeof(ckhc_t) << lg_curcells, CACHELINE);
if (unlikely(usize == 0 || usize > SC_LARGE_MAXCLASS)) {
return;
}
tab = (ckhc_t *)ipallocztm(tsd_tsdn(tsd), usize, CACHELINE, true, NULL,
true, arena_ichoose(tsd, NULL));
if (tab == NULL) {




return;
}

ttab = ckh->tab;
ckh->tab = tab;
tab = ttab;
ckh->lg_curbuckets = lg_curcells - LG_CKH_BUCKET_CELLS;

if (!ckh_rebuild(ckh, tab)) {
idalloctm(tsd_tsdn(tsd), tab, NULL, NULL, true, true);
#if defined(CKH_COUNT)
ckh->nshrinks++;
#endif
return;
}


idalloctm(tsd_tsdn(tsd), ckh->tab, NULL, NULL, true, true);
ckh->tab = tab;
ckh->lg_curbuckets = lg_prevbuckets;
#if defined(CKH_COUNT)
ckh->nshrinkfails++;
#endif
}

bool
ckh_new(tsd_t *tsd, ckh_t *ckh, size_t minitems, ckh_hash_t *hash,
ckh_keycomp_t *keycomp) {
bool ret;
size_t mincells, usize;
unsigned lg_mincells;

assert(minitems > 0);
assert(hash != NULL);
assert(keycomp != NULL);

#if defined(CKH_COUNT)
ckh->ngrows = 0;
ckh->nshrinks = 0;
ckh->nshrinkfails = 0;
ckh->ninserts = 0;
ckh->nrelocs = 0;
#endif
ckh->prng_state = 42;
ckh->count = 0;








assert(LG_CKH_BUCKET_CELLS > 0);
mincells = ((minitems + (3 - (minitems % 3))) / 3) << 2;
for (lg_mincells = LG_CKH_BUCKET_CELLS;
(ZU(1) << lg_mincells) < mincells;
lg_mincells++) {

}
ckh->lg_minbuckets = lg_mincells - LG_CKH_BUCKET_CELLS;
ckh->lg_curbuckets = lg_mincells - LG_CKH_BUCKET_CELLS;
ckh->hash = hash;
ckh->keycomp = keycomp;

usize = sz_sa2u(sizeof(ckhc_t) << lg_mincells, CACHELINE);
if (unlikely(usize == 0 || usize > SC_LARGE_MAXCLASS)) {
ret = true;
goto label_return;
}
ckh->tab = (ckhc_t *)ipallocztm(tsd_tsdn(tsd), usize, CACHELINE, true,
NULL, true, arena_ichoose(tsd, NULL));
if (ckh->tab == NULL) {
ret = true;
goto label_return;
}

ret = false;
label_return:
return ret;
}

void
ckh_delete(tsd_t *tsd, ckh_t *ckh) {
assert(ckh != NULL);

#if defined(CKH_VERBOSE)
malloc_printf(
"%s(%p): ngrows: %"FMTu64", nshrinks: %"FMTu64","
" nshrinkfails: %"FMTu64", ninserts: %"FMTu64","
" nrelocs: %"FMTu64"\n", __func__, ckh,
(unsigned long long)ckh->ngrows,
(unsigned long long)ckh->nshrinks,
(unsigned long long)ckh->nshrinkfails,
(unsigned long long)ckh->ninserts,
(unsigned long long)ckh->nrelocs);
#endif

idalloctm(tsd_tsdn(tsd), ckh->tab, NULL, NULL, true, true);
if (config_debug) {
memset(ckh, JEMALLOC_FREE_JUNK, sizeof(ckh_t));
}
}

size_t
ckh_count(ckh_t *ckh) {
assert(ckh != NULL);

return ckh->count;
}

bool
ckh_iter(ckh_t *ckh, size_t *tabind, void **key, void **data) {
size_t i, ncells;

for (i = *tabind, ncells = (ZU(1) << (ckh->lg_curbuckets +
LG_CKH_BUCKET_CELLS)); i < ncells; i++) {
if (ckh->tab[i].key != NULL) {
if (key != NULL) {
*key = (void *)ckh->tab[i].key;
}
if (data != NULL) {
*data = (void *)ckh->tab[i].data;
}
*tabind = i + 1;
return false;
}
}

return true;
}

bool
ckh_insert(tsd_t *tsd, ckh_t *ckh, const void *key, const void *data) {
bool ret;

assert(ckh != NULL);
assert(ckh_search(ckh, key, NULL, NULL));

#if defined(CKH_COUNT)
ckh->ninserts++;
#endif

while (ckh_try_insert(ckh, &key, &data)) {
if (ckh_grow(tsd, ckh)) {
ret = true;
goto label_return;
}
}

ret = false;
label_return:
return ret;
}

bool
ckh_remove(tsd_t *tsd, ckh_t *ckh, const void *searchkey, void **key,
void **data) {
size_t cell;

assert(ckh != NULL);

cell = ckh_isearch(ckh, searchkey);
if (cell != SIZE_T_MAX) {
if (key != NULL) {
*key = (void *)ckh->tab[cell].key;
}
if (data != NULL) {
*data = (void *)ckh->tab[cell].data;
}
ckh->tab[cell].key = NULL;
ckh->tab[cell].data = NULL;

ckh->count--;

if (ckh->count < (ZU(1) << (ckh->lg_curbuckets
+ LG_CKH_BUCKET_CELLS - 2)) && ckh->lg_curbuckets
> ckh->lg_minbuckets) {

ckh_shrink(tsd, ckh);
}

return false;
}

return true;
}

bool
ckh_search(ckh_t *ckh, const void *searchkey, void **key, void **data) {
size_t cell;

assert(ckh != NULL);

cell = ckh_isearch(ckh, searchkey);
if (cell != SIZE_T_MAX) {
if (key != NULL) {
*key = (void *)ckh->tab[cell].key;
}
if (data != NULL) {
*data = (void *)ckh->tab[cell].data;
}
return false;
}

return true;
}

void
ckh_string_hash(const void *key, size_t r_hash[2]) {
hash(key, strlen((const char *)key), 0x94122f33U, r_hash);
}

bool
ckh_string_keycomp(const void *k1, const void *k2) {
assert(k1 != NULL);
assert(k2 != NULL);

return !strcmp((char *)k1, (char *)k2);
}

void
ckh_pointer_hash(const void *key, size_t r_hash[2]) {
union {
const void *v;
size_t i;
} u;

assert(sizeof(u.v) == sizeof(u.i));
u.v = key;
hash(&u.i, sizeof(u.i), 0xd983396eU, r_hash);
}

bool
ckh_pointer_keycomp(const void *k1, const void *k2) {
return (k1 == k2);
}
