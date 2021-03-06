
































#include "gdtoaimp.h"
#if !defined(NO_FENV_H)
#include <fenv.h>
#endif

#if defined(USE_LOCALE)
#include "locale.h"
#endif

#if defined(IEEE_Arith)
#if !defined(NO_IEEE_Scale)
#define Avoid_Underflow
#undef tinytens


static CONST double tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128,
9007199254740992.*9007199254740992.e-256
};
#endif
#endif

#if defined(Honor_FLT_ROUNDS)
#undef Check_FLT_ROUNDS
#define Check_FLT_ROUNDS
#else
#define Rounding Flt_Rounds
#endif

#if defined(Avoid_Underflow)
static double
sulp
#if defined(KR_headers)
(x, scale) U *x; int scale;
#else
(U *x, int scale)
#endif
{
U u;
double rv;
int i;

rv = ulp(x);
if (!scale || (i = 2*P + 1 - ((word0(x) & Exp_mask) >> Exp_shift)) <= 0)
return rv;
word0(&u) = Exp_1 + (i << Exp_shift);
word1(&u) = 0;
return rv * u.d;
}
#endif

double
strtod_l
#if defined(KR_headers)
(s00, se, loc) CONST char *s00; char **se; locale_t loc
#else
(CONST char *s00, char **se, locale_t loc)
#endif
{
#if defined(Avoid_Underflow)
int scale;
#endif
int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, decpt, dsign,
e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
CONST char *s, *s0, *s1;
double aadj;
Long L;
U adj, aadj1, rv, rv0;
ULong y, z;
Bigint *bb, *bb1, *bd, *bd0, *bs, *delta;
#if defined(Avoid_Underflow)
ULong Lsb, Lsb1;
#endif
#if defined(SET_INEXACT)
int inexact, oldinexact;
#endif
#if defined(USE_LOCALE)
#if defined(NO_LOCALE_CACHE)
char *decimalpoint = localeconv_l(loc)->decimal_point;
int dplen = strlen(decimalpoint);
#else
char *decimalpoint;
static char *decimalpoint_cache;
static int dplen;
if (!(s0 = decimalpoint_cache)) {
s0 = localeconv_l(loc)->decimal_point;
if ((decimalpoint_cache = (char*)MALLOC(strlen(s0) + 1))) {
strcpy(decimalpoint_cache, s0);
s0 = decimalpoint_cache;
}
dplen = strlen(s0);
}
decimalpoint = (char*)s0;
#endif
#else
#define dplen 1
#endif

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
#endif

sign = nz0 = nz = decpt = 0;
dval(&rv) = 0.;
for(s = s00;;s++) switch(*s) {
case '-':
sign = 1;

case '+':
if (*++s)
goto break2;

case 0:
goto ret0;
case '\t':
case '\n':
case '\v':
case '\f':
case '\r':
case ' ':
continue;
default:
goto break2;
}
break2:
if (*s == '0') {
#if !defined(NO_HEX_FP)
{
static FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI };
Long exp;
ULong bits[2];
switch(s[1]) {
case 'x':
case 'X':
{
#if defined(Honor_FLT_ROUNDS)
FPI fpi1 = fpi;
fpi1.rounding = Rounding;
#else
#define fpi1 fpi
#endif
switch((i = gethex(&s, &fpi1, &exp, &bb, sign)) & STRTOG_Retmask) {
case STRTOG_NoNumber:
s = s00;
sign = 0;
case STRTOG_Zero:
break;
default:
if (bb) {
copybits(bits, fpi.nbits, bb);
Bfree(bb);
}
ULtod(((U*)&rv)->L, bits, exp, i);
}}
goto ret;
}
}
#endif
nz0 = 1;
while(*++s == '0') ;
if (!*s)
goto ret;
}
s0 = s;
y = z = 0;
for(nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
if (nd < 9)
y = 10*y + c - '0';
else if (nd < 16)
z = 10*z + c - '0';
nd0 = nd;
#if defined(USE_LOCALE)
if (c == *decimalpoint) {
for(i = 1; decimalpoint[i]; ++i)
if (s[i] != decimalpoint[i])
goto dig_done;
s += i;
c = *s;
#else
if (c == '.') {
c = *++s;
#endif
decpt = 1;
if (!nd) {
for(; c == '0'; c = *++s)
nz++;
if (c > '0' && c <= '9') {
s0 = s;
nf += nz;
nz = 0;
goto have_dig;
}
goto dig_done;
}
for(; c >= '0' && c <= '9'; c = *++s) {
have_dig:
nz++;
if (c -= '0') {
nf += nz;
for(i = 1; i < nz; i++)
if (nd++ < 9)
y *= 10;
else if (nd <= DBL_DIG + 1)
z *= 10;
if (nd++ < 9)
y = 10*y + c;
else if (nd <= DBL_DIG + 1)
z = 10*z + c;
nz = 0;
}
}
}
dig_done:
e = 0;
if (c == 'e' || c == 'E') {
if (!nd && !nz && !nz0) {
goto ret0;
}
s00 = s;
esign = 0;
switch(c = *++s) {
case '-':
esign = 1;
case '+':
c = *++s;
}
if (c >= '0' && c <= '9') {
while(c == '0')
c = *++s;
if (c > '0' && c <= '9') {
L = c - '0';
s1 = s;
while((c = *++s) >= '0' && c <= '9')
L = 10*L + c - '0';
if (s - s1 > 8 || L > 19999)



e = 19999;
else
e = (int)L;
if (esign)
e = -e;
}
else
e = 0;
}
else
s = s00;
}
if (!nd) {
if (!nz && !nz0) {
#if defined(INFNAN_CHECK)

ULong bits[2];
static FPI fpinan =
{ 52, 1-1023-53+1, 2046-1023-53+1, 1, SI };
if (!decpt)
switch(c) {
case 'i':
case 'I':
if (match(&s,"nf")) {
--s;
if (!match(&s,"inity"))
++s;
word0(&rv) = 0x7ff00000;
word1(&rv) = 0;
goto ret;
}
break;
case 'n':
case 'N':
if (match(&s, "an")) {
#if !defined(No_Hex_NaN)
if (*s == '('
&& hexnan(&s, &fpinan, bits)
== STRTOG_NaNbits) {
word0(&rv) = 0x7ff80000 | bits[1];
word1(&rv) = bits[0];
}
else {
#endif
word0(&rv) = NAN_WORD0;
word1(&rv) = NAN_WORD1;
#if !defined(No_Hex_NaN)
}
#endif
goto ret;
}
}
#endif
ret0:
s = s00;
sign = 0;
}
goto ret;
}
e1 = e -= nf;






if (!nd0)
nd0 = nd;
k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
dval(&rv) = y;
if (k > 9) {
#if defined(SET_INEXACT)
if (k > DBL_DIG)
oldinexact = get_inexact();
#endif
dval(&rv) = tens[k - 9] * dval(&rv) + z;
}
bd0 = 0;
if (nd <= DBL_DIG
#if !defined(RND_PRODQUOT)
#if !defined(Honor_FLT_ROUNDS)
&& Flt_Rounds == 1
#endif
#endif
) {
if (!e)
goto ret;
#if !defined(ROUND_BIASED_without_Round_Up)
if (e > 0) {
if (e <= Ten_pmax) {
#if defined(VAX)
goto vax_ovfl_check;
#else
#if defined(Honor_FLT_ROUNDS)

if (sign) {
rv.d = -rv.d;
sign = 0;
}
#endif
rounded_product(dval(&rv), tens[e]);
goto ret;
#endif
}
i = DBL_DIG - nd;
if (e <= Ten_pmax + i) {



#if defined(Honor_FLT_ROUNDS)

if (sign) {
rv.d = -rv.d;
sign = 0;
}
#endif
e -= i;
dval(&rv) *= tens[i];
#if defined(VAX)



vax_ovfl_check:
word0(&rv) -= P*Exp_msk1;
rounded_product(dval(&rv), tens[e]);
if ((word0(&rv) & Exp_mask)
> Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
goto ovfl;
word0(&rv) += P*Exp_msk1;
#else
rounded_product(dval(&rv), tens[e]);
#endif
goto ret;
}
}
#if !defined(Inaccurate_Divide)
else if (e >= -Ten_pmax) {
#if defined(Honor_FLT_ROUNDS)

if (sign) {
rv.d = -rv.d;
sign = 0;
}
#endif
rounded_quotient(dval(&rv), tens[-e]);
goto ret;
}
#endif
#endif
}
e1 += nd - k;

#if defined(IEEE_Arith)
#if defined(SET_INEXACT)
inexact = 1;
if (k <= DBL_DIG)
oldinexact = get_inexact();
#endif
#if defined(Avoid_Underflow)
scale = 0;
#endif
#if defined(Honor_FLT_ROUNDS)
if (Rounding >= 2) {
if (sign)
Rounding = Rounding == 2 ? 0 : 2;
else
if (Rounding != 2)
Rounding = 0;
}
#endif
#endif



if (e1 > 0) {
if ( (i = e1 & 15) !=0)
dval(&rv) *= tens[i];
if (e1 &= ~15) {
if (e1 > DBL_MAX_10_EXP) {
ovfl:

#if defined(IEEE_Arith)
#if defined(Honor_FLT_ROUNDS)
switch(Rounding) {
case 0:
case 3:
word0(&rv) = Big0;
word1(&rv) = Big1;
break;
default:
word0(&rv) = Exp_mask;
word1(&rv) = 0;
}
#else
word0(&rv) = Exp_mask;
word1(&rv) = 0;
#endif
#if defined(SET_INEXACT)

dval(&rv0) = 1e300;
dval(&rv0) *= dval(&rv0);
#endif
#else
word0(&rv) = Big0;
word1(&rv) = Big1;
#endif
range_err:
if (bd0) {
Bfree(bb);
Bfree(bd);
Bfree(bs);
Bfree(bd0);
Bfree(delta);
}
#if !defined(NO_ERRNO)
errno = ERANGE;
#endif
goto ret;
}
e1 >>= 4;
for(j = 0; e1 > 1; j++, e1 >>= 1)
if (e1 & 1)
dval(&rv) *= bigtens[j];

word0(&rv) -= P*Exp_msk1;
dval(&rv) *= bigtens[j];
if ((z = word0(&rv) & Exp_mask)
> Exp_msk1*(DBL_MAX_EXP+Bias-P))
goto ovfl;
if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P)) {


word0(&rv) = Big0;
word1(&rv) = Big1;
}
else
word0(&rv) += P*Exp_msk1;
}
}
else if (e1 < 0) {
e1 = -e1;
if ( (i = e1 & 15) !=0)
dval(&rv) /= tens[i];
if (e1 >>= 4) {
if (e1 >= 1 << n_bigtens)
goto undfl;
#if defined(Avoid_Underflow)
if (e1 & Scale_Bit)
scale = 2*P;
for(j = 0; e1 > 0; j++, e1 >>= 1)
if (e1 & 1)
dval(&rv) *= tinytens[j];
if (scale && (j = 2*P + 1 - ((word0(&rv) & Exp_mask)
>> Exp_shift)) > 0) {

if (j >= 32) {
word1(&rv) = 0;
if (j >= 53)
word0(&rv) = (P+2)*Exp_msk1;
else
word0(&rv) &= 0xffffffff << (j-32);
}
else
word1(&rv) &= 0xffffffff << j;
}
#else
for(j = 0; e1 > 1; j++, e1 >>= 1)
if (e1 & 1)
dval(&rv) *= tinytens[j];

dval(&rv0) = dval(&rv);
dval(&rv) *= tinytens[j];
if (!dval(&rv)) {
dval(&rv) = 2.*dval(&rv0);
dval(&rv) *= tinytens[j];
#endif
if (!dval(&rv)) {
undfl:
dval(&rv) = 0.;
goto range_err;
}
#if !defined(Avoid_Underflow)
word0(&rv) = Tiny0;
word1(&rv) = Tiny1;



}
#endif
}
}





bd0 = s2b(s0, nd0, nd, y, dplen);

for(;;) {
bd = Balloc(bd0->k);
Bcopy(bd, bd0);
bb = d2b(dval(&rv), &bbe, &bbbits);
bs = i2b(1);

if (e >= 0) {
bb2 = bb5 = 0;
bd2 = bd5 = e;
}
else {
bb2 = bb5 = -e;
bd2 = bd5 = 0;
}
if (bbe >= 0)
bb2 += bbe;
else
bd2 -= bbe;
bs2 = bb2;
#if defined(Honor_FLT_ROUNDS)
if (Rounding != 1)
bs2++;
#endif
#if defined(Avoid_Underflow)
Lsb = LSB;
Lsb1 = 0;
j = bbe - scale;
i = j + bbbits - 1;
j = P + 1 - bbbits;
if (i < Emin) {
i = Emin - i;
j -= i;
if (i < 32)
Lsb <<= i;
else
Lsb1 = Lsb << (i-32);
}
#else
#if defined(Sudden_Underflow)
#if defined(IBM)
j = 1 + 4*P - 3 - bbbits + ((bbe + bbbits - 1) & 3);
#else
j = P + 1 - bbbits;
#endif
#else
j = bbe;
i = j + bbbits - 1;
if (i < Emin)
j += P - Emin;
else
j = P + 1 - bbbits;
#endif
#endif
bb2 += j;
bd2 += j;
#if defined(Avoid_Underflow)
bd2 += scale;
#endif
i = bb2 < bd2 ? bb2 : bd2;
if (i > bs2)
i = bs2;
if (i > 0) {
bb2 -= i;
bd2 -= i;
bs2 -= i;
}
if (bb5 > 0) {
bs = pow5mult(bs, bb5);
bb1 = mult(bs, bb);
Bfree(bb);
bb = bb1;
}
if (bb2 > 0)
bb = lshift(bb, bb2);
if (bd5 > 0)
bd = pow5mult(bd, bd5);
if (bd2 > 0)
bd = lshift(bd, bd2);
if (bs2 > 0)
bs = lshift(bs, bs2);
delta = diff(bb, bd);
dsign = delta->sign;
delta->sign = 0;
i = cmp(delta, bs);
#if defined(Honor_FLT_ROUNDS)
if (Rounding != 1) {
if (i < 0) {

if (!delta->x[0] && delta->wds <= 1) {

#if defined(SET_INEXACT)
inexact = 0;
#endif
break;
}
if (Rounding) {
if (dsign) {
dval(&adj) = 1.;
goto apply_adj;
}
}
else if (!dsign) {
dval(&adj) = -1.;
if (!word1(&rv)
&& !(word0(&rv) & Frac_mask)) {
y = word0(&rv) & Exp_mask;
#if defined(Avoid_Underflow)
if (!scale || y > 2*P*Exp_msk1)
#else
if (y)
#endif
{
delta = lshift(delta,Log2P);
if (cmp(delta, bs) <= 0)
dval(&adj) = -0.5;
}
}
apply_adj:
#if defined(Avoid_Underflow)
if (scale && (y = word0(&rv) & Exp_mask)
<= 2*P*Exp_msk1)
word0(&adj) += (2*P+1)*Exp_msk1 - y;
#else
#if defined(Sudden_Underflow)
if ((word0(&rv) & Exp_mask) <=
P*Exp_msk1) {
word0(&rv) += P*Exp_msk1;
dval(&rv) += adj*ulp(&rv);
word0(&rv) -= P*Exp_msk1;
}
else
#endif
#endif
dval(&rv) += adj.d*ulp(&rv);
}
break;
}
dval(&adj) = ratio(delta, bs);
if (adj.d < 1.)
dval(&adj) = 1.;
if (adj.d <= 0x7ffffffe) {

y = adj.d;
if (y != adj.d) {
if (!((Rounding>>1) ^ dsign))
y++;
dval(&adj) = y;
}
}
#if defined(Avoid_Underflow)
if (scale && (y = word0(&rv) & Exp_mask) <= 2*P*Exp_msk1)
word0(&adj) += (2*P+1)*Exp_msk1 - y;
#else
#if defined(Sudden_Underflow)
if ((word0(&rv) & Exp_mask) <= P*Exp_msk1) {
word0(&rv) += P*Exp_msk1;
dval(&adj) *= ulp(&rv);
if (dsign)
dval(&rv) += adj;
else
dval(&rv) -= adj;
word0(&rv) -= P*Exp_msk1;
goto cont;
}
#endif
#endif
dval(&adj) *= ulp(&rv);
if (dsign) {
if (word0(&rv) == Big0 && word1(&rv) == Big1)
goto ovfl;
dval(&rv) += adj.d;
}
else
dval(&rv) -= adj.d;
goto cont;
}
#endif

if (i < 0) {



if (dsign || word1(&rv) || word0(&rv) & Bndry_mask
#if defined(IEEE_Arith)
#if defined(Avoid_Underflow)
|| (word0(&rv) & Exp_mask) <= (2*P+1)*Exp_msk1
#else
|| (word0(&rv) & Exp_mask) <= Exp_msk1
#endif
#endif
) {
#if defined(SET_INEXACT)
if (!delta->x[0] && delta->wds <= 1)
inexact = 0;
#endif
break;
}
if (!delta->x[0] && delta->wds <= 1) {

#if defined(SET_INEXACT)
inexact = 0;
#endif
break;
}
delta = lshift(delta,Log2P);
if (cmp(delta, bs) > 0)
goto drop_down;
break;
}
if (i == 0) {

if (dsign) {
if ((word0(&rv) & Bndry_mask1) == Bndry_mask1
&& word1(&rv) == (
#if defined(Avoid_Underflow)
(scale && (y = word0(&rv) & Exp_mask) <= 2*P*Exp_msk1)
? (0xffffffff & (0xffffffff << (2*P+1-(y>>Exp_shift)))) :
#endif
0xffffffff)) {

if (word0(&rv) == Big0 && word1(&rv) == Big1)
goto ovfl;
word0(&rv) = (word0(&rv) & Exp_mask)
+ Exp_msk1
#if defined(IBM)
| Exp_msk1 >> 4
#endif
;
word1(&rv) = 0;
#if defined(Avoid_Underflow)
dsign = 0;
#endif
break;
}
}
else if (!(word0(&rv) & Bndry_mask) && !word1(&rv)) {
drop_down:

#if defined(Sudden_Underflow)
L = word0(&rv) & Exp_mask;
#if defined(IBM)
if (L < Exp_msk1)
#else
#if defined(Avoid_Underflow)
if (L <= (scale ? (2*P+1)*Exp_msk1 : Exp_msk1))
#else
if (L <= Exp_msk1)
#endif
#endif
goto undfl;
L -= Exp_msk1;
#else
#if defined(Avoid_Underflow)
if (scale) {
L = word0(&rv) & Exp_mask;
if (L <= (2*P+1)*Exp_msk1) {
if (L > (P+2)*Exp_msk1)


break;

goto undfl;
}
}
#endif
L = (word0(&rv) & Exp_mask) - Exp_msk1;
#endif
word0(&rv) = L | Bndry_mask1;
word1(&rv) = 0xffffffff;
#if defined(IBM)
goto cont;
#else
break;
#endif
}
#if !defined(ROUND_BIASED)
#if defined(Avoid_Underflow)
if (Lsb1) {
if (!(word0(&rv) & Lsb1))
break;
}
else if (!(word1(&rv) & Lsb))
break;
#else
if (!(word1(&rv) & LSB))
break;
#endif
#endif
if (dsign)
#if defined(Avoid_Underflow)
dval(&rv) += sulp(&rv, scale);
#else
dval(&rv) += ulp(&rv);
#endif
#if !defined(ROUND_BIASED)
else {
#if defined(Avoid_Underflow)
dval(&rv) -= sulp(&rv, scale);
#else
dval(&rv) -= ulp(&rv);
#endif
#if !defined(Sudden_Underflow)
if (!dval(&rv))
goto undfl;
#endif
}
#if defined(Avoid_Underflow)
dsign = 1 - dsign;
#endif
#endif
break;
}
if ((aadj = ratio(delta, bs)) <= 2.) {
if (dsign)
aadj = dval(&aadj1) = 1.;
else if (word1(&rv) || word0(&rv) & Bndry_mask) {
#if !defined(Sudden_Underflow)
if (word1(&rv) == Tiny1 && !word0(&rv))
goto undfl;
#endif
aadj = 1.;
dval(&aadj1) = -1.;
}
else {



if (aadj < 2./FLT_RADIX)
aadj = 1./FLT_RADIX;
else
aadj *= 0.5;
dval(&aadj1) = -aadj;
}
}
else {
aadj *= 0.5;
dval(&aadj1) = dsign ? aadj : -aadj;
#if defined(Check_FLT_ROUNDS)
switch(Rounding) {
case 2:
dval(&aadj1) -= 0.5;
break;
case 0:
case 3:
dval(&aadj1) += 0.5;
}
#else
if (Flt_Rounds == 0)
dval(&aadj1) += 0.5;
#endif
}
y = word0(&rv) & Exp_mask;



if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1)) {
dval(&rv0) = dval(&rv);
word0(&rv) -= P*Exp_msk1;
dval(&adj) = dval(&aadj1) * ulp(&rv);
dval(&rv) += dval(&adj);
if ((word0(&rv) & Exp_mask) >=
Exp_msk1*(DBL_MAX_EXP+Bias-P)) {
if (word0(&rv0) == Big0 && word1(&rv0) == Big1)
goto ovfl;
word0(&rv) = Big0;
word1(&rv) = Big1;
goto cont;
}
else
word0(&rv) += P*Exp_msk1;
}
else {
#if defined(Avoid_Underflow)
if (scale && y <= 2*P*Exp_msk1) {
if (aadj <= 0x7fffffff) {
if ((z = aadj) <= 0)
z = 1;
aadj = z;
dval(&aadj1) = dsign ? aadj : -aadj;
}
word0(&aadj1) += (2*P+1)*Exp_msk1 - y;
}
dval(&adj) = dval(&aadj1) * ulp(&rv);
dval(&rv) += dval(&adj);
#else
#if defined(Sudden_Underflow)
if ((word0(&rv) & Exp_mask) <= P*Exp_msk1) {
dval(&rv0) = dval(&rv);
word0(&rv) += P*Exp_msk1;
dval(&adj) = dval(&aadj1) * ulp(&rv);
dval(&rv) += adj;
#if defined(IBM)
if ((word0(&rv) & Exp_mask) < P*Exp_msk1)
#else
if ((word0(&rv) & Exp_mask) <= P*Exp_msk1)
#endif
{
if (word0(&rv0) == Tiny0
&& word1(&rv0) == Tiny1)
goto undfl;
word0(&rv) = Tiny0;
word1(&rv) = Tiny1;
goto cont;
}
else
word0(&rv) -= P*Exp_msk1;
}
else {
dval(&adj) = dval(&aadj1) * ulp(&rv);
dval(&rv) += adj;
}
#else







if (y <= (P-1)*Exp_msk1 && aadj > 1.) {
dval(&aadj1) = (double)(int)(aadj + 0.5);
if (!dsign)
dval(&aadj1) = -dval(&aadj1);
}
dval(&adj) = dval(&aadj1) * ulp(&rv);
dval(&rv) += adj;
#endif
#endif
}
z = word0(&rv) & Exp_mask;
#if !defined(SET_INEXACT)
#if defined(Avoid_Underflow)
if (!scale)
#endif
if (y == z) {

L = (Long)aadj;
aadj -= L;

if (dsign || word1(&rv) || word0(&rv) & Bndry_mask) {
if (aadj < .4999999 || aadj > .5000001)
break;
}
else if (aadj < .4999999/FLT_RADIX)
break;
}
#endif
cont:
Bfree(bb);
Bfree(bd);
Bfree(bs);
Bfree(delta);
}
Bfree(bb);
Bfree(bd);
Bfree(bs);
Bfree(bd0);
Bfree(delta);
#if defined(SET_INEXACT)
if (inexact) {
if (!oldinexact) {
word0(&rv0) = Exp_1 + (70 << Exp_shift);
word1(&rv0) = 0;
dval(&rv0) += 1.;
}
}
else if (!oldinexact)
clear_inexact();
#endif
#if defined(Avoid_Underflow)
if (scale) {
word0(&rv0) = Exp_1 - 2*P*Exp_msk1;
word1(&rv0) = 0;
dval(&rv) *= dval(&rv0);
#if !defined(NO_ERRNO)

#if defined(IEEE_Arith)
if (!(word0(&rv) & Exp_mask))
#else
if (word0(&rv) == 0 && word1(&rv) == 0)
#endif
errno = ERANGE;
#endif
}
#endif
#if defined(SET_INEXACT)
if (inexact && !(word0(&rv) & Exp_mask)) {

dval(&rv0) = 1e-300;
dval(&rv0) *= dval(&rv0);
}
#endif
ret:
if (se)
*se = (char *)s;
return sign ? -dval(&rv) : dval(&rv);
}

double
strtod
#if defined(KR_headers)
(s00, se, loc) CONST char *s00; char **se; locale_t
#else
(CONST char *s00, char **se)
#endif
{
return strtod_l(s00, se, __get_locale());
}

