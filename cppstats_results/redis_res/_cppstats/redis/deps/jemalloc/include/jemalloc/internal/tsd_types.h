#define MALLOC_TSD_CLEANUPS_MAX 2
typedef struct tsd_s tsd_t;
typedef struct tsdn_s tsdn_t;
typedef bool (*malloc_tsd_cleanup_t)(void);
