






#include "openbsd-compat.h"

#if !defined(HAVE_EXPLICIT_BZERO) && !defined(_WIN32)

#include <string.h>





#if defined(HAVE_MEMSET_S)

void
explicit_bzero(void *p, size_t n)
{
if (n == 0)
return;
(void)memset_s(p, n, 0, n);
}

#else





static void (* volatile ssh_bzero)(void *, size_t) = bzero;

void
explicit_bzero(void *p, size_t n)
{
if (n == 0)
return;





#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
memset(p, 0, n);
#endif
#endif

ssh_bzero(p, n);
}

#endif

#endif
