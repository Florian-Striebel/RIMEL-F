






























#include "gdtoaimp.h"

#if defined(USE_LOCALE)
#include "locale.h"
#endif

static CONST int
fivesbits[] = { 0, 3, 5, 7, 10, 12, 14, 17, 19, 21,
24, 26, 28, 31, 33, 35, 38, 40, 42, 45,
47, 49, 52
#if defined(VAX)
, 54, 56
#endif
};

Bigint *
#if defined(KR_headers)
increment(b) Bigint *b;
#else
increment(Bigint *b)
#endif
{
ULong *x, *xe;
Bigint *b1;
#if defined(Pack_16)
ULong carry = 1, y;
#endif

x = b->x;
xe = x + b->wds;
#if defined(Pack_32)
do {
if (*x < (ULong)0xffffffffL) {
++*x;
return b;
}
*x++ = 0;
} while(x < xe);
#else
do {
y = *x + carry;
carry = y >> 16;
*x++ = y & 0xffff;
if (!carry)
return b;
} while(x < xe);
if (carry)
#endif
{
if (b->wds >= b->maxwds) {
b1 = Balloc(b->k+1);
Bcopy(b1,b);
Bfree(b);
b = b1;
}
b->x[b->wds++] = 1;
}
return b;
}

void
#if defined(KR_headers)
decrement(b) Bigint *b;
#else
decrement(Bigint *b)
#endif
{
ULong *x, *xe;
#if defined(Pack_16)
ULong borrow = 1, y;
#endif

x = b->x;
xe = x + b->wds;
#if defined(Pack_32)
do {
if (*x) {
--*x;
break;
}
*x++ = 0xffffffffL;
}
while(x < xe);
#else
do {
y = *x - borrow;
borrow = (y & 0x10000) >> 16;
*x++ = y & 0xffff;
} while(borrow && x < xe);
#endif
}

static int
#if defined(KR_headers)
all_on(b, n) Bigint *b; int n;
#else
all_on(Bigint *b, int n)
#endif
{
ULong *x, *xe;

x = b->x;
xe = x + (n >> kshift);
while(x < xe)
if ((*x++ & ALL_ON) != ALL_ON)
return 0;
if (n &= kmask)
return ((*x | (ALL_ON << n)) & ALL_ON) == ALL_ON;
return 1;
}

Bigint *
#if defined(KR_headers)
set_ones(b, n) Bigint *b; int n;
#else
set_ones(Bigint *b, int n)
#endif
{
int k;
ULong *x, *xe;

k = (n + ((1 << kshift) - 1)) >> kshift;
if (b->k < k) {
Bfree(b);
b = Balloc(k);
}
k = n >> kshift;
if (n &= kmask)
k++;
b->wds = k;
x = b->x;
xe = x + k;
while(x < xe)
*x++ = ALL_ON;
if (n)
x[-1] >>= ULbits - n;
return b;
}

static int
rvOK
#if defined(KR_headers)
(d, fpi, exp, bits, exact, rd, irv)
U *d; FPI *fpi; Long *exp; ULong *bits; int exact, rd, *irv;
#else
(U *d, FPI *fpi, Long *exp, ULong *bits, int exact, int rd, int *irv)
#endif
{
Bigint *b;
ULong carry, inex, lostbits;
int bdif, e, j, k, k1, nb, rv;

carry = rv = 0;
b = d2b(dval(d), &e, &bdif);
bdif -= nb = fpi->nbits;
e += bdif;
if (bdif <= 0) {
if (exact)
goto trunc;
goto ret;
}
if (P == nb) {
if (
#if !defined(IMPRECISE_INEXACT)
exact &&
#endif
fpi->rounding ==
#if defined(RND_PRODQUOT)
FPI_Round_near
#else
Flt_Rounds
#endif
) goto trunc;
goto ret;
}
switch(rd) {
case 1:
goto trunc;
case 2:
break;
default:
k = bdif - 1;
if (k < 0)
goto trunc;
if (!k) {
if (!exact)
goto ret;
if (b->x[0] & 2)
break;
goto trunc;
}
if (b->x[k>>kshift] & ((ULong)1 << (k & kmask)))
break;
goto trunc;
}

carry = 1;
trunc:
inex = lostbits = 0;
if (bdif > 0) {
if ( (lostbits = any_on(b, bdif)) !=0)
inex = STRTOG_Inexlo;
rshift(b, bdif);
if (carry) {
inex = STRTOG_Inexhi;
b = increment(b);
if ( (j = nb & kmask) !=0)
j = ULbits - j;
if (hi0bits(b->x[b->wds - 1]) != j) {
if (!lostbits)
lostbits = b->x[0] & 1;
rshift(b, 1);
e++;
}
}
}
else if (bdif < 0)
b = lshift(b, -bdif);
if (e < fpi->emin) {
k = fpi->emin - e;
e = fpi->emin;
if (k > nb || fpi->sudden_underflow) {
b->wds = inex = 0;
*irv = STRTOG_Underflow | STRTOG_Inexlo;
}
else {
k1 = k - 1;
if (k1 > 0 && !lostbits)
lostbits = any_on(b, k1);
if (!lostbits && !exact)
goto ret;
lostbits |=
carry = b->x[k1>>kshift] & (1 << (k1 & kmask));
rshift(b, k);
*irv = STRTOG_Denormal;
if (carry) {
b = increment(b);
inex = STRTOG_Inexhi | STRTOG_Underflow;
}
else if (lostbits)
inex = STRTOG_Inexlo | STRTOG_Underflow;
}
}
else if (e > fpi->emax) {
e = fpi->emax + 1;
*irv = STRTOG_Infinite | STRTOG_Overflow | STRTOG_Inexhi;
#if !defined(NO_ERRNO)
errno = ERANGE;
#endif
b->wds = inex = 0;
}
*exp = e;
copybits(bits, nb, b);
*irv |= inex;
rv = 1;
ret:
Bfree(b);
return rv;
}

static int
#if defined(KR_headers)
mantbits(d) U *d;
#else
mantbits(U *d)
#endif
{
ULong L;
#if defined(VAX)
L = word1(d) << 16 | word1(d) >> 16;
if (L)
#else
if ( (L = word1(d)) !=0)
#endif
return P - lo0bits(&L);
#if defined(VAX)
L = word0(d) << 16 | word0(d) >> 16 | Exp_msk11;
#else
L = word0(d) | Exp_msk1;
#endif
return P - 32 - lo0bits(&L);
}

int
strtodg_l
#if defined(KR_headers)
(s00, se, fpi, exp, bits, loc)
CONST char *s00; char **se; FPI *fpi; Long *exp; ULong *bits; locale_t loc;
#else
(CONST char *s00, char **se, FPI *fpi, Long *exp, ULong *bits, locale_t loc)
#endif
{
int abe, abits, asub;
int bb0, bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, decpt, denorm;
int dsign, e, e1, e2, emin, esign, finished, i, inex, irv;
int j, k, nbits, nd, nd0, nf, nz, nz0, rd, rvbits, rve, rve1, sign;
int sudden_underflow;
CONST char *s, *s0, *s1;
double adj0, tol;
Long L;
U adj, rv;
ULong *b, *be, y, z;
Bigint *ab, *bb, *bb1, *bd, *bd0, *bs, *delta, *rvb, *rvb0;
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

irv = STRTOG_Zero;
denorm = sign = nz0 = nz = 0;
dval(&rv) = 0.;
rvb = 0;
nbits = fpi->nbits;
for(s = s00;;s++) switch(*s) {
case '-':
sign = 1;

case '+':
if (*++s)
goto break2;

case 0:
sign = 0;
irv = STRTOG_NoNumber;
s = s00;
goto ret;
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
switch(s[1]) {
case 'x':
case 'X':
irv = gethex(&s, fpi, exp, &rvb, sign);
if (irv == STRTOG_NoNumber) {
s = s00;
sign = 0;
}
goto ret;
}
#endif
nz0 = 1;
while(*++s == '0') ;
if (!*s)
goto ret;
}
sudden_underflow = fpi->sudden_underflow;
s0 = s;
y = z = 0;
for(decpt = nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
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
irv = STRTOG_NoNumber;
s = s00;
goto ret;
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

if (!decpt)
switch(c) {
case 'i':
case 'I':
if (match(&s,"nf")) {
--s;
if (!match(&s,"inity"))
++s;
irv = STRTOG_Infinite;
goto infnanexp;
}
break;
case 'n':
case 'N':
if (match(&s, "an")) {
irv = STRTOG_NaN;
*exp = fpi->emax + 1;
#if !defined(No_Hex_NaN)
if (*s == '(')
irv = hexnan(&s, fpi, bits);
#endif
goto infnanexp;
}
}
#endif
irv = STRTOG_NoNumber;
s = s00;
}
goto ret;
}

irv = STRTOG_Normal;
e1 = e -= nf;
rd = 0;
switch(fpi->rounding & 3) {
case FPI_Round_up:
rd = 2 - sign;
break;
case FPI_Round_zero:
rd = 1;
break;
case FPI_Round_down:
rd = 1 + sign;
}






if (!nd0)
nd0 = nd;
k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
dval(&rv) = y;
if (k > 9)
dval(&rv) = tens[k - 9] * dval(&rv) + z;
bd0 = 0;
if (nbits <= P && nd <= DBL_DIG) {
if (!e) {
if (rvOK(&rv, fpi, exp, bits, 1, rd, &irv))
goto ret;
}
else if (e > 0) {
if (e <= Ten_pmax) {
#if defined(VAX)
goto vax_ovfl_check;
#else
i = fivesbits[e] + mantbits(&rv) <= P;
rounded_product(dval(&rv), tens[e]);
if (rvOK(&rv, fpi, exp, bits, i, rd, &irv))
goto ret;
e1 -= e;
goto rv_notOK;
#endif
}
i = DBL_DIG - nd;
if (e <= Ten_pmax + i) {



e2 = e - i;
e1 -= i;
dval(&rv) *= tens[i];
#if defined(VAX)



vax_ovfl_check:
dval(&adj) = dval(&rv);
word0(&adj) -= P*Exp_msk1;
rounded_product(dval(&adj), tens[e2]);
if ((word0(&adj) & Exp_mask)
> Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
goto rv_notOK;
word0(&adj) += P*Exp_msk1;
dval(&rv) = dval(&adj);
#else
rounded_product(dval(&rv), tens[e2]);
#endif
if (rvOK(&rv, fpi, exp, bits, 0, rd, &irv))
goto ret;
e1 -= e2;
}
}
#if !defined(Inaccurate_Divide)
else if (e >= -Ten_pmax) {
rounded_quotient(dval(&rv), tens[-e]);
if (rvOK(&rv, fpi, exp, bits, 0, rd, &irv))
goto ret;
e1 -= e;
}
#endif
}
rv_notOK:
e1 += nd - k;



e2 = 0;
if (e1 > 0) {
if ( (i = e1 & 15) !=0)
dval(&rv) *= tens[i];
if (e1 &= ~15) {
e1 >>= 4;
while(e1 >= (1 << (n_bigtens-1))) {
e2 += ((word0(&rv) & Exp_mask)
>> Exp_shift1) - Bias;
word0(&rv) &= ~Exp_mask;
word0(&rv) |= Bias << Exp_shift1;
dval(&rv) *= bigtens[n_bigtens-1];
e1 -= 1 << (n_bigtens-1);
}
e2 += ((word0(&rv) & Exp_mask) >> Exp_shift1) - Bias;
word0(&rv) &= ~Exp_mask;
word0(&rv) |= Bias << Exp_shift1;
for(j = 0; e1 > 0; j++, e1 >>= 1)
if (e1 & 1)
dval(&rv) *= bigtens[j];
}
}
else if (e1 < 0) {
e1 = -e1;
if ( (i = e1 & 15) !=0)
dval(&rv) /= tens[i];
if (e1 &= ~15) {
e1 >>= 4;
while(e1 >= (1 << (n_bigtens-1))) {
e2 += ((word0(&rv) & Exp_mask)
>> Exp_shift1) - Bias;
word0(&rv) &= ~Exp_mask;
word0(&rv) |= Bias << Exp_shift1;
dval(&rv) *= tinytens[n_bigtens-1];
e1 -= 1 << (n_bigtens-1);
}
e2 += ((word0(&rv) & Exp_mask) >> Exp_shift1) - Bias;
word0(&rv) &= ~Exp_mask;
word0(&rv) |= Bias << Exp_shift1;
for(j = 0; e1 > 0; j++, e1 >>= 1)
if (e1 & 1)
dval(&rv) *= tinytens[j];
}
}
#if defined(IBM)





e2 <<= 2;
#endif
rvb = d2b(dval(&rv), &rve, &rvbits);
rve += e2;
if ((j = rvbits - nbits) > 0) {
rshift(rvb, j);
rvbits = nbits;
rve += j;
}
bb0 = 0;
e2 = rve + rvbits - nbits;
if (e2 > fpi->emax + 1)
goto huge;
rve1 = rve + rvbits - nbits;
if (e2 < (emin = fpi->emin)) {
denorm = 1;
j = rve - emin;
if (j > 0) {
rvb = lshift(rvb, j);
rvbits += j;
}
else if (j < 0) {
rvbits += j;
if (rvbits <= 0) {
if (rvbits < -1) {
ufl:
rvb->wds = 0;
rvb->x[0] = 0;
*exp = emin;
irv = STRTOG_Underflow | STRTOG_Inexlo;
goto ret;
}
rvb->x[0] = rvb->wds = rvbits = 1;
}
else
rshift(rvb, -j);
}
rve = rve1 = emin;
if (sudden_underflow && e2 + 1 < emin)
goto ufl;
}





bd0 = s2b(s0, nd0, nd, y, dplen);

for(;;) {
bd = Balloc(bd0->k);
Bcopy(bd, bd0);
bb = Balloc(rvb->k);
Bcopy(bb, rvb);
bbbits = rvbits - bb0;
bbe = rve + bb0;
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
j = nbits + 1 - bbbits;
i = bbe + bbbits - nbits;
if (i < emin)
j += i - emin;
bb2 += j;
bd2 += j;
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
bb2 -= bb0;
if (bb2 > 0)
bb = lshift(bb, bb2);
else if (bb2 < 0)
rshift(bb, -bb2);
if (bd5 > 0)
bd = pow5mult(bd, bd5);
if (bd2 > 0)
bd = lshift(bd, bd2);
if (bs2 > 0)
bs = lshift(bs, bs2);
asub = 1;
inex = STRTOG_Inexhi;
delta = diff(bb, bd);
if (delta->wds <= 1 && !delta->x[0])
break;
dsign = delta->sign;
delta->sign = finished = 0;
L = 0;
i = cmp(delta, bs);
if (rd && i <= 0) {
irv = STRTOG_Normal;
if ( (finished = dsign ^ (rd&1)) !=0) {
if (dsign != 0) {
irv |= STRTOG_Inexhi;
goto adj1;
}
irv |= STRTOG_Inexlo;
if (rve1 == emin)
goto adj1;
for(i = 0, j = nbits; j >= ULbits;
i++, j -= ULbits) {
if (rvb->x[i] & ALL_ON)
goto adj1;
}
if (j > 1 && lo0bits(rvb->x + i) < j - 1)
goto adj1;
rve = rve1 - 1;
rvb = set_ones(rvb, rvbits = nbits);
break;
}
irv |= dsign ? STRTOG_Inexlo : STRTOG_Inexhi;
break;
}
if (i < 0) {



irv = dsign
? STRTOG_Normal | STRTOG_Inexlo
: STRTOG_Normal | STRTOG_Inexhi;
if (dsign || bbbits > 1 || denorm || rve1 == emin)
break;
delta = lshift(delta,1);
if (cmp(delta, bs) > 0) {
irv = STRTOG_Normal | STRTOG_Inexlo;
goto drop_down;
}
break;
}
if (i == 0) {

if (dsign) {
if (denorm && all_on(rvb, rvbits)) {

rvb->wds = 1;
rvb->x[0] = 1;
rve = emin + nbits - (rvbits = 1);
irv = STRTOG_Normal | STRTOG_Inexhi;
denorm = 0;
break;
}
irv = STRTOG_Normal | STRTOG_Inexlo;
}
else if (bbbits == 1) {
irv = STRTOG_Normal;
drop_down:

if (rve1 == emin) {
irv = STRTOG_Normal | STRTOG_Inexhi;
if (rvb->wds == 1 && rvb->x[0] == 1)
sudden_underflow = 1;
break;
}
rve -= nbits;
rvb = set_ones(rvb, rvbits = nbits);
break;
}
else
irv = STRTOG_Normal | STRTOG_Inexhi;
if ((bbbits < nbits && !denorm) || !(rvb->x[0] & 1))
break;
if (dsign) {
rvb = increment(rvb);
j = kmask & (ULbits - (rvbits & kmask));
if (hi0bits(rvb->x[rvb->wds - 1]) != j)
rvbits++;
irv = STRTOG_Normal | STRTOG_Inexhi;
}
else {
if (bbbits == 1)
goto undfl;
decrement(rvb);
irv = STRTOG_Normal | STRTOG_Inexlo;
}
break;
}
if ((dval(&adj) = ratio(delta, bs)) <= 2.) {
adj1:
inex = STRTOG_Inexlo;
if (dsign) {
asub = 0;
inex = STRTOG_Inexhi;
}
else if (denorm && bbbits <= 1) {
undfl:
rvb->wds = 0;
rve = emin;
irv = STRTOG_Underflow | STRTOG_Inexlo;
break;
}
adj0 = dval(&adj) = 1.;
}
else {
adj0 = dval(&adj) *= 0.5;
if (dsign) {
asub = 0;
inex = STRTOG_Inexlo;
}
if (dval(&adj) < 2147483647.) {
L = adj0;
adj0 -= L;
switch(rd) {
case 0:
if (adj0 >= .5)
goto inc_L;
break;
case 1:
if (asub && adj0 > 0.)
goto inc_L;
break;
case 2:
if (!asub && adj0 > 0.) {
inc_L:
L++;
inex = STRTOG_Inexact - inex;
}
}
dval(&adj) = L;
}
}
y = rve + rvbits;




if (!denorm && rvbits < nbits) {
rvb = lshift(rvb, j = nbits - rvbits);
rve -= j;
rvbits = nbits;
}
ab = d2b(dval(&adj), &abe, &abits);
if (abe < 0)
rshift(ab, -abe);
else if (abe > 0)
ab = lshift(ab, abe);
rvb0 = rvb;
if (asub) {

j = hi0bits(rvb->x[rvb->wds-1]);
rvb = diff(rvb, ab);
k = rvb0->wds - 1;
if (denorm)
;
else if (rvb->wds <= k
|| hi0bits( rvb->x[k]) >
hi0bits(rvb0->x[k])) {

if (rve1 == emin) {
--rvbits;
denorm = 1;
}
else {
rvb = lshift(rvb, 1);
--rve;
--rve1;
L = finished = 0;
}
}
}
else {
rvb = sum(rvb, ab);
k = rvb->wds - 1;
if (k >= rvb0->wds
|| hi0bits(rvb->x[k]) < hi0bits(rvb0->x[k])) {
if (denorm) {
if (++rvbits == nbits)
denorm = 0;
}
else {
rshift(rvb, 1);
rve++;
rve1++;
L = 0;
}
}
}
Bfree(ab);
Bfree(rvb0);
if (finished)
break;

z = rve + rvbits;
if (y == z && L) {

tol = dval(&adj) * 5e-16;
dval(&adj) = adj0 - .5;
if (dval(&adj) < -tol) {
if (adj0 > tol) {
irv |= inex;
break;
}
}
else if (dval(&adj) > tol && adj0 < 1. - tol) {
irv |= inex;
break;
}
}
bb0 = denorm ? 0 : trailz(rvb);
Bfree(bb);
Bfree(bd);
Bfree(bs);
Bfree(delta);
}
if (!denorm && (j = nbits - rvbits)) {
if (j > 0)
rvb = lshift(rvb, j);
else
rshift(rvb, -j);
rve -= j;
}
*exp = rve;
Bfree(bb);
Bfree(bd);
Bfree(bs);
Bfree(bd0);
Bfree(delta);
if (rve > fpi->emax) {
switch(fpi->rounding & 3) {
case FPI_Round_near:
goto huge;
case FPI_Round_up:
if (!sign)
goto huge;
break;
case FPI_Round_down:
if (sign)
goto huge;
}

Bfree(rvb);
rvb = 0;
irv = STRTOG_Normal | STRTOG_Inexlo;
*exp = fpi->emax;
b = bits;
be = b + ((fpi->nbits + 31) >> 5);
while(b < be)
*b++ = -1;
if ((j = fpi->nbits & 0x1f))
*--be >>= (32 - j);
goto ret;
huge:
rvb->wds = 0;
irv = STRTOG_Infinite | STRTOG_Overflow | STRTOG_Inexhi;
#if !defined(NO_ERRNO)
errno = ERANGE;
#endif
infnanexp:
*exp = fpi->emax + 1;
}
ret:
if (denorm) {
if (sudden_underflow) {
rvb->wds = 0;
irv = STRTOG_Underflow | STRTOG_Inexlo;
#if !defined(NO_ERRNO)
errno = ERANGE;
#endif
}
else {
irv = (irv & ~STRTOG_Retmask) |
(rvb->wds > 0 ? STRTOG_Denormal : STRTOG_Zero);
if (irv & STRTOG_Inexact) {
irv |= STRTOG_Underflow;
#if !defined(NO_ERRNO)
errno = ERANGE;
#endif
}
}
}
if (se)
*se = (char *)s;
if (sign)
irv |= STRTOG_Neg;
if (rvb) {
copybits(bits, nbits, rvb);
Bfree(rvb);
}
return irv;
}
