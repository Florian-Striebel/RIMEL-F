

























#include "lafe_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_STDARG_H)
#include <stdarg.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "err.h"

static void lafe_vwarnc(int, const char *, va_list) __LA_PRINTFLIKE(2, 0);

static const char *lafe_progname;

const char *
lafe_getprogname(void)
{

return lafe_progname;
}

void
lafe_setprogname(const char *name, const char *defaultname)
{

if (name == NULL)
name = defaultname;
#if defined(_WIN32) && !defined(__CYGWIN__)
lafe_progname = strrchr(name, '\\');
if (strrchr(name, '/') > lafe_progname)
#endif
lafe_progname = strrchr(name, '/');
if (lafe_progname != NULL)
lafe_progname++;
else
lafe_progname = name;
}

static void
lafe_vwarnc(int code, const char *fmt, va_list ap)
{
fprintf(stderr, "%s: ", lafe_progname);
vfprintf(stderr, fmt, ap);
if (code != 0)
fprintf(stderr, ": %s", strerror(code));
fprintf(stderr, "\n");
}

void
lafe_warnc(int code, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
lafe_vwarnc(code, fmt, ap);
va_end(ap);
}

void
lafe_errc(int eval, int code, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
lafe_vwarnc(code, fmt, ap);
va_end(ap);
exit(eval);
}
