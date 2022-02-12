






























#include "gdtoaimp.h"

#undef _0
#undef _1



#if defined(IEEE_MC68k)
#define _0 0
#define _1 1
#define _2 2
#define _3 3
#define _4 4
#endif
#if defined(IEEE_8087)
#define _0 4
#define _1 3
#define _2 2
#define _3 1
#define _4 0
#endif

int
#if defined(KR_headers)
strtopx(s, sp, V) CONST char *s; char **sp; void *V;
#else
strtopx(CONST char *s, char **sp, void *V)
#endif
{
static FPI fpi0 = { 64, 1-16383-64+1, 32766 - 16383 - 64 + 1, 1, SI };
ULong bits[2];
Long exp;
int k;
UShort *L = (UShort*)V;
#if defined(Honor_FLT_ROUNDS)
#include "gdtoa_fltrnds.h"
#else
#define fpi &fpi0
#endif

k = strtodg(s, sp, fpi, &exp, bits);
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
L[0] = L[1] = L[2] = L[3] = L[4] = 0;
break;

case STRTOG_Denormal:
L[_0] = 0;
goto normal_bits;

case STRTOG_Normal:
case STRTOG_NaNbits:
L[_0] = exp + 0x3fff + 63;
normal_bits:
L[_4] = (UShort)bits[0];
L[_3] = (UShort)(bits[0] >> 16);
L[_2] = (UShort)bits[1];
L[_1] = (UShort)(bits[1] >> 16);
break;

case STRTOG_Infinite:
L[_0] = 0x7fff;
L[_1] = 0x8000;
L[_2] = L[_3] = L[_4] = 0;
break;

case STRTOG_NaN:
L[0] = ldus_QNAN0;
L[1] = ldus_QNAN1;
L[2] = ldus_QNAN2;
L[3] = ldus_QNAN3;
L[4] = ldus_QNAN4;
}
if (k & STRTOG_Neg)
L[_0] |= 0x8000;
return k;
}
