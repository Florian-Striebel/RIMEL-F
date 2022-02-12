
































#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <stdio.h>

#include <arpa/inet.h>

#include "asn1.h"
#include "snmp.h"
#include "snmpmod.h"

#include "snmpd.h"

#define SNMPTREE_TYPES
#define SNMPENUM_FUNCS
#include "tree.h"
#include "oid.h"

extern const struct transport_def inet_trans;

struct inet_port;
struct inet_port_params;
struct port_sock;

typedef int create_func(struct inet_port *, struct inet_port_params *);
typedef void input_func(int, void *);
typedef int activate_func(struct inet_port *);
typedef void deactivate_func(struct inet_port *);
typedef void parse_ctrl_func(struct port_sock *, const struct msghdr *);
typedef void setsrc_func(struct port_sock *, struct msghdr *, char *);

static create_func ipv4_create;
static input_func ipv4_input;
static activate_func ipv4_activate;
static deactivate_func ipv4_deactivate;
static parse_ctrl_func ipv4_parse_ctrl;
static setsrc_func ipv4_setsrc;

static create_func ipv6_create;
static input_func ipv6_input;
static activate_func ipv6_activate;
static deactivate_func ipv6_deactivate;
static parse_ctrl_func ipv6_parse_ctrl;
static setsrc_func ipv6_setsrc;

static create_func ipv6z_create;

static create_func dns_create;
static activate_func dns_activate;
static deactivate_func dns_deactivate;

struct port_sock {

struct port_input input;


TAILQ_ENTRY(port_sock) link;


struct inet_port *port;


struct sockaddr_storage bind_addr;


struct sockaddr_storage ret_dest;


bool set_ret_source;


union {

struct in_addr a4;


struct in6_pktinfo a6;
} ret_source;


parse_ctrl_func *parse_ctrl;


setsrc_func *setsrc;
};
static_assert(offsetof(struct port_sock, input) == 0,
"input not first in port_sock");








struct inet_port {

struct tport tport;


enum BegemotSnmpdTransportProto proto;


enum RowStatus row_status;


TAILQ_HEAD(, port_sock) socks;


char *dns_addr;


uint16_t dns_port;


create_func *create;


activate_func *activate;


deactivate_func *deactivate;
};
static_assert(offsetof(struct inet_port, tport) == 0,
"tport not first in inet_port");


#define AF_DNS AF_VENDOR00


static struct transport *my_trans;


enum {
SET_CREATED,
SET_ACTIVATED,
SET_DEACTIVATE,
SET_DESTROY,
};


static const size_t RECV_CBUF_SIZE =
MAX(CMSG_SPACE(SOCKCREDSIZE(CMGROUP_MAX)) +
CMSG_SPACE(sizeof(struct in_addr)),
CMSG_SPACE(SOCKCREDSIZE(CMGROUP_MAX)) +
CMSG_SPACE(sizeof(struct in6_pktinfo)));


static const size_t XMIT_CBUF_SIZE =
MAX(CMSG_SPACE(sizeof(struct in_addr)),
CMSG_SPACE(sizeof(struct in6_pktinfo)));







static int
inet_start(void)
{
return (trans_register(&inet_trans, &my_trans));
}







static int
inet_stop(int force __unused)
{
if (my_trans != NULL)
if (trans_unregister(my_trans) != 0)
return (SNMP_ERR_GENERR);
return (SNMP_ERR_NOERROR);
}






static void
deactivate_port(struct inet_port *p)
{
p->deactivate(p);
}











static int
inet_activate(struct tport *tp)
{
struct inet_port *port = (struct inet_port *)tp;

return (port->activate(port));
}






static void
inet_destroy_port(struct tport *tp)
{
struct inet_port *port = (struct inet_port *)tp;

deactivate_port(port);

trans_remove_port(tp);

free(port->dns_addr);
free(port);
}









static int
inet_alloc_buf(struct port_input *pi)
{
char drop_buf[2000];

if (pi->buf == NULL) {
if ((pi->buf = buf_alloc(0)) == NULL) {
(void)recvfrom(pi->fd, drop_buf, sizeof(drop_buf),
0, NULL, NULL);
return (-1);
}
pi->buflen = buf_size(0);
}
return (0);
}












static int
inet_read_msg(struct port_input *pi, struct msghdr *msg, char *cbuf)
{
struct iovec iov[1];

iov[0].iov_base = pi->buf;
iov[0].iov_len = pi->buflen;

msg->msg_name = pi->peer;
msg->msg_namelen = pi->peerlen;
msg->msg_iov = iov;
msg->msg_iovlen = 1;
msg->msg_control = cbuf;
msg->msg_controllen = RECV_CBUF_SIZE;
msg->msg_flags = 0;

memset(cbuf, 0, RECV_CBUF_SIZE);

const ssize_t len = recvmsg(pi->fd, msg, 0);

if (len == -1 || len == 0)

return (-1);

if (msg->msg_flags & MSG_TRUNC) {

snmpd_stats.silentDrops++;
snmpd_stats.inTooLong++;
return (-1);
}

pi->length = (size_t)len;

return (0);
}









static ssize_t
inet_recv(struct tport *tp, struct port_input *pi)
{
struct inet_port *port = __containerof(tp, struct inet_port, tport);
struct port_sock *sock = __containerof(pi, struct port_sock, input);

assert(port->proto == BegemotSnmpdTransportProto_udp);

if (inet_alloc_buf(pi) == -1)
return (-1);

char cbuf[RECV_CBUF_SIZE];
struct msghdr msg;

if (inet_read_msg(pi, &msg, cbuf) == -1)
return (-1);

sock->parse_ctrl(sock, &msg);

return (0);
}












static ssize_t
inet_send2(struct tport *tp, const u_char *buf, size_t len,
struct port_input *pi)
{
struct inet_port *p = __containerof(tp, struct inet_port, tport);
struct port_sock *s = (pi == NULL) ? TAILQ_FIRST(&p->socks) :
__containerof(pi, struct port_sock, input);

struct iovec iov;

iov.iov_base = __DECONST(void*, buf);
iov.iov_len = len;

struct msghdr msg;

msg.msg_flags = 0;
msg.msg_iov = &iov;
msg.msg_iovlen = 1;
msg.msg_name = (void *)pi->peer;
msg.msg_namelen = pi->peerlen;

char cbuf[XMIT_CBUF_SIZE];
if (s->set_ret_source) {
s->setsrc(s, &msg, cbuf);
} else {
msg.msg_control = NULL;
msg.msg_controllen = 0;
}

return (sendmsg(s->input.fd, &msg, 0));
}


const struct transport_def inet_trans = {
"inet",
OIDX_begemotSnmpdTransInet,
inet_start,
inet_stop,
inet_destroy_port,
inet_activate,
NULL,
inet_recv,
inet_send2,
};

struct inet_port_params {

struct asn_oid index;


enum InetAddressType type;


u_char *addr;


size_t addr_len;


uint32_t port;


enum BegemotSnmpdTransportProto proto;
};










static int
ipv4_create(struct inet_port *port, struct inet_port_params *params)
{

if (params->addr_len != 4)
return (SNMP_ERR_INCONS_VALUE);

struct port_sock *sock = calloc(1, sizeof(struct port_sock));
if (sock == NULL)
return (SNMP_ERR_GENERR);

snmpd_input_init(&sock->input);

TAILQ_INSERT_HEAD(&port->socks, sock, link);

struct sockaddr_in *sin =
(struct sockaddr_in *)&sock->bind_addr;

sin->sin_len = sizeof(struct sockaddr_in);
sin->sin_family = AF_INET;
sin->sin_port = htons(params->port);
memcpy(&sin->sin_addr, params->addr, 4);

sock->port = port;

return (SNMP_ERR_NOERROR);
}








static void
ipv4_input(int fd __unused, void *udata)
{
struct port_sock *sock = udata;

sock->input.peerlen = sizeof(struct sockaddr_in);
snmpd_input(&sock->input, &sock->port->tport);
}








static int
ipv4_activate_sock(struct port_sock *sock)
{
if ((sock->input.fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
syslog(LOG_ERR, "creating UDP4 socket: %m");
return (SNMP_ERR_RES_UNAVAIL);
}

const struct sockaddr_in *sin =
(const struct sockaddr_in *)&sock->bind_addr;

if (sin->sin_addr.s_addr == INADDR_ANY) {

static const int on = 1;

if (setsockopt(sock->input.fd, IPPROTO_IP, IP_RECVDSTADDR, &on,
sizeof(on)) == -1) {
syslog(LOG_ERR, "setsockopt(IP_RECVDSTADDR): %m");
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_GENERR);
}
sock->set_ret_source = true;
}

if (bind(sock->input.fd, (const struct sockaddr *)sin, sizeof(*sin))) {
if (errno == EADDRNOTAVAIL) {
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_INCONS_NAME);
}
syslog(LOG_ERR, "bind: %s:%u %m", inet_ntoa(sin->sin_addr),
ntohs(sin->sin_port));
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_GENERR);
}

if ((sock->input.id = fd_select(sock->input.fd, ipv4_input,
sock, NULL)) == NULL) {
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_GENERR);
}
sock->input.peer = (struct sockaddr *)&sock->ret_dest;

sock->parse_ctrl = ipv4_parse_ctrl;
sock->setsrc = ipv4_setsrc;

return (SNMP_ERR_NOERROR);
}









static int
ipv4_activate(struct inet_port *p)
{
struct port_sock *sock = TAILQ_FIRST(&p->socks);
assert(sock);
assert(!TAILQ_NEXT(sock, link));

const int ret = ipv4_activate_sock(sock);
if (ret == SNMP_ERR_NOERROR)
p->row_status = RowStatus_active;

return (ret);
}






static void
ipv4_deactivate(struct inet_port *p)
{
struct port_sock *sock = TAILQ_FIRST(&p->socks);
assert(sock);
assert(!TAILQ_NEXT(sock, link));

snmpd_input_close(&sock->input);

p->row_status = RowStatus_notInService;
}










static void
ipv4_parse_ctrl(struct port_sock *sock, const struct msghdr *msg)
{
struct sockcred *cred = NULL;

for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL;
cmsg = CMSG_NXTHDR(msg, cmsg)) {

if (cmsg->cmsg_level == IPPROTO_IP &&
cmsg->cmsg_type == IP_RECVDSTADDR) {
memcpy(&sock->ret_source.a4, CMSG_DATA(cmsg),
sizeof(struct in_addr));

} else if (cmsg->cmsg_level == SOL_SOCKET &&
cmsg->cmsg_type == SCM_CREDS) {
cred = (struct sockcred *)(void *)CMSG_DATA(cmsg);
}
}

sock->input.priv = 0;
if (sock->input.cred && cred)

sock->input.priv = (cred->sc_euid == 0);
}







static void
ipv4_setsrc(struct port_sock *sock, struct msghdr *msg, char *cbuf)
{
struct cmsghdr *cmsg;

msg->msg_control = cbuf;
msg->msg_controllen = CMSG_SPACE(sizeof(struct in_addr));


cmsg = CMSG_FIRSTHDR(msg);
cmsg->cmsg_level = IPPROTO_IP;
cmsg->cmsg_type = IP_SENDSRCADDR;
cmsg->cmsg_len = CMSG_LEN(sizeof(struct in_addr));
memcpy(CMSG_DATA(cmsg), &sock->ret_source.a4,
sizeof(struct in_addr));
}











static int
ipv6_create_common(struct inet_port *port, struct inet_port_params *params,
u_int scope_id)
{
struct port_sock *sock = calloc(1, sizeof(struct port_sock));

if (sock == NULL)
return (SNMP_ERR_GENERR);

struct sockaddr_in6 *sin = (struct sockaddr_in6 *)&sock->bind_addr;

sin->sin6_len = sizeof(struct sockaddr_in6);
sin->sin6_family = AF_INET6;
sin->sin6_port = htons(params->port);
sin->sin6_flowinfo = 0;
sin->sin6_scope_id = scope_id;

memcpy(sin->sin6_addr.s6_addr, params->addr, 16);

if (IN6_IS_ADDR_LINKLOCAL(&sin->sin6_addr) && scope_id == 0) {
char buf[INET6_ADDRSTRLEN];
syslog(LOG_INFO, "%s: link local address used without scope "
"index: %s", __func__, inet_ntop(AF_INET6,
&sin->sin6_addr, buf, sizeof(buf)));
free(sock);
return (SNMP_ERR_NO_CREATION);
}

sock->port = port;

snmpd_input_init(&sock->input);
TAILQ_INSERT_HEAD(&port->socks, sock, link);

return (SNMP_ERR_NOERROR);
}










static int
ipv6_create(struct inet_port *port, struct inet_port_params *params)
{
if (params->addr_len != 16)
return (SNMP_ERR_INCONS_VALUE);

const int ret = ipv6_create_common(port, params, 0);
if (ret != SNMP_ERR_NOERROR)
return (ret);

return (SNMP_ERR_NOERROR);
}








static void
ipv6_input(int fd __unused, void *udata)
{
struct port_sock *sock = udata;

sock->input.peerlen = sizeof(struct sockaddr_in6);
snmpd_input(&sock->input, &sock->port->tport);
}








static int
ipv6_activate_sock(struct port_sock *sock)
{
if ((sock->input.fd = socket(PF_INET6, SOCK_DGRAM, 0)) == -1) {
syslog(LOG_ERR, "creating UDP6 socket: %m");
return (SNMP_ERR_RES_UNAVAIL);
}

const struct sockaddr_in6 *sin =
(const struct sockaddr_in6 *)&sock->bind_addr;

if (memcmp(&sin->sin6_addr, &in6addr_any, sizeof(in6addr_any)) == 0) {

static const int on = 1;

if (setsockopt(sock->input.fd, IPPROTO_IPV6, IPV6_RECVPKTINFO,
&on, sizeof(on)) == -1) {
syslog(LOG_ERR, "setsockopt(IP6_PKTINFO): %m");
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_GENERR);
}
sock->set_ret_source = true;
}

if (bind(sock->input.fd, (const struct sockaddr *)sin, sizeof(*sin))) {
if (community != COMM_INITIALIZE && errno == EADDRNOTAVAIL) {
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_INCONS_NAME);
}
char buf[INET6_ADDRSTRLEN];
syslog(LOG_ERR, "bind: %s:%u:%u %m", inet_ntop(AF_INET6,
&sin->sin6_addr, buf, sizeof(buf)), sin->sin6_scope_id,
ntohs(sin->sin6_port));
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_GENERR);
}
if ((sock->input.id = fd_select(sock->input.fd, ipv6_input,
sock, NULL)) == NULL) {
(void)close(sock->input.fd);
sock->input.fd = -1;
return (SNMP_ERR_GENERR);
}
sock->input.peer = (struct sockaddr *)&sock->ret_dest;

sock->parse_ctrl = ipv6_parse_ctrl;
sock->setsrc = ipv6_setsrc;

return (SNMP_ERR_NOERROR);
}








static int
ipv6_activate(struct inet_port *p)
{
struct port_sock *sock = TAILQ_FIRST(&p->socks);
assert(sock);

const int ret = ipv6_activate_sock(sock);

if (ret == SNMP_ERR_NOERROR)
p->row_status = RowStatus_active;
return (ret);
}






static void
ipv6_deactivate(struct inet_port *p)
{
struct port_sock *sock = TAILQ_FIRST(&p->socks);
assert(sock);
assert(!TAILQ_NEXT(sock, link));

snmpd_input_close(&sock->input);

p->row_status = RowStatus_notInService;
}









static void
ipv6_parse_ctrl(struct port_sock *sock, const struct msghdr *msg)
{
struct sockcred *cred = NULL;

for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL;
cmsg = CMSG_NXTHDR(msg, cmsg)) {

if (cmsg->cmsg_level == IPPROTO_IPV6 &&
cmsg->cmsg_type == IPV6_PKTINFO) {
const struct in6_pktinfo *info =
(const struct in6_pktinfo *)(const void *)
CMSG_DATA(cmsg);
sock->ret_source.a6.ipi6_addr = info->ipi6_addr;
sock->ret_source.a6.ipi6_ifindex =
!IN6_IS_ADDR_LINKLOCAL(&info->ipi6_addr) ? 0:
info->ipi6_ifindex;

} else if (cmsg->cmsg_level == SOL_SOCKET &&
cmsg->cmsg_type == SCM_CREDS) {
cred = (struct sockcred *)(void *)CMSG_DATA(cmsg);
}
}

sock->input.priv = 0;
if (sock->input.cred && cred)

sock->input.priv = (cred->sc_euid == 0);
}







static void
ipv6_setsrc(struct port_sock *sock, struct msghdr *msg, char *cbuf)
{
struct cmsghdr *cmsg;

msg->msg_control = cbuf;
msg->msg_controllen = CMSG_SPACE(sizeof(struct in6_pktinfo));


cmsg = CMSG_FIRSTHDR(msg);
cmsg->cmsg_level = IPPROTO_IPV6;
cmsg->cmsg_type = IPV6_PKTINFO;
cmsg->cmsg_len = CMSG_LEN(sizeof(struct in6_pktinfo));
memcpy(CMSG_DATA(cmsg), &sock->ret_source.a6,
sizeof(struct in6_pktinfo));
}










static int
ipv6z_create(struct inet_port *port, struct inet_port_params *params)
{
if (params->addr_len != 20)
return (SNMP_ERR_INCONS_VALUE);

u_int scope_id = 0;
for (u_int i = 16; i < 20; i++) {
scope_id <<= 8;
scope_id |= params->addr[i];
}

const int ret = ipv6_create_common(port, params, scope_id);
if (ret != SNMP_ERR_NOERROR)
return (ret);

return (SNMP_ERR_NOERROR);
}










static int
dns_create(struct inet_port *port, struct inet_port_params *params)
{
if (params->addr_len > 64)
return (SNMP_ERR_INCONS_VALUE);

if (strnlen(params->addr, params->addr_len) !=
params->addr_len)
return (SNMP_ERR_INCONS_VALUE);

if ((port->dns_addr = realloc(params->addr,
params->addr_len + 1)) == NULL)
return (SNMP_ERR_GENERR);

port->dns_addr[params->addr_len] = '\0';
params->addr = NULL;

port->dns_port = htons(params->port);

return (SNMP_ERR_NOERROR);
}









static int
dns_activate(struct inet_port *port)
{
struct addrinfo hints;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_DGRAM;
hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_NUMERICSERV;

char portbuf[8];
sprintf(portbuf, "%hu", ntohs(port->dns_port));

struct addrinfo *res0;
int error = getaddrinfo(port->dns_addr[0] == '\0'
? NULL : port->dns_addr,
portbuf, &hints, &res0);

if (error) {
syslog(LOG_ERR, "cannot resolve address '%s': %s",
port->dns_addr, gai_strerror(error));
return (SNMP_ERR_GENERR);
}

for (struct addrinfo *res = res0; res != NULL; res = res->ai_next) {
if (res->ai_family != AF_INET && res->ai_family != AF_INET6)
continue;

struct port_sock *sock = calloc(1, sizeof(struct port_sock));
if (sock == NULL)
return (SNMP_ERR_GENERR);

snmpd_input_init(&sock->input);
sock->port = port;

int ret = SNMP_ERR_NOERROR;

if (res->ai_family == AF_INET) {
*(struct sockaddr_in *)&sock->bind_addr =
*(struct sockaddr_in *)(void *)res->ai_addr;
ret = ipv4_activate_sock(sock);
} else {
*(struct sockaddr_in6 *)&sock->bind_addr =
*(struct sockaddr_in6 *)(void *)res->ai_addr;
ret = ipv6_activate_sock(sock);
}

if (ret != SNMP_ERR_NOERROR)
free(sock);
else
TAILQ_INSERT_HEAD(&port->socks, sock, link);
}

if (!TAILQ_EMPTY(&port->socks))
port->row_status = RowStatus_active;

freeaddrinfo(res0);
return (SNMP_ERR_GENERR);
}






static void
dns_deactivate(struct inet_port *port)
{
while (!TAILQ_EMPTY(&port->socks)) {
struct port_sock *sock = TAILQ_FIRST(&port->socks);
TAILQ_REMOVE(&port->socks, sock, link);
snmpd_input_close(&sock->input);
free(sock);
}
port->row_status = RowStatus_notInService;
}

static int
inet_create(struct inet_port_params *params, struct inet_port **pp)
{
int err = SNMP_ERR_NOERROR;
struct inet_port *port = NULL;

if (params->port > 0xffff) {
err = SNMP_ERR_NO_CREATION;
goto fail;
}

if ((port = malloc(sizeof(*port))) == NULL) {
err = SNMP_ERR_GENERR;
goto fail;
}
memset(port, 0, sizeof(*port));
TAILQ_INIT(&port->socks);

port->proto = params->proto;
port->tport.index = params->index;

switch (params->type) {

case InetAddressType_ipv4:
port->create = ipv4_create;
port->activate = ipv4_activate;
port->deactivate = ipv4_deactivate;
break;

case InetAddressType_ipv6:
port->create = ipv6_create;
port->activate = ipv6_activate;
port->deactivate = ipv6_deactivate;
break;

case InetAddressType_ipv6z:
port->create = ipv6z_create;
port->activate = ipv6_activate;
port->deactivate = ipv6_deactivate;
break;

case InetAddressType_dns:
port->create = dns_create;
port->activate = dns_activate;
port->deactivate = dns_deactivate;
break;

default:
err = SNMP_ERR_NO_CREATION;
goto fail;
}

if ((err = port->create(port, params)) != SNMP_ERR_NOERROR)
goto fail;

*pp = port;
trans_insert_port(my_trans, &port->tport);
return (err);

fail:
free(port->dns_addr);
free(port);
return (err);
}

static int
create_and_go(struct snmp_context *ctx, struct inet_port_params *params)
{
int err;
struct inet_port *port;

if ((err = inet_create(params, &port)) != SNMP_ERR_NOERROR)
return (err);

port->row_status = RowStatus_createAndGo;
ctx->scratch->ptr1 = port;

if (community == COMM_INITIALIZE)
return (err);

return (inet_activate(&port->tport));
}

static int
create_and_wait(struct snmp_context *ctx, struct inet_port_params *params)
{
int err;
struct inet_port *port;

if ((err = inet_create(params, &port)) != SNMP_ERR_NOERROR)
return (err);

port->row_status = RowStatus_createAndWait;
ctx->scratch->ptr1 = port;

return (err);
}









static int
inet_port_set(struct snmp_context *ctx, struct inet_port *port,
struct inet_port_params *params, enum RowStatus nstatus)
{
switch (nstatus) {

case RowStatus_createAndGo:
if (port != NULL)
return (SNMP_ERR_INCONS_VALUE);
ctx->scratch->int1 = SET_CREATED;
return (create_and_go(ctx, params));

case RowStatus_createAndWait:
if (port != NULL)
return (SNMP_ERR_INCONS_VALUE);
ctx->scratch->int1 = SET_CREATED;
return (create_and_wait(ctx, params));

case RowStatus_active:
if (port == NULL)
return (SNMP_ERR_INCONS_VALUE);

switch (port->row_status) {

case RowStatus_notReady:

abort();

case RowStatus_notInService:
ctx->scratch->int1 = SET_ACTIVATED;
return (inet_activate(&port->tport));

case RowStatus_active:
return (SNMP_ERR_NOERROR);

case RowStatus_createAndGo:
case RowStatus_createAndWait:
case RowStatus_destroy:
abort();
}
break;

case RowStatus_notInService:
if (port == NULL)
return (SNMP_ERR_INCONS_VALUE);

switch (port->row_status) {

case RowStatus_notReady:

abort();

case RowStatus_notInService:
return (SNMP_ERR_NOERROR);

case RowStatus_active:

ctx->scratch->int1 = SET_DEACTIVATE;
return (SNMP_ERR_NOERROR);

case RowStatus_createAndGo:
case RowStatus_createAndWait:
case RowStatus_destroy:
abort();
}
break;

case RowStatus_destroy:

ctx->scratch->int1 = SET_DESTROY;
return (SNMP_ERR_NOERROR);

case RowStatus_notReady:
return (SNMP_ERR_WRONG_VALUE);
}
abort();
}




int
op_snmp_trans_inet(struct snmp_context *ctx, struct snmp_value *value,
u_int sub, u_int iidx __unused, enum snmp_op op)
{
asn_subid_t which = value->var.subs[sub - 1];
struct inet_port *port;
struct inet_port_params params;
int ret;

switch (op) {

case SNMP_OP_GETNEXT:
if ((port = (struct inet_port *)trans_next_port(my_trans,
&value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
index_append(&value->var, sub, &port->tport.index);
goto fetch;

case SNMP_OP_GET:
if ((port = (struct inet_port *)trans_find_port(my_trans,
&value->var, sub)) == NULL)
return (SNMP_ERR_NOSUCHNAME);
goto fetch;

case SNMP_OP_SET:
port = (struct inet_port *)trans_find_port(my_trans,
&value->var, sub);

if (which != LEAF_begemotSnmpdTransInetStatus)
abort();
if (!isok_RowStatus(value->v.integer))
return (SNMP_ERR_WRONG_VALUE);

if (index_decode(&value->var, sub, iidx, &params.type,
&params.addr, &params.addr_len, &params.port,
&params.proto))
return (SNMP_ERR_NO_CREATION);

asn_slice_oid(&params.index, &value->var, sub, value->var.len);

ret = inet_port_set(ctx, port, &params,
(enum RowStatus)value->v.integer);

free(params.addr);
return (ret);

case SNMP_OP_ROLLBACK:
if ((port = (struct inet_port *)trans_find_port(my_trans,
&value->var, sub)) == NULL)

abort();

switch (ctx->scratch->int1) {

case SET_CREATED:

assert(port != NULL);
inet_destroy_port(&port->tport);
return (SNMP_ERR_NOERROR);

case SET_DESTROY:

assert(port != NULL);
return (SNMP_ERR_NOERROR);

case SET_ACTIVATED:
deactivate_port(port);
return (SNMP_ERR_NOERROR);

case SET_DEACTIVATE:
return (SNMP_ERR_NOERROR);
}
abort();

case SNMP_OP_COMMIT:
if ((port = (struct inet_port *)trans_find_port(my_trans,
&value->var, sub)) == NULL)

abort();

switch (ctx->scratch->int1) {

case SET_CREATED:

assert(port != NULL);
return (SNMP_ERR_NOERROR);

case SET_DESTROY:

assert(port != NULL);
inet_destroy_port(&port->tport);
return (SNMP_ERR_NOERROR);

case SET_ACTIVATED:
return (SNMP_ERR_NOERROR);

case SET_DEACTIVATE:
deactivate_port(port);
return (SNMP_ERR_NOERROR);
}
abort();
}
abort();

fetch:
switch (which) {

case LEAF_begemotSnmpdTransInetStatus:
value->v.integer = port->row_status;
break;

default:
abort();
}

return (SNMP_ERR_NOERROR);
}
