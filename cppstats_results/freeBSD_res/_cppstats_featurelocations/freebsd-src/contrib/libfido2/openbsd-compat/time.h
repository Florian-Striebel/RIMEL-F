




#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#include <../ucrt/time.h>
#elif defined(_MSC_VER) && (_MSC_VER < 1900)
#include <../include/time.h>
#else
#include <time.h>
#endif

#if !defined(_COMPAT_TIME_H)
#define _COMPAT_TIME_H

#if !defined(CLOCK_MONOTONIC)
#define CLOCK_MONOTONIC CLOCK_REALTIME
#endif

#if !defined(CLOCK_REALTIME)
#define CLOCK_REALTIME 0
#endif

#if !defined(HAVE_CLOCK_GETTIME)
typedef int clockid_t;
int clock_gettime(clockid_t, struct timespec *);
#endif

#if defined(HAVE_TIMESPECSUB)
#include <sys/time.h>
#endif

#if !defined(HAVE_TIMESPECSUB)
#define timespecadd(tsp, usp, vsp) do { (vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec; (vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec; if ((vsp)->tv_nsec >= 1000000000L) { (vsp)->tv_sec++; (vsp)->tv_nsec -= 1000000000L; } } while (0)









#define timespecsub(tsp, usp, vsp) do { (vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec; (vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec; if ((vsp)->tv_nsec < 0) { (vsp)->tv_sec--; (vsp)->tv_nsec += 1000000000L; } } while (0)









#define timespeccmp(tsp, usp, cmp) (((tsp)->tv_sec == (usp)->tv_sec) ? ((tsp)->tv_nsec cmp (usp)->tv_nsec) : ((tsp)->tv_sec cmp (usp)->tv_sec))



#endif

#endif
