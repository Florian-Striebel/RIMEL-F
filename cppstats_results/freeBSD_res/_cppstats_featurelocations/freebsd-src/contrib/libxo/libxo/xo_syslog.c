







































#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include "xo_config.h"
#include "xo.h"
#include "xo_encoder.h"
#include "xo_buf.h"





















#define XO_SYSLOG_ENTERPRISE_ID "kern.syslog.enterprise_id"

#if defined(__FreeBSD__)
#define XO_DEFAULT_EID 2238
#elif defined(__macosx__)
#define XO_DEFAULT_EID 63
#else
#define XO_DEFAULT_EID 32473
#endif

#if !defined(HOST_NAME_MAX)
#if defined(_SC_HOST_NAME_MAX)
#define HOST_NAME_MAX _SC_HOST_NAME_MAX
#else
#define HOST_NAME_MAX 255
#endif
#endif

#if !defined(UNUSED)
#define UNUSED __attribute__ ((__unused__))
#endif

static int xo_logfile = -1;
static int xo_status;
static int xo_opened;
static int xo_logstat = 0;
static const char *xo_logtag = NULL;
static int xo_logfacility = LOG_USER;
static int xo_logmask = 0xff;
static pthread_mutex_t xo_syslog_mutex UNUSED = PTHREAD_MUTEX_INITIALIZER;
static int xo_unit_test;

#define REAL_VOID(_x) do { int really_ignored = _x; if (really_ignored) { }} while (0)


#if !defined(HAVE_DECL___ISTHREADED) || !HAVE_DECL___ISTHREADED
#define __isthreaded 1
#endif

#define THREAD_LOCK() do { if (__isthreaded) pthread_mutex_lock(&xo_syslog_mutex); } while(0)



#define THREAD_UNLOCK() do { if (__isthreaded) pthread_mutex_unlock(&xo_syslog_mutex); } while(0)




static void xo_disconnect_log(void);
static void xo_connect_log(void);
static void xo_open_log_unlocked(const char *, int, int);

enum {
NOCONN = 0,
CONNDEF,
CONNPRIV,
};

static xo_syslog_open_t xo_syslog_open;
static xo_syslog_send_t xo_syslog_send;
static xo_syslog_close_t xo_syslog_close;

static char xo_syslog_enterprise_id[12];






void
xo_set_syslog_enterprise_id (unsigned short eid)
{
snprintf(xo_syslog_enterprise_id, sizeof(xo_syslog_enterprise_id),
"%u", eid);
}




static void
xo_send_syslog (char *full_msg, char *v0_hdr,
char *text_only)
{
if (xo_syslog_send) {
xo_syslog_send(full_msg, v0_hdr, text_only);
return;
}

int fd;
int full_len = strlen(full_msg);


if (v0_hdr) {
struct iovec iov[3];
struct iovec *v = iov;
char newline[] = "\n";

v->iov_base = v0_hdr;
v->iov_len = strlen(v0_hdr);
v += 1;
v->iov_base = text_only;
v->iov_len = strlen(text_only);
v += 1;
v->iov_base = newline;
v->iov_len = 1;
v += 1;
REAL_VOID(writev(STDERR_FILENO, iov, 3));
}


if (!xo_opened)
xo_open_log_unlocked(xo_logtag, xo_logstat | LOG_NDELAY, 0);
xo_connect_log();


















if (send(xo_logfile, full_msg, full_len, 0) < 0) {
if (errno != ENOBUFS) {




xo_disconnect_log();
xo_connect_log();
if (send(xo_logfile, full_msg, full_len, 0) >= 0) {
return;
}




}
while (errno == ENOBUFS) {





if (xo_status == CONNPRIV)
break;
usleep(1);
if (send(xo_logfile, full_msg, full_len, 0) >= 0) {
return;
}
}
} else {
return;
}






int flags = O_WRONLY | O_NONBLOCK;
#if defined(O_CLOEXEC)
flags |= O_CLOEXEC;
#endif

if (xo_logstat & LOG_CONS
&& (fd = open(_PATH_CONSOLE, flags, 0)) >= 0) {
struct iovec iov[2];
struct iovec *v = iov;
char crnl[] = "\r\n";
char *p;

p = strchr(full_msg, '>') + 1;
v->iov_base = p;
v->iov_len = full_len - (p - full_msg);
++v;
v->iov_base = crnl;
v->iov_len = 2;
REAL_VOID(writev(fd, iov, 2));
(void) close(fd);
}
}


static void
xo_disconnect_log (void)
{
if (xo_syslog_close) {
xo_syslog_close();
return;
}






if (xo_logfile != -1) {
close(xo_logfile);
xo_logfile = -1;
}
xo_status = NOCONN;
}


static void
xo_connect_log (void)
{
if (xo_syslog_open) {
xo_syslog_open();
return;
}

struct sockaddr_un saddr;

if (xo_logfile == -1) {
int flags = SOCK_DGRAM;
#if defined(SOCK_CLOEXEC)
flags |= SOCK_CLOEXEC;
#endif
if ((xo_logfile = socket(AF_UNIX, flags, 0)) == -1)
return;
}
if (xo_logfile != -1 && xo_status == NOCONN) {
#if defined(HAVE_SUN_LEN)
saddr.sun_len = sizeof(saddr);
#endif
saddr.sun_family = AF_UNIX;






#if defined(_PATH_LOG_PRIV)
(void) strncpy(saddr.sun_path, _PATH_LOG_PRIV,
sizeof saddr.sun_path);
if (connect(xo_logfile, (struct sockaddr *) &saddr,
sizeof(saddr)) != -1)
xo_status = CONNPRIV;
#endif

#if defined(_PATH_LOG)
if (xo_status == NOCONN) {
(void) strncpy(saddr.sun_path, _PATH_LOG,
sizeof saddr.sun_path);
if (connect(xo_logfile, (struct sockaddr *)&saddr,
sizeof(saddr)) != -1)
xo_status = CONNDEF;
}
#endif

#if defined(_PATH_OLDLOG)
if (xo_status == NOCONN) {




(void) strncpy(saddr.sun_path, _PATH_OLDLOG,
sizeof saddr.sun_path);
if (connect(xo_logfile, (struct sockaddr *)&saddr,
sizeof(saddr)) != -1)
xo_status = CONNDEF;
}
#endif

if (xo_status == NOCONN) {
(void) close(xo_logfile);
xo_logfile = -1;
}
}
}

static void
xo_open_log_unlocked (const char *ident, int logstat, int logfac)
{
if (ident != NULL)
xo_logtag = ident;
xo_logstat = logstat;
if (logfac != 0 && (logfac &~ LOG_FACMASK) == 0)
xo_logfacility = logfac;

if (xo_logstat & LOG_NDELAY)
xo_connect_log();

xo_opened = 1;
}

void
xo_open_log (const char *ident, int logstat, int logfac)
{
THREAD_LOCK();
xo_open_log_unlocked(ident, logstat, logfac);
THREAD_UNLOCK();
}


void
xo_close_log (void)
{
THREAD_LOCK();
if (xo_logfile != -1) {
(void) close(xo_logfile);
xo_logfile = -1;
}
xo_logtag = NULL;
xo_status = NOCONN;
THREAD_UNLOCK();
}


int
xo_set_logmask (int pmask)
{
int omask;

THREAD_LOCK();
omask = xo_logmask;
if (pmask != 0)
xo_logmask = pmask;
THREAD_UNLOCK();
return (omask);
}

void
xo_set_syslog_handler (xo_syslog_open_t open_func,
xo_syslog_send_t send_func,
xo_syslog_close_t close_func)
{
xo_syslog_open = open_func;
xo_syslog_send = send_func;
xo_syslog_close = close_func;
}

static ssize_t
xo_snprintf (char *out, ssize_t outsize, const char *fmt, ...)
{
ssize_t status;
ssize_t retval = 0;
va_list ap;

if (out && outsize) {
va_start(ap, fmt);
status = vsnprintf(out, outsize, fmt, ap);
if (status < 0) {
*out = 0;
retval = 0;
} else {
retval = status;
retval = retval > outsize ? outsize : retval;
}
va_end(ap);
}

return retval;
}

static xo_ssize_t
xo_syslog_handle_write (void *opaque, const char *data)
{
xo_buffer_t *xbp = opaque;
int len = strlen(data);
int left = xo_buf_left(xbp);

if (len > left - 1)
len = left - 1;

memcpy(xbp->xb_curp, data, len);
xbp->xb_curp += len;
*xbp->xb_curp = '\0';

return len;
}

static void
xo_syslog_handle_close (void *opaque UNUSED)
{
}

static int
xo_syslog_handle_flush (void *opaque UNUSED)
{
return 0;
}

void
xo_set_unit_test_mode (int value)
{
xo_unit_test = value;
}

void
xo_vsyslog (int pri, const char *name, const char *fmt, va_list vap)
{
int saved_errno = errno;
char tbuf[2048];
char *tp = NULL, *ep = NULL;
unsigned start_of_msg = 0;
char *v0_hdr = NULL;
xo_buffer_t xb;
static pid_t my_pid;
unsigned log_offset;

if (my_pid == 0)
my_pid = xo_unit_test ? 222 : getpid();


if (pri & ~(LOG_PRIMASK|LOG_FACMASK)) {
xo_syslog(LOG_ERR | LOG_CONS | LOG_PERROR | LOG_PID,
"syslog-unknown-priority",
"syslog: unknown facility/priority: %#x", pri);
pri &= LOG_PRIMASK|LOG_FACMASK;
}

THREAD_LOCK();


if (!(LOG_MASK(LOG_PRI(pri)) & xo_logmask)) {
THREAD_UNLOCK();
return;
}


if ((pri & LOG_FACMASK) == 0)
pri |= xo_logfacility;


xb.xb_bufp = tbuf;
xb.xb_curp = tbuf;
xb.xb_size = sizeof(tbuf);

xo_handle_t *xop = xo_create(XO_STYLE_SDPARAMS, 0);
if (xop == NULL) {
THREAD_UNLOCK();
return;
}

#if defined(HAVE_GETPROGNAME)
if (xo_logtag == NULL)
xo_logtag = getprogname();
#endif

xo_set_writer(xop, &xb, xo_syslog_handle_write, xo_syslog_handle_close,
xo_syslog_handle_flush);


struct tm tm;
struct timeval tv;


if (xo_unit_test) {
tv.tv_sec = 1435085229;
tv.tv_usec = 123456;
} else
gettimeofday(&tv, NULL);

(void) localtime_r(&tv.tv_sec, &tm);

if (xo_logstat & LOG_PERROR) {




v0_hdr = alloca(2048);
tp = v0_hdr;
ep = v0_hdr + 2048;

if (xo_logtag != NULL)
tp += xo_snprintf(tp, ep - tp, "%s", xo_logtag);
if (xo_logstat & LOG_PID)
tp += xo_snprintf(tp, ep - tp, "[%d]", my_pid);
if (xo_logtag)
tp += xo_snprintf(tp, ep - tp, ": ");
}

log_offset = xb.xb_curp - xb.xb_bufp;


xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb), "<%d>1 ", pri);


xb.xb_curp += strftime(xb.xb_curp, xo_buf_left(&xb), "%FT%T", &tm);
xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb),
".%03.3u", tv.tv_usec / 1000);
xb.xb_curp += strftime(xb.xb_curp, xo_buf_left(&xb), "%z ", &tm);





char hostname[HOST_NAME_MAX];
hostname[0] = '\0';
if (xo_unit_test)
strcpy(hostname, "worker-host");
else
(void) gethostname(hostname, sizeof(hostname));

xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb), "%s ",
hostname[0] ? hostname : "-");


xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb), "%s ",
xo_logtag ?: "-");


xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb), "%d ", my_pid);






char *buf UNUSED = NULL;
const char *eid = xo_syslog_enterprise_id;
const char *at_sign = "@";

if (name == NULL) {
name = "-";
eid = at_sign = "";

} else if (*name == '@') {

name += 1;
eid = at_sign = "";

} else if (eid[0] == '\0') {
#if defined(HAVE_SYSCTLBYNAME)



size_t size = 0;
if (sysctlbyname(XO_SYSLOG_ENTERPRISE_ID, NULL, &size, NULL, 0) == 0
&& size > 0) {
buf = alloca(size);
if (sysctlbyname(XO_SYSLOG_ENTERPRISE_ID, buf, &size, NULL, 0) == 0
&& size > 0)
eid = buf;
}
#endif

if (eid[0] == '\0') {

xo_set_syslog_enterprise_id(XO_DEFAULT_EID);
eid = xo_syslog_enterprise_id;
}
}

xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb), "%s [%s%s%s ",
name, name, at_sign, eid);






va_list ap;
va_copy(ap, vap);

errno = saved_errno;
xo_emit_hv(xop, fmt, ap);
xo_flush_h(xop);

va_end(ap);


if (xb.xb_curp[-1] == ' ')
xb.xb_curp -= 1;


xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb), "] ");





xb.xb_curp += xo_snprintf(xb.xb_curp, xo_buf_left(&xb),
"%c%c%c", 0xEF, 0xBB, 0xBF);


if (xo_logstat & LOG_PERROR)
start_of_msg = xb.xb_curp - xb.xb_bufp;

xo_set_style(xop, XO_STYLE_TEXT);
xo_set_flags(xop, XOF_UTF8);

errno = saved_errno;
xo_emit_hv(xop, fmt, ap);
xo_flush_h(xop);


if (xb.xb_curp[-1] == '\n')
*--xb.xb_curp = '\0';

if (xo_get_flags(xop) & XOF_LOG_SYSLOG)
fprintf(stderr, "xo: syslog: %s\n", xb.xb_bufp + log_offset);

xo_send_syslog(xb.xb_bufp, v0_hdr, xb.xb_bufp + start_of_msg);

xo_destroy(xop);

THREAD_UNLOCK();
}




void
xo_syslog (int pri, const char *name, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
xo_vsyslog(pri, name, fmt, ap);
va_end(ap);
}
