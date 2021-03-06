

#if !defined(LDNS_DNSSEC_SIGN_H)
#define LDNS_DNSSEC_SIGN_H

#include <ldns/dnssec.h>

#if defined(__cplusplus)
extern "C" {
#endif




#define LDNS_SIGN_DNSKEY_WITH_ZSK 1
#define LDNS_SIGN_WITH_ALL_ALGORITHMS 2







ldns_rr *
ldns_create_empty_rrsig(const ldns_rr_list *rrset,
const ldns_key *key);








ldns_rdf *
ldns_sign_public_buffer(ldns_buffer *sign_buf, ldns_key *key);







ldns_rr_list *ldns_sign_public(ldns_rr_list *rrset, ldns_key_list *keys);

#if LDNS_BUILD_CONFIG_HAVE_SSL






ldns_rdf *ldns_sign_public_dsa(ldns_buffer *to_sign, DSA *key);











ldns_rdf *ldns_sign_public_evp(ldns_buffer *to_sign,
EVP_PKEY *key,
const EVP_MD *digest_type);







ldns_rdf *ldns_sign_public_rsasha1(ldns_buffer *to_sign, RSA *key);







ldns_rdf *ldns_sign_public_rsamd5(ldns_buffer *to_sign, RSA *key);
#endif















ldns_status
ldns_dnssec_zone_mark_and_get_glue(
ldns_dnssec_zone *zone, ldns_rr_list *glue_list);











ldns_status
ldns_dnssec_zone_mark_glue(ldns_dnssec_zone *zone);









ldns_rbnode_t *ldns_dnssec_name_node_next_nonglue(ldns_rbnode_t *node);









ldns_status ldns_dnssec_zone_create_nsecs(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs);




ldns_status
ldns_dnssec_zone_create_nsec3s(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
uint8_t *salt);















ldns_dnssec_rrs *ldns_dnssec_remove_signatures(ldns_dnssec_rrs *signatures,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void *),
void *arg);



















ldns_status ldns_dnssec_zone_create_rrsigs_flg(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void*),
void *arg,
int flags);














ldns_status ldns_dnssec_zone_create_rrsigs(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void*),
void *arg);


























ldns_status ldns_dnssec_zone_sign_flg(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void *),
void *arg,
int flags);

















ldns_status ldns_dnssec_zone_sign_nsec3_flg(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void *),
void *arg,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
uint8_t *salt,
int signflags);




















ldns_status ldns_dnssec_zone_sign_nsec3_flg_mkmap(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void *),
void *arg,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
uint8_t *salt,
int signflags,
ldns_rbtree_t **map
);






















ldns_status ldns_dnssec_zone_sign(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void *),
void *arg);
















ldns_status ldns_dnssec_zone_sign_nsec3(ldns_dnssec_zone *zone,
ldns_rr_list *new_rrs,
ldns_key_list *key_list,
int (*func)(ldns_rr *, void *),
void *arg,
uint8_t algorithm,
uint8_t flags,
uint16_t iterations,
uint8_t salt_length,
uint8_t *salt);







ldns_zone *ldns_zone_sign(const ldns_zone *zone, ldns_key_list *key_list);












ldns_zone *ldns_zone_sign_nsec3(ldns_zone *zone, ldns_key_list *key_list, uint8_t algorithm, uint8_t flags, uint16_t iterations, uint8_t salt_length, uint8_t *salt);

#if defined(__cplusplus)
}
#endif

#endif
