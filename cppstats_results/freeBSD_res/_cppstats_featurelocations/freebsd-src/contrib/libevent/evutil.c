

























#include "event2/event-config.h"
#include "evconfig-private.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <io.h>
#include <tchar.h>
#include <process.h>
#undef _WIN32_WINNT

#define _WIN32_WINNT 0x0501
#include <iphlpapi.h>
#endif

#include <sys/types.h>
#if defined(EVENT__HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#if defined(EVENT__HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(EVENT__HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(EVENT__HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#if defined(EVENT__HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif
#if defined(EVENT__HAVE_NETINET_IN6_H)
#include <netinet/in6.h>
#endif
#if defined(EVENT__HAVE_NETINET_TCP_H)
#include <netinet/tcp.h>
#endif
#if defined(EVENT__HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif
#include <time.h>
#include <sys/stat.h>
#if defined(EVENT__HAVE_IFADDRS_H)
#include <ifaddrs.h>
#endif

#include "event2/util.h"
#include "util-internal.h"
#include "log-internal.h"
#include "mm-internal.h"
#include "evthread-internal.h"

#include "strlcpy-internal.h"
#include "ipv6-internal.h"

#if defined(_WIN32)
#define HT_NO_CACHE_HASH_VALUES
#include "ht-internal.h"
#define open _open
#define read _read
#define close _close
#if !defined(fstat)
#define fstat _fstati64
#endif
#if !defined(stat)
#define stat _stati64
#endif
#define mode_t int
#endif

int
evutil_open_closeonexec_(const char *pathname, int flags, unsigned mode)
{
int fd;

#if defined(O_CLOEXEC)
fd = open(pathname, flags|O_CLOEXEC, (mode_t)mode);
if (fd >= 0 || errno == EINVAL)
return fd;

#endif
fd = open(pathname, flags, (mode_t)mode);
if (fd < 0)
return -1;

#if defined(FD_CLOEXEC)
if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) {
close(fd);
return -1;
}
#endif

return fd;
}











int
evutil_read_file_(const char *filename, char **content_out, size_t *len_out,
int is_binary)
{
int fd, r;
struct stat st;
char *mem;
size_t read_so_far=0;
int mode = O_RDONLY;

EVUTIL_ASSERT(content_out);
EVUTIL_ASSERT(len_out);
*content_out = NULL;
*len_out = 0;

#if defined(O_BINARY)
if (is_binary)
mode |= O_BINARY;
#endif

fd = evutil_open_closeonexec_(filename, mode, 0);
if (fd < 0)
return -1;
if (fstat(fd, &st) || st.st_size < 0 ||
st.st_size > EV_SSIZE_MAX-1 ) {
close(fd);
return -2;
}
mem = mm_malloc((size_t)st.st_size + 1);
if (!mem) {
close(fd);
return -2;
}
read_so_far = 0;
#if defined(_WIN32)
#define N_TO_READ(x) ((x) > INT_MAX) ? INT_MAX : ((int)(x))
#else
#define N_TO_READ(x) (x)
#endif
while ((r = read(fd, mem+read_so_far, N_TO_READ(st.st_size - read_so_far))) > 0) {
read_so_far += r;
if (read_so_far >= (size_t)st.st_size)
break;
EVUTIL_ASSERT(read_so_far < (size_t)st.st_size);
}
close(fd);
if (r < 0) {
mm_free(mem);
return -2;
}
mem[read_so_far] = 0;

*len_out = read_so_far;
*content_out = mem;
return 0;
}

int
evutil_socketpair(int family, int type, int protocol, evutil_socket_t fd[2])
{
#if !defined(_WIN32)
return socketpair(family, type, protocol, fd);
#else
return evutil_ersatz_socketpair_(family, type, protocol, fd);
#endif
}

int
evutil_ersatz_socketpair_(int family, int type, int protocol,
evutil_socket_t fd[2])
{







#if defined(_WIN32)
#define ERR(e) WSA##e
#else
#define ERR(e) e
#endif
evutil_socket_t listener = -1;
evutil_socket_t connector = -1;
evutil_socket_t acceptor = -1;
struct sockaddr_in listen_addr;
struct sockaddr_in connect_addr;
ev_socklen_t size;
int saved_errno = -1;
int family_test;

family_test = family != AF_INET;
#if defined(AF_UNIX)
family_test = family_test && (family != AF_UNIX);
#endif
if (protocol || family_test) {
EVUTIL_SET_SOCKET_ERROR(ERR(EAFNOSUPPORT));
return -1;
}

if (!fd) {
EVUTIL_SET_SOCKET_ERROR(ERR(EINVAL));
return -1;
}

listener = socket(AF_INET, type, 0);
if (listener < 0)
return -1;
memset(&listen_addr, 0, sizeof(listen_addr));
listen_addr.sin_family = AF_INET;
listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
listen_addr.sin_port = 0;
if (bind(listener, (struct sockaddr *) &listen_addr, sizeof (listen_addr))
== -1)
goto tidy_up_and_fail;
if (listen(listener, 1) == -1)
goto tidy_up_and_fail;

connector = socket(AF_INET, type, 0);
if (connector < 0)
goto tidy_up_and_fail;

memset(&connect_addr, 0, sizeof(connect_addr));


size = sizeof(connect_addr);
if (getsockname(listener, (struct sockaddr *) &connect_addr, &size) == -1)
goto tidy_up_and_fail;
if (size != sizeof (connect_addr))
goto abort_tidy_up_and_fail;
if (connect(connector, (struct sockaddr *) &connect_addr,
sizeof(connect_addr)) == -1)
goto tidy_up_and_fail;

size = sizeof(listen_addr);
acceptor = accept(listener, (struct sockaddr *) &listen_addr, &size);
if (acceptor < 0)
goto tidy_up_and_fail;
if (size != sizeof(listen_addr))
goto abort_tidy_up_and_fail;


if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1)
goto tidy_up_and_fail;
if (size != sizeof (connect_addr)
|| listen_addr.sin_family != connect_addr.sin_family
|| listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
|| listen_addr.sin_port != connect_addr.sin_port)
goto abort_tidy_up_and_fail;
evutil_closesocket(listener);
fd[0] = connector;
fd[1] = acceptor;

return 0;

abort_tidy_up_and_fail:
saved_errno = ERR(ECONNABORTED);
tidy_up_and_fail:
if (saved_errno < 0)
saved_errno = EVUTIL_SOCKET_ERROR();
if (listener != -1)
evutil_closesocket(listener);
if (connector != -1)
evutil_closesocket(connector);
if (acceptor != -1)
evutil_closesocket(acceptor);

EVUTIL_SET_SOCKET_ERROR(saved_errno);
return -1;
#undef ERR
}

int
evutil_make_socket_nonblocking(evutil_socket_t fd)
{
#if defined(_WIN32)
{
unsigned long nonblocking = 1;
if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR) {
event_sock_warn(fd, "fcntl(%d, F_GETFL)", (int)fd);
return -1;
}
}
#else
{
int flags;
if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
event_warn("fcntl(%d, F_GETFL)", fd);
return -1;
}
if (!(flags & O_NONBLOCK)) {
if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
event_warn("fcntl(%d, F_SETFL)", fd);
return -1;
}
}
}
#endif
return 0;
}





static int
evutil_fast_socket_nonblocking(evutil_socket_t fd)
{
#if defined(_WIN32)
return evutil_make_socket_nonblocking(fd);
#else
if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
event_warn("fcntl(%d, F_SETFL)", fd);
return -1;
}
return 0;
#endif
}

int
evutil_make_listen_socket_reuseable(evutil_socket_t sock)
{
#if defined(SO_REUSEADDR) && !defined(_WIN32)
int one = 1;



return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &one,
(ev_socklen_t)sizeof(one));
#else
return 0;
#endif
}

int
evutil_make_listen_socket_reuseable_port(evutil_socket_t sock)
{
#if defined __linux__ && defined(SO_REUSEPORT)
int one = 1;


return setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &one,
(ev_socklen_t)sizeof(one));
#else
return 0;
#endif
}

int
evutil_make_tcp_listen_socket_deferred(evutil_socket_t sock)
{
#if defined(EVENT__HAVE_NETINET_TCP_H) && defined(TCP_DEFER_ACCEPT)
int one = 1;



return setsockopt(sock, IPPROTO_TCP, TCP_DEFER_ACCEPT, &one,
(ev_socklen_t)sizeof(one));
#endif
return 0;
}

int
evutil_make_socket_closeonexec(evutil_socket_t fd)
{
#if !defined(_WIN32) && defined(EVENT__HAVE_SETFD)
int flags;
if ((flags = fcntl(fd, F_GETFD, NULL)) < 0) {
event_warn("fcntl(%d, F_GETFD)", fd);
return -1;
}
if (!(flags & FD_CLOEXEC)) {
if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
event_warn("fcntl(%d, F_SETFD)", fd);
return -1;
}
}
#endif
return 0;
}





static int
evutil_fast_socket_closeonexec(evutil_socket_t fd)
{
#if !defined(_WIN32) && defined(EVENT__HAVE_SETFD)
if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) {
event_warn("fcntl(%d, F_SETFD)", fd);
return -1;
}
#endif
return 0;
}

int
evutil_closesocket(evutil_socket_t sock)
{
#if !defined(_WIN32)
return close(sock);
#else
return closesocket(sock);
#endif
}

ev_int64_t
evutil_strtoll(const char *s, char **endptr, int base)
{
#if defined(EVENT__HAVE_STRTOLL)
return (ev_int64_t)strtoll(s, endptr, base);
#elif EVENT__SIZEOF_LONG == 8
return (ev_int64_t)strtol(s, endptr, base);
#elif defined(_WIN32) && defined(_MSC_VER) && _MSC_VER < 1300


ev_int64_t r;
if (base != 10)
return 0;
r = (ev_int64_t) _atoi64(s);
while (isspace(*s))
++s;
if (*s == '-')
++s;
while (isdigit(*s))
++s;
if (endptr)
*endptr = (char*) s;
return r;
#elif defined(_WIN32)
return (ev_int64_t) _strtoi64(s, endptr, base);
#elif defined(EVENT__SIZEOF_LONG_LONG) && EVENT__SIZEOF_LONG_LONG == 8
long long r;
int n;
if (base != 10 && base != 16)
return 0;
if (base == 10) {
n = sscanf(s, "%lld", &r);
} else {
unsigned long long ru=0;
n = sscanf(s, "%llx", &ru);
if (ru > EV_INT64_MAX)
return 0;
r = (long long) ru;
}
if (n != 1)
return 0;
while (EVUTIL_ISSPACE_(*s))
++s;
if (*s == '-')
++s;
if (base == 10) {
while (EVUTIL_ISDIGIT_(*s))
++s;
} else {
while (EVUTIL_ISXDIGIT_(*s))
++s;
}
if (endptr)
*endptr = (char*) s;
return r;
#else
#error "I don't know how to parse 64-bit integers."
#endif
}

#if defined(_WIN32)
int
evutil_socket_geterror(evutil_socket_t sock)
{
int optval, optvallen=sizeof(optval);
int err = WSAGetLastError();
if (err == WSAEWOULDBLOCK && sock >= 0) {
if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)&optval,
&optvallen))
return err;
if (optval)
return optval;
}
return err;
}
#endif



int
evutil_socket_connect_(evutil_socket_t *fd_ptr, const struct sockaddr *sa, int socklen)
{
int made_fd = 0;

if (*fd_ptr < 0) {
if ((*fd_ptr = socket(sa->sa_family, SOCK_STREAM, 0)) < 0)
goto err;
made_fd = 1;
if (evutil_make_socket_nonblocking(*fd_ptr) < 0) {
goto err;
}
}

if (connect(*fd_ptr, sa, socklen) < 0) {
int e = evutil_socket_geterror(*fd_ptr);
if (EVUTIL_ERR_CONNECT_RETRIABLE(e))
return 0;
if (EVUTIL_ERR_CONNECT_REFUSED(e))
return 2;
goto err;
} else {
return 1;
}

err:
if (made_fd) {
evutil_closesocket(*fd_ptr);
*fd_ptr = -1;
}
return -1;
}





int
evutil_socket_finished_connecting_(evutil_socket_t fd)
{
int e;
ev_socklen_t elen = sizeof(e);

if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)&e, &elen) < 0)
return -1;

if (e) {
if (EVUTIL_ERR_CONNECT_RETRIABLE(e))
return 0;
EVUTIL_SET_SOCKET_ERROR(e);
return -1;
}

return 1;
}

#if (EVUTIL_AI_PASSIVE|EVUTIL_AI_CANONNAME|EVUTIL_AI_NUMERICHOST| EVUTIL_AI_NUMERICSERV|EVUTIL_AI_V4MAPPED|EVUTIL_AI_ALL| EVUTIL_AI_ADDRCONFIG) != (EVUTIL_AI_PASSIVE^EVUTIL_AI_CANONNAME^EVUTIL_AI_NUMERICHOST^ EVUTIL_AI_NUMERICSERV^EVUTIL_AI_V4MAPPED^EVUTIL_AI_ALL^ EVUTIL_AI_ADDRCONFIG)





#error "Some of our EVUTIL_AI_* flags seem to overlap with system AI_* flags"
#endif






static int have_checked_interfaces, had_ipv4_address, had_ipv6_address;



#define EVUTIL_V4ADDR_IS_LOCALHOST(addr) (((addr)>>24) == 127)




#define EVUTIL_V4ADDR_IS_CLASSD(addr) ((((addr)>>24) & 0xf0) == 0xe0)

static void
evutil_found_ifaddr(const struct sockaddr *sa)
{
const char ZEROES[] = "\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00";

if (sa->sa_family == AF_INET) {
const struct sockaddr_in *sin = (struct sockaddr_in *)sa;
ev_uint32_t addr = ntohl(sin->sin_addr.s_addr);
if (addr == 0 ||
EVUTIL_V4ADDR_IS_LOCALHOST(addr) ||
EVUTIL_V4ADDR_IS_CLASSD(addr)) {

} else {
event_debug(("Detected an IPv4 interface"));
had_ipv4_address = 1;
}
} else if (sa->sa_family == AF_INET6) {
const struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
const unsigned char *addr =
(unsigned char*)sin6->sin6_addr.s6_addr;
if (!memcmp(addr, ZEROES, 8) ||
((addr[0] & 0xfe) == 0xfc) ||
(addr[0] == 0xfe && (addr[1] & 0xc0) == 0x80) ||
(addr[0] == 0xfe && (addr[1] & 0xc0) == 0xc0) ||
(addr[0] == 0xff)) {


} else {
event_debug(("Detected an IPv6 interface"));
had_ipv6_address = 1;
}
}
}

#if defined(_WIN32)
typedef ULONG (WINAPI *GetAdaptersAddresses_fn_t)(
ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);
#endif

static int
evutil_check_ifaddrs(void)
{
#if defined(EVENT__HAVE_GETIFADDRS)


struct ifaddrs *ifa = NULL;
const struct ifaddrs *i;
if (getifaddrs(&ifa) < 0) {
event_warn("Unable to call getifaddrs()");
return -1;
}

for (i = ifa; i; i = i->ifa_next) {
if (!i->ifa_addr)
continue;
evutil_found_ifaddr(i->ifa_addr);
}

freeifaddrs(ifa);
return 0;
#elif defined(_WIN32)




HMODULE lib = evutil_load_windows_system_library_(TEXT("ihplapi.dll"));
GetAdaptersAddresses_fn_t fn;
ULONG size, res;
IP_ADAPTER_ADDRESSES *addresses = NULL, *address;
int result = -1;

#define FLAGS (GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER)



if (!lib)
goto done;

if (!(fn = (GetAdaptersAddresses_fn_t) GetProcAddress(lib, "GetAdaptersAddresses")))
goto done;


size = 15*1024;
addresses = mm_malloc(size);
if (!addresses)
goto done;
res = fn(AF_UNSPEC, FLAGS, NULL, addresses, &size);
if (res == ERROR_BUFFER_OVERFLOW) {

mm_free(addresses);
addresses = mm_malloc(size);
if (!addresses)
goto done;
res = fn(AF_UNSPEC, FLAGS, NULL, addresses, &size);
}
if (res != NO_ERROR)
goto done;

for (address = addresses; address; address = address->Next) {
IP_ADAPTER_UNICAST_ADDRESS *a;
for (a = address->FirstUnicastAddress; a; a = a->Next) {

struct sockaddr *sa = a->Address.lpSockaddr;
evutil_found_ifaddr(sa);
}
}

result = 0;
done:
if (lib)
FreeLibrary(lib);
if (addresses)
mm_free(addresses);
return result;
#else
return -1;
#endif
}



static int
evutil_check_interfaces(int force_recheck)
{
evutil_socket_t fd = -1;
struct sockaddr_in sin, sin_out;
struct sockaddr_in6 sin6, sin6_out;
ev_socklen_t sin_out_len = sizeof(sin_out);
ev_socklen_t sin6_out_len = sizeof(sin6_out);
int r;
if (have_checked_interfaces && !force_recheck)
return 0;

if (evutil_check_ifaddrs() == 0) {

return 0;
}






memset(&sin, 0, sizeof(sin));
sin.sin_family = AF_INET;
sin.sin_port = htons(53);
r = evutil_inet_pton(AF_INET, "18.244.0.188", &sin.sin_addr);
EVUTIL_ASSERT(r);

memset(&sin6, 0, sizeof(sin6));
sin6.sin6_family = AF_INET6;
sin6.sin6_port = htons(53);
r = evutil_inet_pton(AF_INET6, "2001:4860:b002::68", &sin6.sin6_addr);
EVUTIL_ASSERT(r);

memset(&sin_out, 0, sizeof(sin_out));
memset(&sin6_out, 0, sizeof(sin6_out));


if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0 &&
connect(fd, (struct sockaddr*)&sin, sizeof(sin)) == 0 &&
getsockname(fd, (struct sockaddr*)&sin_out, &sin_out_len) == 0) {

evutil_found_ifaddr((struct sockaddr*) &sin_out);
}
if (fd >= 0)
evutil_closesocket(fd);

if ((fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) >= 0 &&
connect(fd, (struct sockaddr*)&sin6, sizeof(sin6)) == 0 &&
getsockname(fd, (struct sockaddr*)&sin6_out, &sin6_out_len) == 0) {

evutil_found_ifaddr((struct sockaddr*) &sin6_out);
}

if (fd >= 0)
evutil_closesocket(fd);

return 0;
}





#define EVUTIL_AI_LIBEVENT_ALLOCATED 0x80000000






struct evutil_addrinfo *
evutil_new_addrinfo_(struct sockaddr *sa, ev_socklen_t socklen,
const struct evutil_addrinfo *hints)
{
struct evutil_addrinfo *res;
EVUTIL_ASSERT(hints);

if (hints->ai_socktype == 0 && hints->ai_protocol == 0) {

struct evutil_addrinfo *r1, *r2;
struct evutil_addrinfo tmp;
memcpy(&tmp, hints, sizeof(tmp));
tmp.ai_socktype = SOCK_STREAM; tmp.ai_protocol = IPPROTO_TCP;
r1 = evutil_new_addrinfo_(sa, socklen, &tmp);
if (!r1)
return NULL;
tmp.ai_socktype = SOCK_DGRAM; tmp.ai_protocol = IPPROTO_UDP;
r2 = evutil_new_addrinfo_(sa, socklen, &tmp);
if (!r2) {
evutil_freeaddrinfo(r1);
return NULL;
}
r1->ai_next = r2;
return r1;
}


res = mm_calloc(1,sizeof(struct evutil_addrinfo)+socklen);
if (!res)
return NULL;
res->ai_addr = (struct sockaddr*)
(((char*)res) + sizeof(struct evutil_addrinfo));
memcpy(res->ai_addr, sa, socklen);
res->ai_addrlen = socklen;
res->ai_family = sa->sa_family;
res->ai_flags = EVUTIL_AI_LIBEVENT_ALLOCATED;
res->ai_socktype = hints->ai_socktype;
res->ai_protocol = hints->ai_protocol;

return res;
}




struct evutil_addrinfo *
evutil_addrinfo_append_(struct evutil_addrinfo *first,
struct evutil_addrinfo *append)
{
struct evutil_addrinfo *ai = first;
if (!ai)
return append;
while (ai->ai_next)
ai = ai->ai_next;
ai->ai_next = append;

return first;
}

static int
parse_numeric_servname(const char *servname)
{
int n;
char *endptr=NULL;
n = (int) strtol(servname, &endptr, 10);
if (n>=0 && n <= 65535 && servname[0] && endptr && !endptr[0])
return n;
else
return -1;
}




static int
evutil_parse_servname(const char *servname, const char *protocol,
const struct evutil_addrinfo *hints)
{
int n = parse_numeric_servname(servname);
if (n>=0)
return n;
#if defined(EVENT__HAVE_GETSERVBYNAME) || defined(_WIN32)
if (!(hints->ai_flags & EVUTIL_AI_NUMERICSERV)) {
struct servent *ent = getservbyname(servname, protocol);
if (ent) {
return ntohs(ent->s_port);
}
}
#endif
return -1;
}



static const char *
evutil_unparse_protoname(int proto)
{
switch (proto) {
case 0:
return NULL;
case IPPROTO_TCP:
return "tcp";
case IPPROTO_UDP:
return "udp";
#if defined(IPPROTO_SCTP)
case IPPROTO_SCTP:
return "sctp";
#endif
default:
#if defined(EVENT__HAVE_GETPROTOBYNUMBER)
{
struct protoent *ent = getprotobynumber(proto);
if (ent)
return ent->p_name;
}
#endif
return NULL;
}
}

static void
evutil_getaddrinfo_infer_protocols(struct evutil_addrinfo *hints)
{

if (!hints->ai_protocol && hints->ai_socktype) {
if (hints->ai_socktype == SOCK_DGRAM)
hints->ai_protocol = IPPROTO_UDP;
else if (hints->ai_socktype == SOCK_STREAM)
hints->ai_protocol = IPPROTO_TCP;
}


if (!hints->ai_socktype && hints->ai_protocol) {
if (hints->ai_protocol == IPPROTO_UDP)
hints->ai_socktype = SOCK_DGRAM;
else if (hints->ai_protocol == IPPROTO_TCP)
hints->ai_socktype = SOCK_STREAM;
#if defined(IPPROTO_SCTP)
else if (hints->ai_protocol == IPPROTO_SCTP)
hints->ai_socktype = SOCK_STREAM;
#endif
}
}

#if AF_UNSPEC != PF_UNSPEC
#error "I cannot build on a system where AF_UNSPEC != PF_UNSPEC"
#endif














int
evutil_getaddrinfo_common_(const char *nodename, const char *servname,
struct evutil_addrinfo *hints, struct evutil_addrinfo **res, int *portnum)
{
int port = 0;
const char *pname;

if (nodename == NULL && servname == NULL)
return EVUTIL_EAI_NONAME;


if (hints->ai_family != PF_UNSPEC && hints->ai_family != PF_INET &&
hints->ai_family != PF_INET6)
return EVUTIL_EAI_FAMILY;

evutil_getaddrinfo_infer_protocols(hints);


pname = evutil_unparse_protoname(hints->ai_protocol);
if (servname) {


port = evutil_parse_servname(servname, pname, hints);
if (port < 0) {
return EVUTIL_EAI_NONAME;
}
}



if (nodename == NULL) {
struct evutil_addrinfo *res4=NULL, *res6=NULL;
if (hints->ai_family != PF_INET) {
struct sockaddr_in6 sin6;
memset(&sin6, 0, sizeof(sin6));
sin6.sin6_family = AF_INET6;
sin6.sin6_port = htons(port);
if (hints->ai_flags & EVUTIL_AI_PASSIVE) {

} else {

sin6.sin6_addr.s6_addr[15] = 1;
}
res6 = evutil_new_addrinfo_((struct sockaddr*)&sin6,
sizeof(sin6), hints);
if (!res6)
return EVUTIL_EAI_MEMORY;
}

if (hints->ai_family != PF_INET6) {
struct sockaddr_in sin;
memset(&sin, 0, sizeof(sin));
sin.sin_family = AF_INET;
sin.sin_port = htons(port);
if (hints->ai_flags & EVUTIL_AI_PASSIVE) {

} else {

sin.sin_addr.s_addr = htonl(0x7f000001);
}
res4 = evutil_new_addrinfo_((struct sockaddr*)&sin,
sizeof(sin), hints);
if (!res4) {
if (res6)
evutil_freeaddrinfo(res6);
return EVUTIL_EAI_MEMORY;
}
}
*res = evutil_addrinfo_append_(res4, res6);
return 0;
}




if (hints->ai_family == PF_INET6 || hints->ai_family == PF_UNSPEC) {
struct sockaddr_in6 sin6;
memset(&sin6, 0, sizeof(sin6));
if (1==evutil_inet_pton(AF_INET6, nodename, &sin6.sin6_addr)) {

sin6.sin6_family = AF_INET6;
sin6.sin6_port = htons(port);
*res = evutil_new_addrinfo_((struct sockaddr*)&sin6,
sizeof(sin6), hints);
if (!*res)
return EVUTIL_EAI_MEMORY;
return 0;
}
}


if (hints->ai_family == PF_INET || hints->ai_family == PF_UNSPEC) {
struct sockaddr_in sin;
memset(&sin, 0, sizeof(sin));
if (1==evutil_inet_pton(AF_INET, nodename, &sin.sin_addr)) {

sin.sin_family = AF_INET;
sin.sin_port = htons(port);
*res = evutil_new_addrinfo_((struct sockaddr*)&sin,
sizeof(sin), hints);
if (!*res)
return EVUTIL_EAI_MEMORY;
return 0;
}
}




if ((hints->ai_flags & EVUTIL_AI_NUMERICHOST)) {

return EVUTIL_EAI_NONAME;
}
*portnum = port;
return EVUTIL_EAI_NEED_RESOLVE;
}

#if defined(EVENT__HAVE_GETADDRINFO)
#define USE_NATIVE_GETADDRINFO
#endif

#if defined(USE_NATIVE_GETADDRINFO)


static const unsigned int ALL_NONNATIVE_AI_FLAGS =
#if !defined(AI_PASSIVE)
EVUTIL_AI_PASSIVE |
#endif
#if !defined(AI_CANONNAME)
EVUTIL_AI_CANONNAME |
#endif
#if !defined(AI_NUMERICHOST)
EVUTIL_AI_NUMERICHOST |
#endif
#if !defined(AI_NUMERICSERV)
EVUTIL_AI_NUMERICSERV |
#endif
#if !defined(AI_ADDRCONFIG)
EVUTIL_AI_ADDRCONFIG |
#endif
#if !defined(AI_ALL)
EVUTIL_AI_ALL |
#endif
#if !defined(AI_V4MAPPED)
EVUTIL_AI_V4MAPPED |
#endif
EVUTIL_AI_LIBEVENT_ALLOCATED;

static const unsigned int ALL_NATIVE_AI_FLAGS =
#if defined(AI_PASSIVE)
AI_PASSIVE |
#endif
#if defined(AI_CANONNAME)
AI_CANONNAME |
#endif
#if defined(AI_NUMERICHOST)
AI_NUMERICHOST |
#endif
#if defined(AI_NUMERICSERV)
AI_NUMERICSERV |
#endif
#if defined(AI_ADDRCONFIG)
AI_ADDRCONFIG |
#endif
#if defined(AI_ALL)
AI_ALL |
#endif
#if defined(AI_V4MAPPED)
AI_V4MAPPED |
#endif
0;
#endif

#if !defined(USE_NATIVE_GETADDRINFO)



static struct evutil_addrinfo *
addrinfo_from_hostent(const struct hostent *ent,
int port, const struct evutil_addrinfo *hints)
{
int i;
struct sockaddr_in sin;
struct sockaddr_in6 sin6;
struct sockaddr *sa;
int socklen;
struct evutil_addrinfo *res=NULL, *ai;
void *addrp;

if (ent->h_addrtype == PF_INET) {
memset(&sin, 0, sizeof(sin));
sin.sin_family = AF_INET;
sin.sin_port = htons(port);
sa = (struct sockaddr *)&sin;
socklen = sizeof(struct sockaddr_in);
addrp = &sin.sin_addr;
if (ent->h_length != sizeof(sin.sin_addr)) {
event_warnx("Weird h_length from gethostbyname");
return NULL;
}
} else if (ent->h_addrtype == PF_INET6) {
memset(&sin6, 0, sizeof(sin6));
sin6.sin6_family = AF_INET6;
sin6.sin6_port = htons(port);
sa = (struct sockaddr *)&sin6;
socklen = sizeof(struct sockaddr_in6);
addrp = &sin6.sin6_addr;
if (ent->h_length != sizeof(sin6.sin6_addr)) {
event_warnx("Weird h_length from gethostbyname");
return NULL;
}
} else
return NULL;

for (i = 0; ent->h_addr_list[i]; ++i) {
memcpy(addrp, ent->h_addr_list[i], ent->h_length);
ai = evutil_new_addrinfo_(sa, socklen, hints);
if (!ai) {
evutil_freeaddrinfo(res);
return NULL;
}
res = evutil_addrinfo_append_(res, ai);
}

if (res && ((hints->ai_flags & EVUTIL_AI_CANONNAME) && ent->h_name)) {
res->ai_canonname = mm_strdup(ent->h_name);
if (res->ai_canonname == NULL) {
evutil_freeaddrinfo(res);
return NULL;
}
}

return res;
}
#endif





void
evutil_adjust_hints_for_addrconfig_(struct evutil_addrinfo *hints)
{
if (!(hints->ai_flags & EVUTIL_AI_ADDRCONFIG))
return;
if (hints->ai_family != PF_UNSPEC)
return;
if (!have_checked_interfaces)
evutil_check_interfaces(0);
if (had_ipv4_address && !had_ipv6_address) {
hints->ai_family = PF_INET;
} else if (!had_ipv4_address && had_ipv6_address) {
hints->ai_family = PF_INET6;
}
}

#if defined(USE_NATIVE_GETADDRINFO)
static int need_numeric_port_hack_=0;
static int need_socktype_protocol_hack_=0;
static int tested_for_getaddrinfo_hacks=0;




















static struct evutil_addrinfo *ai_find_protocol(struct evutil_addrinfo *ai)
{
while (ai) {
if (ai->ai_protocol)
return ai;
ai = ai->ai_next;
}
return NULL;
}
static void
test_for_getaddrinfo_hacks(void)
{
int r, r2;
struct evutil_addrinfo *ai=NULL, *ai2=NULL, *ai3=NULL;
struct evutil_addrinfo hints;

memset(&hints,0,sizeof(hints));
hints.ai_family = PF_UNSPEC;
hints.ai_flags =
#if defined(AI_NUMERICHOST)
AI_NUMERICHOST |
#endif
#if defined(AI_NUMERICSERV)
AI_NUMERICSERV |
#endif
0;
r = getaddrinfo("1.2.3.4", "80", &hints, &ai);
getaddrinfo("1.2.3.4", NULL, &hints, &ai3);
hints.ai_socktype = SOCK_STREAM;
r2 = getaddrinfo("1.2.3.4", "80", &hints, &ai2);
if (r2 == 0 && r != 0) {
need_numeric_port_hack_=1;
}
if (!ai_find_protocol(ai2) || !ai_find_protocol(ai3)) {
need_socktype_protocol_hack_=1;
}

if (ai)
freeaddrinfo(ai);
if (ai2)
freeaddrinfo(ai2);
if (ai3)
freeaddrinfo(ai3);
tested_for_getaddrinfo_hacks=1;
}

static inline int
need_numeric_port_hack(void)
{
if (!tested_for_getaddrinfo_hacks)
test_for_getaddrinfo_hacks();
return need_numeric_port_hack_;
}

static inline int
need_socktype_protocol_hack(void)
{
if (!tested_for_getaddrinfo_hacks)
test_for_getaddrinfo_hacks();
return need_socktype_protocol_hack_;
}

static void
apply_numeric_port_hack(int port, struct evutil_addrinfo **ai)
{


for ( ; *ai; ai = &(*ai)->ai_next) {
struct sockaddr *sa = (*ai)->ai_addr;
if (sa && sa->sa_family == AF_INET) {
struct sockaddr_in *sin = (struct sockaddr_in*)sa;
sin->sin_port = htons(port);
} else if (sa && sa->sa_family == AF_INET6) {
struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)sa;
sin6->sin6_port = htons(port);
} else {


struct evutil_addrinfo *victim = *ai;
*ai = victim->ai_next;
victim->ai_next = NULL;
freeaddrinfo(victim);
}
}
}

static int
apply_socktype_protocol_hack(struct evutil_addrinfo *ai)
{
struct evutil_addrinfo *ai_new;
for (; ai; ai = ai->ai_next) {
evutil_getaddrinfo_infer_protocols(ai);
if (ai->ai_socktype || ai->ai_protocol)
continue;
ai_new = mm_malloc(sizeof(*ai_new));
if (!ai_new)
return -1;
memcpy(ai_new, ai, sizeof(*ai_new));
ai->ai_socktype = SOCK_STREAM;
ai->ai_protocol = IPPROTO_TCP;
ai_new->ai_socktype = SOCK_DGRAM;
ai_new->ai_protocol = IPPROTO_UDP;

ai_new->ai_next = ai->ai_next;
ai->ai_next = ai_new;
}
return 0;
}
#endif

int
evutil_getaddrinfo(const char *nodename, const char *servname,
const struct evutil_addrinfo *hints_in, struct evutil_addrinfo **res)
{
#if defined(USE_NATIVE_GETADDRINFO)
struct evutil_addrinfo hints;
int portnum=-1, need_np_hack, err;

if (hints_in) {
memcpy(&hints, hints_in, sizeof(hints));
} else {
memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
}

#if !defined(AI_ADDRCONFIG)

if (hints.ai_family == PF_UNSPEC &&
(hints.ai_flags & EVUTIL_AI_ADDRCONFIG)) {
evutil_adjust_hints_for_addrconfig_(&hints);
}
#endif

#if !defined(AI_NUMERICSERV)

if (hints.ai_flags & EVUTIL_AI_NUMERICSERV) {
if (servname && parse_numeric_servname(servname)<0)
return EVUTIL_EAI_NONAME;
}
#endif












#if defined(_WIN32)
{
int tmp_port;
err = evutil_getaddrinfo_common_(nodename,servname,&hints,
res, &tmp_port);
if (err == 0 ||
err == EVUTIL_EAI_MEMORY ||
err == EVUTIL_EAI_NONAME)
return err;


}
#endif


need_np_hack = need_numeric_port_hack() && servname && !hints.ai_socktype
&& ((portnum=parse_numeric_servname(servname)) >= 0);
if (need_np_hack) {
if (!nodename)
return evutil_getaddrinfo_common_(
NULL,servname,&hints, res, &portnum);
servname = NULL;
}

if (need_socktype_protocol_hack()) {
evutil_getaddrinfo_infer_protocols(&hints);
}








EVUTIL_ASSERT((ALL_NONNATIVE_AI_FLAGS & ALL_NATIVE_AI_FLAGS) == 0);


hints.ai_flags &= ~ALL_NONNATIVE_AI_FLAGS;

err = getaddrinfo(nodename, servname, &hints, res);
if (need_np_hack)
apply_numeric_port_hack(portnum, res);

if (need_socktype_protocol_hack()) {
if (apply_socktype_protocol_hack(*res) < 0) {
evutil_freeaddrinfo(*res);
*res = NULL;
return EVUTIL_EAI_MEMORY;
}
}
return err;
#else
int port=0, err;
struct hostent *ent = NULL;
struct evutil_addrinfo hints;

if (hints_in) {
memcpy(&hints, hints_in, sizeof(hints));
} else {
memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
}

evutil_adjust_hints_for_addrconfig_(&hints);

err = evutil_getaddrinfo_common_(nodename, servname, &hints, res, &port);
if (err != EVUTIL_EAI_NEED_RESOLVE) {

return err;
}

err = 0;

{
#if defined(EVENT__HAVE_GETHOSTBYNAME_R_6_ARG)

char buf[2048];
struct hostent hostent;
int r;
r = gethostbyname_r(nodename, &hostent, buf, sizeof(buf), &ent,
&err);
#elif defined(EVENT__HAVE_GETHOSTBYNAME_R_5_ARG)
char buf[2048];
struct hostent hostent;
ent = gethostbyname_r(nodename, &hostent, buf, sizeof(buf),
&err);
#elif defined(EVENT__HAVE_GETHOSTBYNAME_R_3_ARG)
struct hostent_data data;
struct hostent hostent;
memset(&data, 0, sizeof(data));
err = gethostbyname_r(nodename, &hostent, &data);
ent = err ? NULL : &hostent;
#else


ent = gethostbyname(nodename);
#if defined(_WIN32)
err = WSAGetLastError();
#else
err = h_errno;
#endif
#endif


if (!ent) {

switch (err) {
case TRY_AGAIN:
return EVUTIL_EAI_AGAIN;
case NO_RECOVERY:
default:
return EVUTIL_EAI_FAIL;
case HOST_NOT_FOUND:
return EVUTIL_EAI_NONAME;
case NO_ADDRESS:
#if NO_DATA != NO_ADDRESS
case NO_DATA:
#endif
return EVUTIL_EAI_NODATA;
}
}

if (ent->h_addrtype != hints.ai_family &&
hints.ai_family != PF_UNSPEC) {



return EVUTIL_EAI_NONAME;
}


if (ent->h_length == 0)
return EVUTIL_EAI_NODATA;



if (ent->h_addrtype != PF_INET && ent->h_addrtype != PF_INET6)
return EVUTIL_EAI_FAMILY;

*res = addrinfo_from_hostent(ent, port, &hints);
if (! *res)
return EVUTIL_EAI_MEMORY;
}

return 0;
#endif
}

void
evutil_freeaddrinfo(struct evutil_addrinfo *ai)
{
#if defined(EVENT__HAVE_GETADDRINFO)
if (!(ai->ai_flags & EVUTIL_AI_LIBEVENT_ALLOCATED)) {
freeaddrinfo(ai);
return;
}
#endif
while (ai) {
struct evutil_addrinfo *next = ai->ai_next;
if (ai->ai_canonname)
mm_free(ai->ai_canonname);
mm_free(ai);
ai = next;
}
}

static evdns_getaddrinfo_fn evdns_getaddrinfo_impl = NULL;
static evdns_getaddrinfo_cancel_fn evdns_getaddrinfo_cancel_impl = NULL;

void
evutil_set_evdns_getaddrinfo_fn_(evdns_getaddrinfo_fn fn)
{
if (!evdns_getaddrinfo_impl)
evdns_getaddrinfo_impl = fn;
}
void
evutil_set_evdns_getaddrinfo_cancel_fn_(evdns_getaddrinfo_cancel_fn fn)
{
if (!evdns_getaddrinfo_cancel_impl)
evdns_getaddrinfo_cancel_impl = fn;
}





struct evdns_getaddrinfo_request *evutil_getaddrinfo_async_(
struct evdns_base *dns_base,
const char *nodename, const char *servname,
const struct evutil_addrinfo *hints_in,
void (*cb)(int, struct evutil_addrinfo *, void *), void *arg)
{
if (dns_base && evdns_getaddrinfo_impl) {
return evdns_getaddrinfo_impl(
dns_base, nodename, servname, hints_in, cb, arg);
} else {
struct evutil_addrinfo *ai=NULL;
int err;
err = evutil_getaddrinfo(nodename, servname, hints_in, &ai);
cb(err, ai, arg);
return NULL;
}
}

void evutil_getaddrinfo_cancel_async_(struct evdns_getaddrinfo_request *data)
{
if (evdns_getaddrinfo_cancel_impl && data) {
evdns_getaddrinfo_cancel_impl(data);
}
}

const char *
evutil_gai_strerror(int err)
{



switch (err) {
case EVUTIL_EAI_CANCEL:
return "Request canceled";
case 0:
return "No error";

case EVUTIL_EAI_ADDRFAMILY:
return "address family for nodename not supported";
case EVUTIL_EAI_AGAIN:
return "temporary failure in name resolution";
case EVUTIL_EAI_BADFLAGS:
return "invalid value for ai_flags";
case EVUTIL_EAI_FAIL:
return "non-recoverable failure in name resolution";
case EVUTIL_EAI_FAMILY:
return "ai_family not supported";
case EVUTIL_EAI_MEMORY:
return "memory allocation failure";
case EVUTIL_EAI_NODATA:
return "no address associated with nodename";
case EVUTIL_EAI_NONAME:
return "nodename nor servname provided, or not known";
case EVUTIL_EAI_SERVICE:
return "servname not supported for ai_socktype";
case EVUTIL_EAI_SOCKTYPE:
return "ai_socktype not supported";
case EVUTIL_EAI_SYSTEM:
return "system error";
default:
#if defined(USE_NATIVE_GETADDRINFO) && defined(_WIN32)
return gai_strerrorA(err);
#elif defined(USE_NATIVE_GETADDRINFO)
return gai_strerror(err);
#else
return "Unknown error code";
#endif
}
}

#if defined(_WIN32)

static void
chomp (char *s)
{
size_t len;
if (s && (len = strlen (s)) > 0 && s[len - 1] == '\n') {
s[--len] = 0;
if (len > 0 && s[len - 1] == '\r')
s[--len] = 0;
}
}











struct cached_sock_errs_entry {
HT_ENTRY(cached_sock_errs_entry) node;
DWORD code;
char *msg;
};

static inline unsigned
hash_cached_sock_errs(const struct cached_sock_errs_entry *e)
{

DWORD h = e->code;
h ^= h >> 16;
h *= 0x85ebca6b;
h ^= h >> 13;
h *= 0xc2b2ae35;
h ^= h >> 16;
return h;
}

static inline int
eq_cached_sock_errs(const struct cached_sock_errs_entry *a,
const struct cached_sock_errs_entry *b)
{
return a->code == b->code;
}

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
static void *windows_socket_errors_lock_ = NULL;
#endif

static HT_HEAD(cached_sock_errs_map, cached_sock_errs_entry)
windows_socket_errors = HT_INITIALIZER();

HT_PROTOTYPE(cached_sock_errs_map,
cached_sock_errs_entry,
node,
hash_cached_sock_errs,
eq_cached_sock_errs);

HT_GENERATE(cached_sock_errs_map,
cached_sock_errs_entry,
node,
hash_cached_sock_errs,
eq_cached_sock_errs,
0.5,
mm_malloc,
mm_realloc,
mm_free);


const char *
evutil_socket_error_to_string(int errcode)
{
struct cached_sock_errs_entry *errs, *newerr, find;
char *msg = NULL;

EVLOCK_LOCK(windows_socket_errors_lock_, 0);

find.code = errcode;
errs = HT_FIND(cached_sock_errs_map, &windows_socket_errors, &find);
if (errs) {
msg = errs->msg;
goto done;
}

if (0 != FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
FORMAT_MESSAGE_IGNORE_INSERTS |
FORMAT_MESSAGE_ALLOCATE_BUFFER,
NULL, errcode, 0, (char *)&msg, 0, NULL))
chomp (msg);
else {
size_t len = 50;

msg = LocalAlloc(LMEM_FIXED, len);
if (!msg) {
msg = (char *)"LocalAlloc failed during Winsock error";
goto done;
}
evutil_snprintf(msg, len, "winsock error 0x%08x", errcode);
}

newerr = (struct cached_sock_errs_entry *)
mm_malloc(sizeof (struct cached_sock_errs_entry));

if (!newerr) {
LocalFree(msg);
msg = (char *)"malloc failed during Winsock error";
goto done;
}

newerr->code = errcode;
newerr->msg = msg;
HT_INSERT(cached_sock_errs_map, &windows_socket_errors, newerr);

done:
EVLOCK_UNLOCK(windows_socket_errors_lock_, 0);

return msg;
}

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
int
evutil_global_setup_locks_(const int enable_locks)
{
EVTHREAD_SETUP_GLOBAL_LOCK(windows_socket_errors_lock_, 0);
return 0;
}
#endif

static void
evutil_free_sock_err_globals(void)
{
struct cached_sock_errs_entry **errs, *tofree;

for (errs = HT_START(cached_sock_errs_map, &windows_socket_errors)
; errs; ) {
tofree = *errs;
errs = HT_NEXT_RMV(cached_sock_errs_map,
&windows_socket_errors,
errs);
LocalFree(tofree->msg);
mm_free(tofree);
}

HT_CLEAR(cached_sock_errs_map, &windows_socket_errors);

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
if (windows_socket_errors_lock_ != NULL) {
EVTHREAD_FREE_LOCK(windows_socket_errors_lock_, 0);
windows_socket_errors_lock_ = NULL;
}
#endif
}

#else

#if !defined(EVENT__DISABLE_THREAD_SUPPORT)
int
evutil_global_setup_locks_(const int enable_locks)
{
return 0;
}
#endif

static void
evutil_free_sock_err_globals(void)
{
}

#endif

int
evutil_snprintf(char *buf, size_t buflen, const char *format, ...)
{
int r;
va_list ap;
va_start(ap, format);
r = evutil_vsnprintf(buf, buflen, format, ap);
va_end(ap);
return r;
}

int
evutil_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap)
{
int r;
if (!buflen)
return 0;
#if defined(_MSC_VER) || defined(_WIN32)
r = _vsnprintf(buf, buflen, format, ap);
if (r < 0)
r = _vscprintf(format, ap);
#elif defined(sgi)

extern int _xpg5_vsnprintf(char * __restrict,
__SGI_LIBC_NAMESPACE_QUALIFIER size_t,
const char * __restrict, char *);

r = _xpg5_vsnprintf(buf, buflen, format, ap);
#else
r = vsnprintf(buf, buflen, format, ap);
#endif
buf[buflen-1] = '\0';
return r;
}

#define USE_INTERNAL_NTOP
#define USE_INTERNAL_PTON

const char *
evutil_inet_ntop(int af, const void *src, char *dst, size_t len)
{
#if defined(EVENT__HAVE_INET_NTOP) && !defined(USE_INTERNAL_NTOP)
return inet_ntop(af, src, dst, len);
#else
if (af == AF_INET) {
const struct in_addr *in = src;
const ev_uint32_t a = ntohl(in->s_addr);
int r;
r = evutil_snprintf(dst, len, "%d.%d.%d.%d",
(int)(ev_uint8_t)((a>>24)&0xff),
(int)(ev_uint8_t)((a>>16)&0xff),
(int)(ev_uint8_t)((a>>8 )&0xff),
(int)(ev_uint8_t)((a )&0xff));
if (r<0||(size_t)r>=len)
return NULL;
else
return dst;
#if defined(AF_INET6)
} else if (af == AF_INET6) {
const struct in6_addr *addr = src;
char buf[64], *cp;
int longestGapLen = 0, longestGapPos = -1, i,
curGapPos = -1, curGapLen = 0;
ev_uint16_t words[8];
for (i = 0; i < 8; ++i) {
words[i] =
(((ev_uint16_t)addr->s6_addr[2*i])<<8) + addr->s6_addr[2*i+1];
}
if (words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 &&
words[4] == 0 && ((words[5] == 0 && words[6] && words[7]) ||
(words[5] == 0xffff))) {

if (words[5] == 0) {
evutil_snprintf(buf, sizeof(buf), "::%d.%d.%d.%d",
addr->s6_addr[12], addr->s6_addr[13],
addr->s6_addr[14], addr->s6_addr[15]);
} else {
evutil_snprintf(buf, sizeof(buf), "::%x:%d.%d.%d.%d", words[5],
addr->s6_addr[12], addr->s6_addr[13],
addr->s6_addr[14], addr->s6_addr[15]);
}
if (strlen(buf) > len)
return NULL;
strlcpy(dst, buf, len);
return dst;
}
i = 0;
while (i < 8) {
if (words[i] == 0) {
curGapPos = i++;
curGapLen = 1;
while (i<8 && words[i] == 0) {
++i; ++curGapLen;
}
if (curGapLen > longestGapLen) {
longestGapPos = curGapPos;
longestGapLen = curGapLen;
}
} else {
++i;
}
}
if (longestGapLen<=1)
longestGapPos = -1;

cp = buf;
for (i = 0; i < 8; ++i) {
if (words[i] == 0 && longestGapPos == i) {
if (i == 0)
*cp++ = ':';
*cp++ = ':';
while (i < 8 && words[i] == 0)
++i;
--i;
} else {
evutil_snprintf(cp,
sizeof(buf)-(cp-buf), "%x", (unsigned)words[i]);
cp += strlen(cp);
if (i != 7)
*cp++ = ':';
}
}
*cp = '\0';
if (strlen(buf) > len)
return NULL;
strlcpy(dst, buf, len);
return dst;
#endif
} else {
return NULL;
}
#endif
}

int
evutil_inet_pton(int af, const char *src, void *dst)
{
#if defined(EVENT__HAVE_INET_PTON) && !defined(USE_INTERNAL_PTON)
return inet_pton(af, src, dst);
#else
if (af == AF_INET) {
unsigned a,b,c,d;
char more;
struct in_addr *addr = dst;
if (sscanf(src, "%u.%u.%u.%u%c", &a,&b,&c,&d,&more) != 4)
return 0;
if (a > 255) return 0;
if (b > 255) return 0;
if (c > 255) return 0;
if (d > 255) return 0;
addr->s_addr = htonl((a<<24) | (b<<16) | (c<<8) | d);
return 1;
#if defined(AF_INET6)
} else if (af == AF_INET6) {
struct in6_addr *out = dst;
ev_uint16_t words[8];
int gapPos = -1, i, setWords=0;
const char *dot = strchr(src, '.');
const char *eow;
if (dot == src)
return 0;
else if (!dot)
eow = src+strlen(src);
else {
unsigned byte1,byte2,byte3,byte4;
char more;
for (eow = dot-1; eow >= src && EVUTIL_ISDIGIT_(*eow); --eow)
;
++eow;



if (sscanf(eow, "%u.%u.%u.%u%c",
&byte1,&byte2,&byte3,&byte4,&more) != 4)
return 0;

if (byte1 > 255 ||
byte2 > 255 ||
byte3 > 255 ||
byte4 > 255)
return 0;

words[6] = (byte1<<8) | byte2;
words[7] = (byte3<<8) | byte4;
setWords += 2;
}

i = 0;
while (src < eow) {
if (i > 7)
return 0;
if (EVUTIL_ISXDIGIT_(*src)) {
char *next;
long r = strtol(src, &next, 16);
if (next > 4+src)
return 0;
if (next == src)
return 0;
if (r<0 || r>65536)
return 0;

words[i++] = (ev_uint16_t)r;
setWords++;
src = next;
if (*src != ':' && src != eow)
return 0;
++src;
} else if (*src == ':' && i > 0 && gapPos==-1) {
gapPos = i;
++src;
} else if (*src == ':' && i == 0 && src[1] == ':' && gapPos==-1) {
gapPos = i;
src += 2;
} else {
return 0;
}
}

if (setWords > 8 ||
(setWords == 8 && gapPos != -1) ||
(setWords < 8 && gapPos == -1))
return 0;

if (gapPos >= 0) {
int nToMove = setWords - (dot ? 2 : 0) - gapPos;
int gapLen = 8 - setWords;

if (nToMove < 0)
return -1;
memmove(&words[gapPos+gapLen], &words[gapPos],
sizeof(ev_uint16_t)*nToMove);
memset(&words[gapPos], 0, sizeof(ev_uint16_t)*gapLen);
}
for (i = 0; i < 8; ++i) {
out->s6_addr[2*i ] = words[i] >> 8;
out->s6_addr[2*i+1] = words[i] & 0xff;
}

return 1;
#endif
} else {
return -1;
}
#endif
}

int
evutil_parse_sockaddr_port(const char *ip_as_string, struct sockaddr *out, int *outlen)
{
int port;
char buf[128];
const char *cp, *addr_part, *port_part;
int is_ipv6;








cp = strchr(ip_as_string, ':');
if (*ip_as_string == '[') {
size_t len;
if (!(cp = strchr(ip_as_string, ']'))) {
return -1;
}
len = ( cp-(ip_as_string + 1) );
if (len > sizeof(buf)-1) {
return -1;
}
memcpy(buf, ip_as_string+1, len);
buf[len] = '\0';
addr_part = buf;
if (cp[1] == ':')
port_part = cp+2;
else
port_part = NULL;
is_ipv6 = 1;
} else if (cp && strchr(cp+1, ':')) {
is_ipv6 = 1;
addr_part = ip_as_string;
port_part = NULL;
} else if (cp) {
is_ipv6 = 0;
if (cp - ip_as_string > (int)sizeof(buf)-1) {
return -1;
}
memcpy(buf, ip_as_string, cp-ip_as_string);
buf[cp-ip_as_string] = '\0';
addr_part = buf;
port_part = cp+1;
} else {
addr_part = ip_as_string;
port_part = NULL;
is_ipv6 = 0;
}

if (port_part == NULL) {
port = 0;
} else {
port = atoi(port_part);
if (port <= 0 || port > 65535) {
return -1;
}
}

if (!addr_part)
return -1;
#if defined(AF_INET6)
if (is_ipv6)
{
struct sockaddr_in6 sin6;
memset(&sin6, 0, sizeof(sin6));
#if defined(EVENT__HAVE_STRUCT_SOCKADDR_IN6_SIN6_LEN)
sin6.sin6_len = sizeof(sin6);
#endif
sin6.sin6_family = AF_INET6;
sin6.sin6_port = htons(port);
if (1 != evutil_inet_pton(AF_INET6, addr_part, &sin6.sin6_addr))
return -1;
if ((int)sizeof(sin6) > *outlen)
return -1;
memset(out, 0, *outlen);
memcpy(out, &sin6, sizeof(sin6));
*outlen = sizeof(sin6);
return 0;
}
else
#endif
{
struct sockaddr_in sin;
memset(&sin, 0, sizeof(sin));
#if defined(EVENT__HAVE_STRUCT_SOCKADDR_IN_SIN_LEN)
sin.sin_len = sizeof(sin);
#endif
sin.sin_family = AF_INET;
sin.sin_port = htons(port);
if (1 != evutil_inet_pton(AF_INET, addr_part, &sin.sin_addr))
return -1;
if ((int)sizeof(sin) > *outlen)
return -1;
memset(out, 0, *outlen);
memcpy(out, &sin, sizeof(sin));
*outlen = sizeof(sin);
return 0;
}
}

const char *
evutil_format_sockaddr_port_(const struct sockaddr *sa, char *out, size_t outlen)
{
char b[128];
const char *res=NULL;
int port;
if (sa->sa_family == AF_INET) {
const struct sockaddr_in *sin = (const struct sockaddr_in*)sa;
res = evutil_inet_ntop(AF_INET, &sin->sin_addr,b,sizeof(b));
port = ntohs(sin->sin_port);
if (res) {
evutil_snprintf(out, outlen, "%s:%d", b, port);
return out;
}
} else if (sa->sa_family == AF_INET6) {
const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6*)sa;
res = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr,b,sizeof(b));
port = ntohs(sin6->sin6_port);
if (res) {
evutil_snprintf(out, outlen, "[%s]:%d", b, port);
return out;
}
}

evutil_snprintf(out, outlen, "<addr with socktype %d>",
(int)sa->sa_family);
return out;
}

int
evutil_sockaddr_cmp(const struct sockaddr *sa1, const struct sockaddr *sa2,
int include_port)
{
int r;
if (0 != (r = (sa1->sa_family - sa2->sa_family)))
return r;

if (sa1->sa_family == AF_INET) {
const struct sockaddr_in *sin1, *sin2;
sin1 = (const struct sockaddr_in *)sa1;
sin2 = (const struct sockaddr_in *)sa2;
if (sin1->sin_addr.s_addr < sin2->sin_addr.s_addr)
return -1;
else if (sin1->sin_addr.s_addr > sin2->sin_addr.s_addr)
return 1;
else if (include_port &&
(r = ((int)sin1->sin_port - (int)sin2->sin_port)))
return r;
else
return 0;
}
#if defined(AF_INET6)
else if (sa1->sa_family == AF_INET6) {
const struct sockaddr_in6 *sin1, *sin2;
sin1 = (const struct sockaddr_in6 *)sa1;
sin2 = (const struct sockaddr_in6 *)sa2;
if ((r = memcmp(sin1->sin6_addr.s6_addr, sin2->sin6_addr.s6_addr, 16)))
return r;
else if (include_port &&
(r = ((int)sin1->sin6_port - (int)sin2->sin6_port)))
return r;
else
return 0;
}
#endif
return 1;
}






static const ev_uint32_t EVUTIL_ISALPHA_TABLE[8] =
{ 0, 0, 0x7fffffe, 0x7fffffe, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISALNUM_TABLE[8] =
{ 0, 0x3ff0000, 0x7fffffe, 0x7fffffe, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISSPACE_TABLE[8] = { 0x3e00, 0x1, 0, 0, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISXDIGIT_TABLE[8] =
{ 0, 0x3ff0000, 0x7e, 0x7e, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISDIGIT_TABLE[8] = { 0, 0x3ff0000, 0, 0, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISPRINT_TABLE[8] =
{ 0, 0xffffffff, 0xffffffff, 0x7fffffff, 0, 0, 0, 0x0 };
static const ev_uint32_t EVUTIL_ISUPPER_TABLE[8] = { 0, 0, 0x7fffffe, 0, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISLOWER_TABLE[8] = { 0, 0, 0, 0x7fffffe, 0, 0, 0, 0 };


static const unsigned char EVUTIL_TOUPPER_TABLE[256] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
96,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,87,88,89,90,123,124,125,126,127,
128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};
static const unsigned char EVUTIL_TOLOWER_TABLE[256] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
64,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,120,121,122,91,92,93,94,95,
96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

#define IMPL_CTYPE_FN(name) int EVUTIL_##name##_(char c) { ev_uint8_t u = c; return !!(EVUTIL_##name##_TABLE[(u >> 5) & 7] & (1 << (u & 31))); }




IMPL_CTYPE_FN(ISALPHA)
IMPL_CTYPE_FN(ISALNUM)
IMPL_CTYPE_FN(ISSPACE)
IMPL_CTYPE_FN(ISDIGIT)
IMPL_CTYPE_FN(ISXDIGIT)
IMPL_CTYPE_FN(ISPRINT)
IMPL_CTYPE_FN(ISLOWER)
IMPL_CTYPE_FN(ISUPPER)

char EVUTIL_TOLOWER_(char c)
{
return ((char)EVUTIL_TOLOWER_TABLE[(ev_uint8_t)c]);
}
char EVUTIL_TOUPPER_(char c)
{
return ((char)EVUTIL_TOUPPER_TABLE[(ev_uint8_t)c]);
}
int
evutil_ascii_strcasecmp(const char *s1, const char *s2)
{
char c1, c2;
while (1) {
c1 = EVUTIL_TOLOWER_(*s1++);
c2 = EVUTIL_TOLOWER_(*s2++);
if (c1 < c2)
return -1;
else if (c1 > c2)
return 1;
else if (c1 == 0)
return 0;
}
}
int evutil_ascii_strncasecmp(const char *s1, const char *s2, size_t n)
{
char c1, c2;
while (n--) {
c1 = EVUTIL_TOLOWER_(*s1++);
c2 = EVUTIL_TOLOWER_(*s2++);
if (c1 < c2)
return -1;
else if (c1 > c2)
return 1;
else if (c1 == 0)
return 0;
}
return 0;
}

void
evutil_rtrim_lws_(char *str)
{
char *cp;

if (str == NULL)
return;

if ((cp = strchr(str, '\0')) == NULL || (cp == str))
return;

--cp;

while (*cp == ' ' || *cp == '\t') {
*cp = '\0';
if (cp == str)
break;
--cp;
}
}

static int
evutil_issetugid(void)
{
#if defined(EVENT__HAVE_ISSETUGID)
return issetugid();
#else

#if defined(EVENT__HAVE_GETEUID)
if (getuid() != geteuid())
return 1;
#endif
#if defined(EVENT__HAVE_GETEGID)
if (getgid() != getegid())
return 1;
#endif
return 0;
#endif
}

const char *
evutil_getenv_(const char *varname)
{
if (evutil_issetugid())
return NULL;

return getenv(varname);
}

ev_uint32_t
evutil_weakrand_seed_(struct evutil_weakrand_state *state, ev_uint32_t seed)
{
if (seed == 0) {
struct timeval tv;
evutil_gettimeofday(&tv, NULL);
seed = (ev_uint32_t)tv.tv_sec + (ev_uint32_t)tv.tv_usec;
#if defined(_WIN32)
seed += (ev_uint32_t) _getpid();
#else
seed += (ev_uint32_t) getpid();
#endif
}
state->seed = seed;
return seed;
}

ev_int32_t
evutil_weakrand_(struct evutil_weakrand_state *state)
{








state->seed = ((state->seed) * 1103515245 + 12345) & 0x7fffffff;
return (ev_int32_t)(state->seed);
}

ev_int32_t
evutil_weakrand_range_(struct evutil_weakrand_state *state, ev_int32_t top)
{
ev_int32_t divisor, result;





divisor = EVUTIL_WEAKRAND_MAX / top;
do {
result = evutil_weakrand_(state) / divisor;
} while (result >= top);
return result;
}





void * (*volatile evutil_memset_volatile_)(void *, int, size_t) = memset;

void
evutil_memclear_(void *mem, size_t len)
{
evutil_memset_volatile_(mem, 0, len);
}

int
evutil_sockaddr_is_loopback_(const struct sockaddr *addr)
{
static const char LOOPBACK_S6[16] =
"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1";
if (addr->sa_family == AF_INET) {
struct sockaddr_in *sin = (struct sockaddr_in *)addr;
return (ntohl(sin->sin_addr.s_addr) & 0xff000000) == 0x7f000000;
} else if (addr->sa_family == AF_INET6) {
struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)addr;
return !memcmp(sin6->sin6_addr.s6_addr, LOOPBACK_S6, 16);
}
return 0;
}

int
evutil_hex_char_to_int_(char c)
{
switch(c)
{
case '0': return 0;
case '1': return 1;
case '2': return 2;
case '3': return 3;
case '4': return 4;
case '5': return 5;
case '6': return 6;
case '7': return 7;
case '8': return 8;
case '9': return 9;
case 'A': case 'a': return 10;
case 'B': case 'b': return 11;
case 'C': case 'c': return 12;
case 'D': case 'd': return 13;
case 'E': case 'e': return 14;
case 'F': case 'f': return 15;
}
return -1;
}

#if defined(_WIN32)
HMODULE
evutil_load_windows_system_library_(const TCHAR *library_name)
{
TCHAR path[MAX_PATH];
unsigned n;
n = GetSystemDirectory(path, MAX_PATH);
if (n == 0 || n + _tcslen(library_name) + 2 >= MAX_PATH)
return 0;
_tcscat(path, TEXT("\\"));
_tcscat(path, library_name);
return LoadLibrary(path);
}
#endif









evutil_socket_t
evutil_socket_(int domain, int type, int protocol)
{
evutil_socket_t r;
#if defined(SOCK_NONBLOCK) && defined(SOCK_CLOEXEC)
r = socket(domain, type, protocol);
if (r >= 0)
return r;
else if ((type & (SOCK_NONBLOCK|SOCK_CLOEXEC)) == 0)
return -1;
#endif
#define SOCKET_TYPE_MASK (~(EVUTIL_SOCK_NONBLOCK|EVUTIL_SOCK_CLOEXEC))
r = socket(domain, type & SOCKET_TYPE_MASK, protocol);
if (r < 0)
return -1;
if (type & EVUTIL_SOCK_NONBLOCK) {
if (evutil_fast_socket_nonblocking(r) < 0) {
evutil_closesocket(r);
return -1;
}
}
if (type & EVUTIL_SOCK_CLOEXEC) {
if (evutil_fast_socket_closeonexec(r) < 0) {
evutil_closesocket(r);
return -1;
}
}
return r;
}









evutil_socket_t
evutil_accept4_(evutil_socket_t sockfd, struct sockaddr *addr,
ev_socklen_t *addrlen, int flags)
{
evutil_socket_t result;
#if defined(EVENT__HAVE_ACCEPT4) && defined(SOCK_CLOEXEC) && defined(SOCK_NONBLOCK)
result = accept4(sockfd, addr, addrlen, flags);
if (result >= 0 || (errno != EINVAL && errno != ENOSYS)) {





return result;
}
#endif
result = accept(sockfd, addr, addrlen);
if (result < 0)
return result;

if (flags & EVUTIL_SOCK_CLOEXEC) {
if (evutil_fast_socket_closeonexec(result) < 0) {
evutil_closesocket(result);
return -1;
}
}
if (flags & EVUTIL_SOCK_NONBLOCK) {
if (evutil_fast_socket_nonblocking(result) < 0) {
evutil_closesocket(result);
return -1;
}
}
return result;
}





int
evutil_make_internal_pipe_(evutil_socket_t fd[2])
{









#if defined(EVENT__HAVE_PIPE2)
if (pipe2(fd, O_NONBLOCK|O_CLOEXEC) == 0)
return 0;
#endif
#if defined(EVENT__HAVE_PIPE)
if (pipe(fd) == 0) {
if (evutil_fast_socket_nonblocking(fd[0]) < 0 ||
evutil_fast_socket_nonblocking(fd[1]) < 0 ||
evutil_fast_socket_closeonexec(fd[0]) < 0 ||
evutil_fast_socket_closeonexec(fd[1]) < 0) {
close(fd[0]);
close(fd[1]);
fd[0] = fd[1] = -1;
return -1;
}
return 0;
} else {
event_warn("%s: pipe", __func__);
}
#endif

#if defined(_WIN32)
#define LOCAL_SOCKETPAIR_AF AF_INET
#else
#define LOCAL_SOCKETPAIR_AF AF_UNIX
#endif
if (evutil_socketpair(LOCAL_SOCKETPAIR_AF, SOCK_STREAM, 0, fd) == 0) {
if (evutil_fast_socket_nonblocking(fd[0]) < 0 ||
evutil_fast_socket_nonblocking(fd[1]) < 0 ||
evutil_fast_socket_closeonexec(fd[0]) < 0 ||
evutil_fast_socket_closeonexec(fd[1]) < 0) {
evutil_closesocket(fd[0]);
evutil_closesocket(fd[1]);
fd[0] = fd[1] = -1;
return -1;
}
return 0;
}
fd[0] = fd[1] = -1;
return -1;
}





evutil_socket_t
evutil_eventfd_(unsigned initval, int flags)
{
#if defined(EVENT__HAVE_EVENTFD) && defined(EVENT__HAVE_SYS_EVENTFD_H)
int r;
#if defined(EFD_CLOEXEC) && defined(EFD_NONBLOCK)
r = eventfd(initval, flags);
if (r >= 0 || flags == 0)
return r;
#endif
r = eventfd(initval, 0);
if (r < 0)
return r;
if (flags & EVUTIL_EFD_CLOEXEC) {
if (evutil_fast_socket_closeonexec(r) < 0) {
evutil_closesocket(r);
return -1;
}
}
if (flags & EVUTIL_EFD_NONBLOCK) {
if (evutil_fast_socket_nonblocking(r) < 0) {
evutil_closesocket(r);
return -1;
}
}
return r;
#else
return -1;
#endif
}

void
evutil_free_globals_(void)
{
evutil_free_secure_rng_globals_();
evutil_free_sock_err_globals();
}
