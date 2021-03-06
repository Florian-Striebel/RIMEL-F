



















#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "posixver.h"

#include <limits.h>
#include <stdlib.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if !defined(_POSIX2_VERSION)
#define _POSIX2_VERSION 0
#endif

#if !defined(DEFAULT_POSIX2_VERSION)
#define DEFAULT_POSIX2_VERSION _POSIX2_VERSION
#endif




int
posix2_version (void)
{
long int v = DEFAULT_POSIX2_VERSION;
char const *s = getenv ("_POSIX2_VERSION");

if (s && *s)
{
char *e;
long int i = strtol (s, &e, 10);
if (! *e)
v = i;
}

return v < INT_MIN ? INT_MIN : v < INT_MAX ? v : INT_MAX;
}
