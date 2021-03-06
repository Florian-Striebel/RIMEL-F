









#include "drill.h"
#include <ldns/ldns.h>




static void add_rr_list_to_referrals(
ldns_dnssec_zone *referrals, ldns_rr_list *rr_list)
{
size_t i;
ldns_rr *rr;
ldns_dnssec_rrsets *rrset;
ldns_dnssec_rrs *rrs;

for (i = 0; i < ldns_rr_list_rr_count(rr_list); i++) {
rr = ldns_rr_list_rr(rr_list, i);

rrset = ldns_dnssec_zone_find_rrset(
referrals, ldns_rr_owner(rr), ldns_rr_get_type(rr));
if (rrset) {
for (rrs = rrset->rrs; rrs; rrs = rrs->next)
if (ldns_rr_compare(rr, rrs->rr) == 0)
break;
if (rrs) continue;
}
if (ldns_rr_get_type(rr) == LDNS_RR_TYPE_NS && verbosity != -1)
ldns_rr_print(stdout, rr);
(void) ldns_dnssec_zone_add_rr(referrals, rr);
}
}




static void add_referrals(ldns_dnssec_zone *referrals, ldns_pkt *p)
{
ldns_rr_list *l = ldns_pkt_all_noquestion(p);
if (l) {
add_rr_list_to_referrals(referrals, l);
ldns_rr_list_free(l);
}
}




static bool set_nss_for_name(
ldns_resolver *res, ldns_dnssec_zone *referrals, ldns_rdf *name,
ldns_resolver *local_res, ldns_rr_class c)
{
ldns_dnssec_rrsets *nss = NULL;
ldns_dnssec_rrs *nss_rrs;
ldns_dnssec_rrsets *as = NULL;
ldns_dnssec_rrs *as_rrs;
ldns_rdf *lookup = ldns_rdf_clone(name);
ldns_rdf *new_lookup;
ldns_rdf *addr;
ldns_rr_list *addrs;


for (;;) {
nss = ldns_dnssec_zone_find_rrset(
referrals, lookup, LDNS_RR_TYPE_NS);
if (nss != NULL) {
ldns_rdf_deep_free(lookup);
break;
}
new_lookup = ldns_dname_left_chop(lookup);
ldns_rdf_deep_free(lookup);
lookup = new_lookup;
if (!lookup) {
error("No referrals for name found");
return false;
}
}


while ((addr = ldns_resolver_pop_nameserver(res)))
ldns_rdf_deep_free(addr);


for (nss_rrs = nss->rrs; nss_rrs; nss_rrs = nss_rrs->next) {

if ((as = ldns_dnssec_zone_find_rrset(
referrals, ldns_rr_rdf(nss_rrs->rr, 0), LDNS_RR_TYPE_A)))
for (as_rrs = as->rrs; as_rrs; as_rrs = as_rrs->next)
(void) ldns_resolver_push_nameserver(
res, ldns_rr_rdf(as_rrs->rr, 0));

if ((as = ldns_dnssec_zone_find_rrset(
referrals, ldns_rr_rdf(nss_rrs->rr, 0), LDNS_RR_TYPE_AAAA)))
for (as_rrs = as->rrs; as_rrs; as_rrs = as_rrs->next)
(void) ldns_resolver_push_nameserver(
res, ldns_rr_rdf(as_rrs->rr, 0));
}

if (ldns_resolver_nameserver_count(res) > 0)
return true;


addrs = ldns_rr_list_new();
for (nss_rrs = nss->rrs; nss_rrs; nss_rrs = nss_rrs->next) {
ldns_rr_list *addrs_by_name =
ldns_get_rr_list_addr_by_name(
local_res, ldns_rr_rdf(nss_rrs->rr, 0), c, 0);
ldns_rr_list_cat(addrs, addrs_by_name);
ldns_rr_list_free(addrs_by_name);
}

if (ldns_rr_list_rr_count(addrs) == 0)
error("Could not find the nameserver ip addr; abort");

else if (ldns_resolver_push_nameserver_rr_list(res, addrs) !=
LDNS_STATUS_OK)

error("Error adding new nameservers");
else {
ldns_rr_list_deep_free(addrs);
return true;
}
add_rr_list_to_referrals(referrals, addrs);
ldns_rr_list_deep_free(addrs);
return false;
}










void
do_trace(ldns_resolver *local_res, ldns_rdf *name, ldns_rr_type t,
ldns_rr_class c)
{

static uint8_t zero[1] = { 0 };
static const ldns_rdf root_dname = { 1, LDNS_RDF_TYPE_DNAME, &zero };

ldns_resolver *res = NULL;
ldns_pkt *p = NULL;
ldns_rr_list *final_answer;
ldns_rr_list *new_nss;
ldns_rr_list *cname = NULL;
ldns_rr_list *answers = NULL;
uint16_t loop_count;
ldns_status status;
ldns_dnssec_zone* referrals = NULL;
ldns_rdf *addr;

loop_count = 0;
final_answer = NULL;
res = ldns_resolver_new();

if (!res) {
error("Memory allocation failed");
goto cleanup;
}



ldns_resolver_set_ip6(res,
ldns_resolver_ip6(local_res));
ldns_resolver_set_port(res,
ldns_resolver_port(local_res));
ldns_resolver_set_debug(res,
ldns_resolver_debug(local_res));
ldns_resolver_set_dnssec(res,
ldns_resolver_dnssec(local_res));
ldns_resolver_set_fail(res,
ldns_resolver_fail(local_res));
ldns_resolver_set_usevc(res,
ldns_resolver_usevc(local_res));
ldns_resolver_set_random(res,
ldns_resolver_random(local_res));
ldns_resolver_set_source(res,
ldns_resolver_source(local_res));
ldns_resolver_set_recursive(res, false);


status = ldns_resolver_push_nameserver_rr_list(res, global_dns_root);
if (status != LDNS_STATUS_OK) {
fprintf(stderr, "Error adding root servers to resolver: %s\n", ldns_get_errorstr_by_id(status));
ldns_rr_list_print(stdout, global_dns_root);
goto cleanup;
}


status = ldns_resolver_send(&p, res, &root_dname, LDNS_RR_TYPE_NS, c, 0);


if (ldns_pkt_empty(p)) {
warning("No root server information received");
}

if (status == LDNS_STATUS_OK) {
if (!ldns_pkt_empty(p)) {
drill_pkt_print(stdout, local_res, p);
}
referrals = ldns_dnssec_zone_new();
add_referrals(referrals, p);
} else {
error("cannot use local resolver");
goto cleanup;
}
if (! set_nss_for_name(res, referrals, name, local_res, c)) {
goto cleanup;
}
ldns_pkt_free(p);
p = NULL;
status = ldns_resolver_send(&p, res, name, t, c, 0);
while(status == LDNS_STATUS_OK &&
ldns_pkt_reply_type(p) == LDNS_PACKET_REFERRAL) {

if (!p) {

goto cleanup;
}
add_referrals(referrals, p);


drill_pkt_print_footer(stdout, local_res, p);

if (! set_nss_for_name(res, referrals, name, local_res, c)) {
goto cleanup;
}
if (loop_count++ > 20) {

error("Looks like we are looping");
goto cleanup;
}
ldns_pkt_free(p);
p = NULL;
status = ldns_resolver_send(&p, res, name, t, c, 0);


if (status != LDNS_STATUS_OK)
break;


if (ldns_pkt_reply_type(p) != LDNS_PACKET_ANSWER)
continue;


answers = ldns_pkt_rr_list_by_type(p, t, LDNS_SECTION_ANSWER);
if (answers && ldns_rr_list_rr_count(answers) > 0) {
ldns_rr_list_free(answers);
answers = NULL;
break;
}
ldns_rr_list_free(answers);
answers = NULL;


cname = ldns_pkt_rr_list_by_type(
p, LDNS_RR_TYPE_CNAME, LDNS_SECTION_ANSWER);


if (ldns_rr_list_rr_count(cname) == 0)
break;


ldns_rr_list_print(stdout, cname);


name = ldns_rr_rdf(ldns_rr_list_rr(cname, 0), 0);
ldns_rr_list_free(cname);
cname = NULL;


while((addr = ldns_resolver_pop_nameserver(res)))
ldns_rdf_deep_free(addr);


(void) ldns_resolver_push_nameserver_rr_list(
res, global_dns_root);

ldns_pkt_free(p);
p = NULL;
status = ldns_resolver_send(&p, res, name, t, c, 0);
}

ldns_pkt_free(p);
p = NULL;
status = ldns_resolver_send(&p, res, name, t, c, 0);
if (!p) {
goto cleanup;
}
new_nss = ldns_pkt_authority(p);
final_answer = ldns_pkt_answer(p);

if (verbosity != -1) {
ldns_rr_list_print(stdout, final_answer);
ldns_rr_list_print(stdout, new_nss);

}
drill_pkt_print_footer(stdout, local_res, p);
cleanup:
if (res) {
while((addr = ldns_resolver_pop_nameserver(res)))
ldns_rdf_deep_free(addr);
ldns_resolver_free(res);
}
if (referrals)
ldns_dnssec_zone_deep_free(referrals);
if (p)
ldns_pkt_free(p);
}










#if defined(HAVE_SSL)
ldns_status
do_chase(ldns_resolver *res,
ldns_rdf *name,
ldns_rr_type type,
ldns_rr_class c,
ldns_rr_list *trusted_keys,
ldns_pkt *pkt_o,
uint16_t qflags,
ldns_rr_list * ATTR_UNUSED(prev_key_list))
{
ldns_rr_list *rrset = NULL;
ldns_status result;
ldns_rr *orig_rr = NULL;







ldns_pkt *pkt;
ldns_status tree_result;
ldns_dnssec_data_chain *chain;
ldns_dnssec_trust_tree *tree;

const ldns_rr_descriptor *descriptor;
descriptor = ldns_rr_descript(type);

ldns_dname2canonical(name);

pkt = ldns_pkt_clone(pkt_o);
if (!name) {
mesg("No name to chase");
ldns_pkt_free(pkt);
return LDNS_STATUS_EMPTY_LABEL;
}
if (verbosity != -1) {
printf(";; Chasing: ");
ldns_rdf_print(stdout, name);
if (descriptor && descriptor->_name) {
printf(" %s\n", descriptor->_name);
} else {
printf(" type %d\n", type);
}
}

if (!trusted_keys || ldns_rr_list_rr_count(trusted_keys) < 1) {
warning("No trusted keys specified");
}

if (pkt) {
rrset = ldns_pkt_rr_list_by_name_and_type(pkt,
name,
type,
LDNS_SECTION_ANSWER
);
if (!rrset) {

rrset = ldns_pkt_rr_list_by_name_and_type(pkt,
name,
type,
LDNS_SECTION_AUTHORITY
);
}


if (!rrset) {
rrset = ldns_pkt_rr_list_by_name_and_type(pkt,
name,
LDNS_RR_TYPE_CNAME,
LDNS_SECTION_ANSWER
);
if (!rrset) {

rrset = ldns_pkt_rr_list_by_name_and_type(pkt,
name,
LDNS_RR_TYPE_CNAME,
LDNS_SECTION_AUTHORITY
);
}
}
} else {

if (verbosity >= 0) {
fprintf(stderr, "%s", ldns_get_errorstr_by_id(LDNS_STATUS_MEM_ERR));
fprintf(stderr, "\n");
}
return LDNS_STATUS_MEM_ERR;
}

if (!rrset) {

ldns_pkt_free(pkt);
pkt = NULL;
pkt = ldns_resolver_query(res, name, type, c, qflags);

if (!pkt) {
if (verbosity >= 0) {
fprintf(stderr, "%s", ldns_get_errorstr_by_id(LDNS_STATUS_NETWORK_ERR));
fprintf(stderr, "\n");
}
return LDNS_STATUS_NETWORK_ERR;
}
if (verbosity >= 5) {
ldns_pkt_print(stdout, pkt);
}

rrset = ldns_pkt_rr_list_by_name_and_type(pkt,
name,
type,
LDNS_SECTION_ANSWER
);
}

orig_rr = ldns_rr_new();



if (ldns_pkt_ancount(pkt) < 1) {
ldns_rr_set_type(orig_rr, type);
ldns_rr_set_owner(orig_rr, ldns_rdf_clone(name));

chain = ldns_dnssec_build_data_chain(res, qflags, rrset, pkt, ldns_rr_clone(orig_rr));
} else {

chain = ldns_dnssec_build_data_chain(res, qflags, rrset, pkt, NULL);
}

if (verbosity >= 4) {
printf("\n\nDNSSEC Data Chain:\n");
ldns_dnssec_data_chain_print(stdout, chain);
}

result = LDNS_STATUS_OK;

tree = ldns_dnssec_derive_trust_tree(chain, NULL);

if (verbosity >= 2) {
printf("\n\nDNSSEC Trust tree:\n");
ldns_dnssec_trust_tree_print(stdout, tree, 0, true);
}

if (ldns_rr_list_rr_count(trusted_keys) > 0) {
tree_result = ldns_dnssec_trust_tree_contains_keys(tree, trusted_keys);

if (tree_result == LDNS_STATUS_DNSSEC_EXISTENCE_DENIED) {
if (verbosity >= 1) {
printf("Existence denied or verifiably insecure\n");
}
result = LDNS_STATUS_OK;
} else if (tree_result != LDNS_STATUS_OK) {
if (verbosity >= 1) {
printf("No trusted keys found in tree: first error was: %s\n", ldns_get_errorstr_by_id(tree_result));
}
result = tree_result;
}

} else {
if (verbosity >= 0) {
printf("You have not provided any trusted keys.\n");
}
}

ldns_rr_free(orig_rr);
ldns_dnssec_trust_tree_free(tree);
ldns_dnssec_data_chain_deep_free(chain);

ldns_rr_list_deep_free(rrset);
ldns_pkt_free(pkt);


return result;
}
#endif

