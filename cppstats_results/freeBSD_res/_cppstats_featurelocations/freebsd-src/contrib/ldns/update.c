








#include <ldns/config.h>

#include <ldns/ldns.h>

#include <strings.h>
#include <stdlib.h>
#include <limits.h>









ldns_pkt *
ldns_update_pkt_new(ldns_rdf *zone_rdf, ldns_rr_class c,
const ldns_rr_list *pr_rrlist, const ldns_rr_list *up_rrlist, const ldns_rr_list *ad_rrlist)
{
ldns_pkt *p;

if (!zone_rdf || !up_rrlist) {
return NULL;
}

if (c == 0) {
c = LDNS_RR_CLASS_IN;
}


p = ldns_pkt_query_new(zone_rdf, LDNS_RR_TYPE_SOA, c, LDNS_RD);
if (!p) {
return NULL;
}
zone_rdf = NULL;

ldns_pkt_set_opcode(p, LDNS_PACKET_UPDATE);

ldns_rr_list_deep_free(p->_authority);

ldns_pkt_set_authority(p, ldns_rr_list_clone(up_rrlist));

ldns_update_set_upcount(p, ldns_rr_list_rr_count(up_rrlist));

if (pr_rrlist) {
ldns_rr_list_deep_free(p->_answer);
ldns_pkt_set_answer(p, ldns_rr_list_clone(pr_rrlist));
ldns_update_set_prcount(p, ldns_rr_list_rr_count(pr_rrlist));
}

if (ad_rrlist) {
ldns_rr_list_deep_free(p->_additional);
ldns_pkt_set_additional(p, ldns_rr_list_clone(ad_rrlist));
ldns_update_set_adcount(p, ldns_rr_list_rr_count(ad_rrlist));
}
return p;
}

ldns_status
ldns_update_pkt_tsig_add(ldns_pkt *p, const ldns_resolver *r)
{
#if defined(HAVE_SSL)
uint16_t fudge = 300;
if (ldns_resolver_tsig_keyname(r) && ldns_resolver_tsig_keydata(r))
return ldns_pkt_tsig_sign(p, ldns_resolver_tsig_keyname(r),
ldns_resolver_tsig_keydata(r), fudge,
ldns_resolver_tsig_algorithm(r), NULL);
#else

(void)p;
(void)r;
#endif

return LDNS_STATUS_OK;
}



ldns_status
ldns_update_soa_mname(ldns_rdf *zone, ldns_resolver *r,
ldns_rr_class c, ldns_rdf **mname)
{
ldns_rr *soa_rr;
ldns_pkt *query, *resp;


query = ldns_pkt_query_new(ldns_rdf_clone(zone), LDNS_RR_TYPE_SOA,
c, LDNS_RD);
if (!query) {
return LDNS_STATUS_ERR;
}

ldns_pkt_set_random_id(query);
if (ldns_resolver_send_pkt(&resp, r, query) != LDNS_STATUS_OK) {
ldns_pkt_free(query);
return LDNS_STATUS_ERR;
}
ldns_pkt_free(query);
if (!resp) {
return LDNS_STATUS_ERR;
}


*mname = NULL;
while ((soa_rr = ldns_rr_list_pop_rr(ldns_pkt_answer(resp)))) {
if (ldns_rr_get_type(soa_rr) != LDNS_RR_TYPE_SOA
|| ldns_rr_rdf(soa_rr, 0) == NULL)
continue;

*mname = ldns_rdf_clone(ldns_rr_rdf(soa_rr, 0));
break;
}
ldns_pkt_free(resp);

return *mname ? LDNS_STATUS_OK : LDNS_STATUS_ERR;
}


ldns_status
ldns_update_soa_zone_mname(const char *fqdn, ldns_resolver *r,
ldns_rr_class c, ldns_rdf **zone_rdf, ldns_rdf **mname_rdf)
{
ldns_rr *soa_rr, *rr;
ldns_rdf *soa_zone = NULL, *soa_mname = NULL;
ldns_rdf *ipaddr, *fqdn_rdf, *tmp;
ldns_rdf **nslist;
ldns_pkt *query, *resp;
ldns_resolver *tmp_r;
size_t i;







fqdn_rdf = ldns_dname_new_frm_str(fqdn);
query = ldns_pkt_query_new(fqdn_rdf, LDNS_RR_TYPE_SOA, c, LDNS_RD);
if (!query) {
return LDNS_STATUS_ERR;
}
fqdn_rdf = NULL;

ldns_pkt_set_random_id(query);
if (ldns_resolver_send_pkt(&resp, r, query) != LDNS_STATUS_OK) {
ldns_pkt_free(query);
return LDNS_STATUS_ERR;
}
ldns_pkt_free(query);
if (!resp) {
return LDNS_STATUS_ERR;
}


while ((soa_rr = ldns_rr_list_pop_rr(ldns_pkt_authority(resp)))) {
if (ldns_rr_get_type(soa_rr) != LDNS_RR_TYPE_SOA
|| ldns_rr_rdf(soa_rr, 0) == NULL)
continue;

soa_mname = ldns_rdf_clone(ldns_rr_rdf(soa_rr, 0));
break;
}
ldns_pkt_free(resp);
if (!soa_rr) {
return LDNS_STATUS_ERR;
}


query = ldns_pkt_query_new(soa_mname, LDNS_RR_TYPE_A, c, LDNS_RD);
if (!query) {
return LDNS_STATUS_ERR;
}
soa_mname = NULL;

ldns_pkt_set_random_id(query);
if (ldns_resolver_send_pkt(&resp, r, query) != LDNS_STATUS_OK) {
ldns_pkt_free(query);
return LDNS_STATUS_ERR;
}
ldns_pkt_free(query);
if (!resp) {
return LDNS_STATUS_ERR;
}

if (ldns_pkt_ancount(resp) == 0) {
ldns_pkt_free(resp);
return LDNS_STATUS_ERR;
}


rr = ldns_rr_list_pop_rr(ldns_pkt_answer(resp));
ipaddr = ldns_rr_rdf(rr, 0);


if (!(tmp_r = ldns_resolver_clone(r))) {
return LDNS_STATUS_MEM_ERR;
}
nslist = ldns_resolver_nameservers(tmp_r);
for (i = 0; i < ldns_resolver_nameserver_count(tmp_r); i++) {
if (ldns_rdf_compare(ipaddr, nslist[i]) == 0) {
if (i) {
tmp = nslist[0];
nslist[0] = nslist[i];
nslist[i] = tmp;
}
break;
}
}
if (i >= ldns_resolver_nameserver_count(tmp_r)) {

(void) ldns_resolver_push_nameserver(tmp_r, ipaddr);
nslist = ldns_resolver_nameservers(tmp_r);
i = ldns_resolver_nameserver_count(tmp_r) - 1;
tmp = nslist[0];
nslist[0] = nslist[i];
nslist[i] = tmp;
}
ldns_pkt_free(resp);


ldns_resolver_set_random(tmp_r, false);


fqdn_rdf = ldns_dname_new_frm_str(fqdn);
query = ldns_pkt_query_new(fqdn_rdf, LDNS_RR_TYPE_SOA, c, LDNS_RD);
if (!query) {
ldns_resolver_free(tmp_r);
return LDNS_STATUS_ERR;
}
fqdn_rdf = NULL;

ldns_pkt_set_random_id(query);
if (ldns_resolver_send_pkt(&resp, tmp_r, query) != LDNS_STATUS_OK) {
ldns_pkt_free(query);
ldns_resolver_free(tmp_r);
return LDNS_STATUS_ERR;
}
ldns_resolver_free(tmp_r);
ldns_pkt_free(query);
if (!resp) {
return LDNS_STATUS_ERR;
}


while ((soa_rr = ldns_rr_list_pop_rr(ldns_pkt_authority(resp)))) {
if (ldns_rr_get_type(soa_rr) != LDNS_RR_TYPE_SOA
|| ldns_rr_rdf(soa_rr, 0) == NULL)
continue;

soa_mname = ldns_rdf_clone(ldns_rr_rdf(soa_rr, 0));
soa_zone = ldns_rdf_clone(ldns_rr_owner(soa_rr));
break;
}
ldns_pkt_free(resp);
if (!soa_rr) {
return LDNS_STATUS_ERR;
}


*zone_rdf = soa_zone;
*mname_rdf = soa_mname;
return LDNS_STATUS_OK;
}





uint16_t
ldns_update_zocount(const ldns_pkt *p)
{
return ldns_pkt_qdcount(p);
}

uint16_t
ldns_update_prcount(const ldns_pkt *p)
{
return ldns_pkt_ancount(p);
}

uint16_t
ldns_update_upcount(const ldns_pkt *p)
{
return ldns_pkt_nscount(p);
}

uint16_t
ldns_update_ad(const ldns_pkt *p)
{
return ldns_pkt_arcount(p);
}

void
ldns_update_set_zo(ldns_pkt *p, uint16_t v)
{
ldns_pkt_set_qdcount(p, v);
}

void
ldns_update_set_prcount(ldns_pkt *p, uint16_t v)
{
ldns_pkt_set_ancount(p, v);
}

void
ldns_update_set_upcount(ldns_pkt *p, uint16_t v)
{
ldns_pkt_set_nscount(p, v);
}

void
ldns_update_set_adcount(ldns_pkt *p, uint16_t v)
{
ldns_pkt_set_arcount(p, v);
}
