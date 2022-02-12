




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <syslog.h>
#endif

#include "portability.h"

#include "log.h"

static int log_to_systemlog;
static int log_debug_messages;

static void rpcapd_vlog_stderr(log_priority,
PCAP_FORMAT_STRING(const char *), va_list) PCAP_PRINTFLIKE(2, 0);

static void rpcapd_vlog_stderr(log_priority priority, const char *message, va_list ap)
{
const char *tag;












tag = "";

switch (priority) {

case LOGPRIO_DEBUG:
tag = "DEBUG: ";
break;

case LOGPRIO_INFO:
tag = "";
break;

case LOGPRIO_WARNING:
tag = "warning: ";
break;

case LOGPRIO_ERROR:
tag = "error: ";
break;
}

fprintf(stderr, "rpcapd: %s", tag);
vfprintf(stderr, message, ap);
putc('\n', stderr);
}

static void rpcapd_vlog_systemlog(log_priority,
PCAP_FORMAT_STRING(const char *), va_list) PCAP_PRINTFLIKE(2, 0);

#if defined(_WIN32)
#define MESSAGE_SUBKEY "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\rpcapd"


static void rpcapd_vlog_systemlog(log_priority priority, const char *message,
va_list ap)
{
#if 0
static int initialized = 0;
HKEY hey_handle;
static HANDLE log_handle;
WORD eventlog_type;
DWORD event_id;
char msgbuf[1024];
char *strings[1];

if (!initialized) {







if (RegCreateKey(HKEY_LOCAL_MACHINE, MESSAGE_SUBKEY,
&key_handle) != ERROR_SUCCESS) {





log_to_systemlog = 0;
initialized = 1;
rpcapd_vlog_stderr(priority, message, ap);
return;
}
log_handle = RegisterEventSource(NULL, "rpcapd");
initialized = 1;
}

switch (priority) {

case LOGPRIO_DEBUG:



eventlog_type = EVENTLOG_INFORMATION_TYPE;
event_id = RPCAPD_INFO_ID;
break;

case LOGPRIO_INFO:
eventlog_type = EVENTLOG_INFORMATION_TYPE;
event_id = RPCAPD_INFO_ID;
break;

case LOGPRIO_WARNING:
eventlog_type = EVENTLOG_WARNING_TYPE;
event_id = RPCAPD_WARNING_ID;
break;

case LOGPRIO_ERROR:
eventlog_type = EVENTLOG_ERROR_TYPE;
event_id = RPCAPD_ERROR_ID;
break;

default:

return;
}

vsprintf(msgbuf, message, ap);

strings[0] = msgbuf;



(void) ReportEvent(log_handle, eventlog_type, 0, event_id, NULL, 1, 0,
strings, NULL);
#else
rpcapd_vlog_stderr(priority, message, ap);
#endif
}
#else
static void rpcapd_vlog_systemlog(log_priority priority, const char *message,
va_list ap)
{
static int initialized = 0;
int syslog_priority;

if (!initialized) {



openlog("rpcapd", LOG_PID, LOG_DAEMON);
initialized = 1;
}

switch (priority) {

case LOGPRIO_DEBUG:
syslog_priority = LOG_DEBUG;
break;

case LOGPRIO_INFO:
syslog_priority = LOG_INFO;
break;

case LOGPRIO_WARNING:
syslog_priority = LOG_WARNING;
break;

case LOGPRIO_ERROR:
syslog_priority = LOG_ERR;
break;

default:

return;
}

#if defined(HAVE_VSYSLOG)
vsyslog(syslog_priority, message, ap);
#else

















char logbuf[1024+1];

pcap_vsnprintf(logbuf, sizeof logbuf, message, ap);
syslog(syslog_priority, "%s", logbuf);
#endif
}
#endif

void rpcapd_log_set(int log_to_systemlog_arg, int log_debug_messages_arg)
{
log_debug_messages = log_debug_messages_arg;
log_to_systemlog = log_to_systemlog_arg;
}

void rpcapd_log(log_priority priority, const char *message, ...)
{
va_list ap;

if (priority != LOGPRIO_DEBUG || log_debug_messages) {
va_start(ap, message);
if (log_to_systemlog)
{
rpcapd_vlog_systemlog(priority, message, ap);
}
else
{
rpcapd_vlog_stderr(priority, message, ap);
}
va_end(ap);
}
}
