






























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

char*
#if defined(KR_headers)
g_xfmt(buf, V, ndig, bufsize) char *buf; char *V; int ndig; size_t bufsize;
#else
g_xfmt(char *buf, void *V, int ndig, size_t bufsize)
#endif
{
static FPI fpi0 = { 64, 1-16383-64+1, 32766 - 16383 - 64 + 1, 1, 0 };
char *b, *s, *se;
ULong bits[2], sign;
UShort *L;
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

L = (UShort *)V;
sign = L[_0] & 0x8000;
bits[1] = (L[_1] << 16) | L[_2];
bits[0] = (L[_3] << 16) | L[_4];
if ( (ex = L[_0] & 0x7fff) !=0) {
if (ex == 0x7fff) {

if (bits[0] | bits[1])
b = strcp(buf, "NaN");
else {
b = buf;
if (sign)
*b++ = '-';
b = strcp(b, "Infinity");
}
return b;
}
i = STRTOG_Normal;
}
else if (bits[0] | bits[1]) {
i = STRTOG_Denormal;
ex = 1;
}
else {
b = buf;
#if !defined(IGNORE_ZERO_SIGN)
if (sign)
*b++ = '-';
#endif
*b++ = '0';
*b = 0;
return b;
}
ex -= 0x3fff + 63;
mode = 2;
if (ndig <= 0) {
if (bufsize < 32)
return 0;
mode = 0;
}
s = gdtoa(fpi, ex, bits, &i, mode, ndig, &decpt, &se);
return g__fmt(buf, s, se, decpt, sign, bufsize);
}
