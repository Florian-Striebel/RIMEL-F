























#include "inner.h"


void
br_i15_decode_reduce(uint16_t *x,
const void *src, size_t len, const uint16_t *m)
{
uint32_t m_ebitlen, m_rbitlen;
size_t mblen, k;
const unsigned char *buf;
uint32_t acc;
int acc_len;




m_ebitlen = m[0];




if (m_ebitlen == 0) {
x[0] = 0;
return;
}




br_i15_zero(x, m_ebitlen);





m_rbitlen = m_ebitlen >> 4;
m_rbitlen = (m_ebitlen & 15) + (m_rbitlen << 4) - m_rbitlen;
mblen = (m_rbitlen + 7) >> 3;
k = mblen - 1;
if (k >= len) {
br_i15_decode(x, src, len);
x[0] = m_ebitlen;
return;
}
buf = src;
br_i15_decode(x, buf, k);
x[0] = m_ebitlen;




acc = 0;
acc_len = 0;
while (k < len) {
uint32_t v;

v = buf[k ++];
acc = (acc << 8) | v;
acc_len += 8;
if (acc_len >= 15) {
br_i15_muladd_small(x, acc >> (acc_len - 15), m);
acc_len -= 15;
acc &= ~((uint32_t)-1 << acc_len);
}
}





if (acc_len != 0) {
acc = (acc | (x[1] << acc_len)) & 0x7FFF;
br_i15_rshift(x, 15 - acc_len);
br_i15_muladd_small(x, acc, m);
}
}
