


















#if !defined(_KERNEL) && !defined(_STANDALONE)
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: strlcpy.c,v 1.2 2015/01/22 03:48:07 christos Exp $");
#endif

#if defined(_LIBC)
#include "namespace.h"
#endif
#include <sys/types.h>
#include <assert.h>
#include <string.h>

#if defined(_LIBC)
#if defined(__weak_alias)
__weak_alias(strlcpy, _strlcpy)
#endif
#endif
#else
#include <lib/libkern/libkern.h>
#endif


#if !HAVE_STRLCPY





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
