























#include "inner.h"

#if BR_INT128 || BR_UMUL128

#define U (2 + ((BR_MAX_RSA_FACTOR + 30) / 31))
#define TLEN (4 * U)


uint32_t
br_rsa_i62_private(unsigned char *x, const br_rsa_private_key *sk)
{
const unsigned char *p, *q;
size_t plen, qlen;
size_t fwlen;
uint32_t p0i, q0i;
size_t xlen, u;
uint64_t tmp[TLEN];
long z;
uint32_t *mp, *mq, *s1, *s2, *t1, *t2, *t3;
uint32_t r;






p = sk->p;
plen = sk->plen;
while (plen > 0 && *p == 0) {
p ++;
plen --;
}
q = sk->q;
qlen = sk->qlen;
while (qlen > 0 && *q == 0) {
q ++;
qlen --;
}




z = (long)(plen > qlen ? plen : qlen) << 3;
fwlen = 1;
while (z > 0) {
z -= 31;
fwlen ++;
}




fwlen = (fwlen + 1) >> 1;




if (6 * fwlen > TLEN) {
return 0;
}




xlen = (sk->n_bitlen + 7) >> 3;




mq = (uint32_t *)tmp;
br_i31_decode(mq, q, qlen);




t1 = (uint32_t *)(tmp + fwlen);
br_i31_decode(t1, p, plen);






t2 = (uint32_t *)(tmp + 2 * fwlen);
br_i31_zero(t2, mq[0]);
br_i31_mulacc(t2, mq, t1);










t3 = (uint32_t *)(tmp + 4 * fwlen);
br_i31_encode(t3, xlen, t2);
u = xlen;
r = 0;
while (u > 0) {
uint32_t wn, wx;

u --;
wn = ((unsigned char *)t3)[u];
wx = x[u];
r = ((wx - (wn + r)) >> 8) & 1;
}




mp = (uint32_t *)(tmp + 2 * fwlen);
memmove(mp, t1, 2 * fwlen * sizeof *t1);




q0i = br_i31_ninv31(mq[1]);
s2 = (uint32_t *)(tmp + fwlen);
br_i31_decode_reduce(s2, x, xlen, mq);
r &= br_i62_modpow_opt(s2, sk->dq, sk->dqlen, mq, q0i,
tmp + 3 * fwlen, TLEN - 3 * fwlen);




p0i = br_i31_ninv31(mp[1]);
s1 = (uint32_t *)(tmp + 3 * fwlen);
br_i31_decode_reduce(s1, x, xlen, mp);
r &= br_i62_modpow_opt(s1, sk->dp, sk->dplen, mp, p0i,
tmp + 4 * fwlen, TLEN - 4 * fwlen);














t1 = (uint32_t *)(tmp + 4 * fwlen);
t2 = (uint32_t *)(tmp + 5 * fwlen);
br_i31_reduce(t2, s2, mp);
br_i31_add(s1, mp, br_i31_sub(s1, t2, 1));
br_i31_to_monty(s1, mp);
br_i31_decode_reduce(t1, sk->iq, sk->iqlen, mp);
br_i31_montymul(t2, s1, t1, mp, p0i);













t3 = s2;
br_i31_mulacc(t3, mq, t2);





br_i31_encode(x, xlen, t3);





return p0i & q0i & r;
}


br_rsa_private
br_rsa_i62_private_get(void)
{
return &br_rsa_i62_private;
}

#else


br_rsa_private
br_rsa_i62_private_get(void)
{
return 0;
}

#endif
