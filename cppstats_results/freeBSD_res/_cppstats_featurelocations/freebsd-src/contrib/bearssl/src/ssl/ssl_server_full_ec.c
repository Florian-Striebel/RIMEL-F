























#include "inner.h"


void
br_ssl_server_init_full_ec(br_ssl_server_context *cc,
const br_x509_certificate *chain, size_t chain_len,
unsigned cert_issuer_key_type, const br_ec_private_key *sk)
{

















static const uint16_t suites[] = {
BR_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
BR_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,
BR_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384,
BR_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
BR_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,
BR_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384,
BR_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384,
BR_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,
BR_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA,
BR_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA,
BR_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA,
BR_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA,
BR_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA,
BR_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA
};






static const br_hash_class *hashes[] = {
&br_md5_vtable,
&br_sha1_vtable,
&br_sha224_vtable,
&br_sha256_vtable,
&br_sha384_vtable,
&br_sha512_vtable
};

int id;





br_ssl_server_zero(cc);
br_ssl_engine_set_versions(&cc->eng, BR_TLS10, BR_TLS12);




br_ssl_engine_set_suites(&cc->eng, suites,
(sizeof suites) / (sizeof suites[0]));
br_ssl_engine_set_default_ec(&cc->eng);





br_ssl_server_set_single_ec(cc, chain, chain_len, sk,
BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN,
cert_issuer_key_type,
br_ssl_engine_get_ec(&cc->eng),
#if BR_LOMUL
br_ecdsa_i15_sign_asn1
#else
br_ecdsa_i31_sign_asn1
#endif
);




for (id = br_md5_ID; id <= br_sha512_ID; id ++) {
const br_hash_class *hc;

hc = hashes[id - 1];
br_ssl_engine_set_hash(&cc->eng, id, hc);
}




br_ssl_engine_set_prf10(&cc->eng, &br_tls10_prf);
br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);
br_ssl_engine_set_prf_sha384(&cc->eng, &br_tls12_sha384_prf);




br_ssl_engine_set_default_aes_cbc(&cc->eng);
br_ssl_engine_set_default_aes_ccm(&cc->eng);
br_ssl_engine_set_default_aes_gcm(&cc->eng);
br_ssl_engine_set_default_des_cbc(&cc->eng);
br_ssl_engine_set_default_chapol(&cc->eng);
}
