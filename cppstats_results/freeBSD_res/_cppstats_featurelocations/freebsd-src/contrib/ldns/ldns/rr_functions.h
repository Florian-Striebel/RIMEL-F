











#if !defined(LDNS_RR_FUNCTIONS_H)
#define LDNS_RR_FUNCTIONS_H

#if defined(__cplusplus)
extern "C" {
#endif













ldns_rdf* ldns_rr_a_address(const ldns_rr *r);







bool ldns_rr_a_set_address(ldns_rr *r, ldns_rdf *f);







ldns_rdf* ldns_rr_ns_nsdname(const ldns_rr *r);







ldns_rdf* ldns_rr_mx_preference(const ldns_rr *r);





ldns_rdf* ldns_rr_mx_exchange(const ldns_rr *r);







ldns_rdf* ldns_rr_rrsig_typecovered(const ldns_rr *r);






bool ldns_rr_rrsig_set_typecovered(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_rrsig_algorithm(const ldns_rr *r);






bool ldns_rr_rrsig_set_algorithm(ldns_rr *r, ldns_rdf *f);





ldns_rdf *ldns_rr_rrsig_labels(const ldns_rr *r);






bool ldns_rr_rrsig_set_labels(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_rrsig_origttl(const ldns_rr *r);






bool ldns_rr_rrsig_set_origttl(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_rrsig_expiration(const ldns_rr *r);






bool ldns_rr_rrsig_set_expiration(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_rrsig_inception(const ldns_rr *r);






bool ldns_rr_rrsig_set_inception(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_rrsig_keytag(const ldns_rr *r);






bool ldns_rr_rrsig_set_keytag(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_rrsig_signame(const ldns_rr *r);






bool ldns_rr_rrsig_set_signame(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_rrsig_sig(const ldns_rr *r);






bool ldns_rr_rrsig_set_sig(ldns_rr *r, ldns_rdf *f);







ldns_rdf* ldns_rr_dnskey_flags(const ldns_rr *r);






bool ldns_rr_dnskey_set_flags(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_dnskey_protocol(const ldns_rr *r);






bool ldns_rr_dnskey_set_protocol(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_dnskey_algorithm(const ldns_rr *r);






bool ldns_rr_dnskey_set_algorithm(ldns_rr *r, ldns_rdf *f);





ldns_rdf* ldns_rr_dnskey_key(const ldns_rr *r);






bool ldns_rr_dnskey_set_key(ldns_rr *r, ldns_rdf *f);








size_t ldns_rr_dnskey_key_size_raw(const unsigned char *keydata,
const size_t len,
const ldns_algorithm alg);






size_t ldns_rr_dnskey_key_size(const ldns_rr *key);












typedef uint32_t (*ldns_soa_serial_increment_func_t)(uint32_t, void*);







uint32_t ldns_soa_serial_identity(uint32_t unused, void *data);







uint32_t ldns_soa_serial_increment(uint32_t s, void *unused);







uint32_t ldns_soa_serial_increment_by(uint32_t s, void *data);












uint32_t ldns_soa_serial_unixtime(uint32_t s, void *data);












uint32_t ldns_soa_serial_datecounter(uint32_t s, void *data);





void ldns_rr_soa_increment(
ldns_rr *soa);








void ldns_rr_soa_increment_func(
ldns_rr *soa, ldns_soa_serial_increment_func_t f);








void ldns_rr_soa_increment_func_data(
ldns_rr *soa, ldns_soa_serial_increment_func_t f, void *data);












void ldns_rr_soa_increment_func_int(
ldns_rr *soa, ldns_soa_serial_increment_func_t f, int data);

#if defined(__cplusplus)
}
#endif

#endif
