
































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "sockutils.h"
#include "portability.h"
#include "rpcap-protocol.h"
#include <pcap/pcap.h>


































int
rpcap_senderror(SOCKET sock, uint8 ver, unsigned short errcode, const char *error, char *errbuf)
{
char sendbuf[RPCAP_NETBUF_SIZE];
int sendbufidx = 0;
uint16 length;

length = (uint16)strlen(error);

if (length > PCAP_ERRBUF_SIZE)
length = PCAP_ERRBUF_SIZE;

rpcap_createhdr((struct rpcap_header *) sendbuf, ver, RPCAP_MSG_ERROR, errcode, length);

if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf, PCAP_ERRBUF_SIZE))
return -1;

if (sock_bufferize(error, length, sendbuf, &sendbufidx,
RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf, PCAP_ERRBUF_SIZE))
return -1;

if (sock_send(sock, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) < 0)
return -1;

return 0;
}



























void
rpcap_createhdr(struct rpcap_header *header, uint8 ver, uint8 type, uint16 value, uint32 length)
{
memset(header, 0, sizeof(struct rpcap_header));

header->ver = ver;
header->type = type;
header->value = htons(value);
header->plen = htonl(length);
}




static const char *requests[] =
{
NULL,
"RPCAP_MSG_ERROR",
"RPCAP_MSG_FINDALLIF_REQ",
"RPCAP_MSG_OPEN_REQ",
"RPCAP_MSG_STARTCAP_REQ",
"RPCAP_MSG_UPDATEFILTER_REQ",
"RPCAP_MSG_CLOSE",
"RPCAP_MSG_PACKET",
"RPCAP_MSG_AUTH_REQ",
"RPCAP_MSG_STATS_REQ",
"RPCAP_MSG_ENDCAP_REQ",
"RPCAP_MSG_SETSAMPLING_REQ",
};
#define NUM_REQ_TYPES (sizeof requests / sizeof requests[0])

static const char *replies[] =
{
NULL,
NULL,
"RPCAP_MSG_FINDALLIF_REPLY",
"RPCAP_MSG_OPEN_REPLY",
"RPCAP_MSG_STARTCAP_REPLY",
"RPCAP_MSG_UPDATEFILTER_REPLY",
NULL,
NULL,
"RPCAP_MSG_AUTH_REPLY",
"RPCAP_MSG_STATS_REPLY",
"RPCAP_MSG_ENDCAP_REPLY",
"RPCAP_MSG_SETSAMPLING_REPLY",
};
#define NUM_REPLY_TYPES (sizeof replies / sizeof replies[0])

const char *
rpcap_msg_type_string(uint8 type)
{
if (type & RPCAP_MSG_IS_REPLY) {
type &= ~RPCAP_MSG_IS_REPLY;
if (type >= NUM_REPLY_TYPES)
return NULL;
return replies[type];
} else {
if (type >= NUM_REQ_TYPES)
return NULL;
return requests[type];
}
}
