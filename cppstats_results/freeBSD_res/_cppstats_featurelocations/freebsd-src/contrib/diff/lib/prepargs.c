




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif
#include "prepargs.h"
#include <string.h>
#include <sys/types.h>
#include <xalloc.h>

#include <ctype.h>



#if defined(STDC_HEADERS)
#define IN_CTYPE_DOMAIN(c) 1
#else
#define IN_CTYPE_DOMAIN(c) ((c) <= 0177)
#endif

#define ISSPACE(c) (IN_CTYPE_DOMAIN (c) && isspace (c))






static int
prepend_args (char const *options, char *buf, char **argv)
{
char const *o = options;
char *b = buf;
int n = 0;

for (;;)
{
while (ISSPACE ((unsigned char) *o))
o++;
if (!*o)
return n;
if (argv)
argv[n] = b;
n++;

do
if ((*b++ = *o++) == '\\' && *o)
b[-1] = *o++;
while (*o && ! ISSPACE ((unsigned char) *o));

*b++ = '\0';
}
}




void
prepend_default_options (char const *options, int *pargc, char ***pargv)
{
if (options)
{
char *buf = xmalloc (strlen (options) + 1);
int prepended = prepend_args (options, buf, (char **) 0);
int argc = *pargc;
char * const *argv = *pargv;
char **pp = (char **) xmalloc ((prepended + argc + 1) * sizeof *pp);
*pargc = prepended + argc;
*pargv = pp;
*pp++ = *argv++;
pp += prepend_args (options, buf, pp);
while ((*pp++ = *argv++))
continue;
}
}
