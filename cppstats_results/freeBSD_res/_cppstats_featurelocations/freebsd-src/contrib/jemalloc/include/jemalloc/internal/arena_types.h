#if !defined(JEMALLOC_INTERNAL_ARENA_TYPES_H)
#define JEMALLOC_INTERNAL_ARENA_TYPES_H

#include "jemalloc/internal/sc.h"


#define LG_SLAB_MAXREGS (LG_PAGE - SC_LG_TINY_MIN)
#define SLAB_MAXREGS (1U << LG_SLAB_MAXREGS)


#define DIRTY_DECAY_MS_DEFAULT ZD(10 * 1000)
#define MUZZY_DECAY_MS_DEFAULT (0)

#define DECAY_NTICKS_PER_UPDATE 1000

typedef struct arena_slab_data_s arena_slab_data_t;
typedef struct arena_decay_s arena_decay_t;
typedef struct arena_s arena_t;
typedef struct arena_tdata_s arena_tdata_t;
typedef struct alloc_ctx_s alloc_ctx_t;

typedef enum {
percpu_arena_mode_names_base = 0,





percpu_arena_uninit = 0,
per_phycpu_arena_uninit = 1,


percpu_arena_disabled = 2,

percpu_arena_mode_names_limit = 3,
percpu_arena_mode_enabled_base = 3,

percpu_arena = 3,
per_phycpu_arena = 4
} percpu_arena_mode_t;

#define PERCPU_ARENA_ENABLED(m) ((m) >= percpu_arena_mode_enabled_base)
#define PERCPU_ARENA_DEFAULT percpu_arena_disabled





#define OVERSIZE_THRESHOLD_DEFAULT (8 << 20)

#endif
