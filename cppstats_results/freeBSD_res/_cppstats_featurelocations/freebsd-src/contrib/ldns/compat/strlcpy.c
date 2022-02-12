


















#include <ldns/config.h>
#if !defined(HAVE_STRLCPY)

#include <sys/types.h>
#include <string.h>






size_t
strlcpy(char *dst, const char *src, size_t siz)
{
char *d = dst;
const char *s = src;
size_t n = siz;


if (n != 0 && --n != 0) {
do {
if ((*d++ = *s++) == 0)
break;
} while (--n != 0);
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
