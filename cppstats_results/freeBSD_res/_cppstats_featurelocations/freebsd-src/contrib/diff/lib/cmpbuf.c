


















#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <limits.h>

#include <signal.h>
#if !defined(SA_RESTART)
#if defined(SA_INTERRUPT)
#define SA_RESTART SA_INTERRUPT
#else
#define SA_RESTART 0
#endif
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include <sys/types.h>
#include "cmpbuf.h"






#if !defined(TYPE_SIGNED)
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))
#endif
#if !defined(TYPE_MINIMUM)
#define TYPE_MINIMUM(t) ((t) (TYPE_SIGNED (t) ? ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1) : (t) 0))


#endif
#if !defined(TYPE_MAXIMUM)
#define TYPE_MAXIMUM(t) ((t) (~ (t) 0 - TYPE_MINIMUM (t)))
#endif

#if !defined(PTRDIFF_MAX)
#define PTRDIFF_MAX TYPE_MAXIMUM (ptrdiff_t)
#endif
#if !defined(SIZE_MAX)
#define SIZE_MAX TYPE_MAXIMUM (size_t)
#endif
#if !defined(SSIZE_MAX)
#define SSIZE_MAX TYPE_MAXIMUM (ssize_t)
#endif

#undef MIN
#define MIN(a, b) ((a) <= (b) ? (a) : (b))







size_t
block_read (int fd, char *buf, size_t nbytes)
{
char *bp = buf;
char const *buflim = buf + nbytes;
size_t readlim = SSIZE_MAX;

do
{
size_t bytes_to_read = MIN (buflim - bp, readlim);
ssize_t nread = read (fd, bp, bytes_to_read);
if (nread <= 0)
{
if (nread == 0)
break;



if (errno == EINVAL && INT_MAX < bytes_to_read)
{
readlim = INT_MAX;
continue;
}






if (! SA_RESTART && errno == EINTR)
continue;

return SIZE_MAX;
}
bp += nread;
}
while (bp < buflim);

return bp - buf;
}





size_t
buffer_lcm (size_t a, size_t b, size_t lcm_max)
{
size_t lcm, m, n, q, r;


if (!a)
return b ? b : 8 * 1024;
if (!b)
return a;


for (m = a, n = b; (r = m % n) != 0; m = n, n = r)
continue;


q = a / n;
lcm = q * b;
return lcm <= lcm_max && lcm / b == q ? lcm : a;
}
