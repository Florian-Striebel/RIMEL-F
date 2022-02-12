























#include "inner.h"


uint32_t
br_rsa_pss_sig_pad(const br_prng_class **rng,
const br_hash_class *hf_data, const br_hash_class *hf_mgf1,
const unsigned char *hash, size_t salt_len,
uint32_t n_bitlen, unsigned char *x)
{
size_t xlen, hash_len;
br_hash_compat_context hc;
unsigned char *salt, *seed;

hash_len = br_digest_size(hf_data);







n_bitlen --;
if ((n_bitlen & 7) == 0) {
*x ++ = 0;
}
xlen = (n_bitlen + 7) >> 3;





if (hash_len > xlen || salt_len > xlen
|| (hash_len + salt_len + 2) > xlen)
{
return 0;
}




salt = x + xlen - hash_len - salt_len - 1;
if (salt_len != 0) {
(*rng)->generate(rng, salt, salt_len);
}




seed = x + xlen - hash_len - 1;
hf_data->init(&hc.vtable);
memset(seed, 0, 8);
hf_data->update(&hc.vtable, seed, 8);
hf_data->update(&hc.vtable, hash, hash_len);
hf_data->update(&hc.vtable, salt, salt_len);
hf_data->out(&hc.vtable, seed);





memset(x, 0, xlen - salt_len - hash_len - 2);
x[xlen - salt_len - hash_len - 2] = 0x01;




br_mgf1_xor(x, xlen - hash_len - 1, hf_mgf1, seed, hash_len);





x[0] &= 0xFF >> (((uint32_t)xlen << 3) - n_bitlen);





x[xlen - 1] = 0xBC;

return 1;
}
