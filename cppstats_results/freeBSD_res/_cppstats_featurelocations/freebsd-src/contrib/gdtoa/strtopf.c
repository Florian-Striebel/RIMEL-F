






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtopf(s, sp, f) CONST char *s; char **sp; float *f;
#else
strtopf(CONST char *s, char **sp, float *f)
#endif
{
static FPI fpi0 = { 24, 1-127-24+1, 254-127-24+1, 1, SI };
ULong bits[1], *L;
Long exp;
int k;
#if defined(Honor_FLT_ROUNDS)
#include "gdtoa_fltrnds.h"
#else
#define fpi &fpi0
#endif

k = strtodg(s, sp, fpi, &exp, bits);
L = (ULong*)f;
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
L[0] = 0;
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
return k;
}
