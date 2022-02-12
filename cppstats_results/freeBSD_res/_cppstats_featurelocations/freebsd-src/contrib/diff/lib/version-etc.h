


















#if !defined(VERSION_ETC_H)
#define VERSION_ETC_H 1

#include <stdarg.h>
#include <stdio.h>

extern const char *version_etc_copyright;

extern void version_etc_va (FILE *stream,
const char *command_name, const char *package,
const char *version, va_list authors);

extern void version_etc (FILE *stream,
const char *command_name, const char *package,
const char *version,
...);

#endif
