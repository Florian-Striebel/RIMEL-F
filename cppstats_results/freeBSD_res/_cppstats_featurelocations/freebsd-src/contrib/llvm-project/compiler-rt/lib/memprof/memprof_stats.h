











#if !defined(MEMPROF_STATS_H)
#define MEMPROF_STATS_H

#include "memprof_allocator.h"
#include "memprof_internal.h"

namespace __memprof {




struct MemprofStats {


uptr mallocs;
uptr malloced;
uptr malloced_overhead;
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


explicit MemprofStats(LinkerInitialized) {}

MemprofStats();

void Print();
void Clear();
void MergeFrom(const MemprofStats *stats);
};



MemprofStats &GetCurrentThreadStats();

void FlushToDeadThreadStats(MemprofStats *stats);

}

#endif
