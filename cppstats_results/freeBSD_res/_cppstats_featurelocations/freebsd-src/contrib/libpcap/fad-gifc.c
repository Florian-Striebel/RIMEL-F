

































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#if defined(HAVE_SYS_SOCKIO_H)
#include <sys/sockio.h>
#endif
#include <sys/time.h>

struct mbuf;
struct rtentry;
#include <net/if.h>
#include <netinet/in.h>

#include <ctype.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#else
#define INT_MAX 2147483647
#endif

#include "pcap-int.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif


























#if !defined(SA_LEN)
#if defined(HAVE_STRUCT_SOCKADDR_SA_LEN)
#define SA_LEN(addr) ((addr)->sa_len)
#else
#define SA_LEN(addr) (sizeof (struct sockaddr))
#endif
#endif




















#define MAX_SA_LEN 255















int
pcap_findalldevs_interfaces(pcap_if_list_t *devlistp, char *errbuf,
int (*check_usable)(const char *), get_if_flags_func get_flags_func)
{
register int fd;
register struct ifreq *ifrp, *ifend, *ifnext;
size_t n;
struct ifconf ifc;
char *buf = NULL;
unsigned buf_size;
#if defined (HAVE_SOLARIS) || defined (HAVE_HPUX10_20_OR_LATER)
char *p, *q;
#endif
struct ifreq ifrflags, ifrnetmask, ifrbroadaddr, ifrdstaddr;
struct sockaddr *netmask, *broadaddr, *dstaddr;
size_t netmask_size, broadaddr_size, dstaddr_size;
int ret = 0;




fd = socket(AF_INET, SOCK_DGRAM, 0);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
return (-1);
}








buf_size = 8192;
for (;;) {



if (buf_size > INT_MAX) {
(void)pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"interface information requires more than %u bytes",
INT_MAX);
(void)close(fd);
return (-1);
}
buf = malloc(buf_size);
if (buf == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
(void)close(fd);
return (-1);
}

ifc.ifc_len = buf_size;
ifc.ifc_buf = buf;
memset(buf, 0, buf_size);
if (ioctl(fd, SIOCGIFCONF, (char *)&ifc) < 0
&& errno != EINVAL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFCONF");
(void)close(fd);
free(buf);
return (-1);
}
if (ifc.ifc_len < (int)buf_size &&
(buf_size - ifc.ifc_len) > sizeof(ifrp->ifr_name) + MAX_SA_LEN)
break;
free(buf);
buf_size *= 2;
}

ifrp = (struct ifreq *)buf;
ifend = (struct ifreq *)(buf + ifc.ifc_len);

for (; ifrp < ifend; ifrp = ifnext) {










n = SA_LEN(&ifrp->ifr_addr) + sizeof(ifrp->ifr_name);
if (n < sizeof(*ifrp))
ifnext = ifrp + 1;
else
ifnext = (struct ifreq *)((char *)ifrp + n);












if (!(*ifrp->ifr_name))
break;






if (strncmp(ifrp->ifr_name, "dummy", 5) == 0)
continue;




if (!(*check_usable)(ifrp->ifr_name)) {



continue;
}




strncpy(ifrflags.ifr_name, ifrp->ifr_name,
sizeof(ifrflags.ifr_name));
if (ioctl(fd, SIOCGIFFLAGS, (char *)&ifrflags) < 0) {
if (errno == ENXIO)
continue;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFFLAGS: %.*s",
(int)sizeof(ifrflags.ifr_name),
ifrflags.ifr_name);
ret = -1;
break;
}




strncpy(ifrnetmask.ifr_name, ifrp->ifr_name,
sizeof(ifrnetmask.ifr_name));
memcpy(&ifrnetmask.ifr_addr, &ifrp->ifr_addr,
sizeof(ifrnetmask.ifr_addr));
if (ioctl(fd, SIOCGIFNETMASK, (char *)&ifrnetmask) < 0) {
if (errno == EADDRNOTAVAIL) {



netmask = NULL;
netmask_size = 0;
} else {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCGIFNETMASK: %.*s",
(int)sizeof(ifrnetmask.ifr_name),
ifrnetmask.ifr_name);
ret = -1;
break;
}
} else {
netmask = &ifrnetmask.ifr_addr;
netmask_size = SA_LEN(netmask);
}





if (ifrflags.ifr_flags & IFF_BROADCAST) {
strncpy(ifrbroadaddr.ifr_name, ifrp->ifr_name,
sizeof(ifrbroadaddr.ifr_name));
memcpy(&ifrbroadaddr.ifr_addr, &ifrp->ifr_addr,
sizeof(ifrbroadaddr.ifr_addr));
if (ioctl(fd, SIOCGIFBRDADDR,
(char *)&ifrbroadaddr) < 0) {
if (errno == EADDRNOTAVAIL) {



broadaddr = NULL;
broadaddr_size = 0;
} else {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCGIFBRDADDR: %.*s",
(int)sizeof(ifrbroadaddr.ifr_name),
ifrbroadaddr.ifr_name);
ret = -1;
break;
}
} else {
broadaddr = &ifrbroadaddr.ifr_broadaddr;
broadaddr_size = SA_LEN(broadaddr);
}
} else {




broadaddr = NULL;
broadaddr_size = 0;
}





if (ifrflags.ifr_flags & IFF_POINTOPOINT) {
strncpy(ifrdstaddr.ifr_name, ifrp->ifr_name,
sizeof(ifrdstaddr.ifr_name));
memcpy(&ifrdstaddr.ifr_addr, &ifrp->ifr_addr,
sizeof(ifrdstaddr.ifr_addr));
if (ioctl(fd, SIOCGIFDSTADDR,
(char *)&ifrdstaddr) < 0) {
if (errno == EADDRNOTAVAIL) {



dstaddr = NULL;
dstaddr_size = 0;
} else {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCGIFDSTADDR: %.*s",
(int)sizeof(ifrdstaddr.ifr_name),
ifrdstaddr.ifr_name);
ret = -1;
break;
}
} else {
dstaddr = &ifrdstaddr.ifr_dstaddr;
dstaddr_size = SA_LEN(dstaddr);
}
} else {




dstaddr = NULL;
dstaddr_size = 0;
}

#if defined (HAVE_SOLARIS) || defined (HAVE_HPUX10_20_OR_LATER)








p = strchr(ifrp->ifr_name, ':');
if (p != NULL) {



q = p + 1;
while (isdigit((unsigned char)*q))
q++;
if (*q == '\0') {





*p = '\0';
}
}
#endif




if (add_addr_to_if(devlistp, ifrp->ifr_name,
ifrflags.ifr_flags, get_flags_func,
&ifrp->ifr_addr, SA_LEN(&ifrp->ifr_addr),
netmask, netmask_size, broadaddr, broadaddr_size,
dstaddr, dstaddr_size, errbuf) < 0) {
ret = -1;
break;
}
}
free(buf);
(void)close(fd);

return (ret);
}
