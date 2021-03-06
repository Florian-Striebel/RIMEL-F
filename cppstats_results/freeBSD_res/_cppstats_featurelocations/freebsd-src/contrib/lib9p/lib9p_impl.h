


























#if !defined(LIB9P_LIB9P_IMPL_H)
#define LIB9P_LIB9P_IMPL_H

#include <stdio.h>
#include <stdlib.h>

#if !defined(_KERNEL)
static inline void *
l9p_malloc(size_t size)
{
void *r = malloc(size);

if (r == NULL) {
fprintf(stderr, "cannot allocate %zd bytes: out of memory\n",
size);
abort();
}

return (r);
}

static inline void *
l9p_calloc(size_t n, size_t size)
{
void *r = calloc(n, size);

if (r == NULL) {
fprintf(stderr, "cannot allocate %zd bytes: out of memory\n",
n * size);
abort();
}

return (r);
}

static inline void *
l9p_realloc(void *ptr, size_t newsize)
{
void *r = realloc(ptr, newsize);

if (r == NULL) {
fprintf(stderr, "cannot allocate %zd bytes: out of memory\n",
newsize);
abort();
}

return (r);
}
#endif

#endif
