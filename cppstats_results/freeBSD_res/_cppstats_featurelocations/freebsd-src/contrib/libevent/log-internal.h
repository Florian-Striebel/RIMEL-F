

























#if !defined(LOG_INTERNAL_H_INCLUDED_)
#define LOG_INTERNAL_H_INCLUDED_

#include "event2/util.h"

#if defined(__GNUC__)
#define EV_CHECK_FMT(a,b) __attribute__((format(printf, a, b)))
#define EV_NORETURN __attribute__((noreturn))
#else
#define EV_CHECK_FMT(a,b)
#define EV_NORETURN
#endif

#define EVENT_ERR_ABORT_ ((int)0xdeaddead)

#define USE_GLOBAL_FOR_DEBUG_LOGGING

#if !defined(EVENT__DISABLE_DEBUG_MODE) || defined(USE_DEBUG)
#define EVENT_DEBUG_LOGGING_ENABLED
#endif

#if defined(EVENT_DEBUG_LOGGING_ENABLED)
#if defined(USE_GLOBAL_FOR_DEBUG_LOGGING)
extern ev_uint32_t event_debug_logging_mask_;
#define event_debug_get_logging_mask_() (event_debug_logging_mask_)
#else
ev_uint32_t event_debug_get_logging_mask_(void);
#endif
#else
#define event_debug_get_logging_mask_() (0)
#endif

void event_err(int eval, const char *fmt, ...) EV_CHECK_FMT(2,3) EV_NORETURN;
void event_warn(const char *fmt, ...) EV_CHECK_FMT(1,2);
void event_sock_err(int eval, evutil_socket_t sock, const char *fmt, ...) EV_CHECK_FMT(3,4) EV_NORETURN;
void event_sock_warn(evutil_socket_t sock, const char *fmt, ...) EV_CHECK_FMT(2,3);
void event_errx(int eval, const char *fmt, ...) EV_CHECK_FMT(2,3) EV_NORETURN;
void event_warnx(const char *fmt, ...) EV_CHECK_FMT(1,2);
void event_msgx(const char *fmt, ...) EV_CHECK_FMT(1,2);
void event_debugx_(const char *fmt, ...) EV_CHECK_FMT(1,2);

void event_logv_(int severity, const char *errstr, const char *fmt, va_list ap)
EV_CHECK_FMT(3,0);

#if defined(EVENT_DEBUG_LOGGING_ENABLED)
#define event_debug(x) do { if (event_debug_get_logging_mask_()) { event_debugx_ x; } } while (0)




#else
#define event_debug(x) ((void)0)
#endif

#undef EV_CHECK_FMT

#endif
