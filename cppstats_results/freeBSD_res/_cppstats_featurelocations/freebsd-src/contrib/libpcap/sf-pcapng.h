
























#if !defined(sf_pcapng_h)
#define sf_pcapng_h

extern pcap_t *pcap_ng_check_header(const uint8_t *magic, FILE *fp,
u_int precision, char *errbuf, int *err);

#endif
