























#include "inner.h"

#define U (1 + (BR_MAX_RSA_FACTOR >> 5))


uint32_t
br_rsa_i32_private(unsigned char *x, const br_rsa_private_key *sk)
{
const unsigned char *p, *q;
size_t plen, qlen;
uint32_t tmp[6 * U];
uint32_t *mp, *mq, *s1, *s2, *t1, *t2, *t3;
uint32_t p0i, q0i;
size_t xlen, u;
uint32_t r;









mq = tmp;
mp = tmp + U;
t2 = tmp + 2 * U;
s2 = tmp + 3 * U;
s1 = tmp + 4 * U;
t1 = tmp + 5 * U;
t3 = s2;





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
if (plen > (BR_MAX_RSA_FACTOR >> 3)
|| qlen > (BR_MAX_RSA_FACTOR >> 3))
{
return 0;
}




br_i32_decode(mp, p, plen);
br_i32_decode(mq, q, qlen);




br_i32_zero(t2, mp[0]);
br_i32_mulacc(t2, mp, mq);
xlen = (sk->n_bitlen + 7) >> 3;
br_i32_encode(t2 + 2 * U, xlen, t2);
u = xlen;
r = 0;
while (u > 0) {
uint32_t wn, wx;

u --;
wn = ((unsigned char *)(t2 + 2 * U))[u];
wx = x[u];
r = ((wx - (wn + r)) >> 8) & 1;
}




p0i = br_i32_ninv32(mp[1]);
br_i32_decode_reduce(s1, x, xlen, mp);
br_i32_modpow(s1, sk->dp, sk->dplen, mp, p0i, t1, t2);




q0i = br_i32_ninv32(mq[1]);
br_i32_decode_reduce(s2, x, xlen, mq);
br_i32_modpow(s2, sk->dq, sk->dqlen, mq, q0i, t1, t2);














br_i32_reduce(t2, s2, mp);
br_i32_add(s1, mp, br_i32_sub(s1, t2, 1));
br_i32_to_monty(s1, mp);
br_i32_decode_reduce(t1, sk->iq, sk->iqlen, mp);
br_i32_montymul(t2, s1, t1, mp, p0i);












br_i32_mulacc(t3, mq, t2);





br_i32_encode(x, xlen, t3);





return p0i & q0i & r;
}
