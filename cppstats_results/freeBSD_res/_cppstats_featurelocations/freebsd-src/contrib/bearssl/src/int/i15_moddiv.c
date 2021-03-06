























#include "inner.h"
















static void
cond_negate(uint16_t *a, size_t len, uint32_t ctl)
{
size_t k;
uint32_t cc, xm;

cc = ctl;
xm = 0x7FFF & -ctl;
for (k = 0; k < len; k ++) {
uint32_t aw;

aw = a[k];
aw = (aw ^ xm) + cc;
a[k] = aw & 0x7FFF;
cc = (aw >> 15) & 1;
}
}











static void
finish_mod(uint16_t *a, size_t len, const uint16_t *m, uint32_t neg)
{
size_t k;
uint32_t cc, xm, ym;




cc = 0;
for (k = 0; k < len; k ++) {
uint32_t aw, mw;

aw = a[k];
mw = m[k];
cc = (aw - mw - cc) >> 31;
}







xm = 0x7FFF & -neg;
ym = -(neg | (1 - cc));
cc = neg;
for (k = 0; k < len; k ++) {
uint32_t aw, mw;

aw = a[k];
mw = (m[k] ^ xm) & ym;
aw = aw - mw - cc;
a[k] = aw & 0x7FFF;
cc = aw >> 31;
}
}















static uint32_t
co_reduce(uint16_t *a, uint16_t *b, size_t len,
int32_t pa, int32_t pb, int32_t qa, int32_t qb)
{
size_t k;
int32_t cca, ccb;
uint32_t nega, negb;

cca = 0;
ccb = 0;
for (k = 0; k < len; k ++) {
uint32_t wa, wb, za, zb;
uint16_t tta, ttb;














wa = a[k];
wb = b[k];
za = wa * (uint32_t)pa + wb * (uint32_t)pb + (uint32_t)cca;
zb = wa * (uint32_t)qa + wb * (uint32_t)qb + (uint32_t)ccb;
if (k > 0) {
a[k - 1] = za & 0x7FFF;
b[k - 1] = zb & 0x7FFF;
}
tta = za >> 15;
ttb = zb >> 15;
cca = *(int16_t *)&tta;
ccb = *(int16_t *)&ttb;
}
a[len - 1] = (uint16_t)cca;
b[len - 1] = (uint16_t)ccb;
nega = (uint32_t)cca >> 31;
negb = (uint32_t)ccb >> 31;
cond_negate(a, len, nega);
cond_negate(b, len, negb);
return nega | (negb << 1);
}












static void
co_reduce_mod(uint16_t *a, uint16_t *b, size_t len,
int32_t pa, int32_t pb, int32_t qa, int32_t qb,
const uint16_t *m, uint16_t m0i)
{
size_t k;
int32_t cca, ccb, fa, fb;

cca = 0;
ccb = 0;
fa = ((a[0] * (uint32_t)pa + b[0] * (uint32_t)pb) * m0i) & 0x7FFF;
fb = ((a[0] * (uint32_t)qa + b[0] * (uint32_t)qb) * m0i) & 0x7FFF;
for (k = 0; k < len; k ++) {
uint32_t wa, wb, za, zb;
uint32_t tta, ttb;





wa = a[k];
wb = b[k];
za = wa * (uint32_t)pa + wb * (uint32_t)pb
+ m[k] * (uint32_t)fa + (uint32_t)cca;
zb = wa * (uint32_t)qa + wb * (uint32_t)qb
+ m[k] * (uint32_t)fb + (uint32_t)ccb;
if (k > 0) {
a[k - 1] = za & 0x7FFF;
b[k - 1] = zb & 0x7FFF;
}






#define M ((uint32_t)1 << 16)
tta = za >> 15;
ttb = zb >> 15;
tta = (tta ^ M) - M;
ttb = (ttb ^ M) - M;
cca = *(int32_t *)&tta;
ccb = *(int32_t *)&ttb;
#undef M
}
a[len - 1] = (uint32_t)cca;
b[len - 1] = (uint32_t)ccb;









finish_mod(a, len, m, (uint32_t)cca >> 31);
finish_mod(b, len, m, (uint32_t)ccb >> 31);
}


uint32_t
br_i15_moddiv(uint16_t *x, const uint16_t *y, const uint16_t *m, uint16_t m0i,
uint16_t *t)
{

























































size_t len, k;
uint16_t *a, *b, *u, *v;
uint32_t num, r;

len = (m[0] + 15) >> 4;
a = t;
b = a + len;
u = x + 1;
v = b + len;
memcpy(a, y + 1, len * sizeof *y);
memcpy(b, m + 1, len * sizeof *m);
memset(v, 0, len * sizeof *v);





for (num = ((m[0] - (m[0] >> 4)) << 1) + 14; num >= 14; num -= 14) {
size_t j;
uint32_t c0, c1;
uint32_t a0, a1, b0, b1;
uint32_t a_hi, b_hi, a_lo, b_lo;
int32_t pa, pb, qa, qb;
int i;








c0 = (uint32_t)-1;
c1 = (uint32_t)-1;
a0 = 0;
a1 = 0;
b0 = 0;
b1 = 0;
j = len;
while (j -- > 0) {
uint32_t aw, bw;

aw = a[j];
bw = b[j];
a0 ^= (a0 ^ aw) & c0;
a1 ^= (a1 ^ aw) & c1;
b0 ^= (b0 ^ bw) & c0;
b1 ^= (b1 ^ bw) & c1;
c1 = c0;
c0 &= (((aw | bw) + 0xFFFF) >> 16) - (uint32_t)1;
}







a1 |= a0 & c1;
a0 &= ~c1;
b1 |= b0 & c1;
b0 &= ~c1;
a_hi = (a0 << 15) + a1;
b_hi = (b0 << 15) + b1;
a_lo = a[0];
b_lo = b[0];










pa = 1;
pb = 0;
qa = 0;
qb = 1;
for (i = 0; i < 15; i ++) {












uint32_t r, oa, ob, cAB, cBA, cA;











r = GT(a_hi, b_hi);
oa = (a_lo >> i) & 1;
ob = (b_lo >> i) & 1;
cAB = oa & ob & r;
cBA = oa & ob & NOT(r);
cA = cAB | NOT(oa);




a_lo -= b_lo & -cAB;
a_hi -= b_hi & -cAB;
pa -= qa & -(int32_t)cAB;
pb -= qb & -(int32_t)cAB;
b_lo -= a_lo & -cBA;
b_hi -= a_hi & -cBA;
qa -= pa & -(int32_t)cBA;
qb -= pb & -(int32_t)cBA;




a_lo += a_lo & (cA - 1);
pa += pa & ((int32_t)cA - 1);
pb += pb & ((int32_t)cA - 1);
a_hi ^= (a_hi ^ (a_hi >> 1)) & -cA;
b_lo += b_lo & -cA;
qa += qa & -(int32_t)cA;
qb += qb & -(int32_t)cA;
b_hi ^= (b_hi ^ (b_hi >> 1)) & (cA - 1);
}




r = co_reduce(a, b, len, pa, pb, qa, qb);
pa -= pa * ((r & 1) << 1);
pb -= pb * ((r & 1) << 1);
qa -= qa * (r & 2);
qb -= qb * (r & 2);
co_reduce_mod(u, v, len, pa, pb, qa, qb, m + 1, m0i);
}







r = (a[0] | b[0]) ^ 1;
u[0] |= v[0];
for (k = 1; k < len; k ++) {
r |= a[k] | b[k];
u[k] |= v[k];
}
return EQ0(r);
}
