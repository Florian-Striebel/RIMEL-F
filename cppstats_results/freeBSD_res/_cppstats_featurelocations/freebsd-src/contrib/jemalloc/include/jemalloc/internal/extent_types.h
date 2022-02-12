#if !defined(JEMALLOC_INTERNAL_EXTENT_TYPES_H)
#define JEMALLOC_INTERNAL_EXTENT_TYPES_H

typedef struct extent_s extent_t;
typedef struct extents_s extents_t;

typedef struct extent_util_stats_s extent_util_stats_t;
typedef struct extent_util_stats_verbose_s extent_util_stats_verbose_t;

#define EXTENT_HOOKS_INITIALIZER NULL





#define LG_EXTENT_MAX_ACTIVE_FIT_DEFAULT 6

typedef enum {
EXTENT_NOT_HEAD,
EXTENT_IS_HEAD
} extent_head_state_t;

#endif
