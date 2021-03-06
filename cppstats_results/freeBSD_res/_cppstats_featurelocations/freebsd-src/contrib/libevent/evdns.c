

















































#include "event2/event-config.h"
#include "evconfig-private.h"

#include <sys/types.h>

#if !defined(_FORTIFY_SOURCE)
#define _FORTIFY_SOURCE 3
#endif

#include <string.h>
#include <fcntl.h>
#if defined(EVENT__HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(EVENT__HAVE_STDINT_H)
#include <stdint.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(EVENT__HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#if !defined(_WIN32_IE)
#define _WIN32_IE 0x400
#endif
#include <shlobj.h>
#endif

#include "event2/dns.h"
#include "event2/dns_struct.h"
#include "event2/dns_compat.h"
#include "event2/util.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/thread.h"

#include "defer-internal.h"
#include "log-internal.h"
#include "mm-internal.h"
#include "strlcpy-internal.h"
#include "ipv6-internal.h"
#include "util-internal.h"
#include "evthread-internal.h"
#if defined(_WIN32)
#include <ctype.h>
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#if defined(EVENT__HAVE_NETINET_IN6_H)
#include <netinet/in6.h>
#endif

#define EVDNS_LOG_DEBUG EVENT_LOG_DEBUG
#define EVDNS_LOG_WARN EVENT_LOG_WARN
#define EVDNS_LOG_MSG EVENT_LOG_MSG

#if !defined(HOST_NAME_MAX)
#define HOST_NAME_MAX 255
#endif

#include <stdio.h>

#undef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))

#define ASSERT_VALID_REQUEST(req) EVUTIL_ASSERT((req)->handle && (req)->handle->current_req == (req))


#define u64 ev_uint64_t
#define u32 ev_uint32_t
#define u16 ev_uint16_t
#define u8 ev_uint8_t



#define MAX_V4_ADDRS 32
#define MAX_V6_ADDRS 32


#define TYPE_A EVDNS_TYPE_A
#define TYPE_CNAME 5
#define TYPE_PTR EVDNS_TYPE_PTR
#define TYPE_SOA EVDNS_TYPE_SOA
#define TYPE_AAAA EVDNS_TYPE_AAAA

#define CLASS_INET EVDNS_CLASS_INET





struct evdns_request {
struct request *current_req;
struct evdns_base *base;

int pending_cb;



int search_index;
struct search_state *search_state;
char *search_origname;
int search_flags;
};

struct request {
u8 *request;
u8 request_type;
unsigned int request_len;
int reissue_count;
int tx_count;
void *user_pointer;
evdns_callback_type user_callback;
struct nameserver *ns;



struct request *next, *prev;

struct event timeout_event;

u16 trans_id;
unsigned request_appended :1;
unsigned transmit_me :1;


char **put_cname_in_ptr;

struct evdns_base *base;

struct evdns_request *handle;
};

struct reply {
unsigned int type;
unsigned int have_answer : 1;
union {
struct {
u32 addrcount;
u32 addresses[MAX_V4_ADDRS];
} a;
struct {
u32 addrcount;
struct in6_addr addresses[MAX_V6_ADDRS];
} aaaa;
struct {
char name[HOST_NAME_MAX];
} ptr;
} data;
};

struct nameserver {
evutil_socket_t socket;
struct sockaddr_storage address;
ev_socklen_t addrlen;
int failed_times;
int timedout;
struct event event;

struct nameserver *next, *prev;
struct event timeout_event;



struct evdns_request *probe_request;
char state;
char choked;
char write_waiting;
struct evdns_base *base;



int requests_inflight;
};




struct evdns_server_port {
evutil_socket_t socket;
int refcnt;
char choked;
char closing;
evdns_request_callback_fn_type user_callback;
void *user_data;
struct event event;

struct server_request *pending_replies;
struct event_base *event_base;

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
void *lock;
#endif
};


struct server_reply_item {
struct server_reply_item *next;
char *name;
u16 type;
u16 class;
u32 ttl;
char is_name;
u16 datalen;
void *data;
};



struct server_request {



struct server_request *next_pending;
struct server_request *prev_pending;

u16 trans_id;
struct evdns_server_port *port;
struct sockaddr_storage addr;
ev_socklen_t addrlen;

int n_answer;
int n_authority;
int n_additional;

struct server_reply_item *answer;
struct server_reply_item *authority;
struct server_reply_item *additional;



char *response;
size_t response_len;


struct evdns_server_request base;
};

struct evdns_base {



struct request **req_heads;


struct request *req_waiting_head;

struct nameserver *server_head;
int n_req_heads;

struct event_base *event_base;


int global_good_nameservers;



int global_requests_inflight;


int global_requests_waiting;

int global_max_requests_inflight;

struct timeval global_timeout;
int global_max_reissues;
int global_max_retransmits;

int global_max_nameserver_timeout;

int global_randomize_case;



struct timeval global_nameserver_probe_initial_timeout;


struct sockaddr_storage global_outgoing_address;

ev_socklen_t global_outgoing_addrlen;

struct timeval global_getaddrinfo_allow_skew;

int getaddrinfo_ipv4_timeouts;
int getaddrinfo_ipv6_timeouts;
int getaddrinfo_ipv4_answered;
int getaddrinfo_ipv6_answered;

struct search_state *global_search_state;

TAILQ_HEAD(hosts_list, hosts_entry) hostsdb;

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
void *lock;
#endif

int disable_when_inactive;
};

struct hosts_entry {
TAILQ_ENTRY(hosts_entry) next;
union {
struct sockaddr sa;
struct sockaddr_in sin;
struct sockaddr_in6 sin6;
} addr;
int addrlen;
char hostname[1];
};

static struct evdns_base *current_base = NULL;

struct evdns_base *
evdns_get_global_base(void)
{
return current_base;
}



#define TO_SERVER_REQUEST(base_ptr) ((struct server_request*) (((char*)(base_ptr) - evutil_offsetof(struct server_request, base))))



#define REQ_HEAD(base, id) ((base)->req_heads[id % (base)->n_req_heads])

static struct nameserver *nameserver_pick(struct evdns_base *base);
static void evdns_request_insert(struct request *req, struct request **head);
static void evdns_request_remove(struct request *req, struct request **head);
static void nameserver_ready_callback(evutil_socket_t fd, short events, void *arg);
static int evdns_transmit(struct evdns_base *base);
static int evdns_request_transmit(struct request *req);
static void nameserver_send_probe(struct nameserver *const ns);
static void search_request_finished(struct evdns_request *const);
static int search_try_next(struct evdns_request *const req);
static struct request *search_request_new(struct evdns_base *base, struct evdns_request *handle, int type, const char *const name, int flags, evdns_callback_type user_callback, void *user_arg);
static void evdns_requests_pump_waiting_queue(struct evdns_base *base);
static u16 transaction_id_pick(struct evdns_base *base);
static struct request *request_new(struct evdns_base *base, struct evdns_request *handle, int type, const char *name, int flags, evdns_callback_type callback, void *ptr);
static void request_submit(struct request *const req);

static int server_request_free(struct server_request *req);
static void server_request_free_answers(struct server_request *req);
static void server_port_free(struct evdns_server_port *port);
static void server_port_ready_callback(evutil_socket_t fd, short events, void *arg);
static int evdns_base_resolv_conf_parse_impl(struct evdns_base *base, int flags, const char *const filename);
static int evdns_base_set_option_impl(struct evdns_base *base,
const char *option, const char *val, int flags);
static void evdns_base_free_and_unlock(struct evdns_base *base, int fail_requests);
static void evdns_request_timeout_callback(evutil_socket_t fd, short events, void *arg);

static int strtoint(const char *const str);

#if defined(EVENT__DISABLE_THREAD_SUPPORT)
#define EVDNS_LOCK(base) EVUTIL_NIL_STMT_
#define EVDNS_UNLOCK(base) EVUTIL_NIL_STMT_
#define ASSERT_LOCKED(base) EVUTIL_NIL_STMT_
#else
#define EVDNS_LOCK(base) EVLOCK_LOCK((base)->lock, 0)

#define EVDNS_UNLOCK(base) EVLOCK_UNLOCK((base)->lock, 0)

#define ASSERT_LOCKED(base) EVLOCK_ASSERT_LOCKED((base)->lock)

#endif

static evdns_debug_log_fn_type evdns_log_fn = NULL;

void
evdns_set_log_fn(evdns_debug_log_fn_type fn)
{
evdns_log_fn = fn;
}

#if defined(__GNUC__)
#define EVDNS_LOG_CHECK __attribute__ ((format(printf, 2, 3)))
#else
#define EVDNS_LOG_CHECK
#endif

static void evdns_log_(int severity, const char *fmt, ...) EVDNS_LOG_CHECK;
static void
evdns_log_(int severity, const char *fmt, ...)
{
va_list args;
va_start(args,fmt);
if (evdns_log_fn) {
char buf[512];
int is_warn = (severity == EVDNS_LOG_WARN);
evutil_vsnprintf(buf, sizeof(buf), fmt, args);
evdns_log_fn(is_warn, buf);
} else {
event_logv_(severity, NULL, fmt, args);
}
va_end(args);
}

#define log evdns_log_




static struct request *
request_find_from_trans_id(struct evdns_base *base, u16 trans_id) {
struct request *req = REQ_HEAD(base, trans_id);
struct request *const started_at = req;

ASSERT_LOCKED(base);

if (req) {
do {
if (req->trans_id == trans_id) return req;
req = req->next;
} while (req != started_at);
}

return NULL;
}



static void
nameserver_prod_callback(evutil_socket_t fd, short events, void *arg) {
struct nameserver *const ns = (struct nameserver *) arg;
(void)fd;
(void)events;

EVDNS_LOCK(ns->base);
nameserver_send_probe(ns);
EVDNS_UNLOCK(ns->base);
}




static void
nameserver_probe_failed(struct nameserver *const ns) {
struct timeval timeout;
int i;

ASSERT_LOCKED(ns->base);
(void) evtimer_del(&ns->timeout_event);
if (ns->state == 1) {


return;
}

#define MAX_PROBE_TIMEOUT 3600
#define TIMEOUT_BACKOFF_FACTOR 3

memcpy(&timeout, &ns->base->global_nameserver_probe_initial_timeout,
sizeof(struct timeval));
for (i=ns->failed_times; i > 0 && timeout.tv_sec < MAX_PROBE_TIMEOUT; --i) {
timeout.tv_sec *= TIMEOUT_BACKOFF_FACTOR;
timeout.tv_usec *= TIMEOUT_BACKOFF_FACTOR;
if (timeout.tv_usec > 1000000) {
timeout.tv_sec += timeout.tv_usec / 1000000;
timeout.tv_usec %= 1000000;
}
}
if (timeout.tv_sec > MAX_PROBE_TIMEOUT) {
timeout.tv_sec = MAX_PROBE_TIMEOUT;
timeout.tv_usec = 0;
}

ns->failed_times++;

if (evtimer_add(&ns->timeout_event, &timeout) < 0) {
char addrbuf[128];
log(EVDNS_LOG_WARN,
"Error from libevent when adding timer event for %s",
evutil_format_sockaddr_port_(
(struct sockaddr *)&ns->address,
addrbuf, sizeof(addrbuf)));
}
}

static void
request_swap_ns(struct request *req, struct nameserver *ns) {
if (ns && req->ns != ns) {
EVUTIL_ASSERT(req->ns->requests_inflight > 0);
req->ns->requests_inflight--;
ns->requests_inflight++;

req->ns = ns;
}
}



static void
nameserver_failed(struct nameserver *const ns, const char *msg) {
struct request *req, *started_at;
struct evdns_base *base = ns->base;
int i;
char addrbuf[128];

ASSERT_LOCKED(base);


if (!ns->state) return;

log(EVDNS_LOG_MSG, "Nameserver %s has failed: %s",
evutil_format_sockaddr_port_(
(struct sockaddr *)&ns->address,
addrbuf, sizeof(addrbuf)),
msg);

base->global_good_nameservers--;
EVUTIL_ASSERT(base->global_good_nameservers >= 0);
if (base->global_good_nameservers == 0) {
log(EVDNS_LOG_MSG, "All nameservers have failed");
}

ns->state = 0;
ns->failed_times = 1;

if (evtimer_add(&ns->timeout_event,
&base->global_nameserver_probe_initial_timeout) < 0) {
log(EVDNS_LOG_WARN,
"Error from libevent when adding timer event for %s",
evutil_format_sockaddr_port_(
(struct sockaddr *)&ns->address,
addrbuf, sizeof(addrbuf)));

}







if (!base->global_good_nameservers) return;

for (i = 0; i < base->n_req_heads; ++i) {
req = started_at = base->req_heads[i];
if (req) {
do {
if (req->tx_count == 0 && req->ns == ns) {


request_swap_ns(req, nameserver_pick(base));
}
req = req->next;
} while (req != started_at);
}
}
}

static void
nameserver_up(struct nameserver *const ns)
{
char addrbuf[128];
ASSERT_LOCKED(ns->base);
if (ns->state) return;
log(EVDNS_LOG_MSG, "Nameserver %s is back up",
evutil_format_sockaddr_port_(
(struct sockaddr *)&ns->address,
addrbuf, sizeof(addrbuf)));
evtimer_del(&ns->timeout_event);
if (ns->probe_request) {
evdns_cancel_request(ns->base, ns->probe_request);
ns->probe_request = NULL;
}
ns->state = 1;
ns->failed_times = 0;
ns->timedout = 0;
ns->base->global_good_nameservers++;
}

static void
request_trans_id_set(struct request *const req, const u16 trans_id) {
req->trans_id = trans_id;
*((u16 *) req->request) = htons(trans_id);
}





static void
request_finished(struct request *const req, struct request **head, int free_handle) {
struct evdns_base *base = req->base;
int was_inflight = (head != &base->req_waiting_head);
EVDNS_LOCK(base);
ASSERT_VALID_REQUEST(req);

if (head)
evdns_request_remove(req, head);

log(EVDNS_LOG_DEBUG, "Removing timeout for request %p", req);
if (was_inflight) {
evtimer_del(&req->timeout_event);
base->global_requests_inflight--;
req->ns->requests_inflight--;
} else {
base->global_requests_waiting--;
}

event_debug_unassign(&req->timeout_event);

if (req->ns &&
req->ns->requests_inflight == 0 &&
req->base->disable_when_inactive) {
event_del(&req->ns->event);
evtimer_del(&req->ns->timeout_event);
}

if (!req->request_appended) {

mm_free(req->request);
} else {


}

if (req->handle) {
EVUTIL_ASSERT(req->handle->current_req == req);

if (free_handle) {
search_request_finished(req->handle);
req->handle->current_req = NULL;
if (! req->handle->pending_cb) {


mm_free(req->handle);
}
req->handle = NULL;

} else {
req->handle->current_req = NULL;
}
}

mm_free(req);

evdns_requests_pump_waiting_queue(base);
EVDNS_UNLOCK(base);
}







static int
request_reissue(struct request *req) {
const struct nameserver *const last_ns = req->ns;
ASSERT_LOCKED(req->base);
ASSERT_VALID_REQUEST(req);



request_swap_ns(req, nameserver_pick(req->base));
if (req->ns == last_ns) {



return 1;
}

req->reissue_count++;
req->tx_count = 0;
req->transmit_me = 1;

return 0;
}






static void
evdns_requests_pump_waiting_queue(struct evdns_base *base) {
ASSERT_LOCKED(base);
while (base->global_requests_inflight < base->global_max_requests_inflight &&
base->global_requests_waiting) {
struct request *req;

EVUTIL_ASSERT(base->req_waiting_head);
req = base->req_waiting_head;

req->ns = nameserver_pick(base);
if (!req->ns)
return;


req->ns->requests_inflight++;

evdns_request_remove(req, &base->req_waiting_head);

base->global_requests_waiting--;
base->global_requests_inflight++;

request_trans_id_set(req, transaction_id_pick(base));

evdns_request_insert(req, &REQ_HEAD(base, req->trans_id));
evdns_request_transmit(req);
evdns_transmit(base);
}
}


struct deferred_reply_callback {
struct event_callback deferred;
struct evdns_request *handle;
u8 request_type;
u8 have_reply;
u32 ttl;
u32 err;
evdns_callback_type user_callback;
struct reply reply;
};

static void
reply_run_callback(struct event_callback *d, void *user_pointer)
{
struct deferred_reply_callback *cb =
EVUTIL_UPCAST(d, struct deferred_reply_callback, deferred);

switch (cb->request_type) {
case TYPE_A:
if (cb->have_reply)
cb->user_callback(DNS_ERR_NONE, DNS_IPv4_A,
cb->reply.data.a.addrcount, cb->ttl,
cb->reply.data.a.addresses,
user_pointer);
else
cb->user_callback(cb->err, 0, 0, cb->ttl, NULL, user_pointer);
break;
case TYPE_PTR:
if (cb->have_reply) {
char *name = cb->reply.data.ptr.name;
cb->user_callback(DNS_ERR_NONE, DNS_PTR, 1, cb->ttl,
&name, user_pointer);
} else {
cb->user_callback(cb->err, 0, 0, cb->ttl, NULL, user_pointer);
}
break;
case TYPE_AAAA:
if (cb->have_reply)
cb->user_callback(DNS_ERR_NONE, DNS_IPv6_AAAA,
cb->reply.data.aaaa.addrcount, cb->ttl,
cb->reply.data.aaaa.addresses,
user_pointer);
else
cb->user_callback(cb->err, 0, 0, cb->ttl, NULL, user_pointer);
break;
default:
EVUTIL_ASSERT(0);
}

if (cb->handle && cb->handle->pending_cb) {
mm_free(cb->handle);
}

mm_free(cb);
}

static void
reply_schedule_callback(struct request *const req, u32 ttl, u32 err, struct reply *reply)
{
struct deferred_reply_callback *d = mm_calloc(1, sizeof(*d));

if (!d) {
event_warn("%s: Couldn't allocate space for deferred callback.",
__func__);
return;
}

ASSERT_LOCKED(req->base);

d->request_type = req->request_type;
d->user_callback = req->user_callback;
d->ttl = ttl;
d->err = err;
if (reply) {
d->have_reply = 1;
memcpy(&d->reply, reply, sizeof(struct reply));
}

if (req->handle) {
req->handle->pending_cb = 1;
d->handle = req->handle;
}

event_deferred_cb_init_(
&d->deferred,
event_get_priority(&req->timeout_event),
reply_run_callback,
req->user_pointer);
event_deferred_cb_schedule_(
req->base->event_base,
&d->deferred);
}


static void
reply_handle(struct request *const req, u16 flags, u32 ttl, struct reply *reply) {
int error;
char addrbuf[128];
static const int error_codes[] = {
DNS_ERR_FORMAT, DNS_ERR_SERVERFAILED, DNS_ERR_NOTEXIST,
DNS_ERR_NOTIMPL, DNS_ERR_REFUSED
};

ASSERT_LOCKED(req->base);
ASSERT_VALID_REQUEST(req);

if (flags & 0x020f || !reply || !reply->have_answer) {

if (flags & 0x0200) {
error = DNS_ERR_TRUNCATED;
} else if (flags & 0x000f) {
u16 error_code = (flags & 0x000f) - 1;
if (error_code > 4) {
error = DNS_ERR_UNKNOWN;
} else {
error = error_codes[error_code];
}
} else if (reply && !reply->have_answer) {
error = DNS_ERR_NODATA;
} else {
error = DNS_ERR_UNKNOWN;
}

switch (error) {
case DNS_ERR_NOTIMPL:
case DNS_ERR_REFUSED:

if (req->reissue_count < req->base->global_max_reissues) {
char msg[64];
evutil_snprintf(msg, sizeof(msg), "Bad response %d (%s)",
error, evdns_err_to_string(error));
nameserver_failed(req->ns, msg);
if (!request_reissue(req)) return;
}
break;
case DNS_ERR_SERVERFAILED:





log(EVDNS_LOG_DEBUG, "Got a SERVERFAILED from nameserver"
"at %s; will allow the request to time out.",
evutil_format_sockaddr_port_(
(struct sockaddr *)&req->ns->address,
addrbuf, sizeof(addrbuf)));

evdns_request_timeout_callback(0, 0, req);
return;
default:

if (req->handle == req->ns->probe_request) {

req->ns->probe_request = NULL;
}

nameserver_up(req->ns);
}

if (req->handle->search_state &&
req->request_type != TYPE_PTR) {


if (!search_try_next(req->handle)) {





return;
}
}


reply_schedule_callback(req, ttl, error, NULL);
request_finished(req, &REQ_HEAD(req->base, req->trans_id), 1);
} else {

reply_schedule_callback(req, ttl, 0, reply);
if (req->handle == req->ns->probe_request)
req->ns->probe_request = NULL;
nameserver_up(req->ns);
request_finished(req, &REQ_HEAD(req->base, req->trans_id), 1);
}
}

static int
name_parse(u8 *packet, int length, int *idx, char *name_out, int name_out_len) {
int name_end = -1;
int j = *idx;
int ptr_count = 0;
#define GET32(x) do { if (j + 4 > length) goto err; memcpy(&t32_, packet + j, 4); j += 4; x = ntohl(t32_); } while (0)
#define GET16(x) do { if (j + 2 > length) goto err; memcpy(&t_, packet + j, 2); j += 2; x = ntohs(t_); } while (0)
#define GET8(x) do { if (j >= length) goto err; x = packet[j++]; } while (0)

char *cp = name_out;
const char *const end = name_out + name_out_len;







for (;;) {
u8 label_len;
GET8(label_len);
if (!label_len) break;
if (label_len & 0xc0) {
u8 ptr_low;
GET8(ptr_low);
if (name_end < 0) name_end = j;
j = (((int)label_len & 0x3f) << 8) + ptr_low;

if (j < 0 || j >= length) return -1;


if (++ptr_count > length) return -1;
continue;
}
if (label_len > 63) return -1;
if (cp != name_out) {
if (cp + 1 >= end) return -1;
*cp++ = '.';
}
if (cp + label_len >= end) return -1;
if (j + label_len > length) return -1;
memcpy(cp, packet + j, label_len);
cp += label_len;
j += label_len;
}
if (cp >= end) return -1;
*cp = '\0';
if (name_end < 0)
*idx = j;
else
*idx = name_end;
return 0;
err:
return -1;
}


static int
reply_parse(struct evdns_base *base, u8 *packet, int length) {
int j = 0, k = 0;
u16 t_;
u32 t32_;
char tmp_name[256], cmp_name[256];
int name_matches = 0;

u16 trans_id, questions, answers, authority, additional, datalength;
u16 flags = 0;
u32 ttl, ttl_r = 0xffffffff;
struct reply reply;
struct request *req = NULL;
unsigned int i;

ASSERT_LOCKED(base);

GET16(trans_id);
GET16(flags);
GET16(questions);
GET16(answers);
GET16(authority);
GET16(additional);
(void) authority;
(void) additional;

req = request_find_from_trans_id(base, trans_id);
if (!req) return -1;
EVUTIL_ASSERT(req->base == base);

memset(&reply, 0, sizeof(reply));


if (!(flags & 0x8000)) return -1;
if ((flags & 0x020f) && (flags & 0x020f) != DNS_ERR_NOTEXIST) {

goto err;
}



#define SKIP_NAME do { tmp_name[0] = '\0'; if (name_parse(packet, length, &j, tmp_name, sizeof(tmp_name))<0) goto err; } while (0)






reply.type = req->request_type;


for (i = 0; i < questions; ++i) {



tmp_name[0] = '\0';
cmp_name[0] = '\0';
k = j;
if (name_parse(packet, length, &j, tmp_name, sizeof(tmp_name)) < 0)
goto err;
if (name_parse(req->request, req->request_len, &k,
cmp_name, sizeof(cmp_name))<0)
goto err;
if (!base->global_randomize_case) {
if (strcmp(tmp_name, cmp_name) == 0)
name_matches = 1;
} else {
if (evutil_ascii_strcasecmp(tmp_name, cmp_name) == 0)
name_matches = 1;
}

j += 4;
if (j > length)
goto err;
}

if (!name_matches)
goto err;





for (i = 0; i < answers; ++i) {
u16 type, class;

SKIP_NAME;
GET16(type);
GET16(class);
GET32(ttl);
GET16(datalength);

if (type == TYPE_A && class == CLASS_INET) {
int addrcount, addrtocopy;
if (req->request_type != TYPE_A) {
j += datalength; continue;
}
if ((datalength & 3) != 0)
goto err;
addrcount = datalength >> 2;
addrtocopy = MIN(MAX_V4_ADDRS - reply.data.a.addrcount, (unsigned)addrcount);

ttl_r = MIN(ttl_r, ttl);

if (j + 4*addrtocopy > length) goto err;
memcpy(&reply.data.a.addresses[reply.data.a.addrcount],
packet + j, 4*addrtocopy);
j += 4*addrtocopy;
reply.data.a.addrcount += addrtocopy;
reply.have_answer = 1;
if (reply.data.a.addrcount == MAX_V4_ADDRS) break;
} else if (type == TYPE_PTR && class == CLASS_INET) {
if (req->request_type != TYPE_PTR) {
j += datalength; continue;
}
if (name_parse(packet, length, &j, reply.data.ptr.name,
sizeof(reply.data.ptr.name))<0)
goto err;
ttl_r = MIN(ttl_r, ttl);
reply.have_answer = 1;
break;
} else if (type == TYPE_CNAME) {
char cname[HOST_NAME_MAX];
if (!req->put_cname_in_ptr || *req->put_cname_in_ptr) {
j += datalength; continue;
}
if (name_parse(packet, length, &j, cname,
sizeof(cname))<0)
goto err;
*req->put_cname_in_ptr = mm_strdup(cname);
} else if (type == TYPE_AAAA && class == CLASS_INET) {
int addrcount, addrtocopy;
if (req->request_type != TYPE_AAAA) {
j += datalength; continue;
}
if ((datalength & 15) != 0)
goto err;
addrcount = datalength >> 4;
addrtocopy = MIN(MAX_V6_ADDRS - reply.data.aaaa.addrcount, (unsigned)addrcount);
ttl_r = MIN(ttl_r, ttl);


if (j + 16*addrtocopy > length) goto err;
memcpy(&reply.data.aaaa.addresses[reply.data.aaaa.addrcount],
packet + j, 16*addrtocopy);
reply.data.aaaa.addrcount += addrtocopy;
j += 16*addrtocopy;
reply.have_answer = 1;
if (reply.data.aaaa.addrcount == MAX_V6_ADDRS) break;
} else {

j += datalength;
}
}

if (!reply.have_answer) {
for (i = 0; i < authority; ++i) {
u16 type, class;
SKIP_NAME;
GET16(type);
GET16(class);
GET32(ttl);
GET16(datalength);
if (type == TYPE_SOA && class == CLASS_INET) {
u32 serial, refresh, retry, expire, minimum;
SKIP_NAME;
SKIP_NAME;
GET32(serial);
GET32(refresh);
GET32(retry);
GET32(expire);
GET32(minimum);
(void)expire;
(void)retry;
(void)refresh;
(void)serial;
ttl_r = MIN(ttl_r, ttl);
ttl_r = MIN(ttl_r, minimum);
} else {

j += datalength;
}
}
}

if (ttl_r == 0xffffffff)
ttl_r = 0;

reply_handle(req, flags, ttl_r, &reply);
return 0;
err:
if (req)
reply_handle(req, flags, 0, NULL);
return -1;
}




static int
request_parse(u8 *packet, int length, struct evdns_server_port *port, struct sockaddr *addr, ev_socklen_t addrlen)
{
int j = 0;
u16 t_;
char tmp_name[256];

int i;
u16 trans_id, flags, questions, answers, authority, additional;
struct server_request *server_req = NULL;

ASSERT_LOCKED(port);


GET16(trans_id);
GET16(flags);
GET16(questions);
GET16(answers);
GET16(authority);
GET16(additional);
(void)answers;
(void)additional;
(void)authority;

if (flags & 0x8000) return -1;
flags &= 0x0110;

server_req = mm_malloc(sizeof(struct server_request));
if (server_req == NULL) return -1;
memset(server_req, 0, sizeof(struct server_request));

server_req->trans_id = trans_id;
memcpy(&server_req->addr, addr, addrlen);
server_req->addrlen = addrlen;

server_req->base.flags = flags;
server_req->base.nquestions = 0;
server_req->base.questions = mm_calloc(sizeof(struct evdns_server_question *), questions);
if (server_req->base.questions == NULL)
goto err;

for (i = 0; i < questions; ++i) {
u16 type, class;
struct evdns_server_question *q;
int namelen;
if (name_parse(packet, length, &j, tmp_name, sizeof(tmp_name))<0)
goto err;
GET16(type);
GET16(class);
namelen = (int)strlen(tmp_name);
q = mm_malloc(sizeof(struct evdns_server_question) + namelen);
if (!q)
goto err;
q->type = type;
q->dns_question_class = class;
memcpy(q->name, tmp_name, namelen+1);
server_req->base.questions[server_req->base.nquestions++] = q;
}



server_req->port = port;
port->refcnt++;


if (flags & 0x7800) {
evdns_server_request_respond(&(server_req->base), DNS_ERR_NOTIMPL);
return -1;
}

port->user_callback(&(server_req->base), port->user_data);

return 0;
err:
if (server_req) {
if (server_req->base.questions) {
for (i = 0; i < server_req->base.nquestions; ++i)
mm_free(server_req->base.questions[i]);
mm_free(server_req->base.questions);
}
mm_free(server_req);
}
return -1;

#undef SKIP_NAME
#undef GET32
#undef GET16
#undef GET8
}


void
evdns_set_transaction_id_fn(ev_uint16_t (*fn)(void))
{
}

void
evdns_set_random_bytes_fn(void (*fn)(char *, size_t))
{
}


static u16
transaction_id_pick(struct evdns_base *base) {
ASSERT_LOCKED(base);
for (;;) {
u16 trans_id;
evutil_secure_rng_get_bytes(&trans_id, sizeof(trans_id));

if (trans_id == 0xffff) continue;

if (request_find_from_trans_id(base, trans_id) == NULL)
return trans_id;
}
}




static struct nameserver *
nameserver_pick(struct evdns_base *base) {
struct nameserver *started_at = base->server_head, *picked;
ASSERT_LOCKED(base);
if (!base->server_head) return NULL;



if (!base->global_good_nameservers) {
base->server_head = base->server_head->next;
return base->server_head;
}


for (;;) {
if (base->server_head->state) {

picked = base->server_head;
base->server_head = base->server_head->next;
return picked;
}

base->server_head = base->server_head->next;
if (base->server_head == started_at) {



EVUTIL_ASSERT(base->global_good_nameservers == 0);
picked = base->server_head;
base->server_head = base->server_head->next;
return picked;
}
}
}


static void
nameserver_read(struct nameserver *ns) {
struct sockaddr_storage ss;
ev_socklen_t addrlen = sizeof(ss);
u8 packet[1500];
char addrbuf[128];
ASSERT_LOCKED(ns->base);

for (;;) {
const int r = recvfrom(ns->socket, (void*)packet,
sizeof(packet), 0,
(struct sockaddr*)&ss, &addrlen);
if (r < 0) {
int err = evutil_socket_geterror(ns->socket);
if (EVUTIL_ERR_RW_RETRIABLE(err))
return;
nameserver_failed(ns,
evutil_socket_error_to_string(err));
return;
}
if (evutil_sockaddr_cmp((struct sockaddr*)&ss,
(struct sockaddr*)&ns->address, 0)) {
log(EVDNS_LOG_WARN, "Address mismatch on received "
"DNS packet. Apparent source was %s",
evutil_format_sockaddr_port_(
(struct sockaddr *)&ss,
addrbuf, sizeof(addrbuf)));
return;
}

ns->timedout = 0;
reply_parse(ns->base, packet, r);
}
}



static void
server_port_read(struct evdns_server_port *s) {
u8 packet[1500];
struct sockaddr_storage addr;
ev_socklen_t addrlen;
int r;
ASSERT_LOCKED(s);

for (;;) {
addrlen = sizeof(struct sockaddr_storage);
r = recvfrom(s->socket, (void*)packet, sizeof(packet), 0,
(struct sockaddr*) &addr, &addrlen);
if (r < 0) {
int err = evutil_socket_geterror(s->socket);
if (EVUTIL_ERR_RW_RETRIABLE(err))
return;
log(EVDNS_LOG_WARN,
"Error %s (%d) while reading request.",
evutil_socket_error_to_string(err), err);
return;
}
request_parse(packet, r, s, (struct sockaddr*) &addr, addrlen);
}
}


static void
server_port_flush(struct evdns_server_port *port)
{
struct server_request *req = port->pending_replies;
ASSERT_LOCKED(port);
while (req) {
int r = sendto(port->socket, req->response, (int)req->response_len, 0,
(struct sockaddr*) &req->addr, (ev_socklen_t)req->addrlen);
if (r < 0) {
int err = evutil_socket_geterror(port->socket);
if (EVUTIL_ERR_RW_RETRIABLE(err))
return;
log(EVDNS_LOG_WARN, "Error %s (%d) while writing response to port; dropping", evutil_socket_error_to_string(err), err);
}
if (server_request_free(req)) {

return;
} else {
EVUTIL_ASSERT(req != port->pending_replies);
req = port->pending_replies;
}
}


(void) event_del(&port->event);
event_assign(&port->event, port->event_base,
port->socket, EV_READ | EV_PERSIST,
server_port_ready_callback, port);

if (event_add(&port->event, NULL) < 0) {
log(EVDNS_LOG_WARN, "Error from libevent when adding event for DNS server.");

}
}




static void
nameserver_write_waiting(struct nameserver *ns, char waiting) {
ASSERT_LOCKED(ns->base);
if (ns->write_waiting == waiting) return;

ns->write_waiting = waiting;
(void) event_del(&ns->event);
event_assign(&ns->event, ns->base->event_base,
ns->socket, EV_READ | (waiting ? EV_WRITE : 0) | EV_PERSIST,
nameserver_ready_callback, ns);
if (event_add(&ns->event, NULL) < 0) {
char addrbuf[128];
log(EVDNS_LOG_WARN, "Error from libevent when adding event for %s",
evutil_format_sockaddr_port_(
(struct sockaddr *)&ns->address,
addrbuf, sizeof(addrbuf)));

}
}



static void
nameserver_ready_callback(evutil_socket_t fd, short events, void *arg) {
struct nameserver *ns = (struct nameserver *) arg;
(void)fd;

EVDNS_LOCK(ns->base);
if (events & EV_WRITE) {
ns->choked = 0;
if (!evdns_transmit(ns->base)) {
nameserver_write_waiting(ns, 0);
}
}
if (events & EV_READ) {
nameserver_read(ns);
}
EVDNS_UNLOCK(ns->base);
}



static void
server_port_ready_callback(evutil_socket_t fd, short events, void *arg) {
struct evdns_server_port *port = (struct evdns_server_port *) arg;
(void) fd;

EVDNS_LOCK(port);
if (events & EV_WRITE) {
port->choked = 0;
server_port_flush(port);
}
if (events & EV_READ) {
server_port_read(port);
}
EVDNS_UNLOCK(port);
}



#define MAX_LABELS 128

struct dnslabel_entry { char *v; off_t pos; };
struct dnslabel_table {
int n_labels;

struct dnslabel_entry labels[MAX_LABELS];
};


static void
dnslabel_table_init(struct dnslabel_table *table)
{
table->n_labels = 0;
}


static void
dnslabel_clear(struct dnslabel_table *table)
{
int i;
for (i = 0; i < table->n_labels; ++i)
mm_free(table->labels[i].v);
table->n_labels = 0;
}



static int
dnslabel_table_get_pos(const struct dnslabel_table *table, const char *label)
{
int i;
for (i = 0; i < table->n_labels; ++i) {
if (!strcmp(label, table->labels[i].v))
return table->labels[i].pos;
}
return -1;
}


static int
dnslabel_table_add(struct dnslabel_table *table, const char *label, off_t pos)
{
char *v;
int p;
if (table->n_labels == MAX_LABELS)
return (-1);
v = mm_strdup(label);
if (v == NULL)
return (-1);
p = table->n_labels++;
table->labels[p].v = v;
table->labels[p].pos = pos;

return (0);
}












static off_t
dnsname_to_labels(u8 *const buf, size_t buf_len, off_t j,
const char *name, const size_t name_len,
struct dnslabel_table *table) {
const char *end = name + name_len;
int ref = 0;
u16 t_;

#define APPEND16(x) do { if (j + 2 > (off_t)buf_len) goto overflow; t_ = htons(x); memcpy(buf + j, &t_, 2); j += 2; } while (0)






#define APPEND32(x) do { if (j + 4 > (off_t)buf_len) goto overflow; t32_ = htonl(x); memcpy(buf + j, &t32_, 4); j += 4; } while (0)







if (name_len > 255) return -2;

for (;;) {
const char *const start = name;
if (table && (ref = dnslabel_table_get_pos(table, name)) >= 0) {
APPEND16(ref | 0xc000);
return j;
}
name = strchr(name, '.');
if (!name) {
const size_t label_len = end - start;
if (label_len > 63) return -1;
if ((size_t)(j+label_len+1) > buf_len) return -2;
if (table) dnslabel_table_add(table, start, j);
buf[j++] = (ev_uint8_t)label_len;

memcpy(buf + j, start, label_len);
j += (int) label_len;
break;
} else {

const size_t label_len = name - start;
if (label_len > 63) return -1;
if ((size_t)(j+label_len+1) > buf_len) return -2;
if (table) dnslabel_table_add(table, start, j);
buf[j++] = (ev_uint8_t)label_len;

memcpy(buf + j, start, label_len);
j += (int) label_len;

name++;
}
}




if (!j || buf[j-1]) buf[j++] = 0;
return j;
overflow:
return (-2);
}




static size_t
evdns_request_len(const size_t name_len) {
return 96 +
name_len + 2 +
4;
}





static int
evdns_request_data_build(const char *const name, const size_t name_len,
const u16 trans_id, const u16 type, const u16 class,
u8 *const buf, size_t buf_len) {
off_t j = 0;
u16 t_;

APPEND16(trans_id);
APPEND16(0x0100);
APPEND16(1);
APPEND16(0);
APPEND16(0);
APPEND16(0);

j = dnsname_to_labels(buf, buf_len, j, name, name_len, NULL);
if (j < 0) {
return (int)j;
}

APPEND16(type);
APPEND16(class);

return (int)j;
overflow:
return (-1);
}


struct evdns_server_port *
evdns_add_server_port_with_base(struct event_base *base, evutil_socket_t socket, int flags, evdns_request_callback_fn_type cb, void *user_data)
{
struct evdns_server_port *port;
if (flags)
return NULL;
if (!(port = mm_malloc(sizeof(struct evdns_server_port))))
return NULL;
memset(port, 0, sizeof(struct evdns_server_port));


port->socket = socket;
port->refcnt = 1;
port->choked = 0;
port->closing = 0;
port->user_callback = cb;
port->user_data = user_data;
port->pending_replies = NULL;
port->event_base = base;

event_assign(&port->event, port->event_base,
port->socket, EV_READ | EV_PERSIST,
server_port_ready_callback, port);
if (event_add(&port->event, NULL) < 0) {
mm_free(port);
return NULL;
}
EVTHREAD_ALLOC_LOCK(port->lock, EVTHREAD_LOCKTYPE_RECURSIVE);
return port;
}

struct evdns_server_port *
evdns_add_server_port(evutil_socket_t socket, int flags, evdns_request_callback_fn_type cb, void *user_data)
{
return evdns_add_server_port_with_base(NULL, socket, flags, cb, user_data);
}


void
evdns_close_server_port(struct evdns_server_port *port)
{
EVDNS_LOCK(port);
if (--port->refcnt == 0) {
EVDNS_UNLOCK(port);
server_port_free(port);
} else {
port->closing = 1;
}
}


int
evdns_server_request_add_reply(struct evdns_server_request *req_, int section, const char *name, int type, int class, int ttl, int datalen, int is_name, const char *data)
{
struct server_request *req = TO_SERVER_REQUEST(req_);
struct server_reply_item **itemp, *item;
int *countp;
int result = -1;

EVDNS_LOCK(req->port);
if (req->response)
goto done;

switch (section) {
case EVDNS_ANSWER_SECTION:
itemp = &req->answer;
countp = &req->n_answer;
break;
case EVDNS_AUTHORITY_SECTION:
itemp = &req->authority;
countp = &req->n_authority;
break;
case EVDNS_ADDITIONAL_SECTION:
itemp = &req->additional;
countp = &req->n_additional;
break;
default:
goto done;
}
while (*itemp) {
itemp = &((*itemp)->next);
}
item = mm_malloc(sizeof(struct server_reply_item));
if (!item)
goto done;
item->next = NULL;
if (!(item->name = mm_strdup(name))) {
mm_free(item);
goto done;
}
item->type = type;
item->dns_question_class = class;
item->ttl = ttl;
item->is_name = is_name != 0;
item->datalen = 0;
item->data = NULL;
if (data) {
if (item->is_name) {
if (!(item->data = mm_strdup(data))) {
mm_free(item->name);
mm_free(item);
goto done;
}
item->datalen = (u16)-1;
} else {
if (!(item->data = mm_malloc(datalen))) {
mm_free(item->name);
mm_free(item);
goto done;
}
item->datalen = datalen;
memcpy(item->data, data, datalen);
}
}

*itemp = item;
++(*countp);
result = 0;
done:
EVDNS_UNLOCK(req->port);
return result;
}


int
evdns_server_request_add_a_reply(struct evdns_server_request *req, const char *name, int n, const void *addrs, int ttl)
{
return evdns_server_request_add_reply(
req, EVDNS_ANSWER_SECTION, name, TYPE_A, CLASS_INET,
ttl, n*4, 0, addrs);
}


int
evdns_server_request_add_aaaa_reply(struct evdns_server_request *req, const char *name, int n, const void *addrs, int ttl)
{
return evdns_server_request_add_reply(
req, EVDNS_ANSWER_SECTION, name, TYPE_AAAA, CLASS_INET,
ttl, n*16, 0, addrs);
}


int
evdns_server_request_add_ptr_reply(struct evdns_server_request *req, struct in_addr *in, const char *inaddr_name, const char *hostname, int ttl)
{
u32 a;
char buf[32];
if (in && inaddr_name)
return -1;
else if (!in && !inaddr_name)
return -1;
if (in) {
a = ntohl(in->s_addr);
evutil_snprintf(buf, sizeof(buf), "%d.%d.%d.%d.in-addr.arpa",
(int)(u8)((a )&0xff),
(int)(u8)((a>>8 )&0xff),
(int)(u8)((a>>16)&0xff),
(int)(u8)((a>>24)&0xff));
inaddr_name = buf;
}
return evdns_server_request_add_reply(
req, EVDNS_ANSWER_SECTION, inaddr_name, TYPE_PTR, CLASS_INET,
ttl, -1, 1, hostname);
}


int
evdns_server_request_add_cname_reply(struct evdns_server_request *req, const char *name, const char *cname, int ttl)
{
return evdns_server_request_add_reply(
req, EVDNS_ANSWER_SECTION, name, TYPE_CNAME, CLASS_INET,
ttl, -1, 1, cname);
}


void
evdns_server_request_set_flags(struct evdns_server_request *exreq, int flags)
{
struct server_request *req = TO_SERVER_REQUEST(exreq);
req->base.flags &= ~(EVDNS_FLAGS_AA|EVDNS_FLAGS_RD);
req->base.flags |= flags;
}

static int
evdns_server_request_format_response(struct server_request *req, int err)
{
unsigned char buf[1500];
size_t buf_len = sizeof(buf);
off_t j = 0, r;
u16 t_;
u32 t32_;
int i;
u16 flags;
struct dnslabel_table table;

if (err < 0 || err > 15) return -1;



flags = req->base.flags;
flags |= (0x8000 | err);

dnslabel_table_init(&table);
APPEND16(req->trans_id);
APPEND16(flags);
APPEND16(req->base.nquestions);
APPEND16(req->n_answer);
APPEND16(req->n_authority);
APPEND16(req->n_additional);


for (i=0; i < req->base.nquestions; ++i) {
const char *s = req->base.questions[i]->name;
j = dnsname_to_labels(buf, buf_len, j, s, strlen(s), &table);
if (j < 0) {
dnslabel_clear(&table);
return (int) j;
}
APPEND16(req->base.questions[i]->type);
APPEND16(req->base.questions[i]->dns_question_class);
}


for (i=0; i<3; ++i) {
struct server_reply_item *item;
if (i==0)
item = req->answer;
else if (i==1)
item = req->authority;
else
item = req->additional;
while (item) {
r = dnsname_to_labels(buf, buf_len, j, item->name, strlen(item->name), &table);
if (r < 0)
goto overflow;
j = r;

APPEND16(item->type);
APPEND16(item->dns_question_class);
APPEND32(item->ttl);
if (item->is_name) {
off_t len_idx = j, name_start;
j += 2;
name_start = j;
r = dnsname_to_labels(buf, buf_len, j, item->data, strlen(item->data), &table);
if (r < 0)
goto overflow;
j = r;
t_ = htons( (short) (j-name_start) );
memcpy(buf+len_idx, &t_, 2);
} else {
APPEND16(item->datalen);
if (j+item->datalen > (off_t)buf_len)
goto overflow;
memcpy(buf+j, item->data, item->datalen);
j += item->datalen;
}
item = item->next;
}
}

if (j > 512) {
overflow:
j = 512;
buf[2] |= 0x02;
}

req->response_len = j;

if (!(req->response = mm_malloc(req->response_len))) {
server_request_free_answers(req);
dnslabel_clear(&table);
return (-1);
}
memcpy(req->response, buf, req->response_len);
server_request_free_answers(req);
dnslabel_clear(&table);
return (0);
}


int
evdns_server_request_respond(struct evdns_server_request *req_, int err)
{
struct server_request *req = TO_SERVER_REQUEST(req_);
struct evdns_server_port *port = req->port;
int r = -1;

EVDNS_LOCK(port);
if (!req->response) {
if ((r = evdns_server_request_format_response(req, err))<0)
goto done;
}

r = sendto(port->socket, req->response, (int)req->response_len, 0,
(struct sockaddr*) &req->addr, (ev_socklen_t)req->addrlen);
if (r<0) {
int sock_err = evutil_socket_geterror(port->socket);
if (EVUTIL_ERR_RW_RETRIABLE(sock_err))
goto done;

if (port->pending_replies) {
req->prev_pending = port->pending_replies->prev_pending;
req->next_pending = port->pending_replies;
req->prev_pending->next_pending =
req->next_pending->prev_pending = req;
} else {
req->prev_pending = req->next_pending = req;
port->pending_replies = req;
port->choked = 1;

(void) event_del(&port->event);
event_assign(&port->event, port->event_base, port->socket, (port->closing?0:EV_READ) | EV_WRITE | EV_PERSIST, server_port_ready_callback, port);

if (event_add(&port->event, NULL) < 0) {
log(EVDNS_LOG_WARN, "Error from libevent when adding event for DNS server");
}

}

r = 1;
goto done;
}
if (server_request_free(req)) {
r = 0;
goto done;
}

if (port->pending_replies)
server_port_flush(port);

r = 0;
done:
EVDNS_UNLOCK(port);
return r;
}


static void
server_request_free_answers(struct server_request *req)
{
struct server_reply_item *victim, *next, **list;
int i;
for (i = 0; i < 3; ++i) {
if (i==0)
list = &req->answer;
else if (i==1)
list = &req->authority;
else
list = &req->additional;

victim = *list;
while (victim) {
next = victim->next;
mm_free(victim->name);
if (victim->data)
mm_free(victim->data);
mm_free(victim);
victim = next;
}
*list = NULL;
}
}



static int
server_request_free(struct server_request *req)
{
int i, rc=1, lock=0;
if (req->base.questions) {
for (i = 0; i < req->base.nquestions; ++i)
mm_free(req->base.questions[i]);
mm_free(req->base.questions);
}

if (req->port) {
EVDNS_LOCK(req->port);
lock=1;
if (req->port->pending_replies == req) {
if (req->next_pending && req->next_pending != req)
req->port->pending_replies = req->next_pending;
else
req->port->pending_replies = NULL;
}
rc = --req->port->refcnt;
}

if (req->response) {
mm_free(req->response);
}

server_request_free_answers(req);

if (req->next_pending && req->next_pending != req) {
req->next_pending->prev_pending = req->prev_pending;
req->prev_pending->next_pending = req->next_pending;
}

if (rc == 0) {
EVDNS_UNLOCK(req->port);
server_port_free(req->port);
mm_free(req);
return (1);
}
if (lock)
EVDNS_UNLOCK(req->port);
mm_free(req);
return (0);
}


static void
server_port_free(struct evdns_server_port *port)
{
EVUTIL_ASSERT(port);
EVUTIL_ASSERT(!port->refcnt);
EVUTIL_ASSERT(!port->pending_replies);
if (port->socket > 0) {
evutil_closesocket(port->socket);
port->socket = -1;
}
(void) event_del(&port->event);
event_debug_unassign(&port->event);
EVTHREAD_FREE_LOCK(port->lock, EVTHREAD_LOCKTYPE_RECURSIVE);
mm_free(port);
}


int
evdns_server_request_drop(struct evdns_server_request *req_)
{
struct server_request *req = TO_SERVER_REQUEST(req_);
server_request_free(req);
return 0;
}


int
evdns_server_request_get_requesting_addr(struct evdns_server_request *req_, struct sockaddr *sa, int addr_len)
{
struct server_request *req = TO_SERVER_REQUEST(req_);
if (addr_len < (int)req->addrlen)
return -1;
memcpy(sa, &(req->addr), req->addrlen);
return req->addrlen;
}

#undef APPEND16
#undef APPEND32



static void
evdns_request_timeout_callback(evutil_socket_t fd, short events, void *arg) {
struct request *const req = (struct request *) arg;
struct evdns_base *base = req->base;

(void) fd;
(void) events;

log(EVDNS_LOG_DEBUG, "Request %p timed out", arg);
EVDNS_LOCK(base);

if (req->tx_count >= req->base->global_max_retransmits) {
struct nameserver *ns = req->ns;

log(EVDNS_LOG_DEBUG, "Giving up on request %p; tx_count==%d",
arg, req->tx_count);
reply_schedule_callback(req, 0, DNS_ERR_TIMEOUT, NULL);

request_finished(req, &REQ_HEAD(req->base, req->trans_id), 1);
nameserver_failed(ns, "request timed out.");
} else {

log(EVDNS_LOG_DEBUG, "Retransmitting request %p; tx_count==%d",
arg, req->tx_count);
(void) evtimer_del(&req->timeout_event);
request_swap_ns(req, nameserver_pick(base));
evdns_request_transmit(req);

req->ns->timedout++;
if (req->ns->timedout > req->base->global_max_nameserver_timeout) {
req->ns->timedout = 0;
nameserver_failed(req->ns, "request timed out.");
}
}

EVDNS_UNLOCK(base);
}







static int
evdns_request_transmit_to(struct request *req, struct nameserver *server) {
int r;
ASSERT_LOCKED(req->base);
ASSERT_VALID_REQUEST(req);

if (server->requests_inflight == 1 &&
req->base->disable_when_inactive &&
event_add(&server->event, NULL) < 0) {
return 1;
}

r = sendto(server->socket, (void*)req->request, req->request_len, 0,
(struct sockaddr *)&server->address, server->addrlen);
if (r < 0) {
int err = evutil_socket_geterror(server->socket);
if (EVUTIL_ERR_RW_RETRIABLE(err))
return 1;
nameserver_failed(req->ns, evutil_socket_error_to_string(err));
return 2;
} else if (r != (int)req->request_len) {
return 1;
} else {
return 0;
}
}







static int
evdns_request_transmit(struct request *req) {
int retcode = 0, r;

ASSERT_LOCKED(req->base);
ASSERT_VALID_REQUEST(req);


req->transmit_me = 1;
EVUTIL_ASSERT(req->trans_id != 0xffff);

if (!req->ns)
{

return 1;
}

if (req->ns->choked) {


return 1;
}

r = evdns_request_transmit_to(req, req->ns);
switch (r) {
case 1:

req->ns->choked = 1;
nameserver_write_waiting(req->ns, 1);
return 1;
case 2:

retcode = 1;


default:

log(EVDNS_LOG_DEBUG,
"Setting timeout for request %p, sent to nameserver %p", req, req->ns);
if (evtimer_add(&req->timeout_event, &req->base->global_timeout) < 0) {
log(EVDNS_LOG_WARN,
"Error from libevent when adding timer for request %p",
req);

}
req->tx_count++;
req->transmit_me = 0;
return retcode;
}
}

static void
nameserver_probe_callback(int result, char type, int count, int ttl, void *addresses, void *arg) {
struct nameserver *const ns = (struct nameserver *) arg;
(void) type;
(void) count;
(void) ttl;
(void) addresses;

if (result == DNS_ERR_CANCEL) {



return;
}

EVDNS_LOCK(ns->base);
ns->probe_request = NULL;
if (result == DNS_ERR_NONE || result == DNS_ERR_NOTEXIST) {

nameserver_up(ns);
} else {
nameserver_probe_failed(ns);
}
EVDNS_UNLOCK(ns->base);
}

static void
nameserver_send_probe(struct nameserver *const ns) {
struct evdns_request *handle;
struct request *req;
char addrbuf[128];



ASSERT_LOCKED(ns->base);
log(EVDNS_LOG_DEBUG, "Sending probe to %s",
evutil_format_sockaddr_port_(
(struct sockaddr *)&ns->address,
addrbuf, sizeof(addrbuf)));
handle = mm_calloc(1, sizeof(*handle));
if (!handle) return;
req = request_new(ns->base, handle, TYPE_A, "google.com", DNS_QUERY_NO_SEARCH, nameserver_probe_callback, ns);
if (!req) {
mm_free(handle);
return;
}
ns->probe_request = handle;

request_trans_id_set(req, transaction_id_pick(ns->base));
req->ns = ns;
request_submit(req);
}




static int
evdns_transmit(struct evdns_base *base) {
char did_try_to_transmit = 0;
int i;

ASSERT_LOCKED(base);
for (i = 0; i < base->n_req_heads; ++i) {
if (base->req_heads[i]) {
struct request *const started_at = base->req_heads[i], *req = started_at;

do {
if (req->transmit_me) {
did_try_to_transmit = 1;
evdns_request_transmit(req);
}

req = req->next;
} while (req != started_at);
}
}

return did_try_to_transmit;
}


int
evdns_base_count_nameservers(struct evdns_base *base)
{
const struct nameserver *server;
int n = 0;

EVDNS_LOCK(base);
server = base->server_head;
if (!server)
goto done;
do {
++n;
server = server->next;
} while (server != base->server_head);
done:
EVDNS_UNLOCK(base);
return n;
}

int
evdns_count_nameservers(void)
{
return evdns_base_count_nameservers(current_base);
}


int
evdns_base_clear_nameservers_and_suspend(struct evdns_base *base)
{
struct nameserver *server, *started_at;
int i;

EVDNS_LOCK(base);
server = base->server_head;
started_at = base->server_head;
if (!server) {
EVDNS_UNLOCK(base);
return 0;
}
while (1) {
struct nameserver *next = server->next;
(void) event_del(&server->event);
if (evtimer_initialized(&server->timeout_event))
(void) evtimer_del(&server->timeout_event);
if (server->probe_request) {
evdns_cancel_request(server->base, server->probe_request);
server->probe_request = NULL;
}
if (server->socket >= 0)
evutil_closesocket(server->socket);
mm_free(server);
if (next == started_at)
break;
server = next;
}
base->server_head = NULL;
base->global_good_nameservers = 0;

for (i = 0; i < base->n_req_heads; ++i) {
struct request *req, *req_started_at;
req = req_started_at = base->req_heads[i];
while (req) {
struct request *next = req->next;
req->tx_count = req->reissue_count = 0;
req->ns = NULL;

(void) evtimer_del(&req->timeout_event);
req->trans_id = 0;
req->transmit_me = 0;

base->global_requests_waiting++;
evdns_request_insert(req, &base->req_waiting_head);




base->req_waiting_head = base->req_waiting_head->prev;

if (next == req_started_at)
break;
req = next;
}
base->req_heads[i] = NULL;
}

base->global_requests_inflight = 0;

EVDNS_UNLOCK(base);
return 0;
}

int
evdns_clear_nameservers_and_suspend(void)
{
return evdns_base_clear_nameservers_and_suspend(current_base);
}



int
evdns_base_resume(struct evdns_base *base)
{
EVDNS_LOCK(base);
evdns_requests_pump_waiting_queue(base);
EVDNS_UNLOCK(base);

return 0;
}

int
evdns_resume(void)
{
return evdns_base_resume(current_base);
}

static int
evdns_nameserver_add_impl_(struct evdns_base *base, const struct sockaddr *address, int addrlen) {


const struct nameserver *server = base->server_head, *const started_at = base->server_head;
struct nameserver *ns;
int err = 0;
char addrbuf[128];

ASSERT_LOCKED(base);
if (server) {
do {
if (!evutil_sockaddr_cmp((struct sockaddr*)&server->address, address, 1)) return 3;
server = server->next;
} while (server != started_at);
}
if (addrlen > (int)sizeof(ns->address)) {
log(EVDNS_LOG_DEBUG, "Addrlen %d too long.", (int)addrlen);
return 2;
}

ns = (struct nameserver *) mm_malloc(sizeof(struct nameserver));
if (!ns) return -1;

memset(ns, 0, sizeof(struct nameserver));
ns->base = base;

evtimer_assign(&ns->timeout_event, ns->base->event_base, nameserver_prod_callback, ns);

ns->socket = evutil_socket_(address->sa_family,
SOCK_DGRAM|EVUTIL_SOCK_NONBLOCK|EVUTIL_SOCK_CLOEXEC, 0);
if (ns->socket < 0) { err = 1; goto out1; }

if (base->global_outgoing_addrlen &&
!evutil_sockaddr_is_loopback_(address)) {
if (bind(ns->socket,
(struct sockaddr*)&base->global_outgoing_address,
base->global_outgoing_addrlen) < 0) {
log(EVDNS_LOG_WARN,"Couldn't bind to outgoing address");
err = 2;
goto out2;
}
}

memcpy(&ns->address, address, addrlen);
ns->addrlen = addrlen;
ns->state = 1;
event_assign(&ns->event, ns->base->event_base, ns->socket,
EV_READ | EV_PERSIST, nameserver_ready_callback, ns);
if (!base->disable_when_inactive && event_add(&ns->event, NULL) < 0) {
err = 2;
goto out2;
}

log(EVDNS_LOG_DEBUG, "Added nameserver %s as %p",
evutil_format_sockaddr_port_(address, addrbuf, sizeof(addrbuf)), ns);


if (!base->server_head) {
ns->next = ns->prev = ns;
base->server_head = ns;
} else {
ns->next = base->server_head->next;
ns->prev = base->server_head;
base->server_head->next = ns;
ns->next->prev = ns;
}

base->global_good_nameservers++;

return 0;

out2:
evutil_closesocket(ns->socket);
out1:
event_debug_unassign(&ns->event);
mm_free(ns);
log(EVDNS_LOG_WARN, "Unable to add nameserver %s: error %d",
evutil_format_sockaddr_port_(address, addrbuf, sizeof(addrbuf)), err);
return err;
}


int
evdns_base_nameserver_add(struct evdns_base *base, unsigned long int address)
{
struct sockaddr_in sin;
int res;
memset(&sin, 0, sizeof(sin));
sin.sin_addr.s_addr = address;
sin.sin_port = htons(53);
sin.sin_family = AF_INET;
EVDNS_LOCK(base);
res = evdns_nameserver_add_impl_(base, (struct sockaddr*)&sin, sizeof(sin));
EVDNS_UNLOCK(base);
return res;
}

int
evdns_nameserver_add(unsigned long int address) {
if (!current_base)
current_base = evdns_base_new(NULL, 0);
return evdns_base_nameserver_add(current_base, address);
}

static void
sockaddr_setport(struct sockaddr *sa, ev_uint16_t port)
{
if (sa->sa_family == AF_INET) {
((struct sockaddr_in *)sa)->sin_port = htons(port);
} else if (sa->sa_family == AF_INET6) {
((struct sockaddr_in6 *)sa)->sin6_port = htons(port);
}
}

static ev_uint16_t
sockaddr_getport(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return ntohs(((struct sockaddr_in *)sa)->sin_port);
} else if (sa->sa_family == AF_INET6) {
return ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
} else {
return 0;
}
}


int
evdns_base_nameserver_ip_add(struct evdns_base *base, const char *ip_as_string) {
struct sockaddr_storage ss;
struct sockaddr *sa;
int len = sizeof(ss);
int res;
if (evutil_parse_sockaddr_port(ip_as_string, (struct sockaddr *)&ss,
&len)) {
log(EVDNS_LOG_WARN, "Unable to parse nameserver address %s",
ip_as_string);
return 4;
}
sa = (struct sockaddr *) &ss;
if (sockaddr_getport(sa) == 0)
sockaddr_setport(sa, 53);

EVDNS_LOCK(base);
res = evdns_nameserver_add_impl_(base, sa, len);
EVDNS_UNLOCK(base);
return res;
}

int
evdns_nameserver_ip_add(const char *ip_as_string) {
if (!current_base)
current_base = evdns_base_new(NULL, 0);
return evdns_base_nameserver_ip_add(current_base, ip_as_string);
}

int
evdns_base_nameserver_sockaddr_add(struct evdns_base *base,
const struct sockaddr *sa, ev_socklen_t len, unsigned flags)
{
int res;
EVUTIL_ASSERT(base);
EVDNS_LOCK(base);
res = evdns_nameserver_add_impl_(base, sa, len);
EVDNS_UNLOCK(base);
return res;
}

int
evdns_base_get_nameserver_addr(struct evdns_base *base, int idx,
struct sockaddr *sa, ev_socklen_t len)
{
int result = -1;
int i;
struct nameserver *server;
EVDNS_LOCK(base);
server = base->server_head;
for (i = 0; i < idx && server; ++i, server = server->next) {
if (server->next == base->server_head)
goto done;
}
if (! server)
goto done;

if (server->addrlen > len) {
result = (int) server->addrlen;
goto done;
}

memcpy(sa, &server->address, server->addrlen);
result = (int) server->addrlen;
done:
EVDNS_UNLOCK(base);
return result;
}


static void
evdns_request_remove(struct request *req, struct request **head)
{
ASSERT_LOCKED(req->base);
ASSERT_VALID_REQUEST(req);

#if 0
{
struct request *ptr;
int found = 0;
EVUTIL_ASSERT(*head != NULL);

ptr = *head;
do {
if (ptr == req) {
found = 1;
break;
}
ptr = ptr->next;
} while (ptr != *head);
EVUTIL_ASSERT(found);

EVUTIL_ASSERT(req->next);
}
#endif

if (req->next == req) {

*head = NULL;
} else {
req->next->prev = req->prev;
req->prev->next = req->next;
if (*head == req) *head = req->next;
}
req->next = req->prev = NULL;
}


static void
evdns_request_insert(struct request *req, struct request **head) {
ASSERT_LOCKED(req->base);
ASSERT_VALID_REQUEST(req);
if (!*head) {
*head = req;
req->next = req->prev = req;
return;
}

req->prev = (*head)->prev;
req->prev->next = req;
req->next = *head;
(*head)->prev = req;
}

static int
string_num_dots(const char *s) {
int count = 0;
while ((s = strchr(s, '.'))) {
s++;
count++;
}
return count;
}

static struct request *
request_new(struct evdns_base *base, struct evdns_request *handle, int type,
const char *name, int flags, evdns_callback_type callback,
void *user_ptr) {

const char issuing_now =
(base->global_requests_inflight < base->global_max_requests_inflight) ? 1 : 0;

const size_t name_len = strlen(name);
const size_t request_max_len = evdns_request_len(name_len);
const u16 trans_id = issuing_now ? transaction_id_pick(base) : 0xffff;

struct request *const req =
mm_malloc(sizeof(struct request) + request_max_len);
int rlen;
char namebuf[256];
(void) flags;

ASSERT_LOCKED(base);

if (!req) return NULL;

if (name_len >= sizeof(namebuf)) {
mm_free(req);
return NULL;
}

memset(req, 0, sizeof(struct request));
req->base = base;

evtimer_assign(&req->timeout_event, req->base->event_base, evdns_request_timeout_callback, req);

if (base->global_randomize_case) {
unsigned i;
char randbits[(sizeof(namebuf)+7)/8];
strlcpy(namebuf, name, sizeof(namebuf));
evutil_secure_rng_get_bytes(randbits, (name_len+7)/8);
for (i = 0; i < name_len; ++i) {
if (EVUTIL_ISALPHA_(namebuf[i])) {
if ((randbits[i >> 3] & (1<<(i & 7))))
namebuf[i] |= 0x20;
else
namebuf[i] &= ~0x20;
}
}
name = namebuf;
}


req->request = ((u8 *) req) + sizeof(struct request);

req->request_appended = 1;
rlen = evdns_request_data_build(name, name_len, trans_id,
type, CLASS_INET, req->request, request_max_len);
if (rlen < 0)
goto err1;

req->request_len = rlen;
req->trans_id = trans_id;
req->tx_count = 0;
req->request_type = type;
req->user_pointer = user_ptr;
req->user_callback = callback;
req->ns = issuing_now ? nameserver_pick(base) : NULL;
req->next = req->prev = NULL;
req->handle = handle;
if (handle) {
handle->current_req = req;
handle->base = base;
}

return req;
err1:
mm_free(req);
return NULL;
}

static void
request_submit(struct request *const req) {
struct evdns_base *base = req->base;
ASSERT_LOCKED(base);
ASSERT_VALID_REQUEST(req);
if (req->ns) {


evdns_request_insert(req, &REQ_HEAD(base, req->trans_id));

base->global_requests_inflight++;
req->ns->requests_inflight++;

evdns_request_transmit(req);
} else {
evdns_request_insert(req, &base->req_waiting_head);
base->global_requests_waiting++;
}
}


void
evdns_cancel_request(struct evdns_base *base, struct evdns_request *handle)
{
struct request *req;

if (!handle->current_req)
return;

if (!base) {

base = handle->base;
if (!base)
base = handle->current_req->base;
}

EVDNS_LOCK(base);
if (handle->pending_cb) {
EVDNS_UNLOCK(base);
return;
}

req = handle->current_req;
ASSERT_VALID_REQUEST(req);

reply_schedule_callback(req, 0, DNS_ERR_CANCEL, NULL);
if (req->ns) {

request_finished(req, &REQ_HEAD(base, req->trans_id), 1);
} else {

request_finished(req, &base->req_waiting_head, 1);
}
EVDNS_UNLOCK(base);
}


struct evdns_request *
evdns_base_resolve_ipv4(struct evdns_base *base, const char *name, int flags,
evdns_callback_type callback, void *ptr) {
struct evdns_request *handle;
struct request *req;
log(EVDNS_LOG_DEBUG, "Resolve requested for %s", name);
handle = mm_calloc(1, sizeof(*handle));
if (handle == NULL)
return NULL;
EVDNS_LOCK(base);
if (flags & DNS_QUERY_NO_SEARCH) {
req =
request_new(base, handle, TYPE_A, name, flags,
callback, ptr);
if (req)
request_submit(req);
} else {
search_request_new(base, handle, TYPE_A, name, flags,
callback, ptr);
}
if (handle->current_req == NULL) {
mm_free(handle);
handle = NULL;
}
EVDNS_UNLOCK(base);
return handle;
}

int evdns_resolve_ipv4(const char *name, int flags,
evdns_callback_type callback, void *ptr)
{
return evdns_base_resolve_ipv4(current_base, name, flags, callback, ptr)
? 0 : -1;
}



struct evdns_request *
evdns_base_resolve_ipv6(struct evdns_base *base,
const char *name, int flags,
evdns_callback_type callback, void *ptr)
{
struct evdns_request *handle;
struct request *req;
log(EVDNS_LOG_DEBUG, "Resolve requested for %s", name);
handle = mm_calloc(1, sizeof(*handle));
if (handle == NULL)
return NULL;
EVDNS_LOCK(base);
if (flags & DNS_QUERY_NO_SEARCH) {
req = request_new(base, handle, TYPE_AAAA, name, flags,
callback, ptr);
if (req)
request_submit(req);
} else {
search_request_new(base, handle, TYPE_AAAA, name, flags,
callback, ptr);
}
if (handle->current_req == NULL) {
mm_free(handle);
handle = NULL;
}
EVDNS_UNLOCK(base);
return handle;
}

int evdns_resolve_ipv6(const char *name, int flags,
evdns_callback_type callback, void *ptr) {
return evdns_base_resolve_ipv6(current_base, name, flags, callback, ptr)
? 0 : -1;
}

struct evdns_request *
evdns_base_resolve_reverse(struct evdns_base *base, const struct in_addr *in, int flags, evdns_callback_type callback, void *ptr) {
char buf[32];
struct evdns_request *handle;
struct request *req;
u32 a;
EVUTIL_ASSERT(in);
a = ntohl(in->s_addr);
evutil_snprintf(buf, sizeof(buf), "%d.%d.%d.%d.in-addr.arpa",
(int)(u8)((a )&0xff),
(int)(u8)((a>>8 )&0xff),
(int)(u8)((a>>16)&0xff),
(int)(u8)((a>>24)&0xff));
handle = mm_calloc(1, sizeof(*handle));
if (handle == NULL)
return NULL;
log(EVDNS_LOG_DEBUG, "Resolve requested for %s (reverse)", buf);
EVDNS_LOCK(base);
req = request_new(base, handle, TYPE_PTR, buf, flags, callback, ptr);
if (req)
request_submit(req);
if (handle->current_req == NULL) {
mm_free(handle);
handle = NULL;
}
EVDNS_UNLOCK(base);
return (handle);
}

int evdns_resolve_reverse(const struct in_addr *in, int flags, evdns_callback_type callback, void *ptr) {
return evdns_base_resolve_reverse(current_base, in, flags, callback, ptr)
? 0 : -1;
}

struct evdns_request *
evdns_base_resolve_reverse_ipv6(struct evdns_base *base, const struct in6_addr *in, int flags, evdns_callback_type callback, void *ptr) {

char buf[73];
char *cp;
struct evdns_request *handle;
struct request *req;
int i;
EVUTIL_ASSERT(in);
cp = buf;
for (i=15; i >= 0; --i) {
u8 byte = in->s6_addr[i];
*cp++ = "0123456789abcdef"[byte & 0x0f];
*cp++ = '.';
*cp++ = "0123456789abcdef"[byte >> 4];
*cp++ = '.';
}
EVUTIL_ASSERT(cp + strlen("ip6.arpa") < buf+sizeof(buf));
memcpy(cp, "ip6.arpa", strlen("ip6.arpa")+1);
handle = mm_calloc(1, sizeof(*handle));
if (handle == NULL)
return NULL;
log(EVDNS_LOG_DEBUG, "Resolve requested for %s (reverse)", buf);
EVDNS_LOCK(base);
req = request_new(base, handle, TYPE_PTR, buf, flags, callback, ptr);
if (req)
request_submit(req);
if (handle->current_req == NULL) {
mm_free(handle);
handle = NULL;
}
EVDNS_UNLOCK(base);
return (handle);
}

int evdns_resolve_reverse_ipv6(const struct in6_addr *in, int flags, evdns_callback_type callback, void *ptr) {
return evdns_base_resolve_reverse_ipv6(current_base, in, flags, callback, ptr)
? 0 : -1;
}














struct search_domain {
int len;
struct search_domain *next;

};

struct search_state {
int refcount;
int ndots;
int num_domains;
struct search_domain *head;
};

static void
search_state_decref(struct search_state *const state) {
if (!state) return;
state->refcount--;
if (!state->refcount) {
struct search_domain *next, *dom;
for (dom = state->head; dom; dom = next) {
next = dom->next;
mm_free(dom);
}
mm_free(state);
}
}

static struct search_state *
search_state_new(void) {
struct search_state *state = (struct search_state *) mm_malloc(sizeof(struct search_state));
if (!state) return NULL;
memset(state, 0, sizeof(struct search_state));
state->refcount = 1;
state->ndots = 1;

return state;
}

static void
search_postfix_clear(struct evdns_base *base) {
search_state_decref(base->global_search_state);

base->global_search_state = search_state_new();
}


void
evdns_base_search_clear(struct evdns_base *base)
{
EVDNS_LOCK(base);
search_postfix_clear(base);
EVDNS_UNLOCK(base);
}

void
evdns_search_clear(void) {
evdns_base_search_clear(current_base);
}

static void
search_postfix_add(struct evdns_base *base, const char *domain) {
size_t domain_len;
struct search_domain *sdomain;
while (domain[0] == '.') domain++;
domain_len = strlen(domain);

ASSERT_LOCKED(base);
if (!base->global_search_state) base->global_search_state = search_state_new();
if (!base->global_search_state) return;
base->global_search_state->num_domains++;

sdomain = (struct search_domain *) mm_malloc(sizeof(struct search_domain) + domain_len);
if (!sdomain) return;
memcpy( ((u8 *) sdomain) + sizeof(struct search_domain), domain, domain_len);
sdomain->next = base->global_search_state->head;
sdomain->len = (int) domain_len;

base->global_search_state->head = sdomain;
}



static void
search_reverse(struct evdns_base *base) {
struct search_domain *cur, *prev = NULL, *next;
ASSERT_LOCKED(base);
cur = base->global_search_state->head;
while (cur) {
next = cur->next;
cur->next = prev;
prev = cur;
cur = next;
}

base->global_search_state->head = prev;
}


void
evdns_base_search_add(struct evdns_base *base, const char *domain) {
EVDNS_LOCK(base);
search_postfix_add(base, domain);
EVDNS_UNLOCK(base);
}
void
evdns_search_add(const char *domain) {
evdns_base_search_add(current_base, domain);
}


void
evdns_base_search_ndots_set(struct evdns_base *base, const int ndots) {
EVDNS_LOCK(base);
if (!base->global_search_state) base->global_search_state = search_state_new();
if (base->global_search_state)
base->global_search_state->ndots = ndots;
EVDNS_UNLOCK(base);
}
void
evdns_search_ndots_set(const int ndots) {
evdns_base_search_ndots_set(current_base, ndots);
}

static void
search_set_from_hostname(struct evdns_base *base) {
char hostname[HOST_NAME_MAX + 1], *domainname;

ASSERT_LOCKED(base);
search_postfix_clear(base);
if (gethostname(hostname, sizeof(hostname))) return;
domainname = strchr(hostname, '.');
if (!domainname) return;
search_postfix_add(base, domainname);
}


static char *
search_make_new(const struct search_state *const state, int n, const char *const base_name) {
const size_t base_len = strlen(base_name);
char need_to_append_dot;
struct search_domain *dom;

if (!base_len) return NULL;
need_to_append_dot = base_name[base_len - 1] == '.' ? 0 : 1;

for (dom = state->head; dom; dom = dom->next) {
if (!n--) {


const u8 *const postfix = ((u8 *) dom) + sizeof(struct search_domain);
const int postfix_len = dom->len;
char *const newname = (char *) mm_malloc(base_len + need_to_append_dot + postfix_len + 1);
if (!newname) return NULL;
memcpy(newname, base_name, base_len);
if (need_to_append_dot) newname[base_len] = '.';
memcpy(newname + base_len + need_to_append_dot, postfix, postfix_len);
newname[base_len + need_to_append_dot + postfix_len] = 0;
return newname;
}
}


EVUTIL_ASSERT(0);
return NULL;
}

static struct request *
search_request_new(struct evdns_base *base, struct evdns_request *handle,
int type, const char *const name, int flags,
evdns_callback_type user_callback, void *user_arg) {
ASSERT_LOCKED(base);
EVUTIL_ASSERT(type == TYPE_A || type == TYPE_AAAA);
EVUTIL_ASSERT(handle->current_req == NULL);
if ( ((flags & DNS_QUERY_NO_SEARCH) == 0) &&
base->global_search_state &&
base->global_search_state->num_domains) {

struct request *req;
if (string_num_dots(name) >= base->global_search_state->ndots) {
req = request_new(base, handle, type, name, flags, user_callback, user_arg);
if (!req) return NULL;
handle->search_index = -1;
} else {
char *const new_name = search_make_new(base->global_search_state, 0, name);
if (!new_name) return NULL;
req = request_new(base, handle, type, new_name, flags, user_callback, user_arg);
mm_free(new_name);
if (!req) return NULL;
handle->search_index = 0;
}
EVUTIL_ASSERT(handle->search_origname == NULL);
handle->search_origname = mm_strdup(name);
if (handle->search_origname == NULL) {

if (req)
mm_free(req);
return NULL;
}
handle->search_state = base->global_search_state;
handle->search_flags = flags;
base->global_search_state->refcount++;
request_submit(req);
return req;
} else {
struct request *const req = request_new(base, handle, type, name, flags, user_callback, user_arg);
if (!req) return NULL;
request_submit(req);
return req;
}
}






static int
search_try_next(struct evdns_request *const handle) {
struct request *req = handle->current_req;
struct evdns_base *base = req->base;
struct request *newreq;
ASSERT_LOCKED(base);
if (handle->search_state) {

char *new_name;
handle->search_index++;
if (handle->search_index >= handle->search_state->num_domains) {


if (string_num_dots(handle->search_origname) < handle->search_state->ndots) {

newreq = request_new(base, NULL, req->request_type, handle->search_origname, handle->search_flags, req->user_callback, req->user_pointer);
log(EVDNS_LOG_DEBUG, "Search: trying raw query %s", handle->search_origname);
if (newreq) {
search_request_finished(handle);
goto submit_next;
}
}
return 1;
}

new_name = search_make_new(handle->search_state, handle->search_index, handle->search_origname);
if (!new_name) return 1;
log(EVDNS_LOG_DEBUG, "Search: now trying %s (%d)", new_name, handle->search_index);
newreq = request_new(base, NULL, req->request_type, new_name, handle->search_flags, req->user_callback, req->user_pointer);
mm_free(new_name);
if (!newreq) return 1;
goto submit_next;
}
return 1;

submit_next:
request_finished(req, &REQ_HEAD(req->base, req->trans_id), 0);
handle->current_req = newreq;
newreq->handle = handle;
request_submit(newreq);
return 0;
}

static void
search_request_finished(struct evdns_request *const handle) {
ASSERT_LOCKED(handle->current_req->base);
if (handle->search_state) {
search_state_decref(handle->search_state);
handle->search_state = NULL;
}
if (handle->search_origname) {
mm_free(handle->search_origname);
handle->search_origname = NULL;
}
}




static void
evdns_resolv_set_defaults(struct evdns_base *base, int flags) {

ASSERT_LOCKED(base);
if (flags & DNS_OPTION_SEARCH) search_set_from_hostname(base);
if (flags & DNS_OPTION_NAMESERVERS) evdns_base_nameserver_ip_add(base,"127.0.0.1");
}

#if !defined(EVENT__HAVE_STRTOK_R)
static char *
strtok_r(char *s, const char *delim, char **state) {
char *cp, *start;
start = cp = s ? s : *state;
if (!cp)
return NULL;
while (*cp && !strchr(delim, *cp))
++cp;
if (!*cp) {
if (cp == start)
return NULL;
*state = NULL;
return start;
} else {
*cp++ = '\0';
*state = cp;
return start;
}
}
#endif


static int
strtoint(const char *const str)
{
char *endptr;
const int r = strtol(str, &endptr, 10);
if (*endptr) return -1;
return r;
}


static int
evdns_strtotimeval(const char *const str, struct timeval *out)
{
double d;
char *endptr;
d = strtod(str, &endptr);
if (*endptr) return -1;
if (d < 0) return -1;
out->tv_sec = (int) d;
out->tv_usec = (int) ((d - (int) d)*1000000);
if (out->tv_sec == 0 && out->tv_usec < 1000)
return -1;
return 0;
}


static int
strtoint_clipped(const char *const str, int min, int max)
{
int r = strtoint(str);
if (r == -1)
return r;
else if (r<min)
return min;
else if (r>max)
return max;
else
return r;
}

static int
evdns_base_set_max_requests_inflight(struct evdns_base *base, int maxinflight)
{
int old_n_heads = base->n_req_heads, n_heads;
struct request **old_heads = base->req_heads, **new_heads, *req;
int i;

ASSERT_LOCKED(base);
if (maxinflight < 1)
maxinflight = 1;
n_heads = (maxinflight+4) / 5;
EVUTIL_ASSERT(n_heads > 0);
new_heads = mm_calloc(n_heads, sizeof(struct request*));
if (!new_heads)
return (-1);
if (old_heads) {
for (i = 0; i < old_n_heads; ++i) {
while (old_heads[i]) {
req = old_heads[i];
evdns_request_remove(req, &old_heads[i]);
evdns_request_insert(req, &new_heads[req->trans_id % n_heads]);
}
}
mm_free(old_heads);
}
base->req_heads = new_heads;
base->n_req_heads = n_heads;
base->global_max_requests_inflight = maxinflight;
return (0);
}


int
evdns_base_set_option(struct evdns_base *base,
const char *option, const char *val)
{
int res;
EVDNS_LOCK(base);
res = evdns_base_set_option_impl(base, option, val, DNS_OPTIONS_ALL);
EVDNS_UNLOCK(base);
return res;
}

static inline int
str_matches_option(const char *s1, const char *optionname)
{



size_t optlen = strlen(optionname);
size_t slen = strlen(s1);
if (slen == optlen || slen == optlen - 1)
return !strncmp(s1, optionname, slen);
else if (slen > optlen)
return !strncmp(s1, optionname, optlen);
else
return 0;
}

static int
evdns_base_set_option_impl(struct evdns_base *base,
const char *option, const char *val, int flags)
{
ASSERT_LOCKED(base);
if (str_matches_option(option, "ndots:")) {
const int ndots = strtoint(val);
if (ndots == -1) return -1;
if (!(flags & DNS_OPTION_SEARCH)) return 0;
log(EVDNS_LOG_DEBUG, "Setting ndots to %d", ndots);
if (!base->global_search_state) base->global_search_state = search_state_new();
if (!base->global_search_state) return -1;
base->global_search_state->ndots = ndots;
} else if (str_matches_option(option, "timeout:")) {
struct timeval tv;
if (evdns_strtotimeval(val, &tv) == -1) return -1;
if (!(flags & DNS_OPTION_MISC)) return 0;
log(EVDNS_LOG_DEBUG, "Setting timeout to %s", val);
memcpy(&base->global_timeout, &tv, sizeof(struct timeval));
} else if (str_matches_option(option, "getaddrinfo-allow-skew:")) {
struct timeval tv;
if (evdns_strtotimeval(val, &tv) == -1) return -1;
if (!(flags & DNS_OPTION_MISC)) return 0;
log(EVDNS_LOG_DEBUG, "Setting getaddrinfo-allow-skew to %s",
val);
memcpy(&base->global_getaddrinfo_allow_skew, &tv,
sizeof(struct timeval));
} else if (str_matches_option(option, "max-timeouts:")) {
const int maxtimeout = strtoint_clipped(val, 1, 255);
if (maxtimeout == -1) return -1;
if (!(flags & DNS_OPTION_MISC)) return 0;
log(EVDNS_LOG_DEBUG, "Setting maximum allowed timeouts to %d",
maxtimeout);
base->global_max_nameserver_timeout = maxtimeout;
} else if (str_matches_option(option, "max-inflight:")) {
const int maxinflight = strtoint_clipped(val, 1, 65000);
if (maxinflight == -1) return -1;
if (!(flags & DNS_OPTION_MISC)) return 0;
log(EVDNS_LOG_DEBUG, "Setting maximum inflight requests to %d",
maxinflight);
evdns_base_set_max_requests_inflight(base, maxinflight);
} else if (str_matches_option(option, "attempts:")) {
int retries = strtoint(val);
if (retries == -1) return -1;
if (retries > 255) retries = 255;
if (!(flags & DNS_OPTION_MISC)) return 0;
log(EVDNS_LOG_DEBUG, "Setting retries to %d", retries);
base->global_max_retransmits = retries;
} else if (str_matches_option(option, "randomize-case:")) {
int randcase = strtoint(val);
if (!(flags & DNS_OPTION_MISC)) return 0;
base->global_randomize_case = randcase;
} else if (str_matches_option(option, "bind-to:")) {


int len = sizeof(base->global_outgoing_address);
if (!(flags & DNS_OPTION_NAMESERVERS)) return 0;
if (evutil_parse_sockaddr_port(val,
(struct sockaddr*)&base->global_outgoing_address, &len))
return -1;
base->global_outgoing_addrlen = len;
} else if (str_matches_option(option, "initial-probe-timeout:")) {
struct timeval tv;
if (evdns_strtotimeval(val, &tv) == -1) return -1;
if (tv.tv_sec > 3600)
tv.tv_sec = 3600;
if (!(flags & DNS_OPTION_MISC)) return 0;
log(EVDNS_LOG_DEBUG, "Setting initial probe timeout to %s",
val);
memcpy(&base->global_nameserver_probe_initial_timeout, &tv,
sizeof(tv));
}
return 0;
}

int
evdns_set_option(const char *option, const char *val, int flags)
{
if (!current_base)
current_base = evdns_base_new(NULL, 0);
return evdns_base_set_option(current_base, option, val);
}

static void
resolv_conf_parse_line(struct evdns_base *base, char *const start, int flags) {
char *strtok_state;
static const char *const delims = " \t";
#define NEXT_TOKEN strtok_r(NULL, delims, &strtok_state)


char *const first_token = strtok_r(start, delims, &strtok_state);
ASSERT_LOCKED(base);
if (!first_token) return;

if (!strcmp(first_token, "nameserver") && (flags & DNS_OPTION_NAMESERVERS)) {
const char *const nameserver = NEXT_TOKEN;

if (nameserver)
evdns_base_nameserver_ip_add(base, nameserver);
} else if (!strcmp(first_token, "domain") && (flags & DNS_OPTION_SEARCH)) {
const char *const domain = NEXT_TOKEN;
if (domain) {
search_postfix_clear(base);
search_postfix_add(base, domain);
}
} else if (!strcmp(first_token, "search") && (flags & DNS_OPTION_SEARCH)) {
const char *domain;
search_postfix_clear(base);

while ((domain = NEXT_TOKEN)) {
search_postfix_add(base, domain);
}
search_reverse(base);
} else if (!strcmp(first_token, "options")) {
const char *option;
while ((option = NEXT_TOKEN)) {
const char *val = strchr(option, ':');
evdns_base_set_option_impl(base, option, val ? val+1 : "", flags);
}
}
#undef NEXT_TOKEN
}









int
evdns_base_resolv_conf_parse(struct evdns_base *base, int flags, const char *const filename) {
int res;
EVDNS_LOCK(base);
res = evdns_base_resolv_conf_parse_impl(base, flags, filename);
EVDNS_UNLOCK(base);
return res;
}

static char *
evdns_get_default_hosts_filename(void)
{
#if defined(_WIN32)





char path[MAX_PATH+1];
static const char hostfile[] = "\\drivers\\etc\\hosts";
char *path_out;
size_t len_out;

if (! SHGetSpecialFolderPathA(NULL, path, CSIDL_SYSTEM, 0))
return NULL;
len_out = strlen(path)+strlen(hostfile)+1;
path_out = mm_malloc(len_out);
evutil_snprintf(path_out, len_out, "%s%s", path, hostfile);
return path_out;
#else
return mm_strdup("/etc/hosts");
#endif
}

static int
evdns_base_resolv_conf_parse_impl(struct evdns_base *base, int flags, const char *const filename) {
size_t n;
char *resolv;
char *start;
int err = 0;

log(EVDNS_LOG_DEBUG, "Parsing resolv.conf file %s", filename);

if (flags & DNS_OPTION_HOSTSFILE) {
char *fname = evdns_get_default_hosts_filename();
evdns_base_load_hosts(base, fname);
if (fname)
mm_free(fname);
}

if ((err = evutil_read_file_(filename, &resolv, &n, 0)) < 0) {
if (err == -1) {

evdns_resolv_set_defaults(base, flags);
return 1;
} else {
return 2;
}
}

start = resolv;
for (;;) {
char *const newline = strchr(start, '\n');
if (!newline) {
resolv_conf_parse_line(base, start, flags);
break;
} else {
*newline = 0;
resolv_conf_parse_line(base, start, flags);
start = newline + 1;
}
}

if (!base->server_head && (flags & DNS_OPTION_NAMESERVERS)) {

evdns_base_nameserver_ip_add(base, "127.0.0.1");
err = 6;
}
if (flags & DNS_OPTION_SEARCH && (!base->global_search_state || base->global_search_state->num_domains == 0)) {
search_set_from_hostname(base);
}

mm_free(resolv);
return err;
}

int
evdns_resolv_conf_parse(int flags, const char *const filename) {
if (!current_base)
current_base = evdns_base_new(NULL, 0);
return evdns_base_resolv_conf_parse(current_base, flags, filename);
}


#if defined(_WIN32)

static int
evdns_nameserver_ip_add_line(struct evdns_base *base, const char *ips) {
const char *addr;
char *buf;
int r;
ASSERT_LOCKED(base);
while (*ips) {
while (isspace(*ips) || *ips == ',' || *ips == '\t')
++ips;
addr = ips;
while (isdigit(*ips) || *ips == '.' || *ips == ':' ||
*ips=='[' || *ips==']')
++ips;
buf = mm_malloc(ips-addr+1);
if (!buf) return 4;
memcpy(buf, addr, ips-addr);
buf[ips-addr] = '\0';
r = evdns_base_nameserver_ip_add(base, buf);
mm_free(buf);
if (r) return r;
}
return 0;
}

typedef DWORD(WINAPI *GetNetworkParams_fn_t)(FIXED_INFO *, DWORD*);



static int
load_nameservers_with_getnetworkparams(struct evdns_base *base)
{

FIXED_INFO *fixed;
HMODULE handle = 0;
ULONG size = sizeof(FIXED_INFO);
void *buf = NULL;
int status = 0, r, added_any;
IP_ADDR_STRING *ns;
GetNetworkParams_fn_t fn;

ASSERT_LOCKED(base);
if (!(handle = evutil_load_windows_system_library_(
TEXT("iphlpapi.dll")))) {
log(EVDNS_LOG_WARN, "Could not open iphlpapi.dll");
status = -1;
goto done;
}
if (!(fn = (GetNetworkParams_fn_t) GetProcAddress(handle, "GetNetworkParams"))) {
log(EVDNS_LOG_WARN, "Could not get address of function.");
status = -1;
goto done;
}

buf = mm_malloc(size);
if (!buf) { status = 4; goto done; }
fixed = buf;
r = fn(fixed, &size);
if (r != ERROR_SUCCESS && r != ERROR_BUFFER_OVERFLOW) {
status = -1;
goto done;
}
if (r != ERROR_SUCCESS) {
mm_free(buf);
buf = mm_malloc(size);
if (!buf) { status = 4; goto done; }
fixed = buf;
r = fn(fixed, &size);
if (r != ERROR_SUCCESS) {
log(EVDNS_LOG_DEBUG, "fn() failed.");
status = -1;
goto done;
}
}

EVUTIL_ASSERT(fixed);
added_any = 0;
ns = &(fixed->DnsServerList);
while (ns) {
r = evdns_nameserver_ip_add_line(base, ns->IpAddress.String);
if (r) {
log(EVDNS_LOG_DEBUG,"Could not add nameserver %s to list,error: %d",
(ns->IpAddress.String),(int)GetLastError());
status = r;
} else {
++added_any;
log(EVDNS_LOG_DEBUG,"Successfully added %s as nameserver",ns->IpAddress.String);
}

ns = ns->Next;
}

if (!added_any) {
log(EVDNS_LOG_DEBUG, "No nameservers added.");
if (status == 0)
status = -1;
} else {
status = 0;
}

done:
if (buf)
mm_free(buf);
if (handle)
FreeLibrary(handle);
return status;
}

static int
config_nameserver_from_reg_key(struct evdns_base *base, HKEY key, const TCHAR *subkey)
{
char *buf;
DWORD bufsz = 0, type = 0;
int status = 0;

ASSERT_LOCKED(base);
if (RegQueryValueEx(key, subkey, 0, &type, NULL, &bufsz)
!= ERROR_MORE_DATA)
return -1;
if (!(buf = mm_malloc(bufsz)))
return -1;

if (RegQueryValueEx(key, subkey, 0, &type, (LPBYTE)buf, &bufsz)
== ERROR_SUCCESS && bufsz > 1) {
status = evdns_nameserver_ip_add_line(base,buf);
}

mm_free(buf);
return status;
}

#define SERVICES_KEY TEXT("System\\CurrentControlSet\\Services\\")
#define WIN_NS_9X_KEY SERVICES_KEY TEXT("VxD\\MSTCP")
#define WIN_NS_NT_KEY SERVICES_KEY TEXT("Tcpip\\Parameters")

static int
load_nameservers_from_registry(struct evdns_base *base)
{
int found = 0;
int r;
#define TRY(k, name) if (!found && config_nameserver_from_reg_key(base,k,TEXT(name)) == 0) { log(EVDNS_LOG_DEBUG,"Found nameservers in %s/%s",#k,name); found = 1; } else if (!found) { log(EVDNS_LOG_DEBUG,"Didn't find nameservers in %s/%s", #k,#name); }








ASSERT_LOCKED(base);

if (((int)GetVersion()) > 0) {
HKEY nt_key = 0, interfaces_key = 0;

if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, WIN_NS_NT_KEY, 0,
KEY_READ, &nt_key) != ERROR_SUCCESS) {
log(EVDNS_LOG_DEBUG,"Couldn't open nt key, %d",(int)GetLastError());
return -1;
}
r = RegOpenKeyEx(nt_key, TEXT("Interfaces"), 0,
KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS,
&interfaces_key);
if (r != ERROR_SUCCESS) {
log(EVDNS_LOG_DEBUG,"Couldn't open interfaces key, %d",(int)GetLastError());
return -1;
}
TRY(nt_key, "NameServer");
TRY(nt_key, "DhcpNameServer");
TRY(interfaces_key, "NameServer");
TRY(interfaces_key, "DhcpNameServer");
RegCloseKey(interfaces_key);
RegCloseKey(nt_key);
} else {
HKEY win_key = 0;
if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, WIN_NS_9X_KEY, 0,
KEY_READ, &win_key) != ERROR_SUCCESS) {
log(EVDNS_LOG_DEBUG, "Couldn't open registry key, %d", (int)GetLastError());
return -1;
}
TRY(win_key, "NameServer");
RegCloseKey(win_key);
}

if (found == 0) {
log(EVDNS_LOG_WARN,"Didn't find any nameservers.");
}

return found ? 0 : -1;
#undef TRY
}

int
evdns_base_config_windows_nameservers(struct evdns_base *base)
{
int r;
char *fname;
if (base == NULL)
base = current_base;
if (base == NULL)
return -1;
EVDNS_LOCK(base);
fname = evdns_get_default_hosts_filename();
log(EVDNS_LOG_DEBUG, "Loading hosts entries from %s", fname);
evdns_base_load_hosts(base, fname);
if (fname)
mm_free(fname);

if (load_nameservers_with_getnetworkparams(base) == 0) {
EVDNS_UNLOCK(base);
return 0;
}
r = load_nameservers_from_registry(base);

EVDNS_UNLOCK(base);
return r;
}

int
evdns_config_windows_nameservers(void)
{
if (!current_base) {
current_base = evdns_base_new(NULL, 1);
return current_base == NULL ? -1 : 0;
} else {
return evdns_base_config_windows_nameservers(current_base);
}
}
#endif

struct evdns_base *
evdns_base_new(struct event_base *event_base, int flags)
{
struct evdns_base *base;

if (evutil_secure_rng_init() < 0) {
log(EVDNS_LOG_WARN, "Unable to seed random number generator; "
"DNS can't run.");
return NULL;
}




evutil_set_evdns_getaddrinfo_fn_(evdns_getaddrinfo);
evutil_set_evdns_getaddrinfo_cancel_fn_(evdns_getaddrinfo_cancel);

base = mm_malloc(sizeof(struct evdns_base));
if (base == NULL)
return (NULL);
memset(base, 0, sizeof(struct evdns_base));
base->req_waiting_head = NULL;

EVTHREAD_ALLOC_LOCK(base->lock, EVTHREAD_LOCKTYPE_RECURSIVE);
EVDNS_LOCK(base);


base->req_heads = NULL;

evdns_base_set_max_requests_inflight(base, 64);

base->server_head = NULL;
base->event_base = event_base;
base->global_good_nameservers = base->global_requests_inflight =
base->global_requests_waiting = 0;

base->global_timeout.tv_sec = 5;
base->global_timeout.tv_usec = 0;
base->global_max_reissues = 1;
base->global_max_retransmits = 3;
base->global_max_nameserver_timeout = 3;
base->global_search_state = NULL;
base->global_randomize_case = 1;
base->global_getaddrinfo_allow_skew.tv_sec = 3;
base->global_getaddrinfo_allow_skew.tv_usec = 0;
base->global_nameserver_probe_initial_timeout.tv_sec = 10;
base->global_nameserver_probe_initial_timeout.tv_usec = 0;

TAILQ_INIT(&base->hostsdb);

#define EVDNS_BASE_ALL_FLAGS (0x8001)
if (flags & ~EVDNS_BASE_ALL_FLAGS) {
flags = EVDNS_BASE_INITIALIZE_NAMESERVERS;
log(EVDNS_LOG_WARN,
"Unrecognized flag passed to evdns_base_new(). Assuming "
"you meant EVDNS_BASE_INITIALIZE_NAMESERVERS.");
}
#undef EVDNS_BASE_ALL_FLAGS

if (flags & EVDNS_BASE_INITIALIZE_NAMESERVERS) {
int r;
#if defined(_WIN32)
r = evdns_base_config_windows_nameservers(base);
#else
r = evdns_base_resolv_conf_parse(base, DNS_OPTIONS_ALL, "/etc/resolv.conf");
#endif
if (r == -1) {
evdns_base_free_and_unlock(base, 0);
return NULL;
}
}
if (flags & EVDNS_BASE_DISABLE_WHEN_INACTIVE) {
base->disable_when_inactive = 1;
}

EVDNS_UNLOCK(base);
return base;
}

int
evdns_init(void)
{
struct evdns_base *base = evdns_base_new(NULL, 1);
if (base) {
current_base = base;
return 0;
} else {
return -1;
}
}

const char *
evdns_err_to_string(int err)
{
switch (err) {
case DNS_ERR_NONE: return "no error";
case DNS_ERR_FORMAT: return "misformatted query";
case DNS_ERR_SERVERFAILED: return "server failed";
case DNS_ERR_NOTEXIST: return "name does not exist";
case DNS_ERR_NOTIMPL: return "query not implemented";
case DNS_ERR_REFUSED: return "refused";

case DNS_ERR_TRUNCATED: return "reply truncated or ill-formed";
case DNS_ERR_UNKNOWN: return "unknown";
case DNS_ERR_TIMEOUT: return "request timed out";
case DNS_ERR_SHUTDOWN: return "dns subsystem shut down";
case DNS_ERR_CANCEL: return "dns request canceled";
case DNS_ERR_NODATA: return "no records in the reply";
default: return "[Unknown error code]";
}
}

static void
evdns_nameserver_free(struct nameserver *server)
{
if (server->socket >= 0)
evutil_closesocket(server->socket);
(void) event_del(&server->event);
event_debug_unassign(&server->event);
if (server->state == 0)
(void) event_del(&server->timeout_event);
if (server->probe_request) {
evdns_cancel_request(server->base, server->probe_request);
server->probe_request = NULL;
}
event_debug_unassign(&server->timeout_event);
mm_free(server);
}

static void
evdns_base_free_and_unlock(struct evdns_base *base, int fail_requests)
{
struct nameserver *server, *server_next;
struct search_domain *dom, *dom_next;
int i;





for (i = 0; i < base->n_req_heads; ++i) {
while (base->req_heads[i]) {
if (fail_requests)
reply_schedule_callback(base->req_heads[i], 0, DNS_ERR_SHUTDOWN, NULL);
request_finished(base->req_heads[i], &REQ_HEAD(base, base->req_heads[i]->trans_id), 1);
}
}
while (base->req_waiting_head) {
if (fail_requests)
reply_schedule_callback(base->req_waiting_head, 0, DNS_ERR_SHUTDOWN, NULL);
request_finished(base->req_waiting_head, &base->req_waiting_head, 1);
}
base->global_requests_inflight = base->global_requests_waiting = 0;

for (server = base->server_head; server; server = server_next) {
server_next = server->next;

server->probe_request = NULL;
evdns_nameserver_free(server);
if (server_next == base->server_head)
break;
}
base->server_head = NULL;
base->global_good_nameservers = 0;

if (base->global_search_state) {
for (dom = base->global_search_state->head; dom; dom = dom_next) {
dom_next = dom->next;
mm_free(dom);
}
mm_free(base->global_search_state);
base->global_search_state = NULL;
}

{
struct hosts_entry *victim;
while ((victim = TAILQ_FIRST(&base->hostsdb))) {
TAILQ_REMOVE(&base->hostsdb, victim, next);
mm_free(victim);
}
}

mm_free(base->req_heads);

EVDNS_UNLOCK(base);
EVTHREAD_FREE_LOCK(base->lock, EVTHREAD_LOCKTYPE_RECURSIVE);

mm_free(base);
}

void
evdns_base_free(struct evdns_base *base, int fail_requests)
{
EVDNS_LOCK(base);
evdns_base_free_and_unlock(base, fail_requests);
}

void
evdns_base_clear_host_addresses(struct evdns_base *base)
{
struct hosts_entry *victim;
EVDNS_LOCK(base);
while ((victim = TAILQ_FIRST(&base->hostsdb))) {
TAILQ_REMOVE(&base->hostsdb, victim, next);
mm_free(victim);
}
EVDNS_UNLOCK(base);
}

void
evdns_shutdown(int fail_requests)
{
if (current_base) {
struct evdns_base *b = current_base;
current_base = NULL;
evdns_base_free(b, fail_requests);
}
evdns_log_fn = NULL;
}

static int
evdns_base_parse_hosts_line(struct evdns_base *base, char *line)
{
char *strtok_state;
static const char *const delims = " \t";
char *const addr = strtok_r(line, delims, &strtok_state);
char *hostname, *hash;
struct sockaddr_storage ss;
int socklen = sizeof(ss);
ASSERT_LOCKED(base);

#define NEXT_TOKEN strtok_r(NULL, delims, &strtok_state)

if (!addr || *addr == '#')
return 0;

memset(&ss, 0, sizeof(ss));
if (evutil_parse_sockaddr_port(addr, (struct sockaddr*)&ss, &socklen)<0)
return -1;
if (socklen > (int)sizeof(struct sockaddr_in6))
return -1;

if (sockaddr_getport((struct sockaddr*)&ss))
return -1;

while ((hostname = NEXT_TOKEN)) {
struct hosts_entry *he;
size_t namelen;
if ((hash = strchr(hostname, '#'))) {
if (hash == hostname)
return 0;
*hash = '\0';
}

namelen = strlen(hostname);

he = mm_calloc(1, sizeof(struct hosts_entry)+namelen);
if (!he)
return -1;
EVUTIL_ASSERT(socklen <= (int)sizeof(he->addr));
memcpy(&he->addr, &ss, socklen);
memcpy(he->hostname, hostname, namelen+1);
he->addrlen = socklen;

TAILQ_INSERT_TAIL(&base->hostsdb, he, next);

if (hash)
return 0;
}

return 0;
#undef NEXT_TOKEN
}

static int
evdns_base_load_hosts_impl(struct evdns_base *base, const char *hosts_fname)
{
char *str=NULL, *cp, *eol;
size_t len;
int err=0;

ASSERT_LOCKED(base);

if (hosts_fname == NULL ||
(err = evutil_read_file_(hosts_fname, &str, &len, 0)) < 0) {
char tmp[64];
strlcpy(tmp, "127.0.0.1 localhost", sizeof(tmp));
evdns_base_parse_hosts_line(base, tmp);
strlcpy(tmp, "::1 localhost", sizeof(tmp));
evdns_base_parse_hosts_line(base, tmp);
return err ? -1 : 0;
}



cp = str;
for (;;) {
eol = strchr(cp, '\n');

if (eol) {
*eol = '\0';
evdns_base_parse_hosts_line(base, cp);
cp = eol+1;
} else {
evdns_base_parse_hosts_line(base, cp);
break;
}
}

mm_free(str);
return 0;
}

int
evdns_base_load_hosts(struct evdns_base *base, const char *hosts_fname)
{
int res;
if (!base)
base = current_base;
EVDNS_LOCK(base);
res = evdns_base_load_hosts_impl(base, hosts_fname);
EVDNS_UNLOCK(base);
return res;
}


struct getaddrinfo_subrequest {
struct evdns_request *r;
ev_uint32_t type;
};


struct evdns_getaddrinfo_request {
struct evdns_base *evdns_base;


struct evutil_addrinfo hints;

evdns_getaddrinfo_cb user_cb;

void *user_data;

ev_uint16_t port;

struct getaddrinfo_subrequest ipv4_request;

struct getaddrinfo_subrequest ipv6_request;


char *cname_result;



struct evutil_addrinfo *pending_result;


struct event timeout;


int pending_error;

unsigned user_canceled : 1;


unsigned request_done : 1;
};


static int
evdns_err_to_getaddrinfo_err(int e1)
{

if (e1 == DNS_ERR_NONE)
return 0;
else if (e1 == DNS_ERR_NOTEXIST)
return EVUTIL_EAI_NONAME;
else
return EVUTIL_EAI_FAIL;
}


static int
getaddrinfo_merge_err(int e1, int e2)
{

if (e1 == 0)
return e2;
else
return e1;
}

static void
free_getaddrinfo_request(struct evdns_getaddrinfo_request *data)
{


if (data->pending_result)
evutil_freeaddrinfo(data->pending_result);
if (data->cname_result)
mm_free(data->cname_result);
event_del(&data->timeout);
mm_free(data);
return;
}

static void
add_cname_to_reply(struct evdns_getaddrinfo_request *data,
struct evutil_addrinfo *ai)
{
if (data->cname_result && ai) {
ai->ai_canonname = data->cname_result;
data->cname_result = NULL;
}
}





static void
evdns_getaddrinfo_timeout_cb(evutil_socket_t fd, short what, void *ptr)
{
int v4_timedout = 0, v6_timedout = 0;
struct evdns_getaddrinfo_request *data = ptr;


if (data->ipv4_request.r) {


evdns_cancel_request(NULL, data->ipv4_request.r);
v4_timedout = 1;
EVDNS_LOCK(data->evdns_base);
++data->evdns_base->getaddrinfo_ipv4_timeouts;
EVDNS_UNLOCK(data->evdns_base);
}
if (data->ipv6_request.r) {


evdns_cancel_request(NULL, data->ipv6_request.r);
v6_timedout = 1;
EVDNS_LOCK(data->evdns_base);
++data->evdns_base->getaddrinfo_ipv6_timeouts;
EVDNS_UNLOCK(data->evdns_base);
}



EVUTIL_ASSERT(!v4_timedout || !v6_timedout);


if (data->pending_result) {
add_cname_to_reply(data, data->pending_result);
data->user_cb(0, data->pending_result, data->user_data);
data->pending_result = NULL;
} else {
int e = data->pending_error;
if (!e)
e = EVUTIL_EAI_AGAIN;
data->user_cb(e, NULL, data->user_data);
}

data->user_cb = NULL;



if (!v4_timedout && !v6_timedout) {

free_getaddrinfo_request(data);
}
}

static int
evdns_getaddrinfo_set_timeout(struct evdns_base *evdns_base,
struct evdns_getaddrinfo_request *data)
{
return event_add(&data->timeout, &evdns_base->global_getaddrinfo_allow_skew);
}

static inline int
evdns_result_is_answer(int result)
{
return (result != DNS_ERR_NOTIMPL && result != DNS_ERR_REFUSED &&
result != DNS_ERR_SERVERFAILED && result != DNS_ERR_CANCEL);
}

static void
evdns_getaddrinfo_gotresolve(int result, char type, int count,
int ttl, void *addresses, void *arg)
{
int i;
struct getaddrinfo_subrequest *req = arg;
struct getaddrinfo_subrequest *other_req;
struct evdns_getaddrinfo_request *data;

struct evutil_addrinfo *res;

struct sockaddr_in sin;
struct sockaddr_in6 sin6;
struct sockaddr *sa;
int socklen, addrlen;
void *addrp;
int err;
int user_canceled;

EVUTIL_ASSERT(req->type == DNS_IPv4_A || req->type == DNS_IPv6_AAAA);
if (req->type == DNS_IPv4_A) {
data = EVUTIL_UPCAST(req, struct evdns_getaddrinfo_request, ipv4_request);
other_req = &data->ipv6_request;
} else {
data = EVUTIL_UPCAST(req, struct evdns_getaddrinfo_request, ipv6_request);
other_req = &data->ipv4_request;
}


if (result != DNS_ERR_SHUTDOWN) {
EVDNS_LOCK(data->evdns_base);
if (evdns_result_is_answer(result)) {
if (req->type == DNS_IPv4_A)
++data->evdns_base->getaddrinfo_ipv4_answered;
else
++data->evdns_base->getaddrinfo_ipv6_answered;
}
user_canceled = data->user_canceled;
if (other_req->r == NULL)
data->request_done = 1;
EVDNS_UNLOCK(data->evdns_base);
} else {
data->evdns_base = NULL;
user_canceled = data->user_canceled;
}

req->r = NULL;

if (result == DNS_ERR_CANCEL && ! user_canceled) {


if (other_req->r == NULL)
free_getaddrinfo_request(data);
return;
}

if (data->user_cb == NULL) {


free_getaddrinfo_request(data);
return;
}

if (result == DNS_ERR_NONE) {
if (count == 0)
err = EVUTIL_EAI_NODATA;
else
err = 0;
} else {
err = evdns_err_to_getaddrinfo_err(result);
}

if (err) {

if (other_req->r) {



if (result != DNS_ERR_SHUTDOWN) {
evdns_getaddrinfo_set_timeout(data->evdns_base, data);
}
data->pending_error = err;
return;
}

if (user_canceled) {
data->user_cb(EVUTIL_EAI_CANCEL, NULL, data->user_data);
} else if (data->pending_result) {


add_cname_to_reply(data, data->pending_result);
data->user_cb(0, data->pending_result, data->user_data);
data->pending_result = NULL;
} else {
if (data->pending_error)
err = getaddrinfo_merge_err(err,
data->pending_error);
data->user_cb(err, NULL, data->user_data);
}
free_getaddrinfo_request(data);
return;
} else if (user_canceled) {
if (other_req->r) {



return;
}
data->user_cb(EVUTIL_EAI_CANCEL, NULL, data->user_data);
free_getaddrinfo_request(data);
return;
}



EVUTIL_ASSERT(type == DNS_IPv4_A || type == DNS_IPv6_AAAA);

if (type == DNS_IPv4_A) {
memset(&sin, 0, sizeof(sin));
sin.sin_family = AF_INET;
sin.sin_port = htons(data->port);

sa = (struct sockaddr *)&sin;
socklen = sizeof(sin);
addrlen = 4;
addrp = &sin.sin_addr.s_addr;
} else {
memset(&sin6, 0, sizeof(sin6));
sin6.sin6_family = AF_INET6;
sin6.sin6_port = htons(data->port);

sa = (struct sockaddr *)&sin6;
socklen = sizeof(sin6);
addrlen = 16;
addrp = &sin6.sin6_addr.s6_addr;
}

res = NULL;
for (i=0; i < count; ++i) {
struct evutil_addrinfo *ai;
memcpy(addrp, ((char*)addresses)+i*addrlen, addrlen);
ai = evutil_new_addrinfo_(sa, socklen, &data->hints);
if (!ai) {
if (other_req->r) {
evdns_cancel_request(NULL, other_req->r);
}
data->user_cb(EVUTIL_EAI_MEMORY, NULL, data->user_data);
if (res)
evutil_freeaddrinfo(res);

if (other_req->r == NULL)
free_getaddrinfo_request(data);
return;
}
res = evutil_addrinfo_append_(res, ai);
}

if (other_req->r) {


evdns_getaddrinfo_set_timeout(data->evdns_base, data);
data->pending_result = res;
return;
} else {


if (data->pending_result) {
if (req->type == DNS_IPv4_A)
res = evutil_addrinfo_append_(res,
data->pending_result);
else
res = evutil_addrinfo_append_(
data->pending_result, res);
data->pending_result = NULL;
}


add_cname_to_reply(data, res);
data->user_cb(0, res, data->user_data);


free_getaddrinfo_request(data);
}
}

static struct hosts_entry *
find_hosts_entry(struct evdns_base *base, const char *hostname,
struct hosts_entry *find_after)
{
struct hosts_entry *e;

if (find_after)
e = TAILQ_NEXT(find_after, next);
else
e = TAILQ_FIRST(&base->hostsdb);

for (; e; e = TAILQ_NEXT(e, next)) {
if (!evutil_ascii_strcasecmp(e->hostname, hostname))
return e;
}
return NULL;
}

static int
evdns_getaddrinfo_fromhosts(struct evdns_base *base,
const char *nodename, struct evutil_addrinfo *hints, ev_uint16_t port,
struct evutil_addrinfo **res)
{
int n_found = 0;
struct hosts_entry *e;
struct evutil_addrinfo *ai=NULL;
int f = hints->ai_family;

EVDNS_LOCK(base);
for (e = find_hosts_entry(base, nodename, NULL); e;
e = find_hosts_entry(base, nodename, e)) {
struct evutil_addrinfo *ai_new;
++n_found;
if ((e->addr.sa.sa_family == AF_INET && f == PF_INET6) ||
(e->addr.sa.sa_family == AF_INET6 && f == PF_INET))
continue;
ai_new = evutil_new_addrinfo_(&e->addr.sa, e->addrlen, hints);
if (!ai_new) {
n_found = 0;
goto out;
}
sockaddr_setport(ai_new->ai_addr, port);
ai = evutil_addrinfo_append_(ai, ai_new);
}
EVDNS_UNLOCK(base);
out:
if (n_found) {


*res = ai;
return 0;
} else {
if (ai)
evutil_freeaddrinfo(ai);
return -1;
}
}

struct evdns_getaddrinfo_request *
evdns_getaddrinfo(struct evdns_base *dns_base,
const char *nodename, const char *servname,
const struct evutil_addrinfo *hints_in,
evdns_getaddrinfo_cb cb, void *arg)
{
struct evdns_getaddrinfo_request *data;
struct evutil_addrinfo hints;
struct evutil_addrinfo *res = NULL;
int err;
int port = 0;
int want_cname = 0;

if (!dns_base) {
dns_base = current_base;
if (!dns_base) {
log(EVDNS_LOG_WARN,
"Call to getaddrinfo_async with no "
"evdns_base configured.");
cb(EVUTIL_EAI_FAIL, NULL, arg);
return NULL;
}
}


if ((hints_in && (hints_in->ai_flags & EVUTIL_AI_NUMERICHOST))) {
res = NULL;
err = evutil_getaddrinfo(nodename, servname, hints_in, &res);
cb(err, res, arg);
return NULL;
}

if (hints_in) {
memcpy(&hints, hints_in, sizeof(hints));
} else {
memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
}

evutil_adjust_hints_for_addrconfig_(&hints);






err = evutil_getaddrinfo_common_(nodename, servname, &hints, &res, &port);
if (err != EVUTIL_EAI_NEED_RESOLVE) {
cb(err, res, arg);
return NULL;
}


if (!evdns_getaddrinfo_fromhosts(dns_base, nodename, &hints, port, &res)) {
cb(0, res, arg);
return NULL;
}




data = mm_calloc(1,sizeof(struct evdns_getaddrinfo_request));
if (!data) {
cb(EVUTIL_EAI_MEMORY, NULL, arg);
return NULL;
}

memcpy(&data->hints, &hints, sizeof(data->hints));
data->port = (ev_uint16_t)port;
data->ipv4_request.type = DNS_IPv4_A;
data->ipv6_request.type = DNS_IPv6_AAAA;
data->user_cb = cb;
data->user_data = arg;
data->evdns_base = dns_base;

want_cname = (hints.ai_flags & EVUTIL_AI_CANONNAME);














if (hints.ai_family != PF_INET6) {
log(EVDNS_LOG_DEBUG, "Sending request for %s on ipv4 as %p",
nodename, &data->ipv4_request);

data->ipv4_request.r = evdns_base_resolve_ipv4(dns_base,
nodename, 0, evdns_getaddrinfo_gotresolve,
&data->ipv4_request);
if (want_cname && data->ipv4_request.r)
data->ipv4_request.r->current_req->put_cname_in_ptr =
&data->cname_result;
}
if (hints.ai_family != PF_INET) {
log(EVDNS_LOG_DEBUG, "Sending request for %s on ipv6 as %p",
nodename, &data->ipv6_request);

data->ipv6_request.r = evdns_base_resolve_ipv6(dns_base,
nodename, 0, evdns_getaddrinfo_gotresolve,
&data->ipv6_request);
if (want_cname && data->ipv6_request.r)
data->ipv6_request.r->current_req->put_cname_in_ptr =
&data->cname_result;
}

evtimer_assign(&data->timeout, dns_base->event_base,
evdns_getaddrinfo_timeout_cb, data);

if (data->ipv4_request.r || data->ipv6_request.r) {
return data;
} else {
mm_free(data);
cb(EVUTIL_EAI_FAIL, NULL, arg);
return NULL;
}
}

void
evdns_getaddrinfo_cancel(struct evdns_getaddrinfo_request *data)
{
EVDNS_LOCK(data->evdns_base);
if (data->request_done) {
EVDNS_UNLOCK(data->evdns_base);
return;
}
event_del(&data->timeout);
data->user_canceled = 1;
if (data->ipv4_request.r)
evdns_cancel_request(data->evdns_base, data->ipv4_request.r);
if (data->ipv6_request.r)
evdns_cancel_request(data->evdns_base, data->ipv6_request.r);
EVDNS_UNLOCK(data->evdns_base);
}
