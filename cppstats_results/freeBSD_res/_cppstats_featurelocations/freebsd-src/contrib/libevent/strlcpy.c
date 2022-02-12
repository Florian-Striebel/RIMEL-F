




























#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$OpenBSD: strlcpy.c,v 1.5 2001/05/13 15:40:16 deraadt Exp $";
#endif

#include "event2/event-config.h"
#include "evconfig-private.h"

#include <sys/types.h>

#if !defined(EVENT__HAVE_STRLCPY)
#include "strlcpy-internal.h"






size_t
event_strlcpy_(dst, src, siz)
char *dst;
const char *src;
size_t siz;
{
register char *d = dst;
register const char *s = src;
register size_t n = siz;


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

return (s - src - 1);
}
#endif
