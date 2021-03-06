























#include "inner.h"

#if BR_INT128 || BR_UMUL128


uint32_t
br_rsa_i62_pkcs1_sign(const unsigned char *hash_oid,
const unsigned char *hash, size_t hash_len,
const br_rsa_private_key *sk, unsigned char *x)
{
if (!br_rsa_pkcs1_sig_pad(hash_oid, hash, hash_len, sk->n_bitlen, x)) {
return 0;
}
return br_rsa_i62_private(x, sk);
}


br_rsa_pkcs1_sign
br_rsa_i62_pkcs1_sign_get(void)
{
return &br_rsa_i62_pkcs1_sign;
}

#else


br_rsa_pkcs1_sign
br_rsa_i62_pkcs1_sign_get(void)
{
return 0;
}

#endif
