#if !defined(JEMALLOC_INTERNAL_BACKGROUND_THREAD_STRUCTS_H)
#define JEMALLOC_INTERNAL_BACKGROUND_THREAD_STRUCTS_H



#if defined(JEMALLOC_BACKGROUND_THREAD) || defined(JEMALLOC_LAZY_LOCK)
#define JEMALLOC_PTHREAD_CREATE_WRAPPER
#endif

#define BACKGROUND_THREAD_INDEFINITE_SLEEP UINT64_MAX
#define MAX_BACKGROUND_THREAD_LIMIT MALLOCX_ARENA_LIMIT
#define DEFAULT_NUM_BACKGROUND_THREAD 4

typedef enum {
background_thread_stopped,
background_thread_started,

background_thread_paused,
} background_thread_state_t;

struct background_thread_info_s {
#if defined(JEMALLOC_BACKGROUND_THREAD)

pthread_t thread;
pthread_cond_t cond;
#endif
malloc_mutex_t mtx;
background_thread_state_t state;

atomic_b_t indefinite_sleep;

nstime_t next_wakeup;






size_t npages_to_purge_new;

uint64_t tot_n_runs;

nstime_t tot_sleep_time;
};
typedef struct background_thread_info_s background_thread_info_t;

struct background_thread_stats_s {
size_t num_threads;
uint64_t num_runs;
nstime_t run_interval;
};
typedef struct background_thread_stats_s background_thread_stats_t;

#endif
