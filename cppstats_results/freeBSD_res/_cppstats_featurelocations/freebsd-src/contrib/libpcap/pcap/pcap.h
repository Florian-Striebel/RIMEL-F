



































































#if !defined(lib_pcap_pcap_h)
#define lib_pcap_pcap_h

#include <pcap/funcattrs.h>

#include <pcap/pcap-inttypes.h>

#if defined(_WIN32)
#include <winsock2.h>
#include <io.h>
#elif defined(MSDOS)
#include <sys/types.h>
#include <sys/socket.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <pcap/socket.h>
#include <net/bpf.h>

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif








#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4

#define PCAP_ERRBUF_SIZE 256





#if BPF_RELEASE - 0 < 199406
typedef int bpf_int32;
typedef u_int bpf_u_int32;
#endif

typedef struct pcap pcap_t;
typedef struct pcap_dumper pcap_dumper_t;
typedef struct pcap_if pcap_if_t;
typedef struct pcap_addr pcap_addr_t;







































struct pcap_file_header {
bpf_u_int32 magic;
u_short version_major;
u_short version_minor;
bpf_int32 thiszone;
bpf_u_int32 sigfigs;
bpf_u_int32 snaplen;
bpf_u_int32 linktype;
};







#define LT_FCS_LENGTH_PRESENT(x) ((x) & 0x04000000)
#define LT_FCS_LENGTH(x) (((x) & 0xF0000000) >> 28)
#define LT_FCS_DATALINK_EXT(x) ((((x) & 0xF) << 28) | 0x04000000)

typedef enum {
PCAP_D_INOUT = 0,
PCAP_D_IN,
PCAP_D_OUT
} pcap_direction_t;













struct pcap_pkthdr {
struct timeval ts;
bpf_u_int32 caplen;
bpf_u_int32 len;
};




struct pcap_stat {
u_int ps_recv;
u_int ps_drop;
u_int ps_ifdrop;
#if defined(_WIN32)
u_int ps_capt;
u_int ps_sent;
u_int ps_netdrop;
#endif
};

#if defined(MSDOS)



struct pcap_stat_ex {
u_long rx_packets;
u_long tx_packets;
u_long rx_bytes;
u_long tx_bytes;
u_long rx_errors;
u_long tx_errors;
u_long rx_dropped;
u_long tx_dropped;
u_long multicast;
u_long collisions;


u_long rx_length_errors;
u_long rx_over_errors;
u_long rx_crc_errors;
u_long rx_frame_errors;
u_long rx_fifo_errors;
u_long rx_missed_errors;


u_long tx_aborted_errors;
u_long tx_carrier_errors;
u_long tx_fifo_errors;
u_long tx_heartbeat_errors;
u_long tx_window_errors;
};
#endif




struct pcap_if {
struct pcap_if *next;
char *name;
char *description;
struct pcap_addr *addresses;
bpf_u_int32 flags;
};

#define PCAP_IF_LOOPBACK 0x00000001
#define PCAP_IF_UP 0x00000002
#define PCAP_IF_RUNNING 0x00000004
#define PCAP_IF_WIRELESS 0x00000008
#define PCAP_IF_CONNECTION_STATUS 0x00000030
#define PCAP_IF_CONNECTION_STATUS_UNKNOWN 0x00000000
#define PCAP_IF_CONNECTION_STATUS_CONNECTED 0x00000010
#define PCAP_IF_CONNECTION_STATUS_DISCONNECTED 0x00000020
#define PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE 0x00000030




struct pcap_addr {
struct pcap_addr *next;
struct sockaddr *addr;
struct sockaddr *netmask;
struct sockaddr *broadaddr;
struct sockaddr *dstaddr;
};

typedef void (*pcap_handler)(u_char *, const struct pcap_pkthdr *,
const u_char *);







#define PCAP_ERROR -1
#define PCAP_ERROR_BREAK -2
#define PCAP_ERROR_NOT_ACTIVATED -3
#define PCAP_ERROR_ACTIVATED -4
#define PCAP_ERROR_NO_SUCH_DEVICE -5
#define PCAP_ERROR_RFMON_NOTSUP -6
#define PCAP_ERROR_NOT_RFMON -7
#define PCAP_ERROR_PERM_DENIED -8
#define PCAP_ERROR_IFACE_NOT_UP -9
#define PCAP_ERROR_CANTSET_TSTAMP_TYPE -10
#define PCAP_ERROR_PROMISC_PERM_DENIED -11
#define PCAP_ERROR_TSTAMP_PRECISION_NOTSUP -12






#define PCAP_WARNING 1
#define PCAP_WARNING_PROMISC_NOTSUP 2
#define PCAP_WARNING_TSTAMP_TYPE_NOTSUP 3





#define PCAP_NETMASK_UNKNOWN 0xffffffff






PCAP_API char *pcap_lookupdev(char *)
PCAP_DEPRECATED(pcap_lookupdev, "use 'pcap_findalldevs' and use the first device");

PCAP_API int pcap_lookupnet(const char *, bpf_u_int32 *, bpf_u_int32 *, char *);

PCAP_API pcap_t *pcap_create(const char *, char *);
PCAP_API int pcap_set_snaplen(pcap_t *, int);
PCAP_API int pcap_set_promisc(pcap_t *, int);
PCAP_API int pcap_can_set_rfmon(pcap_t *);
PCAP_API int pcap_set_rfmon(pcap_t *, int);
PCAP_API int pcap_set_timeout(pcap_t *, int);
PCAP_API int pcap_set_tstamp_type(pcap_t *, int);
PCAP_API int pcap_set_immediate_mode(pcap_t *, int);
PCAP_API int pcap_set_buffer_size(pcap_t *, int);
PCAP_API int pcap_set_tstamp_precision(pcap_t *, int);
PCAP_API int pcap_get_tstamp_precision(pcap_t *);
PCAP_API int pcap_activate(pcap_t *);

PCAP_API int pcap_list_tstamp_types(pcap_t *, int **);
PCAP_API void pcap_free_tstamp_types(int *);
PCAP_API int pcap_tstamp_type_name_to_val(const char *);
PCAP_API const char *pcap_tstamp_type_val_to_name(int);
PCAP_API const char *pcap_tstamp_type_val_to_description(int);

#if defined(__linux__)
PCAP_API int pcap_set_protocol_linux(pcap_t *, int);
#endif







































#define PCAP_TSTAMP_HOST 0
#define PCAP_TSTAMP_HOST_LOWPREC 1
#define PCAP_TSTAMP_HOST_HIPREC 2
#define PCAP_TSTAMP_ADAPTER 3
#define PCAP_TSTAMP_ADAPTER_UNSYNCED 4







#define PCAP_TSTAMP_PRECISION_MICRO 0
#define PCAP_TSTAMP_PRECISION_NANO 1

PCAP_API pcap_t *pcap_open_live(const char *, int, int, int, char *);
PCAP_API pcap_t *pcap_open_dead(int, int);
PCAP_API pcap_t *pcap_open_dead_with_tstamp_precision(int, int, u_int);
PCAP_API pcap_t *pcap_open_offline_with_tstamp_precision(const char *, u_int, char *);
PCAP_API pcap_t *pcap_open_offline(const char *, char *);
#if defined(_WIN32)
PCAP_API pcap_t *pcap_hopen_offline_with_tstamp_precision(intptr_t, u_int, char *);
PCAP_API pcap_t *pcap_hopen_offline(intptr_t, char *);













#if !defined(BUILDING_PCAP)
#define pcap_fopen_offline_with_tstamp_precision(f,p,b) pcap_hopen_offline_with_tstamp_precision(_get_osfhandle(_fileno(f)), p, b)

#define pcap_fopen_offline(f,b) pcap_hopen_offline(_get_osfhandle(_fileno(f)), b)

#endif
#else
PCAP_API pcap_t *pcap_fopen_offline_with_tstamp_precision(FILE *, u_int, char *);
PCAP_API pcap_t *pcap_fopen_offline(FILE *, char *);
#endif

PCAP_API void pcap_close(pcap_t *);
PCAP_API int pcap_loop(pcap_t *, int, pcap_handler, u_char *);
PCAP_API int pcap_dispatch(pcap_t *, int, pcap_handler, u_char *);
PCAP_API const u_char *pcap_next(pcap_t *, struct pcap_pkthdr *);
PCAP_API int pcap_next_ex(pcap_t *, struct pcap_pkthdr **, const u_char **);
PCAP_API void pcap_breakloop(pcap_t *);
PCAP_API int pcap_stats(pcap_t *, struct pcap_stat *);
PCAP_API int pcap_setfilter(pcap_t *, struct bpf_program *);
PCAP_API int pcap_setdirection(pcap_t *, pcap_direction_t);
PCAP_API int pcap_getnonblock(pcap_t *, char *);
PCAP_API int pcap_setnonblock(pcap_t *, int, char *);
PCAP_API int pcap_inject(pcap_t *, const void *, size_t);
PCAP_API int pcap_sendpacket(pcap_t *, const u_char *, int);
PCAP_API const char *pcap_statustostr(int);
PCAP_API const char *pcap_strerror(int);
PCAP_API char *pcap_geterr(pcap_t *);
PCAP_API void pcap_perror(pcap_t *, const char *);
PCAP_API int pcap_compile(pcap_t *, struct bpf_program *, const char *, int,
bpf_u_int32);
PCAP_API int pcap_compile_nopcap(int, int, struct bpf_program *,
const char *, int, bpf_u_int32);
PCAP_API void pcap_freecode(struct bpf_program *);
PCAP_API int pcap_offline_filter(const struct bpf_program *,
const struct pcap_pkthdr *, const u_char *);
PCAP_API int pcap_datalink(pcap_t *);
PCAP_API int pcap_datalink_ext(pcap_t *);
PCAP_API int pcap_list_datalinks(pcap_t *, int **);
PCAP_API int pcap_set_datalink(pcap_t *, int);
PCAP_API void pcap_free_datalinks(int *);
PCAP_API int pcap_datalink_name_to_val(const char *);
PCAP_API const char *pcap_datalink_val_to_name(int);
PCAP_API const char *pcap_datalink_val_to_description(int);
PCAP_API const char *pcap_datalink_val_to_description_or_dlt(int);
PCAP_API int pcap_snapshot(pcap_t *);
PCAP_API int pcap_is_swapped(pcap_t *);
PCAP_API int pcap_major_version(pcap_t *);
PCAP_API int pcap_minor_version(pcap_t *);
PCAP_API int pcap_bufsize(pcap_t *);


PCAP_API FILE *pcap_file(pcap_t *);
PCAP_API int pcap_fileno(pcap_t *);

#if defined(_WIN32)
PCAP_API int pcap_wsockinit(void);
#endif

PCAP_API pcap_dumper_t *pcap_dump_open(pcap_t *, const char *);
#if defined(_WIN32)
PCAP_API pcap_dumper_t *pcap_dump_hopen(pcap_t *, intptr_t);













#if !defined(BUILDING_PCAP)
#define pcap_dump_fopen(p,f) pcap_dump_hopen(p, _get_osfhandle(_fileno(f)))

#endif
#else
PCAP_API pcap_dumper_t *pcap_dump_fopen(pcap_t *, FILE *fp);
#endif
PCAP_API pcap_dumper_t *pcap_dump_open_append(pcap_t *, const char *);
PCAP_API FILE *pcap_dump_file(pcap_dumper_t *);
PCAP_API long pcap_dump_ftell(pcap_dumper_t *);
PCAP_API int64_t pcap_dump_ftell64(pcap_dumper_t *);
PCAP_API int pcap_dump_flush(pcap_dumper_t *);
PCAP_API void pcap_dump_close(pcap_dumper_t *);
PCAP_API void pcap_dump(u_char *, const struct pcap_pkthdr *, const u_char *);

PCAP_API int pcap_findalldevs(pcap_if_t **, char *);
PCAP_API void pcap_freealldevs(pcap_if_t *);
















PCAP_API const char *pcap_lib_version(void);








#if !defined(__NetBSD__) && !defined(__QNX__)
PCAP_API u_int bpf_filter(const struct bpf_insn *, const u_char *, u_int, u_int);
#endif
PCAP_API int bpf_validate(const struct bpf_insn *f, int len);
PCAP_API char *bpf_image(const struct bpf_insn *, int);
PCAP_API void bpf_dump(const struct bpf_program *, int);

#if defined(_WIN32)








struct pcap_send_queue
{
u_int maxlen;

u_int len;
char *buffer;
};

typedef struct pcap_send_queue pcap_send_queue;




#if !defined(AIRPCAP_HANDLE__EAE405F5_0171_9592_B3C2_C19EC426AD34__DEFINED_)
#define AIRPCAP_HANDLE__EAE405F5_0171_9592_B3C2_C19EC426AD34__DEFINED_
typedef struct _AirpcapHandle *PAirpcapHandle;
#endif

PCAP_API int pcap_setbuff(pcap_t *p, int dim);
PCAP_API int pcap_setmode(pcap_t *p, int mode);
PCAP_API int pcap_setmintocopy(pcap_t *p, int size);

PCAP_API HANDLE pcap_getevent(pcap_t *p);

PCAP_API int pcap_oid_get_request(pcap_t *, bpf_u_int32, void *, size_t *);
PCAP_API int pcap_oid_set_request(pcap_t *, bpf_u_int32, const void *, size_t *);

PCAP_API pcap_send_queue* pcap_sendqueue_alloc(u_int memsize);

PCAP_API void pcap_sendqueue_destroy(pcap_send_queue* queue);

PCAP_API int pcap_sendqueue_queue(pcap_send_queue* queue, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);

PCAP_API u_int pcap_sendqueue_transmit(pcap_t *p, pcap_send_queue* queue, int sync);

PCAP_API struct pcap_stat *pcap_stats_ex(pcap_t *p, int *pcap_stat_size);

PCAP_API int pcap_setuserbuffer(pcap_t *p, int size);

PCAP_API int pcap_live_dump(pcap_t *p, char *filename, int maxsize, int maxpacks);

PCAP_API int pcap_live_dump_ended(pcap_t *p, int sync);

PCAP_API int pcap_start_oem(char* err_str, int flags);

PCAP_API PAirpcapHandle pcap_get_airpcap_handle(pcap_t *p);

#define MODE_CAPT 0
#define MODE_STAT 1
#define MODE_MON 2

#elif defined(MSDOS)





PCAP_API int pcap_stats_ex (pcap_t *, struct pcap_stat_ex *);
PCAP_API void pcap_set_wait (pcap_t *p, void (*yield)(void), int wait);
PCAP_API u_long pcap_mac_packets (void);

#else





PCAP_API int pcap_get_selectable_fd(pcap_t *);
PCAP_API struct timeval *pcap_get_required_select_timeout(pcap_t *);

#endif

#if 0














#define PCAP_BUF_SIZE 1024




#define PCAP_SRC_FILE 2
#define PCAP_SRC_IFLOCAL 3
#define PCAP_SRC_IFREMOTE 4










































#define PCAP_SRC_FILE_STRING "file://"






#define PCAP_SRC_IF_STRING "rpcap://"








#define PCAP_OPENFLAG_PROMISCUOUS 0x00000001













#define PCAP_OPENFLAG_DATATX_UDP 0x00000002













#define PCAP_OPENFLAG_NOCAPTURE_RPCAP 0x00000004










#define PCAP_OPENFLAG_NOCAPTURE_LOCAL 0x00000008















#define PCAP_OPENFLAG_MAX_RESPONSIVENESS 0x00000010













#define RPCAP_RMTAUTH_NULL 0













#define RPCAP_RMTAUTH_PWD 1














struct pcap_rmtauth
{








int type;







char *username;







char *password;
};



















PCAP_API pcap_t *pcap_open(const char *source, int snaplen, int flags,
int read_timeout, struct pcap_rmtauth *auth, char *errbuf);
PCAP_API int pcap_createsrcstr(char *source, int type, const char *host,
const char *port, const char *name, char *errbuf);
PCAP_API int pcap_parsesrcstr(const char *source, int *type, char *host,
char *port, char *name, char *errbuf);




















PCAP_API int pcap_findalldevs_ex(const char *source,
struct pcap_rmtauth *auth, pcap_if_t **alldevs, char *errbuf);















#define PCAP_SAMP_NOSAMP 0










#define PCAP_SAMP_1_EVERY_N 1










#define PCAP_SAMP_FIRST_AFTER_N_MS 2













struct pcap_samp
{



int method;





int value;
};




PCAP_API struct pcap_samp *pcap_setsampling(pcap_t *p);






#define RPCAP_HOSTLIST_SIZE 1024

PCAP_API SOCKET pcap_remoteact_accept(const char *address, const char *port,
const char *hostlist, char *connectinghost,
struct pcap_rmtauth *auth, char *errbuf);
PCAP_API int pcap_remoteact_list(char *hostlist, char sep, int size,
char *errbuf);
PCAP_API int pcap_remoteact_close(const char *host, char *errbuf);
PCAP_API void pcap_remoteact_cleanup(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif
