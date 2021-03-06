


























#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"

static const char *l9p_log_level_names[] = {
"DEBUG",
"INFO",
"WARN",
"ERROR"
};

void
l9p_logf(enum l9p_log_level level, const char *func, const char *fmt, ...)
{
const char *dest = NULL;
static FILE *stream = NULL;
va_list ap;

if (stream == NULL) {
dest = getenv("LIB9P_LOGGING");
if (dest == NULL)
return;
else if (!strcmp(dest, "stderr"))
stream = stderr;
else {
stream = fopen(dest, "a");
if (stream == NULL)
return;
}
}

va_start(ap, fmt);
fprintf(stream, "[%s]\t %s: ", l9p_log_level_names[level], func);
vfprintf(stream, fmt, ap);
fprintf(stream, "\n");
fflush(stream);
va_end(ap);
}
