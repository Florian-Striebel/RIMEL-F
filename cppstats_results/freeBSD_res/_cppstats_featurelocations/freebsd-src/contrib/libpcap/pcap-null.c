




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <string.h>

#include "pcap-int.h"

static char nosup[] = "live packet capture not supported on this system";

pcap_t *
pcap_create_interface(const char *device _U_, char *ebuf)
{
(void)pcap_strlcpy(ebuf, nosup, PCAP_ERRBUF_SIZE);
return (NULL);
}

int
pcap_platform_finddevs(pcap_if_list_t *devlistp _U_, char *errbuf _U_)
{



return (0);
}

#if defined(_WIN32)
int
pcap_lookupnet(const char *device _U_, bpf_u_int32 *netp _U_,
bpf_u_int32 *maskp _U_, char *errbuf)
{
(void)pcap_strlcpy(errbuf, nosup, PCAP_ERRBUF_SIZE);
return (-1);
}
#endif




const char *
pcap_lib_version(void)
{
return (PCAP_VERSION_STRING);
}
