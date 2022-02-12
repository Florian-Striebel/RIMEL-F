















#include "openbsd-compat.h"

#if !defined(HAVE_FREEZERO)

void
freezero(void *ptr, size_t sz)
{
if (ptr == NULL)
return;
explicit_bzero(ptr, sz);
free(ptr);
}

#endif
