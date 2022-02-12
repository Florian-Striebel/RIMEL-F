
































#if !defined(__PCAP_RPCAP_INT_H__)
#define __PCAP_RPCAP_INT_H__

#include "pcap.h"
#include "sockutils.h"


























#define RPCAP_NETBUF_SIZE 64000






void rpcap_createhdr(struct rpcap_header *header, uint8 type, uint16 value, uint32 length);
int rpcap_senderror(SOCKET sock, char *error, unsigned short errcode, char *errbuf);

#endif
