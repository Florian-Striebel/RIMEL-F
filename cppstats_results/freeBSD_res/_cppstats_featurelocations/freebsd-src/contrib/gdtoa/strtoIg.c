






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtoIg(s00, se, fpi, exp, B, rvp) CONST char *s00; char **se; FPI *fpi; Long *exp; Bigint **B; int *rvp;
#else
strtoIg(CONST char *s00, char **se, FPI *fpi, Long *exp, Bigint **B, int *rvp)
#endif
{
Bigint *b, *b1;
int i, nb, nw, nw1, rv, rv1, swap;
unsigned int nb1, nb11;
Long e1;

b = *B;
rv = strtodg(s00, se, fpi, exp, b->x);
if (!(rv & STRTOG_Inexact)) {
B[1] = 0;
return *rvp = rv;
}
e1 = exp[0];
rv1 = rv ^ STRTOG_Inexact;
b1 = Balloc(b->k);
Bcopy(b1, b);
nb = fpi->nbits;
nb1 = nb & 31;
nb11 = (nb1 - 1) & 31;
nw = b->wds;
nw1 = nw - 1;
if (rv & STRTOG_Inexlo) {
swap = 0;
b1 = increment(b1);
if ((rv & STRTOG_Retmask) == STRTOG_Zero) {
if (fpi->sudden_underflow) {
b1->x[0] = 0;
b1->x[nw1] = 1L << nb11;
rv1 += STRTOG_Normal - STRTOG_Zero;
rv1 &= ~STRTOG_Underflow;
goto swapcheck;
}
rv1 &= STRTOG_Inexlo | STRTOG_Underflow | STRTOG_Zero;
rv1 |= STRTOG_Inexhi | STRTOG_Denormal;
goto swapcheck;
}
if (b1->wds > nw
|| (nb1 && b1->x[nw1] & 1L << nb1)) {
if (++e1 > fpi->emax)
rv1 = STRTOG_Infinite | STRTOG_Inexhi;
rshift(b1, 1);
}
else if ((rv & STRTOG_Retmask) == STRTOG_Denormal) {
if (b1->x[nw1] & 1L << nb11) {
rv1 += STRTOG_Normal - STRTOG_Denormal;
rv1 &= ~STRTOG_Underflow;
}
}
}
else {
swap = STRTOG_Neg;
if ((rv & STRTOG_Retmask) == STRTOG_Infinite) {
b1 = set_ones(b1, nb);
e1 = fpi->emax;
rv1 = STRTOG_Normal | STRTOG_Inexlo;
goto swapcheck;
}
decrement(b1);
if ((rv & STRTOG_Retmask) == STRTOG_Denormal) {
for(i = nw1; !b1->x[i]; --i)
if (!i) {
rv1 = STRTOG_Zero | STRTOG_Inexlo;
break;
}
goto swapcheck;
}
if (!(b1->x[nw1] & 1L << nb11)) {
if (e1 == fpi->emin) {
if (fpi->sudden_underflow)
rv1 += STRTOG_Zero - STRTOG_Normal;
else
rv1 += STRTOG_Denormal - STRTOG_Normal;
rv1 |= STRTOG_Underflow;
}
else {
b1 = lshift(b1, 1);
b1->x[0] |= 1;
--e1;
}
}
}
swapcheck:
if (swap ^ (rv & STRTOG_Neg)) {
rvp[0] = rv1;
rvp[1] = rv;
B[0] = b1;
B[1] = b;
exp[1] = exp[0];
exp[0] = e1;
}
else {
rvp[0] = rv;
rvp[1] = rv1;
B[1] = b1;
exp[1] = e1;
}
return rv;
}
