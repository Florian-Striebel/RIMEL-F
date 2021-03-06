






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtopdd(s, sp, dd) CONST char *s; char **sp; double *dd;
#else
strtopdd(CONST char *s, char **sp, double *dd)
#endif
{
#if defined(Sudden_Underflow)
static FPI fpi0 = { 106, 1-1023, 2046-1023-106+1, 1, 1 };
#else
static FPI fpi0 = { 106, 1-1023-53+1, 2046-1023-106+1, 1, 0 };
#endif
ULong bits[4];
Long exp;
int i, j, rv;
typedef union {
double d[2];
ULong L[4];
} U;
U *u;
#if defined(Honor_FLT_ROUNDS)
#include "gdtoa_fltrnds.h"
#else
#define fpi &fpi0
#endif

rv = strtodg(s, sp, fpi, &exp, bits);
u = (U*)dd;
switch(rv & STRTOG_Retmask) {
case STRTOG_NoNumber:
case STRTOG_Zero:
u->d[0] = u->d[1] = 0.;
break;

case STRTOG_Normal:
u->L[_1] = (bits[1] >> 21 | bits[2] << 11) & 0xffffffffL;
u->L[_0] = (bits[2] >> 21) | ((bits[3] << 11) & 0xfffff)
| ((exp + 0x3ff + 105) << 20);
exp += 0x3ff + 52;
if (bits[1] &= 0x1fffff) {
i = hi0bits(bits[1]) - 11;
if (i >= exp) {
i = exp - 1;
exp = 0;
}
else
exp -= i;
if (i > 0) {
bits[1] = bits[1] << i | bits[0] >> (32-i);
bits[0] = bits[0] << i & 0xffffffffL;
}
}
else if (bits[0]) {
i = hi0bits(bits[0]) + 21;
if (i >= exp) {
i = exp - 1;
exp = 0;
}
else
exp -= i;
if (i < 32) {
bits[1] = bits[0] >> (32 - i);
bits[0] = bits[0] << i & 0xffffffffL;
}
else {
bits[1] = bits[0] << (i - 32);
bits[0] = 0;
}
}
else {
u->L[2] = u->L[3] = 0;
break;
}
u->L[2+_1] = bits[0];
u->L[2+_0] = (bits[1] & 0xfffff) | (exp << 20);
break;

case STRTOG_Denormal:
if (bits[3])
goto nearly_normal;
if (bits[2])
goto partly_normal;
if (bits[1] & 0xffe00000)
goto hardly_normal;

u->L[2] = u->L[3] = 0;
u->L[_1] = bits[0];
u->L[_0] = bits[1];
break;

nearly_normal:
i = hi0bits(bits[3]) - 11;
j = 32 - i;
u->L[_0] = ((bits[3] << i | bits[2] >> j) & 0xfffff)
| ((65 - i) << 20);
u->L[_1] = (bits[2] << i | bits[1] >> j) & 0xffffffffL;
u->L[2+_0] = bits[1] & ((1L << j) - 1);
u->L[2+_1] = bits[0];
break;

partly_normal:
i = hi0bits(bits[2]) - 11;
if (i < 0) {
j = -i;
i += 32;
u->L[_0] = (bits[2] >> j & 0xfffff) | (33 + j) << 20;
u->L[_1] = ((bits[2] << i) | (bits[1] >> j)) & 0xffffffffL;
u->L[2+_0] = bits[1] & ((1L << j) - 1);
u->L[2+_1] = bits[0];
break;
}
if (i == 0) {
u->L[_0] = (bits[2] & 0xfffff) | (33 << 20);
u->L[_1] = bits[1];
u->L[2+_0] = 0;
u->L[2+_1] = bits[0];
break;
}
j = 32 - i;
u->L[_0] = (((bits[2] << i) | (bits[1] >> j)) & 0xfffff)
| ((j + 1) << 20);
u->L[_1] = (bits[1] << i | bits[0] >> j) & 0xffffffffL;
u->L[2+_0] = 0;
u->L[2+_1] = bits[0] & ((1L << j) - 1);
break;

hardly_normal:
j = 11 - hi0bits(bits[1]);
i = 32 - j;
u->L[_0] = (bits[1] >> j & 0xfffff) | ((j + 1) << 20);
u->L[_1] = (bits[1] << i | bits[0] >> j) & 0xffffffffL;
u->L[2+_0] = 0;
u->L[2+_1] = bits[0] & ((1L << j) - 1);
break;

case STRTOG_Infinite:
u->L[_0] = u->L[2+_0] = 0x7ff00000;
u->L[_1] = u->L[2+_1] = 0;
break;

case STRTOG_NaN:
u->L[0] = u->L[2] = d_QNAN0;
u->L[1] = u->L[3] = d_QNAN1;
}
if (rv & STRTOG_Neg) {
u->L[ _0] |= 0x80000000L;
u->L[2+_0] |= 0x80000000L;
}
return rv;
}
