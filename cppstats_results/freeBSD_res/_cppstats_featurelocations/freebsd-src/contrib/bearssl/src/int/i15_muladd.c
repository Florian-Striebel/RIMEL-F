























#include "inner.h"





static uint32_t
divrem16(uint32_t x, uint32_t d, uint32_t *r)
{
int i;
uint32_t q;

q = 0;
d <<= 16;
for (i = 16; i >= 0; i --) {
uint32_t ctl;

ctl = LE(d, x);
q |= ctl << i;
x -= (-ctl) & d;
d >>= 1;
}
if (r != NULL) {
*r = x;
}
return q;
}


void
br_i15_muladd_small(uint16_t *x, uint16_t z, const uint16_t *m)
{




unsigned m_bitlen, mblr;
size_t u, mlen;
uint32_t hi, a0, a, b, q;
uint32_t cc, tb, over, under;




m_bitlen = m[0];
if (m_bitlen == 0) {
return;
}
if (m_bitlen <= 15) {
uint32_t rem;

divrem16(((uint32_t)x[1] << 15) | z, m[1], &rem);
x[1] = rem;
return;
}
mlen = (m_bitlen + 15) >> 4;
mblr = m_bitlen & 15;



























hi = x[mlen];
if (mblr == 0) {
a0 = x[mlen];
memmove(x + 2, x + 1, (mlen - 1) * sizeof *x);
x[1] = z;
a = (a0 << 15) + x[mlen];
b = m[mlen];
} else {
a0 = (x[mlen] << (15 - mblr)) | (x[mlen - 1] >> mblr);
memmove(x + 2, x + 1, (mlen - 1) * sizeof *x);
x[1] = z;
a = (a0 << 15) | (((x[mlen] << (15 - mblr))
| (x[mlen - 1] >> mblr)) & 0x7FFF);
b = (m[mlen] << (15 - mblr)) | (m[mlen - 1] >> mblr);
}
q = divrem16(a, b, NULL);









q = MUX(EQ(b, a0), 0x7FFF, q - 1 + ((q - 1) >> 31));










cc = 0;
tb = 1;
for (u = 1; u <= mlen; u ++) {
uint32_t mw, zl, xw, nxw;

mw = m[u];
zl = MUL15(mw, q) + cc;
cc = zl >> 15;
zl &= 0x7FFF;
xw = x[u];
nxw = xw - zl;
cc += nxw >> 31;
nxw &= 0x7FFF;
x[u] = nxw;
tb = MUX(EQ(nxw, mw), tb, GT(nxw, mw));
}








over = GT(cc, hi);
under = ~over & (tb | LT(cc, hi));
br_i15_add(x, m, over);
br_i15_sub(x, m, under);
}
