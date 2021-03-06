

























#if !defined(EVENT2_HTTP_H_INCLUDED_)
#define EVENT2_HTTP_H_INCLUDED_


#include <event2/util.h>
#include <event2/visibility.h>

#if defined(__cplusplus)
extern "C" {
#endif


struct evbuffer;
struct event_base;
struct bufferevent;
struct evhttp_connection;













#define HTTP_OK 200
#define HTTP_NOCONTENT 204
#define HTTP_MOVEPERM 301
#define HTTP_MOVETEMP 302
#define HTTP_NOTMODIFIED 304
#define HTTP_BADREQUEST 400
#define HTTP_NOTFOUND 404
#define HTTP_BADMETHOD 405
#define HTTP_ENTITYTOOLARGE 413
#define HTTP_EXPECTATIONFAILED 417
#define HTTP_INTERNAL 500
#define HTTP_NOTIMPLEMENTED 501
#define HTTP_SERVUNAVAIL 503

struct evhttp;
struct evhttp_request;
struct evkeyvalq;
struct evhttp_bound_socket;
struct evconnlistener;
struct evdns_base;








EVENT2_EXPORT_SYMBOL
struct evhttp *evhttp_new(struct event_base *base);













EVENT2_EXPORT_SYMBOL
int evhttp_bind_socket(struct evhttp *http, const char *address, ev_uint16_t port);












EVENT2_EXPORT_SYMBOL
struct evhttp_bound_socket *evhttp_bind_socket_with_handle(struct evhttp *http, const char *address, ev_uint16_t port);

















EVENT2_EXPORT_SYMBOL
int evhttp_accept_socket(struct evhttp *http, evutil_socket_t fd);











EVENT2_EXPORT_SYMBOL
struct evhttp_bound_socket *evhttp_accept_socket_with_handle(struct evhttp *http, evutil_socket_t fd);






EVENT2_EXPORT_SYMBOL
struct evhttp_bound_socket *evhttp_bind_listener(struct evhttp *http, struct evconnlistener *listener);




EVENT2_EXPORT_SYMBOL
struct evconnlistener *evhttp_bound_socket_get_listener(struct evhttp_bound_socket *bound);

typedef void evhttp_bound_socket_foreach_fn(struct evhttp_bound_socket *, void *);










EVENT2_EXPORT_SYMBOL
void evhttp_foreach_bound_socket(struct evhttp *http, evhttp_bound_socket_foreach_fn *function, void *argument);


















EVENT2_EXPORT_SYMBOL
void evhttp_del_accept_socket(struct evhttp *http, struct evhttp_bound_socket *bound_socket);








EVENT2_EXPORT_SYMBOL
evutil_socket_t evhttp_bound_socket_get_fd(struct evhttp_bound_socket *bound_socket);









EVENT2_EXPORT_SYMBOL
void evhttp_free(struct evhttp* http);


EVENT2_EXPORT_SYMBOL
void evhttp_set_max_headers_size(struct evhttp* http, ev_ssize_t max_headers_size);

EVENT2_EXPORT_SYMBOL
void evhttp_set_max_body_size(struct evhttp* http, ev_ssize_t max_body_size);









EVENT2_EXPORT_SYMBOL
void evhttp_set_default_content_type(struct evhttp *http,
const char *content_type);












EVENT2_EXPORT_SYMBOL
void evhttp_set_allowed_methods(struct evhttp* http, ev_uint16_t methods);










EVENT2_EXPORT_SYMBOL
int evhttp_set_cb(struct evhttp *http, const char *path,
void (*cb)(struct evhttp_request *, void *), void *cb_arg);


EVENT2_EXPORT_SYMBOL
int evhttp_del_cb(struct evhttp *, const char *);












EVENT2_EXPORT_SYMBOL
void evhttp_set_gencb(struct evhttp *http,
void (*cb)(struct evhttp_request *, void *), void *arg);















EVENT2_EXPORT_SYMBOL
void evhttp_set_bevcb(struct evhttp *http,
struct bufferevent *(*cb)(struct event_base *, void *), void *arg);























EVENT2_EXPORT_SYMBOL
int evhttp_add_virtual_host(struct evhttp* http, const char *pattern,
struct evhttp* vhost);









EVENT2_EXPORT_SYMBOL
int evhttp_remove_virtual_host(struct evhttp* http, struct evhttp* vhost);









EVENT2_EXPORT_SYMBOL
int evhttp_add_server_alias(struct evhttp *http, const char *alias);








EVENT2_EXPORT_SYMBOL
int evhttp_remove_server_alias(struct evhttp *http, const char *alias);







EVENT2_EXPORT_SYMBOL
void evhttp_set_timeout(struct evhttp *http, int timeout_in_secs);







EVENT2_EXPORT_SYMBOL
void evhttp_set_timeout_tv(struct evhttp *http, const struct timeval* tv);



#define EVHTTP_SERVER_LINGERING_CLOSE 0x0001







EVENT2_EXPORT_SYMBOL
int evhttp_set_flags(struct evhttp *http, int flags);











EVENT2_EXPORT_SYMBOL
void evhttp_send_error(struct evhttp_request *req, int error,
const char *reason);














EVENT2_EXPORT_SYMBOL
void evhttp_send_reply(struct evhttp_request *req, int code,
const char *reason, struct evbuffer *databuf);

















EVENT2_EXPORT_SYMBOL
void evhttp_send_reply_start(struct evhttp_request *req, int code,
const char *reason);












EVENT2_EXPORT_SYMBOL
void evhttp_send_reply_chunk(struct evhttp_request *req,
struct evbuffer *databuf);














EVENT2_EXPORT_SYMBOL
void evhttp_send_reply_chunk_with_cb(struct evhttp_request *, struct evbuffer *,
void (*cb)(struct evhttp_connection *, void *), void *arg);






EVENT2_EXPORT_SYMBOL
void evhttp_send_reply_end(struct evhttp_request *req);












enum evhttp_cmd_type {
EVHTTP_REQ_GET = 1 << 0,
EVHTTP_REQ_POST = 1 << 1,
EVHTTP_REQ_HEAD = 1 << 2,
EVHTTP_REQ_PUT = 1 << 3,
EVHTTP_REQ_DELETE = 1 << 4,
EVHTTP_REQ_OPTIONS = 1 << 5,
EVHTTP_REQ_TRACE = 1 << 6,
EVHTTP_REQ_CONNECT = 1 << 7,
EVHTTP_REQ_PATCH = 1 << 8
};


enum evhttp_request_kind { EVHTTP_REQUEST, EVHTTP_RESPONSE };
















EVENT2_EXPORT_SYMBOL
struct evhttp_connection *evhttp_connection_base_bufferevent_new(
struct event_base *base, struct evdns_base *dnsbase, struct bufferevent* bev, const char *address, ev_uint16_t port);




EVENT2_EXPORT_SYMBOL
struct bufferevent* evhttp_connection_get_bufferevent(struct evhttp_connection *evcon);




EVENT2_EXPORT_SYMBOL
struct evhttp *evhttp_connection_get_server(struct evhttp_connection *evcon);






EVENT2_EXPORT_SYMBOL
struct evhttp_request *evhttp_request_new(
void (*cb)(struct evhttp_request *, void *), void *arg);








EVENT2_EXPORT_SYMBOL
void evhttp_request_set_chunked_cb(struct evhttp_request *,
void (*cb)(struct evhttp_request *, void *));







EVENT2_EXPORT_SYMBOL
void evhttp_request_set_header_cb(struct evhttp_request *,
int (*cb)(struct evhttp_request *, void *));






enum evhttp_request_error {



EVREQ_HTTP_TIMEOUT,



EVREQ_HTTP_EOF,



EVREQ_HTTP_INVALID_HEADER,



EVREQ_HTTP_BUFFER_ERROR,



EVREQ_HTTP_REQUEST_CANCEL,



EVREQ_HTTP_DATA_TOO_LONG
};







EVENT2_EXPORT_SYMBOL
void evhttp_request_set_error_cb(struct evhttp_request *,
void (*)(enum evhttp_request_error, void *));













EVENT2_EXPORT_SYMBOL
void evhttp_request_set_on_complete_cb(struct evhttp_request *req,
void (*cb)(struct evhttp_request *, void *), void *cb_arg);


EVENT2_EXPORT_SYMBOL
void evhttp_request_free(struct evhttp_request *req);













EVENT2_EXPORT_SYMBOL
struct evhttp_connection *evhttp_connection_base_new(
struct event_base *base, struct evdns_base *dnsbase,
const char *address, ev_uint16_t port);




EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_family(struct evhttp_connection *evcon,
int family);


#define EVHTTP_CON_REUSE_CONNECTED_ADDR 0x0008



#define EVHTTP_CON_READ_ON_WRITE_ERROR 0x0010

#define EVHTTP_CON_LINGERING_CLOSE 0x0020

#define EVHTTP_CON_PUBLIC_FLAGS_END 0x100000







EVENT2_EXPORT_SYMBOL
int evhttp_connection_set_flags(struct evhttp_connection *evcon,
int flags);






EVENT2_EXPORT_SYMBOL
void evhttp_request_own(struct evhttp_request *req);


EVENT2_EXPORT_SYMBOL
int evhttp_request_is_owned(struct evhttp_request *req);







EVENT2_EXPORT_SYMBOL
struct evhttp_connection *evhttp_request_get_connection(struct evhttp_request *req);




EVENT2_EXPORT_SYMBOL
struct event_base *evhttp_connection_get_base(struct evhttp_connection *req);

EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_max_headers_size(struct evhttp_connection *evcon,
ev_ssize_t new_max_headers_size);

EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_max_body_size(struct evhttp_connection* evcon,
ev_ssize_t new_max_body_size);


EVENT2_EXPORT_SYMBOL
void evhttp_connection_free(struct evhttp_connection *evcon);






EVENT2_EXPORT_SYMBOL
void evhttp_connection_free_on_completion(struct evhttp_connection *evcon);


EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_local_address(struct evhttp_connection *evcon,
const char *address);


EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_local_port(struct evhttp_connection *evcon,
ev_uint16_t port);


EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_timeout(struct evhttp_connection *evcon,
int timeout_in_secs);



EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_timeout_tv(struct evhttp_connection *evcon,
const struct timeval *tv);





EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_initial_retry_tv(struct evhttp_connection *evcon,
const struct timeval *tv);


EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_retries(struct evhttp_connection *evcon,
int retry_max);


EVENT2_EXPORT_SYMBOL
void evhttp_connection_set_closecb(struct evhttp_connection *evcon,
void (*)(struct evhttp_connection *, void *), void *);


EVENT2_EXPORT_SYMBOL
void evhttp_connection_get_peer(struct evhttp_connection *evcon,
char **address, ev_uint16_t *port);







EVENT2_EXPORT_SYMBOL
const struct sockaddr*
evhttp_connection_get_addr(struct evhttp_connection *evcon);














EVENT2_EXPORT_SYMBOL
int evhttp_make_request(struct evhttp_connection *evcon,
struct evhttp_request *req,
enum evhttp_cmd_type type, const char *uri);














EVENT2_EXPORT_SYMBOL
void evhttp_cancel_request(struct evhttp_request *req);




struct evhttp_uri;


EVENT2_EXPORT_SYMBOL
const char *evhttp_request_get_uri(const struct evhttp_request *req);

EVENT2_EXPORT_SYMBOL
const struct evhttp_uri *evhttp_request_get_evhttp_uri(const struct evhttp_request *req);

EVENT2_EXPORT_SYMBOL
enum evhttp_cmd_type evhttp_request_get_command(const struct evhttp_request *req);

EVENT2_EXPORT_SYMBOL
int evhttp_request_get_response_code(const struct evhttp_request *req);
EVENT2_EXPORT_SYMBOL
const char * evhttp_request_get_response_code_line(const struct evhttp_request *req);


EVENT2_EXPORT_SYMBOL
struct evkeyvalq *evhttp_request_get_input_headers(struct evhttp_request *req);

EVENT2_EXPORT_SYMBOL
struct evkeyvalq *evhttp_request_get_output_headers(struct evhttp_request *req);

EVENT2_EXPORT_SYMBOL
struct evbuffer *evhttp_request_get_input_buffer(struct evhttp_request *req);

EVENT2_EXPORT_SYMBOL
struct evbuffer *evhttp_request_get_output_buffer(struct evhttp_request *req);




EVENT2_EXPORT_SYMBOL
const char *evhttp_request_get_host(struct evhttp_request *req);












EVENT2_EXPORT_SYMBOL
const char *evhttp_find_header(const struct evkeyvalq *headers,
const char *key);









EVENT2_EXPORT_SYMBOL
int evhttp_remove_header(struct evkeyvalq *headers, const char *key);










EVENT2_EXPORT_SYMBOL
int evhttp_add_header(struct evkeyvalq *headers, const char *key, const char *value);






EVENT2_EXPORT_SYMBOL
void evhttp_clear_headers(struct evkeyvalq *headers);















EVENT2_EXPORT_SYMBOL
char *evhttp_encode_uri(const char *str);















EVENT2_EXPORT_SYMBOL
char *evhttp_uriencode(const char *str, ev_ssize_t size, int space_to_plus);















EVENT2_EXPORT_SYMBOL
char *evhttp_decode_uri(const char *uri);
















EVENT2_EXPORT_SYMBOL
char *evhttp_uridecode(const char *uri, int decode_plus,
size_t *size_out);




















EVENT2_EXPORT_SYMBOL
int evhttp_parse_query(const char *uri, struct evkeyvalq *headers);


















EVENT2_EXPORT_SYMBOL
int evhttp_parse_query_str(const char *uri, struct evkeyvalq *headers);












EVENT2_EXPORT_SYMBOL
char *evhttp_htmlescape(const char *html);




EVENT2_EXPORT_SYMBOL
struct evhttp_uri *evhttp_uri_new(void);





EVENT2_EXPORT_SYMBOL
void evhttp_uri_set_flags(struct evhttp_uri *uri, unsigned flags);



EVENT2_EXPORT_SYMBOL
const char *evhttp_uri_get_scheme(const struct evhttp_uri *uri);




EVENT2_EXPORT_SYMBOL
const char *evhttp_uri_get_userinfo(const struct evhttp_uri *uri);












EVENT2_EXPORT_SYMBOL
const char *evhttp_uri_get_host(const struct evhttp_uri *uri);

EVENT2_EXPORT_SYMBOL
int evhttp_uri_get_port(const struct evhttp_uri *uri);

EVENT2_EXPORT_SYMBOL
const char *evhttp_uri_get_path(const struct evhttp_uri *uri);


EVENT2_EXPORT_SYMBOL
const char *evhttp_uri_get_query(const struct evhttp_uri *uri);


EVENT2_EXPORT_SYMBOL
const char *evhttp_uri_get_fragment(const struct evhttp_uri *uri);



EVENT2_EXPORT_SYMBOL
int evhttp_uri_set_scheme(struct evhttp_uri *uri, const char *scheme);


EVENT2_EXPORT_SYMBOL
int evhttp_uri_set_userinfo(struct evhttp_uri *uri, const char *userinfo);


EVENT2_EXPORT_SYMBOL
int evhttp_uri_set_host(struct evhttp_uri *uri, const char *host);


EVENT2_EXPORT_SYMBOL
int evhttp_uri_set_port(struct evhttp_uri *uri, int port);


EVENT2_EXPORT_SYMBOL
int evhttp_uri_set_path(struct evhttp_uri *uri, const char *path);



EVENT2_EXPORT_SYMBOL
int evhttp_uri_set_query(struct evhttp_uri *uri, const char *query);



EVENT2_EXPORT_SYMBOL
int evhttp_uri_set_fragment(struct evhttp_uri *uri, const char *fragment);



































EVENT2_EXPORT_SYMBOL
struct evhttp_uri *evhttp_uri_parse_with_flags(const char *source_uri,
unsigned flags);













#define EVHTTP_URI_NONCONFORMANT 0x01


EVENT2_EXPORT_SYMBOL
struct evhttp_uri *evhttp_uri_parse(const char *source_uri);








EVENT2_EXPORT_SYMBOL
void evhttp_uri_free(struct evhttp_uri *uri);














EVENT2_EXPORT_SYMBOL
char *evhttp_uri_join(struct evhttp_uri *uri, char *buf, size_t limit);

#if defined(__cplusplus)
}
#endif

#endif
