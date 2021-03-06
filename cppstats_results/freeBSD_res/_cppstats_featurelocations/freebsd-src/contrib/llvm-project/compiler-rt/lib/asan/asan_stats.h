











#if !defined(ASAN_STATS_H)
#define ASAN_STATS_H

#include "asan_allocator.h"
#include "asan_internal.h"

namespace __asan {




struct AsanStats {


uptr mallocs;
uptr malloced;
uptr malloced_redzones;
uptr frees;
uptr freed;
uptr real_frees;
uptr really_freed;
uptr reallocs;
uptr realloced;
uptr mmaps;
uptr mmaped;
uptr munmaps;
uptr munmaped;
uptr malloc_large;
uptr malloced_by_size[kNumberOfSizeClasses];


explicit AsanStats(LinkerInitialized) { }

AsanStats();

void Print();
void Clear();
void MergeFrom(const AsanStats *stats);
};



AsanStats &GetCurrentThreadStats();

void FlushToDeadThreadStats(AsanStats *stats);


struct AsanMallocStats {
uptr blocks_in_use;
uptr size_in_use;
uptr max_size_in_use;
uptr size_allocated;
};

void FillMallocStatistics(AsanMallocStats *malloc_stats);

}

#endif
