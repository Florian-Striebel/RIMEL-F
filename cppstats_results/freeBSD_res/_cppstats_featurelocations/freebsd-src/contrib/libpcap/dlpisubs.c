












#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#if !defined(DL_IPATM)
#define DL_IPATM 0x12
#endif

#if defined(HAVE_SYS_BUFMOD_H)






#define CHUNKSIZE 65536





#define PKTBUFSIZE CHUNKSIZE

#else






#define MAXDLBUF 8192
#define PKTBUFSIZE (MAXDLBUF * sizeof(bpf_u_int32))

#endif

#include <sys/types.h>
#include <sys/time.h>
#if defined(HAVE_SYS_BUFMOD_H)
#include <sys/bufmod.h>
#endif
#include <sys/dlpi.h>
#include <sys/stream.h>

#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>

#if defined(HAVE_LIBDLPI)
#include <libdlpi.h>
#endif

#include "pcap-int.h"
#include "dlpisubs.h"

#if defined(HAVE_SYS_BUFMOD_H)
static void pcap_stream_err(const char *, int, char *);
#endif




int
pcap_stats_dlpi(pcap_t *p, struct pcap_stat *ps)
{
struct pcap_dlpi *pd = p->priv;
























*ps = pd->stat;




ps->ps_recv += ps->ps_drop;
return (0);
}





int
pcap_process_pkts(pcap_t *p, pcap_handler callback, u_char *user,
int count, u_char *bufp, int len)
{
struct pcap_dlpi *pd = p->priv;
int n, caplen, origlen;
u_char *ep, *pk;
struct pcap_pkthdr pkthdr;
#if defined(HAVE_SYS_BUFMOD_H)
struct sb_hdr *sbp;
#if defined(LBL_ALIGN)
struct sb_hdr sbhdr;
#endif
#endif


ep = bufp + len;
n = 0;

#if defined(HAVE_SYS_BUFMOD_H)
while (bufp < ep) {









if (p->break_loop) {
if (n == 0) {
p->break_loop = 0;
return (-2);
} else {
p->bp = bufp;
p->cc = ep - bufp;
return (n);
}
}
#if defined(LBL_ALIGN)
if ((long)bufp & 3) {
sbp = &sbhdr;
memcpy(sbp, bufp, sizeof(*sbp));
} else
#endif
sbp = (struct sb_hdr *)bufp;
pd->stat.ps_drop = sbp->sbh_drops;
pk = bufp + sizeof(*sbp);
bufp += sbp->sbh_totlen;
origlen = sbp->sbh_origlen;
caplen = sbp->sbh_msglen;
#else
origlen = len;
caplen = min(p->snapshot, len);
pk = bufp;
bufp += caplen;
#endif
++pd->stat.ps_recv;
if (bpf_filter(p->fcode.bf_insns, pk, origlen, caplen)) {
#if defined(HAVE_SYS_BUFMOD_H)
pkthdr.ts.tv_sec = sbp->sbh_timestamp.tv_sec;
pkthdr.ts.tv_usec = sbp->sbh_timestamp.tv_usec;
#else
(void) gettimeofday(&pkthdr.ts, NULL);
#endif
pkthdr.len = origlen;
pkthdr.caplen = caplen;

if (pkthdr.caplen > (bpf_u_int32)p->snapshot)
pkthdr.caplen = (bpf_u_int32)p->snapshot;
(*callback)(user, &pkthdr, pk);
if (++n >= count && !PACKET_COUNT_IS_UNLIMITED(count)) {
p->cc = ep - bufp;
p->bp = bufp;
return (n);
}
}
#if defined(HAVE_SYS_BUFMOD_H)
}
#endif
p->cc = 0;
return (n);
}




int
pcap_process_mactype(pcap_t *p, u_int mactype)
{
int retv = 0;

switch (mactype) {

case DL_CSMACD:
case DL_ETHER:
p->linktype = DLT_EN10MB;
p->offset = 2;










p->dlt_list = (u_int *)malloc(sizeof(u_int) * 2);



if (p->dlt_list != NULL) {
p->dlt_list[0] = DLT_EN10MB;
p->dlt_list[1] = DLT_DOCSIS;
p->dlt_count = 2;
}
break;

case DL_FDDI:
p->linktype = DLT_FDDI;
p->offset = 3;
break;

case DL_TPR:

p->linktype = DLT_IEEE802;
p->offset = 2;
break;

#if defined(HAVE_SOLARIS)
case DL_IPATM:
p->linktype = DLT_SUNATM;
p->offset = 0;
break;
#endif

#if defined(DL_IPV4)
case DL_IPV4:
p->linktype = DLT_IPV4;
p->offset = 0;
break;
#endif

#if defined(DL_IPV6)
case DL_IPV6:
p->linktype = DLT_IPV6;
p->offset = 0;
break;
#endif

#if defined(DL_IPNET)
case DL_IPNET:









p->linktype = DLT_RAW;
p->offset = 0;
break;
#endif

default:
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "unknown mactype 0x%x",
mactype);
retv = -1;
}

return (retv);
}

#if defined(HAVE_SYS_BUFMOD_H)



int
pcap_conf_bufmod(pcap_t *p, int snaplen)
{
struct timeval to;
bpf_u_int32 ss, chunksize;


if (ioctl(p->fd, I_PUSH, "bufmod") != 0) {
pcap_stream_err("I_PUSH bufmod", errno, p->errbuf);
return (-1);
}

ss = snaplen;
if (ss > 0 &&
strioctl(p->fd, SBIOCSSNAP, sizeof(ss), (char *)&ss) != 0) {
pcap_stream_err("SBIOCSSNAP", errno, p->errbuf);
return (-1);
}

if (p->opt.immediate) {

to.tv_sec = 0;
to.tv_usec = 0;
if (strioctl(p->fd, SBIOCSTIME, sizeof(to), (char *)&to) != 0) {
pcap_stream_err("SBIOCSTIME", errno, p->errbuf);
return (-1);
}
} else {

if (p->opt.timeout != 0) {
to.tv_sec = p->opt.timeout / 1000;
to.tv_usec = (p->opt.timeout * 1000) % 1000000;
if (strioctl(p->fd, SBIOCSTIME, sizeof(to), (char *)&to) != 0) {
pcap_stream_err("SBIOCSTIME", errno, p->errbuf);
return (-1);
}
}


chunksize = CHUNKSIZE;
if (strioctl(p->fd, SBIOCSCHUNK, sizeof(chunksize), (char *)&chunksize)
!= 0) {
pcap_stream_err("SBIOCSCHUNKP", errno, p->errbuf);
return (-1);
}
}

return (0);
}
#endif




int
pcap_alloc_databuf(pcap_t *p)
{
p->bufsize = PKTBUFSIZE;
p->buffer = malloc(p->bufsize + p->offset);
if (p->buffer == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (-1);
}

return (0);
}





int
strioctl(int fd, int cmd, int len, char *dp)
{
struct strioctl str;
int retv;

str.ic_cmd = cmd;
str.ic_timout = -1;
str.ic_len = len;
str.ic_dp = dp;
if ((retv = ioctl(fd, I_STR, &str)) < 0)
return (retv);

return (str.ic_len);
}

#if defined(HAVE_SYS_BUFMOD_H)



static void
pcap_stream_err(const char *func, int err, char *errbuf)
{
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE, err, "%s", func);
}
#endif
