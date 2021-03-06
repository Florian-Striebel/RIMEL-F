



















#if !defined(LDNS_PACKET_H)
#define LDNS_PACKET_H

#define LDNS_MAX_PACKETLEN 65535


#define LDNS_QR 1
#define LDNS_AA 2
#define LDNS_TC 4
#define LDNS_RD 8
#define LDNS_CD 16
#define LDNS_RA 32
#define LDNS_AD 64

#include <ldns/error.h>
#include <ldns/common.h>
#include <ldns/rr.h>
#include <sys/time.h>

#if defined(__cplusplus)
extern "C" {
#endif


enum ldns_enum_pkt_opcode {
LDNS_PACKET_QUERY = 0,
LDNS_PACKET_IQUERY = 1,
LDNS_PACKET_STATUS = 2,
LDNS_PACKET_NOTIFY = 4,
LDNS_PACKET_UPDATE = 5
};
typedef enum ldns_enum_pkt_opcode ldns_pkt_opcode;


enum ldns_enum_pkt_rcode {
LDNS_RCODE_NOERROR = 0,
LDNS_RCODE_FORMERR = 1,
LDNS_RCODE_SERVFAIL = 2,
LDNS_RCODE_NXDOMAIN = 3,
LDNS_RCODE_NOTIMPL = 4,
LDNS_RCODE_REFUSED = 5,
LDNS_RCODE_YXDOMAIN = 6,
LDNS_RCODE_YXRRSET = 7,
LDNS_RCODE_NXRRSET = 8,
LDNS_RCODE_NOTAUTH = 9,
LDNS_RCODE_NOTZONE = 10
};
typedef enum ldns_enum_pkt_rcode ldns_pkt_rcode;



























































































































struct ldns_struct_hdr
{

uint16_t _id;

bool _qr;

bool _aa;

bool _tc;

bool _rd;

bool _cd;

bool _ra;

bool _ad;

ldns_pkt_opcode _opcode;

uint8_t _rcode;

uint16_t _qdcount;

uint16_t _ancount;

uint16_t _nscount;

uint16_t _arcount;
};
typedef struct ldns_struct_hdr ldns_hdr;









struct ldns_struct_pkt
{

ldns_hdr *_header;


ldns_rdf *_answerfrom;

struct timeval timestamp;

uint32_t _querytime;

size_t _size;

ldns_rr *_tsig_rr;

uint16_t _edns_udp_size;

uint8_t _edns_extended_rcode;

uint8_t _edns_version;

uint8_t _edns_present;

uint16_t _edns_z;

ldns_rdf *_edns_data;

ldns_rr_list *_question;

ldns_rr_list *_answer;

ldns_rr_list *_authority;

ldns_rr_list *_additional;
};
typedef struct ldns_struct_pkt ldns_pkt;




enum ldns_enum_pkt_section {
LDNS_SECTION_QUESTION = 0,
LDNS_SECTION_ANSWER = 1,
LDNS_SECTION_AUTHORITY = 2,
LDNS_SECTION_ADDITIONAL = 3,

LDNS_SECTION_ANY = 4,

LDNS_SECTION_ANY_NOQUESTION = 5
};
typedef enum ldns_enum_pkt_section ldns_pkt_section;




enum ldns_enum_pkt_type {
LDNS_PACKET_QUESTION,
LDNS_PACKET_REFERRAL,
LDNS_PACKET_ANSWER,
LDNS_PACKET_NXDOMAIN,
LDNS_PACKET_NODATA,
LDNS_PACKET_UNKNOWN
};
typedef enum ldns_enum_pkt_type ldns_pkt_type;










uint16_t ldns_pkt_id(const ldns_pkt *p);





bool ldns_pkt_qr(const ldns_pkt *p);





bool ldns_pkt_aa(const ldns_pkt *p);





bool ldns_pkt_tc(const ldns_pkt *p);





bool ldns_pkt_rd(const ldns_pkt *p);





bool ldns_pkt_cd(const ldns_pkt *p);





bool ldns_pkt_ra(const ldns_pkt *p);





bool ldns_pkt_ad(const ldns_pkt *p);





ldns_pkt_opcode ldns_pkt_get_opcode(const ldns_pkt *p);





ldns_pkt_rcode ldns_pkt_get_rcode(const ldns_pkt *p);





uint16_t ldns_pkt_qdcount(const ldns_pkt *p);





uint16_t ldns_pkt_ancount(const ldns_pkt *p);





uint16_t ldns_pkt_nscount(const ldns_pkt *p);





uint16_t ldns_pkt_arcount(const ldns_pkt *p);






ldns_rdf *ldns_pkt_answerfrom(const ldns_pkt *p);






struct timeval ldns_pkt_timestamp(const ldns_pkt *p);





uint32_t ldns_pkt_querytime(const ldns_pkt *p);






size_t ldns_pkt_size(const ldns_pkt *p);










uint16_t ldns_pkt_section_count(const ldns_pkt *p, ldns_pkt_section s);






ldns_rr *ldns_pkt_tsig(const ldns_pkt *p);






ldns_rr_list *ldns_pkt_question(const ldns_pkt *p);





ldns_rr_list *ldns_pkt_answer(const ldns_pkt *p);





ldns_rr_list *ldns_pkt_authority(const ldns_pkt *p);





ldns_rr_list *ldns_pkt_additional(const ldns_pkt *p);






ldns_rr_list *ldns_pkt_all(const ldns_pkt *p);






ldns_rr_list *ldns_pkt_all_noquestion(const ldns_pkt *p);








ldns_rr_list *ldns_pkt_get_section_clone(const ldns_pkt *p, ldns_pkt_section s);









ldns_rr_list *ldns_pkt_rr_list_by_name(const ldns_pkt *p, const ldns_rdf *r, ldns_pkt_section s);








ldns_rr_list *ldns_pkt_rr_list_by_type(const ldns_pkt *p, ldns_rr_type t, ldns_pkt_section s);









ldns_rr_list *ldns_pkt_rr_list_by_name_and_type(const ldns_pkt *packet, const ldns_rdf *ownername, ldns_rr_type type, ldns_pkt_section sec);








bool ldns_pkt_rr(const ldns_pkt *pkt, ldns_pkt_section sec, const ldns_rr *rr);








bool ldns_pkt_set_flags(ldns_pkt *pkt, uint16_t flags);






void ldns_pkt_set_id(ldns_pkt *p, uint16_t id);




void ldns_pkt_set_random_id(ldns_pkt *p);





void ldns_pkt_set_qr(ldns_pkt *p, bool b);





void ldns_pkt_set_aa(ldns_pkt *p, bool b);





void ldns_pkt_set_tc(ldns_pkt *p, bool b);





void ldns_pkt_set_rd(ldns_pkt *p, bool b);





void ldns_pkt_set_cd(ldns_pkt *p, bool b);





void ldns_pkt_set_ra(ldns_pkt *p, bool b);





void ldns_pkt_set_ad(ldns_pkt *p, bool b);






void ldns_pkt_set_opcode(ldns_pkt *p, ldns_pkt_opcode c);





void ldns_pkt_set_rcode(ldns_pkt *p, uint8_t c);





void ldns_pkt_set_qdcount(ldns_pkt *p, uint16_t c);





void ldns_pkt_set_ancount(ldns_pkt *p, uint16_t c);





void ldns_pkt_set_nscount(ldns_pkt *p, uint16_t c);





void ldns_pkt_set_arcount(ldns_pkt *p, uint16_t c);





void ldns_pkt_set_answerfrom(ldns_pkt *p, ldns_rdf *r);





void ldns_pkt_set_querytime(ldns_pkt *p, uint32_t t);





void ldns_pkt_set_size(ldns_pkt *p, size_t s);






void ldns_pkt_set_timestamp(ldns_pkt *p, struct timeval timeval);






void ldns_pkt_set_section_count(ldns_pkt *p, ldns_pkt_section s, uint16_t x);





void ldns_pkt_set_tsig(ldns_pkt *p, ldns_rr *t);







ldns_pkt_type ldns_pkt_reply_type(const ldns_pkt *p);






uint16_t ldns_pkt_edns_udp_size(const ldns_pkt *packet);





uint8_t ldns_pkt_edns_extended_rcode(const ldns_pkt *packet);





uint8_t ldns_pkt_edns_version(const ldns_pkt *packet);





uint16_t ldns_pkt_edns_z(const ldns_pkt *packet);





ldns_rdf *ldns_pkt_edns_data(const ldns_pkt *packet);






bool ldns_pkt_edns_do(const ldns_pkt *packet);





void ldns_pkt_set_edns_do(ldns_pkt *packet, bool value);










bool ldns_pkt_edns(const ldns_pkt *packet);






void ldns_pkt_set_edns_udp_size(ldns_pkt *packet, uint16_t s);





void ldns_pkt_set_edns_extended_rcode(ldns_pkt *packet, uint8_t c);





void ldns_pkt_set_edns_version(ldns_pkt *packet, uint8_t v);





void ldns_pkt_set_edns_z(ldns_pkt *packet, uint16_t z);





void ldns_pkt_set_edns_data(ldns_pkt *packet, ldns_rdf *data);





ldns_pkt *ldns_pkt_new(void);






void ldns_pkt_free(ldns_pkt *packet);










ldns_status ldns_pkt_query_new_frm_str(ldns_pkt **p, const char *rr_name, ldns_rr_type rr_type, ldns_rr_class rr_class , uint16_t flags);











ldns_status ldns_pkt_ixfr_request_new_frm_str(ldns_pkt **p, const char *rr_name, ldns_rr_class rr_class, uint16_t flags, ldns_rr* soa);










ldns_pkt *ldns_pkt_query_new(ldns_rdf *rr_name, ldns_rr_type rr_type, ldns_rr_class rr_class, uint16_t flags);











ldns_pkt *ldns_pkt_ixfr_request_new(ldns_rdf *rr_name, ldns_rr_class rr_class, uint16_t flags, ldns_rr* soa);







ldns_pkt *ldns_pkt_clone(const ldns_pkt *pkt);






void ldns_pkt_set_additional(ldns_pkt *p, ldns_rr_list *rr);






void ldns_pkt_set_answer(ldns_pkt *p, ldns_rr_list *rr);






void ldns_pkt_set_question(ldns_pkt *p, ldns_rr_list *rr);






void ldns_pkt_set_authority(ldns_pkt *p, ldns_rr_list *rr);








bool ldns_pkt_push_rr(ldns_pkt *packet, ldns_pkt_section section, ldns_rr *rr);








bool ldns_pkt_safe_push_rr(ldns_pkt *pkt, ldns_pkt_section sec, ldns_rr *rr);








bool ldns_pkt_push_rr_list(ldns_pkt *packet, ldns_pkt_section section, ldns_rr_list *list);








bool ldns_pkt_safe_push_rr_list(ldns_pkt *pkt, ldns_pkt_section sec, ldns_rr_list *list);






bool ldns_pkt_empty(ldns_pkt *p);

#if defined(__cplusplus)
}
#endif

#endif
