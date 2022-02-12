








#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/param.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pcap-int.h"

#include <ctype.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct mbuf;
struct rtentry;
#include <net/if.h>

#include "dagnew.h"
#include "dagapi.h"
#include "dagpci.h"
#include "dag_config_api.h"

#include "pcap-dag.h"






#if !defined(DAG_MAX_BOARDS)
#define DAG_MAX_BOARDS 32
#endif


#if !defined(ERF_TYPE_AAL5)
#define ERF_TYPE_AAL5 4
#endif

#if !defined(ERF_TYPE_MC_HDLC)
#define ERF_TYPE_MC_HDLC 5
#endif

#if !defined(ERF_TYPE_MC_RAW)
#define ERF_TYPE_MC_RAW 6
#endif

#if !defined(ERF_TYPE_MC_ATM)
#define ERF_TYPE_MC_ATM 7
#endif

#if !defined(ERF_TYPE_MC_RAW_CHANNEL)
#define ERF_TYPE_MC_RAW_CHANNEL 8
#endif

#if !defined(ERF_TYPE_MC_AAL5)
#define ERF_TYPE_MC_AAL5 9
#endif

#if !defined(ERF_TYPE_COLOR_HDLC_POS)
#define ERF_TYPE_COLOR_HDLC_POS 10
#endif

#if !defined(ERF_TYPE_COLOR_ETH)
#define ERF_TYPE_COLOR_ETH 11
#endif

#if !defined(ERF_TYPE_MC_AAL2)
#define ERF_TYPE_MC_AAL2 12
#endif

#if !defined(ERF_TYPE_IP_COUNTER)
#define ERF_TYPE_IP_COUNTER 13
#endif

#if !defined(ERF_TYPE_TCP_FLOW_COUNTER)
#define ERF_TYPE_TCP_FLOW_COUNTER 14
#endif

#if !defined(ERF_TYPE_DSM_COLOR_HDLC_POS)
#define ERF_TYPE_DSM_COLOR_HDLC_POS 15
#endif

#if !defined(ERF_TYPE_DSM_COLOR_ETH)
#define ERF_TYPE_DSM_COLOR_ETH 16
#endif

#if !defined(ERF_TYPE_COLOR_MC_HDLC_POS)
#define ERF_TYPE_COLOR_MC_HDLC_POS 17
#endif

#if !defined(ERF_TYPE_AAL2)
#define ERF_TYPE_AAL2 18
#endif

#if !defined(ERF_TYPE_COLOR_HASH_POS)
#define ERF_TYPE_COLOR_HASH_POS 19
#endif

#if !defined(ERF_TYPE_COLOR_HASH_ETH)
#define ERF_TYPE_COLOR_HASH_ETH 20
#endif

#if !defined(ERF_TYPE_INFINIBAND)
#define ERF_TYPE_INFINIBAND 21
#endif

#if !defined(ERF_TYPE_IPV4)
#define ERF_TYPE_IPV4 22
#endif

#if !defined(ERF_TYPE_IPV6)
#define ERF_TYPE_IPV6 23
#endif

#if !defined(ERF_TYPE_RAW_LINK)
#define ERF_TYPE_RAW_LINK 24
#endif

#if !defined(ERF_TYPE_INFINIBAND_LINK)
#define ERF_TYPE_INFINIBAND_LINK 25
#endif

#if !defined(ERF_TYPE_META)
#define ERF_TYPE_META 27
#endif

#if !defined(ERF_TYPE_PAD)
#define ERF_TYPE_PAD 48
#endif

#define ATM_CELL_SIZE 52
#define ATM_HDR_SIZE 4




#define MTP2_SENT_OFFSET 0
#define MTP2_ANNEX_A_USED_OFFSET 1
#define MTP2_LINK_NUMBER_OFFSET 2
#define MTP2_HDR_LEN 4

#define MTP2_ANNEX_A_NOT_USED 0
#define MTP2_ANNEX_A_USED 1
#define MTP2_ANNEX_A_USED_UNKNOWN 2


struct sunatm_hdr {
unsigned char flags;
unsigned char vpi;
unsigned short vci;
};




struct pcap_dag {
struct pcap_stat stat;
u_char *dag_mem_bottom;
u_char *dag_mem_top;
int dag_fcs_bits;
int dag_flags;
int dag_stream;
int dag_timeout;


dag_card_ref_t dag_ref;
dag_component_t dag_root;
attr_uuid_t drop_attr;
struct timeval required_select_timeout;

};

typedef struct pcap_dag_node {
struct pcap_dag_node *next;
pcap_t *p;
pid_t pid;
} pcap_dag_node_t;

static pcap_dag_node_t *pcap_dags = NULL;
static int atexit_handler_installed = 0;
static const unsigned short endian_test_word = 0x0100;

#define IS_BIGENDIAN() (*((unsigned char *)&endian_test_word))

#define MAX_DAG_PACKET 65536

static unsigned char TempPkt[MAX_DAG_PACKET];

#if !defined(HAVE_DAG_LARGE_STREAMS_API)
#define dag_attach_stream64(a, b, c, d) dag_attach_stream(a, b, c, d)
#define dag_get_stream_poll64(a, b, c, d, e) dag_get_stream_poll(a, b, c, d, e)
#define dag_set_stream_poll64(a, b, c, d, e) dag_set_stream_poll(a, b, c, d, e)
#define dag_size_t uint32_t
#endif

static int dag_setfilter(pcap_t *p, struct bpf_program *fp);
static int dag_stats(pcap_t *p, struct pcap_stat *ps);
static int dag_set_datalink(pcap_t *p, int dlt);
static int dag_get_datalink(pcap_t *p);
static int dag_setnonblock(pcap_t *p, int nonblock);

static void
delete_pcap_dag(pcap_t *p)
{
pcap_dag_node_t *curr = NULL, *prev = NULL;

for (prev = NULL, curr = pcap_dags; curr != NULL && curr->p != p; prev = curr, curr = curr->next) {

}

if (curr != NULL && curr->p == p) {
if (prev != NULL) {
prev->next = curr->next;
} else {
pcap_dags = curr->next;
}
}
}






static void
dag_platform_cleanup(pcap_t *p)
{
struct pcap_dag *pd = p->priv;

if(dag_stop_stream(p->fd, pd->dag_stream) < 0)
fprintf(stderr,"dag_stop_stream: %s\n", strerror(errno));

if(dag_detach_stream(p->fd, pd->dag_stream) < 0)
fprintf(stderr,"dag_detach_stream: %s\n", strerror(errno));

if(pd->dag_ref != NULL) {
dag_config_dispose(pd->dag_ref);







p->fd = -1;
pd->dag_ref = NULL;
}
delete_pcap_dag(p);
pcap_cleanup_live_common(p);
}

static void
atexit_handler(void)
{
while (pcap_dags != NULL) {
if (pcap_dags->pid == getpid()) {
if (pcap_dags->p != NULL)
dag_platform_cleanup(pcap_dags->p);
} else {
delete_pcap_dag(pcap_dags->p);
}
}
}

static int
new_pcap_dag(pcap_t *p)
{
pcap_dag_node_t *node = NULL;

if ((node = malloc(sizeof(pcap_dag_node_t))) == NULL) {
return -1;
}

if (!atexit_handler_installed) {
atexit(atexit_handler);
atexit_handler_installed = 1;
}

node->next = pcap_dags;
node->p = p;
node->pid = getpid();

pcap_dags = node;

return 0;
}

static unsigned int
dag_erf_ext_header_count(uint8_t * erf, size_t len)
{
uint32_t hdr_num = 0;
uint8_t hdr_type;


if ( erf == NULL )
return 0;
if ( len < 16 )
return 0;


if ( (erf[8] & 0x80) == 0x00 )
return 0;


do {


if ( len < (24 + (hdr_num * 8)) )
return hdr_num;


hdr_type = erf[(16 + (hdr_num * 8))];
hdr_num++;

} while ( hdr_type & 0x80 );

return hdr_num;
}






static int
dag_read(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_dag *pd = p->priv;
unsigned int processed = 0;
unsigned int nonblocking = pd->dag_flags & DAGF_NONBLOCK;
unsigned int num_ext_hdr = 0;
unsigned int ticks_per_second;


while (pd->dag_mem_top - pd->dag_mem_bottom < dag_record_size) {




if (p->break_loop) {





p->break_loop = 0;
return -2;
}












if ( NULL == (pd->dag_mem_top = dag_advance_stream(p->fd, pd->dag_stream, &(pd->dag_mem_bottom))) ) {
return -1;
}

if (nonblocking && (pd->dag_mem_top - pd->dag_mem_bottom < dag_record_size))
{

return 0;
}

if(!nonblocking &&
pd->dag_timeout &&
(pd->dag_mem_top - pd->dag_mem_bottom < dag_record_size))
{

return 0;
}

}


while (pd->dag_mem_top - pd->dag_mem_bottom >= dag_record_size) {

unsigned short packet_len = 0;
int caplen = 0;
struct pcap_pkthdr pcap_header;

dag_record_t *header = (dag_record_t *)(pd->dag_mem_bottom);

u_char *dp = ((u_char *)header);
unsigned short rlen;




if (p->break_loop) {





p->break_loop = 0;
return -2;
}

rlen = ntohs(header->rlen);
if (rlen < dag_record_size)
{
strncpy(p->errbuf, "dag_read: record too small", PCAP_ERRBUF_SIZE);
return -1;
}
pd->dag_mem_bottom += rlen;


switch((header->type & 0x7f)) {

case ERF_TYPE_COLOR_HDLC_POS:
case ERF_TYPE_COLOR_ETH:
case ERF_TYPE_DSM_COLOR_HDLC_POS:
case ERF_TYPE_DSM_COLOR_ETH:
case ERF_TYPE_COLOR_MC_HDLC_POS:
case ERF_TYPE_COLOR_HASH_ETH:
case ERF_TYPE_COLOR_HASH_POS:
break;

default:
if ( (pd->drop_attr == kNullAttributeUuid) && (header->lctr) ) {
pd->stat.ps_drop += ntohs(header->lctr);
}
}

if ((header->type & 0x7f) == ERF_TYPE_PAD) {
continue;
}

num_ext_hdr = dag_erf_ext_header_count(dp, rlen);















if (p->linktype == DLT_ERF) {
packet_len = ntohs(header->wlen) + dag_record_size;
caplen = rlen;
switch ((header->type & 0x7f)) {
case ERF_TYPE_MC_AAL5:
case ERF_TYPE_MC_ATM:
case ERF_TYPE_MC_HDLC:
case ERF_TYPE_MC_RAW_CHANNEL:
case ERF_TYPE_MC_RAW:
case ERF_TYPE_MC_AAL2:
case ERF_TYPE_COLOR_MC_HDLC_POS:
packet_len += 4;
break;

case ERF_TYPE_COLOR_HASH_ETH:
case ERF_TYPE_DSM_COLOR_ETH:
case ERF_TYPE_COLOR_ETH:
case ERF_TYPE_ETH:
packet_len += 2;
break;
}


packet_len += (8 * num_ext_hdr);

if (caplen > packet_len) {
caplen = packet_len;
}
} else {



dp += dag_record_size;

dp += 8 * num_ext_hdr;

switch((header->type & 0x7f)) {
case ERF_TYPE_ATM:
case ERF_TYPE_AAL5:
if ((header->type & 0x7f) == ERF_TYPE_AAL5) {
packet_len = ntohs(header->wlen);
caplen = rlen - dag_record_size;
}
case ERF_TYPE_MC_ATM:
if ((header->type & 0x7f) == ERF_TYPE_MC_ATM) {
caplen = packet_len = ATM_CELL_SIZE;
dp+=4;
}
case ERF_TYPE_MC_AAL5:
if ((header->type & 0x7f) == ERF_TYPE_MC_AAL5) {
packet_len = ntohs(header->wlen);
caplen = rlen - dag_record_size - 4;
dp+=4;
}

caplen -= (8 * num_ext_hdr);

if ((header->type & 0x7f) == ERF_TYPE_ATM) {
caplen = packet_len = ATM_CELL_SIZE;
}
if (p->linktype == DLT_SUNATM) {
struct sunatm_hdr *sunatm = (struct sunatm_hdr *)dp;
unsigned long rawatm;

rawatm = ntohl(*((unsigned long *)dp));
sunatm->vci = htons((rawatm >> 4) & 0xffff);
sunatm->vpi = (rawatm >> 20) & 0x00ff;
sunatm->flags = ((header->flags.iface & 1) ? 0x80 : 0x00) |
((sunatm->vpi == 0 && sunatm->vci == htons(5)) ? 6 :
((sunatm->vpi == 0 && sunatm->vci == htons(16)) ? 5 :
((dp[ATM_HDR_SIZE] == 0xaa &&
dp[ATM_HDR_SIZE+1] == 0xaa &&
dp[ATM_HDR_SIZE+2] == 0x03) ? 2 : 1)));

} else if (p->linktype == DLT_ATM_RFC1483) {
packet_len -= ATM_HDR_SIZE;
caplen -= ATM_HDR_SIZE;
dp += ATM_HDR_SIZE;
} else
continue;
break;

case ERF_TYPE_COLOR_HASH_ETH:
case ERF_TYPE_DSM_COLOR_ETH:
case ERF_TYPE_COLOR_ETH:
case ERF_TYPE_ETH:
if ((p->linktype != DLT_EN10MB) &&
(p->linktype != DLT_DOCSIS))
continue;
packet_len = ntohs(header->wlen);
packet_len -= (pd->dag_fcs_bits >> 3);
caplen = rlen - dag_record_size - 2;

caplen -= (8 * num_ext_hdr);
if (caplen > packet_len) {
caplen = packet_len;
}
dp += 2;
break;

case ERF_TYPE_COLOR_HASH_POS:
case ERF_TYPE_DSM_COLOR_HDLC_POS:
case ERF_TYPE_COLOR_HDLC_POS:
case ERF_TYPE_HDLC_POS:
if ((p->linktype != DLT_CHDLC) &&
(p->linktype != DLT_PPP_SERIAL) &&
(p->linktype != DLT_FRELAY))
continue;
packet_len = ntohs(header->wlen);
packet_len -= (pd->dag_fcs_bits >> 3);
caplen = rlen - dag_record_size;

caplen -= (8 * num_ext_hdr);
if (caplen > packet_len) {
caplen = packet_len;
}
break;

case ERF_TYPE_COLOR_MC_HDLC_POS:
case ERF_TYPE_MC_HDLC:
if ((p->linktype != DLT_CHDLC) &&
(p->linktype != DLT_PPP_SERIAL) &&
(p->linktype != DLT_FRELAY) &&
(p->linktype != DLT_MTP2) &&
(p->linktype != DLT_MTP2_WITH_PHDR) &&
(p->linktype != DLT_LAPD))
continue;
packet_len = ntohs(header->wlen);
packet_len -= (pd->dag_fcs_bits >> 3);
caplen = rlen - dag_record_size - 4;

caplen -= (8 * num_ext_hdr);
if (caplen > packet_len) {
caplen = packet_len;
}

dp += 4;
#if defined(DLT_MTP2_WITH_PHDR)
if (p->linktype == DLT_MTP2_WITH_PHDR) {

caplen += MTP2_HDR_LEN;
packet_len += MTP2_HDR_LEN;

TempPkt[MTP2_SENT_OFFSET] = 0;
TempPkt[MTP2_ANNEX_A_USED_OFFSET] = MTP2_ANNEX_A_USED_UNKNOWN;
*(TempPkt+MTP2_LINK_NUMBER_OFFSET) = ((header->rec.mc_hdlc.mc_header>>16)&0x01);
*(TempPkt+MTP2_LINK_NUMBER_OFFSET+1) = ((header->rec.mc_hdlc.mc_header>>24)&0xff);
memcpy(TempPkt+MTP2_HDR_LEN, dp, caplen);
dp = TempPkt;
}
#endif
break;

case ERF_TYPE_IPV4:
if ((p->linktype != DLT_RAW) &&
(p->linktype != DLT_IPV4))
continue;
packet_len = ntohs(header->wlen);
caplen = rlen - dag_record_size;

caplen -= (8 * num_ext_hdr);
if (caplen > packet_len) {
caplen = packet_len;
}
break;

case ERF_TYPE_IPV6:
if ((p->linktype != DLT_RAW) &&
(p->linktype != DLT_IPV6))
continue;
packet_len = ntohs(header->wlen);
caplen = rlen - dag_record_size;

caplen -= (8 * num_ext_hdr);
if (caplen > packet_len) {
caplen = packet_len;
}
break;


case ERF_TYPE_MC_RAW:
case ERF_TYPE_MC_RAW_CHANNEL:
case ERF_TYPE_IP_COUNTER:
case ERF_TYPE_TCP_FLOW_COUNTER:
case ERF_TYPE_INFINIBAND:
case ERF_TYPE_RAW_LINK:
case ERF_TYPE_INFINIBAND_LINK:
default:



continue;
}

}

if (caplen > p->snapshot)
caplen = p->snapshot;


if ((p->fcode.bf_insns == NULL) || bpf_filter(p->fcode.bf_insns, dp, packet_len, caplen)) {


register unsigned long long ts;

if (IS_BIGENDIAN()) {
ts = SWAPLL(header->ts);
} else {
ts = header->ts;
}

switch (p->opt.tstamp_precision) {
case PCAP_TSTAMP_PRECISION_NANO:
ticks_per_second = 1000000000;
break;
case PCAP_TSTAMP_PRECISION_MICRO:
default:
ticks_per_second = 1000000;
break;

}
pcap_header.ts.tv_sec = ts >> 32;
ts = (ts & 0xffffffffULL) * ticks_per_second;
ts += 0x80000000;
pcap_header.ts.tv_usec = ts >> 32;
if (pcap_header.ts.tv_usec >= ticks_per_second) {
pcap_header.ts.tv_usec -= ticks_per_second;
pcap_header.ts.tv_sec++;
}


pcap_header.caplen = caplen;
pcap_header.len = packet_len;


pd->stat.ps_recv++;


callback(user, &pcap_header, dp);


processed++;
if (processed == cnt && !PACKET_COUNT_IS_UNLIMITED(cnt))
{

return cnt;
}
}
}

return processed;
}

static int
dag_inject(pcap_t *p, const void *buf _U_, size_t size _U_)
{
pcap_strlcpy(p->errbuf, "Sending packets isn't supported on DAG cards",
PCAP_ERRBUF_SIZE);
return (-1);
}












static int dag_activate(pcap_t* p)
{
struct pcap_dag *pd = p->priv;
char *s;
int n;
daginf_t* daginf;
char * newDev = NULL;
char * device = p->opt.device;
int ret;
dag_size_t mindata;
struct timeval maxwait;
struct timeval poll;

if (device == NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "device is NULL");
return PCAP_ERROR;
}


newDev = (char *)malloc(strlen(device) + 16);
if (newDev == NULL) {
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't allocate string for device name");
goto fail;
}


if (dag_parse_name(device, newDev, strlen(device) + 16, &pd->dag_stream) < 0) {






ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_parse_name");
goto fail;
}
device = newDev;

if (pd->dag_stream%2) {
ret = PCAP_ERROR;
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "dag_parse_name: tx (even numbered) streams not supported for capture");
goto fail;
}


if((pd->dag_ref = dag_config_init((char *)device)) == NULL) {



if (errno == ENOENT)
ret = PCAP_ERROR_NO_SUCH_DEVICE;
else if (errno == EPERM || errno == EACCES)
ret = PCAP_ERROR_PERM_DENIED;
else
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_config_init %s", device);
goto fail;
}

if((p->fd = dag_config_get_card_fd(pd->dag_ref)) < 0) {



ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_config_get_card_fd %s", device);
goto failclose;
}


if (dag_attach_stream64(p->fd, pd->dag_stream, 0, 0) < 0) {
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_attach_stream");
goto failclose;
}


pd->drop_attr = kNullAttributeUuid;
pd->dag_root = dag_config_get_root_component(pd->dag_ref);
if ( dag_component_get_subcomponent(pd->dag_root, kComponentStreamFeatures, 0) )
{
pd->drop_attr = dag_config_get_indexed_attribute_uuid(pd->dag_ref, kUint32AttributeStreamDropCount, pd->dag_stream/2);
}




if (dag_get_stream_poll64(p->fd, pd->dag_stream,
&mindata, &maxwait, &poll) < 0) {
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_get_stream_poll");
goto faildetach;
}





pd->required_select_timeout = poll;
p->required_select_timeout = &pd->required_select_timeout;









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;

if (p->opt.immediate) {



mindata = 0;
} else {




mindata = 65536;
}




maxwait.tv_sec = p->opt.timeout/1000;
maxwait.tv_usec = (p->opt.timeout%1000) * 1000;

if (dag_set_stream_poll64(p->fd, pd->dag_stream,
mindata, &maxwait, &poll) < 0) {
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_set_stream_poll");
goto faildetach;
}





#if 0



if (p->snapshot == 0 || p->snapshot > MAX_DAG_SNAPLEN) {
p->snapshot = MAX_DAG_SNAPLEN;
} else if (snaplen < MIN_DAG_SNAPLEN) {
p->snapshot = MIN_DAG_SNAPLEN;
}

#endif

if(dag_start_stream(p->fd, pd->dag_stream) < 0) {
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_start_stream %s", device);
goto faildetach;
}






pd->dag_mem_bottom = 0;
pd->dag_mem_top = 0;





daginf = dag_info(p->fd);
if ((0x4200 == daginf->device_code) || (0x4230 == daginf->device_code)) {

pd->dag_fcs_bits = 0;


p->linktype_ext = LT_FCS_DATALINK_EXT(0);
} else {



pd->dag_fcs_bits = 32;


if ((s = getenv("ERF_FCS_BITS")) != NULL) {
if ((n = atoi(s)) == 0 || n == 16 || n == 32) {
pd->dag_fcs_bits = n;
} else {
ret = PCAP_ERROR;
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"pcap_activate %s: bad ERF_FCS_BITS value (%d) in environment", device, n);
goto failstop;
}
}




if ((s = getenv("ERF_DONT_STRIP_FCS")) != NULL) {


p->linktype_ext = LT_FCS_DATALINK_EXT(pd->dag_fcs_bits/16);


pd->dag_fcs_bits = 0;
}
}

pd->dag_timeout = p->opt.timeout;

p->linktype = -1;
if (dag_get_datalink(p) < 0) {
ret = PCAP_ERROR;
goto failstop;
}

p->bufsize = 0;

if (new_pcap_dag(p) < 0) {
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "new_pcap_dag %s", device);
goto failstop;
}




p->selectable_fd = -1;

if (newDev != NULL) {
free((char *)newDev);
}

p->read_op = dag_read;
p->inject_op = dag_inject;
p->setfilter_op = dag_setfilter;
p->setdirection_op = NULL;
p->set_datalink_op = dag_set_datalink;
p->getnonblock_op = pcap_getnonblock_fd;
p->setnonblock_op = dag_setnonblock;
p->stats_op = dag_stats;
p->cleanup_op = dag_platform_cleanup;
pd->stat.ps_drop = 0;
pd->stat.ps_recv = 0;
pd->stat.ps_ifdrop = 0;
return 0;

failstop:
if (dag_stop_stream(p->fd, pd->dag_stream) < 0) {
fprintf(stderr,"dag_stop_stream: %s\n", strerror(errno));
}

faildetach:
if (dag_detach_stream(p->fd, pd->dag_stream) < 0)
fprintf(stderr,"dag_detach_stream: %s\n", strerror(errno));

failclose:
dag_config_dispose(pd->dag_ref);






p->fd = -1;
pd->dag_ref = NULL;
delete_pcap_dag(p);

fail:
pcap_cleanup_live_common(p);
if (newDev != NULL) {
free((char *)newDev);
}

return ret;
}

pcap_t *dag_create(const char *device, char *ebuf, int *is_ours)
{
const char *cp;
char *cpend;
long devnum;
pcap_t *p;
long stream = 0;


cp = strrchr(device, '/');
if (cp == NULL)
cp = device;

if (strncmp(cp, "dag", 3) != 0) {

*is_ours = 0;
return NULL;
}

cp += 3;
devnum = strtol(cp, &cpend, 10);
if (*cpend == ':') {

stream = strtol(++cpend, &cpend, 10);
}

if (cpend == cp || *cpend != '\0') {

*is_ours = 0;
return NULL;
}

if (devnum < 0 || devnum >= DAG_MAX_BOARDS) {

*is_ours = 0;
return NULL;
}

if (stream <0 || stream >= DAG_STREAM_MAX) {

*is_ours = 0;
return NULL;
}


*is_ours = 1;

p = pcap_create_common(ebuf, sizeof (struct pcap_dag));
if (p == NULL)
return NULL;

p->activate_op = dag_activate;








p->tstamp_precision_count = 2;
p->tstamp_precision_list = malloc(2 * sizeof(u_int));
if (p->tstamp_precision_list == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
pcap_close(p);
return NULL;
}
p->tstamp_precision_list[0] = PCAP_TSTAMP_PRECISION_MICRO;
p->tstamp_precision_list[1] = PCAP_TSTAMP_PRECISION_NANO;
return p;
}

static int
dag_stats(pcap_t *p, struct pcap_stat *ps) {
struct pcap_dag *pd = p->priv;
uint32_t stream_drop;
dag_err_t dag_error;








if(pd->drop_attr != kNullAttributeUuid) {



if ((dag_error = dag_config_get_uint32_attribute_ex(pd->dag_ref, pd->drop_attr, &stream_drop) == kDagErrNone)) {
pd->stat.ps_drop = stream_drop;
} else {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "reading stream drop attribute: %s",
dag_config_strerror(dag_error));
return -1;
}
}

*ps = pd->stat;

return 0;
}




int
dag_findalldevs(pcap_if_list_t *devlistp, char *errbuf)
{
char name[12];
int c;
char dagname[DAGNAME_BUFSIZE];
int dagstream;
int dagfd;
dag_card_inf_t *inf;
char *description;
int stream, rxstreams;


for (c = 0; c < DAG_MAX_BOARDS; c++) {
pcap_snprintf(name, 12, "dag%d", c);
if (-1 == dag_parse_name(name, dagname, DAGNAME_BUFSIZE, &dagstream))
{
(void) pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"dag: device name %s can't be parsed", name);
return (-1);
}
if ( (dagfd = dag_open(dagname)) >= 0 ) {
description = NULL;
if ((inf = dag_pciinfo(dagfd)))
description = dag_device_name(inf->device_code, 1);









if (add_dev(devlistp, name, 0, description, errbuf) == NULL) {



return (-1);
}
rxstreams = dag_rx_get_stream_count(dagfd);
for(stream=0;stream<DAG_STREAM_MAX;stream+=2) {
if (0 == dag_attach_stream64(dagfd, stream, 0, 0)) {
dag_detach_stream(dagfd, stream);

pcap_snprintf(name, 10, "dag%d:%d", c, stream);
if (add_dev(devlistp, name, 0, description, errbuf) == NULL) {



return (-1);
}

rxstreams--;
if(rxstreams <= 0) {
break;
}
}
}
dag_close(dagfd);
}

}
return (0);
}






static int
dag_setfilter(pcap_t *p, struct bpf_program *fp)
{
if (!p)
return -1;
if (!fp) {
strncpy(p->errbuf, "setfilter: No filter specified",
sizeof(p->errbuf));
return -1;
}



if (install_bpf_program(p, fp) < 0)
return -1;

return (0);
}

static int
dag_set_datalink(pcap_t *p, int dlt)
{
p->linktype = dlt;

return (0);
}

static int
dag_setnonblock(pcap_t *p, int nonblock)
{
struct pcap_dag *pd = p->priv;
dag_size_t mindata;
struct timeval maxwait;
struct timeval poll;







if (pcap_setnonblock_fd(p, nonblock) < 0)
return (-1);

if (dag_get_stream_poll64(p->fd, pd->dag_stream,
&mindata, &maxwait, &poll) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_get_stream_poll");
return -1;
}





if(nonblock)
mindata = 0;
else
mindata = 65536;

if (dag_set_stream_poll64(p->fd, pd->dag_stream,
mindata, &maxwait, &poll) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "dag_set_stream_poll");
return -1;
}

if (nonblock) {
pd->dag_flags |= DAGF_NONBLOCK;
} else {
pd->dag_flags &= ~DAGF_NONBLOCK;
}
return (0);
}

static int
dag_get_datalink(pcap_t *p)
{
struct pcap_dag *pd = p->priv;
int index=0, dlt_index=0;
uint8_t types[255];

memset(types, 0, 255);

if (p->dlt_list == NULL && (p->dlt_list = malloc(255*sizeof(*(p->dlt_list)))) == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "malloc");
return (-1);
}

p->linktype = 0;

#if defined(HAVE_DAG_GET_STREAM_ERF_TYPES)

if (dag_get_stream_erf_types(p->fd, pd->dag_stream, types, 255) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "dag_get_stream_erf_types");
return (-1);
}

while (types[index]) {

#elif defined HAVE_DAG_GET_ERF_TYPES

if (dag_get_erf_types(p->fd, types, 255) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "dag_get_erf_types");
return (-1);
}

while (types[index]) {
#else

types[index] = dag_linktype(p->fd);

{
#endif
switch((types[index] & 0x7f)) {

case ERF_TYPE_HDLC_POS:
case ERF_TYPE_COLOR_HDLC_POS:
case ERF_TYPE_DSM_COLOR_HDLC_POS:
case ERF_TYPE_COLOR_HASH_POS:

if (p->dlt_list != NULL) {
p->dlt_list[dlt_index++] = DLT_CHDLC;
p->dlt_list[dlt_index++] = DLT_PPP_SERIAL;
p->dlt_list[dlt_index++] = DLT_FRELAY;
}
if(!p->linktype)
p->linktype = DLT_CHDLC;
break;

case ERF_TYPE_ETH:
case ERF_TYPE_COLOR_ETH:
case ERF_TYPE_DSM_COLOR_ETH:
case ERF_TYPE_COLOR_HASH_ETH:










if (p->dlt_list != NULL) {
p->dlt_list[dlt_index++] = DLT_EN10MB;
p->dlt_list[dlt_index++] = DLT_DOCSIS;
}
if(!p->linktype)
p->linktype = DLT_EN10MB;
break;

case ERF_TYPE_ATM:
case ERF_TYPE_AAL5:
case ERF_TYPE_MC_ATM:
case ERF_TYPE_MC_AAL5:
if (p->dlt_list != NULL) {
p->dlt_list[dlt_index++] = DLT_ATM_RFC1483;
p->dlt_list[dlt_index++] = DLT_SUNATM;
}
if(!p->linktype)
p->linktype = DLT_ATM_RFC1483;
break;

case ERF_TYPE_COLOR_MC_HDLC_POS:
case ERF_TYPE_MC_HDLC:
if (p->dlt_list != NULL) {
p->dlt_list[dlt_index++] = DLT_CHDLC;
p->dlt_list[dlt_index++] = DLT_PPP_SERIAL;
p->dlt_list[dlt_index++] = DLT_FRELAY;
p->dlt_list[dlt_index++] = DLT_MTP2;
p->dlt_list[dlt_index++] = DLT_MTP2_WITH_PHDR;
p->dlt_list[dlt_index++] = DLT_LAPD;
}
if(!p->linktype)
p->linktype = DLT_CHDLC;
break;

case ERF_TYPE_IPV4:
if (p->dlt_list != NULL) {
p->dlt_list[dlt_index++] = DLT_RAW;
p->dlt_list[dlt_index++] = DLT_IPV4;
}
if(!p->linktype)
p->linktype = DLT_RAW;
break;

case ERF_TYPE_IPV6:
if (p->dlt_list != NULL) {
p->dlt_list[dlt_index++] = DLT_RAW;
p->dlt_list[dlt_index++] = DLT_IPV6;
}
if(!p->linktype)
p->linktype = DLT_RAW;
break;

case ERF_TYPE_LEGACY:
case ERF_TYPE_MC_RAW:
case ERF_TYPE_MC_RAW_CHANNEL:
case ERF_TYPE_IP_COUNTER:
case ERF_TYPE_TCP_FLOW_COUNTER:
case ERF_TYPE_INFINIBAND:
case ERF_TYPE_RAW_LINK:
case ERF_TYPE_INFINIBAND_LINK:
case ERF_TYPE_META:
default:


break;

}
index++;
}

p->dlt_list[dlt_index++] = DLT_ERF;

p->dlt_count = dlt_index;

if(!p->linktype)
p->linktype = DLT_ERF;

return p->linktype;
}

#if defined(DAG_ONLY)








int
pcap_platform_finddevs(pcap_if_list_t *devlistp _U_, char *errbuf)
{
return (0);
}




pcap_t *
pcap_create_interface(const char *device, char *errbuf)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"This version of libpcap only supports DAG cards");
return NULL;
}




const char *
pcap_lib_version(void)
{
return (PCAP_VERSION_STRING " (DAG-only)");
}
#endif
