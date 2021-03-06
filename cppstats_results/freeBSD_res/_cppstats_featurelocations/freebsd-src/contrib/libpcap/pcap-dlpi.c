





































































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#if defined(HAVE_SYS_BUFMOD_H)
#include <sys/bufmod.h>
#endif
#include <sys/dlpi.h>
#if defined(HAVE_SYS_DLPI_EXT_H)
#include <sys/dlpi_ext.h>
#endif
#if defined(HAVE_HPUX9)
#include <sys/socket.h>
#endif
#if defined(DL_HP_PPA_REQ)
#include <sys/stat.h>
#endif
#include <sys/stream.h>
#if defined(HAVE_SOLARIS) && defined(HAVE_SYS_BUFMOD_H)
#include <sys/systeminfo.h>
#endif

#if defined(HAVE_HPUX9)
#include <net/if.h>
#endif

#include <ctype.h>
#if defined(HAVE_HPUX9)
#include <nlist.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>

#if defined(HAVE_LIMITS_H)
#include <limits.h>
#else
#define INT_MAX 2147483647
#endif

#include "pcap-int.h"
#include "dlpisubs.h"

#if defined(HAVE_OS_PROTO_H)
#include "os-proto.h"
#endif

#if defined(__hpux)




#define HAVE_DEV_DLPI
#elif defined(_AIX)




#define PCAP_DEV_PREFIX "/dev/dlpi"
#elif defined(HAVE_SOLARIS)



#define PCAP_DEV_PREFIX "/dev"
#endif

#define MAXDLBUF 8192


static char *split_dname(char *, u_int *, char *);
static int dl_doattach(int, int, char *);
#if defined(DL_HP_RAWDLS)
static int dl_dohpuxbind(int, char *);
#endif
static int dlpromiscon(pcap_t *, bpf_u_int32);
static int dlbindreq(int, bpf_u_int32, char *);
static int dlbindack(int, char *, char *, int *);
static int dlokack(int, const char *, char *, char *);
static int dlinforeq(int, char *);
static int dlinfoack(int, char *, char *);

#if defined(HAVE_DL_PASSIVE_REQ_T)
static void dlpassive(int, char *);
#endif

#if defined(DL_HP_RAWDLS)
static int dlrawdatareq(int, const u_char *, int);
#endif
static int recv_ack(int, int, const char *, char *, char *, int *);
static char *dlstrerror(char *, size_t, bpf_u_int32);
static char *dlprim(char *, size_t, bpf_u_int32);
#if defined(HAVE_SOLARIS) && defined(HAVE_SYS_BUFMOD_H)
#define GET_RELEASE_BUFSIZE 32
static void get_release(char *, size_t, bpf_u_int32 *, bpf_u_int32 *,
bpf_u_int32 *);
#endif
static int send_request(int, char *, int, char *, char *);
#if defined(HAVE_HPUX9)
static int dlpi_kread(int, off_t, void *, u_int, char *);
#endif
#if defined(HAVE_DEV_DLPI)
static int get_dlpi_ppa(int, const char *, u_int, u_int *, char *);
#endif





#define MAKE_DL_PRIMITIVES(ptr) ((union DL_primitives *)(void *)(ptr))

static int
pcap_read_dlpi(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
int cc;
u_char *bp;
int flags;
bpf_u_int32 ctlbuf[MAXDLBUF];
struct strbuf ctl = {
MAXDLBUF,
0,
(char *)ctlbuf
};
struct strbuf data;

flags = 0;
cc = p->cc;
if (cc == 0) {
data.buf = (char *)p->buffer + p->offset;
data.maxlen = p->bufsize;
data.len = 0;
do {



if (p->break_loop) {






p->break_loop = 0;
return (-2);
}





ctl.buf = (char *)ctlbuf;
ctl.maxlen = MAXDLBUF;
ctl.len = 0;
if (getmsg(p->fd, &ctl, &data, &flags) < 0) {

switch (errno) {

case EINTR:
cc = 0;
continue;

case EAGAIN:
return (0);
}
pcap_fmt_errmsg_for_errno(p->errbuf,
sizeof(p->errbuf), errno, "getmsg");
return (-1);
}
cc = data.len;
} while (cc == 0);
bp = (u_char *)p->buffer + p->offset;
} else
bp = p->bp;

return (pcap_process_pkts(p, callback, user, cnt, bp, cc));
}

static int
pcap_inject_dlpi(pcap_t *p, const void *buf, size_t size)
{
#if defined(DL_HP_RAWDLS)
struct pcap_dlpi *pd = p->priv;
#endif
int ret;

#if defined(DLIOCRAW)
ret = write(p->fd, buf, size);
if (ret == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "send");
return (-1);
}
#elif defined(DL_HP_RAWDLS)
if (pd->send_fd < 0) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"send: Output FD couldn't be opened");
return (-1);
}
ret = dlrawdatareq(pd->send_fd, buf, size);
if (ret == -1) {
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "send");
return (-1);
}







ret = size;
#else




















pcap_strlcpy(p->errbuf, "send: Not supported on this version of this OS",
PCAP_ERRBUF_SIZE);
ret = -1;
#endif
return (ret);
}

#if !defined(DL_IPATM)
#define DL_IPATM 0x12
#endif

#if defined(HAVE_SOLARIS)



#if !defined(A_GET_UNITS)
#define A_GET_UNITS (('A'<<8)|118)
#endif
#if !defined(A_PROMISCON_REQ)
#define A_PROMISCON_REQ (('A'<<8)|121)
#endif
#endif

static void
pcap_cleanup_dlpi(pcap_t *p)
{
#if defined(DL_HP_RAWDLS)
struct pcap_dlpi *pd = p->priv;

if (pd->send_fd >= 0) {
close(pd->send_fd);
pd->send_fd = -1;
}
#endif
pcap_cleanup_live_common(p);
}

static int
open_dlpi_device(const char *name, u_int *ppa, char *errbuf)
{
int status;
char dname[100];
char *cp;
int fd;
#if defined(HAVE_DEV_DLPI)
u_int unit;
#else
char dname2[100];
#endif

#if defined(HAVE_DEV_DLPI)



cp = strrchr(name, '/');
if (cp == NULL)
pcap_strlcpy(dname, name, sizeof(dname));
else
pcap_strlcpy(dname, cp + 1, sizeof(dname));





cp = split_dname(dname, &unit, errbuf);
if (cp == NULL)
return (PCAP_ERROR_NO_SUCH_DEVICE);
*cp = '\0';












cp = "/dev/dlpi";
if ((fd = open(cp, O_RDWR)) < 0) {
if (errno == EPERM || errno == EACCES)
status = PCAP_ERROR_PERM_DENIED;
else
status = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "%s", cp);
return (status);
}





status = get_dlpi_ppa(fd, dname, unit, ppa, errbuf);
if (status < 0) {
close(fd);
return (status);
}
#else






if (*name == '/')
pcap_strlcpy(dname, name, sizeof(dname));
else
pcap_snprintf(dname, sizeof(dname), "%s/%s", PCAP_DEV_PREFIX,
name);





cp = split_dname(dname, ppa, errbuf);
if (cp == NULL)
return (PCAP_ERROR_NO_SUCH_DEVICE);





pcap_strlcpy(dname2, dname, sizeof(dname));
*cp = '\0';


if ((fd = open(dname, O_RDWR)) < 0) {
if (errno != ENOENT) {
if (errno == EPERM || errno == EACCES)
status = PCAP_ERROR_PERM_DENIED;
else
status = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "%s", dname);
return (status);
}


if ((fd = open(dname2, O_RDWR)) < 0) {
if (errno == ENOENT) {
status = PCAP_ERROR_NO_SUCH_DEVICE;
























pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
"%s: No DLPI device found", name);
} else {
if (errno == EPERM || errno == EACCES)
status = PCAP_ERROR_PERM_DENIED;
else
status = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(errbuf,
PCAP_ERRBUF_SIZE, errno, "%s", dname2);
}
return (status);
}

*ppa = 0;
}
#endif
return (fd);
}

static int
pcap_activate_dlpi(pcap_t *p)
{
#if defined(DL_HP_RAWDLS)
struct pcap_dlpi *pd = p->priv;
#endif
int status = 0;
int retv;
u_int ppa;
#if defined(HAVE_SOLARIS)
int isatm = 0;
#endif
register dl_info_ack_t *infop;
#if defined(HAVE_SYS_BUFMOD_H)
bpf_u_int32 ss;
#if defined(HAVE_SOLARIS)
char release[GET_RELEASE_BUFSIZE];
bpf_u_int32 osmajor, osminor, osmicro;
#endif
#endif
bpf_u_int32 buf[MAXDLBUF];

p->fd = open_dlpi_device(p->opt.device, &ppa, p->errbuf);
if (p->fd < 0) {
status = p->fd;
goto bad;
}

#if defined(DL_HP_RAWDLS)











pd->send_fd = open("/dev/dlpi", O_RDWR);
#endif




if (dlinforeq(p->fd, p->errbuf) < 0 ||
dlinfoack(p->fd, (char *)buf, p->errbuf) < 0) {
status = PCAP_ERROR;
goto bad;
}
infop = &(MAKE_DL_PRIMITIVES(buf))->info_ack;
#if defined(HAVE_SOLARIS)
if (infop->dl_mac_type == DL_IPATM)
isatm = 1;
#endif
if (infop->dl_provider_style == DL_STYLE2) {
retv = dl_doattach(p->fd, ppa, p->errbuf);
if (retv < 0) {
status = retv;
goto bad;
}
#if defined(DL_HP_RAWDLS)
if (pd->send_fd >= 0) {
retv = dl_doattach(pd->send_fd, ppa, p->errbuf);
if (retv < 0) {
status = retv;
goto bad;
}
}
#endif
}

if (p->opt.rfmon) {




status = PCAP_ERROR_RFMON_NOTSUP;
goto bad;
}

#if defined(HAVE_DL_PASSIVE_REQ_T)




dlpassive(p->fd, p->errbuf);
#endif




#if !defined(HAVE_HPUX9) && !defined(HAVE_HPUX10_20_OR_LATER) && !defined(sinix)
#if defined(_AIX)


















if ((dlbindreq(p->fd, 1537, p->errbuf) < 0 &&
dlbindreq(p->fd, 2, p->errbuf) < 0) ||
dlbindack(p->fd, (char *)buf, p->errbuf, NULL) < 0) {
status = PCAP_ERROR;
goto bad;
}
#elif defined(DL_HP_RAWDLS)



if (dl_dohpuxbind(p->fd, p->errbuf) < 0) {
status = PCAP_ERROR;
goto bad;
}
if (pd->send_fd >= 0) {





if (dl_dohpuxbind(pd->send_fd, p->errbuf) < 0) {
status = PCAP_ERROR;
goto bad;
}
}
#else




if (dlbindreq(p->fd, 0, p->errbuf) < 0 ||
dlbindack(p->fd, (char *)buf, p->errbuf, NULL) < 0) {
status = PCAP_ERROR;
goto bad;
}
#endif
#endif









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;

#if defined(HAVE_SOLARIS)
if (isatm) {






if (strioctl(p->fd, A_PROMISCON_REQ, 0, NULL) < 0) {
status = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "A_PROMISCON_REQ");
goto bad;
}
} else
#endif
if (p->opt.promisc) {



retv = dlpromiscon(p, DL_PROMISC_PHYS);
if (retv < 0) {
if (retv == PCAP_ERROR_PERM_DENIED)
status = PCAP_ERROR_PROMISC_PERM_DENIED;
else
status = retv;
goto bad;
}






#if !defined(__hpux) && !defined(sinix)
retv = dlpromiscon(p, DL_PROMISC_MULTI);
if (retv < 0)
status = PCAP_WARNING;
#endif
}





#if !defined(sinix)
#if defined(__hpux)

if (!p->opt.promisc) {
#elif defined(HAVE_SOLARIS)

if (!isatm) {
#else

{
#endif
retv = dlpromiscon(p, DL_PROMISC_SAP);
if (retv < 0) {
if (p->opt.promisc) {






status = PCAP_WARNING;
} else {



status = retv;
goto bad;
}
}
}
#endif





#if defined(HAVE_HPUX9) || defined(HAVE_HPUX10_20_OR_LATER)
if (dl_dohpuxbind(p->fd, p->errbuf) < 0) {
status = PCAP_ERROR;
goto bad;
}





if (pd->send_fd >= 0) {





if (dl_dohpuxbind(pd->send_fd, p->errbuf) < 0) {
status = PCAP_ERROR;
goto bad;
}
}
#endif






if (dlinforeq(p->fd, p->errbuf) < 0 ||
dlinfoack(p->fd, (char *)buf, p->errbuf) < 0) {
status = PCAP_ERROR;
goto bad;
}

infop = &(MAKE_DL_PRIMITIVES(buf))->info_ack;
if (pcap_process_mactype(p, infop->dl_mac_type) != 0) {
status = PCAP_ERROR;
goto bad;
}

#if defined(DLIOCRAW)




if (strioctl(p->fd, DLIOCRAW, 0, NULL) < 0) {
status = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "DLIOCRAW");
goto bad;
}
#endif

#if defined(HAVE_SYS_BUFMOD_H)
ss = p->snapshot;









#if defined(HAVE_SOLARIS)
get_release(release, sizeof (release), &osmajor, &osminor, &osmicro);
if (osmajor == 5 && (osminor <= 2 || (osminor == 3 && osmicro < 2)) &&
getenv("BUFMOD_FIXED") == NULL) {
pcap_snprintf(p->errbuf, PCAP_ERRBUF_SIZE,
"WARNING: bufmod is broken in SunOS %s; ignoring snaplen.",
release);
ss = 0;
status = PCAP_WARNING;
}
#endif


if (pcap_conf_bufmod(p, ss) != 0) {
status = PCAP_ERROR;
goto bad;
}
#endif




if (ioctl(p->fd, I_FLUSH, FLUSHR) != 0) {
status = PCAP_ERROR;
pcap_fmt_errmsg_for_errno(p->errbuf, PCAP_ERRBUF_SIZE,
errno, "FLUSHR");
goto bad;
}


if (pcap_alloc_databuf(p) != 0) {
status = PCAP_ERROR;
goto bad;
}







p->selectable_fd = p->fd;

p->read_op = pcap_read_dlpi;
p->inject_op = pcap_inject_dlpi;
p->setfilter_op = install_bpf_program;
p->setdirection_op = NULL;
p->set_datalink_op = NULL;
p->getnonblock_op = pcap_getnonblock_fd;
p->setnonblock_op = pcap_setnonblock_fd;
p->stats_op = pcap_stats_dlpi;
p->cleanup_op = pcap_cleanup_dlpi;

return (status);
bad:
pcap_cleanup_dlpi(p);
return (status);
}









static char *
split_dname(char *device, u_int *unitp, char *ebuf)
{
char *cp;
char *eos;
long unit;




cp = device + strlen(device) - 1;
if (*cp < '0' || *cp > '9') {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE, "%s missing unit number",
device);
return (NULL);
}


while (cp-1 >= device && *(cp-1) >= '0' && *(cp-1) <= '9')
cp--;

errno = 0;
unit = strtol(cp, &eos, 10);
if (*eos != '\0') {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE, "%s bad unit number", device);
return (NULL);
}
if (errno == ERANGE || unit > INT_MAX) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE, "%s unit number too large",
device);
return (NULL);
}
if (unit < 0) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE, "%s unit number is negative",
device);
return (NULL);
}
*unitp = (u_int)unit;
return (cp);
}

static int
dl_doattach(int fd, int ppa, char *ebuf)
{
dl_attach_req_t req;
bpf_u_int32 buf[MAXDLBUF];
int err;

req.dl_primitive = DL_ATTACH_REQ;
req.dl_ppa = ppa;
if (send_request(fd, (char *)&req, sizeof(req), "attach", ebuf) < 0)
return (PCAP_ERROR);

err = dlokack(fd, "attach", (char *)buf, ebuf);
if (err < 0)
return (err);
return (0);
}

#if defined(DL_HP_RAWDLS)
static int
dl_dohpuxbind(int fd, char *ebuf)
{
int hpsap;
int uerror;
bpf_u_int32 buf[MAXDLBUF];








hpsap = 22;
for (;;) {
if (dlbindreq(fd, hpsap, ebuf) < 0)
return (-1);
if (dlbindack(fd, (char *)buf, ebuf, &uerror) >= 0)
break;



if (uerror != EBUSY) {




return (-1);
}







*ebuf = '\0';
hpsap++;
if (hpsap > 100) {
pcap_strlcpy(ebuf,
"All SAPs from 22 through 100 are in use",
PCAP_ERRBUF_SIZE);
return (-1);
}
}
return (0);
}
#endif

#define STRINGIFY(n) #n

static int
dlpromiscon(pcap_t *p, bpf_u_int32 level)
{
dl_promiscon_req_t req;
bpf_u_int32 buf[MAXDLBUF];
int err;

req.dl_primitive = DL_PROMISCON_REQ;
req.dl_level = level;
if (send_request(p->fd, (char *)&req, sizeof(req), "promiscon",
p->errbuf) < 0)
return (PCAP_ERROR);
err = dlokack(p->fd, "promiscon" STRINGIFY(level), (char *)buf,
p->errbuf);
if (err < 0)
return (err);
return (0);
}








static int
is_dlpi_interface(const char *name)
{
int fd;
u_int ppa;
char errbuf[PCAP_ERRBUF_SIZE];

fd = open_dlpi_device(name, &ppa, errbuf);
if (fd < 0) {



if (fd == PCAP_ERROR_NO_SUCH_DEVICE) {




return (0);
}














return (1);
}




close(fd);
return (1);
}

static int
get_if_flags(const char *name _U_, bpf_u_int32 *flags _U_, char *errbuf _U_)
{









if (*flags & PCAP_IF_LOOPBACK) {




*flags |= PCAP_IF_CONNECTION_STATUS_NOT_APPLICABLE;
return (0);
}
return (0);
}

int
pcap_platform_finddevs(pcap_if_list_t *devlistp, char *errbuf)
{
#if defined(HAVE_SOLARIS)
int fd;
union {
u_int nunits;
char pad[516];

} buf;
char baname[2+1+1];
u_int i;
#endif




if (pcap_findalldevs_interfaces(devlistp, errbuf, is_dlpi_interface,
get_if_flags) == -1)
return (-1);

#if defined(HAVE_SOLARIS)



if ((fd = open("/dev/ba", O_RDWR)) < 0) {








return (0);
}

if (strioctl(fd, A_GET_UNITS, sizeof(buf), (char *)&buf) < 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
errno, "A_GET_UNITS");
return (-1);
}
for (i = 0; i < buf.nunits; i++) {
pcap_snprintf(baname, sizeof baname, "ba%u", i);





if (add_dev(devlistp, baname, 0, NULL, errbuf) == NULL)
return (-1);
}
#endif

return (0);
}

static int
send_request(int fd, char *ptr, int len, char *what, char *ebuf)
{
struct strbuf ctl;
int flags;

ctl.maxlen = 0;
ctl.len = len;
ctl.buf = ptr;

flags = 0;
if (putmsg(fd, &ctl, (struct strbuf *) NULL, flags) < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "send_request: putmsg \"%s\"", what);
return (-1);
}
return (0);
}

static int
recv_ack(int fd, int size, const char *what, char *bufp, char *ebuf, int *uerror)
{
union DL_primitives *dlp;
struct strbuf ctl;
int flags;
char errmsgbuf[PCAP_ERRBUF_SIZE];
char dlprimbuf[64];





if (uerror != NULL)
*uerror = 0;

ctl.maxlen = MAXDLBUF;
ctl.len = 0;
ctl.buf = bufp;

flags = 0;
if (getmsg(fd, &ctl, (struct strbuf*)NULL, &flags) < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "recv_ack: %s getmsg", what);
return (PCAP_ERROR);
}

dlp = MAKE_DL_PRIMITIVES(ctl.buf);
switch (dlp->dl_primitive) {

case DL_INFO_ACK:
case DL_BIND_ACK:
case DL_OK_ACK:
#if defined(DL_HP_PPA_ACK)
case DL_HP_PPA_ACK:
#endif

break;

case DL_ERROR_ACK:
switch (dlp->error_ack.dl_errno) {

case DL_SYSERR:
if (uerror != NULL)
*uerror = dlp->error_ack.dl_unix_errno;
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
dlp->error_ack.dl_unix_errno,
"recv_ack: %s: UNIX error", what);
if (dlp->error_ack.dl_unix_errno == EPERM ||
dlp->error_ack.dl_unix_errno == EACCES)
return (PCAP_ERROR_PERM_DENIED);
break;

default:
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"recv_ack: %s: %s", what,
dlstrerror(errmsgbuf, sizeof (errmsgbuf), dlp->error_ack.dl_errno));
if (dlp->error_ack.dl_errno == DL_BADPPA)
return (PCAP_ERROR_NO_SUCH_DEVICE);
else if (dlp->error_ack.dl_errno == DL_ACCESS)
return (PCAP_ERROR_PERM_DENIED);
break;
}
return (PCAP_ERROR);

default:
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"recv_ack: %s: Unexpected primitive ack %s",
what, dlprim(dlprimbuf, sizeof (dlprimbuf), dlp->dl_primitive));
return (PCAP_ERROR);
}

if (ctl.len < size) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"recv_ack: %s: Ack too small (%d < %d)",
what, ctl.len, size);
return (PCAP_ERROR);
}
return (ctl.len);
}

static char *
dlstrerror(char *errbuf, size_t errbufsize, bpf_u_int32 dl_errno)
{
switch (dl_errno) {

case DL_ACCESS:
return ("Improper permissions for request");

case DL_BADADDR:
return ("DLSAP addr in improper format or invalid");

case DL_BADCORR:
return ("Seq number not from outstand DL_CONN_IND");

case DL_BADDATA:
return ("User data exceeded provider limit");

case DL_BADPPA:
#if defined(HAVE_DEV_DLPI)





return ("Specified PPA was invalid");
#else




return ("Specified PPA (device unit) was invalid");
#endif

case DL_BADPRIM:
return ("Primitive received not known by provider");

case DL_BADQOSPARAM:
return ("QOS parameters contained invalid values");

case DL_BADQOSTYPE:
return ("QOS structure type is unknown/unsupported");

case DL_BADSAP:
return ("Bad LSAP selector");

case DL_BADTOKEN:
return ("Token used not an active stream");

case DL_BOUND:
return ("Attempted second bind with dl_max_conind");

case DL_INITFAILED:
return ("Physical link initialization failed");

case DL_NOADDR:
return ("Provider couldn't allocate alternate address");

case DL_NOTINIT:
return ("Physical link not initialized");

case DL_OUTSTATE:
return ("Primitive issued in improper state");

case DL_SYSERR:
return ("UNIX system error occurred");

case DL_UNSUPPORTED:
return ("Requested service not supplied by provider");

case DL_UNDELIVERABLE:
return ("Previous data unit could not be delivered");

case DL_NOTSUPPORTED:
return ("Primitive is known but not supported");

case DL_TOOMANY:
return ("Limit exceeded");

case DL_NOTENAB:
return ("Promiscuous mode not enabled");

case DL_BUSY:
return ("Other streams for PPA in post-attached");

case DL_NOAUTO:
return ("Automatic handling XID&TEST not supported");

case DL_NOXIDAUTO:
return ("Automatic handling of XID not supported");

case DL_NOTESTAUTO:
return ("Automatic handling of TEST not supported");

case DL_XIDAUTO:
return ("Automatic handling of XID response");

case DL_TESTAUTO:
return ("Automatic handling of TEST response");

case DL_PENDING:
return ("Pending outstanding connect indications");

default:
pcap_snprintf(errbuf, errbufsize, "Error %02x", dl_errno);
return (errbuf);
}
}

static char *
dlprim(char *primbuf, size_t primbufsize, bpf_u_int32 prim)
{
switch (prim) {

case DL_INFO_REQ:
return ("DL_INFO_REQ");

case DL_INFO_ACK:
return ("DL_INFO_ACK");

case DL_ATTACH_REQ:
return ("DL_ATTACH_REQ");

case DL_DETACH_REQ:
return ("DL_DETACH_REQ");

case DL_BIND_REQ:
return ("DL_BIND_REQ");

case DL_BIND_ACK:
return ("DL_BIND_ACK");

case DL_UNBIND_REQ:
return ("DL_UNBIND_REQ");

case DL_OK_ACK:
return ("DL_OK_ACK");

case DL_ERROR_ACK:
return ("DL_ERROR_ACK");

case DL_SUBS_BIND_REQ:
return ("DL_SUBS_BIND_REQ");

case DL_SUBS_BIND_ACK:
return ("DL_SUBS_BIND_ACK");

case DL_UNITDATA_REQ:
return ("DL_UNITDATA_REQ");

case DL_UNITDATA_IND:
return ("DL_UNITDATA_IND");

case DL_UDERROR_IND:
return ("DL_UDERROR_IND");

case DL_UDQOS_REQ:
return ("DL_UDQOS_REQ");

case DL_CONNECT_REQ:
return ("DL_CONNECT_REQ");

case DL_CONNECT_IND:
return ("DL_CONNECT_IND");

case DL_CONNECT_RES:
return ("DL_CONNECT_RES");

case DL_CONNECT_CON:
return ("DL_CONNECT_CON");

case DL_TOKEN_REQ:
return ("DL_TOKEN_REQ");

case DL_TOKEN_ACK:
return ("DL_TOKEN_ACK");

case DL_DISCONNECT_REQ:
return ("DL_DISCONNECT_REQ");

case DL_DISCONNECT_IND:
return ("DL_DISCONNECT_IND");

case DL_RESET_REQ:
return ("DL_RESET_REQ");

case DL_RESET_IND:
return ("DL_RESET_IND");

case DL_RESET_RES:
return ("DL_RESET_RES");

case DL_RESET_CON:
return ("DL_RESET_CON");

default:
pcap_snprintf(primbuf, primbufsize, "unknown primitive 0x%x",
prim);
return (primbuf);
}
}

static int
dlbindreq(int fd, bpf_u_int32 sap, char *ebuf)
{

dl_bind_req_t req;

memset((char *)&req, 0, sizeof(req));
req.dl_primitive = DL_BIND_REQ;

#if defined(DL_HP_RAWDLS)
req.dl_max_conind = 1;
req.dl_service_mode = DL_HP_RAWDLS;
#elif defined(DL_CLDLS)
req.dl_service_mode = DL_CLDLS;
#endif
req.dl_sap = sap;

return (send_request(fd, (char *)&req, sizeof(req), "bind", ebuf));
}

static int
dlbindack(int fd, char *bufp, char *ebuf, int *uerror)
{

return (recv_ack(fd, DL_BIND_ACK_SIZE, "bind", bufp, ebuf, uerror));
}

static int
dlokack(int fd, const char *what, char *bufp, char *ebuf)
{

return (recv_ack(fd, DL_OK_ACK_SIZE, what, bufp, ebuf, NULL));
}


static int
dlinforeq(int fd, char *ebuf)
{
dl_info_req_t req;

req.dl_primitive = DL_INFO_REQ;

return (send_request(fd, (char *)&req, sizeof(req), "info", ebuf));
}

static int
dlinfoack(int fd, char *bufp, char *ebuf)
{

return (recv_ack(fd, DL_INFO_ACK_SIZE, "info", bufp, ebuf, NULL));
}

#if defined(HAVE_DL_PASSIVE_REQ_T)




static void
dlpassive(int fd, char *ebuf)
{
dl_passive_req_t req;
bpf_u_int32 buf[MAXDLBUF];

req.dl_primitive = DL_PASSIVE_REQ;

if (send_request(fd, (char *)&req, sizeof(req), "dlpassive", ebuf) == 0)
(void) dlokack(fd, "dlpassive", (char *)buf, ebuf);
}
#endif

#if defined(DL_HP_RAWDLS)



static int
dlrawdatareq(int fd, const u_char *datap, int datalen)
{
struct strbuf ctl, data;
long buf[MAXDLBUF];
union DL_primitives *dlp;
int dlen;

dlp = MAKE_DL_PRIMITIVES(buf);

dlp->dl_primitive = DL_HP_RAWDATA_REQ;
dlen = DL_HP_RAWDATA_REQ_SIZE;








ctl.maxlen = 0;
ctl.len = dlen;
ctl.buf = (void *)buf;

data.maxlen = 0;
data.len = datalen;
data.buf = (void *)datap;

return (putmsg(fd, &ctl, &data, 0));
}
#endif

#if defined(HAVE_SOLARIS) && defined(HAVE_SYS_BUFMOD_H)
static void
get_release(char *buf, size_t bufsize, bpf_u_int32 *majorp,
bpf_u_int32 *minorp, bpf_u_int32 *microp)
{
char *cp;

*majorp = 0;
*minorp = 0;
*microp = 0;
if (sysinfo(SI_RELEASE, buf, bufsize) < 0) {
pcap_strlcpy(buf, "?", bufsize);
return;
}
cp = buf;
if (!isdigit((unsigned char)*cp))
return;
*majorp = strtol(cp, &cp, 10);
if (*cp++ != '.')
return;
*minorp = strtol(cp, &cp, 10);
if (*cp++ != '.')
return;
*microp = strtol(cp, &cp, 10);
}
#endif

#if defined(DL_HP_PPA_REQ)







































static int
get_dlpi_ppa(register int fd, register const char *device, register u_int unit,
u_int *ppa, register char *ebuf)
{
register dl_hp_ppa_ack_t *ap;
register dl_hp_ppa_info_t *ipstart, *ip;
register u_int i;
char dname[100];
register u_long majdev;
struct stat statbuf;
dl_hp_ppa_req_t req;
char buf[MAXDLBUF];
char *ppa_data_buf;
dl_hp_ppa_ack_t *dlp;
struct strbuf ctl;
int flags;

memset((char *)&req, 0, sizeof(req));
req.dl_primitive = DL_HP_PPA_REQ;

memset((char *)buf, 0, sizeof(buf));
if (send_request(fd, (char *)&req, sizeof(req), "hpppa", ebuf) < 0)
return (PCAP_ERROR);

ctl.maxlen = DL_HP_PPA_ACK_SIZE;
ctl.len = 0;
ctl.buf = (char *)buf;

flags = 0;













if (getmsg(fd, &ctl, (struct strbuf *)NULL, &flags) < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "get_dlpi_ppa: hpppa getmsg");
return (PCAP_ERROR);
}
if (ctl.len == -1) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"get_dlpi_ppa: hpppa getmsg: control buffer has no data");
return (PCAP_ERROR);
}

dlp = (dl_hp_ppa_ack_t *)ctl.buf;
if (dlp->dl_primitive != DL_HP_PPA_ACK) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"get_dlpi_ppa: hpppa unexpected primitive ack 0x%x",
(bpf_u_int32)dlp->dl_primitive);
return (PCAP_ERROR);
}

if ((size_t)ctl.len < DL_HP_PPA_ACK_SIZE) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"get_dlpi_ppa: hpppa ack too small (%d < %lu)",
ctl.len, (unsigned long)DL_HP_PPA_ACK_SIZE);
return (PCAP_ERROR);
}


if ((ppa_data_buf = (char *)malloc(dlp->dl_length)) == NULL) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "get_dlpi_ppa: hpppa malloc");
return (PCAP_ERROR);
}
ctl.maxlen = dlp->dl_length;
ctl.len = 0;
ctl.buf = (char *)ppa_data_buf;

if (getmsg(fd, &ctl, (struct strbuf *)NULL, &flags) < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "get_dlpi_ppa: hpppa getmsg");
free(ppa_data_buf);
return (PCAP_ERROR);
}
if (ctl.len == -1) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"get_dlpi_ppa: hpppa getmsg: control buffer has no data");
return (PCAP_ERROR);
}
if ((u_int)ctl.len < dlp->dl_length) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"get_dlpi_ppa: hpppa ack too small (%d < %lu)",
ctl.len, (unsigned long)dlp->dl_length);
free(ppa_data_buf);
return (PCAP_ERROR);
}

ap = (dl_hp_ppa_ack_t *)buf;
ipstart = (dl_hp_ppa_info_t *)ppa_data_buf;
ip = ipstart;

#if defined(HAVE_DL_HP_PPA_INFO_T_DL_MODULE_ID_1)













for (i = 0; i < ap->dl_count; i++) {
if ((strcmp((const char *)ip->dl_module_id_1, device) == 0 ||
strcmp((const char *)ip->dl_module_id_2, device) == 0) &&
ip->dl_instance_num == unit)
break;

ip = (dl_hp_ppa_info_t *)((u_char *)ipstart + ip->dl_next_offset);
}
#else




i = ap->dl_count;
#endif

if (i == ap->dl_count) {















pcap_snprintf(dname, sizeof(dname), "/dev/%s%u", device, unit);
if (stat(dname, &statbuf) < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "stat: %s", dname);
return (PCAP_ERROR);
}
majdev = major(statbuf.st_rdev);

ip = ipstart;

for (i = 0; i < ap->dl_count; i++) {
if (ip->dl_mjr_num == majdev &&
ip->dl_instance_num == unit)
break;

ip = (dl_hp_ppa_info_t *)((u_char *)ipstart + ip->dl_next_offset);
}
}
if (i == ap->dl_count) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"can't find /dev/dlpi PPA for %s%u", device, unit);
return (PCAP_ERROR_NO_SUCH_DEVICE);
}
if (ip->dl_hdw_state == HDW_DEAD) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"%s%d: hardware state: DOWN\n", device, unit);
free(ppa_data_buf);
return (PCAP_ERROR);
}
*ppa = ip->dl_ppa;
free(ppa_data_buf);
return (0);
}
#endif

#if defined(HAVE_HPUX9)




static struct nlist nl[] = {
#define NL_IFNET 0
{ "ifnet" },
{ "" }
};

static char path_vmunix[] = "/hp-ux";


static int
get_dlpi_ppa(register int fd, register const char *ifname, register u_int unit,
u_int *ppa, register char *ebuf)
{
register const char *cp;
register int kd;
void *addr;
struct ifnet ifnet;
char if_name[sizeof(ifnet.if_name) + 1];

cp = strrchr(ifname, '/');
if (cp != NULL)
ifname = cp + 1;
if (nlist(path_vmunix, &nl) < 0) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE, "nlist %s failed",
path_vmunix);
return (PCAP_ERROR);
}
if (nl[NL_IFNET].n_value == 0) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE,
"could't find %s kernel symbol",
nl[NL_IFNET].n_name);
return (PCAP_ERROR);
}
kd = open("/dev/kmem", O_RDONLY);
if (kd < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "kmem open");
return (PCAP_ERROR);
}
if (dlpi_kread(kd, nl[NL_IFNET].n_value,
&addr, sizeof(addr), ebuf) < 0) {
close(kd);
return (PCAP_ERROR);
}
for (; addr != NULL; addr = ifnet.if_next) {
if (dlpi_kread(kd, (off_t)addr,
&ifnet, sizeof(ifnet), ebuf) < 0 ||
dlpi_kread(kd, (off_t)ifnet.if_name,
if_name, sizeof(ifnet.if_name), ebuf) < 0) {
(void)close(kd);
return (PCAP_ERROR);
}
if_name[sizeof(ifnet.if_name)] = '\0';
if (strcmp(if_name, ifname) == 0 && ifnet.if_unit == unit) {
*ppa = ifnet.if_index;
return (0);
}
}

pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE, "Can't find %s", ifname);
return (PCAP_ERROR_NO_SUCH_DEVICE);
}

static int
dlpi_kread(register int fd, register off_t addr,
register void *buf, register u_int len, register char *ebuf)
{
register int cc;

if (lseek(fd, addr, SEEK_SET) < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "lseek");
return (-1);
}
cc = read(fd, buf, len);
if (cc < 0) {
pcap_fmt_errmsg_for_errno(ebuf, PCAP_ERRBUF_SIZE,
errno, "read");
return (-1);
} else if (cc != len) {
pcap_snprintf(ebuf, PCAP_ERRBUF_SIZE, "short read (%d != %d)", cc,
len);
return (-1);
}
return (cc);
}
#endif

pcap_t *
pcap_create_interface(const char *device _U_, char *ebuf)
{
pcap_t *p;
#if defined(DL_HP_RAWDLS)
struct pcap_dlpi *pd;
#endif

p = pcap_create_common(ebuf, sizeof (struct pcap_dlpi));
if (p == NULL)
return (NULL);

#if defined(DL_HP_RAWDLS)
pd = p->priv;
pd->send_fd = -1;
#endif

p->activate_op = pcap_activate_dlpi;
return (p);
}




const char *
pcap_lib_version(void)
{
return (PCAP_VERSION_STRING);
}
