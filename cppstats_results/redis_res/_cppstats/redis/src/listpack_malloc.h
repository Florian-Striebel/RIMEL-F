#include "zmalloc.h"
#define lp_malloc zmalloc
#define lp_realloc zrealloc
#define lp_free zfree
#define lp_malloc_size zmalloc_usable_size
