#include "jemalloc/internal/bitmap.h"
struct arena_slab_data_s {
bitmap_t bitmap[BITMAP_GROUPS_MAX];
};
