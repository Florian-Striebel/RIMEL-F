



















#if !defined(LDNS_KEYS_H)
#define LDNS_KEYS_H

#include <ldns/common.h>
#if LDNS_BUILD_CONFIG_HAVE_SSL
#include <openssl/ssl.h>
#endif
#include <ldns/util.h>
#include <errno.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern ldns_lookup_table ldns_signing_algorithms[];

#define LDNS_KEY_ZONE_KEY 0x0100
#define LDNS_KEY_SEP_KEY 0x0001
#define LDNS_KEY_REVOKE_KEY 0x0080




enum ldns_enum_algorithm
{
LDNS_RSAMD5 = 1,
LDNS_DH = 2,
LDNS_DSA = 3,
LDNS_ECC = 4,
LDNS_RSASHA1 = 5,
LDNS_DSA_NSEC3 = 6,
LDNS_RSASHA1_NSEC3 = 7,
LDNS_RSASHA256 = 8,
LDNS_RSASHA512 = 10,
LDNS_ECC_GOST = 12,
LDNS_ECDSAP256SHA256 = 13,
LDNS_ECDSAP384SHA384 = 14,
#if defined(USE_ED25519)




LDNS_ED25519 = 15,
#endif
#if defined(USE_ED448)
LDNS_ED448 = 16,
#endif
LDNS_INDIRECT = 252,
LDNS_PRIVATEDNS = 253,
LDNS_PRIVATEOID = 254
};
typedef enum ldns_enum_algorithm ldns_algorithm;




enum ldns_enum_hash
{
LDNS_SHA1 = 1,
LDNS_SHA256 = 2,
LDNS_HASH_GOST = 3,
LDNS_SHA384 = 4
};
typedef enum ldns_enum_hash ldns_hash;




enum ldns_enum_signing_algorithm
{
LDNS_SIGN_RSAMD5 = LDNS_RSAMD5,
LDNS_SIGN_RSASHA1 = LDNS_RSASHA1,
LDNS_SIGN_DSA = LDNS_DSA,
LDNS_SIGN_RSASHA1_NSEC3 = LDNS_RSASHA1_NSEC3,
LDNS_SIGN_RSASHA256 = LDNS_RSASHA256,
LDNS_SIGN_RSASHA512 = LDNS_RSASHA512,
LDNS_SIGN_DSA_NSEC3 = LDNS_DSA_NSEC3,
LDNS_SIGN_ECC_GOST = LDNS_ECC_GOST,
LDNS_SIGN_ECDSAP256SHA256 = LDNS_ECDSAP256SHA256,
LDNS_SIGN_ECDSAP384SHA384 = LDNS_ECDSAP384SHA384,
#if defined(USE_ED25519)
LDNS_SIGN_ED25519 = LDNS_ED25519,
#endif
#if defined(USE_ED448)
LDNS_SIGN_ED448 = LDNS_ED448,
#endif
LDNS_SIGN_HMACMD5 = 157,
LDNS_SIGN_HMACSHA1 = 158,
LDNS_SIGN_HMACSHA256 = 159,
LDNS_SIGN_HMACSHA224 = 162,
LDNS_SIGN_HMACSHA384 = 164,
LDNS_SIGN_HMACSHA512 = 165
};
typedef enum ldns_enum_signing_algorithm ldns_signing_algorithm;











struct ldns_struct_key {
ldns_signing_algorithm _alg;

bool _use;


struct {
#if LDNS_BUILD_CONFIG_HAVE_SSL
#if !defined(S_SPLINT_S)


EVP_PKEY *key;
#endif
#endif



struct {
unsigned char *key;
size_t size;
} hmac;



void *external_key;
} _key;

union {

struct {

uint32_t orig_ttl;

uint32_t inception;

uint32_t expiration;

uint16_t keytag;

uint16_t flags;
} dnssec;
} _extra;

ldns_rdf *_pubkey_owner;
};
typedef struct ldns_struct_key ldns_key;




struct ldns_struct_key_list
{
size_t _key_count;
ldns_key **_keys;
};
typedef struct ldns_struct_key_list ldns_key_list;






ldns_key_list *ldns_key_list_new(void);





ldns_key *ldns_key_new(void);








ldns_key *ldns_key_new_frm_algorithm(ldns_signing_algorithm a, uint16_t size);











ldns_status ldns_key_new_frm_fp(ldns_key **k, FILE *fp);












ldns_status ldns_key_new_frm_fp_l(ldns_key **k, FILE *fp, int *line_nr);

#if LDNS_BUILD_CONFIG_HAVE_SSL




ldns_status ldns_key_new_frm_engine(ldns_key **key, ENGINE *e, char *key_id, ldns_algorithm);








RSA *ldns_key_new_frm_fp_rsa(FILE *fp);
#endif

#if LDNS_BUILD_CONFIG_HAVE_SSL







RSA *ldns_key_new_frm_fp_rsa_l(FILE *fp, int *line_nr);
#endif

#if LDNS_BUILD_CONFIG_HAVE_SSL






DSA *ldns_key_new_frm_fp_dsa(FILE *fp);
#endif

#if LDNS_BUILD_CONFIG_HAVE_SSL







DSA *ldns_key_new_frm_fp_dsa_l(FILE *fp, int *line_nr);
#endif

#if LDNS_BUILD_CONFIG_HAVE_SSL








unsigned char *ldns_key_new_frm_fp_hmac(FILE *fp, size_t *hmac_size);
#endif

#if LDNS_BUILD_CONFIG_HAVE_SSL









unsigned char *ldns_key_new_frm_fp_hmac_l(FILE *fp, int *line_nr, size_t *hmac_size);
#endif







void ldns_key_set_algorithm(ldns_key *k, ldns_signing_algorithm l);
#if LDNS_BUILD_CONFIG_HAVE_SSL





void ldns_key_set_evp_key(ldns_key *k, EVP_PKEY *e);







void ldns_key_set_rsa_key(ldns_key *k, RSA *r);







void ldns_key_set_dsa_key(ldns_key *k, DSA *d);







void ldns_key_assign_rsa_key(ldns_key *k, RSA *r);







void ldns_key_assign_dsa_key(ldns_key *k, DSA *d);






int ldns_key_EVP_load_gost_id(void);


void ldns_key_EVP_unload_gost(void);
#endif






void ldns_key_set_hmac_key(ldns_key *k, unsigned char *hmac);











void ldns_key_set_external_key(ldns_key *key, void *external_key);






void ldns_key_set_hmac_size(ldns_key *k, size_t hmac_size);





void ldns_key_set_origttl(ldns_key *k, uint32_t t);





void ldns_key_set_inception(ldns_key *k, uint32_t i);





void ldns_key_set_expiration(ldns_key *k, uint32_t e);





void ldns_key_set_pubkey_owner(ldns_key *k, ldns_rdf *r);





void ldns_key_set_keytag(ldns_key *k, uint16_t tag);





void ldns_key_set_flags(ldns_key *k, uint16_t flags);





void ldns_key_list_set_key_count(ldns_key_list *key, size_t count);







bool ldns_key_list_push_key(ldns_key_list *key_list, ldns_key *key);






size_t ldns_key_list_key_count(const ldns_key_list *key_list);







ldns_key *ldns_key_list_key(const ldns_key_list *key, size_t nr);

#if LDNS_BUILD_CONFIG_HAVE_SSL





RSA *ldns_key_rsa_key(const ldns_key *k);





EVP_PKEY *ldns_key_evp_key(const ldns_key *k);
#endif




#if LDNS_BUILD_CONFIG_HAVE_SSL
DSA *ldns_key_dsa_key(const ldns_key *k);
#endif






ldns_signing_algorithm ldns_key_algorithm(const ldns_key *k);





void ldns_key_set_use(ldns_key *k, bool v);





bool ldns_key_use(const ldns_key *k);





unsigned char *ldns_key_hmac_key(const ldns_key *k);





void *ldns_key_external_key(const ldns_key *k);





size_t ldns_key_hmac_size(const ldns_key *k);





uint32_t ldns_key_origttl(const ldns_key *k);





uint32_t ldns_key_inception(const ldns_key *k);





uint32_t ldns_key_expiration(const ldns_key *k);





uint16_t ldns_key_keytag(const ldns_key *k);





ldns_rdf *ldns_key_pubkey_owner(const ldns_key *k);





void
ldns_key_list_set_use(ldns_key_list *keys, bool v);






uint16_t ldns_key_flags(const ldns_key *k);






ldns_key *ldns_key_list_pop_key(ldns_key_list *key_list);










ldns_rr *ldns_key2rr(const ldns_key *k);







void ldns_key_print(FILE *output, const ldns_key *k);






void ldns_key_free(ldns_key *key);







void ldns_key_deep_free(ldns_key *key);





void ldns_key_list_free(ldns_key_list *key_list);






ldns_rr * ldns_read_anchor_file(const char *filename);











char *ldns_key_get_file_base_name(const ldns_key *key);






int ldns_key_algo_supported(int algo);






ldns_signing_algorithm ldns_get_signing_algorithm_by_name(const char* name);

#if defined(__cplusplus)
}
#endif

#endif
