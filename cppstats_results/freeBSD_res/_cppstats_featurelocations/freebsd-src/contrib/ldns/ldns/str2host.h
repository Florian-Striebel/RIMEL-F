









#if !defined(LDNS_2HOST_H)
#define LDNS_2HOST_H

#include <ldns/common.h>
#include <ldns/error.h>
#include <ldns/rr.h>
#include <ldns/rdata.h>
#include <ldns/packet.h>
#include <ldns/buffer.h>
#include <ctype.h>

#if defined(__cplusplus)
extern "C" {
#endif














ldns_status ldns_str2rdf_int8(ldns_rdf **rd, const char *bytestr);







ldns_status ldns_str2rdf_int16(ldns_rdf **rd, const char *shortstr);







ldns_status ldns_str2rdf_int32(ldns_rdf **rd, const char *longstr);







ldns_status ldns_str2rdf_time(ldns_rdf **rd, const char *time);






ldns_status ldns_str2rdf_nsec3_salt(ldns_rdf **rd, const char *nsec3_salt);






ldns_status ldns_str2rdf_period(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_a(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_aaaa(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_str(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_apl(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_b64(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_b32_ext(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_hex(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_nsec(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_type(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_class(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_cert_alg(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_alg(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_certificate_usage(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_selector(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_matching_type(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_unknown(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_service(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_loc(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_wks(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_nsap(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_atma(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_ipseckey(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_dname(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_ilnp64(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_eui48(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_eui64(ldns_rdf **rd, const char *str);







ldns_status ldns_str2rdf_tag(ldns_rdf **rd, const char *str);








ldns_status ldns_str2rdf_long_str(ldns_rdf **rd, const char *str);









ldns_status ldns_str2rdf_hip(ldns_rdf **rd, const char *str);


#if defined(__cplusplus)
}
#endif

#endif
