#if !defined(HAVE_STRLCPY)





















#include "dfcompat.h"

#include <sys/types.h>
#include <string.h>






size_t
strlcpy(char *dst, const char *src, size_t siz)
{
char *d = dst;
const char *s = src;
size_t n = siz;


if (n != 0) {
while (--n != 0) {
if ((*d++ = *s++) == '\0')
break;
}
}


if (n == 0) {
if (siz != 0)
*d = '\0';
while (*s++)
;
}

return(s - src - 1);
}

#endif

#if !defined(HAVE_REALLOCF)





























#include <stdlib.h>

void *
reallocf(void *ptr, size_t size)
{
void *nptr;

nptr = realloc(ptr, size);
if (!nptr && ptr && size != 0)
free(ptr);
return (nptr);
}

#endif

#if !defined(HAVE_GETPROGNAME)

#if defined(__GLIBC__)

#include <errno.h>

const char *
getprogname(void)
{
return (program_invocation_short_name);
}

#else
#error "no getprogname implementation available"
#endif

#endif
