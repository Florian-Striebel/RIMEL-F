























#include "inner.h"


uint32_t
br_rsa_i32_pkcs1_sign(const unsigned char *hash_oid,
const unsigned char *hash, size_t hash_len,
const br_rsa_private_key *sk, unsigned char *x)
{
if (!br_rsa_pkcs1_sig_pad(hash_oid, hash, hash_len, sk->n_bitlen, x)) {
return 0;
}
return br_rsa_i32_private(x, sk);
}
