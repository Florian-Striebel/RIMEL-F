























#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#if defined(DECNETLIB)
#include <sys/types.h>
#include <netdnet/dnetdb.h>
#endif

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>

#if defined(INET6)





















#include <wspiapi.h>
#endif
#else
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <netinet/in.h>

#if defined(HAVE_ETHER_HOSTTON)
#if defined(NET_ETHERNET_H_DECLARES_ETHER_HOSTTON)



#include <net/ethernet.h>
#elif defined(NETINET_ETHER_H_DECLARES_ETHER_HOSTTON)



#include <netinet/ether.h>
#elif defined(SYS_ETHERNET_H_DECLARES_ETHER_HOSTTON)



#include <sys/ethernet.h>
#elif defined(ARPA_INET_H_DECLARES_ETHER_HOSTTON)



#include <arpa/inet.h>
#elif defined(NETINET_IF_ETHER_H_DECLARES_ETHER_HOSTTON)




#define NEED_NETINET_IF_ETHER_H
#else





#if defined(HAVE_STRUCT_ETHER_ADDR)
#define NEED_NETINET_IF_ETHER_H
#else
struct ether_addr {
unsigned char ether_addr_octet[6];
};
#endif
#endif

#if defined(NEED_NETINET_IF_ETHER_H)
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#endif

#if !defined(HAVE_DECL_ETHER_HOSTTON)



extern int ether_hostton(const char *, struct ether_addr *);
#endif
#endif

#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pcap-int.h"

#include "gencode.h"
#include <pcap/namedb.h>
#include "nametoaddr.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

#if !defined(NTOHL)
#define NTOHL(x) (x) = ntohl(x)
#define NTOHS(x) (x) = ntohs(x)
#endif






bpf_u_int32 **
pcap_nametoaddr(const char *name)
{
#if !defined(h_addr)
static bpf_u_int32 *hlist[2];
#endif
bpf_u_int32 **p;
struct hostent *hp;

if ((hp = gethostbyname(name)) != NULL) {
#if !defined(h_addr)
hlist[0] = (bpf_u_int32 *)hp->h_addr;
NTOHL(hp->h_addr);
return hlist;
#else
for (p = (bpf_u_int32 **)hp->h_addr_list; *p; ++p)
NTOHL(**p);
return (bpf_u_int32 **)hp->h_addr_list;
#endif
}
else
return 0;
}

struct addrinfo *
pcap_nametoaddrinfo(const char *name)
{
struct addrinfo hints, *res;
int error;

memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = IPPROTO_TCP;
error = getaddrinfo(name, NULL, &hints, &res);
if (error)
return NULL;
else
return res;
}







bpf_u_int32
pcap_nametonetaddr(const char *name)
{
#if defined(_WIN32)












return 0;
#else



struct netent *np;
#if defined(HAVE_LINUX_GETNETBYNAME_R)



struct netent result_buf;
char buf[1024];
int h_errnoval;
int err;















np = NULL;
err = getnetbyname_r(name, &result_buf, buf, sizeof buf, &np,
&h_errnoval);
if (err != 0) {




return 0;
}
#elif defined(HAVE_SOLARIS_IRIX_GETNETBYNAME_R)



struct netent result_buf;
char buf[1024];

np = getnetbyname_r(name, &result_buf, buf, (int)sizeof buf);
#elif defined(HAVE_AIX_GETNETBYNAME_R)



struct netent result_buf;
struct netent_data net_data;

if (getnetbyname_r(name, &result_buf, &net_data) == -1)
np = NULL;
else
np = &result_buf;
#else










np = getnetbyname(name);
#endif
if (np != NULL)
return np->n_net;
else
return 0;
#endif
}






int
pcap_nametoport(const char *name, int *port, int *proto)
{
struct addrinfo hints, *res, *ai;
int error;
struct sockaddr_in *in4;
#if defined(INET6)
struct sockaddr_in6 *in6;
#endif
int tcp_port = -1;
int udp_port = -1;





memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = IPPROTO_TCP;
error = getaddrinfo(NULL, name, &hints, &res);
if (error != 0) {
if (error != EAI_NONAME &&
error != EAI_SERVICE) {





return 0;
}
} else {



for (ai = res; ai != NULL; ai = ai->ai_next) {



if (ai->ai_addr != NULL) {



if (ai->ai_addr->sa_family == AF_INET) {
in4 = (struct sockaddr_in *)ai->ai_addr;
tcp_port = ntohs(in4->sin_port);
break;
}
#if defined(INET6)
if (ai->ai_addr->sa_family == AF_INET6) {
in6 = (struct sockaddr_in6 *)ai->ai_addr;
tcp_port = ntohs(in6->sin6_port);
break;
}
#endif
}
}
freeaddrinfo(res);
}

memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_DGRAM;
hints.ai_protocol = IPPROTO_UDP;
error = getaddrinfo(NULL, name, &hints, &res);
if (error != 0) {
if (error != EAI_NONAME &&
error != EAI_SERVICE) {





return 0;
}
} else {



for (ai = res; ai != NULL; ai = ai->ai_next) {



if (ai->ai_addr != NULL) {



if (ai->ai_addr->sa_family == AF_INET) {
in4 = (struct sockaddr_in *)ai->ai_addr;
udp_port = ntohs(in4->sin_port);
break;
}
#if defined(INET6)
if (ai->ai_addr->sa_family == AF_INET6) {
in6 = (struct sockaddr_in6 *)ai->ai_addr;
udp_port = ntohs(in6->sin6_port);
break;
}
#endif
}
}
freeaddrinfo(res);
}







if (tcp_port >= 0) {
*port = tcp_port;
*proto = IPPROTO_TCP;
if (udp_port >= 0) {
if (udp_port == tcp_port)
*proto = PROTO_UNDEF;
#if defined(notdef)
else


warning("ambiguous port %s in /etc/services",
name);
#endif
}
return 1;
}
if (udp_port >= 0) {
*port = udp_port;
*proto = IPPROTO_UDP;
return 1;
}
#if defined(ultrix) || defined(__osf__)

if (strcmp(name, "nfs") == 0) {
*port = 2049;
*proto = PROTO_UNDEF;
return 1;
}
#endif
return 0;
}






int
pcap_nametoportrange(const char *name, int *port1, int *port2, int *proto)
{
u_int p1, p2;
char *off, *cpy;
int save_proto;

if (sscanf(name, "%d-%d", &p1, &p2) != 2) {
if ((cpy = strdup(name)) == NULL)
return 0;

if ((off = strchr(cpy, '-')) == NULL) {
free(cpy);
return 0;
}

*off = '\0';

if (pcap_nametoport(cpy, port1, proto) == 0) {
free(cpy);
return 0;
}
save_proto = *proto;

if (pcap_nametoport(off + 1, port2, proto) == 0) {
free(cpy);
return 0;
}
free(cpy);

if (*proto != save_proto)
*proto = PROTO_UNDEF;
} else {
*port1 = p1;
*port2 = p2;
*proto = PROTO_UNDEF;
}

return 1;
}





int
pcap_nametoproto(const char *str)
{
struct protoent *p;
#if defined(HAVE_LINUX_GETNETBYNAME_R)



struct protoent result_buf;
char buf[1024];
int err;

err = getprotobyname_r(str, &result_buf, buf, sizeof buf, &p);
if (err != 0) {




return 0;
}
#elif defined(HAVE_SOLARIS_IRIX_GETNETBYNAME_R)



struct protoent result_buf;
char buf[1024];

p = getprotobyname_r(str, &result_buf, buf, (int)sizeof buf);
#elif defined(HAVE_AIX_GETNETBYNAME_R)



struct protoent result_buf;
struct protoent_data proto_data;

if (getprotobyname_r(str, &result_buf, &proto_data) == -1)
p = NULL;
else
p = &result_buf;
#else










p = getprotobyname(str);
#endif
if (p != 0)
return p->p_proto;
else
return PROTO_UNDEF;
}

#include "ethertype.h"

struct eproto {
const char *s;
u_short p;
};












PCAP_API struct eproto eproto_db[];
PCAP_API_DEF struct eproto eproto_db[] = {
{ "pup", ETHERTYPE_PUP },
{ "xns", ETHERTYPE_NS },
{ "ip", ETHERTYPE_IP },
#if defined(INET6)
{ "ip6", ETHERTYPE_IPV6 },
#endif
{ "arp", ETHERTYPE_ARP },
{ "rarp", ETHERTYPE_REVARP },
{ "sprite", ETHERTYPE_SPRITE },
{ "mopdl", ETHERTYPE_MOPDL },
{ "moprc", ETHERTYPE_MOPRC },
{ "decnet", ETHERTYPE_DN },
{ "lat", ETHERTYPE_LAT },
{ "sca", ETHERTYPE_SCA },
{ "lanbridge", ETHERTYPE_LANBRIDGE },
{ "vexp", ETHERTYPE_VEXP },
{ "vprod", ETHERTYPE_VPROD },
{ "atalk", ETHERTYPE_ATALK },
{ "atalkarp", ETHERTYPE_AARP },
{ "loopback", ETHERTYPE_LOOPBACK },
{ "decdts", ETHERTYPE_DECDTS },
{ "decdns", ETHERTYPE_DECDNS },
{ (char *)0, 0 }
};

int
pcap_nametoeproto(const char *s)
{
struct eproto *p = eproto_db;

while (p->s != 0) {
if (strcmp(p->s, s) == 0)
return p->p;
p += 1;
}
return PROTO_UNDEF;
}

#include "llc.h"


static struct eproto llc_db[] = {
{ "iso", LLCSAP_ISONS },
{ "stp", LLCSAP_8021D },
{ "ipx", LLCSAP_IPX },
{ "netbeui", LLCSAP_NETBEUI },
{ (char *)0, 0 }
};

int
pcap_nametollc(const char *s)
{
struct eproto *p = llc_db;

while (p->s != 0) {
if (strcmp(p->s, s) == 0)
return p->p;
p += 1;
}
return PROTO_UNDEF;
}


static inline u_char
xdtoi(u_char c)
{
if (isdigit(c))
return (u_char)(c - '0');
else if (islower(c))
return (u_char)(c - 'a' + 10);
else
return (u_char)(c - 'A' + 10);
}

int
__pcap_atoin(const char *s, bpf_u_int32 *addr)
{
u_int n;
int len;

*addr = 0;
len = 0;
for (;;) {
n = 0;
while (*s && *s != '.') {
if (n > 25) {

return -1;
}
n = n * 10 + *s++ - '0';
}
if (n > 255)
return -1;
*addr <<= 8;
*addr |= n & 0xff;
len += 8;
if (*s == '\0')
return len;
++s;
}

}

int
__pcap_atodn(const char *s, bpf_u_int32 *addr)
{
#define AREASHIFT 10
#define AREAMASK 0176000
#define NODEMASK 01777

u_int node, area;

if (sscanf(s, "%d.%d", &area, &node) != 2)
return(0);

*addr = (area << AREASHIFT) & AREAMASK;
*addr |= (node & NODEMASK);

return(32);
}













u_char *
pcap_ether_aton(const char *s)
{
register u_char *ep, *e;
register u_char d;

e = ep = (u_char *)malloc(6);
if (e == NULL)
return (NULL);

while (*s) {
if (*s == ':' || *s == '.' || *s == '-')
s += 1;
d = xdtoi(*s++);
if (isxdigit((unsigned char)*s)) {
d <<= 4;
d |= xdtoi(*s++);
}
*ep++ = d;
}

return (e);
}

#if !defined(HAVE_ETHER_HOSTTON)





u_char *
pcap_ether_hostton(const char *name)
{
register struct pcap_etherent *ep;
register u_char *ap;
static FILE *fp = NULL;
static int init = 0;

if (!init) {
fp = fopen(PCAP_ETHERS_FILE, "r");
++init;
if (fp == NULL)
return (NULL);
} else if (fp == NULL)
return (NULL);
else
rewind(fp);

while ((ep = pcap_next_etherent(fp)) != NULL) {
if (strcmp(ep->name, name) == 0) {
ap = (u_char *)malloc(6);
if (ap != NULL) {
memcpy(ap, ep->addr, 6);
return (ap);
}
break;
}
}
return (NULL);
}
#else




u_char *
pcap_ether_hostton(const char *name)
{
register u_char *ap;
u_char a[6];

ap = NULL;
if (ether_hostton(name, (struct ether_addr *)a) == 0) {
ap = (u_char *)malloc(6);
if (ap != NULL)
memcpy((char *)ap, (char *)a, 6);
}
return (ap);
}
#endif




int
#if defined(DECNETLIB)
__pcap_nametodnaddr(const char *name, u_short *res)
{
struct nodeent *getnodebyname();
struct nodeent *nep;

nep = getnodebyname(name);
if (nep == ((struct nodeent *)0))
return(0);

memcpy((char *)res, (char *)nep->n_addr, sizeof(unsigned short));
return(1);
#else
__pcap_nametodnaddr(const char *name _U_, u_short *res _U_)
{
return(0);
#endif
}
