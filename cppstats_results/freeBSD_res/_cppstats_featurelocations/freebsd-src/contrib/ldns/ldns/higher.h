














#if !defined(LDNS_HIGHER_H)
#define LDNS_HIGHER_H

#include <ldns/resolver.h>
#include <ldns/rdata.h>
#include <ldns/rr.h>
#include <ldns/host2str.h>
#include <ldns/tsig.h>

#if defined(__cplusplus)
extern "C" {
#endif









ldns_rr_list *ldns_get_rr_list_addr_by_name(ldns_resolver *r, const ldns_rdf *name, ldns_rr_class c, uint16_t flags);









ldns_rr_list *ldns_get_rr_list_name_by_addr(ldns_resolver *r, const ldns_rdf *addr, ldns_rr_class c, uint16_t flags);








ldns_rr_list *ldns_get_rr_list_hosts_frm_fp(FILE *fp);









ldns_rr_list *ldns_get_rr_list_hosts_frm_fp_l(FILE *fp, int *line_nr);








ldns_rr_list *ldns_get_rr_list_hosts_frm_file(char *filename);













uint16_t ldns_getaddrinfo(ldns_resolver *res, const ldns_rdf *node, ldns_rr_class c, ldns_rr_list **list);







bool ldns_nsec_type_check(const ldns_rr *nsec, ldns_rr_type t);









void ldns_print_rr_rdf(FILE *fp, ldns_rr *r, int rdfnum, ...);

#if defined(__cplusplus)
}
#endif

#endif
