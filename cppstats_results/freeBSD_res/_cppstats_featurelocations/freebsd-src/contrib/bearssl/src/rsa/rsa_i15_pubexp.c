























#include "inner.h"





static uint32_t
get_pubexp(const unsigned char *pbuf, size_t plen,
const unsigned char *dpbuf, size_t dplen)
{











uint16_t tmp[6 * ((BR_MAX_RSA_FACTOR + 29) / 15)];
uint16_t *p, *dp, *x;
size_t len;
uint32_t e;





while (plen > 0 && *pbuf == 0) {
pbuf ++;
plen --;
}
if (plen == 0 || plen < 5 || plen > (BR_MAX_RSA_FACTOR / 8)) {
return 0;
}





while (dplen > 0 && *dpbuf == 0) {
dpbuf ++;
dplen --;
}
if (dplen > plen || dplen == 0
|| (dplen == plen && dpbuf[0] > pbuf[0]))
{
return 0;
}




if ((pbuf[plen - 1] & 3) != 3 || (dpbuf[dplen - 1] & 1) != 1) {
return 0;
}




p = tmp;
br_i15_decode(p, pbuf, plen);
len = (p[0] + 31) >> 4;
br_i15_rshift(p, 1);






dp = p + len;
memset(dp, 0, len * sizeof *dp);
br_i15_decode(dp, dpbuf, dplen);
dp[0] = p[0];




br_i15_sub(dp, p, NOT(br_i15_sub(dp, p, 0)));






if (br_i15_sub(dp, p, 0) == 0) {
return 0;
}





x = dp + len;
br_i15_zero(x, p[0]);
x[1] = 1;
if (br_i15_moddiv(x, dp, p, br_i15_ninv15(p[1]), x + len) == 0) {
return 0;
}







e = (uint32_t)x[1] | ((uint32_t)x[2] << 15) | ((uint32_t)x[3] << 30);
e &= -LT(br_i15_bit_length(x + 1, len - 1), 35);
e &= -(e & 1);
return e;
}


uint32_t
br_rsa_i15_compute_pubexp(const br_rsa_private_key *sk)
{




uint32_t ep, eq;

ep = get_pubexp(sk->p, sk->plen, sk->dp, sk->dplen);
eq = get_pubexp(sk->q, sk->qlen, sk->dq, sk->dqlen);
return ep & -EQ(ep, eq);
}
