





























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD: support.c,v 1.9 2018/09/18 22:12:19 christos Exp $");

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "support.h"

static __attribute__((__format_arg__(3))) const char *
expandm(char *buf, size_t len, const char *fmt)
{
char *p;
size_t r;

if ((p = strstr(fmt, "%m")) == NULL)
return fmt;

r = (size_t)(p - fmt);
if (r >= len)
return fmt;

strlcpy(buf, fmt, r + 1);
strlcat(buf, strerror(errno), len);
strlcat(buf, fmt + r + 2, len);

return buf;
}

void
vdlog(int level __unused, const char *fmt, va_list ap)
{
char buf[BUFSIZ];


vfprintf(stderr, expandm(buf, sizeof(buf), fmt), ap);
fprintf(stderr, "\n");
}

void
dlog(int level, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
vdlog(level, fmt, ap);
va_end(ap);
}

const char *
fmttime(char *b, size_t l, time_t t)
{
struct tm tm;
if (localtime_r(&t, &tm) == NULL)
snprintf(b, l, "*%jd*", (intmax_t)t);
else
strftime(b, l, "%Y/%m/%d %H:%M:%S", &tm);
return b;
}

const char *
fmtydhms(char *b, size_t l, time_t t)
{
time_t s, m, h, d, y;
int z;
size_t o;

s = t % 60;
t /= 60;

m = t % 60;
t /= 60;

h = t % 24;
t /= 24;

d = t % 365;
t /= 365;

y = t;

z = 0;
o = 0;
#define APPEND(a) if (a) { z = snprintf(b + o, l - o, "%jd%s", (intmax_t)a, __STRING(a)); if (z == -1) return b; o += (size_t)z; if (o >= l) return b; }








APPEND(y)
APPEND(d)
APPEND(h)
APPEND(m)
APPEND(s)
return b;
}

ssize_t
blhexdump(char *buf, size_t len, const char *str, const void *b, size_t l)
{
size_t z, cz;
int r;
const unsigned char *p = b;
const unsigned char *e = p + l;

r = snprintf(buf, len, "%s: ", str);
if (r == -1)
return -1;
if ((cz = z = (size_t)r) >= len)
cz = len;

while (p < e) {
r = snprintf(buf + cz, len - cz, "%.2x", *p++);
if (r == -1)
return -1;
if ((cz = (z += (size_t)r)) >= len)
cz = len;
}
return (ssize_t)z;
}
