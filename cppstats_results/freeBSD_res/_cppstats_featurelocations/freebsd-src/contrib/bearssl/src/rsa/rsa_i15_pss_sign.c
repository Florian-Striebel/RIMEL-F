























#include "inner.h"


uint32_t
br_rsa_i15_pss_sign(const br_prng_class **rng,
const br_hash_class *hf_data, const br_hash_class *hf_mgf1,
const unsigned char *hash, size_t salt_len,
const br_rsa_private_key *sk, unsigned char *x)
{
if (!br_rsa_pss_sig_pad(rng, hf_data, hf_mgf1, hash,
salt_len, sk->n_bitlen, x))
{
return 0;
}
return br_rsa_i15_private(x, sk);
}
