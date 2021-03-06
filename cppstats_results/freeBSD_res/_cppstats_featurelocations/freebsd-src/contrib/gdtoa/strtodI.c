






























#include "gdtoaimp.h"

static double
#if defined(KR_headers)
ulpdown(d) U *d;
#else
ulpdown(U *d)
#endif
{
double u;
ULong *L = d->L;

u = ulp(d);
if (!(L[_1] | (L[_0] & 0xfffff))
&& (L[_0] & 0x7ff00000) > 0x00100000)
u *= 0.5;
return u;
}

int
#if defined(KR_headers)
strtodI(s, sp, dd) CONST char *s; char **sp; double *dd;
#else
strtodI(CONST char *s, char **sp, double *dd)
#endif
{
static FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI };
ULong bits[2], sign;
Long exp;
int j, k;
U *u;

k = strtodg(s, sp, &fpi, &exp, bits);
u = (U*)dd;
sign = k & STRTOG_Neg ? 0x80000000L : 0;
switch(k & STRTOG_Retmask) {
case STRTOG_NoNumber:
dval(&u[0]) = dval(&u[1]) = 0.;
break;

case STRTOG_Zero:
dval(&u[0]) = dval(&u[1]) = 0.;
#if defined(Sudden_Underflow)
if (k & STRTOG_Inexact) {
if (sign)
word0(&u[0]) = 0x80100000L;
else
word0(&u[1]) = 0x100000L;
}
break;
#else
goto contain;
#endif

case STRTOG_Denormal:
word1(&u[0]) = bits[0];
word0(&u[0]) = bits[1];
goto contain;

case STRTOG_Normal:
word1(&u[0]) = bits[0];
word0(&u[0]) = (bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20);
contain:
j = k & STRTOG_Inexact;
if (sign) {
word0(&u[0]) |= sign;
j = STRTOG_Inexact - j;
}
switch(j) {
case STRTOG_Inexlo:
#if defined(Sudden_Underflow)
if ((u->L[_0] & 0x7ff00000) < 0x3500000) {
word0(&u[1]) = word0(&u[0]) + 0x3500000;
word1(&u[1]) = word1(&u[0]);
dval(&u[1]) += ulp(&u[1]);
word0(&u[1]) -= 0x3500000;
if (!(word0(&u[1]) & 0x7ff00000)) {
word0(&u[1]) = sign;
word1(&u[1]) = 0;
}
}
else
#endif
dval(&u[1]) = dval(&u[0]) + ulp(&u[0]);
break;
case STRTOG_Inexhi:
dval(&u[1]) = dval(&u[0]);
#if defined(Sudden_Underflow)
if ((word0(&u[0]) & 0x7ff00000) < 0x3500000) {
word0(&u[0]) += 0x3500000;
dval(&u[0]) -= ulpdown(u);
word0(&u[0]) -= 0x3500000;
if (!(word0(&u[0]) & 0x7ff00000)) {
word0(&u[0]) = sign;
word1(&u[0]) = 0;
}
}
else
#endif
dval(&u[0]) -= ulpdown(u);
break;
default:
dval(&u[1]) = dval(&u[0]);
}
break;

case STRTOG_Infinite:
word0(&u[0]) = word0(&u[1]) = sign | 0x7ff00000;
word1(&u[0]) = word1(&u[1]) = 0;
if (k & STRTOG_Inexact) {
if (sign) {
word0(&u[1]) = 0xffefffffL;
word1(&u[1]) = 0xffffffffL;
}
else {
word0(&u[0]) = 0x7fefffffL;
word1(&u[0]) = 0xffffffffL;
}
}
break;

case STRTOG_NaN:
u->L[0] = (u+1)->L[0] = d_QNAN0;
u->L[1] = (u+1)->L[1] = d_QNAN1;
break;

case STRTOG_NaNbits:
word0(&u[0]) = word0(&u[1]) = 0x7ff00000 | sign | bits[1];
word1(&u[0]) = word1(&u[1]) = bits[0];
}
return k;
}
