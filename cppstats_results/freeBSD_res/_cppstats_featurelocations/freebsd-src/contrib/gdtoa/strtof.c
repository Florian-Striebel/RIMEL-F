
































#include "gdtoaimp.h"

float
#if defined(KR_headers)
strtof_l(s, sp, loc) CONST char *s; char **sp; locale_t loc;
#else
strtof_l(CONST char *s, char **sp, locale_t loc)
#endif
{
static FPI fpi0 = { 24, 1-127-24+1, 254-127-24+1, 1, SI };
ULong bits[1];
Long exp;
int k;
union { ULong L[1]; float f; } u;
#if defined(Honor_FLT_ROUNDS)
#include "gdtoa_fltrnds.h"
#else
#define fpi &fpi0
#endif

k = strtodg_l(s, sp, fpi, &exp, bits, loc);
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
u.L[0] = 0;
break;

case STRTOG_Normal:
u.L[0] = (bits[0] & 0x7fffff) | ((exp + 0x7f + 23) << 23);
break;

case STRTOG_NaNbits:

u.L[0] = bits[0] | 0x7fc00000;
break;

case STRTOG_Denormal:
u.L[0] = bits[0];
break;

case STRTOG_Infinite:
u.L[0] = 0x7f800000;
break;

case STRTOG_NaN:
u.L[0] = f_QNAN;
}
if (k & STRTOG_Neg)
u.L[0] |= 0x80000000L;
return u.f;
}
float
#if defined(KR_headers)
strtof(s, sp) CONST char *s; char **sp;
#else
strtof(CONST char *s, char **sp)
#endif
{
return strtof_l(s, sp, __get_locale());
}

