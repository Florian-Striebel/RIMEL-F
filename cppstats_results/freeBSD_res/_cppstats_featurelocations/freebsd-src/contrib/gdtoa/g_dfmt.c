






























#include "gdtoaimp.h"

char*
#if defined(KR_headers)
g_dfmt(buf, d, ndig, bufsize) char *buf; double *d; int ndig; size_t bufsize;
#else
g_dfmt(char *buf, double *d, int ndig, size_t bufsize)
#endif
{
static FPI fpi0 = { 53, 1-1023-53+1, 2046-1023-53+1, 1, 0 };
char *b, *s, *se;
ULong bits[2], *L, sign;
int decpt, ex, i, mode;
#if defined(Honor_FLT_ROUNDS)
#include "gdtoa_fltrnds.h"
#else
#define fpi &fpi0
#endif

if (ndig < 0)
ndig = 0;
if (bufsize < ndig + 10)
return 0;

L = (ULong*)d;
sign = L[_0] & 0x80000000L;
if ((L[_0] & 0x7ff00000) == 0x7ff00000) {

if (bufsize < 10)
return 0;
if (L[_0] & 0xfffff || L[_1]) {
return strcp(buf, "NaN");
}
b = buf;
if (sign)
*b++ = '-';
return strcp(b, "Infinity");
}
if (L[_1] == 0 && (L[_0] ^ sign) == 0 ) {
b = buf;
#if !defined(IGNORE_ZERO_SIGN)
if (L[_0] & 0x80000000L)
*b++ = '-';
#endif
*b++ = '0';
*b = 0;
return b;
}
bits[0] = L[_1];
bits[1] = L[_0] & 0xfffff;
if ( (ex = (L[_0] >> 20) & 0x7ff) !=0)
bits[1] |= 0x100000;
else
ex = 1;
ex -= 0x3ff + 52;
mode = 2;
if (ndig <= 0)
mode = 0;
i = STRTOG_Normal;
if (sign)
i = STRTOG_Normal | STRTOG_Neg;
s = gdtoa(fpi, ex, bits, &i, mode, ndig, &decpt, &se);
return g__fmt(buf, s, se, decpt, sign, bufsize);
}
