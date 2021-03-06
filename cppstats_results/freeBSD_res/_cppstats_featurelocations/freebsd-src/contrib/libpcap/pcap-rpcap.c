
































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "ftmacros.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "sockutils.h"
#include "pcap-int.h"
#include "rpcap-protocol.h"
#include "pcap-rpcap.h"



















#define PCAP_STATS_STANDARD 0
#if defined(_WIN32)
#define PCAP_STATS_EX 1
#endif











struct activehosts
{
struct sockaddr_storage host;
SOCKET sockctrl;
uint8 protocol_version;
struct activehosts *next;
};


static struct activehosts *activeHosts;







static SOCKET sockmain;




struct pcap_rpcap {





int rmt_clientside;

SOCKET rmt_sockctrl;
SOCKET rmt_sockdata;
int rmt_flags;
int rmt_capstarted;
char *currentfilter;

uint8 protocol_version;

unsigned int TotNetDrops;

















unsigned int TotCapt;

struct pcap_stat stat;

struct pcap *next;
};






static struct pcap_stat *rpcap_stats_rpcap(pcap_t *p, struct pcap_stat *ps, int mode);
static int pcap_pack_bpffilter(pcap_t *fp, char *sendbuf, int *sendbufidx, struct bpf_program *prog);
static int pcap_createfilter_norpcappkt(pcap_t *fp, struct bpf_program *prog);
static int pcap_updatefilter_remote(pcap_t *fp, struct bpf_program *prog);
static void pcap_save_current_filter_rpcap(pcap_t *fp, const char *filter);
static int pcap_setfilter_rpcap(pcap_t *fp, struct bpf_program *prog);
static int pcap_setsampling_remote(pcap_t *fp);
static int pcap_startcapture_remote(pcap_t *fp);
static int rpcap_recv_msg_header(SOCKET sock, struct rpcap_header *header, char *errbuf);
static int rpcap_check_msg_ver(SOCKET sock, uint8 expected_ver, struct rpcap_header *header, char *errbuf);
static int rpcap_check_msg_type(SOCKET sock, uint8 request_type, struct rpcap_header *header, uint16 *errcode, char *errbuf);
static int rpcap_process_msg_header(SOCKET sock, uint8 ver, uint8 request_type, struct rpcap_header *header, char *errbuf);
static int rpcap_recv(SOCKET sock, void *buffer, size_t toread, uint32 *plen, char *errbuf);
static void rpcap_msg_err(SOCKET sockctrl, uint32 plen, char *remote_errbuf);
static int rpcap_discard(SOCKET sock, uint32 len, char *errbuf);
static int rpcap_read_packet_msg(SOCKET sock, pcap_t *p, size_t size);



























































































#define SOCKADDR_IN_LEN 16
#define SOCKADDR_IN6_LEN 28
#define NEW_BSD_AF_INET_BE ((SOCKADDR_IN_LEN << 8) | 2)
#define NEW_BSD_AF_INET_LE (SOCKADDR_IN_LEN << 8)










#define NEW_BSD_AF_INET6_BSD_BE ((SOCKADDR_IN6_LEN << 8) | 24)
#define NEW_BSD_AF_INET6_FREEBSD_BE ((SOCKADDR_IN6_LEN << 8) | 28)
#define NEW_BSD_AF_INET6_DARWIN_BE ((SOCKADDR_IN6_LEN << 8) | 30)
#define NEW_BSD_AF_INET6_LE (SOCKADDR_IN6_LEN << 8)
#define LINUX_AF_INET6 10
#define HPUX_AF_INET6 22
#define AIX_AF_INET6 24
#define SOLARIS_AF_INET6 26

static int
rpcap_deseraddr(struct rpcap_sockaddr *sockaddrin, struct sockaddr_storage **sockaddrout, char *errbuf)
{

switch (ntohs(sockaddrin->family))
{
case RPCAP_AF_INET:
case NEW_BSD_AF_INET_BE:
case NEW_BSD_AF_INET_LE:
{
struct rpcap_sockaddr_in *sockaddrin_ipv4;
struct sockaddr_in *sockaddrout_ipv4;

(*sockaddrout) = (struct sockaddr_storage *) malloc(sizeof(struct sockaddr_in));
if ((*sockaddrout) == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc() failed");
return -1;
}
sockaddrin_ipv4 = (struct rpcap_sockaddr_in *) sockaddrin;
sockaddrout_ipv4 = (struct sockaddr_in *) (*sockaddrout);
sockaddrout_ipv4->sin_family = AF_INET;
sockaddrout_ipv4->sin_port = ntohs(sockaddrin_ipv4->port);
memcpy(&sockaddrout_ipv4->sin_addr, &sockaddrin_ipv4->addr, sizeof(sockaddrout_ipv4->sin_addr));
memset(sockaddrout_ipv4->sin_zero, 0, sizeof(sockaddrout_ipv4->sin_zero));
break;
}

#if defined(AF_INET6)
case RPCAP_AF_INET6:
case NEW_BSD_AF_INET6_BSD_BE:
case NEW_BSD_AF_INET6_FREEBSD_BE:
case NEW_BSD_AF_INET6_DARWIN_BE:
case NEW_BSD_AF_INET6_LE:
case LINUX_AF_INET6:
case HPUX_AF_INET6:
case AIX_AF_INET6:
case SOLARIS_AF_INET6:
{
struct rpcap_sockaddr_in6 *sockaddrin_ipv6;
struct sockaddr_in6 *sockaddrout_ipv6;

(*sockaddrout) = (struct sockaddr_storage *) malloc(sizeof(struct sockaddr_in6));
if ((*sockaddrout) == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc() failed");
return -1;
}
sockaddrin_ipv6 = (struct rpcap_sockaddr_in6 *) sockaddrin;
sockaddrout_ipv6 = (struct sockaddr_in6 *) (*sockaddrout);
sockaddrout_ipv6->sin6_family = AF_INET6;
sockaddrout_ipv6->sin6_port = ntohs(sockaddrin_ipv6->port);
sockaddrout_ipv6->sin6_flowinfo = ntohl(sockaddrin_ipv6->flowinfo);
memcpy(&sockaddrout_ipv6->sin6_addr, &sockaddrin_ipv6->addr, sizeof(sockaddrout_ipv6->sin6_addr));
sockaddrout_ipv6->sin6_scope_id = ntohl(sockaddrin_ipv6->scope_id);
break;
}
#endif

default:




*sockaddrout = NULL;
break;
}
return 0;
}




















static int pcap_read_nocb_remote(pcap_t *p, struct pcap_pkthdr *pkt_header, u_char **pkt_data)
{
struct pcap_rpcap *pr = p->priv;
struct rpcap_header *header;
struct rpcap_pkthdr *net_pkt_header;
u_char *net_pkt_data;
uint32 plen;
int retval;
int msglen;


struct timeval tv;
fd_set rfds;





tv.tv_sec = p->opt.timeout / 1000;
tv.tv_usec = (p->opt.timeout - tv.tv_sec * 1000) * 1000;


FD_ZERO(&rfds);





FD_SET(pr->rmt_sockdata, &rfds);

retval = select((int) pr->rmt_sockdata + 1, &rfds, NULL, NULL, &tv);
if (retval == -1)
{
#if !defined(_WIN32)
if (errno == EINTR)
{

return 0;
}
#endif
sock_geterror("select()", p->errbuf, PCAP_ERRBUF_SIZE);
return -1;
}


if (retval == 0)
return 0;





header = (struct rpcap_header *) p->buffer;
net_pkt_header = (struct rpcap_pkthdr *) ((char *)p->buffer + sizeof(struct rpcap_header));
net_pkt_data = (u_char *)p->buffer + sizeof(struct rpcap_header) + sizeof(struct rpcap_pkthdr);

if (pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP)
{

msglen = sock_recv_dgram(pr->rmt_sockdata, p->buffer,
p->bufsize, p->errbuf, PCAP_ERRBUF_SIZE);
if (msglen == -1)
{

return -1;
}
if (msglen == -3)
{

return 0;
}
if ((size_t)msglen < sizeof(struct rpcap_header))
{



pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"UDP packet message is shorter than an rpcap header");
return -1;
}
plen = ntohl(header->plen);
if ((size_t)msglen < sizeof(struct rpcap_header) + plen)
{




pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"UDP packet message is shorter than its rpcap header claims");
return -1;
}
}
else
{
int status;

if ((size_t)p->cc < sizeof(struct rpcap_header))
{





status = rpcap_read_packet_msg(pr->rmt_sockdata, p,
sizeof(struct rpcap_header));
if (status == -1)
{

return -1;
}
if (status == -3)
{

return 0;
}
}







plen = ntohl(header->plen);
if (plen > p->bufsize - sizeof(struct rpcap_header))
{






pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Server sent us a message larger than the largest expected packet message");
return -1;
}
status = rpcap_read_packet_msg(pr->rmt_sockdata, p,
sizeof(struct rpcap_header) + plen);
if (status == -1)
{

return -1;
}
if (status == -3)
{

return 0;
}






p->bp = p->buffer;
p->cc = 0;
}




header->plen = plen;




if (rpcap_check_msg_ver(pr->rmt_sockdata, pr->protocol_version,
header, p->errbuf) == -1)
{
return 0;
}




if (header->type != RPCAP_MSG_PACKET)
{
return 0;
}

if (ntohl(net_pkt_header->caplen) > plen)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Packet's captured data goes past the end of the received packet message.");
return -1;
}


pkt_header->caplen = ntohl(net_pkt_header->caplen);
pkt_header->len = ntohl(net_pkt_header->len);
pkt_header->ts.tv_sec = ntohl(net_pkt_header->timestamp_sec);
pkt_header->ts.tv_usec = ntohl(net_pkt_header->timestamp_usec);


*pkt_data = net_pkt_data;





pr->TotCapt++;

if (pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP)
{
unsigned int npkt;


npkt = ntohl(net_pkt_header->npkt);

if (pr->TotCapt != npkt)
{
pr->TotNetDrops += (npkt - pr->TotCapt);
pr->TotCapt = npkt;
}
}


return 1;
}








static int pcap_read_rpcap(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_rpcap *pr = p->priv;
struct pcap_pkthdr pkt_header;
u_char *pkt_data;
int n = 0;
int ret;





if (pr->rmt_clientside)
{

if (!pr->rmt_capstarted)
{




if (pcap_startcapture_remote(p))
return -1;
}
}

while (n < cnt || PACKET_COUNT_IS_UNLIMITED(cnt))
{



if (p->break_loop) {





p->break_loop = 0;
return (PCAP_ERROR_BREAK);
}




ret = pcap_read_nocb_remote(p, &pkt_header, &pkt_data);
if (ret == 1)
{




(*callback)(user, &pkt_header, pkt_data);
n++;
}
else if (ret == -1)
{

return ret;
}
else
{







if (p->break_loop) {



p->break_loop = 0;
return (PCAP_ERROR_BREAK);
}

return n;
}
}
return n;
}









static void pcap_cleanup_rpcap(pcap_t *fp)
{
struct pcap_rpcap *pr = fp->priv;
struct rpcap_header header;
struct activehosts *temp;
int active = 0;


temp = activeHosts;
while (temp)
{
if (temp->sockctrl == pr->rmt_sockctrl)
{
active = 1;
break;
}
temp = temp->next;
}

if (!active)
{
rpcap_createhdr(&header, pr->protocol_version,
RPCAP_MSG_CLOSE, 0, 0);






(void)sock_send(pr->rmt_sockctrl, (char *)&header,
sizeof(struct rpcap_header), NULL, 0);
}
else
{
rpcap_createhdr(&header, pr->protocol_version,
RPCAP_MSG_ENDCAP_REQ, 0, 0);






if (sock_send(pr->rmt_sockctrl, (char *)&header,
sizeof(struct rpcap_header), NULL, 0) == 0)
{





if (rpcap_process_msg_header(pr->rmt_sockctrl,
pr->protocol_version, RPCAP_MSG_ENDCAP_REQ,
&header, NULL) == 0)
{
(void)rpcap_discard(pr->rmt_sockctrl,
header.plen, NULL);
}
}
}

if (pr->rmt_sockdata)
{
sock_close(pr->rmt_sockdata, NULL, 0);
pr->rmt_sockdata = 0;
}

if ((!active) && (pr->rmt_sockctrl))
sock_close(pr->rmt_sockctrl, NULL, 0);

pr->rmt_sockctrl = 0;

if (pr->currentfilter)
{
free(pr->currentfilter);
pr->currentfilter = NULL;
}

pcap_cleanup_live_common(fp);


sock_cleanup();
}





static int pcap_stats_rpcap(pcap_t *p, struct pcap_stat *ps)
{
struct pcap_stat *retval;

retval = rpcap_stats_rpcap(p, ps, PCAP_STATS_STANDARD);

if (retval)
return 0;
else
return -1;
}

#if defined(_WIN32)




static struct pcap_stat *pcap_stats_ex_rpcap(pcap_t *p, int *pcap_stat_size)
{
*pcap_stat_size = sizeof (p->stat);


return (rpcap_stats_rpcap(p, &(p->stat), PCAP_STATS_EX));
}
#endif






































static struct pcap_stat *rpcap_stats_rpcap(pcap_t *p, struct pcap_stat *ps, int mode)
{
struct pcap_rpcap *pr = p->priv;
struct rpcap_header header;
struct rpcap_stats netstats;
uint32 plen;

#if defined(_WIN32)
if (mode != PCAP_STATS_STANDARD && mode != PCAP_STATS_EX)
#else
if (mode != PCAP_STATS_STANDARD)
#endif
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Invalid stats mode %d", mode);
return NULL;
}






if (!pr->rmt_capstarted)
{
ps->ps_drop = 0;
ps->ps_ifdrop = 0;
ps->ps_recv = 0;
#if defined(_WIN32)
if (mode == PCAP_STATS_EX)
{
ps->ps_capt = 0;
ps->ps_sent = 0;
ps->ps_netdrop = 0;
}
#endif

return ps;
}

rpcap_createhdr(&header, pr->protocol_version,
RPCAP_MSG_STATS_REQ, 0, 0);


if (sock_send(pr->rmt_sockctrl, (char *)&header,
sizeof(struct rpcap_header), p->errbuf, PCAP_ERRBUF_SIZE) < 0)
return NULL;


if (rpcap_process_msg_header(pr->rmt_sockctrl, pr->protocol_version,
RPCAP_MSG_STATS_REQ, &header, p->errbuf) == -1)
return NULL;

plen = header.plen;


if (rpcap_recv(pr->rmt_sockctrl, (char *)&netstats,
sizeof(struct rpcap_stats), &plen, p->errbuf) == -1)
goto error;

ps->ps_drop = ntohl(netstats.krnldrop);
ps->ps_ifdrop = ntohl(netstats.ifdrop);
ps->ps_recv = ntohl(netstats.ifrecv);
#if defined(_WIN32)
if (mode == PCAP_STATS_EX)
{
ps->ps_capt = pr->TotCapt;
ps->ps_netdrop = pr->TotNetDrops;
ps->ps_sent = ntohl(netstats.svrcapt);
}
#endif


if (rpcap_discard(pr->rmt_sockctrl, plen, p->errbuf) == -1)
goto error_nodiscard;

return ps;

error:





(void)rpcap_discard(pr->rmt_sockctrl, plen, NULL);

error_nodiscard:
return NULL;
}




















static struct activehosts *
rpcap_remoteact_getsock(const char *host, int *error, char *errbuf)
{
struct activehosts *temp;
struct addrinfo hints, *addrinfo, *ai_next;
int retval;


addrinfo = NULL;
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

retval = getaddrinfo(host, "0", &hints, &addrinfo);
if (retval != 0)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "getaddrinfo() %s",
gai_strerror(retval));
*error = 1;
return NULL;
}

temp = activeHosts;

while (temp)
{
ai_next = addrinfo;
while (ai_next)
{
if (sock_cmpaddr(&temp->host, (struct sockaddr_storage *) ai_next->ai_addr) == 0)
{
*error = 0;
freeaddrinfo(addrinfo);
return temp;
}

ai_next = ai_next->ai_next;
}
temp = temp->next;
}

if (addrinfo)
freeaddrinfo(addrinfo);





*error = 0;
return NULL;
}














static int pcap_startcapture_remote(pcap_t *fp)
{
struct pcap_rpcap *pr = fp->priv;
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
char portdata[PCAP_BUF_SIZE];
uint32 plen;
int active = 0;
struct activehosts *temp;
char host[INET6_ADDRSTRLEN + 1];


struct addrinfo hints;
struct addrinfo *addrinfo;
SOCKET sockdata = 0;
struct sockaddr_storage saddr;
socklen_t saddrlen;
int ai_family;


struct rpcap_header header;
struct rpcap_startcapreq *startcapreq;
struct rpcap_startcapreply startcapreply;


int res;
socklen_t itemp;
int sockbufsize = 0;
uint32 server_sockbufsize;





if (pcap_setsampling_remote(fp) != 0)
return -1;


temp = activeHosts;
while (temp)
{
if (temp->sockctrl == pr->rmt_sockctrl)
{
active = 1;
break;
}
temp = temp->next;
}

addrinfo = NULL;








saddrlen = sizeof(struct sockaddr_storage);
if (getpeername(pr->rmt_sockctrl, (struct sockaddr *) &saddr, &saddrlen) == -1)
{
sock_geterror("getsockname()", fp->errbuf, PCAP_ERRBUF_SIZE);
goto error_nodiscard;
}
ai_family = ((struct sockaddr_storage *) &saddr)->ss_family;


if (getnameinfo((struct sockaddr *) &saddr, saddrlen, host,
sizeof(host), NULL, 0, NI_NUMERICHOST))
{
sock_geterror("getnameinfo()", fp->errbuf, PCAP_ERRBUF_SIZE);
goto error_nodiscard;
}






if ((active) || (pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP))
{





memset(&hints, 0, sizeof(struct addrinfo));

hints.ai_family = ai_family;
hints.ai_socktype = (pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP) ? SOCK_DGRAM : SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;


if (sock_initaddress(NULL, "0", &hints, &addrinfo, fp->errbuf, PCAP_ERRBUF_SIZE) == -1)
goto error_nodiscard;

if ((sockdata = sock_open(addrinfo, SOCKOPEN_SERVER,
1 , fp->errbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
goto error_nodiscard;


freeaddrinfo(addrinfo);
addrinfo = NULL;


saddrlen = sizeof(struct sockaddr_storage);
if (getsockname(sockdata, (struct sockaddr *) &saddr, &saddrlen) == -1)
{
sock_geterror("getsockname()", fp->errbuf, PCAP_ERRBUF_SIZE);
goto error_nodiscard;
}


if (getnameinfo((struct sockaddr *) &saddr, saddrlen, NULL,
0, portdata, sizeof(portdata), NI_NUMERICSERV))
{
sock_geterror("getnameinfo()", fp->errbuf, PCAP_ERRBUF_SIZE);
goto error_nodiscard;
}
}





if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf, PCAP_ERRBUF_SIZE))
goto error_nodiscard;

rpcap_createhdr((struct rpcap_header *) sendbuf,
pr->protocol_version, RPCAP_MSG_STARTCAP_REQ, 0,
sizeof(struct rpcap_startcapreq) + sizeof(struct rpcap_filter) + fp->fcode.bf_len * sizeof(struct rpcap_filterbpf_insn));


startcapreq = (struct rpcap_startcapreq *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_startcapreq), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf, PCAP_ERRBUF_SIZE))
goto error_nodiscard;

memset(startcapreq, 0, sizeof(struct rpcap_startcapreq));


fp->opt.timeout = fp->opt.timeout / 2;
startcapreq->read_timeout = htonl(fp->opt.timeout);


if ((active) || (pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP))
{
sscanf(portdata, "%d", (int *)&(startcapreq->portdata));
startcapreq->portdata = htons(startcapreq->portdata);
}

startcapreq->snaplen = htonl(fp->snapshot);
startcapreq->flags = 0;

if (pr->rmt_flags & PCAP_OPENFLAG_PROMISCUOUS)
startcapreq->flags |= RPCAP_STARTCAPREQ_FLAG_PROMISC;
if (pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP)
startcapreq->flags |= RPCAP_STARTCAPREQ_FLAG_DGRAM;
if (active)
startcapreq->flags |= RPCAP_STARTCAPREQ_FLAG_SERVEROPEN;

startcapreq->flags = htons(startcapreq->flags);


if (pcap_pack_bpffilter(fp, &sendbuf[sendbufidx], &sendbufidx, &fp->fcode))
goto error_nodiscard;

if (sock_send(pr->rmt_sockctrl, sendbuf, sendbufidx, fp->errbuf,
PCAP_ERRBUF_SIZE) < 0)
goto error_nodiscard;


if (rpcap_process_msg_header(pr->rmt_sockctrl, pr->protocol_version,
RPCAP_MSG_STARTCAP_REQ, &header, fp->errbuf) == -1)
goto error_nodiscard;

plen = header.plen;

if (rpcap_recv(pr->rmt_sockctrl, (char *)&startcapreply,
sizeof(struct rpcap_startcapreply), &plen, fp->errbuf) == -1)
goto error;












if (!(pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP))
{
if (!active)
{
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = ai_family;
hints.ai_socktype = (pr->rmt_flags & PCAP_OPENFLAG_DATATX_UDP) ? SOCK_DGRAM : SOCK_STREAM;
pcap_snprintf(portdata, PCAP_BUF_SIZE, "%d", ntohs(startcapreply.portdata));


if (sock_initaddress(host, portdata, &hints, &addrinfo, fp->errbuf, PCAP_ERRBUF_SIZE) == -1)
goto error;

if ((sockdata = sock_open(addrinfo, SOCKOPEN_CLIENT, 0, fp->errbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
goto error;


freeaddrinfo(addrinfo);
addrinfo = NULL;
}
else
{
SOCKET socktemp;


saddrlen = sizeof(struct sockaddr_storage);

socktemp = accept(sockdata, (struct sockaddr *) &saddr, &saddrlen);

if (socktemp == INVALID_SOCKET)
{
sock_geterror("accept()", fp->errbuf, PCAP_ERRBUF_SIZE);
goto error;
}


sock_close(sockdata, fp->errbuf, PCAP_ERRBUF_SIZE);
sockdata = socktemp;
}
}


pr->rmt_sockdata = sockdata;






server_sockbufsize = ntohl(startcapreply.bufsize);


itemp = sizeof(sockbufsize);

res = getsockopt(sockdata, SOL_SOCKET, SO_RCVBUF, (char *)&sockbufsize, &itemp);
if (res == -1)
{
sock_geterror("pcap_startcapture_remote(): getsockopt() failed", fp->errbuf, PCAP_ERRBUF_SIZE);
goto error;
}











if (server_sockbufsize <= (u_int) fp->snapshot)
server_sockbufsize += sizeof(struct pcap_pkthdr);


if ((u_int) sockbufsize < server_sockbufsize)
{




for (;;)
{
res = setsockopt(sockdata, SOL_SOCKET, SO_RCVBUF,
(char *)&(server_sockbufsize),
sizeof(server_sockbufsize));

if (res == 0)
break;






server_sockbufsize /= 2;

if ((u_int) sockbufsize >= server_sockbufsize)
{
server_sockbufsize = sockbufsize;
break;
}
}
}










fp->bufsize = sizeof(struct rpcap_header) + sizeof(struct rpcap_pkthdr) + fp->snapshot;

fp->buffer = (u_char *)malloc(fp->bufsize);
if (fp->buffer == NULL)
{
pcap_fmt_errmsg_for_errno(fp->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
goto error;
}




fp->bp = fp->buffer;
fp->cc = 0;


if (rpcap_discard(pr->rmt_sockctrl, plen, fp->errbuf) == -1)
goto error_nodiscard;







if (pr->rmt_flags & PCAP_OPENFLAG_NOCAPTURE_RPCAP)
{
struct bpf_program fcode;

if (pcap_createfilter_norpcappkt(fp, &fcode) == -1)
goto error;



if (pcap_updatefilter_remote(fp, &fcode) == -1)
goto error;

pcap_freecode(&fcode);
}

pr->rmt_capstarted = 1;
return 0;

error:












(void)rpcap_discard(pr->rmt_sockctrl, plen, NULL);

error_nodiscard:
if ((sockdata) && (sockdata != -1))
sock_close(sockdata, NULL, 0);

if (!active)
sock_close(pr->rmt_sockctrl, NULL, 0);

if (addrinfo != NULL)
freeaddrinfo(addrinfo);





#if 0
if (fp)
{
pcap_close(fp);
fp= NULL;
}
#endif

return -1;
}

























static int pcap_pack_bpffilter(pcap_t *fp, char *sendbuf, int *sendbufidx, struct bpf_program *prog)
{
struct rpcap_filter *filter;
struct rpcap_filterbpf_insn *insn;
struct bpf_insn *bf_insn;
struct bpf_program fake_prog;
unsigned int i;

if (prog->bf_len == 0)
{
if (pcap_compile(fp, &fake_prog, NULL , 1, 0) == -1)
return -1;

prog = &fake_prog;
}

filter = (struct rpcap_filter *) sendbuf;

if (sock_bufferize(NULL, sizeof(struct rpcap_filter), NULL, sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf, PCAP_ERRBUF_SIZE))
return -1;

filter->filtertype = htons(RPCAP_UPDATEFILTER_BPF);
filter->nitems = htonl((int32)prog->bf_len);

if (sock_bufferize(NULL, prog->bf_len * sizeof(struct rpcap_filterbpf_insn),
NULL, sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf, PCAP_ERRBUF_SIZE))
return -1;

insn = (struct rpcap_filterbpf_insn *) (filter + 1);
bf_insn = prog->bf_insns;

for (i = 0; i < prog->bf_len; i++)
{
insn->code = htons(bf_insn->code);
insn->jf = bf_insn->jf;
insn->jt = bf_insn->jt;
insn->k = htonl(bf_insn->k);

insn++;
bf_insn++;
}

return 0;
}


























static int pcap_updatefilter_remote(pcap_t *fp, struct bpf_program *prog)
{
struct pcap_rpcap *pr = fp->priv;
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
struct rpcap_header header;

if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf, PCAP_ERRBUF_SIZE))
return -1;

rpcap_createhdr((struct rpcap_header *) sendbuf,
pr->protocol_version, RPCAP_MSG_UPDATEFILTER_REQ, 0,
sizeof(struct rpcap_filter) + prog->bf_len * sizeof(struct rpcap_filterbpf_insn));

if (pcap_pack_bpffilter(fp, &sendbuf[sendbufidx], &sendbufidx, prog))
return -1;

if (sock_send(pr->rmt_sockctrl, sendbuf, sendbufidx, fp->errbuf,
PCAP_ERRBUF_SIZE) < 0)
return -1;


if (rpcap_process_msg_header(pr->rmt_sockctrl, pr->protocol_version,
RPCAP_MSG_UPDATEFILTER_REQ, &header, fp->errbuf) == -1)
return -1;




if (rpcap_discard(pr->rmt_sockctrl, header.plen, fp->errbuf) == -1)
return -1;

return 0;
}

static void
pcap_save_current_filter_rpcap(pcap_t *fp, const char *filter)
{
struct pcap_rpcap *pr = fp->priv;









if (pr->rmt_clientside &&
(pr->rmt_flags & PCAP_OPENFLAG_NOCAPTURE_RPCAP))
{
if (pr->currentfilter)
free(pr->currentfilter);

if (filter == NULL)
filter = "";

pr->currentfilter = strdup(filter);
}
}










static int pcap_setfilter_rpcap(pcap_t *fp, struct bpf_program *prog)
{
struct pcap_rpcap *pr = fp->priv;

if (!pr->rmt_capstarted)
{

if (install_bpf_program(fp, prog) == -1)
return -1;
return 0;
}


if (pcap_updatefilter_remote(fp, prog))
return -1;

return 0;
}











static int pcap_createfilter_norpcappkt(pcap_t *fp, struct bpf_program *prog)
{
struct pcap_rpcap *pr = fp->priv;
int RetVal = 0;


if (pr->rmt_flags & PCAP_OPENFLAG_NOCAPTURE_RPCAP)
{
struct sockaddr_storage saddr;
socklen_t saddrlen;
char myaddress[128];
char myctrlport[128];
char mydataport[128];
char peeraddress[128];
char peerctrlport[128];
char *newfilter;


saddrlen = sizeof(struct sockaddr_storage);
if (getpeername(pr->rmt_sockctrl, (struct sockaddr *) &saddr, &saddrlen) == -1)
{
sock_geterror("getpeername()", fp->errbuf, PCAP_ERRBUF_SIZE);
return -1;
}

if (getnameinfo((struct sockaddr *) &saddr, saddrlen, peeraddress,
sizeof(peeraddress), peerctrlport, sizeof(peerctrlport), NI_NUMERICHOST | NI_NUMERICSERV))
{
sock_geterror("getnameinfo()", fp->errbuf, PCAP_ERRBUF_SIZE);
return -1;
}



if (getsockname(pr->rmt_sockctrl, (struct sockaddr *) &saddr, &saddrlen) == -1)
{
sock_geterror("getsockname()", fp->errbuf, PCAP_ERRBUF_SIZE);
return -1;
}


if (getnameinfo((struct sockaddr *) &saddr, saddrlen, myaddress,
sizeof(myaddress), myctrlport, sizeof(myctrlport), NI_NUMERICHOST | NI_NUMERICSERV))
{
sock_geterror("getnameinfo()", fp->errbuf, PCAP_ERRBUF_SIZE);
return -1;
}


if (getsockname(pr->rmt_sockdata, (struct sockaddr *) &saddr, &saddrlen) == -1)
{
sock_geterror("getsockname()", fp->errbuf, PCAP_ERRBUF_SIZE);
return -1;
}


if (getnameinfo((struct sockaddr *) &saddr, saddrlen, NULL, 0, mydataport, sizeof(mydataport), NI_NUMERICSERV))
{
sock_geterror("getnameinfo()", fp->errbuf, PCAP_ERRBUF_SIZE);
return -1;
}

if (pr->currentfilter && pr->currentfilter[0] != '\0')
{




if (pcap_asprintf(&newfilter,
"(%s) and not (host %s and host %s and port %s and port %s) and not (host %s and host %s and port %s)",
pr->currentfilter, myaddress, peeraddress,
myctrlport, peerctrlport, myaddress, peeraddress,
mydataport) == -1)
{

pcap_snprintf(fp->errbuf, PCAP_ERRBUF_SIZE,
"Can't allocate memory for new filter");
return -1;
}
}
else
{




if (pcap_asprintf(&newfilter,
"not (host %s and host %s and port %s and port %s) and not (host %s and host %s and port %s)",
myaddress, peeraddress, myctrlport, peerctrlport,
myaddress, peeraddress, mydataport) == -1)
{

pcap_snprintf(fp->errbuf, PCAP_ERRBUF_SIZE,
"Can't allocate memory for new filter");
return -1;
}
}






pr->rmt_clientside = 0;

if (pcap_compile(fp, prog, newfilter, 1, 0) == -1)
RetVal = -1;


pr->rmt_clientside = 1;

free(newfilter);
}

return RetVal;
}














static int pcap_setsampling_remote(pcap_t *fp)
{
struct pcap_rpcap *pr = fp->priv;
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
struct rpcap_header header;
struct rpcap_sampling *sampling_pars;


if (fp->rmt_samp.method == PCAP_SAMP_NOSAMP)
return 0;






if (fp->rmt_samp.method < 0 || fp->rmt_samp.method > 255) {
pcap_snprintf(fp->errbuf, PCAP_ERRBUF_SIZE,
"Invalid sampling method %d", fp->rmt_samp.method);
return -1;
}
if (fp->rmt_samp.value < 0 || fp->rmt_samp.value > 65535) {
pcap_snprintf(fp->errbuf, PCAP_ERRBUF_SIZE,
"Invalid sampling value %d", fp->rmt_samp.value);
return -1;
}

if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf, PCAP_ERRBUF_SIZE))
return -1;

rpcap_createhdr((struct rpcap_header *) sendbuf,
pr->protocol_version, RPCAP_MSG_SETSAMPLING_REQ, 0,
sizeof(struct rpcap_sampling));


sampling_pars = (struct rpcap_sampling *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_sampling), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf, PCAP_ERRBUF_SIZE))
return -1;

memset(sampling_pars, 0, sizeof(struct rpcap_sampling));

sampling_pars->method = (uint8)fp->rmt_samp.method;
sampling_pars->value = (uint16)htonl(fp->rmt_samp.value);

if (sock_send(pr->rmt_sockctrl, sendbuf, sendbufidx, fp->errbuf,
PCAP_ERRBUF_SIZE) < 0)
return -1;


if (rpcap_process_msg_header(pr->rmt_sockctrl, pr->protocol_version,
RPCAP_MSG_SETSAMPLING_REQ, &header, fp->errbuf) == -1)
return -1;




if (rpcap_discard(pr->rmt_sockctrl, header.plen, fp->errbuf) == -1)
return -1;

return 0;
}







































static int rpcap_doauth(SOCKET sockctrl, uint8 *ver, struct pcap_rmtauth *auth, char *errbuf)
{
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
uint16 length;
struct rpcap_auth *rpauth;
uint16 auth_type;
struct rpcap_header header;
size_t str_length;
uint32 plen;
struct rpcap_authreply authreply;
uint8 ourvers;

if (auth)
{
switch (auth->type)
{
case RPCAP_RMTAUTH_NULL:
length = sizeof(struct rpcap_auth);
break;

case RPCAP_RMTAUTH_PWD:
length = sizeof(struct rpcap_auth);
if (auth->username)
{
str_length = strlen(auth->username);
if (str_length > 65535)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "User name is too long (> 65535 bytes)");
return -1;
}
length += (uint16)str_length;
}
if (auth->password)
{
str_length = strlen(auth->password);
if (str_length > 65535)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Password is too long (> 65535 bytes)");
return -1;
}
length += (uint16)str_length;
}
break;

default:
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication type not recognized.");
return -1;
}

auth_type = (uint16)auth->type;
}
else
{
auth_type = RPCAP_RMTAUTH_NULL;
length = sizeof(struct rpcap_auth);
}

if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf, PCAP_ERRBUF_SIZE))
return -1;

rpcap_createhdr((struct rpcap_header *) sendbuf, 0,
RPCAP_MSG_AUTH_REQ, 0, length);

rpauth = (struct rpcap_auth *) &sendbuf[sendbufidx];

if (sock_bufferize(NULL, sizeof(struct rpcap_auth), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf, PCAP_ERRBUF_SIZE))
return -1;

memset(rpauth, 0, sizeof(struct rpcap_auth));

rpauth->type = htons(auth_type);

if (auth_type == RPCAP_RMTAUTH_PWD)
{
if (auth->username)
rpauth->slen1 = (uint16)strlen(auth->username);
else
rpauth->slen1 = 0;

if (sock_bufferize(auth->username, rpauth->slen1, sendbuf,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf, PCAP_ERRBUF_SIZE))
return -1;

if (auth->password)
rpauth->slen2 = (uint16)strlen(auth->password);
else
rpauth->slen2 = 0;

if (sock_bufferize(auth->password, rpauth->slen2, sendbuf,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf, PCAP_ERRBUF_SIZE))
return -1;

rpauth->slen1 = htons(rpauth->slen1);
rpauth->slen2 = htons(rpauth->slen2);
}

if (sock_send(sockctrl, sendbuf, sendbufidx, errbuf,
PCAP_ERRBUF_SIZE) < 0)
return -1;


if (rpcap_process_msg_header(sockctrl, 0, RPCAP_MSG_AUTH_REQ,
&header, errbuf) == -1)
return -1;






plen = header.plen;
if (plen != 0)
{

if (plen < sizeof(struct rpcap_authreply))
{

(void)rpcap_discard(sockctrl, plen, NULL);
return -1;
}


if (rpcap_recv(sockctrl, (char *)&authreply,
sizeof(struct rpcap_authreply), &plen, errbuf) == -1)
{
(void)rpcap_discard(sockctrl, plen, NULL);
return -1;
}


if (rpcap_discard(sockctrl, plen, errbuf) == -1)
return -1;





if (authreply.minvers > authreply.maxvers)
{



pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"The server's minimum supported protocol version is greater than its maximum supported protocol version");
return -1;
}
}
else
{

authreply.minvers = 0;
authreply.maxvers = 0;
}




ourvers = authreply.maxvers;

#if RPCAP_MIN_VERSION != 0




if (ourvers < RPCAP_MIN_VERSION)
goto novers;
#endif





if (ourvers > RPCAP_MAX_VERSION)
{
ourvers = RPCAP_MAX_VERSION;





if (ourvers < authreply.minvers)
goto novers;
}

*ver = ourvers;
return 0;

novers:



pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"The server doesn't support any protocol version that we support");
return -1;
}


static int
pcap_getnonblock_rpcap(pcap_t *p)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Non-blocking mode isn't supported for capturing remotely with rpcap");
return (-1);
}

static int
pcap_setnonblock_rpcap(pcap_t *p, int nonblock _U_)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Non-blocking mode isn't supported for capturing remotely with rpcap");
return (-1);
}

static int
rpcap_setup_session(const char *source, struct pcap_rmtauth *auth,
int *activep, SOCKET *sockctrlp, uint8 *protocol_versionp,
char *host, char *port, char *iface, char *errbuf)
{
int type;
struct activehosts *activeconn;
int error;






if (pcap_parsesrcstr(source, &type, host, port, iface, errbuf) == -1)
return -1;




if (type != PCAP_SRC_IFREMOTE)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Non-remote interface passed to remote capture routine");
return -1;
}



if (sock_init(errbuf, PCAP_ERRBUF_SIZE) == -1)
return -1;


activeconn = rpcap_remoteact_getsock(host, &error, errbuf);
if (activeconn != NULL)
{
*activep = 1;
*sockctrlp = activeconn->sockctrl;
*protocol_versionp = activeconn->protocol_version;
}
else
{
*activep = 0;
struct addrinfo hints;
struct addrinfo *addrinfo;

if (error)
{



return -1;
}





memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

if (port[0] == 0)
{

if (sock_initaddress(host, RPCAP_DEFAULT_NETPORT,
&hints, &addrinfo, errbuf, PCAP_ERRBUF_SIZE) == -1)
return -1;
}
else
{
if (sock_initaddress(host, port, &hints, &addrinfo,
errbuf, PCAP_ERRBUF_SIZE) == -1)
return -1;
}

if ((*sockctrlp = sock_open(addrinfo, SOCKOPEN_CLIENT, 0,
errbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
{
freeaddrinfo(addrinfo);
return -1;
}


freeaddrinfo(addrinfo);
addrinfo = NULL;

if (rpcap_doauth(*sockctrlp, protocol_versionp, auth,
errbuf) == -1)
{
sock_close(*sockctrlp, NULL, 0);
return -1;
}
}
return 0;
}








































pcap_t *pcap_open_rpcap(const char *source, int snaplen, int flags, int read_timeout, struct pcap_rmtauth *auth, char *errbuf)
{
pcap_t *fp;
char *source_str;
struct pcap_rpcap *pr;
char host[PCAP_BUF_SIZE], ctrlport[PCAP_BUF_SIZE], iface[PCAP_BUF_SIZE];
SOCKET sockctrl;
uint8 protocol_version;
int active;
uint32 plen;
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;


struct rpcap_header header;
struct rpcap_openreply openreply;

fp = pcap_create_common(errbuf, sizeof (struct pcap_rpcap));
if (fp == NULL)
{
return NULL;
}
source_str = strdup(source);
if (source_str == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return NULL;
}












if (snaplen <= 0 || snaplen > MAXIMUM_SNAPLEN)
snaplen = MAXIMUM_SNAPLEN;

fp->opt.device = source_str;
fp->snapshot = snaplen;
fp->opt.timeout = read_timeout;
pr = fp->priv;
pr->rmt_flags = flags;




if (rpcap_setup_session(fp->opt.device, auth, &active, &sockctrl,
&protocol_version, host, ctrlport, iface, errbuf) == -1)
{

pcap_close(fp);
return NULL;
}





if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
&sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf, PCAP_ERRBUF_SIZE))
goto error_nodiscard;

rpcap_createhdr((struct rpcap_header *) sendbuf, protocol_version,
RPCAP_MSG_OPEN_REQ, 0, (uint32) strlen(iface));

if (sock_bufferize(iface, (int) strlen(iface), sendbuf, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf, PCAP_ERRBUF_SIZE))
goto error_nodiscard;

if (sock_send(sockctrl, sendbuf, sendbufidx, errbuf,
PCAP_ERRBUF_SIZE) < 0)
goto error_nodiscard;


if (rpcap_process_msg_header(sockctrl, protocol_version,
RPCAP_MSG_OPEN_REQ, &header, errbuf) == -1)
goto error_nodiscard;
plen = header.plen;


if (rpcap_recv(sockctrl, (char *)&openreply,
sizeof(struct rpcap_openreply), &plen, errbuf) == -1)
goto error;


if (rpcap_discard(sockctrl, plen, errbuf) == -1)
goto error_nodiscard;


fp->linktype = ntohl(openreply.linktype);
fp->tzoff = ntohl(openreply.tzoff);
pr->rmt_sockctrl = sockctrl;
pr->protocol_version = protocol_version;
pr->rmt_clientside = 1;


fp->read_op = pcap_read_rpcap;
fp->save_current_filter_op = pcap_save_current_filter_rpcap;
fp->setfilter_op = pcap_setfilter_rpcap;
fp->getnonblock_op = pcap_getnonblock_rpcap;
fp->setnonblock_op = pcap_setnonblock_rpcap;
fp->stats_op = pcap_stats_rpcap;
#if defined(_WIN32)
fp->stats_ex_op = pcap_stats_ex_rpcap;
#endif
fp->cleanup_op = pcap_cleanup_rpcap;

fp->activated = 1;
return fp;

error:












(void)rpcap_discard(sockctrl, plen, NULL);

error_nodiscard:
if (!active)
sock_close(sockctrl, NULL, 0);

pcap_close(fp);
return NULL;
}


#define PCAP_TEXT_SOURCE_ADAPTER "Network adapter"
#define PCAP_TEXT_SOURCE_ADAPTER_LEN (sizeof PCAP_TEXT_SOURCE_ADAPTER - 1)

#define PCAP_TEXT_SOURCE_ON_REMOTE_HOST "on remote node"
#define PCAP_TEXT_SOURCE_ON_REMOTE_HOST_LEN (sizeof PCAP_TEXT_SOURCE_ON_REMOTE_HOST - 1)

static void
freeaddr(struct pcap_addr *addr)
{
free(addr->addr);
free(addr->netmask);
free(addr->broadaddr);
free(addr->dstaddr);
free(addr);
}

int
pcap_findalldevs_ex_remote(const char *source, struct pcap_rmtauth *auth, pcap_if_t **alldevs, char *errbuf)
{
uint8 protocol_version;
SOCKET sockctrl;
uint32 plen;
struct rpcap_header header;
int i, j;
int nif;
int active;
char host[PCAP_BUF_SIZE], port[PCAP_BUF_SIZE];
char tmpstring[PCAP_BUF_SIZE + 1];
pcap_if_t *lastdev;
pcap_if_t *dev;


(*alldevs) = NULL;
lastdev = NULL;




if (rpcap_setup_session(source, auth, &active, &sockctrl,
&protocol_version, host, port, NULL, errbuf) == -1)
{

return -1;
}


rpcap_createhdr(&header, protocol_version, RPCAP_MSG_FINDALLIF_REQ,
0, 0);

if (sock_send(sockctrl, (char *)&header, sizeof(struct rpcap_header),
errbuf, PCAP_ERRBUF_SIZE) < 0)
goto error_nodiscard;


if (rpcap_process_msg_header(sockctrl, protocol_version,
RPCAP_MSG_FINDALLIF_REQ, &header, errbuf) == -1)
goto error_nodiscard;

plen = header.plen;


nif = ntohs(header.value);


for (i = 0; i < nif; i++)
{
struct rpcap_findalldevs_if findalldevs_if;
char tmpstring2[PCAP_BUF_SIZE + 1];
struct pcap_addr *addr, *prevaddr;

tmpstring2[PCAP_BUF_SIZE] = 0;


if (rpcap_recv(sockctrl, (char *)&findalldevs_if,
sizeof(struct rpcap_findalldevs_if), &plen, errbuf) == -1)
goto error;

findalldevs_if.namelen = ntohs(findalldevs_if.namelen);
findalldevs_if.desclen = ntohs(findalldevs_if.desclen);
findalldevs_if.naddr = ntohs(findalldevs_if.naddr);


dev = (pcap_if_t *)malloc(sizeof(pcap_if_t));
if (dev == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc() failed");
goto error;
}


memset(dev, 0, sizeof(pcap_if_t));


if (lastdev == NULL)
{



*alldevs = dev;
}
else
{



lastdev->next = dev;
}

lastdev = dev;


if (findalldevs_if.namelen)
{

if (findalldevs_if.namelen >= sizeof(tmpstring))
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Interface name too long");
goto error;
}


if (rpcap_recv(sockctrl, tmpstring,
findalldevs_if.namelen, &plen, errbuf) == -1)
goto error;

tmpstring[findalldevs_if.namelen] = 0;


if (pcap_createsrcstr(tmpstring2, PCAP_SRC_IFREMOTE,
host, port, tmpstring, errbuf) == -1)
goto error;

dev->name = strdup(tmpstring2);
if (dev->name == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno, "malloc() failed");
goto error;
}
}

if (findalldevs_if.desclen)
{
if (findalldevs_if.desclen >= sizeof(tmpstring))
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Interface description too long");
goto error;
}


if (rpcap_recv(sockctrl, tmpstring,
findalldevs_if.desclen, &plen, errbuf) == -1)
goto error;

tmpstring[findalldevs_if.desclen] = 0;

if (pcap_asprintf(&dev->description,
"%s '%s' %s %s", PCAP_TEXT_SOURCE_ADAPTER,
tmpstring, PCAP_TEXT_SOURCE_ON_REMOTE_HOST, host) == -1)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno, "malloc() failed");
goto error;
}
}

dev->flags = ntohl(findalldevs_if.flags);

prevaddr = NULL;

for (j = 0; j < findalldevs_if.naddr; j++)
{
struct rpcap_findalldevs_ifaddr ifaddr;


if (rpcap_recv(sockctrl, (char *)&ifaddr,
sizeof(struct rpcap_findalldevs_ifaddr),
&plen, errbuf) == -1)
goto error;




addr = (struct pcap_addr *) malloc(sizeof(struct pcap_addr));
if (addr == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno, "malloc() failed");
goto error;
}
addr->next = NULL;
addr->addr = NULL;
addr->netmask = NULL;
addr->broadaddr = NULL;
addr->dstaddr = NULL;

if (rpcap_deseraddr(&ifaddr.addr,
(struct sockaddr_storage **) &addr->addr, errbuf) == -1)
{
freeaddr(addr);
goto error;
}
if (rpcap_deseraddr(&ifaddr.netmask,
(struct sockaddr_storage **) &addr->netmask, errbuf) == -1)
{
freeaddr(addr);
goto error;
}
if (rpcap_deseraddr(&ifaddr.broadaddr,
(struct sockaddr_storage **) &addr->broadaddr, errbuf) == -1)
{
freeaddr(addr);
goto error;
}
if (rpcap_deseraddr(&ifaddr.dstaddr,
(struct sockaddr_storage **) &addr->dstaddr, errbuf) == -1)
{
freeaddr(addr);
goto error;
}

if ((addr->addr == NULL) && (addr->netmask == NULL) &&
(addr->broadaddr == NULL) && (addr->dstaddr == NULL))
{




free(addr);
}
else
{



if (prevaddr == NULL)
{
dev->addresses = addr;
}
else
{
prevaddr->next = addr;
}
prevaddr = addr;
}
}
}


if (rpcap_discard(sockctrl, plen, errbuf) == 1)
goto error_nodiscard;


if (!active)
{

if (sock_close(sockctrl, errbuf, PCAP_ERRBUF_SIZE))
return -1;
}


sock_cleanup();

return 0;

error:













(void) rpcap_discard(sockctrl, plen, NULL);

error_nodiscard:

if (!active)
sock_close(sockctrl, NULL, 0);


sock_cleanup();


pcap_freealldevs(*alldevs);

return -1;
}








SOCKET pcap_remoteact_accept(const char *address, const char *port, const char *hostlist, char *connectinghost, struct pcap_rmtauth *auth, char *errbuf)
{

struct addrinfo hints;
struct addrinfo *addrinfo;
struct sockaddr_storage from;
socklen_t fromlen;
SOCKET sockctrl;
uint8 protocol_version;
struct activehosts *temp, *prev;

*connectinghost = 0;


memset(&hints, 0, sizeof(struct addrinfo));

hints.ai_family = AF_INET;
hints.ai_flags = AI_PASSIVE;
hints.ai_socktype = SOCK_STREAM;



if (sock_init(errbuf, PCAP_ERRBUF_SIZE) == -1)
return (SOCKET)-1;


if ((port == NULL) || (port[0] == 0))
{
if (sock_initaddress(address, RPCAP_DEFAULT_NETPORT_ACTIVE, &hints, &addrinfo, errbuf, PCAP_ERRBUF_SIZE) == -1)
{
return (SOCKET)-2;
}
}
else
{
if (sock_initaddress(address, port, &hints, &addrinfo, errbuf, PCAP_ERRBUF_SIZE) == -1)
{
return (SOCKET)-2;
}
}


if ((sockmain = sock_open(addrinfo, SOCKOPEN_SERVER, 1, errbuf, PCAP_ERRBUF_SIZE)) == INVALID_SOCKET)
{
freeaddrinfo(addrinfo);
return (SOCKET)-2;
}
freeaddrinfo(addrinfo);


fromlen = sizeof(struct sockaddr_storage);

sockctrl = accept(sockmain, (struct sockaddr *) &from, &fromlen);



closesocket(sockmain);
sockmain = 0;

if (sockctrl == INVALID_SOCKET)
{
sock_geterror("accept()", errbuf, PCAP_ERRBUF_SIZE);
return (SOCKET)-2;
}


if (getnameinfo((struct sockaddr *) &from, fromlen, connectinghost, RPCAP_HOSTLIST_SIZE, NULL, 0, NI_NUMERICHOST))
{
sock_geterror("getnameinfo()", errbuf, PCAP_ERRBUF_SIZE);
rpcap_senderror(sockctrl, 0, PCAP_ERR_REMOTEACCEPT, errbuf, NULL);
sock_close(sockctrl, NULL, 0);
return (SOCKET)-1;
}


if (sock_check_hostlist((char *)hostlist, RPCAP_HOSTLIST_SEP, &from, errbuf, PCAP_ERRBUF_SIZE) < 0)
{
rpcap_senderror(sockctrl, 0, PCAP_ERR_REMOTEACCEPT, errbuf, NULL);
sock_close(sockctrl, NULL, 0);
return (SOCKET)-1;
}




if (rpcap_doauth(sockctrl, &protocol_version, auth, errbuf) == -1)
{

rpcap_senderror(sockctrl, 0, PCAP_ERR_REMOTEACCEPT, errbuf, NULL);
sock_close(sockctrl, NULL, 0);
return (SOCKET)-3;
}




temp = activeHosts;
prev = NULL;

while (temp)
{

if (sock_cmpaddr(&temp->host, &from) == 0)
return sockctrl;

prev = temp;
temp = temp->next;
}


if (prev)
{
prev->next = (struct activehosts *) malloc(sizeof(struct activehosts));
temp = prev->next;
}
else
{
activeHosts = (struct activehosts *) malloc(sizeof(struct activehosts));
temp = activeHosts;
}

if (temp == NULL)
{
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc() failed");
rpcap_senderror(sockctrl, protocol_version, PCAP_ERR_REMOTEACCEPT, errbuf, NULL);
sock_close(sockctrl, NULL, 0);
return (SOCKET)-1;
}

memcpy(&temp->host, &from, fromlen);
temp->sockctrl = sockctrl;
temp->protocol_version = protocol_version;
temp->next = NULL;

return sockctrl;
}

int pcap_remoteact_close(const char *host, char *errbuf)
{
struct activehosts *temp, *prev;
struct addrinfo hints, *addrinfo, *ai_next;
int retval;

temp = activeHosts;
prev = NULL;


addrinfo = NULL;
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

retval = getaddrinfo(host, "0", &hints, &addrinfo);
if (retval != 0)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "getaddrinfo() %s", gai_strerror(retval));
return -1;
}

while (temp)
{
ai_next = addrinfo;
while (ai_next)
{
if (sock_cmpaddr(&temp->host, (struct sockaddr_storage *) ai_next->ai_addr) == 0)
{
struct rpcap_header header;
int status = 0;


rpcap_createhdr(&header, temp->protocol_version,
RPCAP_MSG_CLOSE, 0, 0);





if (sock_send(temp->sockctrl,
(char *)&header,
sizeof(struct rpcap_header), errbuf,
PCAP_ERRBUF_SIZE) < 0)
{




(void)sock_close(temp->sockctrl, NULL,
0);
status = -1;
}
else
{
if (sock_close(temp->sockctrl, errbuf,
PCAP_ERRBUF_SIZE) == -1)
status = -1;
}





if (prev)
prev->next = temp->next;
else
activeHosts = temp->next;

freeaddrinfo(addrinfo);

free(temp);


sock_cleanup();

return status;
}

ai_next = ai_next->ai_next;
}
prev = temp;
temp = temp->next;
}

if (addrinfo)
freeaddrinfo(addrinfo);


sock_cleanup();

pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "The host you want to close the active connection is not known");
return -1;
}

void pcap_remoteact_cleanup(void)
{

if (sockmain)
{
closesocket(sockmain);


sock_cleanup();
}

}

int pcap_remoteact_list(char *hostlist, char sep, int size, char *errbuf)
{
struct activehosts *temp;
size_t len;
char hoststr[RPCAP_HOSTLIST_SIZE + 1];

temp = activeHosts;

len = 0;
*hostlist = 0;

while (temp)
{



if (sock_getascii_addrport((struct sockaddr_storage *) &temp->host, hoststr,
RPCAP_HOSTLIST_SIZE, NULL, 0, NI_NUMERICHOST, errbuf, PCAP_ERRBUF_SIZE) != -1)


{

return -1;
}

len = len + strlen(hoststr) + 1 ;

if ((size < 0) || (len >= (size_t)size))
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "The string you provided is not able to keep "
"the hostnames for all the active connections");
return -1;
}

pcap_strlcat(hostlist, hoststr, PCAP_ERRBUF_SIZE);
hostlist[len - 1] = sep;
hostlist[len] = 0;

temp = temp->next;
}

return 0;
}




static int rpcap_recv_msg_header(SOCKET sock, struct rpcap_header *header, char *errbuf)
{
int nrecv;

nrecv = sock_recv(sock, (char *) header, sizeof(struct rpcap_header),
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf,
PCAP_ERRBUF_SIZE);
if (nrecv == -1)
{

return -1;
}
header->plen = ntohl(header->plen);
return 0;
}





static int rpcap_check_msg_ver(SOCKET sock, uint8 expected_ver, struct rpcap_header *header, char *errbuf)
{



if (header->ver != expected_ver)
{



if (rpcap_discard(sock, header->plen, errbuf) == -1)
return -1;




if (errbuf != NULL)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Server sent us a message with version %u when we were expecting %u",
header->ver, expected_ver);
}
return -1;
}
return 0;
}





static int rpcap_check_msg_type(SOCKET sock, uint8 request_type, struct rpcap_header *header, uint16 *errcode, char *errbuf)
{
const char *request_type_string;
const char *msg_type_string;




if (header->type == RPCAP_MSG_ERROR)
{




*errcode = ntohs(header->value);
rpcap_msg_err(sock, header->plen, errbuf);
return -1;
}

*errcode = 0;





if (header->type != (request_type | RPCAP_MSG_IS_REPLY))
{







if (rpcap_discard(sock, header->plen, errbuf) == -1)
return -1;




request_type_string = rpcap_msg_type_string(request_type);
msg_type_string = rpcap_msg_type_string(header->type);
if (errbuf != NULL)
{
if (request_type_string == NULL)
{

pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"rpcap_check_msg_type called for request message with type %u",
request_type);
return -1;
}
if (msg_type_string != NULL)
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"%s message received in response to a %s message",
msg_type_string, request_type_string);
else
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Message of unknown type %u message received in response to a %s request",
header->type, request_type_string);
}
return -1;
}

return 0;
}




static int rpcap_process_msg_header(SOCKET sock, uint8 expected_ver, uint8 request_type, struct rpcap_header *header, char *errbuf)
{
uint16 errcode;

if (rpcap_recv_msg_header(sock, header, errbuf) == -1)
{

return -1;
}




if (rpcap_check_msg_ver(sock, expected_ver, header, errbuf) == -1)
return -1;




return rpcap_check_msg_type(sock, request_type, header,
&errcode, errbuf);
}










static int rpcap_recv(SOCKET sock, void *buffer, size_t toread, uint32 *plen, char *errbuf)
{
int nread;

if (toread > *plen)
{

pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Message payload is too short");
return -1;
}
nread = sock_recv(sock, buffer, toread,
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf, PCAP_ERRBUF_SIZE);
if (nread == -1)
{
return -1;
}
*plen -= nread;
return 0;
}




static void rpcap_msg_err(SOCKET sockctrl, uint32 plen, char *remote_errbuf)
{
char errbuf[PCAP_ERRBUF_SIZE];

if (plen >= PCAP_ERRBUF_SIZE)
{




if (sock_recv(sockctrl, remote_errbuf, PCAP_ERRBUF_SIZE - 1,
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf,
PCAP_ERRBUF_SIZE) == -1)
{

pcap_snprintf(remote_errbuf, PCAP_ERRBUF_SIZE, "Read of error message from client failed: %s", errbuf);
return;
}




remote_errbuf[PCAP_ERRBUF_SIZE - 1] = '\0';




(void)rpcap_discard(sockctrl, plen - (PCAP_ERRBUF_SIZE - 1), remote_errbuf);
}
else if (plen == 0)
{

remote_errbuf[0] = '\0';
}
else
{
if (sock_recv(sockctrl, remote_errbuf, plen,
SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf,
PCAP_ERRBUF_SIZE) == -1)
{

pcap_snprintf(remote_errbuf, PCAP_ERRBUF_SIZE, "Read of error message from client failed: %s", errbuf);
return;
}




remote_errbuf[plen] = '\0';
}
}







static int rpcap_discard(SOCKET sock, uint32 len, char *errbuf)
{
if (len != 0)
{
if (sock_discard(sock, len, errbuf, PCAP_ERRBUF_SIZE) == -1)
{

return -1;
}
}
return 0;
}





static int rpcap_read_packet_msg(SOCKET sock, pcap_t *p, size_t size)
{
u_char *bp;
int cc;
int bytes_read;

bp = p->bp;
cc = p->cc;





while ((size_t)cc < size)
{




bytes_read = sock_recv(sock, bp, size - cc,
SOCK_RECEIVEALL_NO|SOCK_EOF_IS_ERROR, p->errbuf,
PCAP_ERRBUF_SIZE);
if (bytes_read == -1)
{




p->bp = bp;
p->cc = cc;
return -1;
}
if (bytes_read == -3)
{





p->bp = bp;
p->cc = cc;
return -3;
}
if (bytes_read == 0)
{





pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The server terminated the connection.");
return -1;
}
bp += bytes_read;
cc += bytes_read;
}
p->bp = bp;
p->cc = cc;
return 0;
}
