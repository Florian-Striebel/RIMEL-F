#include "jemalloc/internal/jemalloc_preamble.h"
#include "jemalloc/internal/div.h"
#include "jemalloc/internal/assert.h"
void
div_init(div_info_t *div_info, size_t d) {
assert(d != 0);
assert(d != 1);
uint64_t two_to_k = ((uint64_t)1 << 32);
uint32_t magic = (uint32_t)(two_to_k / d);
if (two_to_k % d != 0) {
magic++;
}
div_info->magic = magic;
#if defined(JEMALLOC_DEBUG)
div_info->d = d;
#endif
}
