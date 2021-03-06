
































#include "gdtoaimp.h"

#undef _0
#undef _1



#if defined(IEEE_MC68k)
#define _0 0
#define _1 1
#define _2 2
#define _3 3
#endif
#if defined(IEEE_8087)
#define _0 3
#define _1 2
#define _2 1
#define _3 0
#endif

void
#if defined(KR_headers)
ULtoQ(L, bits, exp, k) ULong *L; ULong *bits; Long exp; int k;
#else
ULtoQ(ULong *L, ULong *bits, Long exp, int k)
#endif
{
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
L[0] = L[1] = L[2] = L[3] = 0;
break;

case STRTOG_Normal:
L[_3] = bits[0];
L[_2] = bits[1];
L[_1] = bits[2];
L[_0] = (bits[3] & ~0x10000) | ((exp + 0x3fff + 112) << 16);
break;

case STRTOG_NaNbits:
L[_3] = bits[0];
L[_2] = bits[1];
L[_1] = bits[2];
L[_0] = (bits[3] & ~0x10000)
| (((exp + 0x3fff + 112) << 16) | (1 << 15));
break;

case STRTOG_Denormal:
L[_3] = bits[0];
L[_2] = bits[1];
L[_1] = bits[2];
L[_0] = bits[3];
break;

case STRTOG_Infinite:
L[_0] = 0x7fff0000;
L[_1] = L[_2] = L[_3] = 0;
break;

case STRTOG_NaN:
L[0] = ld_QNAN0;
L[1] = ld_QNAN1;
L[2] = ld_QNAN2;
L[3] = ld_QNAN3;
}
if (k & STRTOG_Neg)
L[_0] |= 0x80000000L;
}

int
#if defined(KR_headers)
strtorQ_l(s, sp, rounding, L, locale) CONST char *s; char **sp; int rounding;
void *L; locale_t locale;
#else
strtorQ_l(CONST char *s, char **sp, int rounding, void *L, locale_t locale)
#endif
{
static FPI fpi0 = { 113, 1-16383-113+1, 32766-16383-113+1, 1, SI };
FPI *fpi, fpi1;
ULong bits[4];
Long exp;
int k;

fpi = &fpi0;
if (rounding != FPI_Round_near) {
fpi1 = fpi0;
fpi1.rounding = rounding;
fpi = &fpi1;
}
k = strtodg_l(s, sp, fpi, &exp, bits, locale);
ULtoQ((ULong*)L, bits, exp, k);
return k;
}
