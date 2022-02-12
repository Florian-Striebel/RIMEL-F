
































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

void
#if defined(KR_headers)
ULtox(L, bits, exp, k) UShort *L; ULong *bits; Long exp; int k;
#else
ULtox(UShort *L, ULong *bits, Long exp, int k)
#endif
{
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
L[0] = L[1] = L[2] = L[3] = L[4] = 0;
break;

case STRTOG_Denormal:
L[_0] = 0;
goto normal_bits;

case STRTOG_Normal:
L[_0] = exp + 0x3fff + 63;
normal_bits:
L[_4] = (UShort)bits[0];
L[_3] = (UShort)(bits[0] >> 16);
L[_2] = (UShort)bits[1];
L[_1] = (UShort)(bits[1] >> 16);
break;

case STRTOG_NaNbits:
L[_0] = exp + 0x3fff + 63;
L[_4] = (UShort)bits[0];
L[_3] = (UShort)(bits[0] >> 16);
L[_2] = (UShort)bits[1];
L[_1] = (UShort)((bits[1] >> 16) | (3 << 14));
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
}

int
#if defined(KR_headers)
strtorx_l(s, sp, rounding, L, locale) CONST char *s; char **sp; int rounding;
void *L; locale_t locale;
#else
strtorx_l(CONST char *s, char **sp, int rounding, void *L, locale_t locale)
#endif
{
static FPI fpi0 = { 64, 1-16383-64+1, 32766 - 16383 - 64 + 1, 1, SI };
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
ULtox((UShort*)L, bits, exp, k);
return k;
}
