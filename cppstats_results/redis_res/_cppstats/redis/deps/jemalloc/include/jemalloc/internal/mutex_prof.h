#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/nstime.h"
#include "jemalloc/internal/tsd_types.h"
#define MUTEX_PROF_GLOBAL_MUTEXES OP(background_thread) OP(ctl) OP(prof)
typedef enum {
#define OP(mtx) global_prof_mutex_##mtx,
MUTEX_PROF_GLOBAL_MUTEXES
#undef OP
mutex_prof_num_global_mutexes
} mutex_prof_global_ind_t;
#define MUTEX_PROF_ARENA_MUTEXES OP(large) OP(extent_avail) OP(extents_dirty) OP(extents_muzzy) OP(extents_retained) OP(decay_dirty) OP(decay_muzzy) OP(base) OP(tcache_list)
typedef enum {
#define OP(mtx) arena_prof_mutex_##mtx,
MUTEX_PROF_ARENA_MUTEXES
#undef OP
mutex_prof_num_arena_mutexes
} mutex_prof_arena_ind_t;
#define MUTEX_PROF_UINT64_COUNTERS OP(num_ops, uint64_t, "n_lock_ops", false, num_ops) OP(num_ops_ps, uint64_t, "(#/sec)", true, num_ops) OP(num_wait, uint64_t, "n_waiting", false, num_wait) OP(num_wait_ps, uint64_t, "(#/sec)", true, num_wait) OP(num_spin_acq, uint64_t, "n_spin_acq", false, num_spin_acq) OP(num_spin_acq_ps, uint64_t, "(#/sec)", true, num_spin_acq) OP(num_owner_switch, uint64_t, "n_owner_switch", false, num_owner_switch) OP(num_owner_switch_ps, uint64_t, "(#/sec)", true, num_owner_switch) OP(total_wait_time, uint64_t, "total_wait_ns", false, total_wait_time) OP(total_wait_time_ps, uint64_t, "(#/sec)", true, total_wait_time) OP(max_wait_time, uint64_t, "max_wait_ns", false, max_wait_time)
#define MUTEX_PROF_UINT32_COUNTERS OP(max_num_thds, uint32_t, "max_n_thds", false, max_num_thds)
#define MUTEX_PROF_COUNTERS MUTEX_PROF_UINT64_COUNTERS MUTEX_PROF_UINT32_COUNTERS
#define OP(counter, type, human, derived, base_counter) mutex_counter_##counter,
#define COUNTER_ENUM(counter_list, t) typedef enum { counter_list mutex_prof_num_##t##_counters } mutex_prof_##t##_counter_ind_t;
COUNTER_ENUM(MUTEX_PROF_UINT64_COUNTERS, uint64_t)
COUNTER_ENUM(MUTEX_PROF_UINT32_COUNTERS, uint32_t)
#undef COUNTER_ENUM
#undef OP
typedef struct {
nstime_t tot_wait_time;
nstime_t max_wait_time;
uint64_t n_wait_times;
uint64_t n_spin_acquired;
uint32_t max_n_thds;
atomic_u32_t n_waiting_thds;
uint64_t n_owner_switches;
tsdn_t *prev_owner;
uint64_t n_lock_ops;
} mutex_prof_data_t;
