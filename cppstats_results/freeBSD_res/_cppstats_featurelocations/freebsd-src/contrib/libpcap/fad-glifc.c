

































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/param.h>
#include <sys/file.h>
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

#include "pcap-int.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif











int
pcap_findalldevs_interfaces(pcap_if_list_t *devlistp, char *errbuf,
int (*check_usable)(const char *), get_if_flags_func get_flags_func)
{
register int fd4, fd6, fd;
register struct lifreq *ifrp, *ifend;
struct lifnum ifn;
struct lifconf ifc;
char *buf = NULL;
unsigned buf_size;
#if defined(HAVE_SOLARIS)
char *p, *q;
#endif
struct lifreq ifrflags, ifrnetmask, ifrbroadaddr, ifrdstaddr;
struct sockaddr *netmask, *broadaddr, *dstaddr;
int ret = 0;





fd4 = socket(AF_INET, SOCK_DGRAM, 0);
if (fd4 < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "socket: AF_INET");
return (-1);
}




fd6 = socket(AF_INET6, SOCK_DGRAM, 0);
if (fd6 < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "socket: AF_INET6");
(void)close(fd4);
return (-1);
}




ifn.lifn_family = AF_UNSPEC;
ifn.lifn_flags = 0;
ifn.lifn_count = 0;
if (ioctl(fd4, SIOCGLIFNUM, (char *)&ifn) < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGLIFNUM");
(void)close(fd6);
(void)close(fd4);
return (-1);
}




buf_size = ifn.lifn_count * sizeof (struct lifreq);
buf = malloc(buf_size);
if (buf == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
(void)close(fd6);
(void)close(fd4);
return (-1);
}




ifc.lifc_len = buf_size;
ifc.lifc_buf = buf;
ifc.lifc_family = AF_UNSPEC;
ifc.lifc_flags = 0;
memset(buf, 0, buf_size);
if (ioctl(fd4, SIOCGLIFCONF, (char *)&ifc) < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGLIFCONF");
(void)close(fd6);
(void)close(fd4);
free(buf);
return (-1);
}




ifrp = (struct lifreq *)buf;
ifend = (struct lifreq *)(buf + ifc.lifc_len);

for (; ifrp < ifend; ifrp++) {





if (strncmp(ifrp->lifr_name, "dummy", 5) == 0)
continue;




if (!(*check_usable)(ifrp->lifr_name)) {



continue;
}




if (((struct sockaddr *)&ifrp->lifr_addr)->sa_family == AF_INET6)
fd = fd6;
else
fd = fd4;




strncpy(ifrflags.lifr_name, ifrp->lifr_name,
sizeof(ifrflags.lifr_name));
if (ioctl(fd, SIOCGLIFFLAGS, (char *)&ifrflags) < 0) {
if (errno == ENXIO)
continue;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGLIFFLAGS: %.*s",
(int)sizeof(ifrflags.lifr_name),
ifrflags.lifr_name);
ret = -1;
break;
}




strncpy(ifrnetmask.lifr_name, ifrp->lifr_name,
sizeof(ifrnetmask.lifr_name));
memcpy(&ifrnetmask.lifr_addr, &ifrp->lifr_addr,
sizeof(ifrnetmask.lifr_addr));
if (ioctl(fd, SIOCGLIFNETMASK, (char *)&ifrnetmask) < 0) {
if (errno == EADDRNOTAVAIL) {



netmask = NULL;
} else {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCGLIFNETMASK: %.*s",
(int)sizeof(ifrnetmask.lifr_name),
ifrnetmask.lifr_name);
ret = -1;
break;
}
} else
netmask = (struct sockaddr *)&ifrnetmask.lifr_addr;





if (ifrflags.lifr_flags & IFF_BROADCAST) {
strncpy(ifrbroadaddr.lifr_name, ifrp->lifr_name,
sizeof(ifrbroadaddr.lifr_name));
memcpy(&ifrbroadaddr.lifr_addr, &ifrp->lifr_addr,
sizeof(ifrbroadaddr.lifr_addr));
if (ioctl(fd, SIOCGLIFBRDADDR,
(char *)&ifrbroadaddr) < 0) {
if (errno == EADDRNOTAVAIL) {



broadaddr = NULL;
} else {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCGLIFBRDADDR: %.*s",
(int)sizeof(ifrbroadaddr.lifr_name),
ifrbroadaddr.lifr_name);
ret = -1;
break;
}
} else
broadaddr = (struct sockaddr *)&ifrbroadaddr.lifr_broadaddr;
} else {




broadaddr = NULL;
}





if (ifrflags.lifr_flags & IFF_POINTOPOINT) {
strncpy(ifrdstaddr.lifr_name, ifrp->lifr_name,
sizeof(ifrdstaddr.lifr_name));
memcpy(&ifrdstaddr.lifr_addr, &ifrp->lifr_addr,
sizeof(ifrdstaddr.lifr_addr));
if (ioctl(fd, SIOCGLIFDSTADDR,
(char *)&ifrdstaddr) < 0) {
if (errno == EADDRNOTAVAIL) {



dstaddr = NULL;
} else {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCGLIFDSTADDR: %.*s",
(int)sizeof(ifrdstaddr.lifr_name),
ifrdstaddr.lifr_name);
ret = -1;
break;
}
} else
dstaddr = (struct sockaddr *)&ifrdstaddr.lifr_dstaddr;
} else
dstaddr = NULL;

#if defined(HAVE_SOLARIS)








p = strchr(ifrp->lifr_name, ':');
if (p != NULL) {



q = p + 1;
while (isdigit((unsigned char)*q))
q++;
if (*q == '\0') {





*p = '\0';
}
}
#endif




if (add_addr_to_if(devlistp, ifrp->lifr_name,
ifrflags.lifr_flags, get_flags_func,
(struct sockaddr *)&ifrp->lifr_addr,
sizeof (struct sockaddr_storage),
netmask, sizeof (struct sockaddr_storage),
broadaddr, sizeof (struct sockaddr_storage),
dstaddr, sizeof (struct sockaddr_storage), errbuf) < 0) {
ret = -1;
break;
}
}
free(buf);
(void)close(fd6);
(void)close(fd4);

return (ret);
}
