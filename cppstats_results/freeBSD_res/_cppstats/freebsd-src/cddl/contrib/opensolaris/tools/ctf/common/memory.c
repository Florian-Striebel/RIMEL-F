#pragma ident "%Z%%M% %I% %E% SMI"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "memory.h"
static void
memory_bailout(void)
{
(void) fprintf(stderr, "Out of memory\n");
exit(1);
}
void *
xmalloc(size_t size)
{
void *mem;
if ((mem = malloc(size)) == NULL)
memory_bailout();
return (mem);
}
void *
xcalloc(size_t size)
{
void *mem;
mem = xmalloc(size);
bzero(mem, size);
return (mem);
}
char *
xstrdup(const char *str)
{
char *newstr;
if ((newstr = strdup(str)) == NULL)
memory_bailout();
return (newstr);
}
char *
xstrndup(char *str, size_t len)
{
char *newstr;
if ((newstr = malloc(len + 1)) == NULL)
memory_bailout();
(void) strncpy(newstr, str, len);
newstr[len] = '\0';
return (newstr);
}
void *
xrealloc(void *ptr, size_t size)
{
void *mem;
if ((mem = realloc(ptr, size)) == NULL)
memory_bailout();
return (mem);
}
