

#if !defined(LDNS_DNSSEC_VERIFY_H)
#define LDNS_DNSSEC_VERIFY_H

#define LDNS_DNSSEC_TRUST_TREE_MAX_PARENTS 10

#include <ldns/dnssec.h>
#include <ldns/host2str.h>

#if defined(__cplusplus)
extern "C" {
#endif





typedef struct ldns_dnssec_data_chain_struct ldns_dnssec_data_chain;
struct ldns_dnssec_data_chain_struct
{
ldns_rr_list *rrset;
ldns_rr_list *signatures;
ldns_rr_type parent_type;
ldns_dnssec_data_chain *parent;
ldns_pkt_rcode packet_rcode;
ldns_rr_type packet_qtype;
bool packet_nodata;
};





ldns_dnssec_data_chain *ldns_dnssec_data_chain_new(void);






void ldns_dnssec_data_chain_free(ldns_dnssec_data_chain *chain);







void ldns_dnssec_data_chain_deep_free(ldns_dnssec_data_chain *chain);







void ldns_dnssec_data_chain_print(FILE *out, const ldns_dnssec_data_chain *chain);








void ldns_dnssec_data_chain_print_fmt(FILE *out,
const ldns_output_format *fmt,
const ldns_dnssec_data_chain *chain);
















ldns_dnssec_data_chain *ldns_dnssec_build_data_chain(ldns_resolver *res,
const uint16_t qflags,
const ldns_rr_list *data_set,
const ldns_pkt *pkt,
ldns_rr *orig_rr);






























typedef struct ldns_dnssec_trust_tree_struct ldns_dnssec_trust_tree;
struct ldns_dnssec_trust_tree_struct
{
ldns_rr *rr;

ldns_rr_list *rrset;
ldns_dnssec_trust_tree *parents[LDNS_DNSSEC_TRUST_TREE_MAX_PARENTS];
ldns_status parent_status[LDNS_DNSSEC_TRUST_TREE_MAX_PARENTS];


ldns_rr *parent_signature[LDNS_DNSSEC_TRUST_TREE_MAX_PARENTS];
size_t parent_count;
};






ldns_dnssec_trust_tree *ldns_dnssec_trust_tree_new(void);









void ldns_dnssec_trust_tree_free(ldns_dnssec_trust_tree *tree);







size_t ldns_dnssec_trust_tree_depth(ldns_dnssec_trust_tree *tree);













void ldns_dnssec_trust_tree_print(FILE *out,
ldns_dnssec_trust_tree *tree,
size_t tabs,
bool extended);














void ldns_dnssec_trust_tree_print_fmt(FILE *out,
const ldns_output_format *fmt,
ldns_dnssec_trust_tree *tree,
size_t tabs,
bool extended);











ldns_status ldns_dnssec_trust_tree_add_parent(ldns_dnssec_trust_tree *tree,
const ldns_dnssec_trust_tree *parent,
const ldns_rr *parent_signature,
const ldns_status parent_status);












ldns_dnssec_trust_tree *ldns_dnssec_derive_trust_tree(
ldns_dnssec_data_chain *data_chain,
ldns_rr *rr);













ldns_dnssec_trust_tree *ldns_dnssec_derive_trust_tree_time(
ldns_dnssec_data_chain *data_chain,
ldns_rr *rr, time_t check_time);








void ldns_dnssec_derive_trust_tree_normal_rrset(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain,
ldns_rr *cur_sig_rr);









void ldns_dnssec_derive_trust_tree_normal_rrset_time(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain,
ldns_rr *cur_sig_rr, time_t check_time);










void ldns_dnssec_derive_trust_tree_dnskey_rrset(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain,
ldns_rr *cur_rr,
ldns_rr *cur_sig_rr);










void ldns_dnssec_derive_trust_tree_dnskey_rrset_time(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain,
ldns_rr *cur_rr, ldns_rr *cur_sig_rr,
time_t check_time);








void ldns_dnssec_derive_trust_tree_ds_rrset(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain,
ldns_rr *cur_rr);









void ldns_dnssec_derive_trust_tree_ds_rrset_time(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain,
ldns_rr *cur_rr, time_t check_time);








void ldns_dnssec_derive_trust_tree_no_sig(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain);









void ldns_dnssec_derive_trust_tree_no_sig_time(
ldns_dnssec_trust_tree *new_tree,
ldns_dnssec_data_chain *data_chain,
time_t check_time);













ldns_status ldns_dnssec_trust_tree_contains_keys(
ldns_dnssec_trust_tree *tree,
ldns_rr_list *keys);












ldns_status ldns_verify(ldns_rr_list *rrset,
ldns_rr_list *rrsig,
const ldns_rr_list *keys,
ldns_rr_list *good_keys);













ldns_status ldns_verify_time(const ldns_rr_list *rrset,
const ldns_rr_list *rrsig,
const ldns_rr_list *keys,
time_t check_time,
ldns_rr_list *good_keys);














ldns_status ldns_verify_notime(ldns_rr_list *rrset,
ldns_rr_list *rrsig,
const ldns_rr_list *keys,
ldns_rr_list *good_keys);















ldns_rr_list *ldns_fetch_valid_domain_keys(const ldns_resolver * res,
const ldns_rdf * domain,
const ldns_rr_list * keys,
ldns_status *status);
















ldns_rr_list *ldns_fetch_valid_domain_keys_time(const ldns_resolver * res,
const ldns_rdf * domain, const ldns_rr_list * keys,
time_t check_time, ldns_status *status);












ldns_rr_list *ldns_validate_domain_dnskey (const ldns_resolver *res,
const ldns_rdf *domain,
const ldns_rr_list *keys);












ldns_rr_list *ldns_validate_domain_dnskey_time(
const ldns_resolver *res, const ldns_rdf *domain,
const ldns_rr_list *keys, time_t check_time);










ldns_rr_list *ldns_validate_domain_ds(const ldns_resolver *res,
const ldns_rdf *
domain,
const ldns_rr_list * keys);










ldns_rr_list *ldns_validate_domain_ds_time(
const ldns_resolver *res, const ldns_rdf *domain,
const ldns_rr_list * keys, time_t check_time);













ldns_status ldns_verify_trusted(ldns_resolver *res,
ldns_rr_list *rrset,
ldns_rr_list *rrsigs,
ldns_rr_list *validating_keys);













ldns_status ldns_verify_trusted_time(
ldns_resolver *res, ldns_rr_list *rrset,
ldns_rr_list *rrsigs, time_t check_time,
ldns_rr_list *validating_keys);












ldns_status ldns_dnssec_verify_denial(ldns_rr *rr,
ldns_rr_list *nsecs,
ldns_rr_list *rrsigs);


















ldns_status ldns_dnssec_verify_denial_nsec3(ldns_rr *rr,
ldns_rr_list *nsecs,
ldns_rr_list *rrsigs,
ldns_pkt_rcode packet_rcode,
ldns_rr_type packet_qtype,
bool packet_nodata);



















ldns_status ldns_dnssec_verify_denial_nsec3_match(ldns_rr *rr,
ldns_rr_list *nsecs,
ldns_rr_list *rrsigs,
ldns_pkt_rcode packet_rcode,
ldns_rr_type packet_qtype,
bool packet_nodata,
ldns_rr **match);










ldns_status ldns_verify_rrsig_buffers(ldns_buffer *rawsig_buf,
ldns_buffer *verify_buf,
ldns_buffer *key_buf,
uint8_t algo);












ldns_status ldns_verify_rrsig_buffers_raw(unsigned char* sig,
size_t siglen,
ldns_buffer *verify_buf,
unsigned char* key,
size_t keylen,
uint8_t algo);












ldns_status ldns_verify_rrsig_keylist(ldns_rr_list *rrset,
ldns_rr *rrsig,
const ldns_rr_list *keys,
ldns_rr_list *good_keys);













ldns_status ldns_verify_rrsig_keylist_time(
const ldns_rr_list *rrset, const ldns_rr *rrsig,
const ldns_rr_list *keys, time_t check_time,
ldns_rr_list *good_keys);













ldns_status ldns_verify_rrsig_keylist_notime(const ldns_rr_list *rrset,
const ldns_rr *rrsig,
const ldns_rr_list *keys,
ldns_rr_list *good_keys);








ldns_status ldns_verify_rrsig(ldns_rr_list *rrset,
ldns_rr *rrsig,
ldns_rr *key);










ldns_status ldns_verify_rrsig_time(
ldns_rr_list *rrset, ldns_rr *rrsig,
ldns_rr *key, time_t check_time);


#if LDNS_BUILD_CONFIG_HAVE_SSL









ldns_status ldns_verify_rrsig_evp(ldns_buffer *sig,
ldns_buffer *rrset,
EVP_PKEY *key,
const EVP_MD *digest_type);









ldns_status ldns_verify_rrsig_evp_raw(const unsigned char *sig,
size_t siglen,
const ldns_buffer *rrset,
EVP_PKEY *key,
const EVP_MD *digest_type);
#endif









ldns_status ldns_verify_rrsig_dsa(ldns_buffer *sig,
ldns_buffer *rrset,
ldns_buffer *key);









ldns_status ldns_verify_rrsig_rsasha1(ldns_buffer *sig,
ldns_buffer *rrset,
ldns_buffer *key);









ldns_status ldns_verify_rrsig_rsamd5(ldns_buffer *sig,
ldns_buffer *rrset,
ldns_buffer *key);









ldns_status ldns_verify_rrsig_dsa_raw(unsigned char* sig,
size_t siglen,
ldns_buffer* rrset,
unsigned char* key,
size_t keylen);









ldns_status ldns_verify_rrsig_rsasha1_raw(unsigned char* sig,
size_t siglen,
ldns_buffer* rrset,
unsigned char* key,
size_t keylen);










ldns_status ldns_verify_rrsig_rsasha256_raw(unsigned char* sig,
size_t siglen,
ldns_buffer* rrset,
unsigned char* key,
size_t keylen);









ldns_status ldns_verify_rrsig_rsasha512_raw(unsigned char* sig,
size_t siglen,
ldns_buffer* rrset,
unsigned char* key,
size_t keylen);









ldns_status ldns_verify_rrsig_rsamd5_raw(unsigned char* sig,
size_t siglen,
ldns_buffer* rrset,
unsigned char* key,
size_t keylen);

#if defined(__cplusplus)
}
#endif

#endif

