#pragma ident "%Z%%M% %I% %E% SMI"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <elf.h>
#include <util.h>
void
die(char *format, ...)
{
va_list ap;
int err = errno;
#if !defined(illumos)
const char *progname = getprogname();
#endif
(void) fprintf(stderr, "%s: ", progname);
va_start(ap, format);
(void) vfprintf(stderr, format, ap);
va_end(ap);
if (format[strlen(format) - 1] != '\n')
(void) fprintf(stderr, ": %s\n", strerror(err));
#if !defined(illumos)
exit(0);
#else
exit(1);
#endif
}
void
elfdie(char *format, ...)
{
va_list ap;
#if !defined(illumos)
const char *progname = getprogname();
#endif
(void) fprintf(stderr, "%s: ", progname);
va_start(ap, format);
(void) vfprintf(stderr, format, ap);
va_end(ap);
if (format[strlen(format) - 1] != '\n')
(void) fprintf(stderr, ": %s\n", elf_errmsg(elf_errno()));
#if !defined(illumos)
exit(0);
#else
exit(1);
#endif
}
