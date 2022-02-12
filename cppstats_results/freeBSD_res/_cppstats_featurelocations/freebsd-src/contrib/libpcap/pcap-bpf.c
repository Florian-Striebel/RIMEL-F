




















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/param.h>
#include <sys/socket.h>
#include <time.h>













#include <sys/ioctl.h>
#if defined(HAVE_SYS_IOCCOM_H)
#include <sys/ioccom.h>
#endif
#include <sys/utsname.h>

#if defined(__FreeBSD__) && defined(SIOCIFCREATE2)



static const char usbus_prefix[] = "usbus";
#define USBUS_PREFIX_LEN (sizeof(usbus_prefix) - 1)
#include <dirent.h>
#endif

#include <net/if.h>

#if defined(_AIX)





#define PCAP_DONT_INCLUDE_PCAP_BPF_H

#include <sys/types.h>






#undef _AIX
#include <net/bpf.h>
#define _AIX





#if defined(BIOCROTZBUF) && defined(BPF_BUFMODE_ZBUF)
#define HAVE_ZEROCOPY_BPF
#include <sys/mman.h>
#include <machine/atomic.h>
#endif

#include <net/if_types.h>
#include <sys/sysconfig.h>
#include <sys/device.h>
#include <sys/cfgodm.h>
#include <cf.h>

#if defined(__64BIT__)
#define domakedev makedev64
#define getmajor major64
#define bpf_hdr bpf_hdr32
#else
#define domakedev makedev
#define getmajor major
#endif

#define BPF_NAME "bpf"
#define BPF_MINORS 4
#define DRIVER_PATH "/usr/lib/drivers"
#define BPF_NODE "/dev/bpf"
static int bpfloadedflag = 0;
static int odmlockid = 0;

static int bpf_load(char *errbuf);

#else

#include <net/bpf.h>

#endif

#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(SIOCGIFMEDIA)
#include <net/if_media.h>
#endif

#include "pcap-int.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif





#if defined(__NetBSD__) && __NetBSD_Version__ > 106000000
#define PCAP_FDDIPAD 3
#endif




struct pcap_bpf {
#if defined(HAVE_ZEROCOPY_BPF)









u_char *zbuf1, *zbuf2, *zbuffer;
u_int zbufsize;
u_int zerocopy;
u_int interrupted;
struct timespec firstsel;





struct bpf_zbuf_header *bzh;
int nonblock;
#endif

char *device;
int filtering_in_kernel;
int must_do_on_close;
};




#define MUST_CLEAR_RFMON 0x00000001
#define MUST_DESTROY_USBUS 0x00000002

#if defined(BIOCGDLTLIST)
#if (defined(HAVE_NET_IF_MEDIA_H) && defined(IFM_IEEE80211)) && !defined(__APPLE__)
#define HAVE_BSD_IEEE80211







#if defined(IFM_GMASK) && IFM_GMASK > 0xFFFFFFFF
#define IFM_ULIST_TYPE uint64_t
#else
#define IFM_ULIST_TYPE int
#endif
#endif

#if defined(__APPLE__) || defined(HAVE_BSD_IEEE80211)
static int find_802_11(struct bpf_dltlist *);

#if defined(HAVE_BSD_IEEE80211)
static int monitor_mode(pcap_t *, int);
#endif

#if defined(__APPLE__)
static void remove_non_802_11(pcap_t *);
static void remove_802_11(pcap_t *);
#endif

#endif

#endif

#if defined(sun) && defined(LIFNAMSIZ) && defined(lifr_zoneid)
#include <zone.h>
#endif





#if !defined(DLT_DOCSIS)
#define DLT_DOCSIS 143
#endif






#if !defined(DLT_PRISM_HEADER)
#define DLT_PRISM_HEADER 119
#endif
#if !defined(DLT_AIRONET_HEADER)
#define DLT_AIRONET_HEADER 120
#endif
#if !defined(DLT_IEEE802_11_RADIO)
#define DLT_IEEE802_11_RADIO 127
#endif
#if !defined(DLT_IEEE802_11_RADIO_AVS)
#define DLT_IEEE802_11_RADIO_AVS 163
#endif

static int pcap_can_set_rfmon_bpf(pcap_t *p);
static int pcap_activate_bpf(pcap_t *p);
static int pcap_setfilter_bpf(pcap_t *p, struct bpf_program *fp);
static int pcap_setdirection_bpf(pcap_t *, pcap_direction_t);
static int pcap_set_datalink_bpf(pcap_t *p, int dlt);






static int
pcap_getnonblock_bpf(pcap_t *p)
{
#if defined(HAVE_ZEROCOPY_BPF)
struct pcap_bpf *pb = p->priv;

if (pb->zerocopy)
return (pb->nonblock);
#endif
return (pcap_getnonblock_fd(p));
}

static int
pcap_setnonblock_bpf(pcap_t *p, int nonblock)
{
#if defined(HAVE_ZEROCOPY_BPF)
struct pcap_bpf *pb = p->priv;

if (pb->zerocopy) {
pb->nonblock = nonblock;
return (0);
}
#endif
return (pcap_setnonblock_fd(p, nonblock));
}

#if defined(HAVE_ZEROCOPY_BPF)









static int
pcap_next_zbuf_shm(pcap_t *p, int *cc)
{
struct pcap_bpf *pb = p->priv;
struct bpf_zbuf_header *bzh;

if (pb->zbuffer == pb->zbuf2 || pb->zbuffer == NULL) {
bzh = (struct bpf_zbuf_header *)pb->zbuf1;
if (bzh->bzh_user_gen !=
atomic_load_acq_int(&bzh->bzh_kernel_gen)) {
pb->bzh = bzh;
pb->zbuffer = (u_char *)pb->zbuf1;
p->buffer = pb->zbuffer + sizeof(*bzh);
*cc = bzh->bzh_kernel_len;
return (1);
}
} else if (pb->zbuffer == pb->zbuf1) {
bzh = (struct bpf_zbuf_header *)pb->zbuf2;
if (bzh->bzh_user_gen !=
atomic_load_acq_int(&bzh->bzh_kernel_gen)) {
pb->bzh = bzh;
pb->zbuffer = (u_char *)pb->zbuf2;
p->buffer = pb->zbuffer + sizeof(*bzh);
*cc = bzh->bzh_kernel_len;
return (1);
}
}
*cc = 0;
return (0);
}








static int
pcap_next_zbuf(pcap_t *p, int *cc)
{
struct pcap_bpf *pb = p->priv;
struct bpf_zbuf bz;
struct timeval tv;
struct timespec cur;
fd_set r_set;
int data, r;
int expire, tmout;

#define TSTOMILLI(ts) (((ts)->tv_sec * 1000) + ((ts)->tv_nsec / 1000000))




data = pcap_next_zbuf_shm(p, cc);
if (data)
return (data);








tmout = p->opt.timeout;
if (tmout)
(void) clock_gettime(CLOCK_MONOTONIC, &cur);
if (pb->interrupted && p->opt.timeout) {
expire = TSTOMILLI(&pb->firstsel) + p->opt.timeout;
tmout = expire - TSTOMILLI(&cur);
#undef TSTOMILLI
if (tmout <= 0) {
pb->interrupted = 0;
data = pcap_next_zbuf_shm(p, cc);
if (data)
return (data);
if (ioctl(p->fd, BIOCROTZBUF, &bz) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno, "BIOCROTZBUF");
return (PCAP_ERROR);
}
return (pcap_next_zbuf_shm(p, cc));
}
}





if (!pb->nonblock) {
FD_ZERO(&r_set);
FD_SET(p->fd, &r_set);
if (tmout != 0) {
tv.tv_sec = tmout / 1000;
tv.tv_usec = (tmout * 1000) % 1000000;
}
r = select(p->fd + 1, &r_set, NULL, NULL,
p->opt.timeout != 0 ? &tv : NULL);
if (r < 0 && errno == EINTR) {
if (!pb->interrupted && p->opt.timeout) {
pb->interrupted = 1;
pb->firstsel = cur;
}
return (0);
} else if (r < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "select");
return (PCAP_ERROR);
}
}
pb->interrupted = 0;





data = pcap_next_zbuf_shm(p, cc);
if (data)
return (data);




if (ioctl(p->fd, BIOCROTZBUF, &bz) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCROTZBUF");
return (PCAP_ERROR);
}
return (pcap_next_zbuf_shm(p, cc));
}





static int
pcap_ack_zbuf(pcap_t *p)
{
struct pcap_bpf *pb = p->priv;

atomic_store_rel_int(&pb->bzh->bzh_user_gen,
pb->bzh->bzh_kernel_gen);
pb->bzh = NULL;
p->buffer = NULL;
return (0);
}
#endif

pcap_t *
pcap_create_interface(const char *device _U_, char *ebuf)
{
pcap_t *p;

p = pcap_create_common(ebuf, sizeof (struct pcap_bpf));
if (p == NULL)
return (NULL);

p->activate_op = pcap_activate_bpf;
p->can_set_rfmon_op = pcap_can_set_rfmon_bpf;
#if defined(BIOCSTSTAMP)




p->tstamp_precision_count = 2;
p->tstamp_precision_list = malloc(2 * sizeof(u_int));
if (p->tstamp_precision_list == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE, errno,
"malloc");
free(p);
return (NULL);
}
p->tstamp_precision_list[0] = PCAP_TSTAMP_PRECISION_MICRO;
p->tstamp_precision_list[1] = PCAP_TSTAMP_PRECISION_NANO;
#endif
return (p);
}





static int
bpf_open(char *errbuf)
{
int fd = -1;
static const char cloning_device[] = "/dev/bpf";
int n = 0;
char device[sizeof "/dev/bpf0000000000"];
static int no_cloning_bpf = 0;

#if defined(_AIX)





if (bpf_load(errbuf) == PCAP_ERROR)
return (PCAP_ERROR);
#endif







if (!no_cloning_bpf &&
(fd = open(cloning_device, O_RDWR)) == -1 &&
((errno != EACCES && errno != ENOENT) ||
(fd = open(cloning_device, O_RDONLY)) == -1)) {
if (errno != ENOENT) {
if (errno == EACCES)
fd = PCAP_ERROR_PERM_DENIED;
else
fd = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "(cannot open device) %s", cloning_device);
return (fd);
}
no_cloning_bpf = 1;
}

if (no_cloning_bpf) {





do {
(void)pcap_snprintf(device, sizeof(device), "/dev/bpf%d", n++);














fd = open(device, O_RDWR);
if (fd == -1 && errno == EACCES)
fd = open(device, O_RDONLY);
} while (fd < 0 && errno == EBUSY);
}




if (fd < 0) {
switch (errno) {

case ENOENT:
fd = PCAP_ERROR;
if (n == 1) {





pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"(there are no BPF devices)");
} else {






pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"(all BPF devices are busy)");
}
break;

case EACCES:





fd = PCAP_ERROR_PERM_DENIED;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "(cannot open BPF device) %s", device);
break;

default:



fd = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "(cannot open BPF device) %s", device);
break;
}
}

return (fd);
}









static int
bpf_open_and_bind(const char *name, char *errbuf)
{
int fd;
struct ifreq ifr;




fd = bpf_open(errbuf);
if (fd < 0)
return (fd);




(void)strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
if (ioctl(fd, BIOCSETIF, (caddr_t)&ifr) < 0) {
switch (errno) {

case ENXIO:



close(fd);
return (PCAP_ERROR_NO_SUCH_DEVICE);

case ENETDOWN:







close(fd);
return (PCAP_ERROR_IFACE_NOT_UP);

default:
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETIF: %s", name);
close(fd);
return (PCAP_ERROR);
}
}




return (fd);
}

#if defined(BIOCGDLTLIST)
static int
get_dlt_list(int fd, int v, struct bpf_dltlist *bdlp, char *ebuf)
{
memset(bdlp, 0, sizeof(*bdlp));
if (ioctl(fd, BIOCGDLTLIST, (caddr_t)bdlp) == 0) {
u_int i;
int is_ethernet;

bdlp->bfl_list = (u_int *) malloc(sizeof(u_int) * (bdlp->bfl_len + 1));
if (bdlp->bfl_list == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (PCAP_ERROR);
}

if (ioctl(fd, BIOCGDLTLIST, (caddr_t)bdlp) < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "BIOCGDLTLIST");
free(bdlp->bfl_list);
return (PCAP_ERROR);
}























if (v == DLT_EN10MB) {
is_ethernet = 1;
for (i = 0; i < bdlp->bfl_len; i++) {
if (bdlp->bfl_list[i] != DLT_EN10MB
#if defined(DLT_IPNET)
&& bdlp->bfl_list[i] != DLT_IPNET
#endif
) {
is_ethernet = 0;
break;
}
}
if (is_ethernet) {




bdlp->bfl_list[bdlp->bfl_len] = DLT_DOCSIS;
bdlp->bfl_len++;
}
}
} else {




if (errno != EINVAL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "BIOCGDLTLIST");
return (PCAP_ERROR);
}
}
return (0);
}
#endif

#if defined(__APPLE__)
static int
pcap_can_set_rfmon_bpf(pcap_t *p)
{
struct utsname osinfo;
struct ifreq ifr;
int fd;
#if defined(BIOCGDLTLIST)
struct bpf_dltlist bdl;
#endif


















if (uname(&osinfo) == -1) {



return (0);
}




if (osinfo.release[0] < '8' && osinfo.release[1] == '.') {




return (0);
}
if (osinfo.release[0] == '8' && osinfo.release[1] == '.') {




if (strncmp(p->opt.device, "en", 2) != 0) {



return (0);
}
fd = socket(AF_INET, SOCK_DGRAM, 0);
if (fd == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "socket");
return (PCAP_ERROR);
}
pcap_strlcpy(ifr.ifr_name, "wlt", sizeof(ifr.ifr_name));
pcap_strlcat(ifr.ifr_name, p->opt.device + 2, sizeof(ifr.ifr_name));
if (ioctl(fd, SIOCGIFFLAGS, (char *)&ifr) < 0) {



close(fd);
return (0);
}
close(fd);
return (1);
}

#if defined(BIOCGDLTLIST)







fd = bpf_open(p->errbuf);
if (fd < 0)
return (fd);




(void)strncpy(ifr.ifr_name, p->opt.device, sizeof(ifr.ifr_name));
if (ioctl(fd, BIOCSETIF, (caddr_t)&ifr) < 0) {
switch (errno) {

case ENXIO:



close(fd);
return (PCAP_ERROR_NO_SUCH_DEVICE);

case ENETDOWN:







close(fd);
return (PCAP_ERROR_IFACE_NOT_UP);

default:
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETIF: %s", p->opt.device);
close(fd);
return (PCAP_ERROR);
}
}








if (get_dlt_list(fd, DLT_NULL, &bdl, p->errbuf) == PCAP_ERROR) {
close(fd);
return (PCAP_ERROR);
}
if (find_802_11(&bdl) != -1) {



free(bdl.bfl_list);
close(fd);
return (1);
}
free(bdl.bfl_list);
close(fd);
#endif
return (0);
}
#elif defined(HAVE_BSD_IEEE80211)
static int
pcap_can_set_rfmon_bpf(pcap_t *p)
{
int ret;

ret = monitor_mode(p, 0);
if (ret == PCAP_ERROR_RFMON_NOTSUP)
return (0);
if (ret == 0)
return (1);
return (ret);
}
#else
static int
pcap_can_set_rfmon_bpf(pcap_t *p _U_)
{
return (0);
}
#endif

static int
pcap_stats_bpf(pcap_t *p, struct pcap_stat *ps)
{
struct bpf_stat s;














if (ioctl(p->fd, BIOCGSTATS, (caddr_t)&s) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCGSTATS");
return (PCAP_ERROR);
}

ps->ps_recv = s.bs_recv;
ps->ps_drop = s.bs_drop;
ps->ps_ifdrop = 0;
return (0);
}

static int
pcap_read_bpf(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_bpf *pb = p->priv;
int cc;
int n = 0;
register u_char *bp, *ep;
u_char *datap;
#if defined(PCAP_FDDIPAD)
register u_int pad;
#endif
#if defined(HAVE_ZEROCOPY_BPF)
int i;
#endif

again:



if (p->break_loop) {





p->break_loop = 0;
return (PCAP_ERROR_BREAK);
}
cc = p->cc;
if (p->cc == 0) {








#if defined(HAVE_ZEROCOPY_BPF)
if (pb->zerocopy) {
if (p->buffer != NULL)
pcap_ack_zbuf(p);
i = pcap_next_zbuf(p, &cc);
if (i == 0)
goto again;
if (i < 0)
return (PCAP_ERROR);
} else
#endif
{
cc = read(p->fd, p->buffer, p->bufsize);
}
if (cc < 0) {

switch (errno) {

case EINTR:
goto again;

#if defined(_AIX)
case EFAULT:





















goto again;
#endif

case EWOULDBLOCK:
return (0);

case ENXIO:
case EIO:











pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"The interface disappeared");
return (PCAP_ERROR);

#if defined(sun) && !defined(BSD) && !defined(__svr4__) && !defined(__SVR4)





case EINVAL:
if (lseek(p->fd, 0L, SEEK_CUR) +
p->bufsize < 0) {
(void)lseek(p->fd, 0L, SEEK_SET);
goto again;
}

#endif
}
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "read");
return (PCAP_ERROR);
}
bp = (u_char *)p->buffer;
} else
bp = p->bp;




#if defined(BIOCSTSTAMP)
#define bhp ((struct bpf_xhdr *)bp)
#else
#define bhp ((struct bpf_hdr *)bp)
#endif
ep = bp + cc;
#if defined(PCAP_FDDIPAD)
pad = p->fddipad;
#endif
while (bp < ep) {
register u_int caplen, hdrlen;











if (p->break_loop) {
p->bp = bp;
p->cc = ep - bp;













if (p->cc < 0)
p->cc = 0;
if (n == 0) {
p->break_loop = 0;
return (PCAP_ERROR_BREAK);
} else
return (n);
}

caplen = bhp->bh_caplen;
hdrlen = bhp->bh_hdrlen;
datap = bp + hdrlen;













if (pb->filtering_in_kernel ||
bpf_filter(p->fcode.bf_insns, datap, bhp->bh_datalen, caplen)) {
struct pcap_pkthdr pkthdr;
#if defined(BIOCSTSTAMP)
struct bintime bt;

bt.sec = bhp->bh_tstamp.bt_sec;
bt.frac = bhp->bh_tstamp.bt_frac;
if (p->opt.tstamp_precision == PCAP_TSTAMP_PRECISION_NANO) {
struct timespec ts;

bintime2timespec(&bt, &ts);
pkthdr.ts.tv_sec = ts.tv_sec;
pkthdr.ts.tv_usec = ts.tv_nsec;
} else {
struct timeval tv;

bintime2timeval(&bt, &tv);
pkthdr.ts.tv_sec = tv.tv_sec;
pkthdr.ts.tv_usec = tv.tv_usec;
}
#else
pkthdr.ts.tv_sec = bhp->bh_tstamp.tv_sec;
#if defined(_AIX)




pkthdr.ts.tv_usec = bhp->bh_tstamp.tv_usec/1000;
#else
pkthdr.ts.tv_usec = bhp->bh_tstamp.tv_usec;
#endif
#endif
#if defined(PCAP_FDDIPAD)
if (caplen > pad)
pkthdr.caplen = caplen - pad;
else
pkthdr.caplen = 0;
if (bhp->bh_datalen > pad)
pkthdr.len = bhp->bh_datalen - pad;
else
pkthdr.len = 0;
datap += pad;
#else
pkthdr.caplen = caplen;
pkthdr.len = bhp->bh_datalen;
#endif
(*callback)(user, &pkthdr, datap);
bp += BPF_WORDALIGN(caplen + hdrlen);
if (++n >= cnt && !PACKET_COUNT_IS_UNLIMITED(cnt)) {
p->bp = bp;
p->cc = ep - bp;



if (p->cc < 0)
p->cc = 0;
return (n);
}
} else {



bp += BPF_WORDALIGN(caplen + hdrlen);
}
}
#undef bhp
p->cc = 0;
return (n);
}

static int
pcap_inject_bpf(pcap_t *p, const void *buf, size_t size)
{
int ret;

ret = write(p->fd, buf, size);
#if defined(__APPLE__)
if (ret == -1 && errno == EAFNOSUPPORT) {


















u_int spoof_eth_src = 0;

if (ioctl(p->fd, BIOCSHDRCMPLT, &spoof_eth_src) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "send: can't turn off BIOCSHDRCMPLT");
return (PCAP_ERROR);
}




ret = write(p->fd, buf, size);
}
#endif
if (ret == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "send");
return (PCAP_ERROR);
}
return (ret);
}

#if defined(_AIX)
static int
bpf_odminit(char *errbuf)
{
char *errstr;

if (odm_initialize() == -1) {
if (odm_err_msg(odmerrno, &errstr) == -1)
errstr = "Unknown error";
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"bpf_load: odm_initialize failed: %s",
errstr);
return (PCAP_ERROR);
}

if ((odmlockid = odm_lock("/etc/objrepos/config_lock", ODM_WAIT)) == -1) {
if (odm_err_msg(odmerrno, &errstr) == -1)
errstr = "Unknown error";
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"bpf_load: odm_lock of /etc/objrepos/config_lock failed: %s",
errstr);
(void)odm_terminate();
return (PCAP_ERROR);
}

return (0);
}

static int
bpf_odmcleanup(char *errbuf)
{
char *errstr;

if (odm_unlock(odmlockid) == -1) {
if (errbuf != NULL) {
if (odm_err_msg(odmerrno, &errstr) == -1)
errstr = "Unknown error";
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"bpf_load: odm_unlock failed: %s",
errstr);
}
return (PCAP_ERROR);
}

if (odm_terminate() == -1) {
if (errbuf != NULL) {
if (odm_err_msg(odmerrno, &errstr) == -1)
errstr = "Unknown error";
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"bpf_load: odm_terminate failed: %s",
errstr);
}
return (PCAP_ERROR);
}

return (0);
}

static int
bpf_load(char *errbuf)
{
long major;
int *minors;
int numminors, i, rc;
char buf[1024];
struct stat sbuf;
struct bpf_config cfg_bpf;
struct cfg_load cfg_ld;
struct cfg_kmod cfg_km;





if (bpfloadedflag)
return (0);

if (bpf_odminit(errbuf) == PCAP_ERROR)
return (PCAP_ERROR);

major = genmajor(BPF_NAME);
if (major == -1) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "bpf_load: genmajor failed");
(void)bpf_odmcleanup(NULL);
return (PCAP_ERROR);
}

minors = getminor(major, &numminors, BPF_NAME);
if (!minors) {
minors = genminor("bpf", major, 0, BPF_MINORS, 1, 1);
if (!minors) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "bpf_load: genminor failed");
(void)bpf_odmcleanup(NULL);
return (PCAP_ERROR);
}
}

if (bpf_odmcleanup(errbuf) == PCAP_ERROR)
return (PCAP_ERROR);

rc = stat(BPF_NODE "0", &sbuf);
if (rc == -1 && errno != ENOENT) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "bpf_load: can't stat %s", BPF_NODE "0");
return (PCAP_ERROR);
}

if (rc == -1 || getmajor(sbuf.st_rdev) != major) {
for (i = 0; i < BPF_MINORS; i++) {
pcap_snprintf(buf, sizeof(buf), "%s%d", BPF_NODE, i);
unlink(buf);
if (mknod(buf, S_IRUSR | S_IFCHR, domakedev(major, i)) == -1) {
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno,
"bpf_load: can't mknod %s", buf);
return (PCAP_ERROR);
}
}
}


memset(&cfg_ld, 0x0, sizeof(cfg_ld));
pcap_snprintf(buf, sizeof(buf), "%s/%s", DRIVER_PATH, BPF_NAME);
cfg_ld.path = buf;
if ((sysconfig(SYS_QUERYLOAD, (void *)&cfg_ld, sizeof(cfg_ld)) == -1) ||
(cfg_ld.kmid == 0)) {

if (sysconfig(SYS_SINGLELOAD, (void *)&cfg_ld, sizeof(cfg_ld)) == -1) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "bpf_load: could not load driver");
return (PCAP_ERROR);
}
}


cfg_km.cmd = CFG_INIT;
cfg_km.kmid = cfg_ld.kmid;
cfg_km.mdilen = sizeof(cfg_bpf);
cfg_km.mdiptr = (void *)&cfg_bpf;
for (i = 0; i < BPF_MINORS; i++) {
cfg_bpf.devno = domakedev(major, i);
if (sysconfig(SYS_CFGKMOD, (void *)&cfg_km, sizeof(cfg_km)) == -1) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "bpf_load: could not configure driver");
return (PCAP_ERROR);
}
}

bpfloadedflag = 1;

return (0);
}
#endif




static void
pcap_cleanup_bpf(pcap_t *p)
{
struct pcap_bpf *pb = p->priv;
#if defined(HAVE_BSD_IEEE80211)
int sock;
struct ifmediareq req;
struct ifreq ifr;
#endif

if (pb->must_do_on_close != 0) {




#if defined(HAVE_BSD_IEEE80211)
if (pb->must_do_on_close & MUST_CLEAR_RFMON) {








sock = socket(AF_INET, SOCK_DGRAM, 0);
if (sock == -1) {
fprintf(stderr,
"Can't restore interface flags (socket() failed: %s).\n"
"Please adjust manually.\n",
strerror(errno));
} else {
memset(&req, 0, sizeof(req));
strncpy(req.ifm_name, pb->device,
sizeof(req.ifm_name));
if (ioctl(sock, SIOCGIFMEDIA, &req) < 0) {
fprintf(stderr,
"Can't restore interface flags (SIOCGIFMEDIA failed: %s).\n"
"Please adjust manually.\n",
strerror(errno));
} else {
if (req.ifm_current & IFM_IEEE80211_MONITOR) {




memset(&ifr, 0, sizeof(ifr));
(void)strncpy(ifr.ifr_name,
pb->device,
sizeof(ifr.ifr_name));
ifr.ifr_media =
req.ifm_current & ~IFM_IEEE80211_MONITOR;
if (ioctl(sock, SIOCSIFMEDIA,
&ifr) == -1) {
fprintf(stderr,
"Can't restore interface flags (SIOCSIFMEDIA failed: %s).\n"
"Please adjust manually.\n",
strerror(errno));
}
}
}
close(sock);
}
}
#endif

#if defined(__FreeBSD__) && defined(SIOCIFCREATE2)



if (pb->must_do_on_close & MUST_DESTROY_USBUS) {
if (if_nametoindex(pb->device) > 0) {
int s;

s = socket(AF_LOCAL, SOCK_DGRAM, 0);
if (s >= 0) {
pcap_strlcpy(ifr.ifr_name, pb->device,
sizeof(ifr.ifr_name));
ioctl(s, SIOCIFDESTROY, &ifr);
close(s);
}
}
}
#endif




pcap_remove_from_pcaps_to_close(p);
pb->must_do_on_close = 0;
}

#if defined(HAVE_ZEROCOPY_BPF)
if (pb->zerocopy) {







if (pb->zbuf1 != MAP_FAILED && pb->zbuf1 != NULL)
(void) munmap(pb->zbuf1, pb->zbufsize);
if (pb->zbuf2 != MAP_FAILED && pb->zbuf2 != NULL)
(void) munmap(pb->zbuf2, pb->zbufsize);
p->buffer = NULL;
}
#endif
if (pb->device != NULL) {
free(pb->device);
pb->device = NULL;
}
pcap_cleanup_live_common(p);
}

static int
check_setif_failure(pcap_t *p, int error)
{
#if defined(__APPLE__)
int fd;
struct ifreq ifr;
int err;
#endif

if (error == ENXIO) {



#if defined(__APPLE__)
if (p->opt.rfmon && strncmp(p->opt.device, "wlt", 3) == 0) {







fd = socket(AF_INET, SOCK_DGRAM, 0);
if (fd != -1) {
pcap_strlcpy(ifr.ifr_name, "en",
sizeof(ifr.ifr_name));
pcap_strlcat(ifr.ifr_name, p->opt.device + 3,
sizeof(ifr.ifr_name));
if (ioctl(fd, SIOCGIFFLAGS, (char *)&ifr) < 0) {





err = PCAP_ERROR_NO_SUCH_DEVICE;
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"SIOCGIFFLAGS on %s failed",
ifr.ifr_name);
} else {










err = PCAP_ERROR_RFMON_NOTSUP;
}
close(fd);
} else {





err = PCAP_ERROR_NO_SUCH_DEVICE;
pcap_fmt_errmsg_for_errno(p->errbuf,
errno, PCAP_ERRBUF_SIZE,
"socket() failed");
}
return (err);
}
#endif



pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETIF failed");
return (PCAP_ERROR_NO_SUCH_DEVICE);
} else if (errno == ENETDOWN) {







return (PCAP_ERROR_IFACE_NOT_UP);
} else {




pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETIF: %s", p->opt.device);
return (PCAP_ERROR);
}
}










#if defined(_AIX)
#define DEFAULT_BUFSIZE 32768
#else
#define DEFAULT_BUFSIZE 524288
#endif

static int
pcap_activate_bpf(pcap_t *p)
{
struct pcap_bpf *pb = p->priv;
int status = 0;
#if defined(HAVE_BSD_IEEE80211)
int retv;
#endif
int fd;
#if defined(LIFNAMSIZ)
char *zonesep;
struct lifreq ifr;
char *ifrname = ifr.lifr_name;
const size_t ifnamsiz = sizeof(ifr.lifr_name);
#else
struct ifreq ifr;
char *ifrname = ifr.ifr_name;
const size_t ifnamsiz = sizeof(ifr.ifr_name);
#endif
struct bpf_version bv;
#if defined(__APPLE__)
int sockfd;
char *wltdev = NULL;
#endif
#if defined(BIOCGDLTLIST)
struct bpf_dltlist bdl;
#if defined(__APPLE__) || defined(HAVE_BSD_IEEE80211)
int new_dlt;
#endif
#endif
#if defined(BIOCGHDRCMPLT) && defined(BIOCSHDRCMPLT)
u_int spoof_eth_src = 1;
#endif
u_int v;
struct bpf_insn total_insn;
struct bpf_program total_prog;
struct utsname osinfo;
int have_osinfo = 0;
#if defined(HAVE_ZEROCOPY_BPF)
struct bpf_zbuf bz;
u_int bufmode, zbufmax;
#endif

fd = bpf_open(p->errbuf);
if (fd < 0) {
status = fd;
goto bad;
}

p->fd = fd;

if (ioctl(fd, BIOCVERSION, (caddr_t)&bv) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCVERSION");
status = PCAP_ERROR;
goto bad;
}
if (bv.bv_major != BPF_MAJOR_VERSION ||
bv.bv_minor < BPF_MINOR_VERSION) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"kernel bpf filter out of date");
status = PCAP_ERROR;
goto bad;
}









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;

#if defined(LIFNAMSIZ) && defined(ZONENAME_MAX) && defined(lifr_zoneid)



if ((ifr.lifr_zoneid = getzoneid()) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "getzoneid()");
status = PCAP_ERROR;
goto bad;
}







if ((zonesep = strchr(p->opt.device, '/')) != NULL) {
char path_zname[ZONENAME_MAX];
int znamelen;
char *lnamep;

if (ifr.lifr_zoneid != GLOBAL_ZONEID) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"zonename/linkname only valid in global zone.");
status = PCAP_ERROR;
goto bad;
}
znamelen = zonesep - p->opt.device;
(void) pcap_strlcpy(path_zname, p->opt.device, znamelen + 1);
ifr.lifr_zoneid = getzoneidbyname(path_zname);
if (ifr.lifr_zoneid == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "getzoneidbyname(%s)", path_zname);
status = PCAP_ERROR;
goto bad;
}
lnamep = strdup(zonesep + 1);
if (lnamep == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "strdup");
status = PCAP_ERROR;
goto bad;
}
free(p->opt.device);
p->opt.device = lnamep;
}
#endif

pb->device = strdup(p->opt.device);
if (pb->device == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "strdup");
status = PCAP_ERROR;
goto bad;
}




if (uname(&osinfo) == 0)
have_osinfo = 1;

#if defined(__APPLE__)




if (p->opt.rfmon) {
if (have_osinfo) {




if (osinfo.release[0] < '8' &&
osinfo.release[1] == '.') {



status = PCAP_ERROR_RFMON_NOTSUP;
goto bad;
}
if (osinfo.release[0] == '8' &&
osinfo.release[1] == '.') {



if (strncmp(p->opt.device, "en", 2) != 0) {




sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if (sockfd != -1) {
pcap_strlcpy(ifrname,
p->opt.device, ifnamsiz);
if (ioctl(sockfd, SIOCGIFFLAGS,
(char *)&ifr) < 0) {







status = PCAP_ERROR_NO_SUCH_DEVICE;
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE,
errno,
"SIOCGIFFLAGS failed");
} else
status = PCAP_ERROR_RFMON_NOTSUP;
close(sockfd);
} else {





status = PCAP_ERROR_NO_SUCH_DEVICE;
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"socket() failed");
}
goto bad;
}
wltdev = malloc(strlen(p->opt.device) + 2);
if (wltdev == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"malloc");
status = PCAP_ERROR;
goto bad;
}
strcpy(wltdev, "wlt");
strcat(wltdev, p->opt.device + 2);
free(p->opt.device);
p->opt.device = wltdev;
}




}
}
#endif





#if defined(__FreeBSD__) && defined(SIOCIFCREATE2)
if (strncmp(p->opt.device, usbus_prefix, USBUS_PREFIX_LEN) == 0) {



if (if_nametoindex(p->opt.device) == 0) {





int s;





s = socket(AF_LOCAL, SOCK_DGRAM, 0);
if (s < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"Can't open socket");
status = PCAP_ERROR;
goto bad;
}





if (!pcap_do_addexit(p)) {




pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"atexit failed");
close(s);
status = PCAP_ERROR;
goto bad;
}




pcap_strlcpy(ifr.ifr_name, p->opt.device, sizeof(ifr.ifr_name));
if (ioctl(s, SIOCIFCREATE2, &ifr) < 0) {
if (errno == EINVAL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"Invalid USB bus interface %s",
p->opt.device);
} else {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"Can't create interface for %s",
p->opt.device);
}
close(s);
status = PCAP_ERROR;
goto bad;
}




pb->must_do_on_close |= MUST_DESTROY_USBUS;




pcap_add_to_pcaps_to_close(p);
}
}
#endif

#if defined(HAVE_ZEROCOPY_BPF)





bufmode = BPF_BUFMODE_ZBUF;
if (ioctl(fd, BIOCSETBUFMODE, (caddr_t)&bufmode) == 0) {



pb->zerocopy = 1;










if (ioctl(fd, BIOCGETZMAX, (caddr_t)&zbufmax) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCGETZMAX");
status = PCAP_ERROR;
goto bad;
}

if (p->opt.buffer_size != 0) {



v = p->opt.buffer_size;
} else {
if ((ioctl(fd, BIOCGBLEN, (caddr_t)&v) < 0) ||
v < DEFAULT_BUFSIZE)
v = DEFAULT_BUFSIZE;
}
#if !defined(roundup)
#define roundup(x, y) ((((x)+((y)-1))/(y))*(y))
#endif
pb->zbufsize = roundup(v, getpagesize());
if (pb->zbufsize > zbufmax)
pb->zbufsize = zbufmax;
pb->zbuf1 = mmap(NULL, pb->zbufsize, PROT_READ | PROT_WRITE,
MAP_ANON, -1, 0);
pb->zbuf2 = mmap(NULL, pb->zbufsize, PROT_READ | PROT_WRITE,
MAP_ANON, -1, 0);
if (pb->zbuf1 == MAP_FAILED || pb->zbuf2 == MAP_FAILED) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "mmap");
status = PCAP_ERROR;
goto bad;
}
memset(&bz, 0, sizeof(bz));
bz.bz_bufa = pb->zbuf1;
bz.bz_bufb = pb->zbuf2;
bz.bz_buflen = pb->zbufsize;
if (ioctl(fd, BIOCSETZBUF, (caddr_t)&bz) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETZBUF");
status = PCAP_ERROR;
goto bad;
}
(void)strncpy(ifrname, p->opt.device, ifnamsiz);
if (ioctl(fd, BIOCSETIF, (caddr_t)&ifr) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETIF: %s", p->opt.device);
status = PCAP_ERROR;
goto bad;
}
v = pb->zbufsize - sizeof(struct bpf_zbuf_header);
} else
#endif
{




if (p->opt.buffer_size != 0) {



if (ioctl(fd, BIOCSBLEN,
(caddr_t)&p->opt.buffer_size) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno,
"BIOCSBLEN: %s", p->opt.device);
status = PCAP_ERROR;
goto bad;
}




(void)strncpy(ifrname, p->opt.device, ifnamsiz);
#if defined(BIOCSETLIF)
if (ioctl(fd, BIOCSETLIF, (caddr_t)&ifr) < 0)
#else
if (ioctl(fd, BIOCSETIF, (caddr_t)&ifr) < 0)
#endif
{
status = check_setif_failure(p, errno);
goto bad;
}
} else {









if ((ioctl(fd, BIOCGBLEN, (caddr_t)&v) < 0) ||
v < DEFAULT_BUFSIZE)
v = DEFAULT_BUFSIZE;
for ( ; v != 0; v >>= 1) {







(void) ioctl(fd, BIOCSBLEN, (caddr_t)&v);

(void)strncpy(ifrname, p->opt.device, ifnamsiz);
#if defined(BIOCSETLIF)
if (ioctl(fd, BIOCSETLIF, (caddr_t)&ifr) >= 0)
#else
if (ioctl(fd, BIOCSETIF, (caddr_t)&ifr) >= 0)
#endif
break;

if (errno != ENOBUFS) {
status = check_setif_failure(p, errno);
goto bad;
}
}

if (v == 0) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"BIOCSBLEN: %s: No buffer size worked",
p->opt.device);
status = PCAP_ERROR;
goto bad;
}
}
}


if (ioctl(fd, BIOCGDLT, (caddr_t)&v) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCGDLT");
status = PCAP_ERROR;
goto bad;
}

#if defined(_AIX)



switch (v) {

case IFT_ETHER:
case IFT_ISO88023:
v = DLT_EN10MB;
break;

case IFT_FDDI:
v = DLT_FDDI;
break;

case IFT_ISO88025:
v = DLT_IEEE802;
break;

case IFT_LOOP:
v = DLT_NULL;
break;

default:



pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "unknown interface type %u",
v);
status = PCAP_ERROR;
goto bad;
}
#endif
#if _BSDI_VERSION - 0 >= 199510

switch (v) {

case DLT_SLIP:
v = DLT_SLIP_BSDOS;
break;

case DLT_PPP:
v = DLT_PPP_BSDOS;
break;

case 11:
v = DLT_FRELAY;
break;

case 12:
v = DLT_CHDLC;
break;
}
#endif

#if defined(BIOCGDLTLIST)





if (get_dlt_list(fd, v, &bdl, p->errbuf) == -1) {
status = PCAP_ERROR;
goto bad;
}
p->dlt_count = bdl.bfl_len;
p->dlt_list = bdl.bfl_list;

#if defined(__APPLE__)

















if (have_osinfo) {
if (isdigit((unsigned)osinfo.release[0]) &&
(osinfo.release[0] == '9' ||
isdigit((unsigned)osinfo.release[1]))) {



new_dlt = find_802_11(&bdl);
if (new_dlt != -1) {






if (p->opt.rfmon) {






remove_non_802_11(p);






if ((u_int)new_dlt != v) {
if (ioctl(p->fd, BIOCSDLT,
&new_dlt) != -1) {





v = new_dlt;
}
}
} else {









if (!p->oldstyle)
remove_802_11(p);
}
} else {
if (p->opt.rfmon) {






status = PCAP_ERROR_RFMON_NOTSUP;
goto bad;
}
}
}
}
#elif defined(HAVE_BSD_IEEE80211)




if (p->opt.rfmon) {



retv = monitor_mode(p, 1);
if (retv != 0) {



status = retv;
goto bad;
}







new_dlt = find_802_11(&bdl);
if (new_dlt != -1) {








if ((u_int)new_dlt != v) {
if (ioctl(p->fd, BIOCSDLT, &new_dlt) != -1) {




v = new_dlt;
}
}
}
}
#endif
#endif









if (v == DLT_EN10MB && p->dlt_count == 0) {
p->dlt_list = (u_int *) malloc(sizeof(u_int) * 2);



if (p->dlt_list != NULL) {
p->dlt_list[0] = DLT_EN10MB;
p->dlt_list[1] = DLT_DOCSIS;
p->dlt_count = 2;
}
}
#if defined(PCAP_FDDIPAD)
if (v == DLT_FDDI)
p->fddipad = PCAP_FDDIPAD;
else
#endif
p->fddipad = 0;
p->linktype = v;

#if defined(BIOCGHDRCMPLT) && defined(BIOCSHDRCMPLT)









if (ioctl(fd, BIOCSHDRCMPLT, &spoof_eth_src) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSHDRCMPLT");
status = PCAP_ERROR;
goto bad;
}
#endif

#if defined(HAVE_ZEROCOPY_BPF)





if (p->opt.timeout && !pb->zerocopy) {
#else
if (p->opt.timeout) {
#endif

















struct timeval to;
#if defined(HAVE_STRUCT_BPF_TIMEVAL)
struct BPF_TIMEVAL bpf_to;

if (IOCPARM_LEN(BIOCSRTIMEOUT) != sizeof(struct timeval)) {
bpf_to.tv_sec = p->opt.timeout / 1000;
bpf_to.tv_usec = (p->opt.timeout * 1000) % 1000000;
if (ioctl(p->fd, BIOCSRTIMEOUT, (caddr_t)&bpf_to) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf,
errno, PCAP_ERRBUF_SIZE, "BIOCSRTIMEOUT");
status = PCAP_ERROR;
goto bad;
}
} else {
#endif
to.tv_sec = p->opt.timeout / 1000;
to.tv_usec = (p->opt.timeout * 1000) % 1000000;
if (ioctl(p->fd, BIOCSRTIMEOUT, (caddr_t)&to) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf,
errno, PCAP_ERRBUF_SIZE, "BIOCSRTIMEOUT");
status = PCAP_ERROR;
goto bad;
}
#if defined(HAVE_STRUCT_BPF_TIMEVAL)
}
#endif
}

#if defined(BIOCIMMEDIATE)



















#if !defined(_AIX)
if (p->opt.immediate) {
#endif
v = 1;
if (ioctl(p->fd, BIOCIMMEDIATE, &v) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCIMMEDIATE");
status = PCAP_ERROR;
goto bad;
}
#if !defined(_AIX)
}
#endif
#else
if (p->opt.immediate) {



pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Immediate mode not supported");
status = PCAP_ERROR;
goto bad;
}
#endif

if (p->opt.promisc) {

if (ioctl(p->fd, BIOCPROMISC, NULL) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCPROMISC");
status = PCAP_WARNING_PROMISC_NOTSUP;
}
}

#if defined(BIOCSTSTAMP)
v = BPF_T_BINTIME;
if (ioctl(p->fd, BIOCSTSTAMP, &v) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSTSTAMP");
status = PCAP_ERROR;
goto bad;
}
#endif

if (ioctl(fd, BIOCGBLEN, (caddr_t)&v) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCGBLEN");
status = PCAP_ERROR;
goto bad;
}
p->bufsize = v;
#if defined(HAVE_ZEROCOPY_BPF)
if (!pb->zerocopy) {
#endif
p->buffer = malloc(p->bufsize);
if (p->buffer == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
status = PCAP_ERROR;
goto bad;
}
#if defined(_AIX)


memset(p->buffer, 0x0, p->bufsize);
#endif
#if defined(HAVE_ZEROCOPY_BPF)
}
#endif










total_insn.code = (u_short)(BPF_RET | BPF_K);
total_insn.jt = 0;
total_insn.jf = 0;
total_insn.k = p->snapshot;

total_prog.bf_len = 1;
total_prog.bf_insns = &total_insn;
if (ioctl(p->fd, BIOCSETF, (caddr_t)&total_prog) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETF");
status = PCAP_ERROR;
goto bad;
}





































p->selectable_fd = p->fd;
if (have_osinfo) {



if (strcmp(osinfo.sysname, "FreeBSD") == 0) {
if (strncmp(osinfo.release, "4.3-", 4) == 0 ||
strncmp(osinfo.release, "4.4-", 4) == 0)
p->selectable_fd = -1;
}
}

p->read_op = pcap_read_bpf;
p->inject_op = pcap_inject_bpf;
p->setfilter_op = pcap_setfilter_bpf;
p->setdirection_op = pcap_setdirection_bpf;
p->set_datalink_op = pcap_set_datalink_bpf;
p->getnonblock_op = pcap_getnonblock_bpf;
p->setnonblock_op = pcap_setnonblock_bpf;
p->stats_op = pcap_stats_bpf;
p->cleanup_op = pcap_cleanup_bpf;

return (status);
bad:
pcap_cleanup_bpf(p);
return (status);
}








static int
check_bpf_bindable(const char *name)
{
int fd;
char errbuf[PCAP_ERRBUF_SIZE];

















#if defined(__APPLE__)
if (strncmp(name, "wlt", 3) == 0) {
char *en_name;
size_t en_name_len;





en_name_len = strlen(name) - 1;
en_name = malloc(en_name_len + 1);
if (en_name == NULL) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
return (-1);
}
strcpy(en_name, "en");
strcat(en_name, name + 3);
fd = bpf_open_and_bind(en_name, errbuf);
free(en_name);
} else
#endif
fd = bpf_open_and_bind(name, errbuf);
if (fd < 0) {



if (fd == PCAP_ERROR_NO_SUCH_DEVICE) {




return (0);
}










return (1);
}




close(fd);
return (1);
}

#if defined(__FreeBSD__) && defined(SIOCIFCREATE2)
static int
get_usb_if_flags(const char *name _U_, bpf_u_int32 *flags _U_, char *errbuf _U_)
{





return (0);
}

static int
finddevs_usb(pcap_if_list_t *devlistp, char *errbuf)
{
DIR *usbdir;
struct dirent *usbitem;
size_t name_max;
char *name;












usbdir = opendir("/dev/usb");
if (usbdir == NULL) {



return (0);
}






name_max = USBUS_PREFIX_LEN + 10 + 1;
name = malloc(name_max);
if (name == NULL) {
closedir(usbdir);
return (0);
}
while ((usbitem = readdir(usbdir)) != NULL) {
char *p;
size_t busnumlen;

if (strcmp(usbitem->d_name, ".") == 0 ||
strcmp(usbitem->d_name, "..") == 0) {



continue;
}
p = strchr(usbitem->d_name, '.');
if (p == NULL)
continue;
busnumlen = p - usbitem->d_name;
memcpy(name, usbus_prefix, USBUS_PREFIX_LEN);
memcpy(name + USBUS_PREFIX_LEN, usbitem->d_name, busnumlen);
*(name + USBUS_PREFIX_LEN + busnumlen) = '\0';







if (find_or_add_dev(devlistp, name, PCAP_IF_UP,
get_usb_if_flags, NULL, errbuf) == NULL) {
free(name);
closedir(usbdir);
return (PCAP_ERROR);
}
}
free(name);
closedir(usbdir);
return (0);
}
#endif




#if defined(SIOCGIFMEDIA)
static int
get_if_flags(const char *name, bpf_u_int32 *flags, char *errbuf)
{
int sock;
struct ifmediareq req;

sock = socket(AF_INET, SOCK_DGRAM, 0);
if (sock == -1) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE, errno,
"Can't create socket to get media information for %s",
name);
return (-1);
}
memset(&req, 0, sizeof(req));
strncpy(req.ifm_name, name, sizeof(req.ifm_name));
if (ioctl(sock, SIOCGIFMEDIA, &req) < 0) {
if (errno == EOPNOTSUPP || errno == EINVAL || errno == ENOTTY ||
errno == ENODEV || errno == EPERM) {















*flags |= PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE;
close(sock);
return (0);
}
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE, errno,
"SIOCGIFMEDIA on %s failed", name);
close(sock);
return (-1);
}
close(sock);




switch (IFM_TYPE(req.ifm_active)) {

case IFM_IEEE80211:



*flags |= PCAP_IF_WIRELESS;
break;
}




if (req.ifm_status & IFM_AVALID) {



if (req.ifm_status & IFM_ACTIVE) {



*flags |= PCAP_IF_CONNECTION_STATUS_CONNECTED;
} else {



*flags |= PCAP_IF_CONNECTION_STATUS_DISCONNECTED;
}
}
return (0);
}
#else
static int
get_if_flags(const char *name _U_, bpf_u_int32 *flags _U_, char *errbuf _U_)
{









if (*flags & PCAP_IF_LOOPBACK) {




*flags |= PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE;
return (0);
}
return (0);
}
#endif

int
pcap_platform_finddevs(pcap_if_list_t *devlistp, char *errbuf)
{



if (pcap_findalldevs_interfaces(devlistp, errbuf, check_bpf_bindable,
get_if_flags) == -1)
return (-1);

#if defined(__FreeBSD__) && defined(SIOCIFCREATE2)
if (finddevs_usb(devlistp, errbuf) == -1)
return (-1);
#endif

return (0);
}

#if defined(HAVE_BSD_IEEE80211)
static int
monitor_mode(pcap_t *p, int set)
{
struct pcap_bpf *pb = p->priv;
int sock;
struct ifmediareq req;
IFM_ULIST_TYPE *media_list;
int i;
int can_do;
struct ifreq ifr;

sock = socket(AF_INET, SOCK_DGRAM, 0);
if (sock == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "can't open socket");
return (PCAP_ERROR);
}

memset(&req, 0, sizeof req);
strncpy(req.ifm_name, p->opt.device, sizeof req.ifm_name);




if (ioctl(sock, SIOCGIFMEDIA, &req) < 0) {



switch (errno) {

case ENXIO:



close(sock);
return (PCAP_ERROR_NO_SUCH_DEVICE);

case EINVAL:



close(sock);
return (PCAP_ERROR_RFMON_NOTSUP);

default:
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFMEDIA");
close(sock);
return (PCAP_ERROR);
}
}
if (req.ifm_count == 0) {



close(sock);
return (PCAP_ERROR_RFMON_NOTSUP);
}





media_list = malloc(req.ifm_count * sizeof(*media_list));
if (media_list == NULL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "malloc");
close(sock);
return (PCAP_ERROR);
}
req.ifm_ulist = media_list;
if (ioctl(sock, SIOCGIFMEDIA, &req) < 0) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "SIOCGIFMEDIA");
free(media_list);
close(sock);
return (PCAP_ERROR);
}






can_do = 0;
for (i = 0; i < req.ifm_count; i++) {
if (IFM_TYPE(media_list[i]) == IFM_IEEE80211
&& IFM_SUBTYPE(media_list[i]) == IFM_AUTO) {

if (media_list[i] & IFM_IEEE80211_MONITOR) {
can_do = 1;
break;
}
}
}
free(media_list);
if (!can_do) {



close(sock);
return (PCAP_ERROR_RFMON_NOTSUP);
}

if (set) {




if ((req.ifm_current & IFM_IEEE80211_MONITOR) == 0) {










if (!pcap_do_addexit(p)) {




close(sock);
return (PCAP_ERROR);
}
memset(&ifr, 0, sizeof(ifr));
(void)strncpy(ifr.ifr_name, p->opt.device,
sizeof(ifr.ifr_name));
ifr.ifr_media = req.ifm_current | IFM_IEEE80211_MONITOR;
if (ioctl(sock, SIOCSIFMEDIA, &ifr) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf,
PCAP_ERRBUF_SIZE, errno, "SIOCSIFMEDIA");
close(sock);
return (PCAP_ERROR);
}

pb->must_do_on_close |= MUST_CLEAR_RFMON;




pcap_add_to_pcaps_to_close(p);
}
}
return (0);
}
#endif

#if defined(BIOCGDLTLIST) && (defined(__APPLE__) || defined(HAVE_BSD_IEEE80211))










static int
find_802_11(struct bpf_dltlist *bdlp)
{
int new_dlt;
u_int i;





new_dlt = -1;
for (i = 0; i < bdlp->bfl_len; i++) {
switch (bdlp->bfl_list[i]) {

case DLT_IEEE802_11:







if (new_dlt == -1)
new_dlt = bdlp->bfl_list[i];
break;

#if defined(DLT_PRISM_HEADER)
case DLT_PRISM_HEADER:
#endif
#if defined(DLT_AIRONET_HEADER)
case DLT_AIRONET_HEADER:
#endif
case DLT_IEEE802_11_RADIO_AVS:






if (new_dlt != DLT_IEEE802_11_RADIO)
new_dlt = bdlp->bfl_list[i];
break;

case DLT_IEEE802_11_RADIO:





new_dlt = bdlp->bfl_list[i];
break;

default:



break;
}
}

return (new_dlt);
}
#endif

#if defined(__APPLE__) && defined(BIOCGDLTLIST)




static void
remove_non_802_11(pcap_t *p)
{
int i, j;




j = 0;
for (i = 0; i < p->dlt_count; i++) {
switch (p->dlt_list[i]) {

case DLT_EN10MB:
case DLT_RAW:



continue;

default:



break;
}




p->dlt_list[j] = p->dlt_list[i];
j++;
}




p->dlt_count = j;
}






static void
remove_802_11(pcap_t *p)
{
int i, j;




j = 0;
for (i = 0; i < p->dlt_count; i++) {
switch (p->dlt_list[i]) {

case DLT_IEEE802_11:
#if defined(DLT_PRISM_HEADER)
case DLT_PRISM_HEADER:
#endif
#if defined(DLT_AIRONET_HEADER)
case DLT_AIRONET_HEADER:
#endif
case DLT_IEEE802_11_RADIO:
case DLT_IEEE802_11_RADIO_AVS:
#if defined(DLT_PPI)
case DLT_PPI:
#endif



continue;

default:



break;
}




p->dlt_list[j] = p->dlt_list[i];
j++;
}




p->dlt_count = j;
}
#endif

static int
pcap_setfilter_bpf(pcap_t *p, struct bpf_program *fp)
{
struct pcap_bpf *pb = p->priv;




pcap_freecode(&p->fcode);




if (ioctl(p->fd, BIOCSETF, (caddr_t)fp) == 0) {



pb->filtering_in_kernel = 1;








p->cc = 0;
return (0);
}















if (errno != EINVAL) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "BIOCSETF");
return (-1);
}






if (install_bpf_program(p, fp) < 0)
return (-1);
pb->filtering_in_kernel = 0;
return (0);
}





#if defined(BIOCSDIRECTION)
static int
pcap_setdirection_bpf(pcap_t *p, pcap_direction_t d)
{
u_int direction;

direction = (d == PCAP_D_IN) ? BPF_D_IN :
((d == PCAP_D_OUT) ? BPF_D_OUT : BPF_D_INOUT);
if (ioctl(p->fd, BIOCSDIRECTION, &direction) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "Cannot set direction to %s",
(d == PCAP_D_IN) ? "PCAP_D_IN" :
((d == PCAP_D_OUT) ? "PCAP_D_OUT" : "PCAP_D_INOUT"));
return (-1);
}
return (0);
}
#elif defined(BIOCSSEESENT)
static int
pcap_setdirection_bpf(pcap_t *p, pcap_direction_t d)
{
u_int seesent;




if (d == PCAP_D_OUT) {
pcap_snprintf(p->errbuf, sizeof(p->errbuf),
"Setting direction to PCAP_D_OUT is not supported on BPF");
return -1;
}

seesent = (d == PCAP_D_INOUT);
if (ioctl(p->fd, BIOCSSEESENT, &seesent) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "Cannot set direction to %s",
(d == PCAP_D_INOUT) ? "PCAP_D_INOUT" : "PCAP_D_IN");
return (-1);
}
return (0);
}
#else
static int
pcap_setdirection_bpf(pcap_t *p, pcap_direction_t d _U_)
{
(void) pcap_snprintf(p->errbuf, sizeof(p->errbuf),
"This system doesn't support BIOCSSEESENT, so the direction can't be set");
return (-1);
}
#endif

#if defined(BIOCSDLT)
static int
pcap_set_datalink_bpf(pcap_t *p, int dlt)
{
if (ioctl(p->fd, BIOCSDLT, &dlt) == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, sizeof(p->errbuf),
errno, "Cannot set DLT %d", dlt);
return (-1);
}
return (0);
}
#else
static int
pcap_set_datalink_bpf(pcap_t *p _U_, int dlt _U_)
{
return (0);
}
#endif




const char *
pcap_lib_version(void)
{
#if defined(HAVE_ZEROCOPY_BPF)
return (PCAP_VERSION_STRING " (with zerocopy support)");
#else
return (PCAP_VERSION_STRING);
#endif
}
