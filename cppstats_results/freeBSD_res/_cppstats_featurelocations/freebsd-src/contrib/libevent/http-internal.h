








#if !defined(HTTP_INTERNAL_H_INCLUDED_)
#define HTTP_INTERNAL_H_INCLUDED_

#include "event2/event_struct.h"
#include "util-internal.h"
#include "defer-internal.h"

#define HTTP_CONNECT_TIMEOUT 45
#define HTTP_WRITE_TIMEOUT 50
#define HTTP_READ_TIMEOUT 50

#define HTTP_PREFIX "http://"
#define HTTP_DEFAULTPORT 80

enum message_read_status {
ALL_DATA_READ = 1,
MORE_DATA_EXPECTED = 0,
DATA_CORRUPTED = -1,
REQUEST_CANCELED = -2,
DATA_TOO_LONG = -3
};

struct evbuffer;
struct addrinfo;
struct evhttp_request;


#define EVHTTP_REQ_UNKNOWN_ (1<<15)

enum evhttp_connection_state {
EVCON_DISCONNECTED,
EVCON_CONNECTING,
EVCON_IDLE,
EVCON_READING_FIRSTLINE,

EVCON_READING_HEADERS,
EVCON_READING_BODY,
EVCON_READING_TRAILER,
EVCON_WRITING
};

struct event_base;


struct evhttp_connection {


TAILQ_ENTRY(evhttp_connection) next;

evutil_socket_t fd;
struct bufferevent *bufev;

struct event retry_ev;

char *bind_address;
ev_uint16_t bind_port;

char *address;
ev_uint16_t port;

size_t max_headers_size;
ev_uint64_t max_body_size;

int flags;
#define EVHTTP_CON_INCOMING 0x0001
#define EVHTTP_CON_OUTGOING 0x0002
#define EVHTTP_CON_CLOSEDETECT 0x0004

#define EVHTTP_CON_AUTOFREE EVHTTP_CON_PUBLIC_FLAGS_END


#define EVHTTP_CON_READING_ERROR (EVHTTP_CON_AUTOFREE << 1)

struct timeval timeout;
int retry_cnt;
int retry_max;
struct timeval initial_retry_timeout;



enum evhttp_connection_state state;


struct evhttp *http_server;

TAILQ_HEAD(evcon_requestq, evhttp_request) requests;

void (*cb)(struct evhttp_connection *, void *);
void *cb_arg;

void (*closecb)(struct evhttp_connection *, void *);
void *closecb_arg;

struct event_callback read_more_deferred_cb;

struct event_base *base;
struct evdns_base *dns_base;
int ai_family;
};


struct evhttp_cb {
TAILQ_ENTRY(evhttp_cb) next;

char *what;

void (*cb)(struct evhttp_request *req, void *);
void *cbarg;
};


TAILQ_HEAD(evconq, evhttp_connection);


struct evhttp_bound_socket {
TAILQ_ENTRY(evhttp_bound_socket) next;

struct evconnlistener *listener;
};


struct evhttp_server_alias {
TAILQ_ENTRY(evhttp_server_alias) next;

char *alias;
};

struct evhttp {

TAILQ_ENTRY(evhttp) next_vhost;


TAILQ_HEAD(boundq, evhttp_bound_socket) sockets;

TAILQ_HEAD(httpcbq, evhttp_cb) callbacks;


struct evconq connections;

TAILQ_HEAD(vhostsq, evhttp) virtualhosts;

TAILQ_HEAD(aliasq, evhttp_server_alias) aliases;


char *vhost_pattern;

struct timeval timeout;

size_t default_max_headers_size;
ev_uint64_t default_max_body_size;
int flags;
const char *default_content_type;



ev_uint16_t allowed_methods;



void (*gencb)(struct evhttp_request *req, void *);
void *gencbarg;
struct bufferevent* (*bevcb)(struct event_base *, void *);
void *bevcbarg;

struct event_base *base;
};




void evhttp_connection_reset_(struct evhttp_connection *);


int evhttp_connection_connect_(struct evhttp_connection *);

enum evhttp_request_error;

void evhttp_connection_fail_(struct evhttp_connection *,
enum evhttp_request_error error);

enum message_read_status;

enum message_read_status evhttp_parse_firstline_(struct evhttp_request *, struct evbuffer*);
enum message_read_status evhttp_parse_headers_(struct evhttp_request *, struct evbuffer*);

void evhttp_start_read_(struct evhttp_connection *);
void evhttp_start_write_(struct evhttp_connection *);


void evhttp_response_code_(struct evhttp_request *, int, const char *);
void evhttp_send_page_(struct evhttp_request *, struct evbuffer *);

int evhttp_decode_uri_internal(const char *uri, size_t length,
char *ret, int decode_plus);

#endif
