





































































































































#if !defined(EVENT2_DNS_H_INCLUDED_)
#define EVENT2_DNS_H_INCLUDED_

#include <event2/visibility.h>

#if defined(__cplusplus)
extern "C" {
#endif


#include <event2/util.h>


#define DNS_ERR_NONE 0

#define DNS_ERR_FORMAT 1


#define DNS_ERR_SERVERFAILED 2

#define DNS_ERR_NOTEXIST 3

#define DNS_ERR_NOTIMPL 4


#define DNS_ERR_REFUSED 5

#define DNS_ERR_TRUNCATED 65

#define DNS_ERR_UNKNOWN 66

#define DNS_ERR_TIMEOUT 67

#define DNS_ERR_SHUTDOWN 68

#define DNS_ERR_CANCEL 69



#define DNS_ERR_NODATA 70

#define DNS_IPv4_A 1
#define DNS_PTR 2
#define DNS_IPv6_AAAA 3

#define DNS_QUERY_NO_SEARCH 1

#define DNS_OPTION_SEARCH 1
#define DNS_OPTION_NAMESERVERS 2
#define DNS_OPTION_MISC 4
#define DNS_OPTION_HOSTSFILE 8
#define DNS_OPTIONS_ALL 15


#define DNS_NO_SEARCH DNS_QUERY_NO_SEARCH











typedef void (*evdns_callback_type) (int result, char type, int count, int ttl, void *addresses, void *arg);

struct evdns_base;
struct event_base;


#define EVDNS_BASE_INITIALIZE_NAMESERVERS 1


#define EVDNS_BASE_DISABLE_WHEN_INACTIVE 0x8000














EVENT2_EXPORT_SYMBOL
struct evdns_base * evdns_base_new(struct event_base *event_base, int initialize_nameservers);














EVENT2_EXPORT_SYMBOL
void evdns_base_free(struct evdns_base *base, int fail_requests);







EVENT2_EXPORT_SYMBOL
void evdns_base_clear_host_addresses(struct evdns_base *base);







EVENT2_EXPORT_SYMBOL
const char *evdns_err_to_string(int err);













EVENT2_EXPORT_SYMBOL
int evdns_base_nameserver_add(struct evdns_base *base,
unsigned long int address);













EVENT2_EXPORT_SYMBOL
int evdns_base_count_nameservers(struct evdns_base *base);










EVENT2_EXPORT_SYMBOL
int evdns_base_clear_nameservers_and_suspend(struct evdns_base *base);












EVENT2_EXPORT_SYMBOL
int evdns_base_resume(struct evdns_base *base);


















EVENT2_EXPORT_SYMBOL
int evdns_base_nameserver_ip_add(struct evdns_base *base,
const char *ip_as_string);




EVENT2_EXPORT_SYMBOL
int
evdns_base_nameserver_sockaddr_add(struct evdns_base *base,
const struct sockaddr *sa, ev_socklen_t len, unsigned flags);

struct evdns_request;












EVENT2_EXPORT_SYMBOL
struct evdns_request *evdns_base_resolve_ipv4(struct evdns_base *base, const char *name, int flags, evdns_callback_type callback, void *ptr);












EVENT2_EXPORT_SYMBOL
struct evdns_request *evdns_base_resolve_ipv6(struct evdns_base *base, const char *name, int flags, evdns_callback_type callback, void *ptr);

struct in_addr;
struct in6_addr;












EVENT2_EXPORT_SYMBOL
struct evdns_request *evdns_base_resolve_reverse(struct evdns_base *base, const struct in_addr *in, int flags, evdns_callback_type callback, void *ptr);













EVENT2_EXPORT_SYMBOL
struct evdns_request *evdns_base_resolve_reverse_ipv6(struct evdns_base *base, const struct in6_addr *in, int flags, evdns_callback_type callback, void *ptr);








EVENT2_EXPORT_SYMBOL
void evdns_cancel_request(struct evdns_base *base, struct evdns_request *req);

















EVENT2_EXPORT_SYMBOL
int evdns_base_set_option(struct evdns_base *base, const char *option, const char *val);
























EVENT2_EXPORT_SYMBOL
int evdns_base_resolv_conf_parse(struct evdns_base *base, int flags, const char *const filename);














EVENT2_EXPORT_SYMBOL
int evdns_base_load_hosts(struct evdns_base *base, const char *hosts_fname);











#if defined(_WIN32)
EVENT2_EXPORT_SYMBOL
int evdns_base_config_windows_nameservers(struct evdns_base *);
#define EVDNS_BASE_CONFIG_WINDOWS_NAMESERVERS_IMPLEMENTED
#endif





EVENT2_EXPORT_SYMBOL
void evdns_base_search_clear(struct evdns_base *base);







EVENT2_EXPORT_SYMBOL
void evdns_base_search_add(struct evdns_base *base, const char *domain);










EVENT2_EXPORT_SYMBOL
void evdns_base_search_ndots_set(struct evdns_base *base, const int ndots);







typedef void (*evdns_debug_log_fn_type)(int is_warning, const char *msg);









EVENT2_EXPORT_SYMBOL
void evdns_set_log_fn(evdns_debug_log_fn_type fn);











EVENT2_EXPORT_SYMBOL
void evdns_set_transaction_id_fn(ev_uint16_t (*fn)(void));










EVENT2_EXPORT_SYMBOL
void evdns_set_random_bytes_fn(void (*fn)(char *, size_t));





struct evdns_server_request;
struct evdns_server_question;












typedef void (*evdns_request_callback_fn_type)(struct evdns_server_request *, void *);
#define EVDNS_ANSWER_SECTION 0
#define EVDNS_AUTHORITY_SECTION 1
#define EVDNS_ADDITIONAL_SECTION 2

#define EVDNS_TYPE_A 1
#define EVDNS_TYPE_NS 2
#define EVDNS_TYPE_CNAME 5
#define EVDNS_TYPE_SOA 6
#define EVDNS_TYPE_PTR 12
#define EVDNS_TYPE_MX 15
#define EVDNS_TYPE_TXT 16
#define EVDNS_TYPE_AAAA 28

#define EVDNS_QTYPE_AXFR 252
#define EVDNS_QTYPE_ALL 255

#define EVDNS_CLASS_INET 1


#define EVDNS_FLAGS_AA 0x400
#define EVDNS_FLAGS_RD 0x080











EVENT2_EXPORT_SYMBOL
struct evdns_server_port *evdns_add_server_port_with_base(struct event_base *base, evutil_socket_t socket, int flags, evdns_request_callback_fn_type callback, void *user_data);

EVENT2_EXPORT_SYMBOL
void evdns_close_server_port(struct evdns_server_port *port);




EVENT2_EXPORT_SYMBOL
void evdns_server_request_set_flags(struct evdns_server_request *req, int flags);



EVENT2_EXPORT_SYMBOL
int evdns_server_request_add_reply(struct evdns_server_request *req, int section, const char *name, int type, int dns_class, int ttl, int datalen, int is_name, const char *data);
EVENT2_EXPORT_SYMBOL
int evdns_server_request_add_a_reply(struct evdns_server_request *req, const char *name, int n, const void *addrs, int ttl);
EVENT2_EXPORT_SYMBOL
int evdns_server_request_add_aaaa_reply(struct evdns_server_request *req, const char *name, int n, const void *addrs, int ttl);
EVENT2_EXPORT_SYMBOL
int evdns_server_request_add_ptr_reply(struct evdns_server_request *req, struct in_addr *in, const char *inaddr_name, const char *hostname, int ttl);
EVENT2_EXPORT_SYMBOL
int evdns_server_request_add_cname_reply(struct evdns_server_request *req, const char *name, const char *cname, int ttl);




EVENT2_EXPORT_SYMBOL
int evdns_server_request_respond(struct evdns_server_request *req, int err);



EVENT2_EXPORT_SYMBOL
int evdns_server_request_drop(struct evdns_server_request *req);
struct sockaddr;



EVENT2_EXPORT_SYMBOL
int evdns_server_request_get_requesting_addr(struct evdns_server_request *req, struct sockaddr *sa, int addr_len);


typedef void (*evdns_getaddrinfo_cb)(int result, struct evutil_addrinfo *res, void *arg);

struct evdns_base;
struct evdns_getaddrinfo_request;

















EVENT2_EXPORT_SYMBOL
struct evdns_getaddrinfo_request *evdns_getaddrinfo(
struct evdns_base *dns_base,
const char *nodename, const char *servname,
const struct evutil_addrinfo *hints_in,
evdns_getaddrinfo_cb cb, void *arg);




EVENT2_EXPORT_SYMBOL
void evdns_getaddrinfo_cancel(struct evdns_getaddrinfo_request *req);













EVENT2_EXPORT_SYMBOL
int evdns_base_get_nameserver_addr(struct evdns_base *base, int idx,
struct sockaddr *sa, ev_socklen_t len);

#if defined(__cplusplus)
}
#endif

#endif
