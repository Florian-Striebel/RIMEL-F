
















#if !defined(LDNS_RR_H)
#define LDNS_RR_H

#include <ldns/common.h>
#include <ldns/rdata.h>
#include <ldns/buffer.h>
#include <ldns/error.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define LDNS_MAX_LABELLEN 63

#define LDNS_MAX_DOMAINLEN 255

#define LDNS_MAX_POINTERS 65535

#define LDNS_RR_OVERHEAD 10


#define LDNS_RDATA_FIELD_DESCRIPTORS_COMMON 259






enum ldns_enum_rr_class
{

LDNS_RR_CLASS_IN = 1,

LDNS_RR_CLASS_CH = 3,

LDNS_RR_CLASS_HS = 4,

LDNS_RR_CLASS_NONE = 254,

LDNS_RR_CLASS_ANY = 255,

LDNS_RR_CLASS_FIRST = 0,
LDNS_RR_CLASS_LAST = 65535,
LDNS_RR_CLASS_COUNT = LDNS_RR_CLASS_LAST - LDNS_RR_CLASS_FIRST + 1
};
typedef enum ldns_enum_rr_class ldns_rr_class;




enum ldns_enum_rr_compress
{

LDNS_RR_COMPRESS,
LDNS_RR_NO_COMPRESS
};
typedef enum ldns_enum_rr_compress ldns_rr_compress;




enum ldns_enum_rr_type
{

LDNS_RR_TYPE_A = 1,

LDNS_RR_TYPE_NS = 2,

LDNS_RR_TYPE_MD = 3,

LDNS_RR_TYPE_MF = 4,

LDNS_RR_TYPE_CNAME = 5,

LDNS_RR_TYPE_SOA = 6,

LDNS_RR_TYPE_MB = 7,

LDNS_RR_TYPE_MG = 8,

LDNS_RR_TYPE_MR = 9,

LDNS_RR_TYPE_NULL = 10,

LDNS_RR_TYPE_WKS = 11,

LDNS_RR_TYPE_PTR = 12,

LDNS_RR_TYPE_HINFO = 13,

LDNS_RR_TYPE_MINFO = 14,

LDNS_RR_TYPE_MX = 15,

LDNS_RR_TYPE_TXT = 16,

LDNS_RR_TYPE_RP = 17,

LDNS_RR_TYPE_AFSDB = 18,

LDNS_RR_TYPE_X25 = 19,

LDNS_RR_TYPE_ISDN = 20,

LDNS_RR_TYPE_RT = 21,

LDNS_RR_TYPE_NSAP = 22,

LDNS_RR_TYPE_NSAP_PTR = 23,

LDNS_RR_TYPE_SIG = 24,

LDNS_RR_TYPE_KEY = 25,

LDNS_RR_TYPE_PX = 26,

LDNS_RR_TYPE_GPOS = 27,

LDNS_RR_TYPE_AAAA = 28,

LDNS_RR_TYPE_LOC = 29,

LDNS_RR_TYPE_NXT = 30,

LDNS_RR_TYPE_EID = 31,

LDNS_RR_TYPE_NIMLOC = 32,

LDNS_RR_TYPE_SRV = 33,

LDNS_RR_TYPE_ATMA = 34,

LDNS_RR_TYPE_NAPTR = 35,

LDNS_RR_TYPE_KX = 36,

LDNS_RR_TYPE_CERT = 37,

LDNS_RR_TYPE_A6 = 38,

LDNS_RR_TYPE_DNAME = 39,

LDNS_RR_TYPE_SINK = 40,

LDNS_RR_TYPE_OPT = 41,

LDNS_RR_TYPE_APL = 42,

LDNS_RR_TYPE_DS = 43,

LDNS_RR_TYPE_SSHFP = 44,

LDNS_RR_TYPE_IPSECKEY = 45,

LDNS_RR_TYPE_RRSIG = 46,
LDNS_RR_TYPE_NSEC = 47,
LDNS_RR_TYPE_DNSKEY = 48,

LDNS_RR_TYPE_DHCID = 49,

LDNS_RR_TYPE_NSEC3 = 50,
LDNS_RR_TYPE_NSEC3PARAM = 51,
LDNS_RR_TYPE_NSEC3PARAMS = 51,
LDNS_RR_TYPE_TLSA = 52,
LDNS_RR_TYPE_SMIMEA = 53,

LDNS_RR_TYPE_HIP = 55,


LDNS_RR_TYPE_NINFO = 56,

LDNS_RR_TYPE_RKEY = 57,

LDNS_RR_TYPE_TALINK = 58,
LDNS_RR_TYPE_CDS = 59,
LDNS_RR_TYPE_CDNSKEY = 60,
LDNS_RR_TYPE_OPENPGPKEY = 61,
LDNS_RR_TYPE_CSYNC = 62,

LDNS_RR_TYPE_SPF = 99,

LDNS_RR_TYPE_UINFO = 100,
LDNS_RR_TYPE_UID = 101,
LDNS_RR_TYPE_GID = 102,
LDNS_RR_TYPE_UNSPEC = 103,

LDNS_RR_TYPE_NID = 104,
LDNS_RR_TYPE_L32 = 105,
LDNS_RR_TYPE_L64 = 106,
LDNS_RR_TYPE_LP = 107,

LDNS_RR_TYPE_EUI48 = 108,
LDNS_RR_TYPE_EUI64 = 109,

LDNS_RR_TYPE_TKEY = 249,
LDNS_RR_TYPE_TSIG = 250,
LDNS_RR_TYPE_IXFR = 251,
LDNS_RR_TYPE_AXFR = 252,

LDNS_RR_TYPE_MAILB = 253,

LDNS_RR_TYPE_MAILA = 254,

LDNS_RR_TYPE_ANY = 255,
LDNS_RR_TYPE_URI = 256,
LDNS_RR_TYPE_CAA = 257,
LDNS_RR_TYPE_AVC = 258,


LDNS_RR_TYPE_TA = 32768,

LDNS_RR_TYPE_DLV = 32769,




LDNS_RR_TYPE_FIRST = 0,
LDNS_RR_TYPE_LAST = 65535,
LDNS_RR_TYPE_COUNT = LDNS_RR_TYPE_LAST - LDNS_RR_TYPE_FIRST + 1
};
typedef enum ldns_enum_rr_type ldns_rr_type;






























































struct ldns_struct_rr
{

ldns_rdf *_owner;

uint32_t _ttl;

size_t _rd_count;

ldns_rr_type _rr_type;

ldns_rr_class _rr_class;


ldns_rdf **_rdata_fields;



bool _rr_question;
};
typedef struct ldns_struct_rr ldns_rr;







struct ldns_struct_rr_list
{
size_t _rr_count;
size_t _rr_capacity;
ldns_rr **_rrs;
};
typedef struct ldns_struct_rr_list ldns_rr_list;






struct ldns_struct_rr_descriptor
{

ldns_rr_type _type;

const char *_name;

uint8_t _minimum;

uint8_t _maximum;

const ldns_rdf_type *_wireformat;

ldns_rdf_type _variable;

ldns_rr_compress _compress;

uint8_t _dname_count;
};
typedef struct ldns_struct_rr_descriptor ldns_rr_descriptor;








ldns_status ldns_rdf_bitmap_known_rr_types_space(ldns_rdf** rdf);






ldns_status ldns_rdf_bitmap_known_rr_types(ldns_rdf** rdf);






ldns_rr* ldns_rr_new(void);





ldns_rr* ldns_rr_new_frm_type(ldns_rr_type t);






void ldns_rr_free(ldns_rr *rr);

















ldns_status ldns_rr_new_frm_str(ldns_rr **n, const char *str,
uint32_t default_ttl, const ldns_rdf *origin,
ldns_rdf **prev);














ldns_status ldns_rr_new_question_frm_str(ldns_rr **n, const char *str,
const ldns_rdf *origin, ldns_rdf **prev);















ldns_status ldns_rr_new_frm_fp(ldns_rr **rr, FILE *fp, uint32_t *default_ttl, ldns_rdf **origin, ldns_rdf **prev);
















ldns_status ldns_rr_new_frm_fp_l(ldns_rr **rr, FILE *fp, uint32_t *default_ttl, ldns_rdf **origin, ldns_rdf **prev, int *line_nr);







void ldns_rr_set_owner(ldns_rr *rr, ldns_rdf *owner);







void ldns_rr_set_question(ldns_rr *rr, bool question);







void ldns_rr_set_ttl(ldns_rr *rr, uint32_t ttl);







void ldns_rr_set_rd_count(ldns_rr *rr, size_t count);







void ldns_rr_set_type(ldns_rr *rr, ldns_rr_type rr_type);







void ldns_rr_set_class(ldns_rr *rr, ldns_rr_class rr_class);









ldns_rdf* ldns_rr_set_rdf(ldns_rr *rr, const ldns_rdf *f, size_t position);








bool ldns_rr_push_rdf(ldns_rr *rr, const ldns_rdf *f);







ldns_rdf* ldns_rr_pop_rdf(ldns_rr *rr);







ldns_rdf* ldns_rr_rdf(const ldns_rr *rr, size_t nr);






ldns_rdf* ldns_rr_owner(const ldns_rr *rr);






bool ldns_rr_is_question(const ldns_rr *rr);






uint32_t ldns_rr_ttl(const ldns_rr *rr);






size_t ldns_rr_rd_count(const ldns_rr *rr);






ldns_rr_type ldns_rr_get_type(const ldns_rr *rr);






ldns_rr_class ldns_rr_get_class(const ldns_rr *rr);








size_t ldns_rr_list_rr_count(const ldns_rr_list *rr_list);







void ldns_rr_list_set_rr_count(ldns_rr_list *rr_list, size_t count);









ldns_rr * ldns_rr_list_set_rr(ldns_rr_list *rr_list, const ldns_rr *r, size_t count);







ldns_rr* ldns_rr_list_rr(const ldns_rr_list *rr_list, size_t nr);





ldns_rr_list* ldns_rr_list_new(void);





void ldns_rr_list_free(ldns_rr_list *rr_list);





void ldns_rr_list_deep_free(ldns_rr_list *rr_list);








bool ldns_rr_list_cat(ldns_rr_list *left, const ldns_rr_list *right);








ldns_rr_list* ldns_rr_list_cat_clone(const ldns_rr_list *left, const ldns_rr_list *right);







bool ldns_rr_list_push_rr(ldns_rr_list *rr_list, const ldns_rr *rr);







bool ldns_rr_list_push_rr_list(ldns_rr_list *rr_list, const ldns_rr_list *push_list);






ldns_rr* ldns_rr_list_pop_rr(ldns_rr_list *rr_list);







ldns_rr_list* ldns_rr_list_pop_rr_list(ldns_rr_list *rr_list, size_t size);








bool ldns_rr_list_contains_rr(const ldns_rr_list *rr_list, const ldns_rr *rr);






bool ldns_is_rrset(const ldns_rr_list *rr_list);







bool ldns_rr_set_push_rr(ldns_rr_list *rr_list, ldns_rr *rr);








ldns_rr* ldns_rr_set_pop_rr(ldns_rr_list *rr_list);






ldns_rr_list *ldns_rr_list_pop_rrset(ldns_rr_list *rr_list);







ldns_rr_type ldns_get_rr_type_by_name(const char *name);






ldns_rr_class ldns_get_rr_class_by_name(const char *name);






ldns_rr* ldns_rr_clone(const ldns_rr *rr);






ldns_rr_list* ldns_rr_list_clone(const ldns_rr_list *rrlist);






void ldns_rr_list_sort(ldns_rr_list *unsorted);









int ldns_rr_compare(const ldns_rr *rr1, const ldns_rr *rr2);









int ldns_rr_compare_no_rdata(const ldns_rr *rr1, const ldns_rr *rr2);









int ldns_rr_compare_wire(const ldns_buffer *rr1_buf, const ldns_buffer *rr2_buf);









bool ldns_rr_compare_ds(const ldns_rr *rr1, const ldns_rr *rr2);









int ldns_rr_list_compare(const ldns_rr_list *rrl1, const ldns_rr_list *rrl2);






size_t ldns_rr_uncompressed_size(const ldns_rr *r);






void ldns_rr2canonical(ldns_rr *rr);






void ldns_rr_list2canonical(const ldns_rr_list *rr_list);






uint8_t ldns_rr_label_count(const ldns_rr *rr);







const ldns_rr_descriptor *ldns_rr_descript(uint16_t type);







size_t ldns_rr_descriptor_minimum(const ldns_rr_descriptor *descriptor);







size_t ldns_rr_descriptor_maximum(const ldns_rr_descriptor *descriptor);








ldns_rdf_type ldns_rr_descriptor_field_type(const ldns_rr_descriptor *descriptor, size_t field);












ldns_rr_list *ldns_rr_list_subtype_by_rdf(const ldns_rr_list *l, const ldns_rdf *r, size_t pos);









ldns_rr_type ldns_rdf2rr_type(const ldns_rdf *rd);








ldns_rr_type
ldns_rr_list_type(const ldns_rr_list *rr_list);








ldns_rdf *
ldns_rr_list_owner(const ldns_rr_list *rr_list);

#if defined(__cplusplus)
}
#endif

#endif
