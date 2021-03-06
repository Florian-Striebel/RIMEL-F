
































#if !defined(pcap_int_h)
#define pcap_int_h

#include <signal.h>

#include <pcap/pcap.h>

#include "varattrs.h"
#include "fmtutils.h"





#define PCAP_VERSION_STRING "libpcap version " PACKAGE_VERSION

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(MSDOS)
#include <fcntl.h>
#include <io.h>
#endif





#define SWAPLL(ull) ((ull & 0xff00000000000000ULL) >> 56) | ((ull & 0x00ff000000000000ULL) >> 40) | ((ull & 0x0000ff0000000000ULL) >> 24) | ((ull & 0x000000ff00000000ULL) >> 8) | ((ull & 0x00000000ff000000ULL) << 8) | ((ull & 0x0000000000ff0000ULL) << 24) | ((ull & 0x000000000000ff00ULL) << 40) | ((ull & 0x00000000000000ffULL) << 56)


































#define MAXIMUM_SNAPLEN 262144

struct pcap_opt {
char *device;
int timeout;
u_int buffer_size;
int promisc;
int rfmon;
int immediate;
int nonblock;
int tstamp_type;
int tstamp_precision;




#if defined(__linux__)
int protocol;
#endif
#if defined(_WIN32)
int nocapture_local;
#endif
};

typedef int (*activate_op_t)(pcap_t *);
typedef int (*can_set_rfmon_op_t)(pcap_t *);
typedef int (*read_op_t)(pcap_t *, int cnt, pcap_handler, u_char *);
typedef int (*next_packet_op_t)(pcap_t *, struct pcap_pkthdr *, u_char **);
typedef int (*inject_op_t)(pcap_t *, const void *, size_t);
typedef void (*save_current_filter_op_t)(pcap_t *, const char *);
typedef int (*setfilter_op_t)(pcap_t *, struct bpf_program *);
typedef int (*setdirection_op_t)(pcap_t *, pcap_direction_t);
typedef int (*set_datalink_op_t)(pcap_t *, int);
typedef int (*getnonblock_op_t)(pcap_t *);
typedef int (*setnonblock_op_t)(pcap_t *, int);
typedef int (*stats_op_t)(pcap_t *, struct pcap_stat *);
#if defined(_WIN32)
typedef struct pcap_stat *(*stats_ex_op_t)(pcap_t *, int *);
typedef int (*setbuff_op_t)(pcap_t *, int);
typedef int (*setmode_op_t)(pcap_t *, int);
typedef int (*setmintocopy_op_t)(pcap_t *, int);
typedef HANDLE (*getevent_op_t)(pcap_t *);
typedef int (*oid_get_request_op_t)(pcap_t *, bpf_u_int32, void *, size_t *);
typedef int (*oid_set_request_op_t)(pcap_t *, bpf_u_int32, const void *, size_t *);
typedef u_int (*sendqueue_transmit_op_t)(pcap_t *, pcap_send_queue *, int);
typedef int (*setuserbuffer_op_t)(pcap_t *, int);
typedef int (*live_dump_op_t)(pcap_t *, char *, int, int);
typedef int (*live_dump_ended_op_t)(pcap_t *, int);
typedef PAirpcapHandle (*get_airpcap_handle_op_t)(pcap_t *);
#endif
typedef void (*cleanup_op_t)(pcap_t *);





struct pcap {



read_op_t read_op;




next_packet_op_t next_packet_op;

#if defined(_WIN32)
HANDLE handle;
#else
int fd;
#endif




u_int bufsize;
void *buffer;
u_char *bp;
int cc;

sig_atomic_t break_loop;

void *priv;

#if defined(ENABLE_REMOTE)
struct pcap_samp rmt_samp;
#endif

int swapped;
FILE *rfile;
u_int fddipad;
struct pcap *next;







int version_major;
int version_minor;

int snapshot;
int linktype;
int linktype_ext;
int tzoff;
int offset;
int activated;
int oldstyle;

struct pcap_opt opt;




u_char *pkt;

#if defined(_WIN32)
struct pcap_stat stat;
#endif


pcap_direction_t direction;




int bpf_codegen_flags;

#if !defined(_WIN32) && !defined(MSDOS)
int selectable_fd;












struct timeval *required_select_timeout;
#endif




struct bpf_program fcode;

char errbuf[PCAP_ERRBUF_SIZE + 1];
int dlt_count;
u_int *dlt_list;
int tstamp_type_count;
u_int *tstamp_type_list;
int tstamp_precision_count;
u_int *tstamp_precision_list;

struct pcap_pkthdr pcap_header;




activate_op_t activate_op;
can_set_rfmon_op_t can_set_rfmon_op;
inject_op_t inject_op;
save_current_filter_op_t save_current_filter_op;
setfilter_op_t setfilter_op;
setdirection_op_t setdirection_op;
set_datalink_op_t set_datalink_op;
getnonblock_op_t getnonblock_op;
setnonblock_op_t setnonblock_op;
stats_op_t stats_op;




pcap_handler oneshot_callback;

#if defined(_WIN32)




stats_ex_op_t stats_ex_op;
setbuff_op_t setbuff_op;
setmode_op_t setmode_op;
setmintocopy_op_t setmintocopy_op;
getevent_op_t getevent_op;
oid_get_request_op_t oid_get_request_op;
oid_set_request_op_t oid_set_request_op;
sendqueue_transmit_op_t sendqueue_transmit_op;
setuserbuffer_op_t setuserbuffer_op;
live_dump_op_t live_dump_op;
live_dump_ended_op_t live_dump_ended_op;
get_airpcap_handle_op_t get_airpcap_handle_op;
#endif
cleanup_op_t cleanup_op;
};




#define BPF_SPECIAL_VLAN_HANDLING 0x00000001











struct pcap_timeval {
bpf_int32 tv_sec;
bpf_int32 tv_usec;
};































struct pcap_sf_pkthdr {
struct pcap_timeval ts;
bpf_u_int32 caplen;
bpf_u_int32 len;
};











struct pcap_sf_patched_pkthdr {
struct pcap_timeval ts;
bpf_u_int32 caplen;
bpf_u_int32 len;
int index;
unsigned short protocol;
unsigned char pkt_type;
};





struct oneshot_userdata {
struct pcap_pkthdr *hdr;
const u_char **pkt;
pcap_t *pd;
};

#if !defined(min)
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

int pcap_offline_read(pcap_t *, int, pcap_handler, u_char *);

#include <stdarg.h>

#include "portability.h"





#define PACKET_COUNT_IS_UNLIMITED(count) ((count) <= 0)




#if !defined(_WIN32) && !defined(MSDOS)
int pcap_getnonblock_fd(pcap_t *);
int pcap_setnonblock_fd(pcap_t *p, int);
#endif












pcap_t *pcap_create_interface(const char *, char *);
pcap_t *pcap_create_common(char *, size_t);
int pcap_do_addexit(pcap_t *);
void pcap_add_to_pcaps_to_close(pcap_t *);
void pcap_remove_from_pcaps_to_close(pcap_t *);
void pcap_cleanup_live_common(pcap_t *);
int pcap_check_activated(pcap_t *);






















struct pcap_if_list;
typedef struct pcap_if_list pcap_if_list_t;
typedef int (*get_if_flags_func)(const char *, bpf_u_int32 *, char *);
int pcap_platform_finddevs(pcap_if_list_t *, char *);
#if !defined(_WIN32) && !defined(MSDOS)
int pcap_findalldevs_interfaces(pcap_if_list_t *, char *,
int (*)(const char *), get_if_flags_func);
#endif
pcap_if_t *find_or_add_dev(pcap_if_list_t *, const char *, bpf_u_int32,
get_if_flags_func, const char *, char *);
pcap_if_t *find_dev(pcap_if_list_t *, const char *);
pcap_if_t *add_dev(pcap_if_list_t *, const char *, bpf_u_int32, const char *,
char *);
int add_addr_to_dev(pcap_if_t *, struct sockaddr *, size_t,
struct sockaddr *, size_t, struct sockaddr *, size_t,
struct sockaddr *dstaddr, size_t, char *errbuf);
#if !defined(_WIN32)
pcap_if_t *find_or_add_if(pcap_if_list_t *, const char *, bpf_u_int32,
get_if_flags_func, char *);
int add_addr_to_if(pcap_if_list_t *, const char *, bpf_u_int32,
get_if_flags_func,
struct sockaddr *, size_t, struct sockaddr *, size_t,
struct sockaddr *, size_t, struct sockaddr *, size_t, char *);
#endif














pcap_t *pcap_open_offline_common(char *ebuf, size_t size);
bpf_u_int32 pcap_adjust_snapshot(bpf_u_int32 linktype, bpf_u_int32 snaplen);
void sf_cleanup(pcap_t *p);










struct bpf_aux_data {
u_short vlan_tag_present;
u_short vlan_tag;
};





u_int bpf_filter_with_aux_data(const struct bpf_insn *,
const u_char *, u_int, u_int, const struct bpf_aux_data *);








void pcap_oneshot(u_char *, const struct pcap_pkthdr *, const u_char *);

int install_bpf_program(pcap_t *, struct bpf_program *);

int pcap_strcasecmp(const char *, const char *);

#if defined(YYDEBUG)
extern int pcap_debug;
#endif

#if defined(__cplusplus)
}
#endif

#endif
