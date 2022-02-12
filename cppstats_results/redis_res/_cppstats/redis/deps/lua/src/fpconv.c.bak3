





























#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "fpconv.h"






static char locale_decimal_point = '.';








static void fpconv_update_locale()
{
char buf[8];

snprintf(buf, sizeof(buf), "%g", 0.5);



if (buf[0] != '0' || buf[2] != '5' || buf[3] != 0) {
fprintf(stderr, "Error: wide characters found or printf() bug.");
abort();
}

locale_decimal_point = buf[1];
}








static inline int valid_number_character(char ch)
{
char lower_ch;

if ('0' <= ch && ch <= '9')
return 1;
if (ch == '-' || ch == '+' || ch == '.')
return 1;


lower_ch = ch | 0x20;
if ('a' <= lower_ch && lower_ch <= 'y')
return 1;

return 0;
}



static int strtod_buffer_size(const char *s)
{
const char *p = s;

while (valid_number_character(*p))
p++;

return p - s;
}



double fpconv_strtod(const char *nptr, char **endptr)
{
char localbuf[FPCONV_G_FMT_BUFSIZE];
char *buf, *endbuf, *dp;
int buflen;
double value;


if (locale_decimal_point == '.')
return strtod(nptr, endptr);

buflen = strtod_buffer_size(nptr);
if (!buflen) {

*endptr = (char *)nptr;
return 0;
}


if (buflen >= FPCONV_G_FMT_BUFSIZE) {

buf = malloc(buflen + 1);
if (!buf) {
fprintf(stderr, "Out of memory");
abort();
}
} else {

buf = localbuf;
}
memcpy(buf, nptr, buflen);
buf[buflen] = 0;


dp = strchr(buf, '.');
if (dp)
*dp = locale_decimal_point;

value = strtod(buf, &endbuf);
*endptr = (char *)&nptr[endbuf - buf];
if (buflen >= FPCONV_G_FMT_BUFSIZE)
free(buf);

return value;
}


static void set_number_format(char *fmt, int precision)
{
int d1, d2, i;

assert(1 <= precision && precision <= 14);


d1 = precision / 10;
d2 = precision % 10;
fmt[0] = '%';
fmt[1] = '.';
i = 2;
if (d1) {
fmt[i++] = '0' + d1;
}
fmt[i++] = '0' + d2;
fmt[i++] = 'g';
fmt[i] = 0;
}


int fpconv_g_fmt(char *str, double num, int precision)
{
char buf[FPCONV_G_FMT_BUFSIZE];
char fmt[6];
int len;
char *b;

set_number_format(fmt, precision);


if (locale_decimal_point == '.')
return snprintf(str, FPCONV_G_FMT_BUFSIZE, fmt, num);


len = snprintf(buf, FPCONV_G_FMT_BUFSIZE, fmt, num);


b = buf;
do {
*str++ = (*b == locale_decimal_point ? '.' : *b);
} while(*b++);

return len;
}

void fpconv_init()
{
fpconv_update_locale();
}



