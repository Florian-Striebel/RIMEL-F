



















#if !defined(LDNS_RDATA_H)
#define LDNS_RDATA_H

#include <ldns/common.h>
#include <ldns/error.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LDNS_MAX_RDFLEN 65535

#define LDNS_RDF_SIZE_BYTE 1
#define LDNS_RDF_SIZE_WORD 2
#define LDNS_RDF_SIZE_DOUBLEWORD 4
#define LDNS_RDF_SIZE_6BYTES 6
#define LDNS_RDF_SIZE_8BYTES 8
#define LDNS_RDF_SIZE_16BYTES 16

#define LDNS_NSEC3_VARS_OPTOUT_MASK 0x01




enum ldns_enum_rdf_type
{

LDNS_RDF_TYPE_NONE,

LDNS_RDF_TYPE_DNAME,

LDNS_RDF_TYPE_INT8,

LDNS_RDF_TYPE_INT16,

LDNS_RDF_TYPE_INT32,

LDNS_RDF_TYPE_A,

LDNS_RDF_TYPE_AAAA,

LDNS_RDF_TYPE_STR,

LDNS_RDF_TYPE_APL,

LDNS_RDF_TYPE_B32_EXT,

LDNS_RDF_TYPE_B64,

LDNS_RDF_TYPE_HEX,

LDNS_RDF_TYPE_NSEC,

LDNS_RDF_TYPE_TYPE,

LDNS_RDF_TYPE_CLASS,

LDNS_RDF_TYPE_CERT_ALG,

LDNS_RDF_TYPE_ALG,

LDNS_RDF_TYPE_UNKNOWN,

LDNS_RDF_TYPE_TIME,

LDNS_RDF_TYPE_PERIOD,

LDNS_RDF_TYPE_TSIGTIME,



LDNS_RDF_TYPE_HIP,


LDNS_RDF_TYPE_INT16_DATA,

LDNS_RDF_TYPE_SERVICE,

LDNS_RDF_TYPE_LOC,

LDNS_RDF_TYPE_WKS,

LDNS_RDF_TYPE_NSAP,

LDNS_RDF_TYPE_ATMA,

LDNS_RDF_TYPE_IPSECKEY,

LDNS_RDF_TYPE_NSEC3_SALT,

LDNS_RDF_TYPE_NSEC3_NEXT_OWNER,




LDNS_RDF_TYPE_ILNP64,


LDNS_RDF_TYPE_EUI48,

LDNS_RDF_TYPE_EUI64,




LDNS_RDF_TYPE_TAG,





LDNS_RDF_TYPE_LONG_STR,





LDNS_RDF_TYPE_CERTIFICATE_USAGE,
LDNS_RDF_TYPE_SELECTOR,
LDNS_RDF_TYPE_MATCHING_TYPE,


LDNS_RDF_TYPE_BITMAP = LDNS_RDF_TYPE_NSEC
};
typedef enum ldns_enum_rdf_type ldns_rdf_type;




enum ldns_enum_cert_algorithm
{
LDNS_CERT_PKIX = 1,
LDNS_CERT_SPKI = 2,
LDNS_CERT_PGP = 3,
LDNS_CERT_IPKIX = 4,
LDNS_CERT_ISPKI = 5,
LDNS_CERT_IPGP = 6,
LDNS_CERT_ACPKIX = 7,
LDNS_CERT_IACPKIX = 8,
LDNS_CERT_URI = 253,
LDNS_CERT_OID = 254
};
typedef enum ldns_enum_cert_algorithm ldns_cert_algorithm;










struct ldns_struct_rdf
{

size_t _size;

ldns_rdf_type _type;

void *_data;
};
typedef struct ldns_struct_rdf ldns_rdf;











void ldns_rdf_set_size(ldns_rdf *rd, size_t size);







void ldns_rdf_set_type(ldns_rdf *rd, ldns_rdf_type type);







void ldns_rdf_set_data(ldns_rdf *rd, void *data);








size_t ldns_rdf_size(const ldns_rdf *rd);







ldns_rdf_type ldns_rdf_get_type(const ldns_rdf *rd);







uint8_t *ldns_rdf_data(const ldns_rdf *rd);












ldns_rdf *ldns_rdf_new(ldns_rdf_type type, size_t size, void *data);










ldns_rdf *ldns_rdf_new_frm_data(ldns_rdf_type type, size_t size, const void *data);







ldns_rdf *ldns_rdf_new_frm_str(ldns_rdf_type type, const char *str);








ldns_status ldns_rdf_new_frm_fp(ldns_rdf **r, ldns_rdf_type type, FILE *fp);









ldns_status ldns_rdf_new_frm_fp_l(ldns_rdf **r, ldns_rdf_type type, FILE *fp, int *line_nr);









void ldns_rdf_free(ldns_rdf *rd);







void ldns_rdf_deep_free(ldns_rdf *rd);









ldns_rdf *ldns_native2rdf_int8(ldns_rdf_type type, uint8_t value);







ldns_rdf *ldns_native2rdf_int16(ldns_rdf_type type, uint16_t value);










ldns_rdf *ldns_native2rdf_int32(ldns_rdf_type type, uint32_t value);











ldns_rdf *ldns_native2rdf_int16_data(size_t size, uint8_t *data);







ldns_rdf *ldns_rdf_address_reverse(const ldns_rdf *rd);






uint8_t ldns_rdf2native_int8(const ldns_rdf *rd);






uint16_t ldns_rdf2native_int16(const ldns_rdf *rd);






uint32_t ldns_rdf2native_int32(const ldns_rdf *rd);






time_t ldns_rdf2native_time_t(const ldns_rdf *rd);







uint32_t ldns_str2period(const char *nptr, const char **endptr);








ldns_status ldns_octet(char *word, size_t *length);






ldns_rdf *ldns_rdf_clone(const ldns_rdf *rd);










int ldns_rdf_compare(const ldns_rdf *rd1, const ldns_rdf *rd2);












ldns_status ldns_rdf_hip_get_alg_hit_pk(ldns_rdf *rdf, uint8_t* alg,
uint8_t *hit_size, uint8_t** hit,
uint16_t *pk_size, uint8_t** pk);











ldns_status ldns_rdf_hip_new_frm_alg_hit_pk(ldns_rdf** rdf, uint8_t alg,
uint8_t hit_size, uint8_t *hit, uint16_t pk_size, uint8_t *pk);

#if defined(__cplusplus)
}
#endif

#endif
