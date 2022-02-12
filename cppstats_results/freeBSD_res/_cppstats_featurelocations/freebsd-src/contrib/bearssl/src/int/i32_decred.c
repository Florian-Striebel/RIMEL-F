























#include "inner.h"


void
br_i32_decode_reduce(uint32_t *x,
const void *src, size_t len, const uint32_t *m)
{
uint32_t m_bitlen;
size_t mblen, k, q;
const unsigned char *buf;

m_bitlen = m[0];




if (m_bitlen == 0) {
x[0] = 0;
return;
}




br_i32_zero(x, m_bitlen);






mblen = (m_bitlen + 7) >> 3;
k = mblen - 1;




if (k >= len) {
br_i32_decode(x, src, len);
x[0] = m_bitlen;
return;
}






buf = src;
q = (len - k + 3) & ~(size_t)3;







if (q > len) {
int i;
uint32_t w;

w = 0;
for (i = 0; i < 4; i ++) {
w <<= 8;
if (q <= len) {
w |= buf[len - q];
}
q --;
}
br_i32_muladd_small(x, w, m);
} else {
br_i32_decode(x, buf, len - q);
x[0] = m_bitlen;
}





for (k = len - q; k < len; k += 4) {
br_i32_muladd_small(x, br_dec32be(buf + k), m);
}
}
