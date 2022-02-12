
































#if !defined(lib_pcap_namedb_h)
#define lib_pcap_namedb_h

#if defined(__cplusplus)
extern "C" {
#endif











struct pcap_etherent {
u_char addr[6];
char name[122];
};
#if !defined(PCAP_ETHERS_FILE)
#define PCAP_ETHERS_FILE "/etc/ethers"
#endif
PCAP_API struct pcap_etherent *pcap_next_etherent(FILE *);
PCAP_API u_char *pcap_ether_hostton(const char*);
PCAP_API u_char *pcap_ether_aton(const char *);

PCAP_API bpf_u_int32 **pcap_nametoaddr(const char *);
PCAP_API struct addrinfo *pcap_nametoaddrinfo(const char *);
PCAP_API bpf_u_int32 pcap_nametonetaddr(const char *);

PCAP_API int pcap_nametoport(const char *, int *, int *);
PCAP_API int pcap_nametoportrange(const char *, int *, int *, int *);
PCAP_API int pcap_nametoproto(const char *);
PCAP_API int pcap_nametoeproto(const char *);
PCAP_API int pcap_nametollc(const char *);






#define PROTO_UNDEF -1

#if defined(__cplusplus)
}
#endif

#endif
