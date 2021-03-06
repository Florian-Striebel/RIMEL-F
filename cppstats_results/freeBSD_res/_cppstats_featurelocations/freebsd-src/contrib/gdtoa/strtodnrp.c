




































#include "gdtoaimp.h"

double
#if defined(KR_headers)
strtod(s, sp) CONST char *s; char **sp;
#else
strtod(CONST char *s, char **sp)
#endif
{
static FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI };
ULong bits[2];
Long exp;
int k;
union { ULong L[2]; double d; } u;

k = strtodg(s, sp, &fpi, &exp, bits);
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
u.L[0] = u.L[1] = 0;
break;

case STRTOG_Normal:
u.L[_1] = bits[0];
u.L[_0] = (bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20);
break;

case STRTOG_Denormal:
u.L[_1] = bits[0];
u.L[_0] = bits[1];
break;

case STRTOG_Infinite:
u.L[_0] = 0x7ff00000;
u.L[_1] = 0;
break;

case STRTOG_NaN:
u.L[0] = d_QNAN0;
u.L[1] = d_QNAN1;
break;

case STRTOG_NaNbits:
u.L[_0] = 0x7ff00000 | bits[1];
u.L[_1] = bits[0];
}
if (k & STRTOG_Neg)
u.L[_0] |= 0x80000000L;
return u.d;
}
