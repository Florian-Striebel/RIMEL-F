























#include "inner.h"

#if BR_INT128 || BR_UMUL128


uint32_t
br_rsa_i62_oaep_decrypt(const br_hash_class *dig,
const void *label, size_t label_len,
const br_rsa_private_key *sk, void *data, size_t *len)
{
uint32_t r;

if (*len != ((sk->n_bitlen + 7) >> 3)) {
return 0;
}
r = br_rsa_i62_private(data, sk);
r &= br_rsa_oaep_unpad(dig, label, label_len, data, len);
return r;
}


br_rsa_oaep_decrypt
br_rsa_i62_oaep_decrypt_get(void)
{
return &br_rsa_i62_oaep_decrypt;
}

#else


br_rsa_oaep_decrypt
br_rsa_i62_oaep_decrypt_get(void)
{
return 0;
}

#endif
