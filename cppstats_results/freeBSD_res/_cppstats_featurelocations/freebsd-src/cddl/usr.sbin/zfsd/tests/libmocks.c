

































#include <stdio.h>
#include <stdarg.h>
#include "libmocks.h"







int syslog_last_priority;
char syslog_last_message[4096];
void syslog(int priority, const char* message, ...) {
va_list ap;

syslog_last_priority = priority;
va_start(ap, message);
vsnprintf(syslog_last_message, 4096, message, ap);
va_end(ap);
}

int zpool_iter(libzfs_handle_t* handle, zpool_iter_f iter, void* arg) {
return (0);
}
