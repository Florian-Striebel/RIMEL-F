























#include "inner.h"

#define ORDER_LEN ((BR_MAX_EC_SIZE + 7) >> 3)


size_t
br_ecdsa_i31_sign_asn1(const br_ec_impl *impl,
const br_hash_class *hf, const void *hash_value,
const br_ec_private_key *sk, void *sig)
{
unsigned char rsig[(ORDER_LEN << 1) + 12];
size_t sig_len;

sig_len = br_ecdsa_i31_sign_raw(impl, hf, hash_value, sk, rsig);
if (sig_len == 0) {
return 0;
}
sig_len = br_ecdsa_raw_to_asn1(rsig, sig_len);
memcpy(sig, rsig, sig_len);
return sig_len;
}
