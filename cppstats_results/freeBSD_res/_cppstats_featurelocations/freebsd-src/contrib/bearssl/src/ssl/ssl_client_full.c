























#include "inner.h"


void
br_ssl_client_init_full(br_ssl_client_context *cc,
br_x509_minimal_context *xc,
const br_x509_trust_anchor *trust_anchors, size_t trust_anchors_num)
{



















static const uint16_t suites[] = {
BR_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
BR_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
BR_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
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
BR_TLS_RSA_WITH_AES_128_GCM_SHA256,
BR_TLS_RSA_WITH_AES_256_GCM_SHA384,
BR_TLS_RSA_WITH_AES_128_CCM,
BR_TLS_RSA_WITH_AES_256_CCM,
BR_TLS_RSA_WITH_AES_128_CCM_8,
BR_TLS_RSA_WITH_AES_256_CCM_8,
BR_TLS_RSA_WITH_AES_128_CBC_SHA256,
BR_TLS_RSA_WITH_AES_256_CBC_SHA256,
BR_TLS_RSA_WITH_AES_128_CBC_SHA,
BR_TLS_RSA_WITH_AES_256_CBC_SHA,
BR_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA,
BR_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,
BR_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA,
BR_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA,
BR_TLS_RSA_WITH_3DES_EDE_CBC_SHA
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





br_ssl_client_zero(cc);
br_ssl_engine_set_versions(&cc->eng, BR_TLS10, BR_TLS12);





br_x509_minimal_init(xc, &br_sha256_vtable,
trust_anchors, trust_anchors_num);







br_ssl_engine_set_suites(&cc->eng, suites,
(sizeof suites) / (sizeof suites[0]));
br_ssl_client_set_default_rsapub(cc);
br_ssl_engine_set_default_rsavrfy(&cc->eng);
br_ssl_engine_set_default_ecdsa(&cc->eng);
br_x509_minimal_set_rsa(xc, br_ssl_engine_get_rsavrfy(&cc->eng));
br_x509_minimal_set_ecdsa(xc,
br_ssl_engine_get_ec(&cc->eng),
br_ssl_engine_get_ecdsa(&cc->eng));





for (id = br_md5_ID; id <= br_sha512_ID; id ++) {
const br_hash_class *hc;

hc = hashes[id - 1];
br_ssl_engine_set_hash(&cc->eng, id, hc);
br_x509_minimal_set_hash(xc, id, hc);
}




br_ssl_engine_set_x509(&cc->eng, &xc->vtable);




br_ssl_engine_set_prf10(&cc->eng, &br_tls10_prf);
br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);
br_ssl_engine_set_prf_sha384(&cc->eng, &br_tls12_sha384_prf);





br_ssl_engine_set_default_aes_cbc(&cc->eng);
br_ssl_engine_set_default_aes_ccm(&cc->eng);
br_ssl_engine_set_default_aes_gcm(&cc->eng);
br_ssl_engine_set_default_des_cbc(&cc->eng);
br_ssl_engine_set_default_chapol(&cc->eng);
}
