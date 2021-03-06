























#include "inner.h"


uint32_t
br_rsa_i31_pss_vrfy(const unsigned char *x, size_t xlen,
const br_hash_class *hf_data, const br_hash_class *hf_mgf1,
const void *hash, size_t salt_len, const br_rsa_public_key *pk)
{
unsigned char sig[BR_MAX_RSA_SIZE >> 3];

if (xlen > (sizeof sig)) {
return 0;
}
memcpy(sig, x, xlen);
if (!br_rsa_i31_public(sig, xlen, pk)) {
return 0;
}
return br_rsa_pss_sig_unpad(hf_data, hf_mgf1,
hash, salt_len, pk, sig);
}
