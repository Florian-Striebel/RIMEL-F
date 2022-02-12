






#if !defined(LDNS_DNSSEC_ZONE_H)
#define LDNS_DNSSEC_ZONE_H

#include <ldns/rbtree.h>
#include <ldns/host2str.h>

#if defined(__cplusplus)
extern "C" {
#endif




typedef struct ldns_struct_dnssec_rrs ldns_dnssec_rrs;
struct ldns_struct_dnssec_rrs
{
ldns_rr *rr;
ldns_dnssec_rrs *next;
};




typedef struct ldns_struct_dnssec_rrsets ldns_dnssec_rrsets;
struct ldns_struct_dnssec_rrsets
{
ldns_dnssec_rrs *rrs;
ldns_rr_type type;
ldns_dnssec_rrs *signatures;
ldns_dnssec_rrsets *next;
};





typedef struct ldns_struct_dnssec_name ldns_dnssec_name;
struct ldns_struct_dnssec_name
{




ldns_rdf *name;







bool name_alloced;



ldns_dnssec_rrsets *rrsets;



ldns_rr *nsec;



ldns_dnssec_rrs *nsec_signatures;









bool is_glue;



ldns_rdf *hashed_name;
};




struct ldns_struct_dnssec_zone {

ldns_dnssec_name *soa;

ldns_rbtree_t *names;

ldns_rbtree_t *hashed_names;




ldns_rr *_nsec3params;
};
typedef struct ldns_struct_dnssec_zone ldns_dnssec_zone;





ldns_dnssec_rrs *ldns_dnssec_rrs_new(void);







void ldns_dnssec_rrs_free(ldns_dnssec_rrs *rrs);







void ldns_dnssec_rrs_deep_free(ldns_dnssec_rrs *rrs);









ldns_status ldns_dnssec_rrs_add_rr(ldns_dnssec_rrs *rrs, ldns_rr *rr);







void ldns_dnssec_rrs_print(FILE *out, const ldns_dnssec_rrs *rrs);








void ldns_dnssec_rrs_print_fmt(FILE *out,
const ldns_output_format *fmt, const ldns_dnssec_rrs *rrs);





ldns_dnssec_rrsets *ldns_dnssec_rrsets_new(void);







void ldns_dnssec_rrsets_free(ldns_dnssec_rrsets *rrsets);







void ldns_dnssec_rrsets_deep_free(ldns_dnssec_rrsets *rrsets);







ldns_rr_type ldns_dnssec_rrsets_type(const ldns_dnssec_rrsets *rrsets);








ldns_status ldns_dnssec_rrsets_set_type(ldns_dnssec_rrsets *rrsets,
ldns_rr_type type);









ldns_status ldns_dnssec_rrsets_add_rr(ldns_dnssec_rrsets *rrsets, ldns_rr *rr);








void ldns_dnssec_rrsets_print(FILE *out,
const ldns_dnssec_rrsets *rrsets,
bool follow);









void ldns_dnssec_rrsets_print_fmt(FILE *out,
const ldns_output_format *fmt,
const ldns_dnssec_rrsets *rrsets,
bool follow);






ldns_dnssec_name *ldns_dnssec_name_new(void);






ldns_dnssec_name *ldns_dnssec_name_new_frm_rr(ldns_rr *rr);







void ldns_dnssec_name_free(ldns_dnssec_name *name);







void ldns_dnssec_name_deep_free(ldns_dnssec_name *name);







ldns_rdf *ldns_dnssec_name_name(const ldns_dnssec_name *name);








void ldns_dnssec_name_set_name(ldns_dnssec_name *name,
ldns_rdf *dname);











bool ldns_dnssec_name_is_glue(const ldns_dnssec_name *name);







void ldns_dnssec_name_set_nsec(ldns_dnssec_name *name, ldns_rr *nsec);











int ldns_dnssec_name_cmp(const void *a, const void *b);









ldns_status ldns_dnssec_name_add_rr(ldns_dnssec_name *name,
ldns_rr *rr);








ldns_dnssec_rrsets *ldns_dnssec_name_find_rrset(const ldns_dnssec_name *name,
ldns_rr_type type);









ldns_dnssec_rrsets *ldns_dnssec_zone_find_rrset(const ldns_dnssec_zone *zone,
const ldns_rdf *dname,
ldns_rr_type type);








void ldns_dnssec_name_print(FILE *out, const ldns_dnssec_name *name);









void ldns_dnssec_name_print_fmt(FILE *out,
const ldns_output_format *fmt, const ldns_dnssec_name *name);





ldns_dnssec_zone *ldns_dnssec_zone_new(void);











ldns_status ldns_dnssec_zone_new_frm_fp(ldns_dnssec_zone** z, FILE* fp,
const ldns_rdf* origin, uint32_t ttl, ldns_rr_class c);












ldns_status ldns_dnssec_zone_new_frm_fp_l(ldns_dnssec_zone** z, FILE* fp,
const ldns_rdf* origin, uint32_t ttl, ldns_rr_class c, int* line_nr);






void ldns_dnssec_zone_free(ldns_dnssec_zone *zone);






void ldns_dnssec_zone_deep_free(ldns_dnssec_zone *zone);











ldns_status ldns_dnssec_zone_add_rr(ldns_dnssec_zone *zone,
ldns_rr *rr);








void ldns_dnssec_zone_names_print(FILE *out, const ldns_rbtree_t *tree, bool print_soa);









void ldns_dnssec_zone_names_print_fmt(FILE *out, const ldns_output_format *fmt,
const ldns_rbtree_t *tree, bool print_soa);







void ldns_dnssec_zone_print(FILE *out, const ldns_dnssec_zone *zone);








void ldns_dnssec_zone_print_fmt(FILE *out,
const ldns_output_format *fmt, const ldns_dnssec_zone *zone);








ldns_status ldns_dnssec_zone_add_empty_nonterminals(ldns_dnssec_zone *zone);








bool ldns_dnssec_zone_is_nsec3_optout(const ldns_dnssec_zone* zone);

#if defined(__cplusplus)
}
#endif

#endif
