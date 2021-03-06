






























#include "gdtoaimp.h"

#if defined(USE_LOCALE)
#include "locale.h"
#endif

char *
#if defined(KR_headers)
g__fmt(b, s, se, decpt, sign, blen) char *b; char *s; char *se; int decpt; ULong sign; size_t blen;
#else
g__fmt(char *b, char *s, char *se, int decpt, ULong sign, size_t blen)
#endif
{
int i, j, k;
char *be, *s0;
size_t len;
#if defined(USE_LOCALE)
#if defined(NO_LOCALE_CACHE)
char *decimalpoint = localeconv()->decimal_point;
size_t dlen = strlen(decimalpoint);
#else
char *decimalpoint;
static char *decimalpoint_cache;
static size_t dlen;
if (!(s0 = decimalpoint_cache)) {
s0 = localeconv()->decimal_point;
dlen = strlen(s0);
if ((decimalpoint_cache = (char*)MALLOC(strlen(s0) + 1))) {
strcpy(decimalpoint_cache, s0);
s0 = decimalpoint_cache;
}
}
decimalpoint = s0;
#endif
#else
#define dlen 0
#endif
s0 = s;
len = (se-s) + dlen + 6;
if (blen < len)
goto ret0;
be = b + blen - 1;
if (sign)
*b++ = '-';
if (decpt <= -4 || decpt > se - s + 5) {
*b++ = *s++;
if (*s) {
#if defined(USE_LOCALE)
while((*b = *decimalpoint++))
++b;
#else
*b++ = '.';
#endif
while((*b = *s++) !=0)
b++;
}
*b++ = 'e';

if (--decpt < 0) {
*b++ = '-';
decpt = -decpt;
}
else
*b++ = '+';
for(j = 2, k = 10; 10*k <= decpt; j++, k *= 10){}
for(;;) {
i = decpt / k;
if (b >= be)
goto ret0;
*b++ = i + '0';
if (--j <= 0)
break;
decpt -= i*k;
decpt *= 10;
}
*b = 0;
}
else if (decpt <= 0) {
#if defined(USE_LOCALE)
while((*b = *decimalpoint++))
++b;
#else
*b++ = '.';
#endif
if (be < b - decpt + (se - s))
goto ret0;
for(; decpt < 0; decpt++)
*b++ = '0';
while((*b = *s++) != 0)
b++;
}
else {
while((*b = *s++) != 0) {
b++;
if (--decpt == 0 && *s) {
#if defined(USE_LOCALE)
while(*b = *decimalpoint++)
++b;
#else
*b++ = '.';
#endif
}
}
if (b + decpt > be) {
ret0:
b = 0;
goto ret;
}
for(; decpt > 0; decpt--)
*b++ = '0';
*b = 0;
}
ret:
freedtoa(s0);
return b;
}
