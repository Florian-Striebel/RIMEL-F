


















#if !defined(_KERNEL) && !defined(_STANDALONE)
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: strlcat.c,v 1.2 2015/01/22 03:48:07 christos Exp $");
#endif

#if defined(_LIBC)
#include "namespace.h"
#endif
#include <sys/types.h>
#include <assert.h>
#include <string.h>

#if defined(_LIBC)
#if defined(__weak_alias)
__weak_alias(strlcat, _strlcat)
#endif
#endif

#else
#include <lib/libkern/libkern.h>
#endif

#if !HAVE_STRLCAT







size_t
strlcat(char *dst, const char *src, size_t siz)
{
#if 1
char *d = dst;
const char *s = src;
size_t n = siz;
size_t dlen;


while (n-- != 0 && *d != '\0')
d++;
dlen = d - dst;
n = siz - dlen;

if (n == 0)
return(dlen + strlen(s));
while (*s != '\0') {
if (n != 1) {
*d++ = *s;
n--;
}
s++;
}
*d = '\0';

return(dlen + (s - src));
#else




size_t dlen = strnlen(dst, siz);





return dlen + strlcpy(dst + dlen, src, siz - dlen);
#endif
}
#endif
