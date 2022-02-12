






































#include <sys/cdefs.h>

#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <stdbool.h>


#define HN_DECIMAL 0x01
#define HN_NOSPACE 0x02
#define HN_B 0x04
#define HN_DIVISOR_1000 0x08

#define HN_GETSCALE 0x10
#define HN_AUTOSCALE 0x20

static int
xo_humanize_number (char *buf, size_t len, int64_t bytes,
const char *suffix, int scale, int flags)
{
const char *prefixes, *sep;
int b, i, r, maxscale, s1, s2, sign;
int64_t divisor, max;



#if defined(__LP64__)
__int128_t scalable = (__int128_t)bytes;
#else
long double scalable = (long double)bytes;
#endif
size_t baselen;

assert(buf != NULL);
assert(suffix != NULL);
assert(scale >= 0);

if (flags & HN_DIVISOR_1000) {

divisor = 1000;
if (flags & HN_B)
prefixes = "B\0k\0M\0G\0T\0P\0E";
else
prefixes = "\0\0k\0M\0G\0T\0P\0E";
} else {




divisor = 1024;
if (flags & HN_B)
prefixes = "B\0K\0M\0G\0T\0P\0E";
else
prefixes = "\0\0K\0M\0G\0T\0P\0E";
}

#define SCALE2PREFIX(scale) (&prefixes[(scale) << 1])
maxscale = 7;

if (scale >= maxscale &&
(scale & (HN_AUTOSCALE | HN_GETSCALE)) == 0)
return (-1);

if (buf == NULL || suffix == NULL)
return (-1);

if (len > 0)
buf[0] = '\0';
if (bytes < 0) {
sign = -1;
scalable *= -100;
baselen = 3;
} else {
sign = 1;
scalable *= 100;
baselen = 2;
}
if (flags & HN_NOSPACE)
sep = "";
else {
sep = " ";
baselen++;
}
baselen += strlen(suffix);


if (len < baselen + 1)
return (-1);

if (scale & (HN_AUTOSCALE | HN_GETSCALE)) {

for (max = 100, i = len - baselen; i-- > 0;)
max *= 10;

for (i = 0; scalable >= max && i < maxscale; i++)
scalable /= divisor;

if (scale & HN_GETSCALE)
return (i);
} else
for (i = 0; i < scale && i < maxscale; i++)
scalable /= divisor;


if (scalable < 995 && i > 0 && flags & HN_DECIMAL) {

if (len < baselen + 1 + 2)
return (-1);
b = ((int)scalable + 5) / 10;
s1 = b / 10;
s2 = b % 10;
r = snprintf(buf, len, "%s%d%s%d%s%s%s",
((sign == -1) ? "-" : ""),
s1, localeconv()->decimal_point, s2,
sep, SCALE2PREFIX(i), suffix);
} else
r = snprintf(buf, len, "%s%lld%s%s%s",

((sign == -1) ? "-" : ""),
(long long)((scalable + 50) / 100),
sep, SCALE2PREFIX(i), suffix);

return (r);
}
