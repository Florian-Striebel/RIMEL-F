
































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <errno.h>
#define PCAP_DONT_INCLUDE_PCAP_BPF_H
#include <Packet32.h>
#include <pcap-int.h>
#include <pcap/dlt.h>



#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
#include <ddk/ntddndis.h>
#include <ddk/ndis.h>
#else
#include <ntddndis.h>
#endif

#if defined(HAVE_DAG_API)
#include <dagnew.h>
#include <dagapi.h>
#endif

static int pcap_setfilter_npf(pcap_t *, struct bpf_program *);
static int pcap_setfilter_win32_dag(pcap_t *, struct bpf_program *);
static int pcap_getnonblock_npf(pcap_t *);
static int pcap_setnonblock_npf(pcap_t *, int);


#define WIN32_DEFAULT_USER_BUFFER_SIZE 256000


#define WIN32_DEFAULT_KERNEL_BUFFER_SIZE 1000000


#define SWAPS(_X) ((_X & 0xff) << 8) | (_X >> 8)




struct pcap_win {
ADAPTER *adapter;
int nonblock;
int rfmon_selfstart;
int filtering_in_kernel;

#if defined(HAVE_DAG_API)
int dag_fcs_bits;
#endif

#if defined(ENABLE_REMOTE)
int samp_npkt;
struct timeval samp_time;
#endif
};






#if !defined(HAVE_NPCAP_PACKET_API)
static int
PacketIsMonitorModeSupported(PCHAR AdapterName _U_)
{



return (0);
}

static int
PacketSetMonitorMode(PCHAR AdapterName _U_, int mode _U_)
{





return (0);
}

static int
PacketGetMonitorMode(PCHAR AdapterName _U_)
{





return (-1);
}
#endif











#define NDIS_STATUS_INVALID_OID 0xc0010017
#define NDIS_STATUS_NOT_SUPPORTED 0xc00000bb
#define NDIS_STATUS_NOT_RECOGNIZED 0x00010001

static int
oid_get_request(ADAPTER *adapter, bpf_u_int32 oid, void *data, size_t *lenp,
char *errbuf)
{
PACKET_OID_DATA *oid_data_arg;








oid_data_arg = malloc(sizeof (PACKET_OID_DATA) + *lenp);
if (oid_data_arg == NULL) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"Couldn't allocate argument buffer for PacketRequest");
return (PCAP_ERROR);
}




oid_data_arg->Oid = oid;
oid_data_arg->Length = (ULONG)(*lenp);
if (!PacketRequest(adapter, FALSE, oid_data_arg)) {
pcap_fmt_errmsg_for_win32_err(errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "Error calling PacketRequest");
free(oid_data_arg);
return (-1);
}




*lenp = oid_data_arg->Length;




memcpy(data, oid_data_arg->Data, *lenp);
free(oid_data_arg);
return (0);
}

static int
pcap_stats_npf(pcap_t *p, struct pcap_stat *ps)
{
struct pcap_win *pw = p->priv;
struct bpf_stat bstats;














if (!PacketGetStats(pw->adapter, &bstats)) {
pcap_fmt_errmsg_for_win32_err(p->errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "PacketGetStats error");
return (-1);
}
ps->ps_recv = bstats.bs_recv;
ps->ps_drop = bstats.bs_drop;





#if 0
ps->ps_ifdrop = bstats.ps_ifdrop;
#else
ps->ps_ifdrop = 0;
#endif

return (0);
}






















struct pcap_stat *
pcap_stats_ex_npf(pcap_t *p, int *pcap_stat_size)
{
struct pcap_win *pw = p->priv;
struct bpf_stat bstats;

*pcap_stat_size = sizeof (p->stat);








if (!PacketGetStatsEx(pw->adapter, &bstats)) {
pcap_fmt_errmsg_for_win32_err(p->errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "PacketGetStatsEx error");
return (NULL);
}
p->stat.ps_recv = bstats.bs_recv;
p->stat.ps_drop = bstats.bs_drop;
p->stat.ps_ifdrop = bstats.ps_ifdrop;
#if defined(ENABLE_REMOTE)
p->stat.ps_capt = bstats.bs_capt;
#endif
return (&p->stat);
}


static int
pcap_setbuff_npf(pcap_t *p, int dim)
{
struct pcap_win *pw = p->priv;

if(PacketSetBuff(pw->adapter,dim)==FALSE)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "driver error: not enough memory to allocate the kernel buffer");
return (-1);
}
return (0);
}


static int
pcap_setmode_npf(pcap_t *p, int mode)
{
struct pcap_win *pw = p->priv;

if(PacketSetMode(pw->adapter,mode)==FALSE)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "driver error: working mode not recognized");
return (-1);
}

return (0);
}


static int
pcap_setmintocopy_npf(pcap_t *p, int size)
{
struct pcap_win *pw = p->priv;

if(PacketSetMinToCopy(pw->adapter, size)==FALSE)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "driver error: unable to set the requested mintocopy size");
return (-1);
}
return (0);
}

static HANDLE
pcap_getevent_npf(pcap_t *p)
{
struct pcap_win *pw = p->priv;

return (PacketGetReadEvent(pw->adapter));
}

static int
pcap_oid_get_request_npf(pcap_t *p, bpf_u_int32 oid, void *data, size_t *lenp)
{
struct pcap_win *pw = p->priv;

return (oid_get_request(pw->adapter, oid, data, lenp, p->errbuf));
}

static int
pcap_oid_set_request_npf(pcap_t *p, bpf_u_int32 oid, const void *data,
size_t *lenp)
{
struct pcap_win *pw = p->priv;
PACKET_OID_DATA *oid_data_arg;








oid_data_arg = malloc(sizeof (PACKET_OID_DATA) + *lenp);
if (oid_data_arg == NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Couldn't allocate argument buffer for PacketRequest");
return (PCAP_ERROR);
}

oid_data_arg->Oid = oid;
oid_data_arg->Length = (ULONG)(*lenp);
memcpy(oid_data_arg->Data, data, *lenp);
if (!PacketRequest(pw->adapter, TRUE, oid_data_arg)) {
pcap_fmt_errmsg_for_win32_err(p->errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "Error calling PacketRequest");
free(oid_data_arg);
return (PCAP_ERROR);
}




*lenp = oid_data_arg->Length;




free(oid_data_arg);
return (0);
}

static u_int
pcap_sendqueue_transmit_npf(pcap_t *p, pcap_send_queue *queue, int sync)
{
struct pcap_win *pw = p->priv;
u_int res;

if (pw->adapter==NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Cannot transmit a queue to an offline capture or to a TurboCap port");
return (0);
}

res = PacketSendPackets(pw->adapter,
queue->buffer,
queue->len,
(BOOLEAN)sync);

if(res != queue->len){
pcap_fmt_errmsg_for_win32_err(p->errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "Error opening adapter");
}

return (res);
}

static int
pcap_setuserbuffer_npf(pcap_t *p, int size)
{
unsigned char *new_buff;

if (size<=0) {

pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Error: invalid size %d",size);
return (-1);
}


new_buff=(unsigned char*)malloc(sizeof(char)*size);

if (!new_buff) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Error: not enough memory");
return (-1);
}

free(p->buffer);

p->buffer=new_buff;
p->bufsize=size;

return (0);
}

static int
pcap_live_dump_npf(pcap_t *p, char *filename, int maxsize, int maxpacks)
{
struct pcap_win *pw = p->priv;
BOOLEAN res;


res = PacketSetMode(pw->adapter, PACKET_MODE_DUMP);
if(res == FALSE){
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Error setting dump mode");
return (-1);
}


res = PacketSetDumpName(pw->adapter, filename, (int)strlen(filename));
if(res == FALSE){
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Error setting kernel dump file name");
return (-1);
}


res = PacketSetDumpLimits(pw->adapter, maxsize, maxpacks);
if(res == FALSE) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Error setting dump limit");
return (-1);
}

return (0);
}

static int
pcap_live_dump_ended_npf(pcap_t *p, int sync)
{
struct pcap_win *pw = p->priv;

return (PacketIsDumpEnded(pw->adapter, (BOOLEAN)sync));
}

static PAirpcapHandle
pcap_get_airpcap_handle_npf(pcap_t *p)
{
#if defined(HAVE_AIRPCAP_API)
struct pcap_win *pw = p->priv;

return (PacketGetAirPcapHandle(pw->adapter));
#else
return (NULL);
#endif
}

static int
pcap_read_npf(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
PACKET Packet;
int cc;
int n = 0;
register u_char *bp, *ep;
u_char *datap;
struct pcap_win *pw = p->priv;

cc = p->cc;
if (p->cc == 0) {



if (p->break_loop) {





p->break_loop = 0;
return (PCAP_ERROR_BREAK);
}













PacketInitPacket(&Packet, (BYTE *)p->buffer, p->bufsize);
if (!PacketReceivePacket(pw->adapter, &Packet, TRUE)) {




DWORD errcode = GetLastError();

if (errcode == ERROR_GEN_FAILURE) {














pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The interface disappeared");
} else {
pcap_fmt_errmsg_for_win32_err(p->errbuf,
PCAP_ERRBUF_SIZE, errcode,
"PacketReceivePacket error");
}
return (PCAP_ERROR);
}

cc = Packet.ulBytesReceived;

bp = p->buffer;
}
else
bp = p->bp;




#define bhp ((struct bpf_hdr *)bp)
ep = bp + cc;
for (;;) {
register int caplen, hdrlen;











if (p->break_loop) {
if (n == 0) {
p->break_loop = 0;
return (PCAP_ERROR_BREAK);
} else {
p->bp = bp;
p->cc = (int) (ep - bp);
return (n);
}
}
if (bp >= ep)
break;

caplen = bhp->bh_caplen;
hdrlen = bhp->bh_hdrlen;
datap = bp + hdrlen;










if (pw->filtering_in_kernel ||
p->fcode.bf_insns == NULL ||
bpf_filter(p->fcode.bf_insns, datap, bhp->bh_datalen, caplen)) {
#if defined(ENABLE_REMOTE)
switch (p->rmt_samp.method) {

case PCAP_SAMP_1_EVERY_N:
pw->samp_npkt = (pw->samp_npkt + 1) % p->rmt_samp.value;


if (pw->samp_npkt != 0) {
bp += Packet_WORDALIGN(caplen + hdrlen);
continue;
}
break;

case PCAP_SAMP_FIRST_AFTER_N_MS:
{
struct pcap_pkthdr *pkt_header = (struct pcap_pkthdr*) bp;





if (pkt_header->ts.tv_sec < pw->samp_time.tv_sec ||
(pkt_header->ts.tv_sec == pw->samp_time.tv_sec && pkt_header->ts.tv_usec < pw->samp_time.tv_usec)) {
bp += Packet_WORDALIGN(caplen + hdrlen);
continue;
}






pw->samp_time.tv_usec = pkt_header->ts.tv_usec + p->rmt_samp.value * 1000;
if (pw->samp_time.tv_usec > 1000000) {
pw->samp_time.tv_sec = pkt_header->ts.tv_sec + pw->samp_time.tv_usec / 1000000;
pw->samp_time.tv_usec = pw->samp_time.tv_usec % 1000000;
}
}
}
#endif




(*callback)(user, (struct pcap_pkthdr*)bp, datap);
bp += Packet_WORDALIGN(caplen + hdrlen);
if (++n >= cnt && !PACKET_COUNT_IS_UNLIMITED(cnt)) {
p->bp = bp;
p->cc = (int) (ep - bp);
return (n);
}
} else {



bp += Packet_WORDALIGN(caplen + hdrlen);
}
}
#undef bhp
p->cc = 0;
return (n);
}

#if defined(HAVE_DAG_API)
static int
pcap_read_win32_dag(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_win *pw = p->priv;
PACKET Packet;
u_char *dp = NULL;
int packet_len = 0, caplen = 0;
struct pcap_pkthdr pcap_header;
u_char *endofbuf;
int n = 0;
dag_record_t *header;
unsigned erf_record_len;
ULONGLONG ts;
int cc;
unsigned swt;
unsigned dfp = pw->adapter->DagFastProcess;

cc = p->cc;
if (cc == 0)
{












PacketInitPacket(&Packet, (BYTE *)p->buffer, p->bufsize);
if (!PacketReceivePacket(pw->adapter, &Packet, TRUE)) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "read error: PacketReceivePacket failed");
return (-1);
}

cc = Packet.ulBytesReceived;
if(cc == 0)

return (0);
header = (dag_record_t*)pw->adapter->DagBuffer;
}
else
header = (dag_record_t*)p->bp;

endofbuf = (char*)header + cc;




do
{
erf_record_len = SWAPS(header->rlen);
if((char*)header + erf_record_len > endofbuf)
break;


p->stat.ps_recv++;


dp = ((u_char *)header) + dag_record_size;


switch(header->type)
{
case TYPE_ATM:
packet_len = ATM_SNAPLEN;
caplen = ATM_SNAPLEN;
dp += 4;

break;

case TYPE_ETH:
swt = SWAPS(header->wlen);
packet_len = swt - (pw->dag_fcs_bits);
caplen = erf_record_len - dag_record_size - 2;
if (caplen > packet_len)
{
caplen = packet_len;
}
dp += 2;

break;

case TYPE_HDLC_POS:
swt = SWAPS(header->wlen);
packet_len = swt - (pw->dag_fcs_bits);
caplen = erf_record_len - dag_record_size;
if (caplen > packet_len)
{
caplen = packet_len;
}

break;
}

if(caplen > p->snapshot)
caplen = p->snapshot;










if (p->break_loop)
{
if (n == 0)
{
p->break_loop = 0;
return (-2);
}
else
{
p->bp = (char*)header;
p->cc = endofbuf - (char*)header;
return (n);
}
}

if(!dfp)
{

ts = header->ts;
pcap_header.ts.tv_sec = (int)(ts >> 32);
ts = (ts & 0xffffffffi64) * 1000000;
ts += 0x80000000;
pcap_header.ts.tv_usec = (int)(ts >> 32);
if (pcap_header.ts.tv_usec >= 1000000) {
pcap_header.ts.tv_usec -= 1000000;
pcap_header.ts.tv_sec++;
}
}


if (p->fcode.bf_insns)
{
if (bpf_filter(p->fcode.bf_insns, dp, packet_len, caplen) == 0)
{

header = (dag_record_t*)((char*)header + erf_record_len);
continue;
}
}


pcap_header.caplen = caplen;
pcap_header.len = packet_len;


(*callback)(user, &pcap_header, dp);


header = (dag_record_t*)((char*)header + erf_record_len);


if (++n >= cnt && !PACKET_COUNT_IS_UNLIMITED(cnt))
{
p->bp = (char*)header;
p->cc = endofbuf - (char*)header;
return (n);
}
}
while((u_char*)header < endofbuf);

return (1);
}
#endif


static int
pcap_inject_npf(pcap_t *p, const void *buf, size_t size)
{
struct pcap_win *pw = p->priv;
PACKET pkt;

PacketInitPacket(&pkt, (PVOID)buf, size);
if(PacketSendPacket(pw->adapter,&pkt,TRUE) == FALSE) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "send error: PacketSendPacket failed");
return (-1);
}






return ((int)size);
}

static void
pcap_cleanup_npf(pcap_t *p)
{
struct pcap_win *pw = p->priv;

if (pw->adapter != NULL) {
PacketCloseAdapter(pw->adapter);
pw->adapter = NULL;
}
if (pw->rfmon_selfstart)
{
PacketSetMonitorMode(p->opt.device, 0);
}
pcap_cleanup_live_common(p);
}

static int
pcap_activate_npf(pcap_t *p)
{
struct pcap_win *pw = p->priv;
NetType type;
int res;
int status = 0;

if (p->opt.rfmon) {



if (PacketGetMonitorMode(p->opt.device) == 1)
{
pw->rfmon_selfstart = 0;
}
else
{
if ((res = PacketSetMonitorMode(p->opt.device, 1)) != 1)
{
pw->rfmon_selfstart = 0;

if (res == 0)
{
return PCAP_ERROR_RFMON_NOTSUP;
}
else
{
return PCAP_ERROR;
}
}
else
{
pw->rfmon_selfstart = 1;
}
}
}


pcap_wsockinit();

pw->adapter = PacketOpenAdapter(p->opt.device);

if (pw->adapter == NULL)
{
DWORD errcode = GetLastError();




if (errcode == ERROR_BAD_UNIT) {



return (PCAP_ERROR_NO_SUCH_DEVICE);
} else {



pcap_fmt_errmsg_for_win32_err(p->errbuf, PCAP_ERRBUF_SIZE,
errcode, "Error opening adapter");
if (pw->rfmon_selfstart)
{
PacketSetMonitorMode(p->opt.device, 0);
}
return (PCAP_ERROR);
}
}


if(PacketGetNetType (pw->adapter,&type) == FALSE)
{
pcap_fmt_errmsg_for_win32_err(p->errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "Cannot determine the network type");
goto bad;
}


switch (type.LinkType)
{
case NdisMediumWan:
p->linktype = DLT_EN10MB;
break;

case NdisMedium802_3:
p->linktype = DLT_EN10MB;










p->dlt_list = (u_int *) malloc(sizeof(u_int) * 2);



if (p->dlt_list != NULL) {
p->dlt_list[0] = DLT_EN10MB;
p->dlt_list[1] = DLT_DOCSIS;
p->dlt_count = 2;
}
break;

case NdisMediumFddi:
p->linktype = DLT_FDDI;
break;

case NdisMedium802_5:
p->linktype = DLT_IEEE802;
break;

case NdisMediumArcnetRaw:
p->linktype = DLT_ARCNET;
break;

case NdisMediumArcnet878_2:
p->linktype = DLT_ARCNET;
break;

case NdisMediumAtm:
p->linktype = DLT_ATM_RFC1483;
break;

case NdisMediumCHDLC:
p->linktype = DLT_CHDLC;
break;

case NdisMediumPPPSerial:
p->linktype = DLT_PPP_SERIAL;
break;

case NdisMediumNull:
p->linktype = DLT_NULL;
break;

case NdisMediumBare80211:
p->linktype = DLT_IEEE802_11;
break;

case NdisMediumRadio80211:
p->linktype = DLT_IEEE802_11_RADIO;
break;

case NdisMediumPpi:
p->linktype = DLT_PPI;
break;

#if defined(NdisMediumWirelessWan)
case NdisMediumWirelessWan:
p->linktype = DLT_RAW;
break;
#endif

default:











p->linktype = DLT_EN10MB;
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Unknown NdisMedium value %d, defaulting to DLT_EN10MB",
type.LinkType);
status = PCAP_WARNING;
break;
}









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;


if (p->opt.promisc)
{

if (PacketSetHwFilter(pw->adapter,NDIS_PACKET_TYPE_PROMISCUOUS) == FALSE)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "failed to set hardware filter to promiscuous mode");
goto bad;
}
}
else
{






if (PacketSetHwFilter(pw->adapter,
NDIS_PACKET_TYPE_ALL_LOCAL |
NDIS_PACKET_TYPE_DIRECTED |
NDIS_PACKET_TYPE_BROADCAST |
NDIS_PACKET_TYPE_MULTICAST) == FALSE)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "failed to set hardware filter to non-promiscuous mode");
goto bad;
}
}


p->bufsize = WIN32_DEFAULT_USER_BUFFER_SIZE;

if(!(pw->adapter->Flags & INFO_FLAG_DAG_CARD))
{







if (p->opt.buffer_size == 0)
p->opt.buffer_size = WIN32_DEFAULT_KERNEL_BUFFER_SIZE;

if(PacketSetBuff(pw->adapter,p->opt.buffer_size)==FALSE)
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "driver error: not enough memory to allocate the kernel buffer");
goto bad;
}

p->buffer = malloc(p->bufsize);
if (p->buffer == NULL)
{
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
goto bad;
}

if (p->opt.immediate)
{

if(PacketSetMinToCopy(pw->adapter,0)==FALSE)
{
pcap_fmt_errmsg_for_win32_err(p->errbuf,
PCAP_ERRBUF_SIZE, GetLastError(),
"Error calling PacketSetMinToCopy");
goto bad;
}
}
else
{

if(PacketSetMinToCopy(pw->adapter,16000)==FALSE)
{
pcap_fmt_errmsg_for_win32_err(p->errbuf,
PCAP_ERRBUF_SIZE, GetLastError(),
"Error calling PacketSetMinToCopy");
goto bad;
}
}
} else {



#if defined(HAVE_DAG_API)



LONG status;
HKEY dagkey;
DWORD lptype;
DWORD lpcbdata;
int postype = 0;
char keyname[512];

pcap_snprintf(keyname, sizeof(keyname), "%s\\CardParams\\%s",
"SYSTEM\\CurrentControlSet\\Services\\DAG",
strstr(_strlwr(p->opt.device), "dag"));
do
{
status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_READ, &dagkey);
if(status != ERROR_SUCCESS)
break;

status = RegQueryValueEx(dagkey,
"PosType",
NULL,
&lptype,
(char*)&postype,
&lpcbdata);

if(status != ERROR_SUCCESS)
{
postype = 0;
}

RegCloseKey(dagkey);
}
while(FALSE);


p->snapshot = PacketSetSnapLen(pw->adapter, p->snapshot);



pw->dag_fcs_bits = pw->adapter->DagFcsLen;
#else



goto bad;
#endif
}

PacketSetReadTimeout(pw->adapter, p->opt.timeout);


if (p->opt.nocapture_local)
{
if (!PacketSetLoopbackBehavior(pw->adapter, NPF_DISABLE_LOOPBACK))
{
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Unable to disable the capture of loopback packets.");
goto bad;
}
}

#if defined(HAVE_DAG_API)
if(pw->adapter->Flags & INFO_FLAG_DAG_CARD)
{

p->read_op = pcap_read_win32_dag;
p->setfilter_op = pcap_setfilter_win32_dag;
}
else
{
#endif

p->read_op = pcap_read_npf;
p->setfilter_op = pcap_setfilter_npf;
#if defined(HAVE_DAG_API)
}
#endif
p->setdirection_op = NULL;

p->inject_op = pcap_inject_npf;
p->set_datalink_op = NULL;
p->getnonblock_op = pcap_getnonblock_npf;
p->setnonblock_op = pcap_setnonblock_npf;
p->stats_op = pcap_stats_npf;
p->stats_ex_op = pcap_stats_ex_npf;
p->setbuff_op = pcap_setbuff_npf;
p->setmode_op = pcap_setmode_npf;
p->setmintocopy_op = pcap_setmintocopy_npf;
p->getevent_op = pcap_getevent_npf;
p->oid_get_request_op = pcap_oid_get_request_npf;
p->oid_set_request_op = pcap_oid_set_request_npf;
p->sendqueue_transmit_op = pcap_sendqueue_transmit_npf;
p->setuserbuffer_op = pcap_setuserbuffer_npf;
p->live_dump_op = pcap_live_dump_npf;
p->live_dump_ended_op = pcap_live_dump_ended_npf;
p->get_airpcap_handle_op = pcap_get_airpcap_handle_npf;
p->cleanup_op = pcap_cleanup_npf;












p->handle = pw->adapter->hFile;

return (status);
bad:
pcap_cleanup_npf(p);
return (PCAP_ERROR);
}




static int
pcap_can_set_rfmon_npf(pcap_t *p)
{
return (PacketIsMonitorModeSupported(p->opt.device) == 1);
}

pcap_t *
pcap_create_interface(const char *device _U_, char *ebuf)
{
pcap_t *p;

p = pcap_create_common(ebuf, sizeof(struct pcap_win));
if (p == NULL)
return (NULL);

p->activate_op = pcap_activate_npf;
p->can_set_rfmon_op = pcap_can_set_rfmon_npf;
return (p);
}

static int
pcap_setfilter_npf(pcap_t *p, struct bpf_program *fp)
{
struct pcap_win *pw = p->priv;

if(PacketSetBpf(pw->adapter,fp)==FALSE){































if (install_bpf_program(p, fp) < 0)
return (-1);
pw->filtering_in_kernel = 0;
return (0);
}




pw->filtering_in_kernel = 1;







p->cc = 0;
return (0);
}




static int
pcap_setfilter_win32_dag(pcap_t *p, struct bpf_program *fp) {

if(!fp)
{
pcap_strlcpy(p->errbuf, "setfilter: No filter specified", sizeof(p->errbuf));
return (-1);
}


if (install_bpf_program(p, fp) < 0)
return (-1);

return (0);
}

static int
pcap_getnonblock_npf(pcap_t *p)
{
struct pcap_win *pw = p->priv;






return (pw->nonblock);
}

static int
pcap_setnonblock_npf(pcap_t *p, int nonblock)
{
struct pcap_win *pw = p->priv;
int newtimeout;

if (nonblock) {




newtimeout = -1;
} else {








newtimeout = p->opt.timeout;
}
if (!PacketSetReadTimeout(pw->adapter, newtimeout)) {
pcap_fmt_errmsg_for_win32_err(p->errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "PacketSetReadTimeout");
return (-1);
}
pw->nonblock = (newtimeout == -1);
return (0);
}

static int
pcap_add_if_npf(pcap_if_list_t *devlistp, char *name, bpf_u_int32 flags,
const char *description, char *errbuf)
{
pcap_if_t *curdev;
npf_if_addr if_addrs[MAX_NETWORK_ADDRESSES];
LONG if_addr_size;
int res = 0;

if_addr_size = MAX_NETWORK_ADDRESSES;




curdev = add_dev(devlistp, name, flags, description, errbuf);
if (curdev == NULL) {



return (-1);
}




if (!PacketGetNetInfoEx((void *)name, if_addrs, &if_addr_size)) {









return (0);
}




while (if_addr_size-- > 0) {




res = add_addr_to_dev(curdev,
(struct sockaddr *)&if_addrs[if_addr_size].IPAddress,
sizeof (struct sockaddr_storage),
(struct sockaddr *)&if_addrs[if_addr_size].SubnetMask,
sizeof (struct sockaddr_storage),
(struct sockaddr *)&if_addrs[if_addr_size].Broadcast,
sizeof (struct sockaddr_storage),
NULL,
0,
errbuf);
if (res == -1) {



break;
}
}

return (res);
}

static int
get_if_flags(const char *name, bpf_u_int32 *flags, char *errbuf)
{
char *name_copy;
ADAPTER *adapter;
int status;
size_t len;
NDIS_HARDWARE_STATUS hardware_status;
#if defined(OID_GEN_PHYSICAL_MEDIUM)
NDIS_PHYSICAL_MEDIUM phys_medium;
bpf_u_int32 gen_physical_medium_oids[] = {
#if defined(OID_GEN_PHYSICAL_MEDIUM_EX)
OID_GEN_PHYSICAL_MEDIUM_EX,
#endif
OID_GEN_PHYSICAL_MEDIUM
};
#define N_GEN_PHYSICAL_MEDIUM_OIDS (sizeof gen_physical_medium_oids / sizeof gen_physical_medium_oids[0])
size_t i;
#endif
#if defined(OID_GEN_LINK_STATE)
NDIS_LINK_STATE link_state;
#endif
int connect_status;

if (*flags & PCAP_IF_LOOPBACK) {





*flags |= PCAP_IF_UP | PCAP_IF_RUNNING | PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE;
return (0);
}








name_copy = strdup(name);
adapter = PacketOpenAdapter(name_copy);
free(name_copy);
if (adapter == NULL) {



return (0);
}

#if defined(HAVE_AIRPCAP_API)




if (PacketGetAirPcapHandle(adapter)) {



*flags = PCAP_IF_UP | PCAP_IF_RUNNING;




*flags |= PCAP_IF_WIRELESS;




*flags |= PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE;
PacketCloseAdapter(adapter);
return (0);
}
#endif





len = sizeof (hardware_status);
status = oid_get_request(adapter, OID_GEN_HARDWARE_STATUS,
&hardware_status, &len, errbuf);
if (status == 0) {
switch (hardware_status) {

case NdisHardwareStatusReady:




*flags |= PCAP_IF_UP | PCAP_IF_RUNNING;
break;

case NdisHardwareStatusInitializing:
case NdisHardwareStatusReset:




*flags |= PCAP_IF_UP;
break;

case NdisHardwareStatusClosing:
case NdisHardwareStatusNotReady:




break;
}
} else {




*flags |= PCAP_IF_UP | PCAP_IF_RUNNING;
}




#if defined(OID_GEN_PHYSICAL_MEDIUM)



for (i = 0; i < N_GEN_PHYSICAL_MEDIUM_OIDS; i++) {
len = sizeof (phys_medium);
status = oid_get_request(adapter, gen_physical_medium_oids[i],
&phys_medium, &len, errbuf);
if (status == 0) {



break;
}






}
if (status == 0) {



switch (phys_medium) {

case NdisPhysicalMediumWirelessLan:
case NdisPhysicalMediumWirelessWan:
case NdisPhysicalMediumNative802_11:
case NdisPhysicalMediumBluetooth:
case NdisPhysicalMediumUWB:
case NdisPhysicalMediumIrda:



*flags |= PCAP_IF_WIRELESS;
break;

default:



break;
}
}
#endif




#if defined(OID_GEN_LINK_STATE)
len = sizeof(link_state);
status = oid_get_request(adapter, OID_GEN_LINK_STATE, &link_state,
&len, errbuf);
if (status == 0) {




switch (link_state.MediaConnectState) {

case MediaConnectStateConnected:



*flags |= PCAP_IF_CONNECTION_STATUS_CONNECTED;
break;

case MediaConnectStateDisconnected:



*flags |= PCAP_IF_CONNECTION_STATUS_DISCONNECTED;
break;
}
}
#else



status = -1;
#endif
if (status == -1) {




status = oid_get_request(adapter, OID_GEN_MEDIA_CONNECT_STATUS,
&connect_status, &len, errbuf);
if (status == 0) {
switch (connect_status) {

case NdisMediaStateConnected:



*flags |= PCAP_IF_CONNECTION_STATUS_CONNECTED;
break;

case NdisMediaStateDisconnected:



*flags |= PCAP_IF_CONNECTION_STATUS_DISCONNECTED;
break;
}
}
}
PacketCloseAdapter(adapter);
return (0);
}

int
pcap_platform_finddevs(pcap_if_list_t *devlistp, char *errbuf)
{
int ret = 0;
const char *desc;
char *AdaptersName;
ULONG NameLength;
char *name;


















NameLength = 0;
if (!PacketGetAdapterNames(NULL, &NameLength))
{
DWORD last_error = GetLastError();

if (last_error != ERROR_INSUFFICIENT_BUFFER)
{
pcap_fmt_errmsg_for_win32_err(errbuf, PCAP_ERRBUF_SIZE,
last_error, "PacketGetAdapterNames");
return (-1);
}
}

if (NameLength <= 0)
return 0;
AdaptersName = (char*) malloc(NameLength);
if (AdaptersName == NULL)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Cannot allocate enough memory to list the adapters.");
return (-1);
}

if (!PacketGetAdapterNames(AdaptersName, &NameLength)) {
pcap_fmt_errmsg_for_win32_err(errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "PacketGetAdapterNames");
free(AdaptersName);
return (-1);
}













desc = &AdaptersName[0];
while (*desc != '\0' || *(desc + 1) != '\0')
desc++;







desc += 2;




name = &AdaptersName[0];
while (*name != '\0') {
bpf_u_int32 flags = 0;
#if defined(HAVE_PACKET_IS_LOOPBACK_ADAPTER)



if (PacketIsLoopbackAdapter(name)) {

flags |= PCAP_IF_LOOPBACK;
}
#endif



if (get_if_flags(name, &flags, errbuf) == -1) {



ret = -1;
break;
}




if (pcap_add_if_npf(devlistp, name, flags, desc,
errbuf) == -1) {



ret = -1;
break;
}
name += strlen(name) + 1;
desc += strlen(desc) + 1;
}

free(AdaptersName);
return (ret);
}










#define ADAPTERSNAME_LEN 8192
char *
pcap_lookupdev(char *errbuf)
{
DWORD dwVersion;
DWORD dwWindowsMajorVersion;

#pragma warning (push)
#pragma warning (disable: 4996)
dwVersion = GetVersion();
#pragma warning (pop)
dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

if (dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4) {



ULONG NameLength = ADAPTERSNAME_LEN;
static char AdaptersName[ADAPTERSNAME_LEN];

if (PacketGetAdapterNames(AdaptersName,&NameLength) )
return (AdaptersName);
else
return NULL;
} else {




ULONG NameLength = ADAPTERSNAME_LEN;
static WCHAR AdaptersName[ADAPTERSNAME_LEN];
size_t BufferSpaceLeft;
char *tAstr;
WCHAR *Unameptr;
char *Adescptr;
size_t namelen, i;
WCHAR *TAdaptersName = (WCHAR*)malloc(ADAPTERSNAME_LEN * sizeof(WCHAR));
int NAdapts = 0;

if(TAdaptersName == NULL)
{
(void)pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "memory allocation failure");
return NULL;
}

if ( !PacketGetAdapterNames((PTSTR)TAdaptersName,&NameLength) )
{
pcap_fmt_errmsg_for_win32_err(errbuf, PCAP_ERRBUF_SIZE,
GetLastError(), "PacketGetAdapterNames");
free(TAdaptersName);
return NULL;
}


BufferSpaceLeft = ADAPTERSNAME_LEN * sizeof(WCHAR);
tAstr = (char*)TAdaptersName;
Unameptr = AdaptersName;




do {




namelen = strlen(tAstr) + 1;





if (BufferSpaceLeft < namelen * sizeof(WCHAR)) {



goto quit;
}
BufferSpaceLeft -= namelen * sizeof(WCHAR);






for (i = 0; i < namelen; i++)
*Unameptr++ = *tAstr++;




NAdapts++;
} while (namelen != 1);





Adescptr = (char *)Unameptr;
while(NAdapts--)
{
size_t desclen;

desclen = strlen(tAstr) + 1;





if (BufferSpaceLeft < desclen) {



goto quit;
}






memcpy(Adescptr, tAstr, desclen);
Adescptr += desclen;
tAstr += desclen;
BufferSpaceLeft -= desclen;
}

quit:
free(TAdaptersName);
return (char *)(AdaptersName);
}
}









int
pcap_lookupnet(const char *device, bpf_u_int32 *netp, bpf_u_int32 *maskp,
char *errbuf)
{




npf_if_addr if_addrs[MAX_NETWORK_ADDRESSES];
LONG if_addr_size = MAX_NETWORK_ADDRESSES;
struct sockaddr_in *t_addr;
LONG i;

if (!PacketGetNetInfoEx((void *)device, if_addrs, &if_addr_size)) {
*netp = *maskp = 0;
return (0);
}

for(i = 0; i < if_addr_size; i++)
{
if(if_addrs[i].IPAddress.ss_family == AF_INET)
{
t_addr = (struct sockaddr_in *) &(if_addrs[i].IPAddress);
*netp = t_addr->sin_addr.S_un.S_addr;
t_addr = (struct sockaddr_in *) &(if_addrs[i].SubnetMask);
*maskp = t_addr->sin_addr.S_un.S_addr;

*netp &= *maskp;
return (0);
}

}

*netp = *maskp = 0;
return (0);
}

static const char *pcap_lib_version_string;

#if defined(HAVE_VERSION_H)









#include "../../version.h"

static const char pcap_version_string[] =
WINPCAP_PRODUCT_NAME " version " WINPCAP_VER_STRING ", based on " PCAP_VERSION_STRING;

const char *
pcap_lib_version(void)
{
if (pcap_lib_version_string == NULL) {



char *packet_version_string = PacketGetVersion();

if (strcmp(WINPCAP_VER_STRING, packet_version_string) == 0) {





pcap_lib_version_string = pcap_version_string;
} else {







char *full_pcap_version_string;

if (pcap_asprintf(&full_pcap_version_string,
WINPCAP_PRODUCT_NAME " version " WINPCAP_VER_STRING " (packet.dll version %s), based on " PCAP_VERSION_STRING,
packet_version_string) != -1) {

pcap_lib_version_string = full_pcap_version_string;
}
}
}
return (pcap_lib_version_string);
}

#else





const char *
pcap_lib_version(void)
{
if (pcap_lib_version_string == NULL) {




char *full_pcap_version_string;

if (pcap_asprintf(&full_pcap_version_string,
PCAP_VERSION_STRING " (packet.dll version %s)",
PacketGetVersion()) != -1) {

pcap_lib_version_string = full_pcap_version_string;
}
}
return (pcap_lib_version_string);
}
#endif
