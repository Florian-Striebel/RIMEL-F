





























#if !defined(sf_pcap_h)
#define sf_pcap_h

extern pcap_t *pcap_check_header(const uint8_t *magic, FILE *fp,
u_int precision, char *errbuf, int *err);

#endif
