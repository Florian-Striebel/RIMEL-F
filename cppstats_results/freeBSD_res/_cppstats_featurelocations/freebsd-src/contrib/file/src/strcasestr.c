

































#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: strcasestr.c,v 1.3 2005/11/29 03:12:00 christos Exp $");
__RCSID("$NetBSD: strncasecmp.c,v 1.2 2007/06/04 18:19:27 christos Exp $");
#endif

#include "file.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>

static int
_strncasecmp(const char *s1, const char *s2, size_t n)
{
if (n != 0) {
const unsigned char *us1 = (const unsigned char *)s1,
*us2 = (const unsigned char *)s2;

do {
if (tolower(*us1) != tolower(*us2++))
return tolower(*us1) - tolower(*--us2);
if (*us1++ == '\0')
break;
} while (--n != 0);
}
return 0;
}




char *
strcasestr(const char *s, const char *find)
{
char c, sc;
size_t len;

if ((c = *find++) != 0) {
c = tolower((unsigned char)c);
len = strlen(find);
do {
do {
if ((sc = *s++) == 0)
return (NULL);
} while ((char)tolower((unsigned char)sc) != c);
} while (_strncasecmp(s, find, len) != 0);
s--;
}
return (char *)(intptr_t)(s);
}
