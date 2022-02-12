
































#if !defined(__RPCAP_PROTOCOL_H__)
#define __RPCAP_PROTOCOL_H__

#define RPCAP_DEFAULT_NETPORT "2002"

#define RPCAP_DEFAULT_NETPORT_ACTIVE "2003"
#define RPCAP_DEFAULT_NETADDR ""








































#define RPCAP_MIN_VERSION 0
#define RPCAP_MAX_VERSION 0







#if RPCAP_MIN_VERSION == 0
#define RPCAP_VERSION_IS_SUPPORTED(v) ((v) <= RPCAP_MAX_VERSION)

#else
#define RPCAP_VERSION_IS_SUPPORTED(v) ((v) >= RPCAP_MIN_VERSION && (v) <= RPCAP_MAX_VERSION)

#endif









#define RPCAP_HOSTLIST_SEP " ,;\n\r"




























typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef int int32;


struct rpcap_header
{
uint8 ver;
uint8 type;
uint16 value;
uint32 plen;
};








struct rpcap_authreply
{
uint8 minvers;
uint8 maxvers;
};


struct rpcap_findalldevs_if
{
uint16 namelen;
uint16 desclen;
uint32 flags;
uint16 naddr;
uint16 dummy;
};











































struct rpcap_sockaddr
{
uint16 family;
char data[128-2];
};




#define RPCAP_AF_INET 2
struct rpcap_sockaddr_in
{
uint16 family;
uint16 port;
uint32 addr;
uint8 zero[8];
};




#define RPCAP_AF_INET6 23
struct rpcap_sockaddr_in6
{
uint16 family;
uint16 port;
uint32 flowinfo;
uint8 addr[16];
uint32 scope_id;
};


struct rpcap_findalldevs_ifaddr
{
struct rpcap_sockaddr addr;
struct rpcap_sockaddr netmask;
struct rpcap_sockaddr broadaddr;
struct rpcap_sockaddr dstaddr;
};






struct rpcap_openreply
{
int32 linktype;
int32 tzoff;
};


struct rpcap_startcapreq
{
uint32 snaplen;
uint32 read_timeout;
uint16 flags;
uint16 portdata;
};


struct rpcap_startcapreply
{
int32 bufsize;
uint16 portdata;
uint16 dummy;
};







struct rpcap_pkthdr
{
uint32 timestamp_sec;
uint32 timestamp_usec;
uint32 caplen;
uint32 len;
uint32 npkt;
};


struct rpcap_filter
{
uint16 filtertype;
uint16 dummy;
uint32 nitems;
};


struct rpcap_filterbpf_insn
{
uint16 code;
uint8 jt;
uint8 jf;
int32 k;
};


struct rpcap_auth
{
uint16 type;
uint16 dummy;
uint16 slen1;
uint16 slen2;
};


struct rpcap_stats
{
uint32 ifrecv;
uint32 ifdrop;
uint32 krnldrop;
uint32 svrcapt;
};


struct rpcap_sampling
{
uint8 method;
uint8 dummy1;
uint16 dummy2;
uint32 value;
};







#define RPCAP_MSG_IS_REPLY 0x080

#define RPCAP_MSG_ERROR 1
#define RPCAP_MSG_FINDALLIF_REQ 2
#define RPCAP_MSG_OPEN_REQ 3
#define RPCAP_MSG_STARTCAP_REQ 4
#define RPCAP_MSG_UPDATEFILTER_REQ 5
#define RPCAP_MSG_CLOSE 6
#define RPCAP_MSG_PACKET 7
#define RPCAP_MSG_AUTH_REQ 8
#define RPCAP_MSG_STATS_REQ 9
#define RPCAP_MSG_ENDCAP_REQ 10
#define RPCAP_MSG_SETSAMPLING_REQ 11

#define RPCAP_MSG_FINDALLIF_REPLY (RPCAP_MSG_FINDALLIF_REQ | RPCAP_MSG_IS_REPLY)
#define RPCAP_MSG_OPEN_REPLY (RPCAP_MSG_OPEN_REQ | RPCAP_MSG_IS_REPLY)
#define RPCAP_MSG_STARTCAP_REPLY (RPCAP_MSG_STARTCAP_REQ | RPCAP_MSG_IS_REPLY)
#define RPCAP_MSG_UPDATEFILTER_REPLY (RPCAP_MSG_UPDATEFILTER_REQ | RPCAP_MSG_IS_REPLY)
#define RPCAP_MSG_AUTH_REPLY (RPCAP_MSG_AUTH_REQ | RPCAP_MSG_IS_REPLY)
#define RPCAP_MSG_STATS_REPLY (RPCAP_MSG_STATS_REQ | RPCAP_MSG_IS_REPLY)
#define RPCAP_MSG_ENDCAP_REPLY (RPCAP_MSG_ENDCAP_REQ | RPCAP_MSG_IS_REPLY)
#define RPCAP_MSG_SETSAMPLING_REPLY (RPCAP_MSG_SETSAMPLING_REQ | RPCAP_MSG_IS_REPLY)

#define RPCAP_STARTCAPREQ_FLAG_PROMISC 0x00000001
#define RPCAP_STARTCAPREQ_FLAG_DGRAM 0x00000002
#define RPCAP_STARTCAPREQ_FLAG_SERVEROPEN 0x00000004
#define RPCAP_STARTCAPREQ_FLAG_INBOUND 0x00000008
#define RPCAP_STARTCAPREQ_FLAG_OUTBOUND 0x00000010

#define RPCAP_UPDATEFILTER_BPF 1







#define PCAP_ERR_NETW 1
#define PCAP_ERR_INITTIMEOUT 2
#define PCAP_ERR_AUTH 3
#define PCAP_ERR_FINDALLIF 4
#define PCAP_ERR_NOREMOTEIF 5
#define PCAP_ERR_OPEN 6
#define PCAP_ERR_UPDATEFILTER 7
#define PCAP_ERR_GETSTATS 8
#define PCAP_ERR_READEX 9
#define PCAP_ERR_HOSTNOAUTH 10
#define PCAP_ERR_REMOTEACCEPT 11
#define PCAP_ERR_STARTCAPTURE 12
#define PCAP_ERR_ENDCAPTURE 13
#define PCAP_ERR_RUNTIMETIMEOUT 14
#define PCAP_ERR_SETSAMPLING 15
#define PCAP_ERR_WRONGMSG 16
#define PCAP_ERR_WRONGVER 17
#define PCAP_ERR_AUTH_FAILED 18
#define PCAP_ERR_TLS_REQUIRED 19
#define PCAP_ERR_AUTH_TYPE_NOTSUP 20





#define RPCAP_NETBUF_SIZE 64000







#include "sockutils.h"

extern void rpcap_createhdr(struct rpcap_header *header, uint8 ver, uint8 type, uint16 value, uint32 length);
extern const char *rpcap_msg_type_string(uint8 type);
extern int rpcap_senderror(SOCKET sock, uint8 ver, uint16 errcode, const char *error, char *errbuf);

#endif
