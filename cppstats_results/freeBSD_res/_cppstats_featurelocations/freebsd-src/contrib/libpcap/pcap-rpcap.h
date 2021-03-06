
































#if !defined(pcap_rpcap_h)
#define pcap_rpcap_h




pcap_t *pcap_open_rpcap(const char *source, int snaplen, int flags,
int read_timeout, struct pcap_rmtauth *auth, char *errbuf);




int pcap_findalldevs_ex_remote(const char *source,
struct pcap_rmtauth *auth, pcap_if_t **alldevs, char *errbuf);

#endif
