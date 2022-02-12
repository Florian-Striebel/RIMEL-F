























#include "inner.h"

#if BR_INT128 || BR_UMUL128


uint32_t
br_rsa_i62_keygen(const br_prng_class **rng,
br_rsa_private_key *sk, void *kbuf_priv,
br_rsa_public_key *pk, void *kbuf_pub,
unsigned size, uint32_t pubexp)
{
return br_rsa_i31_keygen_inner(rng,
sk, kbuf_priv, pk, kbuf_pub, size, pubexp,
&br_i62_modpow_opt_as_i31);
}


br_rsa_keygen
br_rsa_i62_keygen_get()
{
return &br_rsa_i62_keygen;
}

#else


br_rsa_keygen
br_rsa_i62_keygen_get()
{
return 0;
}

#endif
