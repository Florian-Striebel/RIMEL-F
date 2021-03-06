









#if !defined(_DRILL_UTIL_H_)
#define _DRILL_UTIL_H_
#include <ldns/ldns.h>





ldns_status read_key_file(const char *filename, ldns_rr_list *key_list,
bool silently);





ldns_rdf * ldns_rdf_new_addr_frm_str(char *);




void print_ds_of_keys(ldns_pkt *p);




void print_rrsig_abbr(FILE *fp, ldns_rr *sig);



void print_dnskey_abbr(FILE *fp, ldns_rr *key);



void print_ds_abbr(FILE *fp, ldns_rr *ds);




void print_rr_list_abbr(FILE *fp, ldns_rr_list *sig, const char *usr);




void *xmalloc(size_t s);




void *xrealloc(void *p, size_t s);




void xfree(void *q);
#endif
