




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <net/raw.h>
#include <net/if.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ip_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pcap-int.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif




struct pcap_snoop {
struct pcap_stat stat;
};

static int
pcap_read_snoop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_snoop *psn = p->priv;
int cc;
register struct snoopheader *sh;
register u_int datalen;
register u_int caplen;
register u_char *cp;

again:



if (p->break_loop) {





p->break_loop = 0;
return (-2);
}
cc = read(p->fd, (char *)p->buffer, p->bufsize);
if (cc < 0) {

switch (errno) {

case EINTR:
goto again;

case EWOULDBLOCK:
return (0);
}
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "read");
return (-1);
}
sh = (struct snoopheader *)p->buffer;
datalen = sh->snoop_packetlen;






if (cc == (p->snapshot + sizeof(struct snoopheader)) &&
(datalen < p->snapshot))
datalen += (64 * 1024);

caplen = (datalen < p->snapshot) ? datalen : p->snapshot;
cp = (u_char *)(sh + 1) + p->offset;







if (p->linktype == DLT_NULL && *((short *)(cp + 2)) == 0) {
u_int *uip = (u_int *)cp;
*uip >>= 16;
}

if (p->fcode.bf_insns == NULL ||
bpf_filter(p->fcode.bf_insns, cp, datalen, caplen)) {
struct pcap_pkthdr h;
++psn->stat.ps_recv;
h.ts.tv_sec = sh->snoop_timestamp.tv_sec;
h.ts.tv_usec = sh->snoop_timestamp.tv_usec;
h.len = datalen;
h.caplen = caplen;
(*callback)(user, &h, cp);
return (1);
}
return (0);
}

static int
pcap_inject_snoop(pcap_t *p, const void *buf, size_t size)
{
int ret;





ret = write(p->fd, buf, size);
if (ret == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "send");
return (-1);
}
return (ret);
}

static int
pcap_stats_snoop(pcap_t *p, struct pcap_stat *ps)
{
struct pcap_snoop *psn = p->priv;
register struct rawstats *rs;
struct rawstats rawstats;

rs = &rawstats;
memset(rs, 0, sizeof(*rs));
if (ioctl(p->fd, SIOCRAWSTATS, (char *)rs) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "SIOCRAWSTATS");
return (-1);
}














psn->stat.ps_drop =
rs->rs_snoop.ss_ifdrops + rs->rs_snoop.ss_sbdrops +
rs->rs_drain.ds_ifdrops + rs->rs_drain.ds_sbdrops;






*ps = psn->stat;
return (0);
}


static int
pcap_activate_snoop(pcap_t *p)
{
int fd;
struct sockaddr_raw sr;
struct snoopfilter sf;
u_int v;
int ll_hdrlen;
int snooplen;
struct ifreq ifr;

fd = socket(PF_RAW, SOCK_RAW, RAWPROTO_SNOOP);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "snoop socket");
goto bad;
}
p->fd = fd;
memset(&sr, 0, sizeof(sr));
sr.sr_family = AF_RAW;
(void)strncpy(sr.sr_ifname, p->opt.device, sizeof(sr.sr_ifname));
if (bind(fd, (struct sockaddr *)&sr, sizeof(sr))) {







pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "snoop bind");
goto bad;
}
memset(&sf, 0, sizeof(sf));
if (ioctl(fd, SIOCADDSNOOP, &sf) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCADDSNOOP");
goto bad;
}
if (p->opt.buffer_size != 0)
v = p->opt.buffer_size;
else
v = 64 * 1024;
(void)setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&v, sizeof(v));



if (strncmp("et", p->opt.device, 2) == 0 ||
strncmp("ec", p->opt.device, 2) == 0 ||

strncmp("ef", p->opt.device, 2) == 0 ||
strncmp("eg", p->opt.device, 2) == 0 ||
strncmp("gfe", p->opt.device, 3) == 0 ||
strncmp("fxp", p->opt.device, 3) == 0 ||
strncmp("ep", p->opt.device, 2) == 0 ||
strncmp("vfe", p->opt.device, 3) == 0 ||
strncmp("fa", p->opt.device, 2) == 0 ||
strncmp("qaa", p->opt.device, 3) == 0 ||
strncmp("cip", p->opt.device, 3) == 0 ||
strncmp("el", p->opt.device, 2) == 0) {
p->linktype = DLT_EN10MB;
p->offset = RAW_HDRPAD(sizeof(struct ether_header));
ll_hdrlen = sizeof(struct ether_header);




















p->dlt_list = (u_int *) malloc(sizeof(u_int) * 2);



if (p->dlt_list != NULL) {
p->dlt_list[0] = DLT_EN10MB;
p->dlt_list[1] = DLT_DOCSIS;
p->dlt_count = 2;
}
} else if (strncmp("ipg", p->opt.device, 3) == 0 ||
strncmp("rns", p->opt.device, 3) == 0 ||
strncmp("xpi", p->opt.device, 3) == 0) {
p->linktype = DLT_FDDI;
p->offset = 3;
ll_hdrlen = 13;
} else if (strncmp("ppp", p->opt.device, 3) == 0) {
p->linktype = DLT_RAW;
ll_hdrlen = 0;
} else if (strncmp("qfa", p->opt.device, 3) == 0) {
p->linktype = DLT_IP_OVER_FC;
ll_hdrlen = 24;
} else if (strncmp("pl", p->opt.device, 2) == 0) {
p->linktype = DLT_RAW;
ll_hdrlen = 0;
} else if (strncmp("lo", p->opt.device, 2) == 0) {
p->linktype = DLT_NULL;
ll_hdrlen = 4;
} else {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"snoop: unknown physical layer type");
goto bad;
}

if (p->opt.rfmon) {




return (PCAP_ERROR_RFMON_NOTSUP);
}









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;

#if defined(SIOCGIFMTU)






(void)strncpy(ifr.ifr_name, p->opt.device, sizeof(ifr.ifr_name));
if (ioctl(fd, SIOCGIFMTU, (char *)&ifr) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFMTU");
goto bad;
}














#if !defined(ifr_mtu)
#define ifr_mtu ifr_metric
#endif
if (p->snapshot > ifr.ifr_mtu + ll_hdrlen)
p->snapshot = ifr.ifr_mtu + ll_hdrlen;
#endif






snooplen = p->snapshot - ll_hdrlen;
if (snooplen < 0)
snooplen = 0;
if (ioctl(fd, SIOCSNOOPLEN, &snooplen) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCSNOOPLEN");
goto bad;
}
v = 1;
if (ioctl(fd, SIOCSNOOPING, &v) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCSNOOPING");
goto bad;
}

p->bufsize = 4096;
p->buffer = malloc(p->bufsize);
if (p->buffer == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
goto bad;
}




p->selectable_fd = p->fd;

p->read_op = pcap_read_snoop;
p->inject_op = pcap_inject_snoop;
p->setfilter_op = install_bpf_program;
p->setdirection_op = NULL;
p->set_datalink_op = NULL;
p->getnonblock_op = pcap_getnonblock_fd;
p->setnonblock_op = pcap_setnonblock_fd;
p->stats_op = pcap_stats_snoop;

return (0);
bad:
pcap_cleanup_live_common(p);
return (PCAP_ERROR);
}

pcap_t *
pcap_create_interface(const char *device _U_, char *ebuf)
{
pcap_t *p;

p = pcap_create_common(ebuf, sizeof (struct pcap_snoop));
if (p == NULL)
return (NULL);

p->activate_op = pcap_activate_snoop;
return (p);
}





static int
can_be_bound(const char *name _U_)
{
return (1);
}

static int
get_if_flags(const char *name _U_, bpf_u_int32 *flags _U_, char *errbuf _U_)
{





return (0);
}

int
pcap_platform_finddevs(pcap_if_list_t *devlistp, char *errbuf)
{
return (pcap_findalldevs_interfaces(devlistp, errbuf, can_be_bound,
get_if_flags));
}




const char *
pcap_lib_version(void)
{
return (PCAP_VERSION_STRING);
}
