























#include "inner.h"


uint32_t
br_rsa_i31_pkcs1_vrfy(const unsigned char *x, size_t xlen,
const unsigned char *hash_oid, size_t hash_len,
const br_rsa_public_key *pk, unsigned char *hash_out)
{
unsigned char sig[BR_MAX_RSA_SIZE >> 3];

if (xlen > (sizeof sig)) {
return 0;
}
memcpy(sig, x, xlen);
if (!br_rsa_i31_public(sig, xlen, pk)) {
return 0;
}
return br_rsa_pkcs1_sig_unpad(sig, xlen, hash_oid, hash_len, hash_out);
}
