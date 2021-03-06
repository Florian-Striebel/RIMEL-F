


































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "pcap-int.h"
#include "pcap-usb-linux.h"
#include "pcap/usb.h"

#if defined(NEED_STRERROR_H)
#include "strerror.h"
#endif

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <byteswap.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#if defined(HAVE_LINUX_USBDEVICE_FS_H)




#if defined(HAVE_LINUX_COMPILER_H)
#include <linux/compiler.h>
#endif
#include <linux/usbdevice_fs.h>
#endif

#define USB_IFACE "usbmon"
#define USB_TEXT_DIR_OLD "/sys/kernel/debug/usbmon"
#define USB_TEXT_DIR "/sys/kernel/debug/usb/usbmon"
#define SYS_USB_BUS_DIR "/sys/bus/usb/devices"
#define PROC_USB_BUS_DIR "/proc/bus/usb"
#define USB_LINE_LEN 4096

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htols(s) s
#define htoll(l) l
#define htol64(ll) ll
#else
#define htols(s) bswap_16(s)
#define htoll(l) bswap_32(l)
#define htol64(ll) bswap_64(ll)
#endif

struct mon_bin_stats {
uint32_t queued;
uint32_t dropped;
};

struct mon_bin_get {
pcap_usb_header *hdr;
void *data;
size_t data_len;
};

struct mon_bin_mfetch {
int32_t *offvec;
int32_t nfetch;
int32_t nflush;
};

#define MON_IOC_MAGIC 0x92

#define MON_IOCQ_URB_LEN _IO(MON_IOC_MAGIC, 1)
#define MON_IOCX_URB _IOWR(MON_IOC_MAGIC, 2, struct mon_bin_hdr)
#define MON_IOCG_STATS _IOR(MON_IOC_MAGIC, 3, struct mon_bin_stats)
#define MON_IOCT_RING_SIZE _IO(MON_IOC_MAGIC, 4)
#define MON_IOCQ_RING_SIZE _IO(MON_IOC_MAGIC, 5)
#define MON_IOCX_GET _IOW(MON_IOC_MAGIC, 6, struct mon_bin_get)
#define MON_IOCX_MFETCH _IOWR(MON_IOC_MAGIC, 7, struct mon_bin_mfetch)
#define MON_IOCH_MFLUSH _IO(MON_IOC_MAGIC, 8)

#define MON_BIN_SETUP 0x1
#define MON_BIN_SETUP_ZERO 0x2
#define MON_BIN_DATA_ZERO 0x4
#define MON_BIN_ERROR 0x8




struct pcap_usb_linux {
u_char *mmapbuf;
size_t mmapbuflen;
int bus_index;
u_int packets_read;
};


static int usb_activate(pcap_t *);
static int usb_stats_linux(pcap_t *, struct pcap_stat *);
static int usb_stats_linux_bin(pcap_t *, struct pcap_stat *);
static int usb_read_linux(pcap_t *, int , pcap_handler , u_char *);
static int usb_read_linux_bin(pcap_t *, int , pcap_handler , u_char *);
static int usb_read_linux_mmap(pcap_t *, int , pcap_handler , u_char *);
static int usb_inject_linux(pcap_t *, const void *, size_t);
static int usb_setdirection_linux(pcap_t *, pcap_direction_t);
static void usb_cleanup_linux_mmap(pcap_t *);

static int
have_binary_usbmon(void)
{
struct utsname utsname;
char *version_component, *endp;
int major, minor, subminor;

if (uname(&utsname) == 0) {




version_component = utsname.release;
major = strtol(version_component, &endp, 10);
if (endp != version_component && *endp == '.') {





if (major >= 3)
return 1;






if (major <= 1)
return 0;
}





version_component = endp + 1;
minor = strtol(version_component, &endp, 10);
if (endp != version_component &&
(*endp == '.' || *endp == '\0')) {






if (minor < 6) {




return 0;
}





version_component = endp + 1;
subminor = strtol(version_component, &endp, 10);
if (endp != version_component &&
(*endp == '.' || *endp == '\0')) {




if (subminor >= 21) {




return 1;
}
}
}
}





return 0;
}


static int
usb_dev_add(pcap_if_list_t *devlistp, int n, char *err_str)
{
char dev_name[10];
char dev_descr[30];
pcap_snprintf(dev_name, 10, USB_IFACE"%d", n);



if (n == 0) {





if (add_dev(devlistp, dev_name,
PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE,
"Raw USB traffic, all USB buses", err_str) == NULL)
return -1;
} else {






pcap_snprintf(dev_descr, 30, "Raw USB traffic, bus number %d", n);
if (add_dev(devlistp, dev_name, 0, dev_descr, err_str) == NULL)
return -1;
}

return 0;
}

int
usb_findalldevs(pcap_if_list_t *devlistp, char *err_str)
{
char usb_mon_dir[PATH_MAX];
char *usb_mon_prefix;
size_t usb_mon_prefix_len;
struct dirent* data;
int ret = 0;
DIR* dir;
int n;
char* name;
size_t len;

if (have_binary_usbmon()) {






pcap_strlcpy(usb_mon_dir, LINUX_USB_MON_DEV, sizeof usb_mon_dir);
usb_mon_prefix = strrchr(usb_mon_dir, '/');
if (usb_mon_prefix == NULL) {




return 0;
}
*usb_mon_prefix++ = '\0';
usb_mon_prefix_len = strlen(usb_mon_prefix);




dir = opendir(usb_mon_dir);
if (dir != NULL) {
while ((ret == 0) && ((data = readdir(dir)) != 0)) {
name = data->d_name;




if (strncmp(name, usb_mon_prefix, usb_mon_prefix_len) != 0)
continue;




if (sscanf(&name[usb_mon_prefix_len], "%d", &n) == 0)
continue;

ret = usb_dev_add(devlistp, n, err_str);
}

closedir(dir);
}
return 0;
} else {



















dir = opendir(SYS_USB_BUS_DIR);
if (dir != NULL) {
while ((ret == 0) && ((data = readdir(dir)) != 0)) {
name = data->d_name;

if (strncmp(name, "usb", 3) != 0)
continue;

if (sscanf(&name[3], "%d", &n) == 0)
continue;

ret = usb_dev_add(devlistp, n, err_str);
}

closedir(dir);
return 0;
}


dir = opendir(PROC_USB_BUS_DIR);
if (dir != NULL) {
while ((ret == 0) && ((data = readdir(dir)) != 0)) {
name = data->d_name;
len = strlen(name);


if ((len < 1) || !isdigit(name[--len]))
continue;
while (isdigit(name[--len]));
if (sscanf(&name[len+1], "%d", &n) != 1)
continue;

ret = usb_dev_add(devlistp, n, err_str);
}

closedir(dir);
return ret;
}


return 0;
}
}




#define MIN_RING_SIZE (8*1024)
#define MAX_RING_SIZE (1200*1024)

static int
usb_set_ring_size(pcap_t* handle, int header_size)
{




















int ring_size;

if (handle->snapshot < header_size)
handle->snapshot = header_size;

ring_size = (handle->snapshot - header_size) * 5;







if (ring_size > MAX_RING_SIZE) {





ring_size = MAX_RING_SIZE;
handle->snapshot = header_size + (MAX_RING_SIZE/5);
} else if (ring_size < MIN_RING_SIZE) {






ring_size = MIN_RING_SIZE;
}

if (ioctl(handle->fd, MON_IOCT_RING_SIZE, ring_size) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't set ring size from fd %d", handle->fd);
return -1;
}
return ring_size;
}

static
int usb_mmap(pcap_t* handle)
{
struct pcap_usb_linux *handlep = handle->priv;
int len;






len = usb_set_ring_size(handle, (int)sizeof(pcap_usb_header_mmapped));
if (len == -1) {

return 0;
}

handlep->mmapbuflen = len;
handlep->mmapbuf = mmap(0, handlep->mmapbuflen, PROT_READ,
MAP_SHARED, handle->fd, 0);
if (handlep->mmapbuf == MAP_FAILED) {




return 0;
}
return 1;
}

#if defined(HAVE_LINUX_USBDEVICE_FS_H)

#define CTRL_TIMEOUT (5*1000)

#define USB_DIR_IN 0x80
#define USB_TYPE_STANDARD 0x00
#define USB_RECIP_DEVICE 0x00

#define USB_REQ_GET_DESCRIPTOR 6

#define USB_DT_DEVICE 1





static void
probe_devices(int bus)
{
struct usbdevfs_ctrltransfer ctrl;
struct dirent* data;
int ret = 0;
char buf[sizeof("/dev/bus/usb/000/") + NAME_MAX];
DIR* dir;


pcap_snprintf(buf, sizeof(buf), "/dev/bus/usb/%03d", bus);
dir = opendir(buf);
if (!dir)
return;

while ((ret >= 0) && ((data = readdir(dir)) != 0)) {
int fd;
char* name = data->d_name;

if (name[0] == '.')
continue;

pcap_snprintf(buf, sizeof(buf), "/dev/bus/usb/%03d/%s", bus, data->d_name);

fd = open(buf, O_RDWR);
if (fd == -1)
continue;





#if defined(HAVE_STRUCT_USBDEVFS_CTRLTRANSFER_BREQUESTTYPE)
ctrl.bRequestType = USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE;
ctrl.bRequest = USB_REQ_GET_DESCRIPTOR;
ctrl.wValue = USB_DT_DEVICE << 8;
ctrl.wIndex = 0;
ctrl.wLength = sizeof(buf);
#else
ctrl.requesttype = USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE;
ctrl.request = USB_REQ_GET_DESCRIPTOR;
ctrl.value = USB_DT_DEVICE << 8;
ctrl.index = 0;
ctrl.length = sizeof(buf);
#endif
ctrl.data = buf;
ctrl.timeout = CTRL_TIMEOUT;

ret = ioctl(fd, USBDEVFS_CONTROL, &ctrl);

close(fd);
}
closedir(dir);
}
#endif

pcap_t *
usb_create(const char *device, char *ebuf, int *is_ours)
{
const char *cp;
char *cpend;
long devnum;
pcap_t *p;


cp = strrchr(device, '/');
if (cp == NULL)
cp = device;

if (strncmp(cp, USB_IFACE, sizeof USB_IFACE - 1) != 0) {

*is_ours = 0;
return NULL;
}

cp += sizeof USB_IFACE - 1;
devnum = strtol(cp, &cpend, 10);
if (cpend == cp || *cpend != '\0') {

*is_ours = 0;
return NULL;
}
if (devnum < 0) {

*is_ours = 0;
return NULL;
}


*is_ours = 1;

p = pcap_create_common(ebuf, sizeof (struct pcap_usb_linux));
if (p == NULL)
return (NULL);

p->activate_op = usb_activate;
return (p);
}

static int
usb_activate(pcap_t* handle)
{
struct pcap_usb_linux *handlep = handle->priv;
char full_path[USB_LINE_LEN];
int ret;









if (handle->snapshot <= 0 || handle->snapshot > MAXIMUM_SNAPLEN)
handle->snapshot = MAXIMUM_SNAPLEN;


handle->bufsize = handle->snapshot;
handle->offset = 0;
handle->linktype = DLT_USB_LINUX;

handle->inject_op = usb_inject_linux;
handle->setfilter_op = install_bpf_program;
handle->setdirection_op = usb_setdirection_linux;
handle->set_datalink_op = NULL;
handle->getnonblock_op = pcap_getnonblock_fd;
handle->setnonblock_op = pcap_setnonblock_fd;


if (sscanf(handle->opt.device, USB_IFACE"%d", &handlep->bus_index) != 1)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Can't get USB bus index from %s", handle->opt.device);
return PCAP_ERROR;
}

if (have_binary_usbmon())
{




pcap_snprintf(full_path, USB_LINE_LEN, LINUX_USB_MON_DEV"%d", handlep->bus_index);
handle->fd = open(full_path, O_RDONLY, 0);
if (handle->fd < 0)
{



switch (errno) {

case ENOENT:











return PCAP_ERROR_NO_SUCH_DEVICE;

case EACCES:



return PCAP_ERROR_PERM_DENIED;

default:



pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"Can't open USB bus file %s", full_path);
return PCAP_ERROR;
}
}

if (handle->opt.rfmon)
{



close(handle->fd);
return PCAP_ERROR_RFMON_NOTSUP;
}


if (usb_mmap(handle))
{

handle->linktype = DLT_USB_LINUX_MMAPPED;
handle->stats_op = usb_stats_linux_bin;
handle->read_op = usb_read_linux_mmap;
handle->cleanup_op = usb_cleanup_linux_mmap;
#if defined(HAVE_LINUX_USBDEVICE_FS_H)
probe_devices(handlep->bus_index);
#endif





handle->selectable_fd = handle->fd;
return 0;
}









if (usb_set_ring_size(handle, (int)sizeof(pcap_usb_header)) == -1) {

close(handle->fd);
return PCAP_ERROR;
}
handle->stats_op = usb_stats_linux_bin;
handle->read_op = usb_read_linux_bin;
#if defined(HAVE_LINUX_USBDEVICE_FS_H)
probe_devices(handlep->bus_index);
#endif
}
else {




pcap_snprintf(full_path, USB_LINE_LEN, USB_TEXT_DIR"/%dt", handlep->bus_index);
handle->fd = open(full_path, O_RDONLY, 0);
if (handle->fd < 0)
{
if (errno == ENOENT)
{




pcap_snprintf(full_path, USB_LINE_LEN, USB_TEXT_DIR_OLD"/%dt", handlep->bus_index);
handle->fd = open(full_path, O_RDONLY, 0);
}
if (handle->fd < 0) {
if (errno == ENOENT)
{







ret = PCAP_ERROR_NO_SUCH_DEVICE;
}
else if (errno == EACCES)
{





ret = PCAP_ERROR_PERM_DENIED;
}
else
{



ret = PCAP_ERROR;
}
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"Can't open USB bus file %s",
full_path);
return ret;
}
}

if (handle->opt.rfmon)
{



close(handle->fd);
return PCAP_ERROR_RFMON_NOTSUP;
}

handle->stats_op = usb_stats_linux;
handle->read_op = usb_read_linux;
}





handle->selectable_fd = handle->fd;



handle->buffer = malloc(handle->bufsize);
if (!handle->buffer) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
close(handle->fd);
return PCAP_ERROR;
}
return 0;
}

static inline int
ascii_to_int(char c)
{
return c < 'A' ? c- '0': ((c<'a') ? c - 'A' + 10: c-'a'+10);
}






static int
usb_read_linux(pcap_t *handle, int max_packets _U_, pcap_handler callback, u_char *user)
{




struct pcap_usb_linux *handlep = handle->priv;
unsigned timestamp;
int tag, cnt, ep_num, dev_addr, dummy, ret, urb_len, data_len;
char etype, pipeid1, pipeid2, status[16], urb_tag, line[USB_LINE_LEN];
char *string = line;
u_char * rawdata = handle->buffer;
struct pcap_pkthdr pkth;
pcap_usb_header* uhdr = (pcap_usb_header*)handle->buffer;
u_char urb_transfer=0;
int incoming=0;


do {
ret = read(handle->fd, line, USB_LINE_LEN - 1);
if (handle->break_loop)
{
handle->break_loop = 0;
return -2;
}
} while ((ret == -1) && (errno == EINTR));
if (ret < 0)
{
if (errno == EAGAIN)
return 0;

pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't read from fd %d", handle->fd);
return -1;
}



string[ret] = 0;
ret = sscanf(string, "%x %d %c %c%c:%d:%d %s%n", &tag, &timestamp, &etype,
&pipeid1, &pipeid2, &dev_addr, &ep_num, status,
&cnt);
if (ret < 8)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Can't parse USB bus message '%s', too few tokens (expected 8 got %d)",
string, ret);
return -1;
}
uhdr->id = tag;
uhdr->device_address = dev_addr;
uhdr->bus_id = handlep->bus_index;
uhdr->status = 0;
string += cnt;


if (gettimeofday(&pkth.ts, NULL) < 0)
{
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't get timestamp for message '%s'", string);
return -1;
}
uhdr->ts_sec = pkth.ts.tv_sec;
uhdr->ts_usec = pkth.ts.tv_usec;


if (pipeid1 == 'C')
urb_transfer = URB_CONTROL;
else if (pipeid1 == 'Z')
urb_transfer = URB_ISOCHRONOUS;
else if (pipeid1 == 'I')
urb_transfer = URB_INTERRUPT;
else if (pipeid1 == 'B')
urb_transfer = URB_BULK;
if (pipeid2 == 'i') {
ep_num |= URB_TRANSFER_IN;
incoming = 1;
}
if (etype == 'C')
incoming = !incoming;


if (incoming)
{
if (handle->direction == PCAP_D_OUT)
return 0;
}
else
if (handle->direction == PCAP_D_IN)
return 0;
uhdr->event_type = etype;
uhdr->transfer_type = urb_transfer;
uhdr->endpoint_number = ep_num;
pkth.caplen = sizeof(pcap_usb_header);
rawdata += sizeof(pcap_usb_header);


ret = sscanf(status, "%d", &dummy);
if (ret != 1)
{



pcap_usb_setup* shdr;
char str1[3], str2[3], str3[5], str4[5], str5[5];
ret = sscanf(string, "%s %s %s %s %s%n", str1, str2, str3, str4,
str5, &cnt);
if (ret < 5)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Can't parse USB bus message '%s', too few tokens (expected 5 got %d)",
string, ret);
return -1;
}
string += cnt;


shdr = &uhdr->setup;
shdr->bmRequestType = strtoul(str1, 0, 16);
shdr->bRequest = strtoul(str2, 0, 16);
shdr->wValue = htols(strtoul(str3, 0, 16));
shdr->wIndex = htols(strtoul(str4, 0, 16));
shdr->wLength = htols(strtoul(str5, 0, 16));

uhdr->setup_flag = 0;
}
else
uhdr->setup_flag = 1;


ret = sscanf(string, " %d%n", &urb_len, &cnt);
if (ret < 1)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Can't parse urb length from '%s'", string);
return -1;
}
string += cnt;



pkth.len = urb_len+pkth.caplen;
uhdr->urb_len = urb_len;
uhdr->data_flag = 1;
data_len = 0;
if (uhdr->urb_len == 0)
goto got;


if (sscanf(string, " %c", &urb_tag) != 1)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Can't parse urb tag from '%s'", string);
return -1;
}

if (urb_tag != '=')
goto got;


string += 3;


uhdr->data_flag = 0;






while ((string[0] != 0) && (string[1] != 0) && (pkth.caplen < (bpf_u_int32)handle->snapshot))
{
rawdata[0] = ascii_to_int(string[0]) * 16 + ascii_to_int(string[1]);
rawdata++;
string+=2;
if (string[0] == ' ')
string++;
pkth.caplen++;
data_len++;
}

got:
uhdr->data_len = data_len;
if (pkth.caplen > (bpf_u_int32)handle->snapshot)
pkth.caplen = (bpf_u_int32)handle->snapshot;

if (handle->fcode.bf_insns == NULL ||
bpf_filter(handle->fcode.bf_insns, handle->buffer,
pkth.len, pkth.caplen)) {
handlep->packets_read++;
callback(user, &pkth, handle->buffer);
return 1;
}
return 0;
}

static int
usb_inject_linux(pcap_t *handle, const void *buf _U_, size_t size _U_)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Packet injection is not supported on USB devices");
return (-1);
}

static int
usb_stats_linux(pcap_t *handle, struct pcap_stat *stats)
{
struct pcap_usb_linux *handlep = handle->priv;
int dummy, ret, consumed, cnt;
char string[USB_LINE_LEN];
char token[USB_LINE_LEN];
char * ptr = string;
int fd;

pcap_snprintf(string, USB_LINE_LEN, USB_TEXT_DIR"/%ds", handlep->bus_index);
fd = open(string, O_RDONLY, 0);
if (fd < 0)
{
if (errno == ENOENT)
{




pcap_snprintf(string, USB_LINE_LEN, USB_TEXT_DIR_OLD"/%ds", handlep->bus_index);
fd = open(string, O_RDONLY, 0);
}
if (fd < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno,
"Can't open USB stats file %s", string);
return -1;
}
}


do {
ret = read(fd, string, USB_LINE_LEN-1);
} while ((ret == -1) && (errno == EINTR));
close(fd);

if (ret < 0)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Can't read stats from fd %d ", fd);
return -1;
}
string[ret] = 0;

stats->ps_recv = handlep->packets_read;
stats->ps_drop = 0;
stats->ps_ifdrop = 0;


for (consumed=0; consumed < ret; ) {






int ntok;

cnt = -1;
ntok = sscanf(ptr, "%s%n", token, &cnt);
if ((ntok < 1) || (cnt < 0))
break;
consumed += cnt;
ptr += cnt;
if (strcmp(token, "text_lost") == 0)
ntok = sscanf(ptr, "%d%n", &stats->ps_drop, &cnt);
else
ntok = sscanf(ptr, "%d%n", &dummy, &cnt);
if ((ntok != 1) || (cnt < 0))
break;
consumed += cnt;
ptr += cnt;
}

return 0;
}

static int
usb_setdirection_linux(pcap_t *p, pcap_direction_t d)
{
p->direction = d;
return 0;
}


static int
usb_stats_linux_bin(pcap_t *handle, struct pcap_stat *stats)
{
struct pcap_usb_linux *handlep = handle->priv;
int ret;
struct mon_bin_stats st;
ret = ioctl(handle->fd, MON_IOCG_STATS, &st);
if (ret < 0)
{
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't read stats from fd %d", handle->fd);
return -1;
}

stats->ps_recv = handlep->packets_read + st.queued;
stats->ps_drop = st.dropped;
stats->ps_ifdrop = 0;
return 0;
}





static int
usb_read_linux_bin(pcap_t *handle, int max_packets _U_, pcap_handler callback, u_char *user)
{
struct pcap_usb_linux *handlep = handle->priv;
struct mon_bin_get info;
int ret;
struct pcap_pkthdr pkth;
u_int clen = handle->snapshot - sizeof(pcap_usb_header);


info.hdr = (pcap_usb_header*) handle->buffer;
info.data = (u_char *)handle->buffer + sizeof(pcap_usb_header);
info.data_len = clen;


do {
ret = ioctl(handle->fd, MON_IOCX_GET, &info);
if (handle->break_loop)
{
handle->break_loop = 0;
return -2;
}
} while ((ret == -1) && (errno == EINTR));
if (ret < 0)
{
if (errno == EAGAIN)
return 0;

pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't read from fd %d", handle->fd);
return -1;
}



















if (info.hdr->data_len < clen)
clen = info.hdr->data_len;
info.hdr->data_len = clen;
pkth.caplen = sizeof(pcap_usb_header) + clen;
if (info.hdr->data_flag) {





pkth.len = sizeof(pcap_usb_header) + info.hdr->data_len;
} else {






pkth.len = sizeof(pcap_usb_header) + info.hdr->urb_len;
}
pkth.ts.tv_sec = info.hdr->ts_sec;
pkth.ts.tv_usec = info.hdr->ts_usec;

if (handle->fcode.bf_insns == NULL ||
bpf_filter(handle->fcode.bf_insns, handle->buffer,
pkth.len, pkth.caplen)) {
handlep->packets_read++;
callback(user, &pkth, handle->buffer);
return 1;
}

return 0;
}





#define VEC_SIZE 32
static int
usb_read_linux_mmap(pcap_t *handle, int max_packets, pcap_handler callback, u_char *user)
{
struct pcap_usb_linux *handlep = handle->priv;
struct mon_bin_mfetch fetch;
int32_t vec[VEC_SIZE];
struct pcap_pkthdr pkth;
pcap_usb_header_mmapped* hdr;
int nflush = 0;
int packets = 0;
u_int clen, max_clen;

max_clen = handle->snapshot - sizeof(pcap_usb_header_mmapped);

for (;;) {
int i, ret;
int limit = max_packets - packets;
if (limit <= 0)
limit = VEC_SIZE;
if (limit > VEC_SIZE)
limit = VEC_SIZE;


fetch.offvec = vec;
fetch.nfetch = limit;
fetch.nflush = nflush;

do {
ret = ioctl(handle->fd, MON_IOCX_MFETCH, &fetch);
if (handle->break_loop)
{
handle->break_loop = 0;
return -2;
}
} while ((ret == -1) && (errno == EINTR));
if (ret < 0)
{
if (errno == EAGAIN)
return 0;

pcap_fmt_errmsg_for_errno(handle->errbuf,
PCAP_ERRBUF_SIZE, errno, "Can't mfetch fd %d",
handle->fd);
return -1;
}


nflush = fetch.nfetch;
for (i=0; i<fetch.nfetch; ++i) {

hdr = (pcap_usb_header_mmapped*) &handlep->mmapbuf[vec[i]];
if (hdr->event_type == '@')
continue;





















clen = max_clen;
if (hdr->data_len < clen)
clen = hdr->data_len;
pkth.caplen = sizeof(pcap_usb_header_mmapped) + clen;
if (hdr->data_flag) {





pkth.len = sizeof(pcap_usb_header_mmapped) +
hdr->data_len;
} else {






pkth.len = sizeof(pcap_usb_header_mmapped) +
(hdr->ndesc * sizeof (usb_isodesc)) + hdr->urb_len;
}
pkth.ts.tv_sec = hdr->ts_sec;
pkth.ts.tv_usec = hdr->ts_usec;

if (handle->fcode.bf_insns == NULL ||
bpf_filter(handle->fcode.bf_insns, (u_char*) hdr,
pkth.len, pkth.caplen)) {
handlep->packets_read++;
callback(user, &pkth, (u_char*) hdr);
packets++;
}
}


if (PACKET_COUNT_IS_UNLIMITED(max_packets) || (packets == max_packets))
break;
}


if (ioctl(handle->fd, MON_IOCH_MFLUSH, nflush) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't mflush fd %d", handle->fd);
return -1;
}
return packets;
}

static void
usb_cleanup_linux_mmap(pcap_t* handle)
{
struct pcap_usb_linux *handlep = handle->priv;


if (handlep->mmapbuf != NULL) {
munmap(handlep->mmapbuf, handlep->mmapbuflen);
handlep->mmapbuf = NULL;
}
pcap_cleanup_live_common(handle);
}
