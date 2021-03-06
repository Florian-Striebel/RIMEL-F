

































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <net/if.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>

#include "pcap-int.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif







#if (defined(linux) || defined(__Lynx__)) && defined(AF_PACKET)
#if defined(HAVE_NETPACKET_PACKET_H)

#include <netpacket/packet.h>
#else

#if defined(__Lynx__)

#include <netpacket/if_packet.h>
#else

#include <linux/types.h>
#include <linux/if_packet.h>
#endif
#endif
#endif
























#if !defined(SA_LEN)
#if defined(HAVE_STRUCT_SOCKADDR_SA_LEN)
#define SA_LEN(addr) ((addr)->sa_len)
#else
#if defined(HAVE_STRUCT_SOCKADDR_STORAGE)
static size_t
get_sa_len(struct sockaddr *addr)
{
switch (addr->sa_family) {

#if defined(AF_INET)
case AF_INET:
return (sizeof (struct sockaddr_in));
#endif

#if defined(AF_INET6)
case AF_INET6:
return (sizeof (struct sockaddr_in6));
#endif

#if (defined(linux) || defined(__Lynx__)) && defined(AF_PACKET)
case AF_PACKET:
return (sizeof (struct sockaddr_ll));
#endif

default:
return (sizeof (struct sockaddr));
}
}
#define SA_LEN(addr) (get_sa_len(addr))
#else
#define SA_LEN(addr) (sizeof (struct sockaddr))
#endif
#endif
#endif







int
pcap_findalldevs_interfaces(pcap_if_list_t *devlistp, char *errbuf,
int (*check_usable)(const char *), get_if_flags_func get_flags_func)
{
struct ifaddrs *ifap, *ifa;
struct sockaddr *addr, *netmask, *broadaddr, *dstaddr;
size_t addr_size, broadaddr_size, dstaddr_size;
int ret = 0;
char *p, *q;















if (getifaddrs(&ifap) != 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "getifaddrs");
return (-1);
}
for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {











p = strchr(ifa->ifa_name, ':');
if (p != NULL) {



q = p + 1;
while (isdigit((unsigned char)*q))
q++;
if (*q == '\0') {





*p = '\0';
}
}




if (!(*check_usable)(ifa->ifa_name)) {



continue;
}








if (ifa->ifa_addr != NULL) {
addr = ifa->ifa_addr;
addr_size = SA_LEN(addr);
netmask = ifa->ifa_netmask;
} else {
addr = NULL;
addr_size = 0;
netmask = NULL;
}
















if (ifa->ifa_flags & IFF_BROADCAST &&
ifa->ifa_broadaddr != NULL) {
broadaddr = ifa->ifa_broadaddr;
broadaddr_size = SA_LEN(broadaddr);
} else {
broadaddr = NULL;
broadaddr_size = 0;
}
if (ifa->ifa_flags & IFF_POINTOPOINT &&
ifa->ifa_dstaddr != NULL) {
dstaddr = ifa->ifa_dstaddr;
dstaddr_size = SA_LEN(ifa->ifa_dstaddr);
} else {
dstaddr = NULL;
dstaddr_size = 0;
}




if (add_addr_to_if(devlistp, ifa->ifa_name, ifa->ifa_flags,
get_flags_func,
addr, addr_size, netmask, addr_size,
broadaddr, broadaddr_size, dstaddr, dstaddr_size,
errbuf) < 0) {
ret = -1;
break;
}
}

freeifaddrs(ifap);

return (ret);
}
