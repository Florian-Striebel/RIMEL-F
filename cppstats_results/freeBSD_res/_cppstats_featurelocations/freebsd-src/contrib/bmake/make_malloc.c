



























#include <errno.h>

#include "make.h"

MAKE_RCSID("$NetBSD: make_malloc.c,v 1.25 2021/01/19 20:51:46 rillig Exp $");

#if !defined(USE_EMALLOC)


static MAKE_ATTR_DEAD void
enomem(void)
{
(void)fprintf(stderr, "%s: %s.\n", progname, strerror(ENOMEM));
exit(2);
}


void *
bmake_malloc(size_t len)
{
void *p;

if ((p = malloc(len)) == NULL)
enomem();
return p;
}


char *
bmake_strdup(const char *str)
{
size_t len;
char *p;

len = strlen(str) + 1;
p = bmake_malloc(len);
return memcpy(p, str, len);
}


char *
bmake_strldup(const char *str, size_t len)
{
char *p = bmake_malloc(len + 1);
memcpy(p, str, len);
p[len] = '\0';
return p;
}


void *
bmake_realloc(void *ptr, size_t size)
{
if ((ptr = realloc(ptr, size)) == NULL)
enomem();
return ptr;
}
#endif


char *
bmake_strsedup(const char *start, const char *end)
{
return bmake_strldup(start, (size_t)(end - start));
}
