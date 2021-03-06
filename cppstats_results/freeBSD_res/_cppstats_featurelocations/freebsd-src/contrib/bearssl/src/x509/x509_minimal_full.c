























#include "inner.h"


void
br_x509_minimal_init_full(br_x509_minimal_context *xc,
const br_x509_trust_anchor *trust_anchors, size_t trust_anchors_num)
{





static const br_hash_class *hashes[] = {
&br_md5_vtable,
&br_sha1_vtable,
&br_sha224_vtable,
&br_sha256_vtable,
&br_sha384_vtable,
&br_sha512_vtable
};

int id;

br_x509_minimal_init(xc, &br_sha256_vtable,
trust_anchors, trust_anchors_num);
br_x509_minimal_set_rsa(xc, &br_rsa_i31_pkcs1_vrfy);
br_x509_minimal_set_ecdsa(xc,
&br_ec_prime_i31, &br_ecdsa_i31_vrfy_asn1);
for (id = br_md5_ID; id <= br_sha512_ID; id ++) {
const br_hash_class *hc;

hc = hashes[id - 1];
br_x509_minimal_set_hash(xc, id, hc);
}
}
