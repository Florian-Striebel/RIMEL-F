

























#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(_WIN32)
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <sys/types.h>
#if defined(EVENT__HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#include <errno.h>
#include <limits.h>
#if !defined(EVENT__HAVE_GETTIMEOFDAY)
#include <sys/timeb.h>
#endif
#if !defined(EVENT__HAVE_NANOSLEEP) && !defined(EVENT_HAVE_USLEEP) && !defined(_WIN32)

#include <sys/select.h>
#endif
#include <time.h>
#include <sys/stat.h>
#include <string.h>


#if defined(_WIN32)
#elif defined(EVENT__HAVE_NANOSLEEP)
#elif defined(EVENT__HAVE_USLEEP)
#include <unistd.h>
#endif

#include "event2/util.h"
#include "util-internal.h"
#include "log-internal.h"
#include "mm-internal.h"

#if !defined(EVENT__HAVE_GETTIMEOFDAY)

int
evutil_gettimeofday(struct timeval *tv, struct timezone *tz)
{
#if defined(_MSC_VER)
#define U64_LITERAL(n) n##ui64
#else
#define U64_LITERAL(n) n##llu
#endif





#define EPOCH_BIAS U64_LITERAL(116444736000000000)
#define UNITS_PER_SEC U64_LITERAL(10000000)
#define USEC_PER_SEC U64_LITERAL(1000000)
#define UNITS_PER_USEC U64_LITERAL(10)
union {
FILETIME ft_ft;
ev_uint64_t ft_64;
} ft;

if (tv == NULL)
return -1;

GetSystemTimeAsFileTime(&ft.ft_ft);

if (EVUTIL_UNLIKELY(ft.ft_64 < EPOCH_BIAS)) {

return -1;
}
ft.ft_64 -= EPOCH_BIAS;
tv->tv_sec = (long) (ft.ft_64 / UNITS_PER_SEC);
tv->tv_usec = (long) ((ft.ft_64 / UNITS_PER_USEC) % USEC_PER_SEC);
return 0;
}
#endif

#define MAX_SECONDS_IN_MSEC_LONG (((LONG_MAX) - 999) / 1000)


long
evutil_tv_to_msec_(const struct timeval *tv)
{
if (tv->tv_usec > 1000000 || tv->tv_sec > MAX_SECONDS_IN_MSEC_LONG)
return -1;

return (tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000);
}





void
evutil_usleep_(const struct timeval *tv)
{
if (!tv)
return;
#if defined(_WIN32)
{
long msec = evutil_tv_to_msec_(tv);
Sleep((DWORD)msec);
}
#elif defined(EVENT__HAVE_NANOSLEEP)
{
struct timespec ts;
ts.tv_sec = tv->tv_sec;
ts.tv_nsec = tv->tv_usec*1000;
nanosleep(&ts, NULL);
}
#elif defined(EVENT__HAVE_USLEEP)

sleep(tv->tv_sec);
usleep(tv->tv_usec);
#else
select(0, NULL, NULL, NULL, tv);
#endif
}

int
evutil_date_rfc1123(char *date, const size_t datelen, const struct tm *tm)
{
static const char *DAYS[] =
{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const char *MONTHS[] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

time_t t = time(NULL);

#if !defined(_WIN32)
struct tm sys;
#endif


if (tm == NULL) {
#if defined(_WIN32)

tm = gmtime(&t);
#else
gmtime_r(&t, &sys);
tm = &sys;
#endif
}

return evutil_snprintf(
date, datelen, "%s, %02d %s %4d %02d:%02d:%02d GMT",
DAYS[tm->tm_wday], tm->tm_mday, MONTHS[tm->tm_mon],
1900 + tm->tm_year, tm->tm_hour, tm->tm_min, tm->tm_sec);
}







static void
adjust_monotonic_time(struct evutil_monotonic_timer *base,
struct timeval *tv)
{
evutil_timeradd(tv, &base->adjust_monotonic_clock, tv);

if (evutil_timercmp(tv, &base->last_time, <)) {

struct timeval adjust;
evutil_timersub(&base->last_time, tv, &adjust);
evutil_timeradd(&adjust, &base->adjust_monotonic_clock,
&base->adjust_monotonic_clock);
*tv = base->last_time;
}
base->last_time = *tv;
}




struct evutil_monotonic_timer *
evutil_monotonic_timer_new(void)
{
struct evutil_monotonic_timer *p = NULL;

p = mm_malloc(sizeof(*p));
if (!p) goto done;

memset(p, 0, sizeof(*p));

done:
return p;
}




void
evutil_monotonic_timer_free(struct evutil_monotonic_timer *timer)
{
if (timer) {
mm_free(timer);
}
}




int
evutil_configure_monotonic_time(struct evutil_monotonic_timer *timer,
int flags)
{
return evutil_configure_monotonic_time_(timer, flags);
}




int
evutil_gettime_monotonic(struct evutil_monotonic_timer *timer,
struct timeval *tp)
{
return evutil_gettime_monotonic_(timer, tp);
}


#if defined(HAVE_POSIX_MONOTONIC)









int
evutil_configure_monotonic_time_(struct evutil_monotonic_timer *base,
int flags)
{



#if defined(CLOCK_MONOTONIC_COARSE)
const int precise = flags & EV_MONOT_PRECISE;
#endif
const int fallback = flags & EV_MONOT_FALLBACK;
struct timespec ts;

#if defined(CLOCK_MONOTONIC_COARSE)
if (CLOCK_MONOTONIC_COARSE < 0) {




event_errx(1,"I didn't expect CLOCK_MONOTONIC_COARSE to be < 0");
}
if (! precise && ! fallback) {
if (clock_gettime(CLOCK_MONOTONIC_COARSE, &ts) == 0) {
base->monotonic_clock = CLOCK_MONOTONIC_COARSE;
return 0;
}
}
#endif
if (!fallback && clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
base->monotonic_clock = CLOCK_MONOTONIC;
return 0;
}

if (CLOCK_MONOTONIC < 0) {
event_errx(1,"I didn't expect CLOCK_MONOTONIC to be < 0");
}

base->monotonic_clock = -1;
return 0;
}

int
evutil_gettime_monotonic_(struct evutil_monotonic_timer *base,
struct timeval *tp)
{
struct timespec ts;

if (base->monotonic_clock < 0) {
if (evutil_gettimeofday(tp, NULL) < 0)
return -1;
adjust_monotonic_time(base, tp);
return 0;
}

if (clock_gettime(base->monotonic_clock, &ts) == -1)
return -1;
tp->tv_sec = ts.tv_sec;
tp->tv_usec = ts.tv_nsec / 1000;

return 0;
}
#endif

#if defined(HAVE_MACH_MONOTONIC)











int
evutil_configure_monotonic_time_(struct evutil_monotonic_timer *base,
int flags)
{
const int fallback = flags & EV_MONOT_FALLBACK;
struct mach_timebase_info mi;
memset(base, 0, sizeof(*base));

if (!fallback &&
mach_timebase_info(&mi) == 0 &&
mach_absolute_time() != 0) {



mi.denom *= 1000;
memcpy(&base->mach_timebase_units, &mi, sizeof(mi));
} else {
base->mach_timebase_units.numer = 0;
}
return 0;
}

int
evutil_gettime_monotonic_(struct evutil_monotonic_timer *base,
struct timeval *tp)
{
ev_uint64_t abstime, usec;
if (base->mach_timebase_units.numer == 0) {
if (evutil_gettimeofday(tp, NULL) < 0)
return -1;
adjust_monotonic_time(base, tp);
return 0;
}

abstime = mach_absolute_time();
usec = (abstime * base->mach_timebase_units.numer)
/ (base->mach_timebase_units.denom);
tp->tv_sec = usec / 1000000;
tp->tv_usec = usec % 1000000;

return 0;
}
#endif

#if defined(HAVE_WIN32_MONOTONIC)




























































static ev_uint64_t
evutil_GetTickCount_(struct evutil_monotonic_timer *base)
{
if (base->GetTickCount64_fn) {

return base->GetTickCount64_fn();
} else if (base->GetTickCount_fn) {





ev_uint64_t v = base->GetTickCount_fn();
return (DWORD)v | ((v >> 18) & 0xFFFFFFFF00000000);
} else {




DWORD ticks = GetTickCount();
if (ticks < base->last_tick_count) {
base->adjust_tick_count += ((ev_uint64_t)1) << 32;
}
base->last_tick_count = ticks;
return ticks + base->adjust_tick_count;
}
}

int
evutil_configure_monotonic_time_(struct evutil_monotonic_timer *base,
int flags)
{
const int precise = flags & EV_MONOT_PRECISE;
const int fallback = flags & EV_MONOT_FALLBACK;
HANDLE h;
memset(base, 0, sizeof(*base));

h = evutil_load_windows_system_library_(TEXT("kernel32.dll"));
if (h != NULL && !fallback) {
base->GetTickCount64_fn = (ev_GetTickCount_func)GetProcAddress(h, "GetTickCount64");
base->GetTickCount_fn = (ev_GetTickCount_func)GetProcAddress(h, "GetTickCount");
}

base->first_tick = base->last_tick_count = evutil_GetTickCount_(base);
if (precise && !fallback) {
LARGE_INTEGER freq;
if (QueryPerformanceFrequency(&freq)) {
LARGE_INTEGER counter;
QueryPerformanceCounter(&counter);
base->first_counter = counter.QuadPart;
base->usec_per_count = 1.0e6 / freq.QuadPart;
base->use_performance_counter = 1;
}
}

return 0;
}

static inline ev_int64_t
abs64(ev_int64_t i)
{
return i < 0 ? -i : i;
}


int
evutil_gettime_monotonic_(struct evutil_monotonic_timer *base,
struct timeval *tp)
{
ev_uint64_t ticks = evutil_GetTickCount_(base);
if (base->use_performance_counter) {





LARGE_INTEGER counter;
ev_int64_t counter_elapsed, counter_usec_elapsed, ticks_elapsed;
QueryPerformanceCounter(&counter);
counter_elapsed = (ev_int64_t)
(counter.QuadPart - base->first_counter);
ticks_elapsed = ticks - base->first_tick;


counter_usec_elapsed = (ev_int64_t)
(counter_elapsed * base->usec_per_count);

if (abs64(ticks_elapsed*1000 - counter_usec_elapsed) > 1000000) {





counter_usec_elapsed = ticks_elapsed * 1000;
base->first_counter = (ev_uint64_t) (counter.QuadPart - counter_usec_elapsed / base->usec_per_count);
}
tp->tv_sec = (time_t) (counter_usec_elapsed / 1000000);
tp->tv_usec = counter_usec_elapsed % 1000000;

} else {

tp->tv_sec = (time_t) (ticks / 1000);
tp->tv_usec = (ticks % 1000) * 1000;
}
adjust_monotonic_time(base, tp);

return 0;
}
#endif

#if defined(HAVE_FALLBACK_MONOTONIC)






int
evutil_configure_monotonic_time_(struct evutil_monotonic_timer *base,
int precise)
{
memset(base, 0, sizeof(*base));
return 0;
}

int
evutil_gettime_monotonic_(struct evutil_monotonic_timer *base,
struct timeval *tp)
{
if (evutil_gettimeofday(tp, NULL) < 0)
return -1;
adjust_monotonic_time(base, tp);
return 0;

}
#endif
