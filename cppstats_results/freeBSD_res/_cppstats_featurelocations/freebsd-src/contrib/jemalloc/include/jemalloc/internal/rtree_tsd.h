#if !defined(JEMALLOC_INTERNAL_RTREE_CTX_H)
#define JEMALLOC_INTERNAL_RTREE_CTX_H


















#define RTREE_CTX_LG_NCACHE 4
#define RTREE_CTX_NCACHE (1 << RTREE_CTX_LG_NCACHE)
#define RTREE_CTX_NCACHE_L2 8





#define RTREE_CTX_ZERO_INITIALIZER {{{0, 0}}, {{0, 0}}}


typedef struct rtree_leaf_elm_s rtree_leaf_elm_t;

typedef struct rtree_ctx_cache_elm_s rtree_ctx_cache_elm_t;
struct rtree_ctx_cache_elm_s {
uintptr_t leafkey;
rtree_leaf_elm_t *leaf;
};

typedef struct rtree_ctx_s rtree_ctx_t;
struct rtree_ctx_s {

rtree_ctx_cache_elm_t cache[RTREE_CTX_NCACHE];

rtree_ctx_cache_elm_t l2_cache[RTREE_CTX_NCACHE_L2];
};

void rtree_ctx_data_init(rtree_ctx_t *ctx);

#endif
