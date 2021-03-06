

























#if !defined(REGRESS_TESTUTILS_H_INCLUDED_)
#define REGRESS_TESTUTILS_H_INCLUDED_

#include "event2/dns.h"

struct regress_dns_server_table {
const char *q;
const char *anstype;
const char *ans;
int seen;
int lower;
};

struct evdns_server_port *
regress_get_dnsserver(struct event_base *base,
ev_uint16_t *portnum,
evutil_socket_t *psock,
evdns_request_callback_fn_type cb,
void *arg);


int regress_get_socket_port(evutil_socket_t fd);


void regress_dns_server_cb(
struct evdns_server_request *req, void *data);


int regress_dnsserver(struct event_base *base, ev_uint16_t *port,
struct regress_dns_server_table *seach_table);


void regress_clean_dnsserver(void);

struct evconnlistener;
struct sockaddr;
int regress_get_listener_addr(struct evconnlistener *lev,
struct sockaddr *sa, ev_socklen_t *socklen);

#endif

