






























#include "gdtoaimp.h"

#undef _0
#undef _1



#if defined(IEEE_MC68k)
#define _0 0
#define _1 1
#define _2 2
#endif
#if defined(IEEE_8087)
#define _0 2
#define _1 1
#define _2 0
#endif

int
#if defined(KR_headers)
strtopxL(s, sp, V) CONST char *s; char **sp; void *V;
#else
strtopxL(CONST char *s, char **sp, void *V)
#endif
{
static FPI fpi0 = { 64, 1-16383-64+1, 32766 - 16383 - 64 + 1, 1, SI };
ULong bits[2];
Long exp;
int k;
ULong *L = (ULong*)V;
#if defined(Honor_FLT_ROUNDS)
#include "gdtoa_fltrnds.h"
#else
#define fpi &fpi0
#endif

k = strtodg(s, sp, fpi, &exp, bits);
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
L[0] = L[1] = L[2] = 0;
break;

case STRTOG_Normal:
case STRTOG_Denormal:
case STRTOG_NaNbits:
L[_2] = bits[0];
L[_1] = bits[1];
L[_0] = (exp + 0x3fff + 63) << 16;
break;

case STRTOG_Infinite:
L[_0] = 0x7fff << 16;
L[_1] = 0x80000000;
L[_2] = 0;
break;

case STRTOG_NaN:
L[0] = ld_QNAN0;
L[1] = ld_QNAN1;
L[2] = ld_QNAN2;
}
if (k & STRTOG_Neg)
L[_0] |= 0x80000000L;
return k;
}
