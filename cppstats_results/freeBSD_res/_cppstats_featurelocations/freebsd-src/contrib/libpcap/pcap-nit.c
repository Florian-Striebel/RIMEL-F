




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/nit.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ip_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#include "pcap-int.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif





#define CHUNKSIZE (2*1024)




#define BUFSPACE (4*CHUNKSIZE)


static int nit_setflags(int, int, int, char *);




struct pcap_nit {
struct pcap_stat stat;
};

static int
pcap_stats_nit(pcap_t *p, struct pcap_stat *ps)
{
struct pcap_nit *pn = p->priv;

















*ps = pn->stat;
return (0);
}

static int
pcap_read_nit(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_nit *pn = p->priv;
register int cc, n;
register u_char *bp, *cp, *ep;
register struct nit_hdr *nh;
register int caplen;

cc = p->cc;
if (cc == 0) {
cc = read(p->fd, (char *)p->buffer, p->bufsize);
if (cc < 0) {
if (errno == EWOULDBLOCK)
return (0);
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "pcap_read");
return (-1);
}
bp = (u_char *)p->buffer;
} else
bp = p->bp;






n = 0;
ep = bp + cc;
while (bp < ep) {









if (p->break_loop) {
if (n == 0) {
p->break_loop = 0;
return (-2);
} else {
p->cc = ep - bp;
p->bp = bp;
return (n);
}
}

nh = (struct nit_hdr *)bp;
cp = bp + sizeof(*nh);

switch (nh->nh_state) {

case NIT_CATCH:
break;

case NIT_NOMBUF:
case NIT_NOCLUSTER:
case NIT_NOSPACE:
pn->stat.ps_drop = nh->nh_dropped;
continue;

case NIT_SEQNO:
continue;

default:
pcap_snprintf(p->errbuf, sizeof(p->errbuf),
"bad nit state %d", nh->nh_state);
return (-1);
}
++pn->stat.ps_recv;
bp += ((sizeof(struct nit_hdr) + nh->nh_datalen +
sizeof(int) - 1) & ~(sizeof(int) - 1));

caplen = nh->nh_wirelen;
if (caplen > p->snapshot)
caplen = p->snapshot;
if (bpf_filter(p->fcode.bf_insns, cp, nh->nh_wirelen, caplen)) {
struct pcap_pkthdr h;
h.ts = nh->nh_timestamp;
h.len = nh->nh_wirelen;
h.caplen = caplen;
(*callback)(user, &h, cp);
if (++n >= cnt && !PACKET_COUNT_IS_UNLIMITED(cnt)) {
p->cc = ep - bp;
p->bp = bp;
return (n);
}
}
}
p->cc = 0;
return (n);
}

static int
pcap_inject_nit(pcap_t *p, const void *buf, size_t size)
{
struct sockaddr sa;
int ret;

memset(&sa, 0, sizeof(sa));
strncpy(sa.sa_data, device, sizeof(sa.sa_data));
ret = sendto(p->fd, buf, size, 0, &sa, sizeof(sa));
if (ret == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "send");
return (-1);
}
return (ret);
}

static int
nit_setflags(pcap_t *p)
{
struct nit_ioc nioc;

memset(&nioc, 0, sizeof(nioc));
nioc.nioc_typetomatch = NT_ALLTYPES;
nioc.nioc_snaplen = p->snapshot;
nioc.nioc_bufalign = sizeof(int);
nioc.nioc_bufoffset = 0;

if (p->opt.buffer_size != 0)
nioc.nioc_bufspace = p->opt.buffer_size;
else {

nioc.nioc_bufspace = BUFSPACE;
}

if (p->opt.immediate) {





nioc.nioc_chunksize = 0;
} else
nioc.nioc_chunksize = CHUNKSIZE;
if (p->opt.timeout != 0) {
nioc.nioc_flags |= NF_TIMEOUT;
nioc.nioc_timeout.tv_sec = p->opt.timeout / 1000;
nioc.nioc_timeout.tv_usec = (p->opt.timeout * 1000) % 1000000;
}
if (p->opt.promisc)
nioc.nioc_flags |= NF_PROMISC;

if (ioctl(p->fd, SIOCSNIT, &nioc) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCSNIT");
return (-1);
}
return (0);
}

static int
pcap_activate_nit(pcap_t *p)
{
int fd;
struct sockaddr_nit snit;

if (p->opt.rfmon) {





return (PCAP_ERROR_RFMON_NOTSUP);
}









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;

if (p->snapshot < 96)



p->snapshot = 96;

memset(p, 0, sizeof(*p));
p->fd = fd = socket(AF_NIT, SOCK_RAW, NITPROTO_RAW);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
goto bad;
}
snit.snit_family = AF_NIT;
(void)strncpy(snit.snit_ifname, p->opt.device, NITIFSIZ);

if (bind(fd, (struct sockaddr *)&snit, sizeof(snit))) {







pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "bind: %s", snit.snit_ifname);
goto bad;
}
if (nit_setflags(p) < 0)
goto bad;




p->linktype = DLT_EN10MB;

p->bufsize = BUFSPACE;
p->buffer = malloc(p->bufsize);
if (p->buffer == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
goto bad;
}




p->selectable_fd = p->fd;











p->dlt_list = (u_int *) malloc(sizeof(u_int) * 2);



if (p->dlt_list != NULL) {
p->dlt_list[0] = DLT_EN10MB;
p->dlt_list[1] = DLT_DOCSIS;
p->dlt_count = 2;
}

p->read_op = pcap_read_nit;
p->inject_op = pcap_inject_nit;
p->setfilter_op = install_bpf_program;
p->setdirection_op = NULL;
p->set_datalink_op = NULL;
p->getnonblock_op = pcap_getnonblock_fd;
p->setnonblock_op = pcap_setnonblock_fd;
p->stats_op = pcap_stats_nit;

return (0);
bad:
pcap_cleanup_live_common(p);
return (PCAP_ERROR);
}

pcap_t *
pcap_create_interface(const char *device _U_, char *ebuf)
{
pcap_t *p;

p = pcap_create_common(ebuf, sizeof (struct pcap_nit));
if (p == NULL)
return (NULL);

p->activate_op = pcap_activate_nit;
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
