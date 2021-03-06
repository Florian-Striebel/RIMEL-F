

























#if !defined(TIME_INTERNAL_H_INCLUDED_)
#define TIME_INTERNAL_H_INCLUDED_

#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(EVENT__HAVE_MACH_MACH_TIME_H)

#include <mach/mach_time.h>
#endif

#include <time.h>

#include "event2/util.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(EVENT__HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
#define HAVE_POSIX_MONOTONIC
#elif defined(EVENT__HAVE_MACH_ABSOLUTE_TIME)
#define HAVE_MACH_MONOTONIC
#elif defined(_WIN32)
#define HAVE_WIN32_MONOTONIC
#else
#define HAVE_FALLBACK_MONOTONIC
#endif

long evutil_tv_to_msec_(const struct timeval *tv);
void evutil_usleep_(const struct timeval *tv);

#if defined(_WIN32)
typedef ULONGLONG (WINAPI *ev_GetTickCount_func)(void);
#endif

struct evutil_monotonic_timer {

#if defined(HAVE_MACH_MONOTONIC)
struct mach_timebase_info mach_timebase_units;
#endif

#if defined(HAVE_POSIX_MONOTONIC)
int monotonic_clock;
#endif

#if defined(HAVE_WIN32_MONOTONIC)
ev_GetTickCount_func GetTickCount64_fn;
ev_GetTickCount_func GetTickCount_fn;
ev_uint64_t last_tick_count;
ev_uint64_t adjust_tick_count;

ev_uint64_t first_tick;
ev_uint64_t first_counter;
double usec_per_count;
int use_performance_counter;
#endif

struct timeval adjust_monotonic_clock;
struct timeval last_time;
};

int evutil_configure_monotonic_time_(struct evutil_monotonic_timer *mt,
int flags);
int evutil_gettime_monotonic_(struct evutil_monotonic_timer *mt, struct timeval *tv);


#if defined(__cplusplus)
}
#endif

#endif
