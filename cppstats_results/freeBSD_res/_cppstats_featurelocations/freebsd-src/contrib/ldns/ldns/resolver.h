


















#if !defined(LDNS_RESOLVER_H)
#define LDNS_RESOLVER_H

#include <ldns/error.h>
#include <ldns/common.h>
#include <ldns/rr.h>
#include <ldns/tsig.h>
#include <ldns/rdata.h>
#include <ldns/packet.h>
#include <sys/time.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define LDNS_RESOLV_CONF "/etc/resolv.conf"

#define LDNS_RESOLV_HOSTS "/etc/hosts"

#define LDNS_RESOLV_KEYWORD -1
#define LDNS_RESOLV_DEFDOMAIN 0
#define LDNS_RESOLV_NAMESERVER 1
#define LDNS_RESOLV_SEARCH 2
#define LDNS_RESOLV_SORTLIST 3
#define LDNS_RESOLV_OPTIONS 4
#define LDNS_RESOLV_ANCHOR 5
#define LDNS_RESOLV_KEYWORDS 6

#define LDNS_RESOLV_INETANY 0
#define LDNS_RESOLV_INET 1
#define LDNS_RESOLV_INET6 2

#define LDNS_RESOLV_RTT_INF 0
#define LDNS_RESOLV_RTT_MIN 1




struct ldns_struct_resolver
{

uint16_t _port;


ldns_rdf **_nameservers;

size_t _nameserver_count;


size_t *_rtt;


bool _recursive;


bool _debug;


ldns_rdf *_domain;


ldns_rdf **_searchlist;


size_t _searchlist_count;


uint8_t _retry;

uint8_t _retrans;

bool _fallback;


bool _dnssec;

bool _dnssec_cd;

ldns_rr_list * _dnssec_anchors;

bool _usevc;

bool _igntc;

uint8_t _ip6;

bool _defnames;

bool _dnsrch;

struct timeval _timeout;

bool _fail;

bool _random;

int _socket;



int _axfr_soa_count;




ldns_pkt *_cur_axfr_pkt;

uint16_t _axfr_i;

uint16_t _edns_udp_size;

uint32_t _serial;





char *_tsig_keyname;

char *_tsig_keydata;

char *_tsig_algorithm;


ldns_rdf *_source;
};
typedef struct ldns_struct_resolver ldns_resolver;









uint16_t ldns_resolver_port(const ldns_resolver *r);






ldns_rdf *ldns_resolver_source(const ldns_resolver *r);






bool ldns_resolver_recursive(const ldns_resolver *r);






bool ldns_resolver_debug(const ldns_resolver *r);






uint8_t ldns_resolver_retry(const ldns_resolver *r);






uint8_t ldns_resolver_retrans(const ldns_resolver *r);






bool ldns_resolver_fallback(const ldns_resolver *r);






uint8_t ldns_resolver_ip6(const ldns_resolver *r);






uint16_t ldns_resolver_edns_udp_size(const ldns_resolver *r);





bool ldns_resolver_usevc(const ldns_resolver *r);





bool ldns_resolver_fail(const ldns_resolver *r);





bool ldns_resolver_defnames(const ldns_resolver *r);





bool ldns_resolver_dnsrch(const ldns_resolver *r);





bool ldns_resolver_dnssec(const ldns_resolver *r);





bool ldns_resolver_dnssec_cd(const ldns_resolver *r);





ldns_rr_list * ldns_resolver_dnssec_anchors(const ldns_resolver *r);





bool ldns_resolver_igntc(const ldns_resolver *r);





bool ldns_resolver_random(const ldns_resolver *r);





size_t ldns_resolver_nameserver_count(const ldns_resolver *r);





ldns_rdf *ldns_resolver_domain(const ldns_resolver *r);





struct timeval ldns_resolver_timeout(const ldns_resolver *r);





ldns_rdf** ldns_resolver_searchlist(const ldns_resolver *r);





ldns_rdf** ldns_resolver_nameservers(const ldns_resolver *r);






size_t * ldns_resolver_rtt(const ldns_resolver *r);






size_t ldns_resolver_nameserver_rtt(const ldns_resolver *r, size_t pos);






const char *ldns_resolver_tsig_keyname(const ldns_resolver *r);






const char *ldns_resolver_tsig_algorithm(const ldns_resolver *r);






const char *ldns_resolver_tsig_keydata(const ldns_resolver *r);





ldns_rdf* ldns_resolver_pop_nameserver(ldns_resolver *r);






size_t ldns_resolver_searchlist_count(const ldns_resolver *r);







void ldns_resolver_set_port(ldns_resolver *r, uint16_t p);






void ldns_resolver_set_source(ldns_resolver *r, ldns_rdf *s);






void ldns_resolver_set_recursive(ldns_resolver *r, bool b);






void ldns_resolver_set_debug(ldns_resolver *r, bool b);





void ldns_resolver_incr_nameserver_count(ldns_resolver *r);





void ldns_resolver_dec_nameserver_count(ldns_resolver *r);






void ldns_resolver_set_nameserver_count(ldns_resolver *r, size_t c);






void ldns_resolver_set_nameservers(ldns_resolver *r, ldns_rdf **rd);







void ldns_resolver_set_domain(ldns_resolver *r, ldns_rdf *rd);






void ldns_resolver_set_timeout(ldns_resolver *r, struct timeval timeout);






void ldns_resolver_push_searchlist(ldns_resolver *r, ldns_rdf *rd);






void ldns_resolver_set_defnames(ldns_resolver *r, bool b);






void ldns_resolver_set_usevc(ldns_resolver *r, bool b);






void ldns_resolver_set_dnsrch(ldns_resolver *r, bool b);






void ldns_resolver_set_dnssec(ldns_resolver *r, bool b);






void ldns_resolver_set_dnssec_cd(ldns_resolver *r, bool b);





void ldns_resolver_set_dnssec_anchors(ldns_resolver *r, ldns_rr_list * l);







ldns_status ldns_resolver_push_dnssec_anchor(ldns_resolver *r, ldns_rr *rr);






void ldns_resolver_set_retrans(ldns_resolver *r, uint8_t re);







void ldns_resolver_set_fallback(ldns_resolver *r, bool fallback);







void ldns_resolver_set_retry(ldns_resolver *r, uint8_t re);






void ldns_resolver_set_ip6(ldns_resolver *r, uint8_t i);






void ldns_resolver_set_fail(ldns_resolver *r, bool b);






void ldns_resolver_set_igntc(ldns_resolver *r, bool b);






void ldns_resolver_set_edns_udp_size(ldns_resolver *r, uint16_t s);






void ldns_resolver_set_tsig_keyname(ldns_resolver *r, const char *tsig_keyname);






void ldns_resolver_set_tsig_algorithm(ldns_resolver *r, const char *tsig_algorithm);






void ldns_resolver_set_tsig_keydata(ldns_resolver *r, const char *tsig_keydata);







void ldns_resolver_set_rtt(ldns_resolver *r, size_t *rtt);








void ldns_resolver_set_nameserver_rtt(ldns_resolver *r, size_t pos, size_t value);






void ldns_resolver_set_random(ldns_resolver *r, bool b);








ldns_status ldns_resolver_push_nameserver(ldns_resolver *r, const ldns_rdf *n);








ldns_status ldns_resolver_push_nameserver_rr(ldns_resolver *r, const ldns_rr *rr);







ldns_status ldns_resolver_push_nameserver_rr_list(ldns_resolver *r, const ldns_rr_list *rrlist);













ldns_pkt* ldns_resolver_search(const ldns_resolver *r, const ldns_rdf *rdf, ldns_rr_type t, ldns_rr_class c, uint16_t flags);















ldns_status ldns_resolver_search_status(ldns_pkt** pkt, ldns_resolver *r, const ldns_rdf *rdf, ldns_rr_type t, ldns_rr_class c, uint16_t flags);












ldns_status ldns_resolver_prepare_query_pkt(ldns_pkt **q, ldns_resolver *r, const ldns_rdf *name, ldns_rr_type t, ldns_rr_class c, uint16_t f);












ldns_status ldns_resolver_send(ldns_pkt **answer, ldns_resolver *r, const ldns_rdf *name, ldns_rr_type t, ldns_rr_class c, uint16_t flags);







ldns_status ldns_resolver_send_pkt(ldns_pkt **answer, ldns_resolver *r, ldns_pkt *query_pkt);













ldns_status ldns_resolver_query_status(ldns_pkt** pkt, ldns_resolver *r, const ldns_rdf *name, ldns_rr_type t, ldns_rr_class c, uint16_t flags);















ldns_pkt* ldns_resolver_query(const ldns_resolver *r, const ldns_rdf *name, ldns_rr_type t, ldns_rr_class c, uint16_t flags);






ldns_resolver* ldns_resolver_new(void);






ldns_resolver* ldns_resolver_clone(ldns_resolver *r);








ldns_status ldns_resolver_new_frm_fp(ldns_resolver **r, FILE *fp);









ldns_status ldns_resolver_new_frm_fp_l(ldns_resolver **r, FILE *fp, int *line_nr);









ldns_status ldns_resolver_new_frm_file(ldns_resolver **r, const char *filename);





void ldns_resolver_free(ldns_resolver *res);





void ldns_resolver_deep_free(ldns_resolver *res);









ldns_rr* ldns_axfr_next(ldns_resolver *resolver);





void ldns_axfr_abort(ldns_resolver *resolver);






bool ldns_axfr_complete(const ldns_resolver *resolver);







ldns_pkt *ldns_axfr_last_pkt(const ldns_resolver *res);






void ldns_resolver_set_ixfr_serial(ldns_resolver *r, uint32_t serial);






uint32_t ldns_resolver_get_ixfr_serial(const ldns_resolver *res);





void ldns_resolver_nameservers_randomize(ldns_resolver *r);








bool ldns_resolver_trusted_key(const ldns_resolver *r, ldns_rr_list * keys, ldns_rr_list * trusted_keys);

#if defined(__cplusplus)
}
#endif

#endif
