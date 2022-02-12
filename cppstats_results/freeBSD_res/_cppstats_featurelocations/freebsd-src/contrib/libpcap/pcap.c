
































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <pcap-types.h>
#if !defined(_WIN32)
#include <sys/param.h>
#if !defined(MSDOS)
#include <sys/file.h>
#endif
#include <sys/ioctl.h>
#include <sys/socket.h>
#if defined(HAVE_SYS_SOCKIO_H)
#include <sys/sockio.h>
#endif

struct mbuf;
struct rtentry;
#include <net/if.h>
#include <netinet/in.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_MSC_VER) && !defined(__BORLANDC__) && !defined(__MINGW32__)
#include <unistd.h>
#endif
#include <fcntl.h>
#include <errno.h>
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#else
#define INT_MAX 2147483647
#endif

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

#if defined(MSDOS)
#include "pcap-dos.h"
#endif

#include "pcap-int.h"

#include "optimize.h"

#if defined(HAVE_DAG_API)
#include "pcap-dag.h"
#endif

#if defined(HAVE_SEPTEL_API)
#include "pcap-septel.h"
#endif

#if defined(HAVE_SNF_API)
#include "pcap-snf.h"
#endif

#if defined(HAVE_TC_API)
#include "pcap-tc.h"
#endif

#if defined(PCAP_SUPPORT_USB)
#include "pcap-usb-linux.h"
#endif

#if defined(PCAP_SUPPORT_BT)
#include "pcap-bt-linux.h"
#endif

#if defined(PCAP_SUPPORT_BT_MONITOR)
#include "pcap-bt-monitor-linux.h"
#endif

#if defined(PCAP_SUPPORT_NETFILTER)
#include "pcap-netfilter-linux.h"
#endif

#if defined(PCAP_SUPPORT_NETMAP)
#include "pcap-netmap.h"
#endif

#if defined(PCAP_SUPPORT_DBUS)
#include "pcap-dbus.h"
#endif

#if defined(PCAP_SUPPORT_RDMASNIFF)
#include "pcap-rdmasniff.h"
#endif

#if defined(_WIN32)



BOOL WINAPI DllMain(
HANDLE hinstDLL,
DWORD dwReason,
LPVOID lpvReserved
)
{
return (TRUE);
}






int
wsockinit(void)
{
WORD wVersionRequested;
WSADATA wsaData;
static int err = -1;
static int done = 0;

if (done)
return (err);

wVersionRequested = MAKEWORD( 1, 1);
err = WSAStartup( wVersionRequested, &wsaData );
atexit ((void(*)(void))WSACleanup);
done = 1;

if ( err != 0 )
err = -1;
return (err);
}




int
pcap_wsockinit(void)
{
return (wsockinit());
}
#endif











PCAP_API char pcap_version[];
PCAP_API_DEF char pcap_version[] = PACKAGE_VERSION;

static void
pcap_set_not_initialized_message(pcap_t *pcap)
{
if (pcap->activated) {

(void)pcap_snprintf(pcap->errbuf, sizeof(pcap->errbuf),
"This operation isn't properly handled by that device");
return;
}

(void)pcap_snprintf(pcap->errbuf, sizeof(pcap->errbuf),
"This handle hasn't been activated yet");
}

static int
pcap_read_not_initialized(pcap_t *pcap, int cnt _U_, pcap_handler callback _U_,
u_char *user _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_inject_not_initialized(pcap_t *pcap, const void * buf _U_, size_t size _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_setfilter_not_initialized(pcap_t *pcap, struct bpf_program *fp _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_setdirection_not_initialized(pcap_t *pcap, pcap_direction_t d _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_set_datalink_not_initialized(pcap_t *pcap, int dlt _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_getnonblock_not_initialized(pcap_t *pcap)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_stats_not_initialized(pcap_t *pcap, struct pcap_stat *ps _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

#if defined(_WIN32)
struct pcap_stat *
pcap_stats_ex_not_initialized(pcap_t *pcap, int *pcap_stat_size _U_)
{
pcap_set_not_initialized_message(pcap);
return (NULL);
}

static int
pcap_setbuff_not_initialized(pcap_t *pcap, int dim _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_setmode_not_initialized(pcap_t *pcap, int mode _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_setmintocopy_not_initialized(pcap_t *pcap, int size _U_)
{
pcap_set_not_initialized_message(pcap);

return (PCAP_ERROR_NOT_ACTIVATED);
}

static HANDLE
pcap_getevent_not_initialized(pcap_t *pcap)
{
pcap_set_not_initialized_message(pcap);
return (INVALID_HANDLE_VALUE);
}

static int
pcap_oid_get_request_not_initialized(pcap_t *pcap, bpf_u_int32 oid _U_,
void *data _U_, size_t *lenp _U_)
{
pcap_set_not_initialized_message(pcap);
return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_oid_set_request_not_initialized(pcap_t *pcap, bpf_u_int32 oid _U_,
const void *data _U_, size_t *lenp _U_)
{
pcap_set_not_initialized_message(pcap);
return (PCAP_ERROR_NOT_ACTIVATED);
}

static u_int
pcap_sendqueue_transmit_not_initialized(pcap_t *pcap, pcap_send_queue* queue, int sync)
{
pcap_set_not_initialized_message(pcap);
return (0);
}

static int
pcap_setuserbuffer_not_initialized(pcap_t *pcap, int size _U_)
{
pcap_set_not_initialized_message(pcap);
return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_live_dump_not_initialized(pcap_t *pcap, char *filename _U_, int maxsize _U_,
int maxpacks _U_)
{
pcap_set_not_initialized_message(pcap);
return (PCAP_ERROR_NOT_ACTIVATED);
}

static int
pcap_live_dump_ended_not_initialized(pcap_t *pcap, int sync _U_)
{
pcap_set_not_initialized_message(pcap);
return (PCAP_ERROR_NOT_ACTIVATED);
}

static PAirpcapHandle
pcap_get_airpcap_handle_not_initialized(pcap_t *pcap)
{
pcap_set_not_initialized_message(pcap);
return (NULL);
}
#endif





int
pcap_can_set_rfmon(pcap_t *p)
{
return (p->can_set_rfmon_op(p));
}




static int
pcap_cant_set_rfmon(pcap_t *p _U_)
{
return (0);
}












int
pcap_list_tstamp_types(pcap_t *p, int **tstamp_typesp)
{
if (p->tstamp_type_count == 0) {





*tstamp_typesp = (int*)malloc(sizeof(**tstamp_typesp));
if (*tstamp_typesp == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "malloc");
return (PCAP_ERROR);
}
**tstamp_typesp = PCAP_TSTAMP_HOST;
return (1);
} else {
*tstamp_typesp = (int*)calloc(sizeof(**tstamp_typesp),
p->tstamp_type_count);
if (*tstamp_typesp == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "malloc");
return (PCAP_ERROR);
}
(void)memcpy(*tstamp_typesp, p->tstamp_type_list,
sizeof(**tstamp_typesp) * p->tstamp_type_count);
return (p->tstamp_type_count);
}
}












void
pcap_free_tstamp_types(int *tstamp_type_list)
{
free(tstamp_type_list);
}






void
pcap_oneshot(u_char *user, const struct pcap_pkthdr *h, const u_char *pkt)
{
struct oneshot_userdata *sp = (struct oneshot_userdata *)user;

*sp->hdr = *h;
*sp->pkt = pkt;
}

const u_char *
pcap_next(pcap_t *p, struct pcap_pkthdr *h)
{
struct oneshot_userdata s;
const u_char *pkt;

s.hdr = h;
s.pkt = &pkt;
s.pd = p;
if (pcap_dispatch(p, 1, p->oneshot_callback, (u_char *)&s) <= 0)
return (0);
return (pkt);
}

int
pcap_next_ex(pcap_t *p, struct pcap_pkthdr **pkt_header,
const u_char **pkt_data)
{
struct oneshot_userdata s;

s.hdr = &p->pcap_header;
s.pkt = pkt_data;
s.pd = p;


*pkt_header= &p->pcap_header;

if (p->rfile != NULL) {
int status;


status = pcap_offline_read(p, 1, p->oneshot_callback,
(u_char *)&s);













if (status == 0)
return (-2);
else
return (status);
}










return (p->read_op(p, 1, p->oneshot_callback, (u_char *)&s));
}




struct pcap_if_list {
pcap_if_t *beginning;
};

static struct capture_source_type {
int (*findalldevs_op)(pcap_if_list_t *, char *);
pcap_t *(*create_op)(const char *, char *, int *);
} capture_source_types[] = {
#if defined(HAVE_DAG_API)
{ dag_findalldevs, dag_create },
#endif
#if defined(HAVE_SEPTEL_API)
{ septel_findalldevs, septel_create },
#endif
#if defined(HAVE_SNF_API)
{ snf_findalldevs, snf_create },
#endif
#if defined(HAVE_TC_API)
{ TcFindAllDevs, TcCreate },
#endif
#if defined(PCAP_SUPPORT_BT)
{ bt_findalldevs, bt_create },
#endif
#if defined(PCAP_SUPPORT_BT_MONITOR)
{ bt_monitor_findalldevs, bt_monitor_create },
#endif
#if defined(PCAP_SUPPORT_USB)
{ usb_findalldevs, usb_create },
#endif
#if defined(PCAP_SUPPORT_NETFILTER)
{ netfilter_findalldevs, netfilter_create },
#endif
#if defined(PCAP_SUPPORT_NETMAP)
{ pcap_netmap_findalldevs, pcap_netmap_create },
#endif
#if defined(PCAP_SUPPORT_DBUS)
{ dbus_findalldevs, dbus_create },
#endif
#if defined(PCAP_SUPPORT_RDMASNIFF)
{ rdmasniff_findalldevs, rdmasniff_create },
#endif
{ NULL, NULL }
};







int
pcap_findalldevs(pcap_if_t **alldevsp, char *errbuf)
{
size_t i;
pcap_if_list_t devlist;





devlist.beginning = NULL;
if (pcap_platform_finddevs(&devlist, errbuf) == -1) {




if (devlist.beginning != NULL)
pcap_freealldevs(devlist.beginning);
*alldevsp = NULL;
return (-1);
}





for (i = 0; capture_source_types[i].findalldevs_op != NULL; i++) {
if (capture_source_types[i].findalldevs_op(&devlist, errbuf) == -1) {




if (devlist.beginning != NULL)
pcap_freealldevs(devlist.beginning);
*alldevsp = NULL;
return (-1);
}
}




*alldevsp = devlist.beginning;
return (0);
}

static struct sockaddr *
dup_sockaddr(struct sockaddr *sa, size_t sa_length)
{
struct sockaddr *newsa;

if ((newsa = malloc(sa_length)) == NULL)
return (NULL);
return (memcpy(newsa, sa, sa_length));
}



















static u_int
get_figure_of_merit(pcap_if_t *dev)
{
const char *cp;
u_int n;

if (strcmp(dev->name, "any") == 0) {





n = 0x1FFFFFFF;
} else {







cp = dev->name + strlen(dev->name) - 1;
while (cp-1 >= dev->name && *(cp-1) >= '0' && *(cp-1) <= '9')
cp--;
if (*cp >= '0' && *cp <= '9')
n = atoi(cp) + 1;
else
n = 0;
}
if (!(dev->flags & PCAP_IF_RUNNING))
n |= 0x80000000;
if (!(dev->flags & PCAP_IF_UP))
n |= 0x40000000;











if (!(dev->flags & PCAP_IF_WIRELESS) &&
(dev->flags & PCAP_IF_CONNECTION_STATUS) == PCAP_IF_CONNECTION_STATUS_DISCONNECTED)
n |= 0x20000000;





if (dev->flags & PCAP_IF_LOOPBACK)
n |= 0x10000000;

return (n);
}

#if !defined(_WIN32)

















































static char *
#if defined(SIOCGIFDESCR)
get_if_description(const char *name)
{
char *description = NULL;
int s;
struct ifreq ifrdesc;
#if !defined(IFDESCRSIZE)
size_t descrlen = 64;
#else
size_t descrlen = IFDESCRSIZE;
#endif




memset(&ifrdesc, 0, sizeof ifrdesc);
pcap_strlcpy(ifrdesc.ifr_name, name, sizeof ifrdesc.ifr_name);
s = socket(AF_INET, SOCK_DGRAM, 0);
if (s >= 0) {
#if defined(__FreeBSD__)






for (;;) {
free(description);
if ((description = malloc(descrlen)) != NULL) {
ifrdesc.ifr_buffer.buffer = description;
ifrdesc.ifr_buffer.length = descrlen;
if (ioctl(s, SIOCGIFDESCR, &ifrdesc) == 0) {
if (ifrdesc.ifr_buffer.buffer ==
description)
break;
else
descrlen = ifrdesc.ifr_buffer.length;
} else {



free(description);
description = NULL;
break;
}
} else
break;
}
#else






if ((description = malloc(descrlen)) != NULL) {
ifrdesc.ifr_data = (caddr_t)description;
if (ioctl(s, SIOCGIFDESCR, &ifrdesc) != 0) {



free(description);
description = NULL;
}
}
#endif
close(s);
if (description != NULL && description[0] == '\0') {



free(description);
description = NULL;
}
}

#if defined(__FreeBSD__)





if (description == NULL) {
if (strncmp(name, "usbus", 5) == 0) {



long busnum;
char *p;

errno = 0;
busnum = strtol(name + 5, &p, 10);
if (errno == 0 && p != name + 5 && *p == '\0' &&
busnum >= 0 && busnum <= INT_MAX) {







if (pcap_asprintf(&description,
"USB bus number %ld", busnum) == -1) {

description = NULL;
}
}
}
}
#endif
return (description);
#else
get_if_description(const char *name _U_)
{
return (NULL);
#endif
}










pcap_if_t *
find_or_add_if(pcap_if_list_t *devlistp, const char *name,
bpf_u_int32 if_flags, get_if_flags_func get_flags_func, char *errbuf)
{
bpf_u_int32 pcap_flags;




pcap_flags = 0;
#if defined(IFF_LOOPBACK)
if (if_flags & IFF_LOOPBACK)
pcap_flags |= PCAP_IF_LOOPBACK;
#else




if (name[0] == 'l' && name[1] == 'o' &&
(isdigit((unsigned char)(name[2])) || name[2] == '\0')
pcap_flags |= PCAP_IF_LOOPBACK;
#endif
#if defined(IFF_UP)
if (if_flags & IFF_UP)
pcap_flags |= PCAP_IF_UP;
#endif
#if defined(IFF_RUNNING)
if (if_flags & IFF_RUNNING)
pcap_flags |= PCAP_IF_RUNNING;
#endif





return (find_or_add_dev(devlistp, name, pcap_flags,
get_flags_func, get_if_description(name), errbuf));
}



















int
add_addr_to_if(pcap_if_list_t *devlistp, const char *name,
bpf_u_int32 if_flags, get_if_flags_func get_flags_func,
struct sockaddr *addr, size_t addr_size,
struct sockaddr *netmask, size_t netmask_size,
struct sockaddr *broadaddr, size_t broadaddr_size,
struct sockaddr *dstaddr, size_t dstaddr_size,
char *errbuf)
{
pcap_if_t *curdev;




curdev = find_or_add_if(devlistp, name, if_flags, get_flags_func,
errbuf);
if (curdev == NULL) {



return (-1);
}

if (addr == NULL) {




return (0);
}






return (add_addr_to_dev(curdev, addr, addr_size, netmask,
netmask_size, broadaddr, broadaddr_size, dstaddr,
dstaddr_size, errbuf));
}
#endif





int
add_addr_to_dev(pcap_if_t *curdev,
struct sockaddr *addr, size_t addr_size,
struct sockaddr *netmask, size_t netmask_size,
struct sockaddr *broadaddr, size_t broadaddr_size,
struct sockaddr *dstaddr, size_t dstaddr_size,
char *errbuf)
{
pcap_addr_t *curaddr, *prevaddr, *nextaddr;




curaddr = (pcap_addr_t *)malloc(sizeof(pcap_addr_t));
if (curaddr == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (-1);
}

curaddr->next = NULL;
if (addr != NULL && addr_size != 0) {
curaddr->addr = (struct sockaddr *)dup_sockaddr(addr, addr_size);
if (curaddr->addr == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
free(curaddr);
return (-1);
}
} else
curaddr->addr = NULL;

if (netmask != NULL && netmask_size != 0) {
curaddr->netmask = (struct sockaddr *)dup_sockaddr(netmask, netmask_size);
if (curaddr->netmask == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
if (curaddr->addr != NULL)
free(curaddr->addr);
free(curaddr);
return (-1);
}
} else
curaddr->netmask = NULL;

if (broadaddr != NULL && broadaddr_size != 0) {
curaddr->broadaddr = (struct sockaddr *)dup_sockaddr(broadaddr, broadaddr_size);
if (curaddr->broadaddr == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
if (curaddr->netmask != NULL)
free(curaddr->netmask);
if (curaddr->addr != NULL)
free(curaddr->addr);
free(curaddr);
return (-1);
}
} else
curaddr->broadaddr = NULL;

if (dstaddr != NULL && dstaddr_size != 0) {
curaddr->dstaddr = (struct sockaddr *)dup_sockaddr(dstaddr, dstaddr_size);
if (curaddr->dstaddr == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
if (curaddr->broadaddr != NULL)
free(curaddr->broadaddr);
if (curaddr->netmask != NULL)
free(curaddr->netmask);
if (curaddr->addr != NULL)
free(curaddr->addr);
free(curaddr);
return (-1);
}
} else
curaddr->dstaddr = NULL;




for (prevaddr = curdev->addresses; prevaddr != NULL; prevaddr = nextaddr) {
nextaddr = prevaddr->next;
if (nextaddr == NULL) {



break;
}
}

if (prevaddr == NULL) {



curdev->addresses = curaddr;
} else {




prevaddr->next = curaddr;
}

return (0);
}










pcap_if_t *
find_or_add_dev(pcap_if_list_t *devlistp, const char *name, bpf_u_int32 flags,
get_if_flags_func get_flags_func, const char *description, char *errbuf)
{
pcap_if_t *curdev;




curdev = find_dev(devlistp, name);
if (curdev != NULL) {



return (curdev);
}








if ((*get_flags_func)(name, &flags, errbuf) == -1) {



return (NULL);
}




return (add_dev(devlistp, name, flags, description, errbuf));
}





pcap_if_t *
find_dev(pcap_if_list_t *devlistp, const char *name)
{
pcap_if_t *curdev;




for (curdev = devlistp->beginning; curdev != NULL;
curdev = curdev->next) {
if (strcmp(name, curdev->name) == 0) {





return (curdev);
}
}




return (NULL);
}









pcap_if_t *
add_dev(pcap_if_list_t *devlistp, const char *name, bpf_u_int32 flags,
const char *description, char *errbuf)
{
pcap_if_t *curdev, *prevdev, *nextdev;
u_int this_figure_of_merit, nextdev_figure_of_merit;

curdev = malloc(sizeof(pcap_if_t));
if (curdev == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (NULL);
}




curdev->next = NULL;
curdev->name = strdup(name);
if (curdev->name == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
free(curdev);
return (NULL);
}
if (description == NULL) {



curdev->description = NULL;
} else {



curdev->description = strdup(description);
if (curdev->description == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
free(curdev->name);
free(curdev);
return (NULL);
}
}
curdev->addresses = NULL;
curdev->flags = flags;





this_figure_of_merit = get_figure_of_merit(curdev);








prevdev = NULL;
for (;;) {



if (prevdev == NULL) {



nextdev = devlistp->beginning;
} else
nextdev = prevdev->next;




if (nextdev == NULL) {



break;
}







nextdev_figure_of_merit = get_figure_of_merit(nextdev);
if (this_figure_of_merit < nextdev_figure_of_merit) {




break;
}

prevdev = nextdev;
}




curdev->next = nextdev;





if (prevdev == NULL) {




devlistp->beginning = curdev;
} else
prevdev->next = curdev;
return (curdev);
}




void
pcap_freealldevs(pcap_if_t *alldevs)
{
pcap_if_t *curdev, *nextdev;
pcap_addr_t *curaddr, *nextaddr;

for (curdev = alldevs; curdev != NULL; curdev = nextdev) {
nextdev = curdev->next;




for (curaddr = curdev->addresses; curaddr != NULL; curaddr = nextaddr) {
nextaddr = curaddr->next;
if (curaddr->addr)
free(curaddr->addr);
if (curaddr->netmask)
free(curaddr->netmask);
if (curaddr->broadaddr)
free(curaddr->broadaddr);
if (curaddr->dstaddr)
free(curaddr->dstaddr);
free(curaddr);
}




free(curdev->name);




if (curdev->description != NULL)
free(curdev->description);




free(curdev);
}
}












#if !defined(HAVE_PACKET32) && !defined(MSDOS)





char *
pcap_lookupdev(char *errbuf)
{
pcap_if_t *alldevs;
#if defined(_WIN32)




#define IF_NAMESIZE 8192
#else






#if !defined(IF_NAMESIZE)
#define IF_NAMESIZE IFNAMSIZ
#endif
#endif
static char device[IF_NAMESIZE + 1];
char *ret;

if (pcap_findalldevs(&alldevs, errbuf) == -1)
return (NULL);

if (alldevs == NULL || (alldevs->flags & PCAP_IF_LOOPBACK)) {











(void)pcap_strlcpy(errbuf, "no suitable device found",
PCAP_ERRBUF_SIZE);
ret = NULL;
} else {



(void)pcap_strlcpy(device, alldevs->name, sizeof(device));
ret = device;
}

pcap_freealldevs(alldevs);
return (ret);
}
#endif

#if !defined(_WIN32) && !defined(MSDOS)








int
pcap_lookupnet(const char *device, bpf_u_int32 *netp, bpf_u_int32 *maskp,
char *errbuf)
{
register int fd;
register struct sockaddr_in *sin4;
struct ifreq ifr;






if (!device || strcmp(device, "any") == 0
#if defined(HAVE_DAG_API)
|| strstr(device, "dag") != NULL
#endif
#if defined(HAVE_SEPTEL_API)
|| strstr(device, "septel") != NULL
#endif
#if defined(PCAP_SUPPORT_BT)
|| strstr(device, "bluetooth") != NULL
#endif
#if defined(PCAP_SUPPORT_USB)
|| strstr(device, "usbmon") != NULL
#endif
#if defined(HAVE_SNF_API)
|| strstr(device, "snf") != NULL
#endif
#if defined(PCAP_SUPPORT_NETMAP)
|| strncmp(device, "netmap:", 7) == 0
|| strncmp(device, "vale", 4) == 0
#endif
) {
*netp = *maskp = 0;
return 0;
}

fd = socket(AF_INET, SOCK_DGRAM, 0);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
return (-1);
}
memset(&ifr, 0, sizeof(ifr));
#if defined(linux)

ifr.ifr_addr.sa_family = AF_INET;
#endif
(void)pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
if (ioctl(fd, SIOCGIFADDR, (char *)&ifr) < 0) {
if (errno == EADDRNOTAVAIL) {
(void)pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"%s: no IPv4 address assigned", device);
} else {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFADDR: %s", device);
}
(void)close(fd);
return (-1);
}
sin4 = (struct sockaddr_in *)&ifr.ifr_addr;
*netp = sin4->sin_addr.s_addr;
memset(&ifr, 0, sizeof(ifr));
#if defined(linux)

ifr.ifr_addr.sa_family = AF_INET;
#endif
(void)pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
if (ioctl(fd, SIOCGIFNETMASK, (char *)&ifr) < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFNETMASK: %s", device);
(void)close(fd);
return (-1);
}
(void)close(fd);
*maskp = sin4->sin_addr.s_addr;
if (*maskp == 0) {
if (IN_CLASSA(*netp))
*maskp = IN_CLASSA_NET;
else if (IN_CLASSB(*netp))
*maskp = IN_CLASSB_NET;
else if (IN_CLASSC(*netp))
*maskp = IN_CLASSC_NET;
else {
(void)pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"inet class for 0x%x unknown", *netp);
return (-1);
}
}
*netp &= *maskp;
return (0);
}
#endif

#if defined(ENABLE_REMOTE)
#include "pcap-rpcap.h"




static char *
get_substring(const char *p, size_t len, char *ebuf)
{
char *token;

token = malloc(len + 1);
if (token == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (NULL);
}
memcpy(token, p, len);
token[len] = '\0';
return (token);
}




































static int
pcap_parse_source(const char *source, char **schemep, char **userinfop,
char **hostp, char **portp, char **pathp, char *ebuf)
{
char *colonp;
size_t scheme_len;
char *scheme;
const char *endp;
size_t authority_len;
char *authority;
char *parsep, *atsignp, *bracketp;
char *userinfo, *host, *port, *path;




*schemep = NULL;
*userinfop = NULL;
*hostp = NULL;
*portp = NULL;
*pathp = NULL;





















colonp = strchr(source, ':');
if (colonp == NULL) {





*pathp = strdup(source);
if (*pathp == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (-1);
}
return (0);
}











if (strncmp(colonp + 1, "//", 2) != 0) {





*pathp = strdup(source);
if (*pathp == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (-1);
}
return (0);
}









scheme_len = colonp - source;
scheme = malloc(scheme_len + 1);
if (scheme == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (-1);
}
memcpy(scheme, source, scheme_len);
scheme[scheme_len] = '\0';





if (pcap_strcasecmp(scheme, "file") == 0) {
*pathp = strdup(colonp + 3);
if (*pathp == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
free(scheme);
return (-1);
}
*schemep = scheme;
return (0);
}









if (pcap_strcasecmp(scheme, "rpcap") == 0 &&
strchr(colonp + 3, '/') == NULL) {






free(scheme);
*pathp = strdup(colonp + 3);
if (*pathp == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (-1);
}
return (0);
}






authority_len = strcspn(colonp + 3, "/");
authority = get_substring(colonp + 3, authority_len, ebuf);
if (authority == NULL) {



free(scheme);
return (-1);
}
endp = colonp + 3 + authority_len;




parsep = authority;




atsignp = strchr(parsep, '@');
if (atsignp != NULL) {



size_t userinfo_len;

userinfo_len = atsignp - parsep;
userinfo = get_substring(parsep, userinfo_len, ebuf);
if (userinfo == NULL) {



free(authority);
free(scheme);
return (-1);
}
parsep = atsignp + 1;
} else {



userinfo = NULL;
}




if (*parsep == '\0') {



host = NULL;
port = NULL;
} else {



size_t host_len;




if (*parsep == '[') {












bracketp = strchr(parsep, ']');
if (bracketp == NULL) {



pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"IP-literal in URL doesn't end with ]");
free(userinfo);
free(authority);
free(scheme);
return (-1);
}
if (*(bracketp + 1) != '\0' &&
*(bracketp + 1) != ':') {




pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"Extra text after IP-literal in URL");
free(userinfo);
free(authority);
free(scheme);
return (-1);
}
host_len = (bracketp - 1) - parsep;
host = get_substring(parsep + 1, host_len, ebuf);
if (host == NULL) {



free(userinfo);
free(authority);
free(scheme);
return (-1);
}
parsep = bracketp + 1;
} else {





host_len = strcspn(parsep, ":");
host = get_substring(parsep, host_len, ebuf);
if (host == NULL) {



free(userinfo);
free(authority);
free(scheme);
return (-1);
}
parsep = parsep + host_len;
}




if (*parsep == ':') {



size_t port_len;

parsep++;
port_len = strlen(parsep);
port = get_substring(parsep, port_len, ebuf);
if (port == NULL) {



free(host);
free(userinfo);
free(authority);
free(scheme);
return (-1);
}
} else {



port = NULL;
}
}
free(authority);




if (*endp == '\0')
path = strdup("");
else
path = strdup(endp + 1);
if (path == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
free(port);
free(host);
free(userinfo);
free(scheme);
return (-1);
}
*schemep = scheme;
*userinfop = userinfo;
*hostp = host;
*portp = port;
*pathp = path;
return (0);
}

int
pcap_createsrcstr(char *source, int type, const char *host, const char *port,
const char *name, char *errbuf)
{
switch (type) {

case PCAP_SRC_FILE:
pcap_strlcpy(source, PCAP_SRC_FILE_STRING, PCAP_BUF_SIZE);
if (name != NULL && *name != '\0') {
pcap_strlcat(source, name, PCAP_BUF_SIZE);
return (0);
} else {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"The file name cannot be NULL.");
return (-1);
}

case PCAP_SRC_IFREMOTE:
pcap_strlcpy(source, PCAP_SRC_IF_STRING, PCAP_BUF_SIZE);
if (host != NULL && *host != '\0') {
if (strchr(host, ':') != NULL) {





pcap_strlcat(source, "[", PCAP_BUF_SIZE);
pcap_strlcat(source, host, PCAP_BUF_SIZE);
pcap_strlcat(source, "]", PCAP_BUF_SIZE);
} else
pcap_strlcat(source, host, PCAP_BUF_SIZE);

if (port != NULL && *port != '\0') {
pcap_strlcat(source, ":", PCAP_BUF_SIZE);
pcap_strlcat(source, port, PCAP_BUF_SIZE);
}

pcap_strlcat(source, "/", PCAP_BUF_SIZE);
} else {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"The host name cannot be NULL.");
return (-1);
}

if (name != NULL && *name != '\0')
pcap_strlcat(source, name, PCAP_BUF_SIZE);

return (0);

case PCAP_SRC_IFLOCAL:
pcap_strlcpy(source, PCAP_SRC_IF_STRING, PCAP_BUF_SIZE);

if (name != NULL && *name != '\0')
pcap_strlcat(source, name, PCAP_BUF_SIZE);

return (0);

default:
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"The interface type is not valid.");
return (-1);
}
}

int
pcap_parsesrcstr(const char *source, int *type, char *host, char *port,
char *name, char *errbuf)
{
char *scheme, *tmpuserinfo, *tmphost, *tmpport, *tmppath;


if (host)
*host = '\0';
if (port)
*port = '\0';
if (name)
*name = '\0';


if (pcap_parse_source(source, &scheme, &tmpuserinfo, &tmphost,
&tmpport, &tmppath, errbuf) == -1) {



return (-1);
}

if (scheme == NULL) {



if (name && tmppath)
pcap_strlcpy(name, tmppath, PCAP_BUF_SIZE);
if (type)
*type = PCAP_SRC_IFLOCAL;
free(tmppath);
free(tmpport);
free(tmphost);
free(tmpuserinfo);
return (0);
}

if (strcmp(scheme, "rpcap") == 0) {






if (host && tmphost) {
if (tmpuserinfo)
pcap_snprintf(host, PCAP_BUF_SIZE, "%s@%s",
tmpuserinfo, tmphost);
else
pcap_strlcpy(host, tmphost, PCAP_BUF_SIZE);
}
if (port && tmpport)
pcap_strlcpy(port, tmpport, PCAP_BUF_SIZE);
if (name && tmppath)
pcap_strlcpy(name, tmppath, PCAP_BUF_SIZE);
if (type)
*type = PCAP_SRC_IFREMOTE;
free(tmppath);
free(tmpport);
free(tmphost);
free(tmpuserinfo);
free(scheme);
return (0);
}

if (strcmp(scheme, "file") == 0) {



if (name && tmppath)
pcap_strlcpy(name, tmppath, PCAP_BUF_SIZE);
if (type)
*type = PCAP_SRC_FILE;
free(tmppath);
free(tmpport);
free(tmphost);
free(tmpuserinfo);
free(scheme);
return (0);
}





if (name)
pcap_strlcpy(name, source, PCAP_BUF_SIZE);
if (type)
*type = PCAP_SRC_IFLOCAL;
free(tmppath);
free(tmpport);
free(tmphost);
free(tmpuserinfo);
free(scheme);
return (0);
}
#endif

pcap_t *
pcap_create(const char *device, char *errbuf)
{
size_t i;
int is_theirs;
pcap_t *p;
char *device_str;








if (device == NULL)
device_str = strdup("any");
else {
#if defined(_WIN32)
























if (device[0] != '\0' && device[1] == '\0') {
size_t length;

length = wcslen((wchar_t *)device);
device_str = (char *)malloc(length + 1);
if (device_str == NULL) {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"malloc");
return (NULL);
}

pcap_snprintf(device_str, length + 1, "%ws",
(const wchar_t *)device);
} else
#endif
device_str = strdup(device);
}
if (device_str == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (NULL);
}






for (i = 0; capture_source_types[i].create_op != NULL; i++) {
is_theirs = 0;
p = capture_source_types[i].create_op(device_str, errbuf,
&is_theirs);
if (is_theirs) {









if (p == NULL) {



free(device_str);
return (NULL);
}
p->opt.device = device_str;
return (p);
}
}




p = pcap_create_interface(device_str, errbuf);
if (p == NULL) {



free(device_str);
return (NULL);
}
p->opt.device = device_str;
return (p);
}






static int
pcap_setnonblock_unactivated(pcap_t *p, int nonblock)
{
p->opt.nonblock = nonblock;
return (0);
}

static void
initialize_ops(pcap_t *p)
{





p->read_op = pcap_read_not_initialized;
p->inject_op = pcap_inject_not_initialized;
p->setfilter_op = pcap_setfilter_not_initialized;
p->setdirection_op = pcap_setdirection_not_initialized;
p->set_datalink_op = pcap_set_datalink_not_initialized;
p->getnonblock_op = pcap_getnonblock_not_initialized;
p->stats_op = pcap_stats_not_initialized;
#if defined(_WIN32)
p->stats_ex_op = pcap_stats_ex_not_initialized;
p->setbuff_op = pcap_setbuff_not_initialized;
p->setmode_op = pcap_setmode_not_initialized;
p->setmintocopy_op = pcap_setmintocopy_not_initialized;
p->getevent_op = pcap_getevent_not_initialized;
p->oid_get_request_op = pcap_oid_get_request_not_initialized;
p->oid_set_request_op = pcap_oid_set_request_not_initialized;
p->sendqueue_transmit_op = pcap_sendqueue_transmit_not_initialized;
p->setuserbuffer_op = pcap_setuserbuffer_not_initialized;
p->live_dump_op = pcap_live_dump_not_initialized;
p->live_dump_ended_op = pcap_live_dump_ended_not_initialized;
p->get_airpcap_handle_op = pcap_get_airpcap_handle_not_initialized;
#endif






p->cleanup_op = pcap_cleanup_live_common;





p->oneshot_callback = pcap_oneshot;
}

static pcap_t *
pcap_alloc_pcap_t(char *ebuf, size_t size)
{
char *chunk;
pcap_t *p;















#define PCAP_T_ALIGNED_SIZE ((sizeof(pcap_t) + 7U) & ~0x7U)
chunk = malloc(PCAP_T_ALIGNED_SIZE + size);
if (chunk == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (NULL);
}
memset(chunk, 0, PCAP_T_ALIGNED_SIZE + size);




p = (pcap_t *)chunk;

#if defined(_WIN32)
p->handle = INVALID_HANDLE_VALUE;
#else
p->fd = -1;
#if !defined(MSDOS)
p->selectable_fd = -1;
p->required_select_timeout = NULL;
#endif
#endif

if (size == 0) {

p->priv = NULL;
} else {




p->priv = (void *)(chunk + PCAP_T_ALIGNED_SIZE);
}

return (p);
}

pcap_t *
pcap_create_common(char *ebuf, size_t size)
{
pcap_t *p;

p = pcap_alloc_pcap_t(ebuf, size);
if (p == NULL)
return (NULL);







p->can_set_rfmon_op = pcap_cant_set_rfmon;






p->setnonblock_op = pcap_setnonblock_unactivated;

initialize_ops(p);


p->snapshot = 0;
p->opt.timeout = 0;
p->opt.buffer_size = 0;
p->opt.promisc = 0;
p->opt.rfmon = 0;
p->opt.immediate = 0;
p->opt.tstamp_type = -1;
p->opt.tstamp_precision = PCAP_TSTAMP_PRECISION_MICRO;



#if defined(__linux__)
p->opt.protocol = 0;
#endif
#if defined(_WIN32)
p->opt.nocapture_local = 0;
#endif




p->bpf_codegen_flags = 0;

return (p);
}

int
pcap_check_activated(pcap_t *p)
{
if (p->activated) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "can't perform "
" operation on activated capture");
return (-1);
}
return (0);
}

int
pcap_set_snaplen(pcap_t *p, int snaplen)
{
if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
p->snapshot = snaplen;
return (0);
}

int
pcap_set_promisc(pcap_t *p, int promisc)
{
if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
p->opt.promisc = promisc;
return (0);
}

int
pcap_set_rfmon(pcap_t *p, int rfmon)
{
if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
p->opt.rfmon = rfmon;
return (0);
}

int
pcap_set_timeout(pcap_t *p, int timeout_ms)
{
if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
p->opt.timeout = timeout_ms;
return (0);
}

int
pcap_set_tstamp_type(pcap_t *p, int tstamp_type)
{
int i;

if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);





if (tstamp_type < 0)
return (PCAP_WARNING_TSTAMP_TYPE_NOTSUP);





if (p->tstamp_type_count == 0) {
if (tstamp_type == PCAP_TSTAMP_HOST) {
p->opt.tstamp_type = tstamp_type;
return (0);
}
} else {



for (i = 0; i < p->tstamp_type_count; i++) {
if (p->tstamp_type_list[i] == (u_int)tstamp_type) {



p->opt.tstamp_type = tstamp_type;
return (0);
}
}
}




return (PCAP_WARNING_TSTAMP_TYPE_NOTSUP);
}

int
pcap_set_immediate_mode(pcap_t *p, int immediate)
{
if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
p->opt.immediate = immediate;
return (0);
}

int
pcap_set_buffer_size(pcap_t *p, int buffer_size)
{
if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
if (buffer_size <= 0) {



return (0);
}
p->opt.buffer_size = buffer_size;
return (0);
}

int
pcap_set_tstamp_precision(pcap_t *p, int tstamp_precision)
{
int i;

if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);





if (tstamp_precision < 0)
return (PCAP_ERROR_TSTAMP_PRECISION_NOTSUP);








if (p->tstamp_precision_count == 0) {
if (tstamp_precision == PCAP_TSTAMP_PRECISION_MICRO) {
p->opt.tstamp_precision = tstamp_precision;
return (0);
}
} else {




for (i = 0; i < p->tstamp_precision_count; i++) {
if (p->tstamp_precision_list[i] == (u_int)tstamp_precision) {



p->opt.tstamp_precision = tstamp_precision;
return (0);
}
}
}




return (PCAP_ERROR_TSTAMP_PRECISION_NOTSUP);
}

int
pcap_get_tstamp_precision(pcap_t *p)
{
return (p->opt.tstamp_precision);
}

int
pcap_activate(pcap_t *p)
{
int status;








if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
status = p->activate_op(p);
if (status >= 0) {




if (p->opt.nonblock) {
status = p->setnonblock_op(p, 1);
if (status < 0) {




p->cleanup_op(p);
initialize_ops(p);
return (status);
}
}
p->activated = 1;
} else {
if (p->errbuf[0] == '\0') {






pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "%s",
pcap_statustostr(status));
}





initialize_ops(p);
}
return (status);
}

pcap_t *
pcap_open_live(const char *device, int snaplen, int promisc, int to_ms, char *errbuf)
{
pcap_t *p;
int status;
#if defined(ENABLE_REMOTE)
char host[PCAP_BUF_SIZE + 1];
char port[PCAP_BUF_SIZE + 1];
char name[PCAP_BUF_SIZE + 1];
int srctype;








if (device == NULL)
device = "any";










if (pcap_parsesrcstr(device, &srctype, host, port, name, errbuf))
return (NULL);

if (srctype == PCAP_SRC_IFREMOTE) {






return (pcap_open_rpcap(device, snaplen,
promisc ? PCAP_OPENFLAG_PROMISCUOUS : 0, to_ms,
NULL, errbuf));
}
if (srctype == PCAP_SRC_FILE) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "unknown URL scheme \"file\"");
return (NULL);
}
if (srctype == PCAP_SRC_IFLOCAL) {





if (strncmp(device, PCAP_SRC_IF_STRING, strlen(PCAP_SRC_IF_STRING)) == 0) {
size_t len = strlen(device) - strlen(PCAP_SRC_IF_STRING) + 1;

if (len > 0)
device += strlen(PCAP_SRC_IF_STRING);
}
}
#endif

p = pcap_create(device, errbuf);
if (p == NULL)
return (NULL);
status = pcap_set_snaplen(p, snaplen);
if (status < 0)
goto fail;
status = pcap_set_promisc(p, promisc);
if (status < 0)
goto fail;
status = pcap_set_timeout(p, to_ms);
if (status < 0)
goto fail;










p->oldstyle = 1;
status = pcap_activate(p);
if (status < 0)
goto fail;
return (p);
fail:
if (status == PCAP_ERROR)
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "%s: %.*s", device,
PCAP_ERRBUF_SIZE - 3, p->errbuf);
else if (status == PCAP_ERROR_NO_SUCH_DEVICE ||
status == PCAP_ERROR_PERM_DENIED ||
status == PCAP_ERROR_PROMISC_PERM_DENIED)
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "%s: %s (%.*s)", device,
pcap_statustostr(status), PCAP_ERRBUF_SIZE - 6, p->errbuf);
else
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "%s: %s", device,
pcap_statustostr(status));
pcap_close(p);
return (NULL);
}

pcap_t *
pcap_open_offline_common(char *ebuf, size_t size)
{
pcap_t *p;

p = pcap_alloc_pcap_t(ebuf, size);
if (p == NULL)
return (NULL);

p->opt.tstamp_precision = PCAP_TSTAMP_PRECISION_MICRO;

return (p);
}

int
pcap_dispatch(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
return (p->read_op(p, cnt, callback, user));
}

int
pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
register int n;

for (;;) {
if (p->rfile != NULL) {



n = pcap_offline_read(p, cnt, callback, user);
} else {




do {
n = p->read_op(p, cnt, callback, user);
} while (n == 0);
}
if (n <= 0)
return (n);
if (!PACKET_COUNT_IS_UNLIMITED(cnt)) {
cnt -= n;
if (cnt <= 0)
return (0);
}
}
}




void
pcap_breakloop(pcap_t *p)
{
p->break_loop = 1;
}

int
pcap_datalink(pcap_t *p)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
return (p->linktype);
}

int
pcap_datalink_ext(pcap_t *p)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
return (p->linktype_ext);
}

int
pcap_list_datalinks(pcap_t *p, int **dlt_buffer)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
if (p->dlt_count == 0) {






*dlt_buffer = (int*)malloc(sizeof(**dlt_buffer));
if (*dlt_buffer == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "malloc");
return (PCAP_ERROR);
}
**dlt_buffer = p->linktype;
return (1);
} else {
*dlt_buffer = (int*)calloc(sizeof(**dlt_buffer), p->dlt_count);
if (*dlt_buffer == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "malloc");
return (PCAP_ERROR);
}
(void)memcpy(*dlt_buffer, p->dlt_list,
sizeof(**dlt_buffer) * p->dlt_count);
return (p->dlt_count);
}
}












void
pcap_free_datalinks(int *dlt_list)
{
free(dlt_list);
}

int
pcap_set_datalink(pcap_t *p, int dlt)
{
int i;
const char *dlt_name;

if (dlt < 0)
goto unsupported;

if (p->dlt_count == 0 || p->set_datalink_op == NULL) {







if (p->linktype != dlt)
goto unsupported;




return (0);
}
for (i = 0; i < p->dlt_count; i++)
if (p->dlt_list[i] == (u_int)dlt)
break;
if (i >= p->dlt_count)
goto unsupported;
if (p->dlt_count == 2 && p->dlt_list[0] == DLT_EN10MB &&
dlt == DLT_DOCSIS) {











p->linktype = dlt;
return (0);
}
if (p->set_datalink_op(p, dlt) == -1)
return (-1);
p->linktype = dlt;
return (0);

unsupported:
dlt_name = pcap_datalink_val_to_name(dlt);
if (dlt_name != NULL) {
(void) pcap_snprintf(p->errbuf, sizeof(p->errbuf),
"%s is not one of the DLTs supported by this device",
dlt_name);
} else {
(void) pcap_snprintf(p->errbuf, sizeof(p->errbuf),
"DLT %d is not one of the DLTs supported by this device",
dlt);
}
return (-1);
}






static const u_char charmap[] = {
(u_char)'\000', (u_char)'\001', (u_char)'\002', (u_char)'\003',
(u_char)'\004', (u_char)'\005', (u_char)'\006', (u_char)'\007',
(u_char)'\010', (u_char)'\011', (u_char)'\012', (u_char)'\013',
(u_char)'\014', (u_char)'\015', (u_char)'\016', (u_char)'\017',
(u_char)'\020', (u_char)'\021', (u_char)'\022', (u_char)'\023',
(u_char)'\024', (u_char)'\025', (u_char)'\026', (u_char)'\027',
(u_char)'\030', (u_char)'\031', (u_char)'\032', (u_char)'\033',
(u_char)'\034', (u_char)'\035', (u_char)'\036', (u_char)'\037',
(u_char)'\040', (u_char)'\041', (u_char)'\042', (u_char)'\043',
(u_char)'\044', (u_char)'\045', (u_char)'\046', (u_char)'\047',
(u_char)'\050', (u_char)'\051', (u_char)'\052', (u_char)'\053',
(u_char)'\054', (u_char)'\055', (u_char)'\056', (u_char)'\057',
(u_char)'\060', (u_char)'\061', (u_char)'\062', (u_char)'\063',
(u_char)'\064', (u_char)'\065', (u_char)'\066', (u_char)'\067',
(u_char)'\070', (u_char)'\071', (u_char)'\072', (u_char)'\073',
(u_char)'\074', (u_char)'\075', (u_char)'\076', (u_char)'\077',
(u_char)'\100', (u_char)'\141', (u_char)'\142', (u_char)'\143',
(u_char)'\144', (u_char)'\145', (u_char)'\146', (u_char)'\147',
(u_char)'\150', (u_char)'\151', (u_char)'\152', (u_char)'\153',
(u_char)'\154', (u_char)'\155', (u_char)'\156', (u_char)'\157',
(u_char)'\160', (u_char)'\161', (u_char)'\162', (u_char)'\163',
(u_char)'\164', (u_char)'\165', (u_char)'\166', (u_char)'\167',
(u_char)'\170', (u_char)'\171', (u_char)'\172', (u_char)'\133',
(u_char)'\134', (u_char)'\135', (u_char)'\136', (u_char)'\137',
(u_char)'\140', (u_char)'\141', (u_char)'\142', (u_char)'\143',
(u_char)'\144', (u_char)'\145', (u_char)'\146', (u_char)'\147',
(u_char)'\150', (u_char)'\151', (u_char)'\152', (u_char)'\153',
(u_char)'\154', (u_char)'\155', (u_char)'\156', (u_char)'\157',
(u_char)'\160', (u_char)'\161', (u_char)'\162', (u_char)'\163',
(u_char)'\164', (u_char)'\165', (u_char)'\166', (u_char)'\167',
(u_char)'\170', (u_char)'\171', (u_char)'\172', (u_char)'\173',
(u_char)'\174', (u_char)'\175', (u_char)'\176', (u_char)'\177',
(u_char)'\200', (u_char)'\201', (u_char)'\202', (u_char)'\203',
(u_char)'\204', (u_char)'\205', (u_char)'\206', (u_char)'\207',
(u_char)'\210', (u_char)'\211', (u_char)'\212', (u_char)'\213',
(u_char)'\214', (u_char)'\215', (u_char)'\216', (u_char)'\217',
(u_char)'\220', (u_char)'\221', (u_char)'\222', (u_char)'\223',
(u_char)'\224', (u_char)'\225', (u_char)'\226', (u_char)'\227',
(u_char)'\230', (u_char)'\231', (u_char)'\232', (u_char)'\233',
(u_char)'\234', (u_char)'\235', (u_char)'\236', (u_char)'\237',
(u_char)'\240', (u_char)'\241', (u_char)'\242', (u_char)'\243',
(u_char)'\244', (u_char)'\245', (u_char)'\246', (u_char)'\247',
(u_char)'\250', (u_char)'\251', (u_char)'\252', (u_char)'\253',
(u_char)'\254', (u_char)'\255', (u_char)'\256', (u_char)'\257',
(u_char)'\260', (u_char)'\261', (u_char)'\262', (u_char)'\263',
(u_char)'\264', (u_char)'\265', (u_char)'\266', (u_char)'\267',
(u_char)'\270', (u_char)'\271', (u_char)'\272', (u_char)'\273',
(u_char)'\274', (u_char)'\275', (u_char)'\276', (u_char)'\277',
(u_char)'\300', (u_char)'\341', (u_char)'\342', (u_char)'\343',
(u_char)'\344', (u_char)'\345', (u_char)'\346', (u_char)'\347',
(u_char)'\350', (u_char)'\351', (u_char)'\352', (u_char)'\353',
(u_char)'\354', (u_char)'\355', (u_char)'\356', (u_char)'\357',
(u_char)'\360', (u_char)'\361', (u_char)'\362', (u_char)'\363',
(u_char)'\364', (u_char)'\365', (u_char)'\366', (u_char)'\367',
(u_char)'\370', (u_char)'\371', (u_char)'\372', (u_char)'\333',
(u_char)'\334', (u_char)'\335', (u_char)'\336', (u_char)'\337',
(u_char)'\340', (u_char)'\341', (u_char)'\342', (u_char)'\343',
(u_char)'\344', (u_char)'\345', (u_char)'\346', (u_char)'\347',
(u_char)'\350', (u_char)'\351', (u_char)'\352', (u_char)'\353',
(u_char)'\354', (u_char)'\355', (u_char)'\356', (u_char)'\357',
(u_char)'\360', (u_char)'\361', (u_char)'\362', (u_char)'\363',
(u_char)'\364', (u_char)'\365', (u_char)'\366', (u_char)'\367',
(u_char)'\370', (u_char)'\371', (u_char)'\372', (u_char)'\373',
(u_char)'\374', (u_char)'\375', (u_char)'\376', (u_char)'\377',
};

int
pcap_strcasecmp(const char *s1, const char *s2)
{
register const u_char *cm = charmap,
*us1 = (const u_char *)s1,
*us2 = (const u_char *)s2;

while (cm[*us1] == cm[*us2++])
if (*us1++ == '\0')
return(0);
return (cm[*us1] - cm[*--us2]);
}

struct dlt_choice {
const char *name;
const char *description;
int dlt;
};

#define DLT_CHOICE(code, description) { #code, description, DLT_ ##code }
#define DLT_CHOICE_SENTINEL { NULL, NULL, 0 }

static struct dlt_choice dlt_choices[] = {
DLT_CHOICE(NULL, "BSD loopback"),
DLT_CHOICE(EN10MB, "Ethernet"),
DLT_CHOICE(IEEE802, "Token ring"),
DLT_CHOICE(ARCNET, "BSD ARCNET"),
DLT_CHOICE(SLIP, "SLIP"),
DLT_CHOICE(PPP, "PPP"),
DLT_CHOICE(FDDI, "FDDI"),
DLT_CHOICE(ATM_RFC1483, "RFC 1483 LLC-encapsulated ATM"),
DLT_CHOICE(RAW, "Raw IP"),
DLT_CHOICE(SLIP_BSDOS, "BSD/OS SLIP"),
DLT_CHOICE(PPP_BSDOS, "BSD/OS PPP"),
DLT_CHOICE(ATM_CLIP, "Linux Classical IP-over-ATM"),
DLT_CHOICE(PPP_SERIAL, "PPP over serial"),
DLT_CHOICE(PPP_ETHER, "PPPoE"),
DLT_CHOICE(SYMANTEC_FIREWALL, "Symantec Firewall"),
DLT_CHOICE(C_HDLC, "Cisco HDLC"),
DLT_CHOICE(IEEE802_11, "802.11"),
DLT_CHOICE(FRELAY, "Frame Relay"),
DLT_CHOICE(LOOP, "OpenBSD loopback"),
DLT_CHOICE(ENC, "OpenBSD encapsulated IP"),
DLT_CHOICE(LINUX_SLL, "Linux cooked v1"),
DLT_CHOICE(LTALK, "Localtalk"),
DLT_CHOICE(PFLOG, "OpenBSD pflog file"),
DLT_CHOICE(PFSYNC, "Packet filter state syncing"),
DLT_CHOICE(PRISM_HEADER, "802.11 plus Prism header"),
DLT_CHOICE(IP_OVER_FC, "RFC 2625 IP-over-Fibre Channel"),
DLT_CHOICE(SUNATM, "Sun raw ATM"),
DLT_CHOICE(IEEE802_11_RADIO, "802.11 plus radiotap header"),
DLT_CHOICE(ARCNET_LINUX, "Linux ARCNET"),
DLT_CHOICE(JUNIPER_MLPPP, "Juniper Multi-Link PPP"),
DLT_CHOICE(JUNIPER_MLFR, "Juniper Multi-Link Frame Relay"),
DLT_CHOICE(JUNIPER_ES, "Juniper Encryption Services PIC"),
DLT_CHOICE(JUNIPER_GGSN, "Juniper GGSN PIC"),
DLT_CHOICE(JUNIPER_MFR, "Juniper FRF.16 Frame Relay"),
DLT_CHOICE(JUNIPER_ATM2, "Juniper ATM2 PIC"),
DLT_CHOICE(JUNIPER_SERVICES, "Juniper Advanced Services PIC"),
DLT_CHOICE(JUNIPER_ATM1, "Juniper ATM1 PIC"),
DLT_CHOICE(APPLE_IP_OVER_IEEE1394, "Apple IP-over-IEEE 1394"),
DLT_CHOICE(MTP2_WITH_PHDR, "SS7 MTP2 with Pseudo-header"),
DLT_CHOICE(MTP2, "SS7 MTP2"),
DLT_CHOICE(MTP3, "SS7 MTP3"),
DLT_CHOICE(SCCP, "SS7 SCCP"),
DLT_CHOICE(DOCSIS, "DOCSIS"),
DLT_CHOICE(LINUX_IRDA, "Linux IrDA"),
DLT_CHOICE(IEEE802_11_RADIO_AVS, "802.11 plus AVS radio information header"),
DLT_CHOICE(JUNIPER_MONITOR, "Juniper Passive Monitor PIC"),
DLT_CHOICE(BACNET_MS_TP, "BACnet MS/TP"),
DLT_CHOICE(PPP_PPPD, "PPP for pppd, with direction flag"),
DLT_CHOICE(JUNIPER_PPPOE, "Juniper PPPoE"),
DLT_CHOICE(JUNIPER_PPPOE_ATM, "Juniper PPPoE/ATM"),
DLT_CHOICE(GPRS_LLC, "GPRS LLC"),
DLT_CHOICE(GPF_T, "GPF-T"),
DLT_CHOICE(GPF_F, "GPF-F"),
DLT_CHOICE(JUNIPER_PIC_PEER, "Juniper PIC Peer"),
DLT_CHOICE(ERF_ETH, "Ethernet with Endace ERF header"),
DLT_CHOICE(ERF_POS, "Packet-over-SONET with Endace ERF header"),
DLT_CHOICE(LINUX_LAPD, "Linux vISDN LAPD"),
DLT_CHOICE(JUNIPER_ETHER, "Juniper Ethernet"),
DLT_CHOICE(JUNIPER_PPP, "Juniper PPP"),
DLT_CHOICE(JUNIPER_FRELAY, "Juniper Frame Relay"),
DLT_CHOICE(JUNIPER_CHDLC, "Juniper C-HDLC"),
DLT_CHOICE(MFR, "FRF.16 Frame Relay"),
DLT_CHOICE(JUNIPER_VP, "Juniper Voice PIC"),
DLT_CHOICE(A429, "Arinc 429"),
DLT_CHOICE(A653_ICM, "Arinc 653 Interpartition Communication"),
DLT_CHOICE(USB_FREEBSD, "USB with FreeBSD header"),
DLT_CHOICE(BLUETOOTH_HCI_H4, "Bluetooth HCI UART transport layer"),
DLT_CHOICE(IEEE802_16_MAC_CPS, "IEEE 802.16 MAC Common Part Sublayer"),
DLT_CHOICE(USB_LINUX, "USB with Linux header"),
DLT_CHOICE(CAN20B, "Controller Area Network (CAN) v. 2.0B"),
DLT_CHOICE(IEEE802_15_4_LINUX, "IEEE 802.15.4 with Linux padding"),
DLT_CHOICE(PPI, "Per-Packet Information"),
DLT_CHOICE(IEEE802_16_MAC_CPS_RADIO, "IEEE 802.16 MAC Common Part Sublayer plus radiotap header"),
DLT_CHOICE(JUNIPER_ISM, "Juniper Integrated Service Module"),
DLT_CHOICE(IEEE802_15_4, "IEEE 802.15.4 with FCS"),
DLT_CHOICE(SITA, "SITA pseudo-header"),
DLT_CHOICE(ERF, "Endace ERF header"),
DLT_CHOICE(RAIF1, "Ethernet with u10 Networks pseudo-header"),
DLT_CHOICE(IPMB_KONTRON, "IPMB with Kontron pseudo-header"),
DLT_CHOICE(JUNIPER_ST, "Juniper Secure Tunnel"),
DLT_CHOICE(BLUETOOTH_HCI_H4_WITH_PHDR, "Bluetooth HCI UART transport layer plus pseudo-header"),
DLT_CHOICE(AX25_KISS, "AX.25 with KISS header"),
DLT_CHOICE(IPMB_LINUX, "IPMB with Linux/Pigeon Point pseudo-header"),
DLT_CHOICE(IEEE802_15_4_NONASK_PHY, "IEEE 802.15.4 with non-ASK PHY data"),
DLT_CHOICE(MPLS, "MPLS with label as link-layer header"),
DLT_CHOICE(LINUX_EVDEV, "Linux evdev events"),
DLT_CHOICE(USB_LINUX_MMAPPED, "USB with padded Linux header"),
DLT_CHOICE(DECT, "DECT"),
DLT_CHOICE(AOS, "AOS Space Data Link protocol"),
DLT_CHOICE(WIHART, "Wireless HART"),
DLT_CHOICE(FC_2, "Fibre Channel FC-2"),
DLT_CHOICE(FC_2_WITH_FRAME_DELIMS, "Fibre Channel FC-2 with frame delimiters"),
DLT_CHOICE(IPNET, "Solaris ipnet"),
DLT_CHOICE(CAN_SOCKETCAN, "CAN-bus with SocketCAN headers"),
DLT_CHOICE(IPV4, "Raw IPv4"),
DLT_CHOICE(IPV6, "Raw IPv6"),
DLT_CHOICE(IEEE802_15_4_NOFCS, "IEEE 802.15.4 without FCS"),
DLT_CHOICE(DBUS, "D-Bus"),
DLT_CHOICE(JUNIPER_VS, "Juniper Virtual Server"),
DLT_CHOICE(JUNIPER_SRX_E2E, "Juniper SRX E2E"),
DLT_CHOICE(JUNIPER_FIBRECHANNEL, "Juniper Fibre Channel"),
DLT_CHOICE(DVB_CI, "DVB-CI"),
DLT_CHOICE(MUX27010, "MUX27010"),
DLT_CHOICE(STANAG_5066_D_PDU, "STANAG 5066 D_PDUs"),
DLT_CHOICE(JUNIPER_ATM_CEMIC, "Juniper ATM CEMIC"),
DLT_CHOICE(NFLOG, "Linux netfilter log messages"),
DLT_CHOICE(NETANALYZER, "Ethernet with Hilscher netANALYZER pseudo-header"),
DLT_CHOICE(NETANALYZER_TRANSPARENT, "Ethernet with Hilscher netANALYZER pseudo-header and with preamble and SFD"),
DLT_CHOICE(IPOIB, "RFC 4391 IP-over-Infiniband"),
DLT_CHOICE(MPEG_2_TS, "MPEG-2 transport stream"),
DLT_CHOICE(NG40, "ng40 protocol tester Iub/Iur"),
DLT_CHOICE(NFC_LLCP, "NFC LLCP PDUs with pseudo-header"),
DLT_CHOICE(INFINIBAND, "InfiniBand"),
DLT_CHOICE(SCTP, "SCTP"),
DLT_CHOICE(USBPCAP, "USB with USBPcap header"),
DLT_CHOICE(RTAC_SERIAL, "Schweitzer Engineering Laboratories RTAC packets"),
DLT_CHOICE(BLUETOOTH_LE_LL, "Bluetooth Low Energy air interface"),
DLT_CHOICE(NETLINK, "Linux netlink"),
DLT_CHOICE(BLUETOOTH_LINUX_MONITOR, "Bluetooth Linux Monitor"),
DLT_CHOICE(BLUETOOTH_BREDR_BB, "Bluetooth Basic Rate/Enhanced Data Rate baseband packets"),
DLT_CHOICE(BLUETOOTH_LE_LL_WITH_PHDR, "Bluetooth Low Energy air interface with pseudo-header"),
DLT_CHOICE(PROFIBUS_DL, "PROFIBUS data link layer"),
DLT_CHOICE(PKTAP, "Apple DLT_PKTAP"),
DLT_CHOICE(EPON, "Ethernet with 802.3 Clause 65 EPON preamble"),
DLT_CHOICE(IPMI_HPM_2, "IPMI trace packets"),
DLT_CHOICE(ZWAVE_R1_R2, "Z-Wave RF profile R1 and R2 packets"),
DLT_CHOICE(ZWAVE_R3, "Z-Wave RF profile R3 packets"),
DLT_CHOICE(WATTSTOPPER_DLM, "WattStopper Digital Lighting Management (DLM) and Legrand Nitoo Open protocol"),
DLT_CHOICE(ISO_14443, "ISO 14443 messages"),
DLT_CHOICE(RDS, "IEC 62106 Radio Data System groups"),
DLT_CHOICE(USB_DARWIN, "USB with Darwin header"),
DLT_CHOICE(OPENFLOW, "OpenBSD DLT_OPENFLOW"),
DLT_CHOICE(SDLC, "IBM SDLC frames"),
DLT_CHOICE(TI_LLN_SNIFFER, "TI LLN sniffer frames"),
DLT_CHOICE(VSOCK, "Linux vsock"),
DLT_CHOICE(NORDIC_BLE, "Nordic Semiconductor Bluetooth LE sniffer frames"),
DLT_CHOICE(DOCSIS31_XRA31, "Excentis XRA-31 DOCSIS 3.1 RF sniffer frames"),
DLT_CHOICE(ETHERNET_MPACKET, "802.3br mPackets"),
DLT_CHOICE(DISPLAYPORT_AUX, "DisplayPort AUX channel monitoring data"),
DLT_CHOICE(LINUX_SLL2, "Linux cooked v2"),
DLT_CHOICE_SENTINEL
};

int
pcap_datalink_name_to_val(const char *name)
{
int i;

for (i = 0; dlt_choices[i].name != NULL; i++) {
if (pcap_strcasecmp(dlt_choices[i].name, name) == 0)
return (dlt_choices[i].dlt);
}
return (-1);
}

const char *
pcap_datalink_val_to_name(int dlt)
{
int i;

for (i = 0; dlt_choices[i].name != NULL; i++) {
if (dlt_choices[i].dlt == dlt)
return (dlt_choices[i].name);
}
return (NULL);
}

const char *
pcap_datalink_val_to_description(int dlt)
{
int i;

for (i = 0; dlt_choices[i].name != NULL; i++) {
if (dlt_choices[i].dlt == dlt)
return (dlt_choices[i].description);
}
return (NULL);
}

const char *
pcap_datalink_val_to_description_or_dlt(int dlt)
{
static char unkbuf[40];
const char *description;

description = pcap_datalink_val_to_description(dlt);
if (description != NULL) {
return description;
} else {
(void)pcap_snprintf(unkbuf, sizeof(unkbuf), "DLT %u", dlt);
return unkbuf;
}
}

struct tstamp_type_choice {
const char *name;
const char *description;
int type;
};

static struct tstamp_type_choice tstamp_type_choices[] = {
{ "host", "Host", PCAP_TSTAMP_HOST },
{ "host_lowprec", "Host, low precision", PCAP_TSTAMP_HOST_LOWPREC },
{ "host_hiprec", "Host, high precision", PCAP_TSTAMP_HOST_HIPREC },
{ "adapter", "Adapter", PCAP_TSTAMP_ADAPTER },
{ "adapter_unsynced", "Adapter, not synced with system time", PCAP_TSTAMP_ADAPTER_UNSYNCED },
{ NULL, NULL, 0 }
};

int
pcap_tstamp_type_name_to_val(const char *name)
{
int i;

for (i = 0; tstamp_type_choices[i].name != NULL; i++) {
if (pcap_strcasecmp(tstamp_type_choices[i].name, name) == 0)
return (tstamp_type_choices[i].type);
}
return (PCAP_ERROR);
}

const char *
pcap_tstamp_type_val_to_name(int tstamp_type)
{
int i;

for (i = 0; tstamp_type_choices[i].name != NULL; i++) {
if (tstamp_type_choices[i].type == tstamp_type)
return (tstamp_type_choices[i].name);
}
return (NULL);
}

const char *
pcap_tstamp_type_val_to_description(int tstamp_type)
{
int i;

for (i = 0; tstamp_type_choices[i].name != NULL; i++) {
if (tstamp_type_choices[i].type == tstamp_type)
return (tstamp_type_choices[i].description);
}
return (NULL);
}

int
pcap_snapshot(pcap_t *p)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
return (p->snapshot);
}

int
pcap_is_swapped(pcap_t *p)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
return (p->swapped);
}

int
pcap_major_version(pcap_t *p)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
return (p->version_major);
}

int
pcap_minor_version(pcap_t *p)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
return (p->version_minor);
}

int
pcap_bufsize(pcap_t *p)
{
if (!p->activated)
return (PCAP_ERROR_NOT_ACTIVATED);
return (p->bufsize);
}

FILE *
pcap_file(pcap_t *p)
{
return (p->rfile);
}

int
pcap_fileno(pcap_t *p)
{
#if !defined(_WIN32)
return (p->fd);
#else
if (p->handle != INVALID_HANDLE_VALUE)
return ((int)(DWORD)p->handle);
else
return (PCAP_ERROR);
#endif
}

#if !defined(_WIN32) && !defined(MSDOS)
int
pcap_get_selectable_fd(pcap_t *p)
{
return (p->selectable_fd);
}

struct timeval *
pcap_get_required_select_timeout(pcap_t *p)
{
return (p->required_select_timeout);
}
#endif

void
pcap_perror(pcap_t *p, const char *prefix)
{
fprintf(stderr, "%s: %s\n", prefix, p->errbuf);
}

char *
pcap_geterr(pcap_t *p)
{
return (p->errbuf);
}

int
pcap_getnonblock(pcap_t *p, char *errbuf)
{
int ret;

ret = p->getnonblock_op(p);
if (ret == -1) {









pcap_strlcpy(errbuf, p->errbuf, PCAP_ERRBUF_SIZE);
}
return (ret);
}





#if !defined(_WIN32) && !defined(MSDOS)
int
pcap_getnonblock_fd(pcap_t *p)
{
int fdflags;

fdflags = fcntl(p->fd, F_GETFL, 0);
if (fdflags == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "F_GETFL");
return (-1);
}
if (fdflags & O_NONBLOCK)
return (1);
else
return (0);
}
#endif

int
pcap_setnonblock(pcap_t *p, int nonblock, char *errbuf)
{
int ret;

ret = p->setnonblock_op(p, nonblock);
if (ret == -1) {









pcap_strlcpy(errbuf, p->errbuf, PCAP_ERRBUF_SIZE);
}
return (ret);
}

#if !defined(_WIN32) && !defined(MSDOS)






int
pcap_setnonblock_fd(pcap_t *p, int nonblock)
{
int fdflags;

fdflags = fcntl(p->fd, F_GETFL, 0);
if (fdflags == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "F_GETFL");
return (-1);
}
if (nonblock)
fdflags |= O_NONBLOCK;
else
fdflags &= ~O_NONBLOCK;
if (fcntl(p->fd, F_SETFL, fdflags) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "F_SETFL");
return (-1);
}
return (0);
}
#endif




const char *
pcap_statustostr(int errnum)
{
static char ebuf[15+10+1];

switch (errnum) {

case PCAP_WARNING:
return("Generic warning");

case PCAP_WARNING_TSTAMP_TYPE_NOTSUP:
return ("That type of time stamp is not supported by that device");

case PCAP_WARNING_PROMISC_NOTSUP:
return ("That device doesn't support promiscuous mode");

case PCAP_ERROR:
return("Generic error");

case PCAP_ERROR_BREAK:
return("Loop terminated by pcap_breakloop");

case PCAP_ERROR_NOT_ACTIVATED:
return("The pcap_t has not been activated");

case PCAP_ERROR_ACTIVATED:
return ("The setting can't be changed after the pcap_t is activated");

case PCAP_ERROR_NO_SUCH_DEVICE:
return ("No such device exists");

case PCAP_ERROR_RFMON_NOTSUP:
return ("That device doesn't support monitor mode");

case PCAP_ERROR_NOT_RFMON:
return ("That operation is supported only in monitor mode");

case PCAP_ERROR_PERM_DENIED:
return ("You don't have permission to capture on that device");

case PCAP_ERROR_IFACE_NOT_UP:
return ("That device is not up");

case PCAP_ERROR_CANTSET_TSTAMP_TYPE:
return ("That device doesn't support setting the time stamp type");

case PCAP_ERROR_PROMISC_PERM_DENIED:
return ("You don't have permission to capture in promiscuous mode on that device");

case PCAP_ERROR_TSTAMP_PRECISION_NOTSUP:
return ("That device doesn't support that time stamp precision");
}
(void)pcap_snprintf(ebuf, sizeof ebuf, "Unknown error: %d", errnum);
return(ebuf);
}




const char *
pcap_strerror(int errnum)
{
#if defined(HAVE_STRERROR)
#if defined(_WIN32)
static char errbuf[PCAP_ERRBUF_SIZE];
errno_t err = strerror_s(errbuf, PCAP_ERRBUF_SIZE, errnum);

if (err != 0)
pcap_strlcpy(errbuf, "strerror_s() error", PCAP_ERRBUF_SIZE);
return (errbuf);
#else
return (strerror(errnum));
#endif
#else
extern int sys_nerr;
extern const char *const sys_errlist[];
static char errbuf[PCAP_ERRBUF_SIZE];

if ((unsigned int)errnum < sys_nerr)
return ((char *)sys_errlist[errnum]);
(void)pcap_snprintf(errbuf, sizeof errbuf, "Unknown error: %d", errnum);
return (errbuf);
#endif
}

int
pcap_setfilter(pcap_t *p, struct bpf_program *fp)
{
return (p->setfilter_op(p, fp));
}







int
pcap_setdirection(pcap_t *p, pcap_direction_t d)
{
if (p->setdirection_op == NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Setting direction is not implemented on this platform");
return (-1);
} else
return (p->setdirection_op(p, d));
}

int
pcap_stats(pcap_t *p, struct pcap_stat *ps)
{
return (p->stats_op(p, ps));
}

#if defined(_WIN32)
struct pcap_stat *
pcap_stats_ex(pcap_t *p, int *pcap_stat_size)
{
return (p->stats_ex_op(p, pcap_stat_size));
}

int
pcap_setbuff(pcap_t *p, int dim)
{
return (p->setbuff_op(p, dim));
}

int
pcap_setmode(pcap_t *p, int mode)
{
return (p->setmode_op(p, mode));
}

int
pcap_setmintocopy(pcap_t *p, int size)
{
return (p->setmintocopy_op(p, size));
}

HANDLE
pcap_getevent(pcap_t *p)
{
return (p->getevent_op(p));
}

int
pcap_oid_get_request(pcap_t *p, bpf_u_int32 oid, void *data, size_t *lenp)
{
return (p->oid_get_request_op(p, oid, data, lenp));
}

int
pcap_oid_set_request(pcap_t *p, bpf_u_int32 oid, const void *data, size_t *lenp)
{
return (p->oid_set_request_op(p, oid, data, lenp));
}

pcap_send_queue *
pcap_sendqueue_alloc(u_int memsize)
{
pcap_send_queue *tqueue;


tqueue = (pcap_send_queue *)malloc(sizeof(pcap_send_queue));
if (tqueue == NULL){
return (NULL);
}


tqueue->buffer = (char *)malloc(memsize);
if (tqueue->buffer == NULL) {
free(tqueue);
return (NULL);
}

tqueue->maxlen = memsize;
tqueue->len = 0;

return (tqueue);
}

void
pcap_sendqueue_destroy(pcap_send_queue *queue)
{
free(queue->buffer);
free(queue);
}

int
pcap_sendqueue_queue(pcap_send_queue *queue, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
if (queue->len + sizeof(struct pcap_pkthdr) + pkt_header->caplen > queue->maxlen){
return (-1);
}


memcpy(queue->buffer + queue->len, pkt_header, sizeof(struct pcap_pkthdr));
queue->len += sizeof(struct pcap_pkthdr);


memcpy(queue->buffer + queue->len, pkt_data, pkt_header->caplen);
queue->len += pkt_header->caplen;

return (0);
}

u_int
pcap_sendqueue_transmit(pcap_t *p, pcap_send_queue *queue, int sync)
{
return (p->sendqueue_transmit_op(p, queue, sync));
}

int
pcap_setuserbuffer(pcap_t *p, int size)
{
return (p->setuserbuffer_op(p, size));
}

int
pcap_live_dump(pcap_t *p, char *filename, int maxsize, int maxpacks)
{
return (p->live_dump_op(p, filename, maxsize, maxpacks));
}

int
pcap_live_dump_ended(pcap_t *p, int sync)
{
return (p->live_dump_ended_op(p, sync));
}

PAirpcapHandle
pcap_get_airpcap_handle(pcap_t *p)
{
PAirpcapHandle handle;

handle = p->get_airpcap_handle_op(p);
if (handle == NULL) {
(void)pcap_snprintf(p->errbuf, sizeof(p->errbuf),
"This isn't an AirPcap device");
}
return (handle);
}
#endif


















static struct pcap *pcaps_to_close;





static int did_atexit;

static void
pcap_close_all(void)
{
struct pcap *handle;

while ((handle = pcaps_to_close) != NULL)
pcap_close(handle);
}

int
pcap_do_addexit(pcap_t *p)
{




if (!did_atexit) {
if (atexit(pcap_close_all) != 0) {



pcap_strlcpy(p->errbuf, "atexit failed", PCAP_ERRBUF_SIZE);
return (0);
}
did_atexit = 1;
}
return (1);
}

void
pcap_add_to_pcaps_to_close(pcap_t *p)
{
p->next = pcaps_to_close;
pcaps_to_close = p;
}

void
pcap_remove_from_pcaps_to_close(pcap_t *p)
{
pcap_t *pc, *prevpc;

for (pc = pcaps_to_close, prevpc = NULL; pc != NULL;
prevpc = pc, pc = pc->next) {
if (pc == p) {



if (prevpc == NULL) {



pcaps_to_close = pc->next;
} else {



prevpc->next = pc->next;
}
break;
}
}
}

void
pcap_cleanup_live_common(pcap_t *p)
{
if (p->buffer != NULL) {
free(p->buffer);
p->buffer = NULL;
}
if (p->dlt_list != NULL) {
free(p->dlt_list);
p->dlt_list = NULL;
p->dlt_count = 0;
}
if (p->tstamp_type_list != NULL) {
free(p->tstamp_type_list);
p->tstamp_type_list = NULL;
p->tstamp_type_count = 0;
}
if (p->tstamp_precision_list != NULL) {
free(p->tstamp_precision_list);
p->tstamp_precision_list = NULL;
p->tstamp_precision_count = 0;
}
pcap_freecode(&p->fcode);
#if !defined(_WIN32) && !defined(MSDOS)
if (p->fd >= 0) {
close(p->fd);
p->fd = -1;
}
p->selectable_fd = -1;
#endif
}







int
pcap_sendpacket(pcap_t *p, const u_char *buf, int size)
{
if (p->inject_op(p, buf, size) == -1)
return (-1);
return (0);
}





int
pcap_inject(pcap_t *p, const void *buf, size_t size)
{
return (p->inject_op(p, buf, size));
}

void
pcap_close(pcap_t *p)
{
if (p->opt.device != NULL)
free(p->opt.device);
p->cleanup_op(p);
free(p);
}







int
pcap_offline_filter(const struct bpf_program *fp, const struct pcap_pkthdr *h,
const u_char *pkt)
{
const struct bpf_insn *fcode = fp->bf_insns;

if (fcode != NULL)
return (bpf_filter(fcode, pkt, h->len, h->caplen));
else
return (0);
}

static int
pcap_can_set_rfmon_dead(pcap_t *p)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Rfmon mode doesn't apply on a pcap_open_dead pcap_t");
return (PCAP_ERROR);
}

static int
pcap_read_dead(pcap_t *p, int cnt _U_, pcap_handler callback _U_,
u_char *user _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Packets aren't available from a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_inject_dead(pcap_t *p, const void *buf _U_, size_t size _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Packets can't be sent on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_setfilter_dead(pcap_t *p, struct bpf_program *fp _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"A filter cannot be set on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_setdirection_dead(pcap_t *p, pcap_direction_t d _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The packet direction cannot be set on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_set_datalink_dead(pcap_t *p, int dlt _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The link-layer header type cannot be set on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_getnonblock_dead(pcap_t *p)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"A pcap_open_dead pcap_t does not have a non-blocking mode setting");
return (-1);
}

static int
pcap_setnonblock_dead(pcap_t *p, int nonblock _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"A pcap_open_dead pcap_t does not have a non-blocking mode setting");
return (-1);
}

static int
pcap_stats_dead(pcap_t *p, struct pcap_stat *ps _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Statistics aren't available from a pcap_open_dead pcap_t");
return (-1);
}

#if defined(_WIN32)
struct pcap_stat *
pcap_stats_ex_dead(pcap_t *p, int *pcap_stat_size _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Statistics aren't available from a pcap_open_dead pcap_t");
return (NULL);
}

static int
pcap_setbuff_dead(pcap_t *p, int dim)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The kernel buffer size cannot be set on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_setmode_dead(pcap_t *p, int mode)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"impossible to set mode on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_setmintocopy_dead(pcap_t *p, int size)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The mintocopy parameter cannot be set on a pcap_open_dead pcap_t");
return (-1);
}

static HANDLE
pcap_getevent_dead(pcap_t *p)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"A pcap_open_dead pcap_t has no event handle");
return (INVALID_HANDLE_VALUE);
}

static int
pcap_oid_get_request_dead(pcap_t *p, bpf_u_int32 oid _U_, void *data _U_,
size_t *lenp _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"An OID get request cannot be performed on a pcap_open_dead pcap_t");
return (PCAP_ERROR);
}

static int
pcap_oid_set_request_dead(pcap_t *p, bpf_u_int32 oid _U_, const void *data _U_,
size_t *lenp _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"An OID set request cannot be performed on a pcap_open_dead pcap_t");
return (PCAP_ERROR);
}

static u_int
pcap_sendqueue_transmit_dead(pcap_t *p, pcap_send_queue *queue, int sync)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Packets cannot be transmitted on a pcap_open_dead pcap_t");
return (0);
}

static int
pcap_setuserbuffer_dead(pcap_t *p, int size)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The user buffer cannot be set on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_live_dump_dead(pcap_t *p, char *filename, int maxsize, int maxpacks)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Live packet dumping cannot be performed on a pcap_open_dead pcap_t");
return (-1);
}

static int
pcap_live_dump_ended_dead(pcap_t *p, int sync)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Live packet dumping cannot be performed on a pcap_open_dead pcap_t");
return (-1);
}

static PAirpcapHandle
pcap_get_airpcap_handle_dead(pcap_t *p)
{
return (NULL);
}
#endif

static void
pcap_cleanup_dead(pcap_t *p _U_)
{

}

pcap_t *
pcap_open_dead_with_tstamp_precision(int linktype, int snaplen, u_int precision)
{
pcap_t *p;

switch (precision) {

case PCAP_TSTAMP_PRECISION_MICRO:
case PCAP_TSTAMP_PRECISION_NANO:
break;

default:






precision = PCAP_TSTAMP_PRECISION_MICRO;
break;
}
p = malloc(sizeof(*p));
if (p == NULL)
return NULL;
memset (p, 0, sizeof(*p));
p->snapshot = snaplen;
p->linktype = linktype;
p->opt.tstamp_precision = precision;
p->can_set_rfmon_op = pcap_can_set_rfmon_dead;
p->read_op = pcap_read_dead;
p->inject_op = pcap_inject_dead;
p->setfilter_op = pcap_setfilter_dead;
p->setdirection_op = pcap_setdirection_dead;
p->set_datalink_op = pcap_set_datalink_dead;
p->getnonblock_op = pcap_getnonblock_dead;
p->setnonblock_op = pcap_setnonblock_dead;
p->stats_op = pcap_stats_dead;
#if defined(_WIN32)
p->stats_ex_op = pcap_stats_ex_dead;
p->setbuff_op = pcap_setbuff_dead;
p->setmode_op = pcap_setmode_dead;
p->setmintocopy_op = pcap_setmintocopy_dead;
p->getevent_op = pcap_getevent_dead;
p->oid_get_request_op = pcap_oid_get_request_dead;
p->oid_set_request_op = pcap_oid_set_request_dead;
p->sendqueue_transmit_op = pcap_sendqueue_transmit_dead;
p->setuserbuffer_op = pcap_setuserbuffer_dead;
p->live_dump_op = pcap_live_dump_dead;
p->live_dump_ended_op = pcap_live_dump_ended_dead;
p->get_airpcap_handle_op = pcap_get_airpcap_handle_dead;
#endif
p->cleanup_op = pcap_cleanup_dead;




p->bpf_codegen_flags = 0;

p->activated = 1;
return (p);
}

pcap_t *
pcap_open_dead(int linktype, int snaplen)
{
return (pcap_open_dead_with_tstamp_precision(linktype, snaplen,
PCAP_TSTAMP_PRECISION_MICRO));
}

#if defined(YYDEBUG)












PCAP_API void pcap_set_parser_debug(int value);

PCAP_API_DEF void
pcap_set_parser_debug(int value)
{
pcap_debug = value;
}
#endif
