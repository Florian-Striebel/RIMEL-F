























#include "inner.h"


uint32_t
br_rsa_pss_sig_unpad(const br_hash_class *hf_data,
const br_hash_class *hf_mgf1,
const unsigned char *hash, size_t salt_len,
const br_rsa_public_key *pk, unsigned char *x)
{
size_t u, xlen, hash_len;
br_hash_compat_context hc;
unsigned char *seed, *salt;
unsigned char tmp[64];
uint32_t r, n_bitlen;

hash_len = br_digest_size(hf_data);




r = 0;





for (u = 0; u < pk->nlen; u ++) {
if (pk->n[u] != 0) {
break;
}
}
if (u == pk->nlen) {
return 0;
}
n_bitlen = BIT_LENGTH(pk->n[u]) + ((uint32_t)(pk->nlen - u - 1) << 3);
n_bitlen --;
if ((n_bitlen & 7) == 0) {
r |= *x ++;
} else {
r |= x[0] & (0xFF << (n_bitlen & 7));
}
xlen = (n_bitlen + 7) >> 3;





if (hash_len > xlen || salt_len > xlen
|| (hash_len + salt_len + 2) > xlen)
{
return 0;
}




r |= x[xlen - 1] ^ 0xBC;





seed = x + xlen - hash_len - 1;
br_mgf1_xor(x, xlen - hash_len - 1, hf_mgf1, seed, hash_len);
if ((n_bitlen & 7) != 0) {
x[0] &= 0xFF >> (8 - (n_bitlen & 7));
}




for (u = 0; u < (xlen - hash_len - salt_len - 2); u ++) {
r |= x[u];
}
r |= x[xlen - hash_len - salt_len - 2] ^ 0x01;




salt = x + xlen - hash_len - salt_len - 1;
hf_data->init(&hc.vtable);
memset(tmp, 0, 8);
hf_data->update(&hc.vtable, tmp, 8);
hf_data->update(&hc.vtable, hash, hash_len);
hf_data->update(&hc.vtable, salt, salt_len);
hf_data->out(&hc.vtable, tmp);





for (u = 0; u < hash_len; u ++) {
r |= tmp[u] ^ x[(xlen - salt_len - 1) + u];
}

return EQ0(r);
}
