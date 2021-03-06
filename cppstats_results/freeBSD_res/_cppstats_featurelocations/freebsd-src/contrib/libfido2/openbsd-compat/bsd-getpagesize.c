

#include "openbsd-compat.h"

#if !defined(HAVE_GETPAGESIZE)

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <limits.h>

int
getpagesize(void)
{
#if defined(HAVE_SYSCONF) && defined(_SC_PAGESIZE)
long r = sysconf(_SC_PAGESIZE);
if (r > 0 && r < INT_MAX)
return (int)r;
#endif




return 4096;
}

#endif
