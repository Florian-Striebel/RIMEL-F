























#include "inner.h"


size_t
br_rsa_i32_oaep_encrypt(
const br_prng_class **rnd, const br_hash_class *dig,
const void *label, size_t label_len,
const br_rsa_public_key *pk,
void *dst, size_t dst_max_len,
const void *src, size_t src_len)
{
size_t dlen;

dlen = br_rsa_oaep_pad(rnd, dig, label, label_len,
pk, dst, dst_max_len, src, src_len);
if (dlen == 0) {
return 0;
}
return dlen & -(size_t)br_rsa_i32_public(dst, dlen, pk);
}
