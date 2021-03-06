























#include "bearssl.h"




















void
example_client_profile(br_ssl_client_context *cc
)
{





static const uint16_t suites[] = {
BR_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
BR_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
BR_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
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





br_ssl_client_zero(cc);








br_ssl_engine_set_versions(&cc->eng, BR_TLS10, BR_TLS12);













br_ssl_engine_set_prf10(&cc->eng, &br_tls10_prf);
br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);
br_ssl_engine_set_prf_sha384(&cc->eng, &br_tls12_sha384_prf);



























br_ssl_engine_set_hash(&cc->eng, br_md5_ID, &br_md5_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha1_ID, &br_sha1_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha224_ID, &br_sha224_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha256_ID, &br_sha256_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha384_ID, &br_sha384_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha512_ID, &br_sha512_vtable);







br_ssl_engine_set_suites(&cc->eng, suites,
(sizeof suites) / (sizeof suites[0]));































































br_ssl_client_set_default_rsapub(cc);
br_ssl_engine_set_default_rsavrfy(&cc->eng);
br_ssl_engine_set_default_ecdsa(&cc->eng);
















br_ssl_engine_set_cbc(&cc->eng,
&br_sslrec_in_cbc_vtable,
&br_sslrec_out_cbc_vtable);
br_ssl_engine_set_gcm(&cc->eng,
&br_sslrec_in_gcm_vtable,
&br_sslrec_out_gcm_vtable);
br_ssl_engine_set_chapol(&cc->eng,
&br_sslrec_in_chapol_vtable,
&br_sslrec_out_chapol_vtable);


























































br_ssl_engine_set_aes_cbc(&cc->eng,
&br_aes_ct_cbcenc_vtable,
&br_aes_ct_cbcdec_vtable);
br_ssl_engine_set_aes_ctr(&cc->eng,
&br_aes_ct_ctr_vtable);





















br_ssl_engine_set_des_cbc(&cc->eng,
&br_des_ct_cbcenc_vtable,
&br_des_ct_cbcdec_vtable);



























br_ssl_engine_set_ghash(&cc->eng,
&br_ghash_ctmul);









#if 0

























br_x509_minimal_init(xc, &br_sha256_vtable,
trust_anchors, trust_anchors_num);











br_x509_minimal_set_rsa(xc, &br_rsa_i31_pkcs1_vrfy);
br_x509_minimal_set_ecdsa(xc,
&br_ec_prime_i31, &br_ecdsa_i31_vrfy_asn1);











br_ssl_engine_set_hash(xc, br_sha1_ID, &br_sha1_vtable);
br_ssl_engine_set_hash(xc, br_sha224_ID, &br_sha224_vtable);
br_ssl_engine_set_hash(xc, br_sha256_ID, &br_sha256_vtable);
br_ssl_engine_set_hash(xc, br_sha384_ID, &br_sha384_vtable);
br_ssl_engine_set_hash(xc, br_sha512_ID, &br_sha512_vtable);




br_ssl_engine_set_x509(&cc->eng, &xc->vtable);
#endif
}








void
example_server_profile(br_ssl_server_context *cc,
const br_x509_certificate *chain, size_t chain_len,
const br_rsa_private_key *sk)
{
static const uint16_t suites[] = {
BR_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
BR_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,
BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
BR_TLS_RSA_WITH_AES_128_GCM_SHA256,
BR_TLS_RSA_WITH_AES_256_GCM_SHA384,
BR_TLS_RSA_WITH_AES_128_CBC_SHA256,
BR_TLS_RSA_WITH_AES_256_CBC_SHA256,
BR_TLS_RSA_WITH_AES_128_CBC_SHA,
BR_TLS_RSA_WITH_AES_256_CBC_SHA,
BR_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,
BR_TLS_RSA_WITH_3DES_EDE_CBC_SHA
};

br_ssl_server_zero(cc);
br_ssl_engine_set_versions(&cc->eng, BR_TLS10, BR_TLS12);

br_ssl_engine_set_prf10(&cc->eng, &br_tls10_prf);
br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);
br_ssl_engine_set_prf_sha384(&cc->eng, &br_tls12_sha384_prf);










br_ssl_engine_set_hash(&cc->eng, br_md5_ID, &br_md5_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha1_ID, &br_sha1_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha224_ID, &br_sha224_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha256_ID, &br_sha256_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha384_ID, &br_sha384_vtable);
br_ssl_engine_set_hash(&cc->eng, br_sha512_ID, &br_sha512_vtable);

br_ssl_engine_set_suites(&cc->eng, suites,
(sizeof suites) / (sizeof suites[0]));





br_ssl_engine_set_ec(&cc->eng, &br_ec_prime_i31);










br_ssl_server_set_single_rsa(cc, chain, chain_len, sk,
BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN,
br_rsa_i31_private, br_rsa_i31_pkcs1_sign);



























br_ssl_engine_set_cbc(&cc->eng,
&br_sslrec_in_cbc_vtable,
&br_sslrec_out_cbc_vtable);
br_ssl_engine_set_gcm(&cc->eng,
&br_sslrec_in_gcm_vtable,
&br_sslrec_out_gcm_vtable);

br_ssl_engine_set_aes_cbc(&cc->eng,
&br_aes_ct_cbcenc_vtable,
&br_aes_ct_cbcdec_vtable);
br_ssl_engine_set_aes_ctr(&cc->eng,
&br_aes_ct_ctr_vtable);





















br_ssl_engine_set_des_cbc(&cc->eng,
&br_des_ct_cbcenc_vtable,
&br_des_ct_cbcdec_vtable);






br_ssl_engine_set_ghash(&cc->eng,
&br_ghash_ctmul);








}
