





























#include "gdtoaimp.h"
#include <string.h>

char *
#if defined(KR_headers)
g_ddfmt(buf, dd0, ndig, bufsize) char *buf; double *dd0; int ndig; size_t bufsize;
#else
g_ddfmt(char *buf, double *dd0, int ndig, size_t bufsize)
#endif
{
FPI fpi;
char *b, *s, *se;
ULong *L, bits0[4], *bits, *zx;
int bx, by, decpt, ex, ey, i, j, mode;
Bigint *x, *y, *z;
U *dd, ddx[2];
#if defined(Honor_FLT_ROUNDS)
int Rounding;
#if defined(Trust_FLT_ROUNDS)
Rounding = Flt_Rounds;
#else
Rounding = 1;
switch(fegetround()) {
case FE_TOWARDZERO: Rounding = 0; break;
case FE_UPWARD: Rounding = 2; break;
case FE_DOWNWARD: Rounding = 3;
}
#endif
#else
#define Rounding FPI_Round_near
#endif

if (bufsize < 10 || bufsize < ndig + 8)
return 0;

dd = (U*)dd0;
L = dd->L;
if ((L[_0] & 0x7ff00000L) == 0x7ff00000L) {

if (L[_0] & 0xfffff || L[_1]) {
nanret:
return strcp(buf, "NaN");
}
if ((L[2+_0] & 0x7ff00000) == 0x7ff00000) {
if (L[2+_0] & 0xfffff || L[2+_1])
goto nanret;
if ((L[_0] ^ L[2+_0]) & 0x80000000L)
goto nanret;
}
infret:
b = buf;
if (L[_0] & 0x80000000L)
*b++ = '-';
return strcp(b, "Infinity");
}
if ((L[2+_0] & 0x7ff00000) == 0x7ff00000) {
L += 2;
if (L[_0] & 0xfffff || L[_1])
goto nanret;
goto infret;
}
if (dval(&dd[0]) + dval(&dd[1]) == 0.) {
b = buf;
#if !defined(IGNORE_ZERO_SIGN)
if (L[_0] & L[2+_0] & 0x80000000L)
*b++ = '-';
#endif
*b++ = '0';
*b = 0;
return b;
}
if ((L[_0] & 0x7ff00000L) < (L[2+_0] & 0x7ff00000L)) {
dval(&ddx[1]) = dval(&dd[0]);
dval(&ddx[0]) = dval(&dd[1]);
dd = ddx;
L = dd->L;
}
z = d2b(dval(&dd[0]), &ex, &bx);
if (dval(&dd[1]) == 0.)
goto no_y;
x = z;
y = d2b(dval(&dd[1]), &ey, &by);
if ( (i = ex - ey) !=0) {
if (i > 0) {
x = lshift(x, i);
ex = ey;
}
else
y = lshift(y, -i);
}
if ((L[_0] ^ L[2+_0]) & 0x80000000L) {
z = diff(x, y);
if (L[_0] & 0x80000000L)
z->sign = 1 - z->sign;
}
else {
z = sum(x, y);
if (L[_0] & 0x80000000L)
z->sign = 1;
}
Bfree(x);
Bfree(y);
no_y:
bits = zx = z->x;
for(i = 0; !*zx; zx++)
i += 32;
i += lo0bits(zx);
if (i) {
rshift(z, i);
ex += i;
}
fpi.nbits = z->wds * 32 - hi0bits(z->x[j = z->wds-1]);
if (fpi.nbits < 106) {
fpi.nbits = 106;
if (j < 3) {
for(i = 0; i <= j; i++)
bits0[i] = bits[i];
while(i < 4)
bits0[i++] = 0;
bits = bits0;
}
}
mode = 2;
if (ndig <= 0) {
if (bufsize < (int)(fpi.nbits * .301029995664) + 10) {
Bfree(z);
return 0;
}
mode = 0;
}
fpi.emin = 1-1023-53+1;
fpi.emax = 2046-1023-106+1;
fpi.rounding = Rounding;
fpi.sudden_underflow = 0;
i = STRTOG_Normal;
s = gdtoa(&fpi, ex, bits, &i, mode, ndig, &decpt, &se);
b = g__fmt(buf, s, se, decpt, z->sign, bufsize);
Bfree(z);
return b;
}
