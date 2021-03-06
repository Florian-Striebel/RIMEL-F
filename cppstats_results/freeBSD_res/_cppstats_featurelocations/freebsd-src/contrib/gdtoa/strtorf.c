






























#include "gdtoaimp.h"

void
#if defined(KR_headers)
ULtof(L, bits, exp, k) ULong *L; ULong *bits; Long exp; int k;
#else
ULtof(ULong *L, ULong *bits, Long exp, int k)
#endif
{
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
*L = 0;
break;

case STRTOG_Normal:
case STRTOG_NaNbits:
L[0] = (bits[0] & 0x7fffff) | ((exp + 0x7f + 23) << 23);
break;

case STRTOG_Denormal:
L[0] = bits[0];
break;

case STRTOG_Infinite:
L[0] = 0x7f800000;
break;

case STRTOG_NaN:
L[0] = f_QNAN;
}
if (k & STRTOG_Neg)
L[0] |= 0x80000000L;
}

int
#if defined(KR_headers)
strtorf(s, sp, rounding, f) CONST char *s; char **sp; int rounding; float *f;
#else
strtorf(CONST char *s, char **sp, int rounding, float *f)
#endif
{
static FPI fpi0 = { 24, 1-127-24+1, 254-127-24+1, 1, SI };
FPI *fpi, fpi1;
ULong bits[1];
Long exp;
int k;

fpi = &fpi0;
if (rounding != FPI_Round_near) {
fpi1 = fpi0;
fpi1.rounding = rounding;
fpi = &fpi1;
}
k = strtodg(s, sp, fpi, &exp, bits);
ULtof((ULong*)f, bits, exp, k);
return k;
}
