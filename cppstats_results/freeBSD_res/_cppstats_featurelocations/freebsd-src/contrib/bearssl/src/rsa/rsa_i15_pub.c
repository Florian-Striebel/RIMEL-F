























#include "inner.h"





#define TLEN (4 * (2 + ((BR_MAX_RSA_SIZE + 14) / 15)))


uint32_t
br_rsa_i15_public(unsigned char *x, size_t xlen,
const br_rsa_public_key *pk)
{
const unsigned char *n;
size_t nlen;
uint16_t tmp[1 + TLEN];
uint16_t *m, *a, *t;
size_t fwlen;
long z;
uint16_t m0i;
uint32_t r;





n = pk->n;
nlen = pk->nlen;
while (nlen > 0 && *n == 0) {
n ++;
nlen --;
}
if (nlen == 0 || nlen > (BR_MAX_RSA_SIZE >> 3) || xlen != nlen) {
return 0;
}
z = (long)nlen << 3;
fwlen = 1;
while (z > 0) {
z -= 15;
fwlen ++;
}



fwlen += (fwlen & 1);









m = tmp;
if (((uintptr_t)m & 2) == 0) {
m ++;
}
a = m + fwlen;
t = m + 2 * fwlen;




br_i15_decode(m, n, nlen);
m0i = br_i15_ninv15(m[1]);





r = m0i & 1;




r &= br_i15_decode_mod(a, x, xlen, m);




br_i15_modpow_opt(a, pk->e, pk->elen, m, m0i, t, TLEN - 2 * fwlen);




br_i15_encode(x, xlen, a);
return r;
}
