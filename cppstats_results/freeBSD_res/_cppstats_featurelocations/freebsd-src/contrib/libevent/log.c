






































#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(_WIN32)
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "event2/event.h"
#include "event2/util.h"

#include "log-internal.h"

static void event_log(int severity, const char *msg);
static void event_exit(int errcode) EV_NORETURN;

static event_fatal_cb fatal_fn = NULL;

#if defined(EVENT_DEBUG_LOGGING_ENABLED)
#if defined(USE_DEBUG)
#define DEFAULT_MASK EVENT_DBG_ALL
#else
#define DEFAULT_MASK 0
#endif

#if defined(USE_GLOBAL_FOR_DEBUG_LOGGING)
ev_uint32_t event_debug_logging_mask_ = DEFAULT_MASK;
#else
static ev_uint32_t event_debug_logging_mask_ = DEFAULT_MASK;
ev_uint32_t
event_debug_get_logging_mask_(void)
{
return event_debug_logging_mask_;
}
#endif
#endif

void
event_enable_debug_logging(ev_uint32_t which)
{
#if defined(EVENT_DEBUG_LOGGING_ENABLED)
event_debug_logging_mask_ = which;
#endif
}

void
event_set_fatal_callback(event_fatal_cb cb)
{
fatal_fn = cb;
}

static void
event_exit(int errcode)
{
if (fatal_fn) {
fatal_fn(errcode);
exit(errcode);
} else if (errcode == EVENT_ERR_ABORT_)
abort();
else
exit(errcode);
}

void
event_err(int eval, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
event_logv_(EVENT_LOG_ERR, strerror(errno), fmt, ap);
va_end(ap);
event_exit(eval);
}

void
event_warn(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
event_logv_(EVENT_LOG_WARN, strerror(errno), fmt, ap);
va_end(ap);
}

void
event_sock_err(int eval, evutil_socket_t sock, const char *fmt, ...)
{
va_list ap;
int err = evutil_socket_geterror(sock);

va_start(ap, fmt);
event_logv_(EVENT_LOG_ERR, evutil_socket_error_to_string(err), fmt, ap);
va_end(ap);
event_exit(eval);
}

void
event_sock_warn(evutil_socket_t sock, const char *fmt, ...)
{
va_list ap;
int err = evutil_socket_geterror(sock);

va_start(ap, fmt);
event_logv_(EVENT_LOG_WARN, evutil_socket_error_to_string(err), fmt, ap);
va_end(ap);
}

void
event_errx(int eval, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
event_logv_(EVENT_LOG_ERR, NULL, fmt, ap);
va_end(ap);
event_exit(eval);
}

void
event_warnx(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
event_logv_(EVENT_LOG_WARN, NULL, fmt, ap);
va_end(ap);
}

void
event_msgx(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
event_logv_(EVENT_LOG_MSG, NULL, fmt, ap);
va_end(ap);
}

void
event_debugx_(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
event_logv_(EVENT_LOG_DEBUG, NULL, fmt, ap);
va_end(ap);
}

void
event_logv_(int severity, const char *errstr, const char *fmt, va_list ap)
{
char buf[1024];
size_t len;

if (severity == EVENT_LOG_DEBUG && !event_debug_get_logging_mask_())
return;

if (fmt != NULL)
evutil_vsnprintf(buf, sizeof(buf), fmt, ap);
else
buf[0] = '\0';

if (errstr) {
len = strlen(buf);
if (len < sizeof(buf) - 3) {
evutil_snprintf(buf + len, sizeof(buf) - len, ": %s", errstr);
}
}

event_log(severity, buf);
}

static event_log_cb log_fn = NULL;

void
event_set_log_callback(event_log_cb cb)
{
log_fn = cb;
}

static void
event_log(int severity, const char *msg)
{
if (log_fn)
log_fn(severity, msg);
else {
const char *severity_str;
switch (severity) {
case EVENT_LOG_DEBUG:
severity_str = "debug";
break;
case EVENT_LOG_MSG:
severity_str = "msg";
break;
case EVENT_LOG_WARN:
severity_str = "warn";
break;
case EVENT_LOG_ERR:
severity_str = "err";
break;
default:
severity_str = "???";
break;
}
(void)fprintf(stderr, "[%s] %s\n", severity_str, msg);
}
}
