

































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "pcap-int.h"
#include "pcap-bt-linux.h"
#include "pcap/bluetooth.h"

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#define BT_IFACE "bluetooth"
#define BT_CTRL_SIZE 128


static int bt_activate(pcap_t *);
static int bt_read_linux(pcap_t *, int , pcap_handler , u_char *);
static int bt_inject_linux(pcap_t *, const void *, size_t);
static int bt_setdirection_linux(pcap_t *, pcap_direction_t);
static int bt_stats_linux(pcap_t *, struct pcap_stat *);




struct pcap_bt {
int dev_id;
};

int
bt_findalldevs(pcap_if_list_t *devlistp, char *err_str)
{
struct hci_dev_list_req *dev_list;
struct hci_dev_req *dev_req;
int sock;
unsigned i;
int ret = 0;

sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
if (sock < 0)
{

if (errno == EAFNOSUPPORT)
return 0;
pcap_fmt_errmsg_for_errno(err_str, PCAP_ERRBUF_SIZE,
errno, "Can't open raw Bluetooth socket");
return -1;
}

dev_list = malloc(HCI_MAX_DEV * sizeof(*dev_req) + sizeof(*dev_list));
if (!dev_list)
{
pcap_snprintf(err_str, PCAP_ERRBUF_SIZE, "Can't allocate %zu bytes for Bluetooth device list",
HCI_MAX_DEV * sizeof(*dev_req) + sizeof(*dev_list));
ret = -1;
goto done;
}

dev_list->dev_num = HCI_MAX_DEV;

if (ioctl(sock, HCIGETDEVLIST, (void *) dev_list) < 0)
{
pcap_fmt_errmsg_for_errno(err_str, PCAP_ERRBUF_SIZE,
errno, "Can't get Bluetooth device list via ioctl");
ret = -1;
goto free;
}

dev_req = dev_list->dev_req;
for (i = 0; i < dev_list->dev_num; i++, dev_req++) {
char dev_name[20], dev_descr[40];

pcap_snprintf(dev_name, sizeof(dev_name), BT_IFACE"%u", dev_req->dev_id);
pcap_snprintf(dev_descr, sizeof(dev_descr), "Bluetooth adapter number %u", i);









if (add_dev(devlistp, dev_name, PCAP_IF_WIRELESS, dev_descr, err_str) == NULL)
{
ret = -1;
break;
}
}

free:
free(dev_list);

done:
close(sock);
return ret;
}

pcap_t *
bt_create(const char *device, char *ebuf, int *is_ours)
{
const char *cp;
char *cpend;
long devnum;
pcap_t *p;


cp = strrchr(device, '/');
if (cp == NULL)
cp = device;

if (strncmp(cp, BT_IFACE, sizeof BT_IFACE - 1) != 0) {

*is_ours = 0;
return NULL;
}

cp += sizeof BT_IFACE - 1;
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

p = pcap_create_common(ebuf, sizeof (struct pcap_bt));
if (p == NULL)
return (NULL);

p->activate_op = bt_activate;
return (p);
}

static int
bt_activate(pcap_t* handle)
{
struct pcap_bt *handlep = handle->priv;
struct sockaddr_hci addr;
int opt;
int dev_id;
struct hci_filter flt;
int err = PCAP_ERROR;


if (sscanf(handle->opt.device, BT_IFACE"%d", &dev_id) != 1)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Can't get Bluetooth device index from %s",
handle->opt.device);
return PCAP_ERROR;
}









if (handle->snapshot <= 0 || handle->snapshot > MAXIMUM_SNAPLEN)
handle->snapshot = MAXIMUM_SNAPLEN;


handle->bufsize = BT_CTRL_SIZE+sizeof(pcap_bluetooth_h4_header)+handle->snapshot;
handle->linktype = DLT_BLUETOOTH_HCI_H4_WITH_PHDR;

handle->read_op = bt_read_linux;
handle->inject_op = bt_inject_linux;
handle->setfilter_op = install_bpf_program;
handle->setdirection_op = bt_setdirection_linux;
handle->set_datalink_op = NULL;
handle->getnonblock_op = pcap_getnonblock_fd;
handle->setnonblock_op = pcap_setnonblock_fd;
handle->stats_op = bt_stats_linux;
handlep->dev_id = dev_id;


handle->fd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
if (handle->fd < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't create raw socket");
return PCAP_ERROR;
}

handle->buffer = malloc(handle->bufsize);
if (!handle->buffer) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't allocate dump buffer");
goto close_fail;
}

opt = 1;
if (setsockopt(handle->fd, SOL_HCI, HCI_DATA_DIR, &opt, sizeof(opt)) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't enable data direction info");
goto close_fail;
}

opt = 1;
if (setsockopt(handle->fd, SOL_HCI, HCI_TIME_STAMP, &opt, sizeof(opt)) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't enable time stamp");
goto close_fail;
}



memset(&flt, 0, sizeof(flt));
memset((void *) &flt.type_mask, 0xff, sizeof(flt.type_mask));
memset((void *) &flt.event_mask, 0xff, sizeof(flt.event_mask));
if (setsockopt(handle->fd, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't set filter");
goto close_fail;
}



addr.hci_family = AF_BLUETOOTH;
addr.hci_dev = handlep->dev_id;
#if defined(HAVE_STRUCT_SOCKADDR_HCI_HCI_CHANNEL)
addr.hci_channel = HCI_CHANNEL_RAW;
#endif
if (bind(handle->fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't attach to device %d", handlep->dev_id);
goto close_fail;
}

if (handle->opt.rfmon) {



err = PCAP_ERROR_RFMON_NOTSUP;
goto close_fail;
}

if (handle->opt.buffer_size != 0) {



if (setsockopt(handle->fd, SOL_SOCKET, SO_RCVBUF,
&handle->opt.buffer_size,
sizeof(handle->opt.buffer_size)) == -1) {
pcap_fmt_errmsg_for_errno(handle->errbuf,
errno, PCAP_ERRBUF_SIZE, "SO_RCVBUF");
goto close_fail;
}
}

handle->selectable_fd = handle->fd;
return 0;

close_fail:
pcap_cleanup_live_common(handle);
return err;
}

static int
bt_read_linux(pcap_t *handle, int max_packets _U_, pcap_handler callback, u_char *user)
{
struct cmsghdr *cmsg;
struct msghdr msg;
struct iovec iv;
ssize_t ret;
struct pcap_pkthdr pkth;
pcap_bluetooth_h4_header* bthdr;
u_char *pktd;
int in = 0;

pktd = (u_char *)handle->buffer + BT_CTRL_SIZE;
bthdr = (pcap_bluetooth_h4_header*)(void *)pktd;
iv.iov_base = pktd + sizeof(pcap_bluetooth_h4_header);
iv.iov_len = handle->snapshot;

memset(&msg, 0, sizeof(msg));
msg.msg_iov = &iv;
msg.msg_iovlen = 1;
msg.msg_control = handle->buffer;
msg.msg_controllen = BT_CTRL_SIZE;


do {
ret = recvmsg(handle->fd, &msg, 0);
if (handle->break_loop)
{
handle->break_loop = 0;
return -2;
}
} while ((ret == -1) && (errno == EINTR));

if (ret < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't receive packet");
return -1;
}

pkth.caplen = ret;


cmsg = CMSG_FIRSTHDR(&msg);
while (cmsg) {
switch (cmsg->cmsg_type) {
case HCI_CMSG_DIR:
memcpy(&in, CMSG_DATA(cmsg), sizeof in);
break;
case HCI_CMSG_TSTAMP:
memcpy(&pkth.ts, CMSG_DATA(cmsg),
sizeof pkth.ts);
break;
}
cmsg = CMSG_NXTHDR(&msg, cmsg);
}
if ((in && (handle->direction == PCAP_D_OUT)) ||
((!in) && (handle->direction == PCAP_D_IN)))
return 0;

bthdr->direction = htonl(in != 0);
pkth.caplen+=sizeof(pcap_bluetooth_h4_header);
pkth.len = pkth.caplen;
if (handle->fcode.bf_insns == NULL ||
bpf_filter(handle->fcode.bf_insns, pktd, pkth.len, pkth.caplen)) {
callback(user, &pkth, pktd);
return 1;
}
return 0;
}

static int
bt_inject_linux(pcap_t *handle, const void *buf _U_, size_t size _U_)
{
pcap_snprintf(handle->errbuf, PCAP_ERRBUF_SIZE,
"Packet injection is not supported on Bluetooth devices");
return (-1);
}


static int
bt_stats_linux(pcap_t *handle, struct pcap_stat *stats)
{
struct pcap_bt *handlep = handle->priv;
int ret;
struct hci_dev_info dev_info;
struct hci_dev_stats * s = &dev_info.stat;
dev_info.dev_id = handlep->dev_id;


do {
ret = ioctl(handle->fd, HCIGETDEVINFO, (void *)&dev_info);
} while ((ret == -1) && (errno == EINTR));

if (ret < 0) {
pcap_fmt_errmsg_for_errno(handle->errbuf, PCAP_ERRBUF_SIZE,
errno, "Can't get stats via ioctl");
return (-1);

}


stats->ps_recv = s->evt_rx + s->acl_rx + s->sco_rx + s->cmd_tx +
s->acl_tx +s->sco_tx;
stats->ps_drop = s->err_rx + s->err_tx;
stats->ps_ifdrop = 0;
return 0;
}

static int
bt_setdirection_linux(pcap_t *p, pcap_direction_t d)
{
p->direction = d;
return 0;
}
