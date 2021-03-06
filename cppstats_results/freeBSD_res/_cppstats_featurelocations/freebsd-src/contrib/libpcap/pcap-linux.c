















































































































#define _GNU_SOURCE

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <net/if_arp.h>
#include <poll.h>
#include <dirent.h>

#include "pcap-int.h"
#include "pcap/sll.h"
#include "pcap/vlan.h"


























#if defined(PF_PACKET)
#include <linux/if_packet.h>











#if defined(PACKET_HOST)
#define HAVE_PF_PACKET_SOCKETS
#if defined(PACKET_AUXDATA)
#define HAVE_PACKET_AUXDATA
#endif
#endif





#if defined(PCAP_SUPPORT_PACKET_RING)
#if defined(TPACKET_HDRLEN)
#define HAVE_PACKET_RING
#if defined(TPACKET3_HDRLEN)
#define HAVE_TPACKET3
#endif
#if defined(TPACKET2_HDRLEN)
#define HAVE_TPACKET2
#else
#define TPACKET_V1 0
#endif
#endif
#endif
#endif

#if defined(SO_ATTACH_FILTER)
#include <linux/types.h>
#include <linux/filter.h>
#endif

#if defined(HAVE_LINUX_NET_TSTAMP_H)
#include <linux/net_tstamp.h>
#endif

#if defined(HAVE_LINUX_SOCKIOS_H)
#include <linux/sockios.h>
#endif

#if defined(HAVE_LINUX_IF_BONDING_H)
#include <linux/if_bonding.h>




#if defined(SIOCBONDINFOQUERY)
#define BOND_INFO_QUERY_IOCTL SIOCBONDINFOQUERY
#elif defined(BOND_INFO_QUERY_OLD)
#define BOND_INFO_QUERY_IOCTL BOND_INFO_QUERY_OLD
#endif
#endif




#if defined(HAVE_LINUX_WIRELESS_H)
#include <linux/wireless.h>
#endif




#if defined(HAVE_LIBNL)
#include <linux/nl80211.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#endif




#if defined(HAVE_LINUX_ETHTOOL_H)
#include <linux/ethtool.h>
#endif

#if !defined(HAVE_SOCKLEN_T)
typedef int socklen_t;
#endif

#if !defined(MSG_TRUNC)








#define MSG_TRUNC 0x20
#endif

#if !defined(SOL_PACKET)






#define SOL_PACKET 263
#endif

#define MAX_LINKHEADER_SIZE 256






#define BIGGER_THAN_ALL_MTUS (64*1024)




struct pcap_linux {
u_int packets_read;
long proc_dropped;
struct pcap_stat stat;

char *device;
int filter_in_userland;
int blocks_to_filter_in_userland;
int must_do_on_close;
int timeout;
int sock_packet;
int cooked;
int ifindex;
int lo_ifindex;
bpf_u_int32 oldmode;
char *mondevice;
u_char *mmapbuf;
size_t mmapbuflen;
int vlan_offset;
u_int tp_version;
u_int tp_hdrlen;
u_char *oneshot_buffer;
int poll_timeout;
#if defined(HAVE_TPACKET3)
unsigned char *current_packet;
int packets_left;
#endif
};




#define MUST_CLEAR_PROMISC 0x00000001
#define MUST_CLEAR_RFMON 0x00000002
#define MUST_DELETE_MONIF 0x00000004




static int get_if_flags(const char *, bpf_u_int32 *, char *);
static int is_wifi(int, const char *);
static void map_arphrd_to_dlt(pcap_t *, int, int, const char *, int);
static int pcap_activate_linux(pcap_t *);
static int activate_old(pcap_t *);
static int activate_new(pcap_t *);
static int activate_mmap(pcap_t *, int *);
static int pcap_can_set_rfmon_linux(pcap_t *);
static int pcap_read_linux(pcap_t *, int, pcap_handler, u_char *);
static int pcap_read_packet(pcap_t *, pcap_handler, u_char *);
static int pcap_inject_linux(pcap_t *, const void *, size_t);
static int pcap_stats_linux(pcap_t *, struct pcap_stat *);
static int pcap_setfilter_linux(pcap_t *, struct bpf_program *);
static int pcap_setdirection_linux(pcap_t *, pcap_direction_t);
static int pcap_set_datalink_linux(pcap_t *, int);
static void pcap_cleanup_linux(pcap_t *);






struct tpacket_hdr_64 {
uint64_t tp_status;
unsigned int tp_len;
unsigned int tp_snaplen;
unsigned short tp_mac;
unsigned short tp_net;
unsigned int tp_sec;
unsigned int tp_usec;
};





#define TPACKET_V1_64 99

union thdr {
struct tpacket_hdr *h1;
struct tpacket_hdr_64 *h1_64;
#if defined(HAVE_TPACKET2)
struct tpacket2_hdr *h2;
#endif
#if defined(HAVE_TPACKET3)
struct tpacket_block_desc *h3;
#endif
void *raw;
};

#if defined(HAVE_PACKET_RING)
#define RING_GET_FRAME_AT(h, offset) (((union thdr **)h->buffer)[(offset)])
#define RING_GET_CURRENT_FRAME(h) RING_GET_FRAME_AT(h, h->offset)

static void destroy_ring(pcap_t *handle);
static int create_ring(pcap_t *handle, int *status);
static int prepare_tpacket_socket(pcap_t *handle);
static void pcap_cleanup_linux_mmap(pcap_t *);
static int pcap_read_linux_mmap_v1(pcap_t *, int, pcap_handler , u_char *);
static int pcap_read_linux_mmap_v1_64(pcap_t *, int, pcap_handler , u_char *);
#if defined(HAVE_TPACKET2)
static int pcap_read_linux_mmap_v2(pcap_t *, int, pcap_handler , u_char *);
#endif
#if defined(HAVE_TPACKET3)
static int pcap_read_linux_mmap_v3(pcap_t *, int, pcap_handler , u_char *);
#endif
static int pcap_setfilter_linux_mmap(pcap_t *, struct bpf_program *);
static int pcap_setnonblock_mmap(pcap_t *p, int nonblock);
static int pcap_getnonblock_mmap(pcap_t *p);
static void pcap_oneshot_mmap(u_char *user, const struct pcap_pkthdr *h,
const u_char *bytes);
#endif



























#if defined(TP_STATUS_VLAN_VALID)
#define VLAN_VALID(hdr, hv) ((hv)->tp_vlan_tci != 0 || ((hdr)->tp_status & TP_STATUS_VLAN_VALID))
#else








#define VLAN_VALID(hdr, hv) ((hv)->tp_vlan_tci != 0 || ((hdr)->tp_status & 0x10))
#endif

#if defined(TP_STATUS_VLAN_TPID_VALID)
#define VLAN_TPID(hdr, hv) (((hv)->tp_vlan_tpid || ((hdr)->tp_status & TP_STATUS_VLAN_TPID_VALID)) ? (hv)->tp_vlan_tpid : ETH_P_8021Q)
#else
#define VLAN_TPID(hdr, hv) ETH_P_8021Q
#endif




#if defined(HAVE_PF_PACKET_SOCKETS)
static int iface_get_id(int fd, const char *device, char *ebuf);
#endif
static int iface_get_mtu(int fd, const char *device, char *ebuf);
static int iface_get_arptype(int fd, const char *device, char *ebuf);
#if defined(HAVE_PF_PACKET_SOCKETS)
static int iface_bind(int fd, int ifindex, char *ebuf, int protocol);
#if defined(IW_MODE_MONITOR)
static int has_wext(int sock_fd, const char *device, char *ebuf);
#endif
static int enter_rfmon_mode(pcap_t *handle, int sock_fd,
const char *device);
#endif
#if defined(HAVE_LINUX_NET_TSTAMP_H) && defined(PACKET_TIMESTAMP)
static int iface_ethtool_get_ts_info(const char *device, pcap_t *handle,
char *ebuf);
#endif
#if defined(HAVE_PACKET_RING)
static int iface_get_offload(pcap_t *handle);
#endif
static int iface_bind_old(int fd, const char *device, char *ebuf);

#if defined(SO_ATTACH_FILTER)
static int fix_program(pcap_t *handle, struct sock_fprog *fcode,
int is_mapped);
static int fix_offset(pcap_t *handle, struct bpf_insn *p);
static int set_kernel_filter(pcap_t *handle, struct sock_fprog *fcode);
static int reset_kernel_filter(pcap_t *handle);

static struct sock_filter total_insn
= BPF_STMT(BPF_RET | BPF_K, 0);
static struct sock_fprog total_fcode
= { 1, &total_insn };
#endif

pcap_t *
pcap_create_interface(const char *device, char *ebuf)
{
pcap_t *handle;

handle = pcap_create_common(ebuf, sizeof (struct pcap_linux));
if (handle == NULL)
return NULL;

handle->activate_op = pcap_activate_linux;
handle->can_set_rfmon_op = pcap_can_set_rfmon_linux;

#if defined(HAVE_LINUX_NET_TSTAMP_H) && defined(PACKET_TIMESTAMP)



if (iface_ethtool_get_ts_info(device, handle, ebuf) == -1) {
pcap_close(handle);
return NULL;
}
#endif

#if defined(SIOCGSTAMPNS) && defined(SO_TIMESTAMPNS)








handle->tstamp_precision_count = 2;
handle->tstamp_precision_list = malloc(2 * sizeof(u_int));
if (handle->tstamp_precision_list == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
pcap_close(handle);
return NULL;
}
handle->tstamp_precision_list[0] = PCAP_TSTAMP_PRECISION_MICRO;
handle->tstamp_precision_list[1] = PCAP_TSTAMP_PRECISION_NANO;
#endif

return handle;
}

#if defined(HAVE_LIBNL)
















































static int
get_mac80211_phydev(pcap_t *handle, const char *device, char *phydev_path,
size_t phydev_max_pathlen)
{
char *pathstr;
ssize_t bytes_read;




if (asprintf(&pathstr, "/sys/class/net/%s/phy80211", device) == -1) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: Can't generate path name string for /sys/class/net device",
device);
return PCAP_ERROR;
}
bytes_read = readlink(pathstr, phydev_path, phydev_max_pathlen);
if (bytes_read == -1) {
if (errno == ENOENT || errno == EINVAL) {




free(pathstr);
return 0;
}
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s: Can't readlink %s", device, pathstr);
free(pathstr);
return PCAP_ERROR;
}
free(pathstr);
phydev_path[bytes_read] = '\0';
return 1;
}

#if defined(HAVE_LIBNL_SOCKETS)
#define get_nl_errmsg nl_geterror
#else


#define nl_sock nl_handle

static inline struct nl_handle *
nl_socket_alloc(void)
{
return nl_handle_alloc();
}

static inline void
nl_socket_free(struct nl_handle *h)
{
nl_handle_destroy(h);
}

#define get_nl_errmsg strerror

static inline int
__genl_ctrl_alloc_cache(struct nl_handle *h, struct nl_cache **cache)
{
struct nl_cache *tmp = genl_ctrl_alloc_cache(h);
if (!tmp)
return -ENOMEM;
*cache = tmp;
return 0;
}
#define genl_ctrl_alloc_cache __genl_ctrl_alloc_cache
#endif

struct nl80211_state {
struct nl_sock *nl_sock;
struct nl_cache *nl_cache;
struct genl_family *nl80211;
};

static int
nl80211_init(pcap_t *handle, struct nl80211_state *state, const char *device)
{
int err;

state->nl_sock = nl_socket_alloc();
if (!state->nl_sock) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: failed to allocate netlink handle", device);
return PCAP_ERROR;
}

if (genl_connect(state->nl_sock)) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: failed to connect to generic netlink", device);
goto out_handle_destroy;
}

err = genl_ctrl_alloc_cache(state->nl_sock, &state->nl_cache);
if (err < 0) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: failed to allocate generic netlink cache: %s",
device, get_nl_errmsg(-err));
goto out_handle_destroy;
}

state->nl80211 = genl_ctrl_search_by_name(state->nl_cache, "nl80211");
if (!state->nl80211) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: nl80211 not found", device);
goto out_cache_free;
}

return 0;

out_cache_free:
nl_cache_free(state->nl_cache);
out_handle_destroy:
nl_socket_free(state->nl_sock);
return PCAP_ERROR;
}

static void
nl80211_cleanup(struct nl80211_state *state)
{
genl_family_put(state->nl80211);
nl_cache_free(state->nl_cache);
nl_socket_free(state->nl_sock);
}

static int
del_mon_if(pcap_t *handle, int sock_fd, struct nl80211_state *state,
const char *device, const char *mondevice);

static int
add_mon_if(pcap_t *handle, int sock_fd, struct nl80211_state *state,
const char *device, const char *mondevice)
{
struct pcap_linux *handlep = handle->priv;
int ifindex;
struct nl_msg *msg;
int err;

ifindex = iface_get_id(sock_fd, device, handle->errbuf);
if (ifindex == -1)
return PCAP_ERROR;

msg = nlmsg_alloc();
if (!msg) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: failed to allocate netlink msg", device);
return PCAP_ERROR;
}

genlmsg_put(msg, 0, 0, genl_family_get_id(state->nl80211), 0,
0, NL80211_CMD_NEW_INTERFACE, 0);
NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifindex);
NLA_PUT_STRING(msg, NL80211_ATTR_IFNAME, mondevice);
NLA_PUT_U32(msg, NL80211_ATTR_IFTYPE, NL80211_IFTYPE_MONITOR);

err = nl_send_auto_complete(state->nl_sock, msg);
if (err < 0) {
#if defined HAVE_LIBNL_NLE
if (err == -NLE_FAILURE) {
#else
if (err == -ENFILE) {
#endif







nlmsg_free(msg);
return 0;
} else {




pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: nl_send_auto_complete failed adding %s interface: %s",
device, mondevice, get_nl_errmsg(-err));
nlmsg_free(msg);
return PCAP_ERROR;
}
}
err = nl_wait_for_ack(state->nl_sock);
if (err < 0) {
#if defined HAVE_LIBNL_NLE
if (err == -NLE_FAILURE) {
#else
if (err == -ENFILE) {
#endif







nlmsg_free(msg);
return 0;
} else {




pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: nl_wait_for_ack failed adding %s interface: %s",
device, mondevice, get_nl_errmsg(-err));
nlmsg_free(msg);
return PCAP_ERROR;
}
}




nlmsg_free(msg);




handlep->mondevice = strdup(mondevice);
if (handlep->mondevice == NULL) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "strdup");



del_mon_if(handle, sock_fd, state, device, mondevice);
return PCAP_ERROR;
}
return 1;

nla_put_failure:
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: nl_put failed adding %s interface",
device, mondevice);
nlmsg_free(msg);
return PCAP_ERROR;
}

static int
del_mon_if(pcap_t *handle, int sock_fd, struct nl80211_state *state,
const char *device, const char *mondevice)
{
int ifindex;
struct nl_msg *msg;
int err;

ifindex = iface_get_id(sock_fd, mondevice, handle->errbuf);
if (ifindex == -1)
return PCAP_ERROR;

msg = nlmsg_alloc();
if (!msg) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: failed to allocate netlink msg", device);
return PCAP_ERROR;
}

genlmsg_put(msg, 0, 0, genl_family_get_id(state->nl80211), 0,
0, NL80211_CMD_DEL_INTERFACE, 0);
NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifindex);

err = nl_send_auto_complete(state->nl_sock, msg);
if (err < 0) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: nl_send_auto_complete failed deleting %s interface: %s",
device, mondevice, get_nl_errmsg(-err));
nlmsg_free(msg);
return PCAP_ERROR;
}
err = nl_wait_for_ack(state->nl_sock);
if (err < 0) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: nl_wait_for_ack failed adding %s interface: %s",
device, mondevice, get_nl_errmsg(-err));
nlmsg_free(msg);
return PCAP_ERROR;
}




nlmsg_free(msg);
return 1;

nla_put_failure:
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: nl_put failed deleting %s interface",
device, mondevice);
nlmsg_free(msg);
return PCAP_ERROR;
}

static int
enter_rfmon_mode_mac80211(pcap_t *handle, int sock_fd, const char *device)
{
struct pcap_linux *handlep = handle->priv;
int ret;
char phydev_path[PATH_MAX+1];
struct nl80211_state nlstate;
struct ifreq ifr;
u_int n;




ret = get_mac80211_phydev(handle, device, phydev_path, PATH_MAX);
if (ret < 0)
return ret;
if (ret == 0)
return 0;











ret = nl80211_init(handle, &nlstate, device);
if (ret != 0)
return ret;
for (n = 0; n < UINT_MAX; n++) {



char mondevice[3+10+1];

pcap_snprintf(mondevice, sizeof mondevice, "mon%u", n);
ret = add_mon_if(handle, sock_fd, &nlstate, device, mondevice);
if (ret == 1) {




goto added;
}
if (ret < 0) {




nl80211_cleanup(&nlstate);
return ret;
}
}

pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: No free monN interfaces", device);
nl80211_cleanup(&nlstate);
return PCAP_ERROR;

added:

#if 0



delay.tv_sec = 0;
delay.tv_nsec = 500000000;
nanosleep(&delay, NULL);
#endif





if (!pcap_do_addexit(handle)) {




del_mon_if(handle, sock_fd, &nlstate, device,
handlep->mondevice);
nl80211_cleanup(&nlstate);
return PCAP_ERROR;
}




memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, handlep->mondevice, sizeof(ifr.ifr_name));
if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s: Can't get flags for %s", device,
handlep->mondevice);
del_mon_if(handle, sock_fd, &nlstate, device,
handlep->mondevice);
nl80211_cleanup(&nlstate);
return PCAP_ERROR;
}
ifr.ifr_flags |= IFF_UP|IFF_RUNNING;
if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s: Can't set flags for %s", device,
handlep->mondevice);
del_mon_if(handle, sock_fd, &nlstate, device,
handlep->mondevice);
nl80211_cleanup(&nlstate);
return PCAP_ERROR;
}




nl80211_cleanup(&nlstate);





handlep->must_do_on_close |= MUST_DELETE_MONIF;




pcap_add_to_pcaps_to_close(handle);

return 1;
}
#endif

#if defined(IW_MODE_MONITOR)











static int
is_bonding_device(int fd, const char *device)
{
#if defined(BOND_INFO_QUERY_IOCTL)
struct ifreq ifr;
ifbond ifb;

memset(&ifr, 0, sizeof ifr);
pcap_strlcpy(ifr.ifr_name, device, sizeof ifr.ifr_name);
memset(&ifb, 0, sizeof ifb);
ifr.ifr_data = (caddr_t)&ifb;
if (ioctl(fd, BOND_INFO_QUERY_IOCTL, &ifr) == 0)
return 1;
#endif

return 0;
}
#endif

static int pcap_protocol(pcap_t *handle)
{
int protocol;

protocol = handle->opt.protocol;
if (protocol == 0)
protocol = ETH_P_ALL;

return htons(protocol);
}

static int
pcap_can_set_rfmon_linux(pcap_t *handle)
{
#if defined(HAVE_LIBNL)
char phydev_path[PATH_MAX+1];
int ret;
#endif
#if defined(IW_MODE_MONITOR)
int sock_fd;
struct iwreq ireq;
#endif

if (strcmp(handle->opt.device, "any") == 0) {



return 0;
}

#if defined(HAVE_LIBNL)












ret = get_mac80211_phydev(handle, handle->opt.device, phydev_path,
PATH_MAX);
if (ret < 0)
return ret;
if (ret == 1)
return 1;
#endif

#if defined(IW_MODE_MONITOR)










sock_fd = socket(PF_PACKET, SOCK_RAW, pcap_protocol(handle));
if (sock_fd == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
return PCAP_ERROR;
}

if (is_bonding_device(sock_fd, handle->opt.device)) {

close(sock_fd);
return 0;
}




pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, handle->opt.device,
sizeof ireq.ifr_ifrn.ifrn_name);
if (ioctl(sock_fd, SIOCGIWMODE, &ireq) != -1) {



close(sock_fd);
return 1;
}
if (errno == ENODEV) {

pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIWMODE failed");
close(sock_fd);
return PCAP_ERROR_NO_SUCH_DEVICE;
}
close(sock_fd);
#endif
return 0;
}









static long int
linux_if_drops(const char * if_name)
{
char buffer[512];
FILE *file;
char *bufptr, *nameptr, *colonptr;
int field_to_convert = 3;
long int dropped_pkts = 0;

file = fopen("/proc/net/dev", "r");
if (!file)
return 0;

while (fgets(buffer, sizeof(buffer), file) != NULL)
{



if (field_to_convert != 4 && strstr(buffer, "bytes"))
{
field_to_convert = 4;
continue;
}







bufptr = buffer;

while (*bufptr == ' ')
bufptr++;
nameptr = bufptr;

colonptr = strchr(nameptr, ':');
if (colonptr == NULL)
{




continue;
}

*colonptr = '\0';
if (strcmp(if_name, nameptr) == 0)
{




bufptr = colonptr + 1;


while (--field_to_convert && *bufptr != '\0')
{





while (*bufptr == ' ')
bufptr++;





while (*bufptr != '\0' && *bufptr != ' ')
bufptr++;
}

if (field_to_convert == 0)
{




while (*bufptr == ' ')
bufptr++;




if (*bufptr != '\0')
dropped_pkts = strtol(bufptr, NULL, 10);
}
break;
}
}

fclose(file);
return dropped_pkts;
}














static void pcap_cleanup_linux( pcap_t *handle )
{
struct pcap_linux *handlep = handle->priv;
struct ifreq ifr;
#if defined(HAVE_LIBNL)
struct nl80211_state nlstate;
int ret;
#endif
#if defined(IW_MODE_MONITOR)
int oldflags;
struct iwreq ireq;
#endif

if (handlep->must_do_on_close != 0) {




if (handlep->must_do_on_close & MUST_CLEAR_PROMISC) {









memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, handlep->device,
sizeof(ifr.ifr_name));
if (ioctl(handle->fd, SIOCGIFFLAGS, &ifr) == -1) {
fprintf(stderr,
"Can't restore interface %s flags (SIOCGIFFLAGS failed: %s).\n"
"Please adjust manually.\n"
"Hint: This can't happen with Linux >= 2.2.0.\n",
handlep->device, strerror(errno));
} else {
if (ifr.ifr_flags & IFF_PROMISC) {




ifr.ifr_flags &= ~IFF_PROMISC;
if (ioctl(handle->fd, SIOCSIFFLAGS,
&ifr) == -1) {
fprintf(stderr,
"Can't restore interface %s flags (SIOCSIFFLAGS failed: %s).\n"
"Please adjust manually.\n"
"Hint: This can't happen with Linux >= 2.2.0.\n",
handlep->device,
strerror(errno));
}
}
}
}

#if defined(HAVE_LIBNL)
if (handlep->must_do_on_close & MUST_DELETE_MONIF) {
ret = nl80211_init(handle, &nlstate, handlep->device);
if (ret >= 0) {
ret = del_mon_if(handle, handle->fd, &nlstate,
handlep->device, handlep->mondevice);
nl80211_cleanup(&nlstate);
}
if (ret < 0) {
fprintf(stderr,
"Can't delete monitor interface %s (%s).\n"
"Please delete manually.\n",
handlep->mondevice, handle->errbuf);
}
}
#endif

#if defined(IW_MODE_MONITOR)
if (handlep->must_do_on_close & MUST_CLEAR_RFMON) {















oldflags = 0;
memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, handlep->device,
sizeof(ifr.ifr_name));
if (ioctl(handle->fd, SIOCGIFFLAGS, &ifr) != -1) {
if (ifr.ifr_flags & IFF_UP) {
oldflags = ifr.ifr_flags;
ifr.ifr_flags &= ~IFF_UP;
if (ioctl(handle->fd, SIOCSIFFLAGS, &ifr) == -1)
oldflags = 0;
}
}




pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, handlep->device,
sizeof ireq.ifr_ifrn.ifrn_name);
ireq.u.mode = handlep->oldmode;
if (ioctl(handle->fd, SIOCSIWMODE, &ireq) == -1) {



fprintf(stderr,
"Can't restore interface %s wireless mode (SIOCSIWMODE failed: %s).\n"
"Please adjust manually.\n",
handlep->device, strerror(errno));
}





if (oldflags != 0) {
ifr.ifr_flags = oldflags;
if (ioctl(handle->fd, SIOCSIFFLAGS, &ifr) == -1) {
fprintf(stderr,
"Can't bring interface %s back up (SIOCSIFFLAGS failed: %s).\n"
"Please adjust manually.\n",
handlep->device, strerror(errno));
}
}
}
#endif





pcap_remove_from_pcaps_to_close(handle);
}

if (handlep->mondevice != NULL) {
free(handlep->mondevice);
handlep->mondevice = NULL;
}
if (handlep->device != NULL) {
free(handlep->device);
handlep->device = NULL;
}
pcap_cleanup_live_common(handle);
}




static void
set_poll_timeout(struct pcap_linux *handlep)
{
#if defined(HAVE_TPACKET3)
struct utsname utsname;
char *version_component, *endp;
int major, minor;
int broken_tpacket_v3 = 1;






if (uname(&utsname) == 0) {






version_component = utsname.release;
major = strtol(version_component, &endp, 10);
if (endp != version_component && *endp == '.') {




version_component = endp + 1;
minor = strtol(version_component, &endp, 10);
if (endp != version_component &&
(*endp == '.' || *endp == '\0')) {




if (major >= 4 || (major == 3 && minor >= 19)) {

broken_tpacket_v3 = 0;
}
}
}
}
#endif
if (handlep->timeout == 0) {
#if defined(HAVE_TPACKET3)












if (handlep->tp_version == TPACKET_V3 && broken_tpacket_v3)
handlep->poll_timeout = 1;
else
#endif
handlep->poll_timeout = -1;
} else if (handlep->timeout > 0) {
#if defined(HAVE_TPACKET3)





if (handlep->tp_version == TPACKET_V3 && !broken_tpacket_v3)
handlep->poll_timeout = -1;
else
#endif
handlep->poll_timeout = handlep->timeout;
} else {





handlep->poll_timeout = 0;
}
}









static int
pcap_activate_linux(pcap_t *handle)
{
struct pcap_linux *handlep = handle->priv;
const char *device;
struct ifreq ifr;
int status = 0;
int ret;

device = handle->opt.device;












if (strlen(device) >= sizeof(ifr.ifr_name)) {
status = PCAP_ERROR_NO_SUCH_DEVICE;
goto fail;
}









if (handle->snapshot <= 0 || handle->snapshot > MAXIMUM_SNAPLEN)
handle->snapshot = MAXIMUM_SNAPLEN;

handle->inject_op = pcap_inject_linux;
handle->setfilter_op = pcap_setfilter_linux;
handle->setdirection_op = pcap_setdirection_linux;
handle->set_datalink_op = pcap_set_datalink_linux;
handle->getnonblock_op = pcap_getnonblock_fd;
handle->setnonblock_op = pcap_setnonblock_fd;
handle->cleanup_op = pcap_cleanup_linux;
handle->read_op = pcap_read_linux;
handle->stats_op = pcap_stats_linux;






if (strcmp(device, "any") == 0) {
if (handle->opt.promisc) {
handle->opt.promisc = 0;

pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Promiscuous mode not supported on the \"any\" device");
status = PCAP_WARNING_PROMISC_NOTSUP;
}
}

handlep->device = strdup(device);
if (handlep->device == NULL) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "strdup");
status = PCAP_ERROR;
goto fail;
}


handlep->timeout = handle->opt.timeout;






if (handle->opt.promisc)
handlep->proc_dropped = linux_if_drops(handlep->device);










ret = activate_new(handle);
if (ret < 0) {





status = ret;
goto fail;
}
if (ret == 1) {




switch (activate_mmap(handle, &status)) {

case 1:









set_poll_timeout(handlep);
return status;

case 0:




break;

case -1:







goto fail;
}
}
else if (ret == 0) {

if ((ret = activate_old(handle)) != 1) {





status = ret;
goto fail;
}
}




if (handle->opt.buffer_size != 0) {



if (setsockopt(handle->fd, SOL_SOCKET, SO_RCVBUF,
&handle->opt.buffer_size,
sizeof(handle->opt.buffer_size)) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "SO_RCVBUF");
status = PCAP_ERROR;
goto fail;
}
}



handle->buffer = malloc(handle->bufsize + handle->offset);
if (!handle->buffer) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
status = PCAP_ERROR;
goto fail;
}





handle->selectable_fd = handle->fd;

return status;

fail:
pcap_cleanup_linux(handle);
return status;
}






static int
pcap_read_linux(pcap_t *handle, int max_packets _U_, pcap_handler callback, u_char *user)
{




return pcap_read_packet(handle, callback, user);
}

static int
pcap_set_datalink_linux(pcap_t *handle, int dlt)
{
handle->linktype = dlt;
return 0;
}






static inline int
linux_check_direction(const pcap_t *handle, const struct sockaddr_ll *sll)
{
struct pcap_linux *handlep = handle->priv;

if (sll->sll_pkttype == PACKET_OUTGOING) {






if (sll->sll_ifindex == handlep->lo_ifindex)
return 0;












if ((sll->sll_protocol == LINUX_SLL_P_CAN ||
sll->sll_protocol == LINUX_SLL_P_CANFD) &&
handle->direction != PCAP_D_OUT)
return 0;




if (handle->direction == PCAP_D_IN)
return 0;
} else {




if (handle->direction == PCAP_D_OUT)
return 0;
}
return 1;
}






static int
pcap_read_packet(pcap_t *handle, pcap_handler callback, u_char *userdata)
{
struct pcap_linux *handlep = handle->priv;
u_char *bp;
int offset;
#if defined(HAVE_PF_PACKET_SOCKETS)
struct sockaddr_ll from;
#else
struct sockaddr from;
#endif
#if defined(HAVE_PACKET_AUXDATA) && defined(HAVE_STRUCT_TPACKET_AUXDATA_TP_VLAN_TCI)
struct iovec iov;
struct msghdr msg;
struct cmsghdr *cmsg;
union {
struct cmsghdr cmsg;
char buf[CMSG_SPACE(sizeof(struct tpacket_auxdata))];
} cmsg_buf;
#else
socklen_t fromlen;
#endif
int packet_len, caplen;
struct pcap_pkthdr pcap_header;

struct bpf_aux_data aux_data;
#if defined(HAVE_PF_PACKET_SOCKETS)




if (handlep->cooked) {
if (handle->linktype == DLT_LINUX_SLL2)
offset = SLL2_HDR_LEN;
else
offset = SLL_HDR_LEN;
} else
offset = 0;
#else




offset = 0;
#endif
















bp = (u_char *)handle->buffer + handle->offset;

#if defined(HAVE_PACKET_AUXDATA) && defined(HAVE_STRUCT_TPACKET_AUXDATA_TP_VLAN_TCI)
msg.msg_name = &from;
msg.msg_namelen = sizeof(from);
msg.msg_iov = &iov;
msg.msg_iovlen = 1;
msg.msg_control = &cmsg_buf;
msg.msg_controllen = sizeof(cmsg_buf);
msg.msg_flags = 0;

iov.iov_len = handle->bufsize - offset;
iov.iov_base = bp + offset;
#endif

do {



if (handle->break_loop) {





handle->break_loop = 0;
return PCAP_ERROR_BREAK;
}

#if defined(HAVE_PACKET_AUXDATA) && defined(HAVE_STRUCT_TPACKET_AUXDATA_TP_VLAN_TCI)
packet_len = recvmsg(handle->fd, &msg, MSG_TRUNC);
#else
fromlen = sizeof(from);
packet_len = recvfrom(
handle->fd, bp + offset,
handle->bufsize - offset, MSG_TRUNC,
(struct sockaddr *) &from, &fromlen);
#endif
} while (packet_len == -1 && errno == EINTR);



if (packet_len == -1) {
switch (errno) {

case EAGAIN:
return 0;

case ENETDOWN:







pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"The interface went down");
return PCAP_ERROR;

default:
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "recvfrom");
return PCAP_ERROR;
}
}

#if defined(HAVE_PF_PACKET_SOCKETS)
if (!handlep->sock_packet) {












if (handlep->ifindex != -1 &&
from.sll_ifindex != handlep->ifindex)
return 0;







if (!linux_check_direction(handle, &from))
return 0;
}
#endif

#if defined(HAVE_PF_PACKET_SOCKETS)



if (handlep->cooked) {




if (handle->linktype == DLT_LINUX_SLL2) {
struct sll2_header *hdrp;

packet_len += SLL2_HDR_LEN;

hdrp = (struct sll2_header *)bp;
hdrp->sll2_protocol = from.sll_protocol;
hdrp->sll2_reserved_mbz = 0;
hdrp->sll2_if_index = htonl(from.sll_ifindex);
hdrp->sll2_hatype = htons(from.sll_hatype);
hdrp->sll2_pkttype = from.sll_pkttype;
hdrp->sll2_halen = from.sll_halen;
memcpy(hdrp->sll2_addr, from.sll_addr,
(from.sll_halen > SLL_ADDRLEN) ?
SLL_ADDRLEN :
from.sll_halen);
} else {
struct sll_header *hdrp;

packet_len += SLL_HDR_LEN;

hdrp = (struct sll_header *)bp;
hdrp->sll_pkttype = htons(from.sll_pkttype);
hdrp->sll_hatype = htons(from.sll_hatype);
hdrp->sll_halen = htons(from.sll_halen);
memcpy(hdrp->sll_addr, from.sll_addr,
(from.sll_halen > SLL_ADDRLEN) ?
SLL_ADDRLEN :
from.sll_halen);
hdrp->sll_protocol = from.sll_protocol;
}
}




aux_data.vlan_tag_present = 0;
aux_data.vlan_tag = 0;
#if defined(HAVE_PACKET_AUXDATA) && defined(HAVE_STRUCT_TPACKET_AUXDATA_TP_VLAN_TCI)
if (handlep->vlan_offset != -1) {
for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
struct tpacket_auxdata *aux;
unsigned int len;
struct vlan_tag *tag;

if (cmsg->cmsg_len < CMSG_LEN(sizeof(struct tpacket_auxdata)) ||
cmsg->cmsg_level != SOL_PACKET ||
cmsg->cmsg_type != PACKET_AUXDATA) {




continue;
}

aux = (struct tpacket_auxdata *)CMSG_DATA(cmsg);
if (!VLAN_VALID(aux, aux)) {




continue;
}

len = (u_int)packet_len > iov.iov_len ? iov.iov_len : (u_int)packet_len;
if (len < (u_int)handlep->vlan_offset)
break;







bp -= VLAN_TAG_LEN;
memmove(bp, bp + VLAN_TAG_LEN, handlep->vlan_offset);




tag = (struct vlan_tag *)(bp + handlep->vlan_offset);
tag->vlan_tpid = htons(VLAN_TPID(aux, aux));
tag->vlan_tci = htons(aux->tp_vlan_tci);







aux_data.vlan_tag_present = 1;
aux_data.vlan_tag = htons(aux->tp_vlan_tci) & 0x0fff;




packet_len += VLAN_TAG_LEN;
}
}
#endif
#endif

































caplen = packet_len;
if (caplen > handle->snapshot)
caplen = handle->snapshot;


if (handlep->filter_in_userland && handle->fcode.bf_insns) {
if (bpf_filter_with_aux_data(handle->fcode.bf_insns, bp,
packet_len, caplen, &aux_data) == 0) {

return 0;
}
}




#if defined(SIOCGSTAMPNS) && defined(SO_TIMESTAMPNS)
if (handle->opt.tstamp_precision == PCAP_TSTAMP_PRECISION_NANO) {
if (ioctl(handle->fd, SIOCGSTAMPNS, &pcap_header.ts) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "SIOCGSTAMPNS");
return PCAP_ERROR;
}
} else
#endif
{
if (ioctl(handle->fd, SIOCGSTAMP, &pcap_header.ts) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "SIOCGSTAMP");
return PCAP_ERROR;
}
}

pcap_header.caplen = caplen;
pcap_header.len = packet_len;













































handlep->packets_read++;


callback(userdata, &pcap_header, bp);

return 1;
}

static int
pcap_inject_linux(pcap_t *handle, const void *buf, size_t size)
{
struct pcap_linux *handlep = handle->priv;
int ret;

#if defined(HAVE_PF_PACKET_SOCKETS)
if (!handlep->sock_packet) {

if (handlep->ifindex == -1) {



pcap_strlcpy(handle->errbuf,
"Sending packets isn't supported on the \"any\" device",
PCAP_ERRBUF_SIZE);
return (-1);
}

if (handlep->cooked) {







pcap_strlcpy(handle->errbuf,
"Sending packets isn't supported in cooked mode",
PCAP_ERRBUF_SIZE);
return (-1);
}
}
#endif

ret = send(handle->fd, buf, size, 0);
if (ret == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "send");
return (-1);
}
return (ret);
}









static int
pcap_stats_linux(pcap_t *handle, struct pcap_stat *stats)
{
struct pcap_linux *handlep = handle->priv;
#if defined(HAVE_STRUCT_TPACKET_STATS)
#if defined(HAVE_TPACKET3)














struct tpacket_stats_v3 kstats;
#else
struct tpacket_stats kstats;
#endif
socklen_t len = sizeof (struct tpacket_stats);
#endif

long if_dropped = 0;




if (handle->opt.promisc)
{
if_dropped = handlep->proc_dropped;
handlep->proc_dropped = linux_if_drops(handlep->device);
handlep->stat.ps_ifdrop += (handlep->proc_dropped - if_dropped);
}

#if defined(HAVE_STRUCT_TPACKET_STATS)



if (getsockopt(handle->fd, SOL_PACKET, PACKET_STATISTICS,
&kstats, &len) > -1) {















































handlep->stat.ps_recv += kstats.tp_packets;
handlep->stat.ps_drop += kstats.tp_drops;
*stats = handlep->stat;
return 0;
}
else
{







if (errno != EOPNOTSUPP) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "pcap_stats");
return -1;
}
}
#endif

























stats->ps_recv = handlep->packets_read;
stats->ps_drop = 0;
stats->ps_ifdrop = handlep->stat.ps_ifdrop;
return 0;
}

static int
add_linux_if(pcap_if_list_t *devlistp, const char *ifname, int fd, char *errbuf)
{
const char *p;
char name[512];
char *q, *saveq;
struct ifreq ifrflags;




p = ifname;
q = &name[0];
while (*p != '\0' && isascii(*p) && !isspace(*p)) {
if (*p == ':') {











saveq = q;
while (isascii(*p) && isdigit(*p))
*q++ = *p++;
if (*p != ':') {




q = saveq;
}
break;
} else
*q++ = *p++;
}
*q = '\0';




pcap_strlcpy(ifrflags.ifr_name, name, sizeof(ifrflags.ifr_name));
if (ioctl(fd, SIOCGIFFLAGS, (char *)&ifrflags) < 0) {
if (errno == ENXIO || errno == ENODEV)
return (0);
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFFLAGS: %.*s",
(int)sizeof(ifrflags.ifr_name),
ifrflags.ifr_name);
return (-1);
}





if (find_or_add_if(devlistp, name, ifrflags.ifr_flags,
get_if_flags, errbuf) == NULL) {



return (-1);
}

return (0);
}

















static int
scan_sys_class_net(pcap_if_list_t *devlistp, char *errbuf)
{
DIR *sys_class_net_d;
int fd;
struct dirent *ent;
char subsystem_path[PATH_MAX+1];
struct stat statb;
int ret = 1;

sys_class_net_d = opendir("/sys/class/net");
if (sys_class_net_d == NULL) {



if (errno == ENOENT)
return (0);




pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't open /sys/class/net");
return (-1);
}




fd = socket(PF_UNIX, SOCK_RAW, 0);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
(void)closedir(sys_class_net_d);
return (-1);
}

for (;;) {
errno = 0;
ent = readdir(sys_class_net_d);
if (ent == NULL) {



break;
}




if (strcmp(ent->d_name, ".") == 0 ||
strcmp(ent->d_name, "..") == 0)
continue;





if (ent->d_type == DT_REG)
continue;








pcap_snprintf(subsystem_path, sizeof subsystem_path,
"/sys/class/net/%s/ifindex", ent->d_name);
if (lstat(subsystem_path, &statb) != 0) {










continue;
}




if (add_linux_if(devlistp, &ent->d_name[0], fd, errbuf) == -1) {

ret = -1;
break;
}
}
if (ret != -1) {




if (errno != 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "Error reading /sys/class/net");
ret = -1;
}
}

(void)close(fd);
(void)closedir(sys_class_net_d);
return (ret);
}








static int
scan_proc_net_dev(pcap_if_list_t *devlistp, char *errbuf)
{
FILE *proc_net_f;
int fd;
char linebuf[512];
int linenum;
char *p;
int ret = 0;

proc_net_f = fopen("/proc/net/dev", "r");
if (proc_net_f == NULL) {



if (errno == ENOENT)
return (0);




pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't open /proc/net/dev");
return (-1);
}




fd = socket(PF_UNIX, SOCK_RAW, 0);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
(void)fclose(proc_net_f);
return (-1);
}

for (linenum = 1;
fgets(linebuf, sizeof linebuf, proc_net_f) != NULL; linenum++) {



if (linenum <= 2)
continue;

p = &linebuf[0];




while (*p != '\0' && isascii(*p) && isspace(*p))
p++;
if (*p == '\0' || *p == '\n')
continue;




if (add_linux_if(devlistp, p, fd, errbuf) == -1) {

ret = -1;
break;
}
}
if (ret != -1) {




if (ferror(proc_net_f)) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "Error reading /proc/net/dev");
ret = -1;
}
}

(void)close(fd);
(void)fclose(proc_net_f);
return (ret);
}




static const char any_descr[] = "Pseudo-device that captures on all interfaces";




static int
can_be_bound(const char *name _U_)
{
return (1);
}




static int
get_if_flags(const char *name, bpf_u_int32 *flags, char *errbuf)
{
int sock;
FILE *fh;
unsigned int arptype;
struct ifreq ifr;
struct ethtool_value info;

if (*flags & PCAP_IF_LOOPBACK) {




*flags |= PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE;
return 0;
}

sock = socket(AF_INET, SOCK_DGRAM, 0);
if (sock == -1) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE, errno,
"Can't create socket to get ethtool information for %s",
name);
return -1;
}





if (is_wifi(sock, name)) {



*flags |= PCAP_IF_WIRELESS;
} else {






char *pathstr;

if (asprintf(&pathstr, "/sys/class/net/%s/type", name) == -1) {
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"%s: Can't generate path name string for /sys/class/net device",
name);
close(sock);
return -1;
}
fh = fopen(pathstr, "r");
if (fh != NULL) {
if (fscanf(fh, "%u", &arptype) == 1) {



switch (arptype) {

#if defined(ARPHRD_LOOPBACK)
case ARPHRD_LOOPBACK:








close(sock);
fclose(fh);
free(pathstr);
return 0;
#endif

case ARPHRD_IRDA:
case ARPHRD_IEEE80211:
case ARPHRD_IEEE80211_PRISM:
case ARPHRD_IEEE80211_RADIOTAP:
#if defined(ARPHRD_IEEE802154)
case ARPHRD_IEEE802154:
#endif
#if defined(ARPHRD_IEEE802154_MONITOR)
case ARPHRD_IEEE802154_MONITOR:
#endif
#if defined(ARPHRD_6LOWPAN)
case ARPHRD_6LOWPAN:
#endif



*flags |= PCAP_IF_WIRELESS;
break;
}
}
fclose(fh);
free(pathstr);
}
}

#if defined(ETHTOOL_GLINK)
memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
info.cmd = ETHTOOL_GLINK;
ifr.ifr_data = (caddr_t)&info;
if (ioctl(sock, SIOCETHTOOL, &ifr) == -1) {
int save_errno = errno;

switch (save_errno) {

case EOPNOTSUPP:
case EINVAL:









*flags |= PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE;
close(sock);
return 0;

case ENODEV:






close(sock);
return 0;

default:



pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
save_errno,
"%s: SIOCETHTOOL(ETHTOOL_GLINK) ioctl failed",
name);
close(sock);
return -1;
}
}




if (info.data) {



*flags |= PCAP_IF_CONNECTION_STATUS_CONNECTED;
} else {



*flags |= PCAP_IF_CONNECTION_STATUS_DISCONNECTED;
}
#endif

close(sock);
return 0;
}

int
pcap_platform_finddevs(pcap_if_list_t *devlistp, char *errbuf)
{
int ret;




if (pcap_findalldevs_interfaces(devlistp, errbuf, can_be_bound,
get_if_flags) == -1)
return (-1);









ret = scan_sys_class_net(devlistp, errbuf);
if (ret == -1)
return (-1);
if (ret == 0) {



if (scan_proc_net_dev(devlistp, errbuf) == -1)
return (-1);
}







if (add_dev(devlistp, "any",
PCAP_IF_UP|PCAP_IF_RUNNING|PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE,
any_descr, errbuf) == NULL)
return (-1);

return (0);
}




static int
pcap_setfilter_linux_common(pcap_t *handle, struct bpf_program *filter,
int is_mmapped)
{
struct pcap_linux *handlep;
#if defined(SO_ATTACH_FILTER)
struct sock_fprog fcode;
int can_filter_in_kernel;
int err = 0;
#endif

if (!handle)
return -1;
if (!filter) {
pcap_strlcpy(handle->errbuf, "setfilter: No filter specified",
PCAP_ERRBUF_SIZE);
return -1;
}

handlep = handle->priv;



if (install_bpf_program(handle, filter) < 0)

return -1;





handlep->filter_in_userland = 1;



#if defined(SO_ATTACH_FILTER)
#if defined(USHRT_MAX)
if (handle->fcode.bf_len > USHRT_MAX) {






fprintf(stderr, "Warning: Filter too complex for kernel\n");
fcode.len = 0;
fcode.filter = NULL;
can_filter_in_kernel = 0;
} else
#endif
{














switch (fix_program(handle, &fcode, is_mmapped)) {

case -1:
default:





return -1;

case 0:




can_filter_in_kernel = 0;
break;

case 1:



can_filter_in_kernel = 1;
break;
}
}

























if (can_filter_in_kernel) {
if ((err = set_kernel_filter(handle, &fcode)) == 0)
{




handlep->filter_in_userland = 0;
}
else if (err == -1)
{





if (errno != ENOPROTOOPT && errno != EOPNOTSUPP) {
fprintf(stderr,
"Warning: Kernel filter failed: %s\n",
pcap_strerror(errno));
}
}
}









if (handlep->filter_in_userland) {
if (reset_kernel_filter(handle) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"can't remove kernel filter");
err = -2;
}
}




if (fcode.filter != NULL)
free(fcode.filter);

if (err == -2)

return -1;
#endif

return 0;
}

static int
pcap_setfilter_linux(pcap_t *handle, struct bpf_program *filter)
{
return pcap_setfilter_linux_common(handle, filter, 0);
}






static int
pcap_setdirection_linux(pcap_t *handle, pcap_direction_t d)
{
#if defined(HAVE_PF_PACKET_SOCKETS)
struct pcap_linux *handlep = handle->priv;

if (!handlep->sock_packet) {
handle->direction = d;
return 0;
}
#endif




pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Setting direction is not supported on SOCK_PACKET sockets");
return -1;
}

static int
is_wifi(int sock_fd
#if !defined(IW_MODE_MONITOR)
_U_
#endif
, const char *device)
{
char *pathstr;
struct stat statb;
#if defined(IW_MODE_MONITOR)
char errbuf[PCAP_ERRBUF_SIZE];
#endif





if (asprintf(&pathstr, "/sys/class/net/%s/wireless", device) == -1) {



return 0;
}
if (stat(pathstr, &statb) == 0) {
free(pathstr);
return 1;
}
free(pathstr);

#if defined(IW_MODE_MONITOR)




if (has_wext(sock_fd, device, errbuf) == 1) {




return 1;
}
#endif
return 0;
}



















static void map_arphrd_to_dlt(pcap_t *handle, int sock_fd, int arptype,
const char *device, int cooked_ok)
{
static const char cdma_rmnet[] = "cdma_rmnet";

switch (arptype) {

case ARPHRD_ETHER:












if (strncmp(device, cdma_rmnet, sizeof cdma_rmnet - 1) == 0) {
handle->linktype = DLT_RAW;
return;
}



















if (!is_wifi(sock_fd, device)) {



handle->dlt_list = (u_int *) malloc(sizeof(u_int) * 2);



if (handle->dlt_list != NULL) {
handle->dlt_list[0] = DLT_EN10MB;
handle->dlt_list[1] = DLT_DOCSIS;
handle->dlt_count = 2;
}
}


case ARPHRD_METRICOM:
case ARPHRD_LOOPBACK:
handle->linktype = DLT_EN10MB;
handle->offset = 2;
break;

case ARPHRD_EETHER:
handle->linktype = DLT_EN3MB;
break;

case ARPHRD_AX25:
handle->linktype = DLT_AX25_KISS;
break;

case ARPHRD_PRONET:
handle->linktype = DLT_PRONET;
break;

case ARPHRD_CHAOS:
handle->linktype = DLT_CHAOS;
break;
#if !defined(ARPHRD_CAN)
#define ARPHRD_CAN 280
#endif
case ARPHRD_CAN:







handle->linktype = DLT_LINUX_SLL;
break;

#if !defined(ARPHRD_IEEE802_TR)
#define ARPHRD_IEEE802_TR 800
#endif
case ARPHRD_IEEE802_TR:
case ARPHRD_IEEE802:
handle->linktype = DLT_IEEE802;
handle->offset = 2;
break;

case ARPHRD_ARCNET:
handle->linktype = DLT_ARCNET_LINUX;
break;

#if !defined(ARPHRD_FDDI)
#define ARPHRD_FDDI 774
#endif
case ARPHRD_FDDI:
handle->linktype = DLT_FDDI;
handle->offset = 3;
break;

#if !defined(ARPHRD_ATM)
#define ARPHRD_ATM 19
#endif
case ARPHRD_ATM:




































if (cooked_ok)
handle->linktype = DLT_LINUX_SLL;
else
handle->linktype = -1;
break;

#if !defined(ARPHRD_IEEE80211)
#define ARPHRD_IEEE80211 801
#endif
case ARPHRD_IEEE80211:
handle->linktype = DLT_IEEE802_11;
break;

#if !defined(ARPHRD_IEEE80211_PRISM)
#define ARPHRD_IEEE80211_PRISM 802
#endif
case ARPHRD_IEEE80211_PRISM:
handle->linktype = DLT_PRISM_HEADER;
break;

#if !defined(ARPHRD_IEEE80211_RADIOTAP)
#define ARPHRD_IEEE80211_RADIOTAP 803
#endif
case ARPHRD_IEEE80211_RADIOTAP:
handle->linktype = DLT_IEEE802_11_RADIO;
break;

case ARPHRD_PPP:



















if (cooked_ok)
handle->linktype = DLT_LINUX_SLL;
else {
















handle->linktype = DLT_RAW;
}
break;

#if !defined(ARPHRD_CISCO)
#define ARPHRD_CISCO 513
#endif
case ARPHRD_CISCO:
handle->linktype = DLT_C_HDLC;
break;



case ARPHRD_TUNNEL:
#if !defined(ARPHRD_SIT)
#define ARPHRD_SIT 776
#endif
case ARPHRD_SIT:
case ARPHRD_CSLIP:
case ARPHRD_SLIP6:
case ARPHRD_CSLIP6:
case ARPHRD_ADAPT:
case ARPHRD_SLIP:
#if !defined(ARPHRD_RAWHDLC)
#define ARPHRD_RAWHDLC 518
#endif
case ARPHRD_RAWHDLC:
#if !defined(ARPHRD_DLCI)
#define ARPHRD_DLCI 15
#endif
case ARPHRD_DLCI:




handle->linktype = DLT_RAW;
break;

#if !defined(ARPHRD_FRAD)
#define ARPHRD_FRAD 770
#endif
case ARPHRD_FRAD:
handle->linktype = DLT_FRELAY;
break;

case ARPHRD_LOCALTLK:
handle->linktype = DLT_LTALK;
break;

case 18:















handle->linktype = DLT_IP_OVER_FC;
break;

#if !defined(ARPHRD_FCPP)
#define ARPHRD_FCPP 784
#endif
case ARPHRD_FCPP:
#if !defined(ARPHRD_FCAL)
#define ARPHRD_FCAL 785
#endif
case ARPHRD_FCAL:
#if !defined(ARPHRD_FCPL)
#define ARPHRD_FCPL 786
#endif
case ARPHRD_FCPL:
#if !defined(ARPHRD_FCFABRIC)
#define ARPHRD_FCFABRIC 787
#endif
case ARPHRD_FCFABRIC:














































handle->dlt_list = (u_int *) malloc(sizeof(u_int) * 3);



if (handle->dlt_list != NULL) {
handle->dlt_list[0] = DLT_FC_2;
handle->dlt_list[1] = DLT_FC_2_WITH_FRAME_DELIMS;
handle->dlt_list[2] = DLT_IP_OVER_FC;
handle->dlt_count = 3;
}
handle->linktype = DLT_FC_2;
break;

#if !defined(ARPHRD_IRDA)
#define ARPHRD_IRDA 783
#endif
case ARPHRD_IRDA:

handle->linktype = DLT_LINUX_IRDA;





break;



#if !defined(ARPHRD_LAPD)
#define ARPHRD_LAPD 8445
#endif
case ARPHRD_LAPD:

handle->linktype = DLT_LINUX_LAPD;
break;

#if !defined(ARPHRD_NONE)
#define ARPHRD_NONE 0xFFFE
#endif
case ARPHRD_NONE:




handle->linktype = DLT_RAW;
break;

#if !defined(ARPHRD_IEEE802154)
#define ARPHRD_IEEE802154 804
#endif
case ARPHRD_IEEE802154:
handle->linktype = DLT_IEEE802_15_4_NOFCS;
break;

#if !defined(ARPHRD_NETLINK)
#define ARPHRD_NETLINK 824
#endif
case ARPHRD_NETLINK:
handle->linktype = DLT_NETLINK;








break;

#if !defined(ARPHRD_VSOCKMON)
#define ARPHRD_VSOCKMON 826
#endif
case ARPHRD_VSOCKMON:
handle->linktype = DLT_VSOCK;
break;

default:
handle->linktype = -1;
break;
}
}



#if defined(PACKET_RESERVE)
static void
set_dlt_list_cooked(pcap_t *handle, int sock_fd)
{
socklen_t len;
unsigned int tp_reserve;





len = sizeof(tp_reserve);
if (getsockopt(sock_fd, SOL_PACKET, PACKET_RESERVE, &tp_reserve,
&len) == 0) {



handle->dlt_list = (u_int *) malloc(sizeof(u_int) * 2);



if (handle->dlt_list != NULL) {
handle->dlt_list[0] = DLT_LINUX_SLL;
handle->dlt_list[1] = DLT_LINUX_SLL2;
handle->dlt_count = 2;
}
}
}
#else




static void
set_dlt_list_cooked(pcap_t *handle _U_, int sock_fd _U_)
{
}
#endif








static int
activate_new(pcap_t *handle)
{
#if defined(HAVE_PF_PACKET_SOCKETS)
struct pcap_linux *handlep = handle->priv;
const char *device = handle->opt.device;
int is_any_device = (strcmp(device, "any") == 0);
int protocol = pcap_protocol(handle);
int sock_fd = -1, arptype, ret;
#if defined(HAVE_PACKET_AUXDATA)
int val;
#endif
int err = 0;
struct packet_mreq mr;
#if defined(SO_BPF_EXTENSIONS) && defined(SKF_AD_VLAN_TAG_PRESENT)
int bpf_extensions;
socklen_t len = sizeof(bpf_extensions);
#endif







sock_fd = is_any_device ?
socket(PF_PACKET, SOCK_DGRAM, protocol) :
socket(PF_PACKET, SOCK_RAW, protocol);

if (sock_fd == -1) {
if (errno == EINVAL || errno == EAFNOSUPPORT) {




return 0;
}
if (errno == EPERM || errno == EACCES) {




ret = PCAP_ERROR_PERM_DENIED;
} else {



ret = PCAP_ERROR;
}
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
return ret;
}


handlep->sock_packet = 0;












handlep->lo_ifindex = iface_get_id(sock_fd, "lo", handle->errbuf);





handle->offset = 0;






if (!is_any_device) {

handlep->cooked = 0;

if (handle->opt.rfmon) {






err = enter_rfmon_mode(handle, sock_fd, device);
if (err < 0) {

close(sock_fd);
return err;
}
if (err == 0) {




close(sock_fd);
return PCAP_ERROR_RFMON_NOTSUP;
}







if (handlep->mondevice != NULL)
device = handlep->mondevice;
}
arptype = iface_get_arptype(sock_fd, device, handle->errbuf);
if (arptype < 0) {
close(sock_fd);
return arptype;
}
map_arphrd_to_dlt(handle, sock_fd, arptype, device, 1);
if (handle->linktype == -1 ||
handle->linktype == DLT_LINUX_SLL ||
handle->linktype == DLT_LINUX_IRDA ||
handle->linktype == DLT_LINUX_LAPD ||
handle->linktype == DLT_NETLINK ||
(handle->linktype == DLT_EN10MB &&
(strncmp("isdn", device, 4) == 0 ||
strncmp("isdY", device, 4) == 0))) {









if (close(sock_fd) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "close");
return PCAP_ERROR;
}
sock_fd = socket(PF_PACKET, SOCK_DGRAM, protocol);
if (sock_fd == -1) {
if (errno == EPERM || errno == EACCES) {




ret = PCAP_ERROR_PERM_DENIED;
} else {



ret = PCAP_ERROR;
}
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "socket");
return ret;
}
handlep->cooked = 1;






if (handle->dlt_list != NULL) {
free(handle->dlt_list);
handle->dlt_list = NULL;
handle->dlt_count = 0;
set_dlt_list_cooked(handle, sock_fd);
}

if (handle->linktype == -1) {






pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"arptype %d not "
"supported by libpcap - "
"falling back to cooked "
"socket",
arptype);
}






if (handle->linktype != DLT_LINUX_IRDA &&
handle->linktype != DLT_LINUX_LAPD &&
handle->linktype != DLT_NETLINK)
handle->linktype = DLT_LINUX_SLL;
}

handlep->ifindex = iface_get_id(sock_fd, device,
handle->errbuf);
if (handlep->ifindex == -1) {
close(sock_fd);
return PCAP_ERROR;
}

if ((err = iface_bind(sock_fd, handlep->ifindex,
handle->errbuf, protocol)) != 1) {
close(sock_fd);
if (err < 0)
return err;
else
return 0;
}
} else {



if (handle->opt.rfmon) {



close(sock_fd);
return PCAP_ERROR_RFMON_NOTSUP;
}




handlep->cooked = 1;
handle->linktype = DLT_LINUX_SLL;
handle->dlt_list = NULL;
handle->dlt_count = 0;
set_dlt_list_cooked(handle, sock_fd);








handlep->ifindex = -1;
}























if (!is_any_device && handle->opt.promisc) {
memset(&mr, 0, sizeof(mr));
mr.mr_ifindex = handlep->ifindex;
mr.mr_type = PACKET_MR_PROMISC;
if (setsockopt(sock_fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
&mr, sizeof(mr)) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "setsockopt (PACKET_ADD_MEMBERSHIP)");
close(sock_fd);
return PCAP_ERROR;
}
}



#if defined(HAVE_PACKET_AUXDATA)
val = 1;
if (setsockopt(sock_fd, SOL_PACKET, PACKET_AUXDATA, &val,
sizeof(val)) == -1 && errno != ENOPROTOOPT) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "setsockopt (PACKET_AUXDATA)");
close(sock_fd);
return PCAP_ERROR;
}
handle->offset += VLAN_TAG_LEN;
#endif



















if (handlep->cooked) {
if (handle->snapshot < SLL2_HDR_LEN + 1)
handle->snapshot = SLL2_HDR_LEN + 1;
}
handle->bufsize = handle->snapshot;





switch (handle->linktype) {

case DLT_EN10MB:




handlep->vlan_offset = 2 * ETH_ALEN;
break;

case DLT_LINUX_SLL:




handlep->vlan_offset = SLL_HDR_LEN - 2;
break;

default:
handlep->vlan_offset = -1;
break;
}

#if defined(SIOCGSTAMPNS) && defined(SO_TIMESTAMPNS)
if (handle->opt.tstamp_precision == PCAP_TSTAMP_PRECISION_NANO) {
int nsec_tstamps = 1;

if (setsockopt(sock_fd, SOL_SOCKET, SO_TIMESTAMPNS, &nsec_tstamps, sizeof(nsec_tstamps)) < 0) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE, "setsockopt: unable to set SO_TIMESTAMPNS");
close(sock_fd);
return PCAP_ERROR;
}
}
#endif




handle->fd = sock_fd;

#if defined(SO_BPF_EXTENSIONS) && defined(SKF_AD_VLAN_TAG_PRESENT)





if (getsockopt(sock_fd, SOL_SOCKET, SO_BPF_EXTENSIONS,
&bpf_extensions, &len) == 0) {
if (bpf_extensions >= SKF_AD_VLAN_TAG_PRESENT) {



handle->bpf_codegen_flags |= BPF_SPECIAL_VLAN_HANDLING;
}
}
#endif

return 1;
#else
pcap_strlcpy(ebuf,
"New packet capturing interface not supported by build "
"environment", PCAP_ERRBUF_SIZE);
return 0;
#endif
}

#if defined(HAVE_PACKET_RING)












static int
activate_mmap(pcap_t *handle, int *status)
{
struct pcap_linux *handlep = handle->priv;
int ret;





handlep->oneshot_buffer = malloc(handle->snapshot);
if (handlep->oneshot_buffer == NULL) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't allocate oneshot buffer");
*status = PCAP_ERROR;
return -1;
}

if (handle->opt.buffer_size == 0) {

handle->opt.buffer_size = 2*1024*1024;
}
ret = prepare_tpacket_socket(handle);
if (ret == -1) {
free(handlep->oneshot_buffer);
*status = PCAP_ERROR;
return ret;
}
ret = create_ring(handle, status);
if (ret == 0) {




free(handlep->oneshot_buffer);
return 0;
}
if (ret == -1) {




free(handlep->oneshot_buffer);
return -1;
}











switch (handlep->tp_version) {
case TPACKET_V1:
handle->read_op = pcap_read_linux_mmap_v1;
break;
case TPACKET_V1_64:
handle->read_op = pcap_read_linux_mmap_v1_64;
break;
#if defined(HAVE_TPACKET2)
case TPACKET_V2:
handle->read_op = pcap_read_linux_mmap_v2;
break;
#endif
#if defined(HAVE_TPACKET3)
case TPACKET_V3:
handle->read_op = pcap_read_linux_mmap_v3;
break;
#endif
}
handle->cleanup_op = pcap_cleanup_linux_mmap;
handle->setfilter_op = pcap_setfilter_linux_mmap;
handle->setnonblock_op = pcap_setnonblock_mmap;
handle->getnonblock_op = pcap_getnonblock_mmap;
handle->oneshot_callback = pcap_oneshot_mmap;
handle->selectable_fd = handle->fd;
return 1;
}
#else
static int
activate_mmap(pcap_t *handle _U_, int *status _U_)
{
return 0;
}
#endif

#if defined(HAVE_PACKET_RING)

#if defined(HAVE_TPACKET2) || defined(HAVE_TPACKET3)







static int
init_tpacket(pcap_t *handle, int version, const char *version_str)
{
struct pcap_linux *handlep = handle->priv;
int val = version;
socklen_t len = sizeof(val);








if (getsockopt(handle->fd, SOL_PACKET, PACKET_HDRLEN, &val, &len) < 0) {
if (errno == ENOPROTOOPT || errno == EINVAL) {






return 1;
}


pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't get %s header len on packet socket",
version_str);
return -1;
}
handlep->tp_hdrlen = val;

val = version;
if (setsockopt(handle->fd, SOL_PACKET, PACKET_VERSION, &val,
sizeof(val)) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't activate %s on packet socket", version_str);
return -1;
}
handlep->tp_version = version;





val = VLAN_TAG_LEN;
if (setsockopt(handle->fd, SOL_PACKET, PACKET_RESERVE, &val,
sizeof(val)) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't set up reserve on packet socket");
return -1;
}

return 0;
}
#endif

















#if defined(__i386__)
#define ISA_64_BIT "x86_64"
#elif defined(__ppc__)
#define ISA_64_BIT "ppc64"
#elif defined(__sparc__)
#define ISA_64_BIT "sparc64"
#elif defined(__s390__)
#define ISA_64_BIT "s390x"
#elif defined(__mips__)
#define ISA_64_BIT "mips64"
#elif defined(__hppa__)
#define ISA_64_BIT "parisc64"
#endif










static int
prepare_tpacket_socket(pcap_t *handle)
{
struct pcap_linux *handlep = handle->priv;
#if defined(HAVE_TPACKET2) || defined(HAVE_TPACKET3)
int ret;
#endif

#if defined(HAVE_TPACKET3)











if (!handle->opt.immediate) {
ret = init_tpacket(handle, TPACKET_V3, "TPACKET_V3");
if (ret == 0) {



return 1;
}
if (ret == -1) {




return -1;
}
}
#endif

#if defined(HAVE_TPACKET2)



ret = init_tpacket(handle, TPACKET_V2, "TPACKET_V2");
if (ret == 0) {



return 1;
}
if (ret == -1) {




return -1;
}
#endif







handlep->tp_version = TPACKET_V1;
handlep->tp_hdrlen = sizeof(struct tpacket_hdr);

#if defined(ISA_64_BIT)








if (sizeof(long) == 4) {



struct utsname utsname;

if (uname(&utsname) == -1) {



pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "uname failed");
return -1;
}
if (strcmp(utsname.machine, ISA_64_BIT) == 0) {








handlep->tp_version = TPACKET_V1_64;
handlep->tp_hdrlen = sizeof(struct tpacket_hdr_64);
}
}
#endif

return 1;
}

#define MAX(a,b) ((a)>(b)?(a):(b))













static int
create_ring(pcap_t *handle, int *status)
{
struct pcap_linux *handlep = handle->priv;
unsigned i, j, frames_per_block;
#if defined(HAVE_TPACKET3)





struct tpacket_req3 req;
#else
struct tpacket_req req;
#endif
socklen_t len;
unsigned int sk_type, tp_reserve, maclen, tp_hdrlen, netoff, macoff;
unsigned int frame_size;




*status = 0;

switch (handlep->tp_version) {

case TPACKET_V1:
case TPACKET_V1_64:
#if defined(HAVE_TPACKET2)
case TPACKET_V2:
#endif































frame_size = handle->snapshot;
if (handle->linktype == DLT_EN10MB) {
unsigned int max_frame_len;
int mtu;
int offload;

mtu = iface_get_mtu(handle->fd, handle->opt.device,
handle->errbuf);
if (mtu == -1) {
*status = PCAP_ERROR;
return -1;
}
offload = iface_get_offload(handle);
if (offload == -1) {
*status = PCAP_ERROR;
return -1;
}
if (offload)
max_frame_len = MAX(mtu, 65535);
else
max_frame_len = mtu;
max_frame_len += 18;

if (frame_size > max_frame_len)
frame_size = max_frame_len;
}




len = sizeof(sk_type);
if (getsockopt(handle->fd, SOL_SOCKET, SO_TYPE, &sk_type,
&len) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "getsockopt (SO_TYPE)");
*status = PCAP_ERROR;
return -1;
}
#if defined(PACKET_RESERVE)
len = sizeof(tp_reserve);
if (getsockopt(handle->fd, SOL_PACKET, PACKET_RESERVE,
&tp_reserve, &len) < 0) {
if (errno != ENOPROTOOPT) {





pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"getsockopt (PACKET_RESERVE)");
*status = PCAP_ERROR;
return -1;
}





tp_reserve = 0;
} else {














tp_reserve += SLL2_HDR_LEN - SLL_HDR_LEN;
len = sizeof(tp_reserve);
if (setsockopt(handle->fd, SOL_PACKET, PACKET_RESERVE,
&tp_reserve, len) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"setsockopt (PACKET_RESERVE)");
*status = PCAP_ERROR;
return -1;
}
}
#else





tp_reserve = 0;
#endif
maclen = (sk_type == SOCK_DGRAM) ? 0 : MAX_LINKHEADER_SIZE;














tp_hdrlen = TPACKET_ALIGN(handlep->tp_hdrlen) + sizeof(struct sockaddr_ll) ;
netoff = TPACKET_ALIGN(tp_hdrlen + (maclen < 16 ? 16 : maclen)) + tp_reserve;











macoff = netoff - maclen;
req.tp_frame_size = TPACKET_ALIGN(macoff + frame_size);







req.tp_frame_nr = (handle->opt.buffer_size + req.tp_frame_size - 1)/req.tp_frame_size;
break;

#if defined(HAVE_TPACKET3)
case TPACKET_V3:



len = sizeof(tp_reserve);
if (getsockopt(handle->fd, SOL_PACKET, PACKET_RESERVE,
&tp_reserve, &len) < 0) {






pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"getsockopt (PACKET_RESERVE)");
*status = PCAP_ERROR;
return -1;
}














tp_reserve += SLL2_HDR_LEN - SLL_HDR_LEN;
len = sizeof(tp_reserve);
if (setsockopt(handle->fd, SOL_PACKET, PACKET_RESERVE,
&tp_reserve, len) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"setsockopt (PACKET_RESERVE)");
*status = PCAP_ERROR;
return -1;
}







req.tp_frame_size = MAXIMUM_SNAPLEN;







req.tp_frame_nr = (handle->opt.buffer_size + req.tp_frame_size - 1)/req.tp_frame_size;
break;
#endif
default:
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Internal error: unknown TPACKET_ value %u",
handlep->tp_version);
*status = PCAP_ERROR;
return -1;
}





req.tp_block_size = getpagesize();
while (req.tp_block_size < req.tp_frame_size)
req.tp_block_size <<= 1;

frames_per_block = req.tp_block_size/req.tp_frame_size;














#if defined(HAVE_LINUX_NET_TSTAMP_H) && defined(PACKET_TIMESTAMP)







if (handle->opt.tstamp_type == PCAP_TSTAMP_ADAPTER ||
handle->opt.tstamp_type == PCAP_TSTAMP_ADAPTER_UNSYNCED) {
struct hwtstamp_config hwconfig;
struct ifreq ifr;
int timesource;





memset(&hwconfig, 0, sizeof(hwconfig));
hwconfig.tx_type = HWTSTAMP_TX_ON;
hwconfig.rx_filter = HWTSTAMP_FILTER_ALL;

memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, handle->opt.device, sizeof(ifr.ifr_name));
ifr.ifr_data = (void *)&hwconfig;

if (ioctl(handle->fd, SIOCSHWTSTAMP, &ifr) < 0) {
switch (errno) {

case EPERM:







*status = PCAP_ERROR_PERM_DENIED;
return -1;

case EOPNOTSUPP:
case ERANGE:














*status = PCAP_WARNING_TSTAMP_TYPE_NOTSUP;
break;

default:
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCSHWTSTAMP failed");
*status = PCAP_ERROR;
return -1;
}
} else {





if (handle->opt.tstamp_type == PCAP_TSTAMP_ADAPTER) {




timesource = SOF_TIMESTAMPING_SYS_HARDWARE;
} else {





timesource = SOF_TIMESTAMPING_RAW_HARDWARE;
}
if (setsockopt(handle->fd, SOL_PACKET, PACKET_TIMESTAMP,
(void *)&timesource, sizeof(timesource))) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"can't set PACKET_TIMESTAMP");
*status = PCAP_ERROR;
return -1;
}
}
}
#endif


retry:
req.tp_block_nr = req.tp_frame_nr / frames_per_block;


req.tp_frame_nr = req.tp_block_nr * frames_per_block;

#if defined(HAVE_TPACKET3)

if (handlep->timeout > 0) {

req.tp_retire_blk_tov = handlep->timeout;
} else if (handlep->timeout == 0) {








req.tp_retire_blk_tov = UINT_MAX;
} else {




req.tp_retire_blk_tov = 0;
}

req.tp_sizeof_priv = 0;

req.tp_feature_req_word = 0;
#endif

if (setsockopt(handle->fd, SOL_PACKET, PACKET_RX_RING,
(void *) &req, sizeof(req))) {
if ((errno == ENOMEM) && (req.tp_block_nr > 1)) {









if (req.tp_frame_nr < 20)
req.tp_frame_nr -= 1;
else
req.tp_frame_nr -= req.tp_frame_nr/20;
goto retry;
}
if (errno == ENOPROTOOPT) {



return 0;
}
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't create rx ring on packet socket");
*status = PCAP_ERROR;
return -1;
}


handlep->mmapbuflen = req.tp_block_nr * req.tp_block_size;
handlep->mmapbuf = mmap(0, handlep->mmapbuflen,
PROT_READ|PROT_WRITE, MAP_SHARED, handle->fd, 0);
if (handlep->mmapbuf == MAP_FAILED) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't mmap rx ring");


destroy_ring(handle);
*status = PCAP_ERROR;
return -1;
}


handle->cc = req.tp_frame_nr;
handle->buffer = malloc(handle->cc * sizeof(union thdr *));
if (!handle->buffer) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't allocate ring of frame headers");

destroy_ring(handle);
*status = PCAP_ERROR;
return -1;
}


handle->offset = 0;
for (i=0; i<req.tp_block_nr; ++i) {
void *base = &handlep->mmapbuf[i*req.tp_block_size];
for (j=0; j<frames_per_block; ++j, ++handle->offset) {
RING_GET_CURRENT_FRAME(handle) = base;
base += req.tp_frame_size;
}
}

handle->bufsize = req.tp_frame_size;
handle->offset = 0;
return 1;
}


static void
destroy_ring(pcap_t *handle)
{
struct pcap_linux *handlep = handle->priv;


struct tpacket_req req;
memset(&req, 0, sizeof(req));

(void)setsockopt(handle->fd, SOL_PACKET, PACKET_RX_RING,
(void *) &req, sizeof(req));


if (handlep->mmapbuf) {

(void)munmap(handlep->mmapbuf, handlep->mmapbuflen);
handlep->mmapbuf = NULL;
}
}


















static void
pcap_oneshot_mmap(u_char *user, const struct pcap_pkthdr *h,
const u_char *bytes)
{
struct oneshot_userdata *sp = (struct oneshot_userdata *)user;
pcap_t *handle = sp->pd;
struct pcap_linux *handlep = handle->priv;

*sp->hdr = *h;
memcpy(handlep->oneshot_buffer, bytes, h->caplen);
*sp->pkt = handlep->oneshot_buffer;
}

static void
pcap_cleanup_linux_mmap( pcap_t *handle )
{
struct pcap_linux *handlep = handle->priv;

destroy_ring(handle);
if (handlep->oneshot_buffer != NULL) {
free(handlep->oneshot_buffer);
handlep->oneshot_buffer = NULL;
}
pcap_cleanup_linux(handle);
}


static int
pcap_getnonblock_mmap(pcap_t *handle)
{
struct pcap_linux *handlep = handle->priv;


return (handlep->timeout<0);
}

static int
pcap_setnonblock_mmap(pcap_t *handle, int nonblock)
{
struct pcap_linux *handlep = handle->priv;





if (pcap_setnonblock_fd(handle, nonblock) == -1)
return -1;





if (nonblock) {
if (handlep->timeout >= 0) {




handlep->timeout = ~handlep->timeout;
}
} else {
if (handlep->timeout < 0) {
handlep->timeout = ~handlep->timeout;
}
}

set_poll_timeout(handlep);
return 0;
}




static inline int
pcap_get_ring_frame_status(pcap_t *handle, int offset)
{
struct pcap_linux *handlep = handle->priv;
union thdr h;

h.raw = RING_GET_FRAME_AT(handle, offset);
switch (handlep->tp_version) {
case TPACKET_V1:
return (h.h1->tp_status);
break;
case TPACKET_V1_64:
return (h.h1_64->tp_status);
break;
#if defined(HAVE_TPACKET2)
case TPACKET_V2:
return (h.h2->tp_status);
break;
#endif
#if defined(HAVE_TPACKET3)
case TPACKET_V3:
return (h.h3->hdr.bh1.block_status);
break;
#endif
}

return 0;
}

#if !defined(POLLRDHUP)
#define POLLRDHUP 0
#endif




static int pcap_wait_for_frames_mmap(pcap_t *handle)
{
struct pcap_linux *handlep = handle->priv;
char c;
struct pollfd pollinfo;
int ret;

pollinfo.fd = handle->fd;
pollinfo.events = POLLIN;

do {








ret = poll(&pollinfo, 1, handlep->poll_timeout);
if (ret < 0 && errno != EINTR) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"can't poll on packet socket");
return PCAP_ERROR;
} else if (ret > 0 &&
(pollinfo.revents & (POLLHUP|POLLRDHUP|POLLERR|POLLNVAL))) {





if (pollinfo.revents & (POLLHUP | POLLRDHUP)) {
pcap_snprintf(handle->errbuf,
PCAP_ERRBUF_SIZE,
"Hangup on packet socket");
return PCAP_ERROR;
}
if (pollinfo.revents & POLLERR) {





if (recv(handle->fd, &c, sizeof c,
MSG_PEEK) != -1)
continue;
if (errno == ENETDOWN) {









pcap_snprintf(handle->errbuf,
PCAP_ERRBUF_SIZE,
"The interface went down");
} else {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"Error condition on packet socket");
}
return PCAP_ERROR;
}
if (pollinfo.revents & POLLNVAL) {
pcap_snprintf(handle->errbuf,
PCAP_ERRBUF_SIZE,
"Invalid polling request on packet socket");
return PCAP_ERROR;
}
}

if (handle->break_loop) {
handle->break_loop = 0;
return PCAP_ERROR_BREAK;
}
} while (ret < 0);
return 0;
}


static int pcap_handle_packet_mmap(
pcap_t *handle,
pcap_handler callback,
u_char *user,
unsigned char *frame,
unsigned int tp_len,
unsigned int tp_mac,
unsigned int tp_snaplen,
unsigned int tp_sec,
unsigned int tp_usec,
int tp_vlan_tci_valid,
__u16 tp_vlan_tci,
__u16 tp_vlan_tpid)
{
struct pcap_linux *handlep = handle->priv;
unsigned char *bp;
struct sockaddr_ll *sll;
struct pcap_pkthdr pcaphdr;
unsigned int snaplen = tp_snaplen;
struct utsname utsname;


if (tp_mac + tp_snaplen > handle->bufsize) {



if (uname(&utsname) != -1) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"corrupted frame on kernel ring mac "
"offset %u + caplen %u > frame len %d "
"(kernel %.32s version %s, machine %.16s)",
tp_mac, tp_snaplen, handle->bufsize,
utsname.release, utsname.version,
utsname.machine);
} else {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"corrupted frame on kernel ring mac "
"offset %u + caplen %u > frame len %d",
tp_mac, tp_snaplen, handle->bufsize);
}
return -1;
}










bp = frame + tp_mac;


sll = (void *)frame + TPACKET_ALIGN(handlep->tp_hdrlen);
if (handlep->cooked) {
if (handle->linktype == DLT_LINUX_SLL2) {
struct sll2_header *hdrp;








bp -= SLL2_HDR_LEN;








if (bp < (u_char *)frame +
TPACKET_ALIGN(handlep->tp_hdrlen) +
sizeof(struct sockaddr_ll)) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"cooked-mode frame doesn't have room for sll header");
return -1;
}




hdrp = (struct sll2_header *)bp;
hdrp->sll2_protocol = sll->sll_protocol;
hdrp->sll2_reserved_mbz = 0;
hdrp->sll2_if_index = htonl(sll->sll_ifindex);
hdrp->sll2_hatype = htons(sll->sll_hatype);
hdrp->sll2_pkttype = sll->sll_pkttype;
hdrp->sll2_halen = sll->sll_halen;
memcpy(hdrp->sll2_addr, sll->sll_addr, SLL_ADDRLEN);

snaplen += sizeof(struct sll2_header);
} else {
struct sll_header *hdrp;








bp -= SLL_HDR_LEN;








if (bp < (u_char *)frame +
TPACKET_ALIGN(handlep->tp_hdrlen) +
sizeof(struct sockaddr_ll)) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"cooked-mode frame doesn't have room for sll header");
return -1;
}




hdrp = (struct sll_header *)bp;
hdrp->sll_pkttype = htons(sll->sll_pkttype);
hdrp->sll_hatype = htons(sll->sll_hatype);
hdrp->sll_halen = htons(sll->sll_halen);
memcpy(hdrp->sll_addr, sll->sll_addr, SLL_ADDRLEN);
hdrp->sll_protocol = sll->sll_protocol;

snaplen += sizeof(struct sll_header);
}
}

if (handlep->filter_in_userland && handle->fcode.bf_insns) {
struct bpf_aux_data aux_data;

aux_data.vlan_tag_present = tp_vlan_tci_valid;
aux_data.vlan_tag = tp_vlan_tci & 0x0fff;

if (bpf_filter_with_aux_data(handle->fcode.bf_insns,
bp,
tp_len,
snaplen,
&aux_data) == 0)
return 0;
}

if (!linux_check_direction(handle, sll))
return 0;


pcaphdr.ts.tv_sec = tp_sec;
pcaphdr.ts.tv_usec = tp_usec;
pcaphdr.caplen = tp_snaplen;
pcaphdr.len = tp_len;


if (handlep->cooked) {

if (handle->linktype == DLT_LINUX_SLL2) {
pcaphdr.caplen += SLL2_HDR_LEN;
pcaphdr.len += SLL2_HDR_LEN;
} else {
pcaphdr.caplen += SLL_HDR_LEN;
pcaphdr.len += SLL_HDR_LEN;
}
}

#if defined(HAVE_TPACKET2) || defined(HAVE_TPACKET3)
if (tp_vlan_tci_valid &&
handlep->vlan_offset != -1 &&
tp_snaplen >= (unsigned int) handlep->vlan_offset)
{
struct vlan_tag *tag;






bp -= VLAN_TAG_LEN;
memmove(bp, bp + VLAN_TAG_LEN, handlep->vlan_offset);




tag = (struct vlan_tag *)(bp + handlep->vlan_offset);
tag->vlan_tpid = htons(tp_vlan_tpid);
tag->vlan_tci = htons(tp_vlan_tci);




pcaphdr.caplen += VLAN_TAG_LEN;
pcaphdr.len += VLAN_TAG_LEN;
}
#endif










if (pcaphdr.caplen > (bpf_u_int32)handle->snapshot)
pcaphdr.caplen = handle->snapshot;


callback(user, &pcaphdr, bp);

return 1;
}

static int
pcap_read_linux_mmap_v1(pcap_t *handle, int max_packets, pcap_handler callback,
u_char *user)
{
struct pcap_linux *handlep = handle->priv;
union thdr h;
int pkts = 0;
int ret;


h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h1->tp_status == TP_STATUS_KERNEL) {




ret = pcap_wait_for_frames_mmap(handle);
if (ret) {
return ret;
}
}



while ((pkts < max_packets) || PACKET_COUNT_IS_UNLIMITED(max_packets)) {




h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h1->tp_status == TP_STATUS_KERNEL)
break;

ret = pcap_handle_packet_mmap(
handle,
callback,
user,
h.raw,
h.h1->tp_len,
h.h1->tp_mac,
h.h1->tp_snaplen,
h.h1->tp_sec,
h.h1->tp_usec,
0,
0,
0);
if (ret == 1) {
pkts++;
handlep->packets_read++;
} else if (ret < 0) {
return ret;
}







h.h1->tp_status = TP_STATUS_KERNEL;
if (handlep->blocks_to_filter_in_userland > 0) {
handlep->blocks_to_filter_in_userland--;
if (handlep->blocks_to_filter_in_userland == 0) {




handlep->filter_in_userland = 0;
}
}


if (++handle->offset >= handle->cc)
handle->offset = 0;


if (handle->break_loop) {
handle->break_loop = 0;
return PCAP_ERROR_BREAK;
}
}
return pkts;
}

static int
pcap_read_linux_mmap_v1_64(pcap_t *handle, int max_packets, pcap_handler callback,
u_char *user)
{
struct pcap_linux *handlep = handle->priv;
union thdr h;
int pkts = 0;
int ret;


h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h1_64->tp_status == TP_STATUS_KERNEL) {




ret = pcap_wait_for_frames_mmap(handle);
if (ret) {
return ret;
}
}



while ((pkts < max_packets) || PACKET_COUNT_IS_UNLIMITED(max_packets)) {




h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h1_64->tp_status == TP_STATUS_KERNEL)
break;

ret = pcap_handle_packet_mmap(
handle,
callback,
user,
h.raw,
h.h1_64->tp_len,
h.h1_64->tp_mac,
h.h1_64->tp_snaplen,
h.h1_64->tp_sec,
h.h1_64->tp_usec,
0,
0,
0);
if (ret == 1) {
pkts++;
handlep->packets_read++;
} else if (ret < 0) {
return ret;
}







h.h1_64->tp_status = TP_STATUS_KERNEL;
if (handlep->blocks_to_filter_in_userland > 0) {
handlep->blocks_to_filter_in_userland--;
if (handlep->blocks_to_filter_in_userland == 0) {




handlep->filter_in_userland = 0;
}
}


if (++handle->offset >= handle->cc)
handle->offset = 0;


if (handle->break_loop) {
handle->break_loop = 0;
return PCAP_ERROR_BREAK;
}
}
return pkts;
}

#if defined(HAVE_TPACKET2)
static int
pcap_read_linux_mmap_v2(pcap_t *handle, int max_packets, pcap_handler callback,
u_char *user)
{
struct pcap_linux *handlep = handle->priv;
union thdr h;
int pkts = 0;
int ret;


h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h2->tp_status == TP_STATUS_KERNEL) {




ret = pcap_wait_for_frames_mmap(handle);
if (ret) {
return ret;
}
}



while ((pkts < max_packets) || PACKET_COUNT_IS_UNLIMITED(max_packets)) {




h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h2->tp_status == TP_STATUS_KERNEL)
break;

ret = pcap_handle_packet_mmap(
handle,
callback,
user,
h.raw,
h.h2->tp_len,
h.h2->tp_mac,
h.h2->tp_snaplen,
h.h2->tp_sec,
handle->opt.tstamp_precision == PCAP_TSTAMP_PRECISION_NANO ? h.h2->tp_nsec : h.h2->tp_nsec / 1000,
VLAN_VALID(h.h2, h.h2),
h.h2->tp_vlan_tci,
VLAN_TPID(h.h2, h.h2));
if (ret == 1) {
pkts++;
handlep->packets_read++;
} else if (ret < 0) {
return ret;
}







h.h2->tp_status = TP_STATUS_KERNEL;
if (handlep->blocks_to_filter_in_userland > 0) {
handlep->blocks_to_filter_in_userland--;
if (handlep->blocks_to_filter_in_userland == 0) {




handlep->filter_in_userland = 0;
}
}


if (++handle->offset >= handle->cc)
handle->offset = 0;


if (handle->break_loop) {
handle->break_loop = 0;
return PCAP_ERROR_BREAK;
}
}
return pkts;
}
#endif

#if defined(HAVE_TPACKET3)
static int
pcap_read_linux_mmap_v3(pcap_t *handle, int max_packets, pcap_handler callback,
u_char *user)
{
struct pcap_linux *handlep = handle->priv;
union thdr h;
int pkts = 0;
int ret;

again:
if (handlep->current_packet == NULL) {

h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h3->hdr.bh1.block_status == TP_STATUS_KERNEL) {




ret = pcap_wait_for_frames_mmap(handle);
if (ret) {
return ret;
}
}
}
h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h3->hdr.bh1.block_status == TP_STATUS_KERNEL) {
if (pkts == 0 && handlep->timeout == 0) {

goto again;
}
return pkts;
}



while ((pkts < max_packets) || PACKET_COUNT_IS_UNLIMITED(max_packets)) {
int packets_to_read;

if (handlep->current_packet == NULL) {
h.raw = RING_GET_CURRENT_FRAME(handle);
if (h.h3->hdr.bh1.block_status == TP_STATUS_KERNEL)
break;

handlep->current_packet = h.raw + h.h3->hdr.bh1.offset_to_first_pkt;
handlep->packets_left = h.h3->hdr.bh1.num_pkts;
}
packets_to_read = handlep->packets_left;

if (!PACKET_COUNT_IS_UNLIMITED(max_packets) &&
packets_to_read > (max_packets - pkts)) {






packets_to_read = max_packets - pkts;
}

while (packets_to_read-- && !handle->break_loop) {
struct tpacket3_hdr* tp3_hdr = (struct tpacket3_hdr*) handlep->current_packet;
ret = pcap_handle_packet_mmap(
handle,
callback,
user,
handlep->current_packet,
tp3_hdr->tp_len,
tp3_hdr->tp_mac,
tp3_hdr->tp_snaplen,
tp3_hdr->tp_sec,
handle->opt.tstamp_precision == PCAP_TSTAMP_PRECISION_NANO ? tp3_hdr->tp_nsec : tp3_hdr->tp_nsec / 1000,
VLAN_VALID(tp3_hdr, &tp3_hdr->hv1),
tp3_hdr->hv1.tp_vlan_tci,
VLAN_TPID(tp3_hdr, &tp3_hdr->hv1));
if (ret == 1) {
pkts++;
handlep->packets_read++;
} else if (ret < 0) {
handlep->current_packet = NULL;
return ret;
}
handlep->current_packet += tp3_hdr->tp_next_offset;
handlep->packets_left--;
}

if (handlep->packets_left <= 0) {







h.h3->hdr.bh1.block_status = TP_STATUS_KERNEL;
if (handlep->blocks_to_filter_in_userland > 0) {
handlep->blocks_to_filter_in_userland--;
if (handlep->blocks_to_filter_in_userland == 0) {




handlep->filter_in_userland = 0;
}
}


if (++handle->offset >= handle->cc)
handle->offset = 0;

handlep->current_packet = NULL;
}


if (handle->break_loop) {
handle->break_loop = 0;
return PCAP_ERROR_BREAK;
}
}
if (pkts == 0 && handlep->timeout == 0) {

goto again;
}
return pkts;
}
#endif

static int
pcap_setfilter_linux_mmap(pcap_t *handle, struct bpf_program *filter)
{
struct pcap_linux *handlep = handle->priv;
int n, offset;
int ret;







ret = pcap_setfilter_linux_common(handle, filter, 1);
if (ret < 0)
return ret;





if (handlep->filter_in_userland)
return ret;










offset = handle->offset;
if (--offset < 0)
offset = handle->cc - 1;
for (n=0; n < handle->cc; ++n) {
if (--offset < 0)
offset = handle->cc - 1;
if (pcap_get_ring_frame_status(handle, offset) != TP_STATUS_KERNEL)
break;
}















if (n != 0)
n--;












handlep->blocks_to_filter_in_userland = handle->cc - n;
handlep->filter_in_userland = 1;
return ret;
}

#endif


#if defined(HAVE_PF_PACKET_SOCKETS)




static int
iface_get_id(int fd, const char *device, char *ebuf)
{
struct ifreq ifr;

memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));

if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFINDEX");
return -1;
}

return ifr.ifr_ifindex;
}






static int
iface_bind(int fd, int ifindex, char *ebuf, int protocol)
{
struct sockaddr_ll sll;
int err;
socklen_t errlen = sizeof(err);

memset(&sll, 0, sizeof(sll));
sll.sll_family = AF_PACKET;
sll.sll_ifindex = ifindex;
sll.sll_protocol = protocol;

if (bind(fd, (struct sockaddr *) &sll, sizeof(sll)) == -1) {
if (errno == ENETDOWN) {







return PCAP_ERROR_IFACE_NOT_UP;
} else {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "bind");
return PCAP_ERROR;
}
}



if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "getsockopt (SO_ERROR)");
return 0;
}

if (err == ENETDOWN) {







return PCAP_ERROR_IFACE_NOT_UP;
} else if (err > 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
err, "bind");
return 0;
}

return 1;
}

#if defined(IW_MODE_MONITOR)





static int
has_wext(int sock_fd, const char *device, char *ebuf)
{
struct iwreq ireq;
int ret;

if (is_bonding_device(sock_fd, device))
return 0;

pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
if (ioctl(sock_fd, SIOCGIWNAME, &ireq) >= 0)
return 1;
if (errno == ENODEV)
ret = PCAP_ERROR_NO_SUCH_DEVICE;
else
ret = 0;
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE, errno,
"%s: SIOCGIWNAME", device);
return ret;
}










typedef enum {
MONITOR_WEXT,
MONITOR_HOSTAP,
MONITOR_PRISM,
MONITOR_PRISM54,
MONITOR_ACX100,
MONITOR_RT2500,
MONITOR_RT2570,
MONITOR_RT73,
MONITOR_RTL8XXX
} monitor_type;









static int
enter_rfmon_mode_wext(pcap_t *handle, int sock_fd, const char *device)
{


























































struct pcap_linux *handlep = handle->priv;
int err;
struct iwreq ireq;
struct iw_priv_args *priv;
monitor_type montype;
int i;
__u32 cmd;
struct ifreq ifr;
int oldflags;
int args[2];
int channel;




err = has_wext(sock_fd, device, handle->errbuf);
if (err <= 0)
return err;




montype = MONITOR_WEXT;
cmd = 0;











memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
ireq.u.data.pointer = (void *)args;
ireq.u.data.length = 0;
ireq.u.data.flags = 0;
if (ioctl(sock_fd, SIOCGIWPRIV, &ireq) != -1) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"%s: SIOCGIWPRIV with a zero-length buffer didn't fail!",
device);
return PCAP_ERROR;
}
if (errno != EOPNOTSUPP) {



if (errno != E2BIG) {



pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "%s: SIOCGIWPRIV", device);
return PCAP_ERROR;
}




priv = malloc(ireq.u.data.length * sizeof (struct iw_priv_args));
if (priv == NULL) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "malloc");
return PCAP_ERROR;
}
ireq.u.data.pointer = (void *)priv;
if (ioctl(sock_fd, SIOCGIWPRIV, &ireq) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "%s: SIOCGIWPRIV", device);
free(priv);
return PCAP_ERROR;
}





for (i = 0; i < ireq.u.data.length; i++) {
if (strcmp(priv[i].name, "monitor_type") == 0) {









if ((priv[i].set_args & IW_PRIV_TYPE_MASK) != IW_PRIV_TYPE_INT)
break;
if (!(priv[i].set_args & IW_PRIV_SIZE_FIXED))
break;
if ((priv[i].set_args & IW_PRIV_SIZE_MASK) != 1)
break;
montype = MONITOR_HOSTAP;
cmd = priv[i].cmd;
break;
}
if (strcmp(priv[i].name, "set_prismhdr") == 0) {






if ((priv[i].set_args & IW_PRIV_TYPE_MASK) != IW_PRIV_TYPE_INT)
break;
if (!(priv[i].set_args & IW_PRIV_SIZE_FIXED))
break;
if ((priv[i].set_args & IW_PRIV_SIZE_MASK) != 1)
break;
montype = MONITOR_PRISM54;
cmd = priv[i].cmd;
break;
}
if (strcmp(priv[i].name, "forceprismheader") == 0) {






if ((priv[i].set_args & IW_PRIV_TYPE_MASK) != IW_PRIV_TYPE_INT)
break;
if (!(priv[i].set_args & IW_PRIV_SIZE_FIXED))
break;
if ((priv[i].set_args & IW_PRIV_SIZE_MASK) != 1)
break;
montype = MONITOR_RT2570;
cmd = priv[i].cmd;
break;
}
if (strcmp(priv[i].name, "forceprism") == 0) {







if ((priv[i].set_args & IW_PRIV_TYPE_MASK) != IW_PRIV_TYPE_CHAR)
break;
if (priv[i].set_args & IW_PRIV_SIZE_FIXED)
break;
montype = MONITOR_RT73;
cmd = priv[i].cmd;
break;
}
if (strcmp(priv[i].name, "prismhdr") == 0) {






if ((priv[i].set_args & IW_PRIV_TYPE_MASK) != IW_PRIV_TYPE_INT)
break;
if (!(priv[i].set_args & IW_PRIV_SIZE_FIXED))
break;
if ((priv[i].set_args & IW_PRIV_SIZE_MASK) != 1)
break;
montype = MONITOR_RTL8XXX;
cmd = priv[i].cmd;
break;
}
if (strcmp(priv[i].name, "rfmontx") == 0) {











if ((priv[i].set_args & IW_PRIV_TYPE_MASK) != IW_PRIV_TYPE_INT)
break;
if ((priv[i].set_args & IW_PRIV_SIZE_MASK) != 2)
break;
montype = MONITOR_RT2500;
cmd = priv[i].cmd;
break;
}
if (strcmp(priv[i].name, "monitor") == 0) {















if ((priv[i].set_args & IW_PRIV_TYPE_MASK) != IW_PRIV_TYPE_INT)
break;
if (!(priv[i].set_args & IW_PRIV_SIZE_FIXED))
break;
switch (priv[i].set_args & IW_PRIV_SIZE_MASK) {

case 1:
montype = MONITOR_PRISM;
cmd = priv[i].cmd;
break;

case 2:
montype = MONITOR_ACX100;
cmd = priv[i].cmd;
break;

default:
break;
}
}
}
free(priv);
}








pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
if (ioctl(sock_fd, SIOCGIWMODE, &ireq) == -1) {



return PCAP_ERROR_RFMON_NOTSUP;
}




if (ireq.u.mode == IW_MODE_MONITOR) {







return 1;
}








if (!pcap_do_addexit(handle)) {




return PCAP_ERROR_RFMON_NOTSUP;
}




handlep->oldmode = ireq.u.mode;





if (montype == MONITOR_PRISM) {







memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
ireq.u.data.length = 1;
args[0] = 3;
memcpy(ireq.u.name, args, sizeof (int));
if (ioctl(sock_fd, cmd, &ireq) != -1) {





handlep->must_do_on_close |= MUST_CLEAR_RFMON;





pcap_add_to_pcaps_to_close(handle);

return 1;
}




}





memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s: Can't get flags", device);
return PCAP_ERROR;
}
oldflags = 0;
if (ifr.ifr_flags & IFF_UP) {
oldflags = ifr.ifr_flags;
ifr.ifr_flags &= ~IFF_UP;
if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "%s: Can't set flags",
device);
return PCAP_ERROR;
}
}




pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
ireq.u.mode = IW_MODE_MONITOR;
if (ioctl(sock_fd, SIOCSIWMODE, &ireq) == -1) {




ifr.ifr_flags = oldflags;
if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "%s: Can't set flags",
device);
return PCAP_ERROR;
}
return PCAP_ERROR_RFMON_NOTSUP;
}










switch (montype) {

case MONITOR_WEXT:



break;

case MONITOR_HOSTAP:



memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 3;
memcpy(ireq.u.name, args, sizeof (int));
if (ioctl(sock_fd, cmd, &ireq) != -1)
break;




memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 2;
memcpy(ireq.u.name, args, sizeof (int));
if (ioctl(sock_fd, cmd, &ireq) != -1)
break;




memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 1;
memcpy(ireq.u.name, args, sizeof (int));
ioctl(sock_fd, cmd, &ireq);
break;

case MONITOR_PRISM:



break;

case MONITOR_PRISM54:



memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 3;
memcpy(ireq.u.name, args, sizeof (int));
ioctl(sock_fd, cmd, &ireq);
break;

case MONITOR_ACX100:



memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
if (ioctl(sock_fd, SIOCGIWFREQ, &ireq) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "%s: SIOCGIWFREQ", device);
return PCAP_ERROR;
}
channel = ireq.u.freq.m;





memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 1;
args[1] = channel;
memcpy(ireq.u.name, args, 2*sizeof (int));
ioctl(sock_fd, cmd, &ireq);
break;

case MONITOR_RT2500:




memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 0;
memcpy(ireq.u.name, args, sizeof (int));
ioctl(sock_fd, cmd, &ireq);
break;

case MONITOR_RT2570:



memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 1;
memcpy(ireq.u.name, args, sizeof (int));
ioctl(sock_fd, cmd, &ireq);
break;

case MONITOR_RT73:



memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
ireq.u.data.length = 1;
ireq.u.data.pointer = "1";
ireq.u.data.flags = 0;
ioctl(sock_fd, cmd, &ireq);
break;

case MONITOR_RTL8XXX:



memset(&ireq, 0, sizeof ireq);
pcap_strlcpy(ireq.ifr_ifrn.ifrn_name, device,
sizeof ireq.ifr_ifrn.ifrn_name);
args[0] = 1;
memcpy(ireq.u.name, args, sizeof (int));
ioctl(sock_fd, cmd, &ireq);
break;
}




if (oldflags != 0) {
ifr.ifr_flags = oldflags;
if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "%s: Can't set flags",
device);





if (ioctl(handle->fd, SIOCSIWMODE, &ireq) == -1) {



fprintf(stderr,
"Can't restore interface wireless mode (SIOCSIWMODE failed: %s).\n"
"Please adjust manually.\n",
strerror(errno));
}
return PCAP_ERROR;
}
}





handlep->must_do_on_close |= MUST_CLEAR_RFMON;




pcap_add_to_pcaps_to_close(handle);

return 1;
}
#endif




static int
enter_rfmon_mode(pcap_t *handle, int sock_fd, const char *device)
{
#if defined(HAVE_LIBNL) || defined(IW_MODE_MONITOR)
int ret;
#endif

#if defined(HAVE_LIBNL)
ret = enter_rfmon_mode_mac80211(handle, sock_fd, device);
if (ret < 0)
return ret;
if (ret == 1)
return 1;
#endif

#if defined(IW_MODE_MONITOR)
ret = enter_rfmon_mode_wext(handle, sock_fd, device);
if (ret < 0)
return ret;
if (ret == 1)
return 1;
#endif






return 0;
}

#if defined(HAVE_LINUX_NET_TSTAMP_H) && defined(PACKET_TIMESTAMP)



static const struct {
int soft_timestamping_val;
int pcap_tstamp_val;
} sof_ts_type_map[3] = {
{ SOF_TIMESTAMPING_SOFTWARE, PCAP_TSTAMP_HOST },
{ SOF_TIMESTAMPING_SYS_HARDWARE, PCAP_TSTAMP_ADAPTER },
{ SOF_TIMESTAMPING_RAW_HARDWARE, PCAP_TSTAMP_ADAPTER_UNSYNCED }
};
#define NUM_SOF_TIMESTAMPING_TYPES (sizeof sof_ts_type_map / sizeof sof_ts_type_map[0])




static void
iface_set_all_ts_types(pcap_t *handle)
{
u_int i;

handle->tstamp_type_count = NUM_SOF_TIMESTAMPING_TYPES;
handle->tstamp_type_list = malloc(NUM_SOF_TIMESTAMPING_TYPES * sizeof(u_int));
for (i = 0; i < NUM_SOF_TIMESTAMPING_TYPES; i++)
handle->tstamp_type_list[i] = sof_ts_type_map[i].pcap_tstamp_val;
}

#if defined(ETHTOOL_GET_TS_INFO)



static int
iface_ethtool_get_ts_info(const char *device, pcap_t *handle, char *ebuf)
{
int fd;
struct ifreq ifr;
struct ethtool_ts_info info;
int num_ts_types;
u_int i, j;








if (strcmp(device, "any") == 0) {
handle->tstamp_type_list = NULL;
return 0;
}




fd = socket(PF_UNIX, SOCK_RAW, 0);
if (fd < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "socket for SIOCETHTOOL(ETHTOOL_GET_TS_INFO)");
return -1;
}

memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
memset(&info, 0, sizeof(info));
info.cmd = ETHTOOL_GET_TS_INFO;
ifr.ifr_data = (caddr_t)&info;
if (ioctl(fd, SIOCETHTOOL, &ifr) == -1) {
int save_errno = errno;

close(fd);
switch (save_errno) {

case EOPNOTSUPP:
case EINVAL:





iface_set_all_ts_types(handle);
return 0;

case ENODEV:






handle->tstamp_type_list = NULL;
return 0;

default:



pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
save_errno,
"%s: SIOCETHTOOL(ETHTOOL_GET_TS_INFO) ioctl failed",
device);
return -1;
}
}
close(fd);




if (!(info.rx_filters & (1 << HWTSTAMP_FILTER_ALL))) {









handle->tstamp_type_list = NULL;
return 0;
}

num_ts_types = 0;
for (i = 0; i < NUM_SOF_TIMESTAMPING_TYPES; i++) {
if (info.so_timestamping & sof_ts_type_map[i].soft_timestamping_val)
num_ts_types++;
}
handle->tstamp_type_count = num_ts_types;
if (num_ts_types != 0) {
handle->tstamp_type_list = malloc(num_ts_types * sizeof(u_int));
for (i = 0, j = 0; i < NUM_SOF_TIMESTAMPING_TYPES; i++) {
if (info.so_timestamping & sof_ts_type_map[i].soft_timestamping_val) {
handle->tstamp_type_list[j] = sof_ts_type_map[i].pcap_tstamp_val;
j++;
}
}
} else
handle->tstamp_type_list = NULL;

return 0;
}
#else
static int
iface_ethtool_get_ts_info(const char *device, pcap_t *handle, char *ebuf _U_)
{







if (strcmp(device, "any") == 0) {
handle->tstamp_type_list = NULL;
return 0;
}





iface_set_all_ts_types(handle);
return 0;
}
#endif

#endif

#if defined(HAVE_PACKET_RING)




















#if defined(SIOCETHTOOL) && (defined(ETHTOOL_GTSO) || defined(ETHTOOL_GUFO) || defined(ETHTOOL_GGSO) || defined(ETHTOOL_GFLAGS) || defined(ETHTOOL_GGRO))
static int
iface_ethtool_flag_ioctl(pcap_t *handle, int cmd, const char *cmdname,
int eperm_ok)
{
struct ifreq ifr;
struct ethtool_value eval;

memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, handle->opt.device, sizeof(ifr.ifr_name));
eval.cmd = cmd;
eval.data = 0;
ifr.ifr_data = (caddr_t)&eval;
if (ioctl(handle->fd, SIOCETHTOOL, &ifr) == -1) {
if (errno == EOPNOTSUPP || errno == EINVAL ||
(errno == EPERM && eperm_ok)) {






return 0;
}
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "%s: SIOCETHTOOL(%s) ioctl failed",
handle->opt.device, cmdname);
return -1;
}
return eval.data;
}
















static int
iface_get_offload(pcap_t *handle)
{
int ret;

#if defined(ETHTOOL_GTSO)
ret = iface_ethtool_flag_ioctl(handle, ETHTOOL_GTSO, "ETHTOOL_GTSO", 0);
if (ret == -1)
return -1;
if (ret)
return 1;
#endif

#if defined(ETHTOOL_GGSO)





ret = iface_ethtool_flag_ioctl(handle, ETHTOOL_GGSO, "ETHTOOL_GGSO", 0);
if (ret == -1)
return -1;
if (ret)
return 1;
#endif

#if defined(ETHTOOL_GFLAGS)
ret = iface_ethtool_flag_ioctl(handle, ETHTOOL_GFLAGS, "ETHTOOL_GFLAGS", 0);
if (ret == -1)
return -1;
if (ret & ETH_FLAG_LRO)
return 1;
#endif

#if defined(ETHTOOL_GGRO)





ret = iface_ethtool_flag_ioctl(handle, ETHTOOL_GGRO, "ETHTOOL_GGRO", 0);
if (ret == -1)
return -1;
if (ret)
return 1;
#endif

#if defined(ETHTOOL_GUFO)






ret = iface_ethtool_flag_ioctl(handle, ETHTOOL_GUFO, "ETHTOOL_GUFO", 1);
if (ret == -1)
return -1;
if (ret)
return 1;
#endif

return 0;
}
#else
static int
iface_get_offload(pcap_t *handle _U_)
{




return 0;
}
#endif

#endif

#endif







static int
activate_old(pcap_t *handle)
{
struct pcap_linux *handlep = handle->priv;
int err;
int arptype;
struct ifreq ifr;
const char *device = handle->opt.device;
struct utsname utsname;
int mtu;





if (strcmp(device, "any") == 0) {
pcap_strlcpy(handle->errbuf, "pcap_activate: The \"any\" device isn't supported on 2.0[.x]-kernel systems",
PCAP_ERRBUF_SIZE);
return PCAP_ERROR;
}


handle->fd = socket(PF_INET, SOCK_PACKET, htons(ETH_P_ALL));
if (handle->fd == -1) {
err = errno;
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
err, "socket");
if (err == EPERM || err == EACCES) {




return PCAP_ERROR_PERM_DENIED;
} else {



return PCAP_ERROR;
}
}


handlep->sock_packet = 1;


handlep->cooked = 0;


if (iface_bind_old(handle->fd, device, handle->errbuf) == -1)
return PCAP_ERROR;




arptype = iface_get_arptype(handle->fd, device, handle->errbuf);
if (arptype < 0)
return PCAP_ERROR;





map_arphrd_to_dlt(handle, handle->fd, arptype, device, 0);
if (handle->linktype == -1) {
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"unknown arptype %d", arptype);
return PCAP_ERROR;
}



if (handle->opt.promisc) {
memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
if (ioctl(handle->fd, SIOCGIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "SIOCGIFFLAGS");
return PCAP_ERROR;
}
if ((ifr.ifr_flags & IFF_PROMISC) == 0) {












if (!pcap_do_addexit(handle)) {





return PCAP_ERROR;
}

ifr.ifr_flags |= IFF_PROMISC;
if (ioctl(handle->fd, SIOCSIFFLAGS, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "SIOCSIFFLAGS");
return PCAP_ERROR;
}
handlep->must_do_on_close |= MUST_CLEAR_PROMISC;





pcap_add_to_pcaps_to_close(handle);
}
}







if (uname(&utsname) < 0 ||
strncmp(utsname.release, "2.0", 3) == 0) {










































mtu = iface_get_mtu(handle->fd, device, handle->errbuf);
if (mtu == -1)
return PCAP_ERROR;
handle->bufsize = MAX_LINKHEADER_SIZE + mtu;
if (handle->bufsize < (u_int)handle->snapshot)
handle->bufsize = (u_int)handle->snapshot;
} else {

















handle->bufsize = (u_int)handle->snapshot;
}





handle->offset = 0;





handlep->vlan_offset = -1;

return 1;
}





static int
iface_bind_old(int fd, const char *device, char *ebuf)
{
struct sockaddr saddr;
int err;
socklen_t errlen = sizeof(err);

memset(&saddr, 0, sizeof(saddr));
pcap_strlcpy(saddr.sa_data, device, sizeof(saddr.sa_data));
if (bind(fd, &saddr, sizeof(saddr)) == -1) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "bind");
return -1;
}



if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "getsockopt (SO_ERROR)");
return -1;
}

if (err > 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
err, "bind");
return -1;
}

return 0;
}







static int
iface_get_mtu(int fd, const char *device, char *ebuf)
{
struct ifreq ifr;

if (!device)
return BIGGER_THAN_ALL_MTUS;

memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));

if (ioctl(fd, SIOCGIFMTU, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFMTU");
return -1;
}

return ifr.ifr_mtu;
}




static int
iface_get_arptype(int fd, const char *device, char *ebuf)
{
struct ifreq ifr;
int ret;

memset(&ifr, 0, sizeof(ifr));
pcap_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));

if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
if (errno == ENODEV) {



ret = PCAP_ERROR_NO_SUCH_DEVICE;
} else
ret = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFHWADDR");
return ret;
}

return ifr.ifr_hwaddr.sa_family;
}

#if defined(SO_ATTACH_FILTER)
static int
fix_program(pcap_t *handle, struct sock_fprog *fcode, int is_mmapped)
{
struct pcap_linux *handlep = handle->priv;
size_t prog_size;
register int i;
register struct bpf_insn *p;
struct bpf_insn *f;
int len;





prog_size = sizeof(*handle->fcode.bf_insns) * handle->fcode.bf_len;
len = handle->fcode.bf_len;
f = (struct bpf_insn *)malloc(prog_size);
if (f == NULL) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return -1;
}
memcpy(f, handle->fcode.bf_insns, prog_size);
fcode->len = len;
fcode->filter = (struct sock_filter *) f;

for (i = 0; i < len; ++i) {
p = &f[i];



switch (BPF_CLASS(p->code)) {

case BPF_RET:




if (!is_mmapped) {





if (BPF_MODE(p->code) == BPF_K) {















if (p->k != 0)
p->k = MAXIMUM_SNAPLEN;
}
}
break;

case BPF_LD:
case BPF_LDX:




switch (BPF_MODE(p->code)) {

case BPF_ABS:
case BPF_IND:
case BPF_MSH:



if (handlep->cooked) {




if (fix_offset(handle, p) < 0) {





return 0;
}
}
break;
}
break;
}
}
return 1;
}

static int
fix_offset(pcap_t *handle, struct bpf_insn *p)
{






if (p->k >= (bpf_u_int32)SKF_AD_OFF)
return 0;
if (handle->linktype == DLT_LINUX_SLL2) {



if (p->k >= SLL2_HDR_LEN) {






p->k -= SLL2_HDR_LEN;
} else if (p->k == 0) {




p->k = SKF_AD_OFF + SKF_AD_PROTOCOL;
} else if (p->k == 10) {




p->k = SKF_AD_OFF + SKF_AD_PKTTYPE;
} else if ((bpf_int32)(p->k) > 0) {





return -1;
}
} else {



if (p->k >= SLL_HDR_LEN) {






p->k -= SLL_HDR_LEN;
} else if (p->k == 0) {




p->k = SKF_AD_OFF + SKF_AD_PKTTYPE;
} else if (p->k == 14) {




p->k = SKF_AD_OFF + SKF_AD_PROTOCOL;
} else if ((bpf_int32)(p->k) > 0) {





return -1;
}
}
return 0;
}

static int
set_kernel_filter(pcap_t *handle, struct sock_fprog *fcode)
{
int total_filter_on = 0;
int save_mode;
int ret;
int save_errno;
































if (setsockopt(handle->fd, SOL_SOCKET, SO_ATTACH_FILTER,
&total_fcode, sizeof(total_fcode)) == 0) {
char drain[1];




total_filter_on = 1;







save_mode = fcntl(handle->fd, F_GETFL, 0);
if (save_mode == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"can't get FD flags when changing filter");
return -2;
}
if (fcntl(handle->fd, F_SETFL, save_mode | O_NONBLOCK) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"can't set nonblocking mode when changing filter");
return -2;
}
while (recv(handle->fd, &drain, sizeof drain, MSG_TRUNC) >= 0)
;
save_errno = errno;
if (save_errno != EAGAIN) {






(void)fcntl(handle->fd, F_SETFL, save_mode);
(void)reset_kernel_filter(handle);
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, save_errno,
"recv failed when changing filter");
return -2;
}
if (fcntl(handle->fd, F_SETFL, save_mode) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"can't restore FD flags when changing filter");
return -2;
}
}




ret = setsockopt(handle->fd, SOL_SOCKET, SO_ATTACH_FILTER,
fcode, sizeof(*fcode));
if (ret == -1 && total_filter_on) {










save_errno = errno;






if (reset_kernel_filter(handle) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"can't remove kernel total filter");
return -2;
}

errno = save_errno;
}
return ret;
}

static int
reset_kernel_filter(pcap_t *handle)
{
int ret;






int dummy = 0;

ret = setsockopt(handle->fd, SOL_SOCKET, SO_DETACH_FILTER,
&dummy, sizeof(dummy));







if (ret == -1 && errno != ENOENT && errno != ENONET)
return -1;
return 0;
}
#endif

int
pcap_set_protocol_linux(pcap_t *p, int protocol)
{
if (pcap_check_activated(p))
return (PCAP_ERROR_ACTIVATED);
p->opt.protocol = protocol;
return (0);
}




const char *
pcap_lib_version(void)
{
#if defined(HAVE_PACKET_RING)
#if defined(HAVE_TPACKET3)
return (PCAP_VERSION_STRING " (with TPACKET_V3)");
#elif defined(HAVE_TPACKET2)
return (PCAP_VERSION_STRING " (with TPACKET_V2)");
#else
return (PCAP_VERSION_STRING " (with TPACKET_V1)");
#endif
#else
return (PCAP_VERSION_STRING " (without TPACKET)");
#endif
}
