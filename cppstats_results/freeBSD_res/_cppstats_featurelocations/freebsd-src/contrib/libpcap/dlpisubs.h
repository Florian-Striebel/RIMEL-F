#if !defined(dlpisubs_h)
#define dlpisubs_h

#if defined(__cplusplus)
extern "C" {
#endif




struct pcap_dlpi {
#if defined(HAVE_LIBDLPI)
dlpi_handle_t dlpi_hd;
#endif
#if defined(DL_HP_RAWDLS)
int send_fd;
#endif

struct pcap_stat stat;
};




int pcap_stats_dlpi(pcap_t *, struct pcap_stat *);
int pcap_process_pkts(pcap_t *, pcap_handler, u_char *, int, u_char *, int);
int pcap_process_mactype(pcap_t *, u_int);
#if defined(HAVE_SYS_BUFMOD_H)
int pcap_conf_bufmod(pcap_t *, int);
#endif
int pcap_alloc_databuf(pcap_t *);
int strioctl(int, int, int, char *);

#if defined(__cplusplus)
}
#endif

#endif
