

























#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/bufmod.h>
#include <sys/stream.h>
#include <libdlpi.h>
#include <errno.h>
#include <memory.h>
#include <stropts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcap-int.h"
#include "dlpisubs.h"


static int dlpromiscon(pcap_t *, bpf_u_int32);
static int pcap_read_libdlpi(pcap_t *, int, pcap_handler, u_char *);
static int pcap_inject_libdlpi(pcap_t *, const void *, size_t);
static void pcap_libdlpi_err(const char *, const char *, int, char *);
static void pcap_cleanup_libdlpi(pcap_t *);





static boolean_t list_interfaces(const char *, void *);

typedef struct linknamelist {
char linkname[DLPI_LINKNAME_MAX];
struct linknamelist *lnl_next;
} linknamelist_t;

typedef struct linkwalk {
linknamelist_t *lw_list;
int lw_err;
} linkwalk_t;





static boolean_t
list_interfaces(const char *linkname, void *arg)
{
linkwalk_t *lwp = arg;
linknamelist_t *entry;

if ((entry = calloc(1, sizeof(linknamelist_t))) == NULL) {
lwp->lw_err = ENOMEM;
return (B_TRUE);
}
(void) pcap_strlcpy(entry->linkname, linkname, DLPI_LINKNAME_MAX);

if (lwp->lw_list == NULL) {
lwp->lw_list = entry;
} else {
entry->lnl_next = lwp->lw_list;
lwp->lw_list = entry;
}

return (B_FALSE);
}

static int
pcap_activate_libdlpi(pcap_t *p)
{
struct pcap_dlpi *pd = p->priv;
int status = 0;
int retv;
dlpi_handle_t dh;
dlpi_info_t dlinfo;






retv = dlpi_open(p->opt.device, &dh, DLPI_RAW|DLPI_PASSIVE);
if (retv != DLPI_SUCCESS) {
if (retv == DLPI_ELINKNAMEINVAL || retv == DLPI_ENOLINK)
status = PCAP_ERROR_NO_SUCH_DEVICE;
else if (retv == DL_SYSERR &&
(errno == EPERM || errno == EACCES))
status = PCAP_ERROR_PERM_DENIED;
else
status = PCAP_ERROR;
pcap_libdlpi_err(p->opt.device, "dlpi_open", retv,
p->errbuf);
return (status);
}
pd->dlpi_hd = dh;

if (p->opt.rfmon) {




status = PCAP_ERROR_RFMON_NOTSUP;
goto bad;
}


if ((retv = dlpi_bind(pd->dlpi_hd, DLPI_ANY_SAP, 0)) != DLPI_SUCCESS) {
status = PCAP_ERROR;
pcap_libdlpi_err(p->opt.device, "dlpi_bind", retv, p->errbuf);
goto bad;
}









if (p->snapshot <= 0 || p->snapshot > MAXIMUM_SNAPLEN)
p->snapshot = MAXIMUM_SNAPLEN;


if (p->opt.promisc) {
retv = dlpromiscon(p, DL_PROMISC_PHYS);
if (retv < 0) {













if (retv == PCAP_ERROR_PERM_DENIED)
status = PCAP_ERROR_PROMISC_PERM_DENIED;
else
status = retv;
goto bad;
}
} else {

retv = dlpromiscon(p, DL_PROMISC_MULTI);
if (retv < 0) {
status = retv;
goto bad;
}
}


retv = dlpromiscon(p, DL_PROMISC_SAP);
if (retv < 0) {




if (p->opt.promisc)
status = PCAP_WARNING;
else {
status = retv;
goto bad;
}
}


if ((retv = dlpi_info(pd->dlpi_hd, &dlinfo, 0)) != DLPI_SUCCESS) {
status = PCAP_ERROR;
pcap_libdlpi_err(p->opt.device, "dlpi_info", retv, p->errbuf);
goto bad;
}

if (pcap_process_mactype(p, dlinfo.di_mactype) != 0) {
status = PCAP_ERROR;
goto bad;
}

p->fd = dlpi_fd(pd->dlpi_hd);


if (pcap_conf_bufmod(p, p->snapshot) != 0) {
status = PCAP_ERROR;
goto bad;
}




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

p->read_op = pcap_read_libdlpi;
p->inject_op = pcap_inject_libdlpi;
p->setfilter_op = install_bpf_program;
p->setdirection_op = NULL;
p->set_datalink_op = NULL;
p->getnonblock_op = pcap_getnonblock_fd;
p->setnonblock_op = pcap_setnonblock_fd;
p->stats_op = pcap_stats_dlpi;
p->cleanup_op = pcap_cleanup_libdlpi;

return (status);
bad:
pcap_cleanup_libdlpi(p);
return (status);
}

#define STRINGIFY(n) #n

static int
dlpromiscon(pcap_t *p, bpf_u_int32 level)
{
struct pcap_dlpi *pd = p->priv;
int retv;
int err;

retv = dlpi_promiscon(pd->dlpi_hd, level);
if (retv != DLPI_SUCCESS) {
if (retv == DL_SYSERR &&
(errno == EPERM || errno == EACCES))
err = PCAP_ERROR_PERM_DENIED;
else
err = PCAP_ERROR;
pcap_libdlpi_err(p->opt.device, "dlpi_promiscon" STRINGIFY(level),
retv, p->errbuf);
return (err);
}
return (0);
}






static int
is_dlpi_interface(const char *name _U_)
{
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
int retv = 0;

linknamelist_t *entry, *next;
linkwalk_t lw = {NULL, 0};
int save_errno;




if (pcap_findalldevs_interfaces(devlistp, errbuf,
is_dlpi_interface, get_if_flags) == -1)
return (-1);










dlpi_walk(list_interfaces, &lw, 0);

if (lw.lw_err != 0) {
pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
lw.lw_err, "dlpi_walk");
retv = -1;
goto done;
}


for (entry = lw.lw_list; entry != NULL; entry = entry->lnl_next) {




if (find_or_add_dev(devlistp, entry->linkname, 0, get_if_flags,
NULL, errbuf) == NULL)
retv = -1;
}
done:
save_errno = errno;
for (entry = lw.lw_list; entry != NULL; entry = next) {
next = entry->lnl_next;
free(entry);
}
errno = save_errno;

return (retv);
}





static int
pcap_read_libdlpi(pcap_t *p, int count, pcap_handler callback, u_char *user)
{
struct pcap_dlpi *pd = p->priv;
int len;
u_char *bufp;
size_t msglen;
int retv;

len = p->cc;
if (len != 0) {
bufp = p->bp;
goto process_pkts;
}
do {

if (p->break_loop) {





p->break_loop = 0;
return (-2);
}

msglen = p->bufsize;
bufp = (u_char *)p->buffer + p->offset;

retv = dlpi_recv(pd->dlpi_hd, NULL, NULL, bufp,
&msglen, -1, NULL);
if (retv != DLPI_SUCCESS) {





if (retv == DL_SYSERR && errno == EINTR) {
len = 0;
continue;
}
pcap_libdlpi_err(dlpi_linkname(pd->dlpi_hd),
"dlpi_recv", retv, p->errbuf);
return (-1);
}
len = msglen;
} while (len == 0);

process_pkts:
return (pcap_process_pkts(p, callback, user, count, bufp, len));
}

static int
pcap_inject_libdlpi(pcap_t *p, const void *buf, size_t size)
{
struct pcap_dlpi *pd = p->priv;
int retv;

retv = dlpi_send(pd->dlpi_hd, NULL, 0, buf, size, NULL);
if (retv != DLPI_SUCCESS) {
pcap_libdlpi_err(dlpi_linkname(pd->dlpi_hd), "dlpi_send", retv,
p->errbuf);
return (-1);
}





return (size);
}




static void
pcap_cleanup_libdlpi(pcap_t *p)
{
struct pcap_dlpi *pd = p->priv;

if (pd->dlpi_hd != NULL) {
dlpi_close(pd->dlpi_hd);
pd->dlpi_hd = NULL;
p->fd = -1;
}
pcap_cleanup_live_common(p);
}




static void
pcap_libdlpi_err(const char *linkname, const char *func, int err, char *errbuf)
{
pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "libpcap: %s failed on %s: %s",
func, linkname, dlpi_strerror(err));
}

pcap_t *
pcap_create_interface(const char *device _U_, char *ebuf)
{
pcap_t *p;

p = pcap_create_common(ebuf, sizeof (struct pcap_dlpi));
if (p == NULL)
return (NULL);

p->activate_op = pcap_activate_libdlpi;
return (p);
}




const char *
pcap_lib_version(void)
{
return (PCAP_VERSION_STRING);
}
