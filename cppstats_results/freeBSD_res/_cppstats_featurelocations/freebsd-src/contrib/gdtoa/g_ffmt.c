






























#include "gdtoaimp.h"

char*
#if defined(KR_headers)
g_ffmt(buf, f, ndig, bufsize) char *buf; float *f; int ndig; size_t bufsize;
#else
g_ffmt(char *buf, float *f, int ndig, size_t bufsize)
#endif
{
static FPI fpi0 = { 24, 1-127-24+1, 254-127-24+1, 1, 0 };
char *b, *s, *se;
ULong bits[1], *L, sign;
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

L = (ULong*)f;
sign = L[0] & 0x80000000L;
if ((L[0] & 0x7f800000) == 0x7f800000) {

if (L[0] & 0x7fffff) {
return strcp(buf, "NaN");
}
b = buf;
if (sign)
*b++ = '-';
return strcp(b, "Infinity");
}
if (*f == 0.) {
b = buf;
#if !defined(IGNORE_ZERO_SIGN)
if (L[0] & 0x80000000L)
*b++ = '-';
#endif
*b++ = '0';
*b = 0;
return b;
}
bits[0] = L[0] & 0x7fffff;
if ( (ex = (L[0] >> 23) & 0xff) !=0)
bits[0] |= 0x800000;
else
ex = 1;
ex -= 0x7f + 23;
mode = 2;
if (ndig <= 0) {
if (bufsize < 16)
return 0;
mode = 0;
}
i = STRTOG_Normal;
s = gdtoa(fpi, ex, bits, &i, mode, ndig, &decpt, &se);
return g__fmt(buf, s, se, decpt, sign, bufsize);
}
