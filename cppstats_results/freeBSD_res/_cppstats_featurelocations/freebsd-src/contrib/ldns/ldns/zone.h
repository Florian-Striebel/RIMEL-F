





















#if !defined(LDNS_ZONE_H)
#define LDNS_ZONE_H

#include <ldns/common.h>
#include <ldns/rdata.h>
#include <ldns/rr.h>
#include <ldns/error.h>

#if defined(__cplusplus)
extern "C" {
#endif








struct ldns_struct_zone
{

ldns_rr *_soa;

ldns_rr_list *_rrs;

};
typedef struct ldns_struct_zone ldns_zone;





ldns_zone * ldns_zone_new(void);






ldns_rr * ldns_zone_soa(const ldns_zone *z);






size_t ldns_zone_rr_count(const ldns_zone *z);






void ldns_zone_set_soa(ldns_zone *z, ldns_rr *soa);








ldns_rr_list * ldns_zone_rrs(const ldns_zone *z);






void ldns_zone_set_rrs(ldns_zone *z, ldns_rr_list *rrlist);








bool ldns_zone_push_rr_list(ldns_zone *z, const ldns_rr_list *list);








bool ldns_zone_push_rr(ldns_zone *z, ldns_rr *rr);














ldns_rr_list *ldns_zone_glue_rr_list(const ldns_zone *z);











ldns_status ldns_zone_new_frm_fp(ldns_zone **z, FILE *fp, const ldns_rdf *origin, uint32_t ttl, ldns_rr_class c);












ldns_status ldns_zone_new_frm_fp_l(ldns_zone **z, FILE *fp, const ldns_rdf *origin, uint32_t ttl, ldns_rr_class c, int *line_nr);





void ldns_zone_free(ldns_zone *zone);






void ldns_zone_deep_free(ldns_zone *zone);





void ldns_zone_sort(ldns_zone *zone);

#if defined(__cplusplus)
}
#endif

#endif
