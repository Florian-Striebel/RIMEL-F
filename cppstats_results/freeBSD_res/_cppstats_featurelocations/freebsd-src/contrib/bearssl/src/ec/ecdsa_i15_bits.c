























#include "inner.h"


void
br_ecdsa_i15_bits2int(uint16_t *x,
const void *src, size_t len, uint32_t ebitlen)
{
uint32_t bitlen, hbitlen;
int sc;

bitlen = ebitlen - (ebitlen >> 4);
hbitlen = (uint32_t)len << 3;
if (hbitlen > bitlen) {
len = (bitlen + 7) >> 3;
sc = (int)((hbitlen - bitlen) & 7);
} else {
sc = 0;
}
br_i15_zero(x, ebitlen);
br_i15_decode(x, src, len);
br_i15_rshift(x, sc);
x[0] = ebitlen;
}
