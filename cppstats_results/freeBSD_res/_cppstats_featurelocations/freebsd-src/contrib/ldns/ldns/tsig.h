







#if !defined(LDNS_TSIG_H)
#define LDNS_TSIG_H

#if defined(__cplusplus)
extern "C" {
#endif











typedef struct ldns_tsig_credentials_struct
{
const char *algorithm;
const char *keyname;
const char *keydata;

} ldns_tsig_credentials;

const char *ldns_tsig_algorithm(const ldns_tsig_credentials *);
const char *ldns_tsig_keyname(const ldns_tsig_credentials *);
const char *ldns_tsig_keydata(const ldns_tsig_credentials *);
char *ldns_tsig_keyname_clone(const ldns_tsig_credentials *);
char *ldns_tsig_keydata_clone(const ldns_tsig_credentials *);












bool ldns_pkt_tsig_verify(ldns_pkt *pkt, const uint8_t *wire, size_t wire_size, const char *key_name, const char *key_data, const ldns_rdf *mac);














bool ldns_pkt_tsig_verify_next(ldns_pkt *pkt, const uint8_t *wire, size_t wire_size, const char *key_name, const char *key_data, const ldns_rdf *mac,
int tsig_timers_only);











ldns_status ldns_pkt_tsig_sign(ldns_pkt *pkt, const char *key_name, const char *key_data, uint16_t fudge,
const char *algorithm_name, const ldns_rdf *query_mac);













ldns_status ldns_pkt_tsig_sign_next(ldns_pkt *pkt, const char *key_name, const char *key_data, uint16_t fudge,
const char *algorithm_name, const ldns_rdf *query_mac, int tsig_timers_only);

#if defined(__cplusplus)
}
#endif

#endif
