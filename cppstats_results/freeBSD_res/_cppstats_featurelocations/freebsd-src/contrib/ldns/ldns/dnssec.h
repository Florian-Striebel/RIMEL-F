





















#if !defined(LDNS_DNSSEC_H)
#define LDNS_DNSSEC_H

#include <ldns/common.h>
#if LDNS_BUILD_CONFIG_HAVE_SSL
#include <openssl/ssl.h>
#include <openssl/evp.h>
#endif
#include <ldns/packet.h>
#include <ldns/keys.h>
#include <ldns/zone.h>
#include <ldns/resolver.h>
#include <ldns/dnssec_zone.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LDNS_MAX_KEYLEN 2048
#define LDNS_DNSSEC_KEYPROTO 3

#define LDNS_DEFAULT_EXP_TIME 2419200


#define LDNS_SIGNATURE_LEAVE_ADD_NEW 0
#define LDNS_SIGNATURE_LEAVE_NO_ADD 1
#define LDNS_SIGNATURE_REMOVE_ADD_NEW 2
#define LDNS_SIGNATURE_REMOVE_NO_ADD 3











ldns_rr *ldns_dnssec_get_rrsig_for_name_and_type(const ldns_rdf *name,
const ldns_rr_type type,
const ldns_rr_list *rrs);










ldns_rr *ldns_dnssec_get_dnskey_for_rrsig(const ldns_rr *rrsig, const ldns_rr_list *rrs);








ldns_rdf *ldns_nsec_get_bitmap(const ldns_rr *nsec);


#define LDNS_NSEC3_MAX_ITERATIONS 65535




ldns_rdf *
ldns_dnssec_nsec3_closest_encloser(const ldns_rdf *qname,
ldns_rr_type qtype,
const ldns_rr_list *nsec3s);




bool
ldns_dnssec_pkt_has_rrsigs(const ldns_pkt *pkt);





ldns_rr_list *ldns_dnssec_pkt_get_rrsigs_for_name_and_type(const ldns_pkt *pkt, const ldns_rdf *name, ldns_rr_type type);




ldns_rr_list *ldns_dnssec_pkt_get_rrsigs_for_type(const ldns_pkt *pkt, ldns_rr_type type);







uint16_t ldns_calc_keytag(const ldns_rr *key);







uint16_t ldns_calc_keytag_raw(const uint8_t* key, size_t keysize);

#if LDNS_BUILD_CONFIG_HAVE_SSL






DSA *ldns_key_buf2dsa(const ldns_buffer *key);






DSA *ldns_key_buf2dsa_raw(const unsigned char* key, size_t len);









int ldns_digest_evp(const unsigned char* data, unsigned int len,
unsigned char* dest, const EVP_MD* md);








EVP_PKEY* ldns_gost2pkey_raw(const unsigned char* key, size_t keylen);









EVP_PKEY* ldns_ecdsa2pkey_raw(const unsigned char* key, size_t keylen, uint8_t algo);








EVP_PKEY* ldns_ed255192pkey_raw(const unsigned char* key, size_t keylen);








EVP_PKEY* ldns_ed4482pkey_raw(const unsigned char* key, size_t keylen);

#endif

#if LDNS_BUILD_CONFIG_HAVE_SSL






RSA *ldns_key_buf2rsa(const ldns_buffer *key);







RSA *ldns_key_buf2rsa_raw(const unsigned char* key, size_t len);
#endif









ldns_rr *ldns_key_rr2ds(const ldns_rr *key, ldns_hash h);




ldns_rdf *
ldns_dnssec_create_nsec_bitmap(ldns_rr_type rr_type_list[],
size_t size,
ldns_rr_type nsec_type);








int
ldns_dnssec_rrsets_contains_type(const ldns_dnssec_rrsets *rrsets, ldns_rr_type type);




ldns_rr *
ldns_dnssec_create_nsec(const ldns_dnssec_name *from,
const ldns_dnssec_name *to,
ldns_rr_type nsec_type);





ldns_rr *
ldns_dnssec_create_nsec3(const ldns_dnssec_name *from,
const ldns_dnssec_name *to,
const ldns_rdf *zone_name,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
const uint8_t *salt);








ldns_rr * ldns_create_nsec(ldns_rdf *cur_owner, ldns_rdf *next_owner, ldns_rr_list *rrs);










ldns_rdf *ldns_nsec3_hash_name(const ldns_rdf *name, uint8_t algorithm, uint16_t iterations, uint8_t salt_length, const uint8_t *salt);











void ldns_nsec3_add_param_rdfs(ldns_rr *rr,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
const uint8_t *salt);



ldns_rr *
ldns_create_nsec3(const ldns_rdf *cur_owner,
const ldns_rdf *cur_zone,
const ldns_rr_list *rrs,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
const uint8_t *salt,
bool emptynonterminal);






uint8_t ldns_nsec3_algorithm(const ldns_rr *nsec3_rr);




uint8_t
ldns_nsec3_flags(const ldns_rr *nsec3_rr);






bool ldns_nsec3_optout(const ldns_rr *nsec3_rr);






uint16_t ldns_nsec3_iterations(const ldns_rr *nsec3_rr);






ldns_rdf *ldns_nsec3_salt(const ldns_rr *nsec3_rr);






uint8_t ldns_nsec3_salt_length(const ldns_rr *nsec3_rr);






uint8_t *ldns_nsec3_salt_data(const ldns_rr *nsec3_rr);






ldns_rdf *ldns_nsec3_next_owner(const ldns_rr *nsec3_rr);






ldns_rdf *ldns_nsec3_bitmap(const ldns_rr *nsec3_rr);







ldns_rdf *ldns_nsec3_hash_name_frm_nsec3(const ldns_rr *nsec, const ldns_rdf *name);







bool ldns_nsec_bitmap_covers_type(const ldns_rdf* bitmap, ldns_rr_type type);








ldns_status ldns_nsec_bitmap_set_type(ldns_rdf* bitmap, ldns_rr_type type);








ldns_status ldns_nsec_bitmap_clear_type(ldns_rdf* bitmap, ldns_rr_type type);











bool ldns_nsec_covers_name(const ldns_rr *nsec, const ldns_rdf *name);

#if LDNS_BUILD_CONFIG_HAVE_SSL











ldns_status ldns_pkt_verify(const ldns_pkt *p, ldns_rr_type t, const ldns_rdf *o, const ldns_rr_list *k, const ldns_rr_list *s, ldns_rr_list *good_keys);













ldns_status ldns_pkt_verify_time(const ldns_pkt *p, ldns_rr_type t, const ldns_rdf *o, const ldns_rr_list *k, const ldns_rr_list *s, time_t check_time, ldns_rr_list *good_keys);

#endif




ldns_status
ldns_dnssec_chain_nsec3_list(ldns_rr_list *nsec3_rrs);




int
qsort_rr_compare_nsec3(const void *a, const void *b);




void
ldns_rr_list_sort_nsec3(ldns_rr_list *unsorted);








int ldns_dnssec_default_add_to_signatures(ldns_rr *sig, void *n);







int ldns_dnssec_default_leave_signatures(ldns_rr *sig, void *n);







int ldns_dnssec_default_delete_signatures(ldns_rr *sig, void *n);







int ldns_dnssec_default_replace_signatures(ldns_rr *sig, void *n);

#if LDNS_BUILD_CONFIG_HAVE_SSL








ldns_rdf *
ldns_convert_dsa_rrsig_asn12rdf(const ldns_buffer *sig,
const long sig_len);









ldns_status
ldns_convert_dsa_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf);















ldns_rdf *
ldns_convert_ecdsa_rrsig_asn1len2rdf(const ldns_buffer *sig,
const long sig_len, int num_bytes);










ldns_status
ldns_convert_ecdsa_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf);










ldns_rdf *
ldns_convert_ed25519_rrsig_asn12rdf(const ldns_buffer *sig, long sig_len);










ldns_status
ldns_convert_ed25519_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf);










ldns_rdf *
ldns_convert_ed448_rrsig_asn12rdf(const ldns_buffer *sig, long sig_len);










ldns_status
ldns_convert_ed448_rrsig_rdf2asn1(ldns_buffer *target_buffer,
const ldns_rdf *sig_rdf);

#endif

#if defined(__cplusplus)
}
#endif

#endif
