
















#if !defined(LDNS_HOST2STR_H)
#define LDNS_HOST2STR_H

#include <ldns/common.h>
#include <ldns/error.h>
#include <ldns/rr.h>
#include <ldns/rdata.h>
#include <ldns/packet.h>
#include <ldns/buffer.h>
#include <ldns/resolver.h>
#include <ldns/zone.h>
#include <ctype.h>

#include "ldns/util.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define LDNS_APL_IP4 1
#define LDNS_APL_IP6 2
#define LDNS_APL_MASK 0x7f
#define LDNS_APL_NEGATION 0x80







#define LDNS_COMMENT_NULLS (1 << 0)

#define LDNS_COMMENT_KEY_ID (1 << 1)

#define LDNS_COMMENT_KEY_TYPE (1 << 2)

#define LDNS_COMMENT_KEY_SIZE (1 << 3)

#define LDNS_COMMENT_BUBBLEBABBLE (1 << 4)

#define LDNS_COMMENT_FLAGS (1 << 5)

#define LDNS_COMMENT_NSEC3_CHAIN (1 << 6)

#define LDNS_COMMENT_LAYOUT (1 << 7)

#define LDNS_COMMENT_RRSIGS (1 << 8)
#define LDNS_FMT_ZEROIZE_RRSIGS (1 << 9)
#define LDNS_FMT_PAD_SOA_SERIAL (1 << 10)
#define LDNS_FMT_RFC3597 (1 << 11)

#define LDNS_FMT_FLAGS_WITH_DATA 2


#define LDNS_COMMENT_KEY (LDNS_COMMENT_KEY_ID |LDNS_COMMENT_KEY_TYPE|LDNS_COMMENT_KEY_SIZE)













struct ldns_struct_output_format
{

int flags;

void *data;
};
typedef struct ldns_struct_output_format ldns_output_format;






struct ldns_struct_output_format_storage
{ int flags;
ldns_rbtree_t* hashmap;
ldns_rdf* bitmap;
};
typedef struct ldns_struct_output_format_storage ldns_output_format_storage;





extern const ldns_output_format *ldns_output_format_nocomments;




extern const ldns_output_format *ldns_output_format_onlykeyids;



extern const ldns_output_format *ldns_output_format_default;





extern const ldns_output_format *ldns_output_format_bubblebabble;






INLINE
ldns_output_format* ldns_output_format_init(ldns_output_format_storage* fmt) {
fmt->flags = ldns_output_format_default->flags;
fmt->hashmap = NULL;
fmt->bitmap = NULL;
return (ldns_output_format*)fmt;
}




INLINE void ldns_output_format_set(ldns_output_format* fmt, int flag) {
fmt->flags |= flag;
}




INLINE void ldns_output_format_clear(ldns_output_format* fmt, int flag) {
fmt->flags &= !flag;
}








ldns_status
ldns_output_format_set_type(ldns_output_format* fmt, ldns_rr_type type);










ldns_status
ldns_output_format_clear_type(ldns_output_format* fmt, ldns_rr_type type);








ldns_status
ldns_pkt_opcode2buffer_str(ldns_buffer *output, ldns_pkt_opcode opcode);








ldns_status
ldns_pkt_rcode2buffer_str(ldns_buffer *output, ldns_pkt_rcode rcode);








ldns_status
ldns_algorithm2buffer_str(ldns_buffer *output,
ldns_algorithm algorithm);








ldns_status
ldns_cert_algorithm2buffer_str(ldns_buffer *output,
ldns_cert_algorithm cert_algorithm);










char *ldns_pkt_opcode2str(ldns_pkt_opcode opcode);









char *ldns_pkt_rcode2str(ldns_pkt_rcode rcode);









char *ldns_pkt_algorithm2str(ldns_algorithm algorithm);









char *ldns_pkt_cert_algorithm2str(ldns_cert_algorithm cert_algorithm);







ldns_status ldns_rdf2buffer_str_a(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_aaaa(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_str(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_b64(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_b32_ext(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_hex(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_type(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_class(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_alg(ldns_buffer *output, const ldns_rdf *rdf);








ldns_status ldns_rr_type2buffer_str(ldns_buffer *output,
const ldns_rr_type type);









char *ldns_rr_type2str(const ldns_rr_type type);








ldns_status ldns_rr_class2buffer_str(ldns_buffer *output,
const ldns_rr_class klass);









char *ldns_rr_class2str(const ldns_rr_class klass);








ldns_status ldns_rdf2buffer_str_cert_alg(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_loc(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_unknown(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_nsap(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_atma(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_wks(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_nsec(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_period(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_tsigtime(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_apl(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_int16_data(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_ipseckey(ldns_buffer *output, const ldns_rdf *rdf);









ldns_status ldns_rdf2buffer_str(ldns_buffer *output, const ldns_rdf *rdf);











ldns_status ldns_rr2buffer_str(ldns_buffer *output, const ldns_rr *rr);













ldns_status ldns_rr2buffer_str_fmt(ldns_buffer *output,
const ldns_output_format *fmt, const ldns_rr *rr);









ldns_status ldns_pkt2buffer_str(ldns_buffer *output, const ldns_pkt *pkt);










ldns_status ldns_pkt2buffer_str_fmt(ldns_buffer *output,
const ldns_output_format *fmt, const ldns_pkt *pkt);







ldns_status ldns_rdf2buffer_str_nsec3_salt(ldns_buffer *output, const ldns_rdf *rdf);










ldns_status ldns_key2buffer_str(ldns_buffer *output, const ldns_key *k);







ldns_status ldns_rdf2buffer_str_int8(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_int16(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_int32(ldns_buffer *output, const ldns_rdf *rdf);







ldns_status ldns_rdf2buffer_str_time(ldns_buffer *output, const ldns_rdf *rdf);








ldns_status ldns_rdf2buffer_str_ilnp64(ldns_buffer *output,
const ldns_rdf *rdf);








ldns_status ldns_rdf2buffer_str_eui48(ldns_buffer *output,
const ldns_rdf *rdf);








ldns_status ldns_rdf2buffer_str_eui64(ldns_buffer *output,
const ldns_rdf *rdf);








ldns_status ldns_rdf2buffer_str_tag(ldns_buffer *output,
const ldns_rdf *rdf);








ldns_status ldns_rdf2buffer_str_long_str(ldns_buffer *output,
const ldns_rdf *rdf);








ldns_status ldns_rdf2buffer_str_hip(ldns_buffer *output,
const ldns_rdf *rdf);









char *ldns_rdf2str(const ldns_rdf *rdf);









char *ldns_rr2str(const ldns_rr *rr);










char *ldns_rr2str_fmt(const ldns_output_format *fmt, const ldns_rr *rr);









char *ldns_pkt2str(const ldns_pkt *pkt);










char *ldns_pkt2str_fmt(const ldns_output_format *fmt, const ldns_pkt *pkt);









char *ldns_key2str(const ldns_key *k);









char *ldns_rr_list2str(const ldns_rr_list *rr_list);










char *ldns_rr_list2str_fmt(
const ldns_output_format *fmt, const ldns_rr_list *rr_list);









char *ldns_buffer2str(ldns_buffer *buffer);










char *ldns_buffer_export2str(ldns_buffer *buffer);









void ldns_rdf_print(FILE *output, const ldns_rdf *rdf);









void ldns_rr_print(FILE *output, const ldns_rr *rr);










void ldns_rr_print_fmt(FILE *output,
const ldns_output_format *fmt, const ldns_rr *rr);









void ldns_pkt_print(FILE *output, const ldns_pkt *pkt);










void ldns_pkt_print_fmt(FILE *output,
const ldns_output_format *fmt, const ldns_pkt *pkt);








ldns_status ldns_rr_list2buffer_str(ldns_buffer *output, const ldns_rr_list *list);









ldns_status ldns_rr_list2buffer_str_fmt(ldns_buffer *output,
const ldns_output_format *fmt, const ldns_rr_list *list);








ldns_status ldns_pktheader2buffer_str(ldns_buffer *output, const ldns_pkt *pkt);






void ldns_rr_list_print(FILE *output, const ldns_rr_list *list);







void ldns_rr_list_print_fmt(FILE *output,
const ldns_output_format *fmt, const ldns_rr_list *list);







void ldns_resolver_print(FILE *output, const ldns_resolver *r);








void ldns_resolver_print_fmt(FILE *output,
const ldns_output_format *fmt, const ldns_resolver *r);







void ldns_zone_print(FILE *output, const ldns_zone *z);








void ldns_zone_print_fmt(FILE *output,
const ldns_output_format *fmt, const ldns_zone *z);







ldns_status ldns_rdf2buffer_str_dname(ldns_buffer *output, const ldns_rdf *dname);

#if defined(__cplusplus)
}
#endif

#endif
