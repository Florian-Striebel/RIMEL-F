#include "jemalloc/internal/bin_stats.h"
#include "jemalloc/internal/bin_types.h"
#include "jemalloc/internal/extent_types.h"
#include "jemalloc/internal/extent_structs.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/sc.h"
typedef struct bin_info_s bin_info_t;
struct bin_info_s {
size_t reg_size;
size_t slab_size;
uint32_t nregs;
uint32_t n_shards;
bitmap_info_t bitmap_info;
};
extern bin_info_t bin_infos[SC_NBINS];
typedef struct bin_s bin_t;
struct bin_s {
malloc_mutex_t lock;
extent_t *slabcur;
extent_heap_t slabs_nonfull;
extent_list_t slabs_full;
bin_stats_t stats;
};
typedef struct bins_s bins_t;
struct bins_s {
bin_t *bin_shards;
};
void bin_shard_sizes_boot(unsigned bin_shards[SC_NBINS]);
bool bin_update_shard_size(unsigned bin_shards[SC_NBINS], size_t start_size,
size_t end_size, size_t nshards);
void bin_boot(sc_data_t *sc_data, unsigned bin_shard_sizes[SC_NBINS]);
bool bin_init(bin_t *bin);
void bin_prefork(tsdn_t *tsdn, bin_t *bin);
void bin_postfork_parent(tsdn_t *tsdn, bin_t *bin);
void bin_postfork_child(tsdn_t *tsdn, bin_t *bin);
static inline void
bin_stats_merge(tsdn_t *tsdn, bin_stats_t *dst_bin_stats, bin_t *bin) {
malloc_mutex_lock(tsdn, &bin->lock);
malloc_mutex_prof_accum(tsdn, &dst_bin_stats->mutex_data, &bin->lock);
dst_bin_stats->nmalloc += bin->stats.nmalloc;
dst_bin_stats->ndalloc += bin->stats.ndalloc;
dst_bin_stats->nrequests += bin->stats.nrequests;
dst_bin_stats->curregs += bin->stats.curregs;
dst_bin_stats->nfills += bin->stats.nfills;
dst_bin_stats->nflushes += bin->stats.nflushes;
dst_bin_stats->nslabs += bin->stats.nslabs;
dst_bin_stats->reslabs += bin->stats.reslabs;
dst_bin_stats->curslabs += bin->stats.curslabs;
dst_bin_stats->nonfull_slabs += bin->stats.nonfull_slabs;
malloc_mutex_unlock(tsdn, &bin->lock);
}
