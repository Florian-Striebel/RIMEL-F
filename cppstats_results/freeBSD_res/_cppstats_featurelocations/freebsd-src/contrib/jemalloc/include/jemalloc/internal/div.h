#if !defined(JEMALLOC_INTERNAL_DIV_H)
#define JEMALLOC_INTERNAL_DIV_H

#include "jemalloc/internal/assert.h"










typedef struct div_info_s div_info_t;
struct div_info_s {
uint32_t magic;
#if defined(JEMALLOC_DEBUG)
size_t d;
#endif
};

void div_init(div_info_t *div_info, size_t divisor);

static inline size_t
div_compute(div_info_t *div_info, size_t n) {
assert(n <= (uint32_t)-1);






size_t i = ((uint64_t)n * (uint64_t)div_info->magic) >> 32;
#if defined(JEMALLOC_DEBUG)
assert(i * div_info->d == n);
#endif
return i;
}

#endif
