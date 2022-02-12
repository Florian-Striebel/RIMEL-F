






























#if !defined(__PCAP_TC_H__)
#define __PCAP_TC_H__




#if !defined(C_ASSERT)
#define C_ASSERT(a)
#endif

#include <TcApi.h>





pcap_t *
TcCreate(const char *device, char *ebuf, int *is_ours);

int
TcFindAllDevs(pcap_if_list_t *devlistp, char *errbuf);

#endif
