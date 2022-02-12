























#include "inner.h"


void
br_ssl_server_init_minf2g(br_ssl_server_context *cc,
const br_x509_certificate *chain, size_t chain_len,
const br_ec_private_key *sk)
{
static const uint16_t suites[] = {
BR_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
};




br_ssl_server_zero(cc);
br_ssl_engine_set_versions(&cc->eng, BR_TLS12, BR_TLS12);




br_ssl_engine_set_suites(&cc->eng, suites,
(sizeof suites) / (sizeof suites[0]));
br_ssl_engine_set_ec(&cc->eng, &br_ec_all_m15);





br_ssl_server_set_single_ec(cc, chain, chain_len, sk,
BR_KEYTYPE_SIGN, 0, &br_ec_all_m15, br_ecdsa_i31_sign_asn1);




br_ssl_engine_set_hash(&cc->eng, br_sha256_ID, &br_sha256_vtable);




br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);




br_ssl_engine_set_default_aes_gcm(&cc->eng);
}
