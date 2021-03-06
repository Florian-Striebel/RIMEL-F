






#include <stdio.h>
#include "semi.h"

static void test_rint(uint32 *in, uint32 *out,
int isfloor, int isceil) {
int sign = in[0] & 0x80000000;
int roundup = (isfloor && sign) || (isceil && !sign);
uint32 xh, xl, roundword;
int ex = (in[0] >> 20) & 0x7FF;
int i;

if ((ex > 0x3ff + 52 - 1) ||
((in[0] & 0x7FFFFFFF) == 0 && in[1] == 0)) {

out[0] = in[0];
out[1] = in[1];
return;
}





if (ex < 0x3ff) {
out[0] = sign | (roundup ? 0x3FF00000 : 0);
out[1] = 0;
return;
}






xh = in[0];
xl = in[1];
roundword = 0;
for (i = ex; i < 0x3ff + 52; i++) {
if (roundword & 1)
roundword |= 2;
roundword = (roundword >> 1) | ((xl & 1) << 31);
xl = (xl >> 1) | ((xh & 1) << 31);
xh = xh >> 1;
}
if (roundword && roundup) {
xl++;
xh += (xl==0);
}
for (i = ex; i < 0x3ff + 52; i++) {
xh = (xh << 1) | ((xl >> 31) & 1);
xl = (xl & 0x7FFFFFFF) << 1;
}
out[0] = xh;
out[1] = xl;
}

char *test_ceil(uint32 *in, uint32 *out) {
test_rint(in, out, 0, 1);
return NULL;
}

char *test_floor(uint32 *in, uint32 *out) {
test_rint(in, out, 1, 0);
return NULL;
}

static void test_rintf(uint32 *in, uint32 *out,
int isfloor, int isceil) {
int sign = *in & 0x80000000;
int roundup = (isfloor && sign) || (isceil && !sign);
uint32 x, roundword;
int ex = (*in >> 23) & 0xFF;
int i;

if ((ex > 0x7f + 23 - 1) ||
(*in & 0x7FFFFFFF) == 0) {

*out = *in;
return;
}





if (ex < 0x7f) {
*out = sign | (roundup ? 0x3F800000 : 0);
return;
}






x = *in;
roundword = 0;
for (i = ex; i < 0x7F + 23; i++) {
if (roundword & 1)
roundword |= 2;
roundword = (roundword >> 1) | ((x & 1) << 31);
x = x >> 1;
}
if (roundword && roundup) {
x++;
}
for (i = ex; i < 0x7F + 23; i++) {
x = x << 1;
}
*out = x;
}

char *test_ceilf(uint32 *in, uint32 *out) {
test_rintf(in, out, 0, 1);
return NULL;
}

char *test_floorf(uint32 *in, uint32 *out) {
test_rintf(in, out, 1, 0);
return NULL;
}

char *test_fmod(uint32 *a, uint32 *b, uint32 *out) {
int sign;
int32 aex, bex;
uint32 am[2], bm[2];

if (((a[0] & 0x7FFFFFFF) << 1) + !!a[1] > 0xFFE00000 ||
((b[0] & 0x7FFFFFFF) << 1) + !!b[1] > 0xFFE00000) {

uint32 an, bn;
out[0] = 0x7ff80000;
out[1] = 1;
an = ((a[0] & 0x7FFFFFFF) << 1) + !!a[1];
bn = ((b[0] & 0x7FFFFFFF) << 1) + !!b[1];
if ((an > 0xFFE00000 && an < 0xFFF00000) ||
(bn > 0xFFE00000 && bn < 0xFFF00000))
return "i";
else
return NULL;
}
if ((b[0] & 0x7FFFFFFF) == 0 && b[1] == 0) {
out[0] = 0x7ff80000;
out[1] = 1;
return "EDOM status=i";
}
if ((a[0] & 0x7FF00000) == 0x7FF00000) {
out[0] = 0x7ff80000;
out[1] = 1;
return "EDOM status=i";
}
if ((b[0] & 0x7FF00000) == 0x7FF00000) {
out[0] = a[0];
out[1] = a[1];
return NULL;
}
if ((a[0] & 0x7FFFFFFF) == 0 && a[1] == 0) {
out[0] = a[0];
out[1] = a[1];
return NULL;
}





sign = a[0] & 0x80000000;
test_frexp(a, am, (uint32 *)&aex);
test_frexp(b, bm, (uint32 *)&bex);
am[0] &= 0xFFFFF, am[0] |= 0x100000;
bm[0] &= 0xFFFFF, bm[0] |= 0x100000;

while (aex >= bex) {
if (am[0] > bm[0] || (am[0] == bm[0] && am[1] >= bm[1])) {
am[1] -= bm[1];
am[0] = am[0] - bm[0] - (am[1] > ~bm[1]);
}
if (aex > bex) {
am[0] = (am[0] << 1) | ((am[1] & 0x80000000) >> 31);
am[1] <<= 1;
aex--;
} else
break;
}





aex += 0x3fd;
am[0] |= sign;
test_ldexp(am, (uint32 *)&aex, out);

return NULL;
}

char *test_fmodf(uint32 *a, uint32 *b, uint32 *out) {
int sign;
int32 aex, bex;
uint32 am, bm;

if ((*a & 0x7FFFFFFF) > 0x7F800000 ||
(*b & 0x7FFFFFFF) > 0x7F800000) {

uint32 an, bn;
*out = 0x7fc00001;
an = *a & 0x7FFFFFFF;
bn = *b & 0x7FFFFFFF;
if ((an > 0x7f800000 && an < 0x7fc00000) ||
(bn > 0x7f800000 && bn < 0x7fc00000))
return "i";
else
return NULL;
}
if ((*b & 0x7FFFFFFF) == 0) {
*out = 0x7fc00001;
return "EDOM status=i";
}
if ((*a & 0x7F800000) == 0x7F800000) {
*out = 0x7fc00001;
return "EDOM status=i";
}
if ((*b & 0x7F800000) == 0x7F800000) {
*out = *a;
return NULL;
}
if ((*a & 0x7FFFFFFF) == 0) {
*out = *a;
return NULL;
}





sign = a[0] & 0x80000000;
test_frexpf(a, &am, (uint32 *)&aex);
test_frexpf(b, &bm, (uint32 *)&bex);
am &= 0x7FFFFF, am |= 0x800000;
bm &= 0x7FFFFF, bm |= 0x800000;

while (aex >= bex) {
if (am >= bm) {
am -= bm;
}
if (aex > bex) {
am <<= 1;
aex--;
} else
break;
}





aex += 0x7d;
am |= sign;
test_ldexpf(&am, (uint32 *)&aex, out);

return NULL;
}

char *test_ldexp(uint32 *x, uint32 *np, uint32 *out) {
int n = *np;
int32 n2;
uint32 y[2];
int ex = (x[0] >> 20) & 0x7FF;
int sign = x[0] & 0x80000000;

if (ex == 0x7FF) {
out[0] = x[0];
out[1] = x[1];
return NULL;
}
if ((x[0] & 0x7FFFFFFF) == 0 && x[1] == 0) {
out[0] = x[0];
out[1] = x[1];
return NULL;
}

test_frexp(x, y, (uint32 *)&n2);
ex = n + n2;
if (ex > 0x400) {
out[0] = sign | 0x7FF00000;
out[1] = 0;
return "overflow";
}










if (ex < -1074 || (ex == -1074 && (y[0] & 0xFFFFF) == 0 && y[1] == 0)) {
out[0] = sign;
out[1] = 0;
return "underflow";
}





if (ex < -1021) {
uint32 roundword;
y[0] &= 0x000FFFFF;
y[0] |= 0x00100000;
roundword = 0;
while (ex < -1021) {
if (roundword & 1)
roundword |= 2;
roundword = (roundword >> 1) | ((y[1] & 1) << 31);
y[1] = (y[1] >> 1) | ((y[0] & 1) << 31);
y[0] = y[0] >> 1;
ex++;
}
if (roundword > 0x80000000 ||
(roundword == 0x80000000 && (y[1] & 1))) {
y[1]++;
y[0] += (y[1] == 0);
}
out[0] = sign | y[0];
out[1] = y[1];



if (roundword)
return "u";
return NULL;
} else {
out[0] = y[0] + (ex << 20);
out[1] = y[1];
return NULL;
}
}

char *test_ldexpf(uint32 *x, uint32 *np, uint32 *out) {
int n = *np;
int32 n2;
uint32 y;
int ex = (*x >> 23) & 0xFF;
int sign = *x & 0x80000000;

if (ex == 0xFF) {
*out = *x;
return NULL;
}
if ((*x & 0x7FFFFFFF) == 0) {
*out = *x;
return NULL;
}

test_frexpf(x, &y, (uint32 *)&n2);
ex = n + n2;
if (ex > 0x80) {
*out = sign | 0x7F800000;
return "overflow";
}










if (ex < -149 || (ex == -149 && (y & 0x7FFFFF) == 0)) {
*out = sign;
return "underflow";
}





if (ex < -125) {
uint32 roundword;
y &= 0x007FFFFF;
y |= 0x00800000;
roundword = 0;
while (ex < -125) {
if (roundword & 1)
roundword |= 2;
roundword = (roundword >> 1) | ((y & 1) << 31);
y = y >> 1;
ex++;
}
if (roundword > 0x80000000 ||
(roundword == 0x80000000 && (y & 1))) {
y++;
}
*out = sign | y;



if (roundword)
return "u";
return NULL;
} else {
*out = y + (ex << 23);
return NULL;
}
}

char *test_frexp(uint32 *x, uint32 *out, uint32 *nout) {
int ex = (x[0] >> 20) & 0x7FF;
if (ex == 0x7FF) {
out[0] = x[0];
out[1] = x[1];
nout[0] = 0;
return NULL;
}
if (ex == 0) {
int sign;
uint32 xh, xl;
if ((x[0] & 0x7FFFFFFF) == 0 && x[1] == 0) {

out[0] = x[0];
out[1] = x[1];
nout[0] = 0;
return NULL;
}
sign = x[0] & 0x80000000;
xh = x[0] & 0x7FFFFFFF;
xl = x[1];
ex = 1;
while (!(xh & 0x100000)) {
ex--;
xh = (xh << 1) | ((xl >> 31) & 1);
xl = (xl & 0x7FFFFFFF) << 1;
}
out[0] = sign | 0x3FE00000 | (xh & 0xFFFFF);
out[1] = xl;
nout[0] = ex - 0x3FE;
return NULL;
}
out[0] = 0x3FE00000 | (x[0] & 0x800FFFFF);
out[1] = x[1];
nout[0] = ex - 0x3FE;
return NULL;
}

char *test_frexpf(uint32 *x, uint32 *out, uint32 *nout) {
int ex = (*x >> 23) & 0xFF;
if (ex == 0xFF) {
*out = *x;
nout[0] = 0;
return NULL;
}
if (ex == 0) {
int sign;
uint32 xv;
if ((*x & 0x7FFFFFFF) == 0) {

*out = *x;
nout[0] = 0;
return NULL;
}
sign = *x & 0x80000000;
xv = *x & 0x7FFFFFFF;
ex = 1;
while (!(xv & 0x800000)) {
ex--;
xv = xv << 1;
}
*out = sign | 0x3F000000 | (xv & 0x7FFFFF);
nout[0] = ex - 0x7E;
return NULL;
}
*out = 0x3F000000 | (*x & 0x807FFFFF);
nout[0] = ex - 0x7E;
return NULL;
}

char *test_modf(uint32 *x, uint32 *fout, uint32 *iout) {
int ex = (x[0] >> 20) & 0x7FF;
int sign = x[0] & 0x80000000;
uint32 fh, fl;

if (((x[0] & 0x7FFFFFFF) | (!!x[1])) > 0x7FF00000) {



fout[0] = iout[0] = x[0];
fout[1] = iout[1] = x[1];
return NULL;
}

test_rint(x, iout, 0, 0);
fh = x[0] - iout[0];
fl = x[1] - iout[1];
if (!fh && !fl) {
fout[0] = sign;
fout[1] = 0;
return NULL;
}
if (!(iout[0] & 0x7FFFFFFF) && !iout[1]) {
fout[0] = x[0];
fout[1] = x[1];
return NULL;
}
while (!(fh & 0x100000)) {
ex--;
fh = (fh << 1) | ((fl >> 31) & 1);
fl = (fl & 0x7FFFFFFF) << 1;
}
fout[0] = sign | (ex << 20) | (fh & 0xFFFFF);
fout[1] = fl;
return NULL;
}

char *test_modff(uint32 *x, uint32 *fout, uint32 *iout) {
int ex = (*x >> 23) & 0xFF;
int sign = *x & 0x80000000;
uint32 f;

if ((*x & 0x7FFFFFFF) > 0x7F800000) {



*fout = *iout = *x;
return NULL;
}

test_rintf(x, iout, 0, 0);
f = *x - *iout;
if (!f) {
*fout = sign;
return NULL;
}
if (!(*iout & 0x7FFFFFFF)) {
*fout = *x;
return NULL;
}
while (!(f & 0x800000)) {
ex--;
f = f << 1;
}
*fout = sign | (ex << 23) | (f & 0x7FFFFF);
return NULL;
}

char *test_copysign(uint32 *x, uint32 *y, uint32 *out)
{
int ysign = y[0] & 0x80000000;
int xhigh = x[0] & 0x7fffffff;

out[0] = ysign | xhigh;
out[1] = x[1];


return NULL;
}

char *test_copysignf(uint32 *x, uint32 *y, uint32 *out)
{
int ysign = y[0] & 0x80000000;
int xhigh = x[0] & 0x7fffffff;

out[0] = ysign | xhigh;


return NULL;
}

char *test_isfinite(uint32 *x, uint32 *out)
{
int xhigh = x[0];

if ((xhigh & 0x7ff00000) == 0x7ff00000) out[0] = 0;
else out[0] = 1;
return NULL;
}

char *test_isfinitef(uint32 *x, uint32 *out)
{

if ((x[0] & 0x7f800000) == 0x7f800000) out[0] = 0;
else out[0] = 1;
return NULL;
}

char *test_isinff(uint32 *x, uint32 *out)
{

if ((x[0] & 0x7fffffff) == 0x7f800000) out[0] = 1;
else out[0] = 0;
return NULL;
}

char *test_isinf(uint32 *x, uint32 *out)
{
int xhigh = x[0];
int xlow = x[1];

if (((xhigh & 0x7fffffff) == 0x7ff00000) && (xlow == 0)) out[0] = 1;
else out[0] = 0;
return NULL;
}

char *test_isnanf(uint32 *x, uint32 *out)
{

int exponent = x[0] & 0x7f800000;
int fraction = x[0] & 0x007fffff;
if ((exponent == 0x7f800000) && (fraction != 0)) out[0] = 1;
else out[0] = 0;
return NULL;
}

char *test_isnan(uint32 *x, uint32 *out)
{

int exponent = x[0] & 0x7ff00000;
int fractionhigh = x[0] & 0x000fffff;
if ((exponent == 0x7ff00000) && ((fractionhigh != 0) || x[1] != 0))
out[0] = 1;
else out[0] = 0;
return NULL;
}

char *test_isnormalf(uint32 *x, uint32 *out)
{

int exponent = x[0] & 0x7f800000;
if (exponent == 0x7f800000) out[0] = 0;
else if (exponent == 0) out[0] = 0;
else out[0] = 1;
return NULL;
}

char *test_isnormal(uint32 *x, uint32 *out)
{

int exponent = x[0] & 0x7ff00000;
if (exponent == 0x7ff00000) out[0] = 0;
else if (exponent == 0) out[0] = 0;
else out[0] = 1;
return NULL;
}

char *test_signbitf(uint32 *x, uint32 *out)
{

out[0] = (x[0] >> 31) & 1;
return NULL;
}

char *test_signbit(uint32 *x, uint32 *out)
{

out[0] = (x[0] >> 31) & 1;
return NULL;
}

char *test_fpclassify(uint32 *x, uint32 *out)
{
int exponent = (x[0] & 0x7ff00000) >> 20;
int fraction = (x[0] & 0x000fffff) | x[1];

if ((exponent == 0x00) && (fraction == 0)) out[0] = 0;
else if ((exponent == 0x00) && (fraction != 0)) out[0] = 4;
else if ((exponent == 0x7ff) && (fraction == 0)) out[0] = 3;
else if ((exponent == 0x7ff) && (fraction != 0)) out[0] = 7;
else out[0] = 5;
return NULL;
}

char *test_fpclassifyf(uint32 *x, uint32 *out)
{
int exponent = (x[0] & 0x7f800000) >> 23;
int fraction = x[0] & 0x007fffff;

if ((exponent == 0x000) && (fraction == 0)) out[0] = 0;
else if ((exponent == 0x000) && (fraction != 0)) out[0] = 4;
else if ((exponent == 0xff) && (fraction == 0)) out[0] = 3;
else if ((exponent == 0xff) && (fraction != 0)) out[0] = 7;
else out[0] = 5;
return NULL;
}






static int fpcmp4(uint32 *x, uint32 *y)
{
int result = 0;






if ((x[0] & 0x7fffffff) >= 0x7ff80000) result = -2;
else if ((x[0] & 0x7fffffff) > 0x7ff00000) result = -3;
else if (((x[0] & 0x7fffffff) == 0x7ff00000) && (x[1] != 0)) result = -3;
if ((y[0] & 0x7fffffff) >= 0x7ff80000 && result != -3) result = -2;
else if ((y[0] & 0x7fffffff) > 0x7ff00000) result = -3;
else if (((y[0] & 0x7fffffff) == 0x7ff00000) && (y[1] != 0)) result = -3;
if (result != 0) return result;




if (((x[0] & 0x7fffffff) == 0) && x[1] == 0 &&
((y[0] & 0x7fffffff) == 0) && y[1] == 0)
return 0;





if ((x[0] >> 31) != (y[0] >> 31))
return ((x[0] >> 31) == 0) - ((y[0] >> 31) == 0);











if ((x[0] & 0x7fffffff) < (y[0] & 0x7fffffff)) result = -1;
else if ((x[0] & 0x7fffffff) > (y[0] & 0x7fffffff)) result = 1;
else if (x[1] < y[1]) result = -1;
else if (x[1] > y[1]) result = 1;
else result = 0;





if ((x[0] >> 31) == 0) return result;
else return -result;
}






static int fpcmp4f(uint32 *x, uint32 *y)
{
int result = 0;






if ((x[0] & 0x7fffffff) >= 0x7fc00000) result = -2;
else if ((x[0] & 0x7fffffff) > 0x7f800000) result = -3;
if ((y[0] & 0x7fffffff) >= 0x7fc00000 && result != -3) result = -2;
else if ((y[0] & 0x7fffffff) > 0x7f800000) result = -3;
if (result != 0) return result;




if (((x[0] & 0x7fffffff) == 0) && ((y[0] & 0x7fffffff) == 0))
return 0;





if ((x[0] >> 31) != (y[0] >> 31))
return ((x[0] >> 31) == 0) - ((y[0] >> 31) == 0);











if ((x[0] & 0x7fffffff) < (y[0] & 0x7fffffff)) result = -1;
else if ((x[0] & 0x7fffffff) > (y[0] & 0x7fffffff)) result = 1;
else result = 0;





if ((x[0] >> 31) == 0) return result;
else return -result;
}

char *test_isgreater(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4(x, y);
*out = (result == 1);
return result == -3 ? "i" : NULL;
}

char *test_isgreaterequal(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4(x, y);
*out = (result >= 0);
return result == -3 ? "i" : NULL;
}

char *test_isless(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4(x, y);
*out = (result == -1);
return result == -3 ? "i" : NULL;
}

char *test_islessequal(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4(x, y);
*out = (result == -1) || (result == 0);
return result == -3 ? "i" : NULL;
}

char *test_islessgreater(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4(x, y);
*out = (result == -1) || (result == 1);
return result == -3 ? "i" : NULL;
}

char *test_isunordered(uint32 *x, uint32 *y, uint32 *out)
{
int normal = 0;
int result = fpcmp4(x, y);

test_isnormal(x, out);
normal |= *out;
test_isnormal(y, out);
normal |= *out;
*out = (result == -2) || (result == -3);
return result == -3 ? "i" : NULL;
}

char *test_isgreaterf(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4f(x, y);
*out = (result == 1);
return result == -3 ? "i" : NULL;
}

char *test_isgreaterequalf(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4f(x, y);
*out = (result >= 0);
return result == -3 ? "i" : NULL;
}

char *test_islessf(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4f(x, y);
*out = (result == -1);
return result == -3 ? "i" : NULL;
}

char *test_islessequalf(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4f(x, y);
*out = (result == -1) || (result == 0);
return result == -3 ? "i" : NULL;
}

char *test_islessgreaterf(uint32 *x, uint32 *y, uint32 *out)
{
int result = fpcmp4f(x, y);
*out = (result == -1) || (result == 1);
return result == -3 ? "i" : NULL;
}

char *test_isunorderedf(uint32 *x, uint32 *y, uint32 *out)
{
int normal = 0;
int result = fpcmp4f(x, y);

test_isnormalf(x, out);
normal |= *out;
test_isnormalf(y, out);
normal |= *out;
*out = (result == -2) || (result == -3);
return result == -3 ? "i" : NULL;
}
