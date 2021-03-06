





#include "openbsd-compat.h"

#if !defined(HAVE_CLOCK_GETTIME)

#if _WIN32
int
clock_gettime(clockid_t clock_id, struct timespec *tp)
{
ULONGLONG ms;

if (clock_id != CLOCK_MONOTONIC) {
errno = EINVAL;
return (-1);
}

ms = GetTickCount64();
tp->tv_sec = ms / 1000L;
tp->tv_nsec = (ms % 1000L) * 1000000L;

return (0);
}
#else
#error "please provide an implementation of clock_gettime() for your platform"
#endif

#endif
