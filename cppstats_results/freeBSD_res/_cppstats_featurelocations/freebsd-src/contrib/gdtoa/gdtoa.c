






























#include "gdtoaimp.h"

static Bigint *
#if defined(KR_headers)
bitstob(bits, nbits, bbits) ULong *bits; int nbits; int *bbits;
#else
bitstob(ULong *bits, int nbits, int *bbits)
#endif
{
int i, k;
Bigint *b;
ULong *be, *x, *x0;

i = ULbits;
k = 0;
while(i < nbits) {
i <<= 1;
k++;
}
#if !defined(Pack_32)
if (!k)
k = 1;
#endif
b = Balloc(k);
be = bits + ((nbits - 1) >> kshift);
x = x0 = b->x;
do {
*x++ = *bits & ALL_ON;
#if defined(Pack_16)
*x++ = (*bits >> 16) & ALL_ON;
#endif
} while(++bits <= be);
i = x - x0;
while(!x0[--i])
if (!i) {
b->wds = 0;
*bbits = 0;
goto ret;
}
b->wds = i + 1;
*bbits = i*ULbits + 32 - hi0bits(b->x[i]);
ret:
return b;
}



































char *
gdtoa
#if defined(KR_headers)
(fpi, be, bits, kindp, mode, ndigits, decpt, rve)
FPI *fpi; int be; ULong *bits;
int *kindp, mode, ndigits, *decpt; char **rve;
#else
(FPI *fpi, int be, ULong *bits, int *kindp, int mode, int ndigits, int *decpt, char **rve)
#endif
{



































int bbits, b2, b5, be0, dig, i, ieps, ilim, ilim0, ilim1, inex;
int j, j1, k, k0, k_check, kind, leftright, m2, m5, nbits;
int rdir, s2, s5, spec_case, try_quick;
Long L;
Bigint *b, *b1, *delta, *mlo, *mhi, *mhi1, *S;
double d2, ds;
char *s, *s0;
U d, eps;

#if !defined(MULTIPLE_THREADS)
if (dtoa_result) {
freedtoa(dtoa_result);
dtoa_result = 0;
}
#endif
inex = 0;
kind = *kindp &= ~STRTOG_Inexact;
switch(kind & STRTOG_Retmask) {
case STRTOG_Zero:
goto ret_zero;
case STRTOG_Normal:
case STRTOG_Denormal:
break;
case STRTOG_Infinite:
*decpt = -32768;
return nrv_alloc("Infinity", rve, 8);
case STRTOG_NaN:
*decpt = -32768;
return nrv_alloc("NaN", rve, 3);
default:
return 0;
}
b = bitstob(bits, nbits = fpi->nbits, &bbits);
be0 = be;
if ( (i = trailz(b)) !=0) {
rshift(b, i);
be += i;
bbits -= i;
}
if (!b->wds) {
Bfree(b);
ret_zero:
*decpt = 1;
return nrv_alloc("0", rve, 1);
}

dval(&d) = b2d(b, &i);
i = be + bbits - 1;
word0(&d) &= Frac_mask1;
word0(&d) |= Exp_11;
#if defined(IBM)
if ( (j = 11 - hi0bits(word0(&d) & Frac_mask)) !=0)
dval(&d) /= 1 << j;
#endif






















#if defined(IBM)
i <<= 2;
i += j;
#endif
ds = (dval(&d)-1.5)*0.289529654602168 + 0.1760912590558 + i*0.301029995663981;


if ((j = i) < 0)
j = -j;
if ((j -= 1077) > 0)
ds += j * 7e-17;

k = (int)ds;
if (ds < 0. && ds != k)
k--;
k_check = 1;
#if defined(IBM)
j = be + bbits - 1;
if ( (j1 = j & 3) !=0)
dval(&d) *= 1 << j1;
word0(&d) += j << Exp_shift - 2 & Exp_mask;
#else
word0(&d) += (be + bbits - 1) << Exp_shift;
#endif
if (k >= 0 && k <= Ten_pmax) {
if (dval(&d) < tens[k])
k--;
k_check = 0;
}
j = bbits - i - 1;
if (j >= 0) {
b2 = 0;
s2 = j;
}
else {
b2 = -j;
s2 = 0;
}
if (k >= 0) {
b5 = 0;
s5 = k;
s2 += k;
}
else {
b2 -= k;
b5 = -k;
s5 = 0;
}
if (mode < 0 || mode > 9)
mode = 0;
try_quick = 1;
if (mode > 5) {
mode -= 4;
try_quick = 0;
}
else if (i >= -4 - Emin || i < Emin)
try_quick = 0;
leftright = 1;
ilim = ilim1 = -1;

switch(mode) {
case 0:
case 1:
i = (int)(nbits * .30103) + 3;
ndigits = 0;
break;
case 2:
leftright = 0;

case 4:
if (ndigits <= 0)
ndigits = 1;
ilim = ilim1 = i = ndigits;
break;
case 3:
leftright = 0;

case 5:
i = ndigits + k + 1;
ilim = i;
ilim1 = i - 1;
if (i <= 0)
i = 1;
}
s = s0 = rv_alloc(i);

if ( (rdir = fpi->rounding - 1) !=0) {
if (rdir < 0)
rdir = 2;
if (kind & STRTOG_Neg)
rdir = 3 - rdir;
}



if (ilim >= 0 && ilim <= Quick_max && try_quick && !rdir
#if !defined(IMPRECISE_INEXACT)
&& k == 0
#endif
) {



i = 0;
d2 = dval(&d);
#if defined(IBM)
if ( (j = 11 - hi0bits(word0(&d) & Frac_mask)) !=0)
dval(&d) /= 1 << j;
#endif
k0 = k;
ilim0 = ilim;
ieps = 2;
if (k > 0) {
ds = tens[k&0xf];
j = k >> 4;
if (j & Bletch) {

j &= Bletch - 1;
dval(&d) /= bigtens[n_bigtens-1];
ieps++;
}
for(; j; j >>= 1, i++)
if (j & 1) {
ieps++;
ds *= bigtens[i];
}
}
else {
ds = 1.;
if ( (j1 = -k) !=0) {
dval(&d) *= tens[j1 & 0xf];
for(j = j1 >> 4; j; j >>= 1, i++)
if (j & 1) {
ieps++;
dval(&d) *= bigtens[i];
}
}
}
if (k_check && dval(&d) < 1. && ilim > 0) {
if (ilim1 <= 0)
goto fast_failed;
ilim = ilim1;
k--;
dval(&d) *= 10.;
ieps++;
}
dval(&eps) = ieps*dval(&d) + 7.;
word0(&eps) -= (P-1)*Exp_msk1;
if (ilim == 0) {
S = mhi = 0;
dval(&d) -= 5.;
if (dval(&d) > dval(&eps))
goto one_digit;
if (dval(&d) < -dval(&eps))
goto no_digits;
goto fast_failed;
}
#if !defined(No_leftright)
if (leftright) {



dval(&eps) = ds*0.5/tens[ilim-1] - dval(&eps);
for(i = 0;;) {
L = (Long)(dval(&d)/ds);
dval(&d) -= L*ds;
*s++ = '0' + (int)L;
if (dval(&d) < dval(&eps)) {
if (dval(&d))
inex = STRTOG_Inexlo;
goto ret1;
}
if (ds - dval(&d) < dval(&eps))
goto bump_up;
if (++i >= ilim)
break;
dval(&eps) *= 10.;
dval(&d) *= 10.;
}
}
else {
#endif

dval(&eps) *= tens[ilim-1];
for(i = 1;; i++, dval(&d) *= 10.) {
if ( (L = (Long)(dval(&d)/ds)) !=0)
dval(&d) -= L*ds;
*s++ = '0' + (int)L;
if (i == ilim) {
ds *= 0.5;
if (dval(&d) > ds + dval(&eps))
goto bump_up;
else if (dval(&d) < ds - dval(&eps)) {
if (dval(&d))
inex = STRTOG_Inexlo;
goto clear_trailing0;
}
break;
}
}
#if !defined(No_leftright)
}
#endif
fast_failed:
s = s0;
dval(&d) = d2;
k = k0;
ilim = ilim0;
}



if (be >= 0 && k <= Int_max) {

ds = tens[k];
if (ndigits < 0 && ilim <= 0) {
S = mhi = 0;
if (ilim < 0 || dval(&d) <= 5*ds)
goto no_digits;
goto one_digit;
}
for(i = 1;; i++, dval(&d) *= 10.) {
L = dval(&d) / ds;
dval(&d) -= L*ds;
#if defined(Check_FLT_ROUNDS)

if (dval(&d) < 0) {
L--;
dval(&d) += ds;
}
#endif
*s++ = '0' + (int)L;
if (dval(&d) == 0.)
break;
if (i == ilim) {
if (rdir) {
if (rdir == 1)
goto bump_up;
inex = STRTOG_Inexlo;
goto ret1;
}
dval(&d) += dval(&d);
#if defined(ROUND_BIASED)
if (dval(&d) >= ds)
#else
if (dval(&d) > ds || (dval(&d) == ds && L & 1))
#endif
{
bump_up:
inex = STRTOG_Inexhi;
while(*--s == '9')
if (s == s0) {
k++;
*s = '0';
break;
}
++*s++;
}
else {
inex = STRTOG_Inexlo;
clear_trailing0:
while(*--s == '0'){}
++s;
}
break;
}
}
goto ret1;
}

m2 = b2;
m5 = b5;
mhi = mlo = 0;
if (leftright) {
i = nbits - bbits;
if (be - i++ < fpi->emin && mode != 3 && mode != 5) {

i = be - fpi->emin + 1;
if (mode >= 2 && ilim > 0 && ilim < i)
goto small_ilim;
}
else if (mode >= 2) {
small_ilim:
j = ilim - 1;
if (m5 >= j)
m5 -= j;
else {
s5 += j -= m5;
b5 += j;
m5 = 0;
}
if ((i = ilim) < 0) {
m2 -= i;
i = 0;
}
}
b2 += i;
s2 += i;
mhi = i2b(1);
}
if (m2 > 0 && s2 > 0) {
i = m2 < s2 ? m2 : s2;
b2 -= i;
m2 -= i;
s2 -= i;
}
if (b5 > 0) {
if (leftright) {
if (m5 > 0) {
mhi = pow5mult(mhi, m5);
b1 = mult(mhi, b);
Bfree(b);
b = b1;
}
if ( (j = b5 - m5) !=0)
b = pow5mult(b, j);
}
else
b = pow5mult(b, b5);
}
S = i2b(1);
if (s5 > 0)
S = pow5mult(S, s5);



spec_case = 0;
if (mode < 2) {
if (bbits == 1 && be0 > fpi->emin + 1) {

b2++;
s2++;
spec_case = 1;
}
}








i = ((s5 ? hi0bits(S->x[S->wds-1]) : ULbits - 1) - s2 - 4) & kmask;
m2 += i;
if ((b2 += i) > 0)
b = lshift(b, b2);
if ((s2 += i) > 0)
S = lshift(S, s2);
if (k_check) {
if (cmp(b,S) < 0) {
k--;
b = multadd(b, 10, 0);
if (leftright)
mhi = multadd(mhi, 10, 0);
ilim = ilim1;
}
}
if (ilim <= 0 && mode > 2) {
if (ilim < 0 || cmp(b,S = multadd(S,5,0)) <= 0) {

no_digits:
k = -1 - ndigits;
inex = STRTOG_Inexlo;
goto ret;
}
one_digit:
inex = STRTOG_Inexhi;
*s++ = '1';
k++;
goto ret;
}
if (leftright) {
if (m2 > 0)
mhi = lshift(mhi, m2);





mlo = mhi;
if (spec_case) {
mhi = Balloc(mhi->k);
Bcopy(mhi, mlo);
mhi = lshift(mhi, 1);
}

for(i = 1;;i++) {
dig = quorem(b,S) + '0';



j = cmp(b, mlo);
delta = diff(S, mhi);
j1 = delta->sign ? 1 : cmp(b, delta);
Bfree(delta);
#if !defined(ROUND_BIASED)
if (j1 == 0 && !mode && !(bits[0] & 1) && !rdir) {
if (dig == '9')
goto round_9_up;
if (j <= 0) {
if (b->wds > 1 || b->x[0])
inex = STRTOG_Inexlo;
}
else {
dig++;
inex = STRTOG_Inexhi;
}
*s++ = dig;
goto ret;
}
#endif
if (j < 0 || (j == 0 && !mode
#if !defined(ROUND_BIASED)
&& !(bits[0] & 1)
#endif
)) {
if (rdir && (b->wds > 1 || b->x[0])) {
if (rdir == 2) {
inex = STRTOG_Inexlo;
goto accept;
}
while (cmp(S,mhi) > 0) {
*s++ = dig;
mhi1 = multadd(mhi, 10, 0);
if (mlo == mhi)
mlo = mhi1;
mhi = mhi1;
b = multadd(b, 10, 0);
dig = quorem(b,S) + '0';
}
if (dig++ == '9')
goto round_9_up;
inex = STRTOG_Inexhi;
goto accept;
}
if (j1 > 0) {
b = lshift(b, 1);
j1 = cmp(b, S);
#if defined(ROUND_BIASED)
if (j1 >= 0
#else
if ((j1 > 0 || (j1 == 0 && dig & 1))
#endif
&& dig++ == '9')
goto round_9_up;
inex = STRTOG_Inexhi;
}
if (b->wds > 1 || b->x[0])
inex = STRTOG_Inexlo;
accept:
*s++ = dig;
goto ret;
}
if (j1 > 0 && rdir != 2) {
if (dig == '9') {
round_9_up:
*s++ = '9';
inex = STRTOG_Inexhi;
goto roundoff;
}
inex = STRTOG_Inexhi;
*s++ = dig + 1;
goto ret;
}
*s++ = dig;
if (i == ilim)
break;
b = multadd(b, 10, 0);
if (mlo == mhi)
mlo = mhi = multadd(mhi, 10, 0);
else {
mlo = multadd(mlo, 10, 0);
mhi = multadd(mhi, 10, 0);
}
}
}
else
for(i = 1;; i++) {
*s++ = dig = quorem(b,S) + '0';
if (i >= ilim)
break;
b = multadd(b, 10, 0);
}



if (rdir) {
if (rdir == 2 || (b->wds <= 1 && !b->x[0]))
goto chopzeros;
goto roundoff;
}
b = lshift(b, 1);
j = cmp(b, S);
#if defined(ROUND_BIASED)
if (j >= 0)
#else
if (j > 0 || (j == 0 && dig & 1))
#endif
{
roundoff:
inex = STRTOG_Inexhi;
while(*--s == '9')
if (s == s0) {
k++;
*s++ = '1';
goto ret;
}
++*s++;
}
else {
chopzeros:
if (b->wds > 1 || b->x[0])
inex = STRTOG_Inexlo;
while(*--s == '0'){}
++s;
}
ret:
Bfree(S);
if (mhi) {
if (mlo && mlo != mhi)
Bfree(mlo);
Bfree(mhi);
}
ret1:
Bfree(b);
*s = 0;
*decpt = k + 1;
if (rve)
*rve = s;
*kindp |= inex;
return s0;
}
