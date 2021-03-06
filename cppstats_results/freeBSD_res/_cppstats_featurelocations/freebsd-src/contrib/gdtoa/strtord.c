






























#include "gdtoaimp.h"

void
#if defined(KR_headers)
ULtod(L, bits, exp, k) ULong *L; ULong *bits; Long exp; int k;
#else
ULtod(ULong *L, ULong *bits, Long exp, int k)
#endif
{
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
L[0] = L[1] = 0;
break;

case STRTOG_Denormal:
L[_1] = bits[0];
L[_0] = bits[1];
break;

case STRTOG_Normal:
case STRTOG_NaNbits:
L[_1] = bits[0];
L[_0] = (bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20);
break;

case STRTOG_Infinite:
L[_0] = 0x7ff00000;
L[_1] = 0;
break;

case STRTOG_NaN:
L[0] = d_QNAN0;
L[1] = d_QNAN1;
}
if (k & STRTOG_Neg)
L[_0] |= 0x80000000L;
}

int
#if defined(KR_headers)
strtord_l(s, sp, rounding, d, locale) CONST char *s; char **sp; int rounding;
double *d; locale_t locale;
#else
strtord_l(CONST char *s, char **sp, int rounding, double *d, locale_t locale)
#endif
{
static FPI fpi0 = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI };
FPI *fpi, fpi1;
ULong bits[2];
Long exp;
int k;

fpi = &fpi0;
if (rounding != FPI_Round_near) {
fpi1 = fpi0;
fpi1.rounding = rounding;
fpi = &fpi1;
}
k = strtodg_l(s, sp, fpi, &exp, bits, locale);
ULtod((ULong*)d, bits, exp, k);
return k;
}

