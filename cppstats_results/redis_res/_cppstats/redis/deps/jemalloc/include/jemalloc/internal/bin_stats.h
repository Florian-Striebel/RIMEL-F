#include "jemalloc/internal/mutex_prof.h"
typedef struct bin_stats_s bin_stats_t;
struct bin_stats_s {
uint64_t nmalloc;
uint64_t ndalloc;
uint64_t nrequests;
size_t curregs;
uint64_t nfills;
uint64_t nflushes;
uint64_t nslabs;
uint64_t reslabs;
size_t curslabs;
size_t nonfull_slabs;
mutex_prof_data_t mutex_data;
};
