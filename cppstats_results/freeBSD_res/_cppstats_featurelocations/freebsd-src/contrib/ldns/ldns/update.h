
















#if !defined(LDNS_UPDATE_H)
#define LDNS_UPDATE_H

#include <ldns/resolver.h>

#if defined(__cplusplus)
extern "C" {
#endif











ldns_pkt *ldns_update_pkt_new(ldns_rdf *zone_rdf, ldns_rr_class clas, const ldns_rr_list *pr_rrlist, const ldns_rr_list *up_rrlist, const ldns_rr_list *ad_rrlist);









ldns_status ldns_update_pkt_tsig_add(ldns_pkt *p, const ldns_resolver *r);








uint16_t ldns_update_zocount(const ldns_pkt *p);





uint16_t ldns_update_prcount(const ldns_pkt *p);





uint16_t ldns_update_upcount(const ldns_pkt *p);





uint16_t ldns_update_ad(const ldns_pkt *p);





void ldns_update_set_zo(ldns_pkt *p, uint16_t c);





void ldns_update_set_prcount(ldns_pkt *p, uint16_t c);





void ldns_update_set_upcount(ldns_pkt *p, uint16_t c);





void ldns_update_set_adcount(ldns_pkt *p, uint16_t c);






ldns_status ldns_update_soa_mname(ldns_rdf *zone, ldns_resolver *r, ldns_rr_class c, ldns_rdf **mname);




ldns_status ldns_update_soa_zone_mname(const char *fqdn, ldns_resolver *r, ldns_rr_class c, ldns_rdf **zone_rdf, ldns_rdf **mname_rdf);

#if defined(__cplusplus)
}
#endif

#endif
