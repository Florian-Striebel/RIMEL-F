























#include "inner.h"

#define I15_LEN ((BR_MAX_EC_SIZE + 29) / 15)
#define POINT_LEN (1 + (((BR_MAX_EC_SIZE + 7) >> 3) << 1))


uint32_t
br_ecdsa_i15_vrfy_raw(const br_ec_impl *impl,
const void *hash, size_t hash_len,
const br_ec_public_key *pk,
const void *sig, size_t sig_len)
{





const br_ec_curve_def *cd;
uint16_t n[I15_LEN], r[I15_LEN], s[I15_LEN], t1[I15_LEN], t2[I15_LEN];
unsigned char tx[(BR_MAX_EC_SIZE + 7) >> 3];
unsigned char ty[(BR_MAX_EC_SIZE + 7) >> 3];
unsigned char eU[POINT_LEN];
size_t nlen, rlen, ulen;
uint16_t n0i;
uint32_t res;




if (((impl->supported_curves >> pk->curve) & 1) == 0) {
return 0;
}




switch (pk->curve) {
case BR_EC_secp256r1:
cd = &br_secp256r1;
break;
case BR_EC_secp384r1:
cd = &br_secp384r1;
break;
case BR_EC_secp521r1:
cd = &br_secp521r1;
break;
default:
return 0;
}




if (sig_len & 1) {
return 0;
}
rlen = sig_len >> 1;




if (pk->qlen != cd->generator_len) {
return 0;
}





nlen = cd->order_len;
br_i15_decode(n, cd->order, nlen);
n0i = br_i15_ninv15(n[1]);
if (!br_i15_decode_mod(r, sig, rlen, n)) {
return 0;
}
if (!br_i15_decode_mod(s, (const unsigned char *)sig + rlen, rlen, n)) {
return 0;
}
if (br_i15_iszero(s)) {
return 0;
}










br_i15_from_monty(s, n, n0i);
memcpy(tx, cd->order, nlen);
tx[nlen - 1] -= 2;
br_i15_modpow(s, tx, nlen, n, n0i, t1, t2);







br_ecdsa_i15_bits2int(t1, hash, hash_len, n[0]);
br_i15_sub(t1, n, br_i15_sub(t1, n, 0) ^ 1);





br_i15_montymul(t2, t1, s, n, n0i);
br_i15_encode(ty, nlen, t2);




br_i15_montymul(t1, r, s, n, n0i);
br_i15_encode(tx, nlen, t1);




ulen = cd->generator_len;
memcpy(eU, pk->q, ulen);
res = impl->muladd(eU, NULL, ulen,
tx, nlen, ty, nlen, cd->curve);









br_i15_zero(t1, n[0]);
br_i15_decode(t1, &eU[1], ulen >> 1);
t1[0] = n[0];
br_i15_sub(t1, n, br_i15_sub(t1, n, 0) ^ 1);
res &= ~br_i15_sub(t1, r, 1);
res &= br_i15_iszero(t1);
return res;
}
