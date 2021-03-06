
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");













#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#include <stdio.h>

#include "archive_string.h"
#include "archive_private.h"





static void
append_uint(struct archive_string *as, uintmax_t d, unsigned base)
{
static const char digits[] = "0123456789abcdef";
if (d >= base)
append_uint(as, d/base, base);
archive_strappend_char(as, digits[d % base]);
}

static void
append_int(struct archive_string *as, intmax_t d, unsigned base)
{
uintmax_t ud;

if (d < 0) {
archive_strappend_char(as, '-');
ud = (d == INTMAX_MIN) ? (uintmax_t)(INTMAX_MAX) + 1 : (uintmax_t)(-d);
} else
ud = d;
append_uint(as, ud, base);
}


void
archive_string_sprintf(struct archive_string *as, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
archive_string_vsprintf(as, fmt, ap);
va_end(ap);
}





void
archive_string_vsprintf(struct archive_string *as, const char *fmt,
va_list ap)
{
char long_flag;
intmax_t s;
uintmax_t u;
const char *p, *p2;
const wchar_t *pw;

if (archive_string_ensure(as, 64) == NULL)
__archive_errx(1, "Out of memory");

if (fmt == NULL) {
as->s[0] = 0;
return;
}

for (p = fmt; *p != '\0'; p++) {
const char *saved_p = p;

if (*p != '%') {
archive_strappend_char(as, *p);
continue;
}

p++;

long_flag = '\0';
switch(*p) {
case 'j':
case 'l':
case 'z':
long_flag = *p;
p++;
break;
}

switch (*p) {
case '%':
archive_strappend_char(as, '%');
break;
case 'c':
s = va_arg(ap, int);
archive_strappend_char(as, (char)s);
break;
case 'd':
switch(long_flag) {
case 'j': s = va_arg(ap, intmax_t); break;
case 'l': s = va_arg(ap, long); break;
case 'z': s = va_arg(ap, ssize_t); break;
default: s = va_arg(ap, int); break;
}
append_int(as, s, 10);
break;
case 's':
switch(long_flag) {
case 'l':
pw = va_arg(ap, wchar_t *);
if (pw == NULL)
pw = L"(null)";
if (archive_string_append_from_wcs(as, pw,
wcslen(pw)) != 0 && errno == ENOMEM)
__archive_errx(1, "Out of memory");
break;
default:
p2 = va_arg(ap, char *);
if (p2 == NULL)
p2 = "(null)";
archive_strcat(as, p2);
break;
}
break;
case 'S':
pw = va_arg(ap, wchar_t *);
if (pw == NULL)
pw = L"(null)";
if (archive_string_append_from_wcs(as, pw,
wcslen(pw)) != 0 && errno == ENOMEM)
__archive_errx(1, "Out of memory");
break;
case 'o': case 'u': case 'x': case 'X':

switch(long_flag) {
case 'j': u = va_arg(ap, uintmax_t); break;
case 'l': u = va_arg(ap, unsigned long); break;
case 'z': u = va_arg(ap, size_t); break;
default: u = va_arg(ap, unsigned int); break;
}

switch (*p) {
case 'o': append_uint(as, u, 8); break;
case 'u': append_uint(as, u, 10); break;
default: append_uint(as, u, 16); break;
}
break;
default:

p = saved_p;
archive_strappend_char(as, *p);
}
}
}
