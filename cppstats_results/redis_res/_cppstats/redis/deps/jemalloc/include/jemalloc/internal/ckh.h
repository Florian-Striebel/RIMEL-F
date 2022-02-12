#include "jemalloc/internal/tsd.h"
#define LG_CKH_BUCKET_CELLS (LG_CACHELINE - LG_SIZEOF_PTR - 1)
typedef void ckh_hash_t (const void *, size_t[2]);
typedef bool ckh_keycomp_t (const void *, const void *);
typedef struct {
const void *key;
const void *data;
} ckhc_t;
typedef struct {
#if defined(CKH_COUNT)
uint64_t ngrows;
uint64_t nshrinks;
uint64_t nshrinkfails;
uint64_t ninserts;
uint64_t nrelocs;
#endif
uint64_t prng_state;
size_t count;
unsigned lg_minbuckets;
unsigned lg_curbuckets;
ckh_hash_t *hash;
ckh_keycomp_t *keycomp;
ckhc_t *tab;
} ckh_t;
bool ckh_new(tsd_t *tsd, ckh_t *ckh, size_t minitems, ckh_hash_t *hash,
ckh_keycomp_t *keycomp);
void ckh_delete(tsd_t *tsd, ckh_t *ckh);
size_t ckh_count(ckh_t *ckh);
bool ckh_iter(ckh_t *ckh, size_t *tabind, void **key, void **data);
bool ckh_insert(tsd_t *tsd, ckh_t *ckh, const void *key, const void *data);
bool ckh_remove(tsd_t *tsd, ckh_t *ckh, const void *searchkey, void **key,
void **data);
bool ckh_search(ckh_t *ckh, const void *searchkey, void **key, void **data);
void ckh_string_hash(const void *key, size_t r_hash[2]);
bool ckh_string_keycomp(const void *k1, const void *k2);
void ckh_pointer_hash(const void *key, size_t r_hash[2]);
bool ckh_pointer_keycomp(const void *k1, const void *k2);
