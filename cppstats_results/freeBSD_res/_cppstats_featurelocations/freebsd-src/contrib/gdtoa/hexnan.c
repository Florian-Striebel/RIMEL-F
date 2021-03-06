






























#include "gdtoaimp.h"

static void
#if defined(KR_headers)
L_shift(x, x1, i) ULong *x; ULong *x1; int i;
#else
L_shift(ULong *x, ULong *x1, int i)
#endif
{
int j;

i = 8 - i;
i <<= 2;
j = ULbits - i;
do {
*x |= x[1] << j;
x[1] >>= i;
} while(++x < x1);
}

int
#if defined(KR_headers)
hexnan(sp, fpi, x0)
CONST char **sp; FPI *fpi; ULong *x0;
#else
hexnan( CONST char **sp, FPI *fpi, ULong *x0)
#endif
{
ULong c, h, *x, *x1, *xe;
CONST char *s;
int havedig, hd0, i, nbits;

if (!hexdig['0'])
hexdig_init_D2A();
nbits = fpi->nbits;
x = x0 + (nbits >> kshift);
if (nbits & kmask)
x++;
*--x = 0;
x1 = xe = x;
havedig = hd0 = i = 0;
s = *sp;

while((c = *(CONST unsigned char*)(s+1)) && c <= ' ')
++s;
if (s[1] == '0' && (s[2] == 'x' || s[2] == 'X')
&& *(CONST unsigned char*)(s+3) > ' ')
s += 2;
while((c = *(CONST unsigned char*)++s)) {
if (!(h = hexdig[c])) {
if (c <= ' ') {
if (hd0 < havedig) {
if (x < x1 && i < 8)
L_shift(x, x1, i);
if (x <= x0) {
i = 8;
continue;
}
hd0 = havedig;
*--x = 0;
x1 = x;
i = 0;
}
while(*(CONST unsigned char*)(s+1) <= ' ')
++s;
if (s[1] == '0' && (s[2] == 'x' || s[2] == 'X')
&& *(CONST unsigned char*)(s+3) > ' ')
s += 2;
continue;
}
if ( c == ')' && havedig) {
*sp = s + 1;
break;
}
#if !defined(GDTOA_NON_PEDANTIC_NANCHECK)
do {
if ( c == ')') {
*sp = s + 1;
break;
}
} while((c = *++s));
#endif
return STRTOG_NaN;
}
havedig++;
if (++i > 8) {
if (x <= x0)
continue;
i = 1;
*--x = 0;
}
*x = (*x << 4) | (h & 0xf);
}
if (!havedig)
return STRTOG_NaN;
if (x < x1 && i < 8)
L_shift(x, x1, i);
if (x > x0) {
x1 = x0;
do *x1++ = *x++;
while(x <= xe);
do *x1++ = 0;
while(x1 <= xe);
}
else {

if ( (i = nbits & (ULbits-1)) !=0)
*xe &= ((ULong)0xffffffff) >> (ULbits - i);
}
for(x1 = xe;; --x1) {
if (*x1 != 0)
break;
if (x1 == x0) {
*x1 = 1;
break;
}
}
return STRTOG_NaNbits;
}
