























#include "inner.h"


void
br_ssl_server_init_mine2c(br_ssl_server_context *cc,
const br_x509_certificate *chain, size_t chain_len,
const br_rsa_private_key *sk)
{
static const uint16_t suites[] = {
BR_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256
};




br_ssl_server_zero(cc);
br_ssl_engine_set_versions(&cc->eng, BR_TLS12, BR_TLS12);




br_ssl_engine_set_suites(&cc->eng, suites,
(sizeof suites) / (sizeof suites[0]));
br_ssl_engine_set_ec(&cc->eng, &br_ec_all_m15);





br_ssl_server_set_single_rsa(cc, chain, chain_len, sk,
BR_KEYTYPE_SIGN, 0, br_rsa_i31_pkcs1_sign);




br_ssl_engine_set_hash(&cc->eng, br_sha256_ID, &br_sha256_vtable);




br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);




br_ssl_engine_set_default_chapol(&cc->eng);
}
