























#include "inner.h"


void
br_i15_reduce(uint16_t *x, const uint16_t *a, const uint16_t *m)
{
uint32_t m_bitlen, a_bitlen;
size_t mlen, alen, u;

m_bitlen = m[0];
mlen = (m_bitlen + 15) >> 4;

x[0] = m_bitlen;
if (m_bitlen == 0) {
return;
}





a_bitlen = a[0];
alen = (a_bitlen + 15) >> 4;
if (a_bitlen < m_bitlen) {
memcpy(x + 1, a + 1, alen * sizeof *a);
for (u = alen; u < mlen; u ++) {
x[u + 1] = 0;
}
return;
}






memcpy(x + 1, a + 2 + (alen - mlen), (mlen - 1) * sizeof *a);
x[mlen] = 0;
for (u = 1 + alen - mlen; u > 0; u --) {
br_i15_muladd_small(x, a[u], m);
}
}
