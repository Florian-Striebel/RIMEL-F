







#include <sys/param.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "make.h"

MAKE_RCSID("$NetBSD: util.c,v 1.76 2021/02/03 08:00:36 rillig Exp $");

#if !defined(MAKE_NATIVE) && !defined(HAVE_STRERROR)
extern int errno, sys_nerr;
extern char *sys_errlist[];

char *
strerror(int e)
{
static char buf[100];
if (e < 0 || e >= sys_nerr) {
snprintf(buf, sizeof buf, "Unknown error %d", e);
return buf;
} else
return sys_errlist[e];
}
#endif

#if !defined(HAVE_GETENV) || !defined(HAVE_SETENV) || !defined(HAVE_UNSETENV)
extern char **environ;

static char *
findenv(const char *name, int *offset)
{
size_t i, len;
char *p, *q;

len = strlen(name);
for (i = 0; (q = environ[i]); i++) {
p = strchr(q, '=');
if (p == NULL || p - q != len)
continue;
if (strncmp(name, q, len) == 0) {
*offset = i;
return q + len + 1;
}
}
*offset = i;
return NULL;
}

char *
getenv(const char *name)
{
int offset;

return findenv(name, &offset);
}

int
unsetenv(const char *name)
{
char **p;
int offset;

if (name == NULL || *name == '\0' || strchr(name, '=') != NULL) {
errno = EINVAL;
return -1;
}

while (findenv(name, &offset)) {
for (p = &environ[offset];; p++)
if (!(*p = *(p + 1)))
break;
}
return 0;
}

int
setenv(const char *name, const char *value, int rewrite)
{
char *c, **newenv;
const char *cc;
size_t l_value, size;
int offset;

if (name == NULL || value == NULL) {
errno = EINVAL;
return -1;
}

if (*value == '=')
value++;
l_value = strlen(value);


if ((c = findenv(name, &offset))) {
if (!rewrite)
return 0;
if (strlen(c) >= l_value)
goto copy;
} else {
size = sizeof(char *) * (offset + 2);
if (savedEnv == environ) {
if ((newenv = realloc(savedEnv, size)) == NULL)
return -1;
savedEnv = newenv;
} else {




if ((savedEnv = malloc(size)) == NULL)
return -1;
(void)memcpy(savedEnv, environ, size - sizeof(char *));
}
environ = savedEnv;
environ[offset + 1] = NULL;
}
for (cc = name; *cc && *cc != '='; cc++)
continue;
size = cc - name;

if ((environ[offset] = malloc(size + l_value + 2)) == NULL)
return -1;
c = environ[offset];
(void)memcpy(c, name, size);
c += size;
*c++ = '=';
copy:
(void)memcpy(c, value, l_value + 1);
return 0;
}

#if defined(TEST)
int
main(int argc, char *argv[])
{
setenv(argv[1], argv[2], 0);
printf("%s\n", getenv(argv[1]));
unsetenv(argv[1]);
printf("%s\n", getenv(argv[1]));
return 0;
}
#endif

#endif


#if defined(__hpux__) || defined(__hpux)




static char *
strrcpy(char *ptr, char *str)
{
int len = strlen(str);

while (len != 0)
*--ptr = str[--len];

return ptr;
}


char *sys_siglist[] = {
"Signal 0",
"Hangup",
"Interrupt",
"Quit",
"Illegal instruction",
"Trace/BPT trap",
"IOT trap",
"EMT trap",
"Floating point exception",
"Killed",
"Bus error",
"Segmentation fault",
"Bad system call",
"Broken pipe",
"Alarm clock",
"Terminated",
"User defined signal 1",
"User defined signal 2",
"Child exited",
"Power-fail restart",
"Virtual timer expired",
"Profiling timer expired",
"I/O possible",
"Window size changes",
"Stopped (signal)",
"Stopped",
"Continued",
"Stopped (tty input)",
"Stopped (tty output)",
"Urgent I/O condition",
"Remote lock lost (NFS)",
"Signal 31",
"DIL signal"
};
#endif

#if defined(__hpux__) || defined(__hpux)
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include <unistd.h>

int
killpg(int pid, int sig)
{
return kill(-pid, sig);
}

#if !defined(BSD) && !defined(d_fileno)
#define d_fileno d_ino
#endif

#if !defined(DEV_DEV_COMPARE)
#define DEV_DEV_COMPARE(a, b) ((a) == (b))
#endif
#define ISDOT(c) ((c)[0] == '.' && (((c)[1] == '\0') || ((c)[1] == '/')))
#define ISDOTDOT(c) ((c)[0] == '.' && ISDOT(&((c)[1])))

char *
getwd(char *pathname)
{
DIR *dp;
struct dirent *d;
extern int errno;

struct stat st_root, st_cur, st_next, st_dotdot;
char pathbuf[MAXPATHLEN], nextpathbuf[MAXPATHLEN * 2];
char *pathptr, *nextpathptr, *cur_name_add;


if (stat("/", &st_root) == -1) {
(void)sprintf(pathname,
"getwd: Cannot stat \"/\" (%s)", strerror(errno));
return NULL;
}
pathbuf[MAXPATHLEN - 1] = '\0';
pathptr = &pathbuf[MAXPATHLEN - 1];
nextpathbuf[MAXPATHLEN - 1] = '\0';
cur_name_add = nextpathptr = &nextpathbuf[MAXPATHLEN - 1];


if (lstat(".", &st_cur) == -1) {
(void)sprintf(pathname,
"getwd: Cannot stat \".\" (%s)", strerror(errno));
return NULL;
}
nextpathptr = strrcpy(nextpathptr, "../");


for (;;) {


if (st_cur.st_ino == st_root.st_ino &&
DEV_DEV_COMPARE(st_cur.st_dev, st_root.st_dev)) {
(void)strcpy(pathname, *pathptr != '/' ? "/" : pathptr);
return pathname;
}


if (stat(nextpathptr, &st_dotdot) == -1) {
(void)sprintf(pathname,
"getwd: Cannot stat directory \"%s\" (%s)",
nextpathptr, strerror(errno));
return NULL;
}
if ((dp = opendir(nextpathptr)) == NULL) {
(void)sprintf(pathname,
"getwd: Cannot open directory \"%s\" (%s)",
nextpathptr, strerror(errno));
return NULL;
}


if (DEV_DEV_COMPARE(st_dotdot.st_dev, st_cur.st_dev)) {

for (d = readdir(dp); d != NULL; d = readdir(dp))
if (d->d_fileno == st_cur.st_ino)
break;
} else {




for (d = readdir(dp); d != NULL; d = readdir(dp)) {
if (ISDOT(d->d_name) || ISDOTDOT(d->d_name))
continue;
(void)strcpy(cur_name_add, d->d_name);
if (lstat(nextpathptr, &st_next) == -1) {
(void)sprintf(pathname,
"getwd: Cannot stat \"%s\" (%s)",
d->d_name, strerror(errno));
(void)closedir(dp);
return NULL;
}

if (st_next.st_ino == st_cur.st_ino &&
DEV_DEV_COMPARE(st_next.st_dev, st_cur.st_dev))
break;
}
}
if (d == NULL) {
(void)sprintf(pathname,
"getwd: Cannot find \".\" in \"..\"");
(void)closedir(dp);
return NULL;
}
st_cur = st_dotdot;
pathptr = strrcpy(pathptr, d->d_name);
pathptr = strrcpy(pathptr, "/");
nextpathptr = strrcpy(nextpathptr, "../");
(void)closedir(dp);
*cur_name_add = '\0';
}
}

#endif

#if !defined(HAVE_GETCWD)
char *
getcwd(path, sz)
char *path;
int sz;
{
return getwd(path);
}
#endif

#if !defined(HAVE_SIGACTION)
#include "sigact.h"
#endif


SignalProc
bmake_signal(int s, SignalProc a)
{
struct sigaction sa, osa;

sa.sa_handler = a;
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;

if (sigaction(s, &sa, &osa) == -1)
return SIG_ERR;
else
return osa.sa_handler;
}

#if !defined(HAVE_VSNPRINTF) || !defined(HAVE_VASPRINTF)
#include <stdarg.h>
#endif

#if !defined(HAVE_VSNPRINTF)
#if !defined(__osf__)
#if defined(_IOSTRG)
#define STRFLAG (_IOSTRG|_IOWRT)
#else
#if 0
#define STRFLAG (_IOREAD)
#endif
#endif
#endif

int
vsnprintf(char *s, size_t n, const char *fmt, va_list args)
{
#if defined(STRFLAG)
FILE fakebuf;

fakebuf._flag = STRFLAG;




fakebuf._ptr = (void *)s;
fakebuf._cnt = n - 1;
fakebuf._file = -1;
_doprnt(fmt, args, &fakebuf);
fakebuf._cnt++;
putc('\0', &fakebuf);
if (fakebuf._cnt < 0)
fakebuf._cnt = 0;
return n - fakebuf._cnt - 1;
#else
#if !defined(_PATH_DEVNULL)
#define _PATH_DEVNULL "/dev/null"
#endif




static FILE *nullfp;
int need = 0;

if (!nullfp)
nullfp = fopen(_PATH_DEVNULL, "w");
if (nullfp) {
need = vfprintf(nullfp, fmt, args);
if (need < n)
(void)vsprintf(s, fmt, args);
}
return need;
#endif
}
#endif

#if !defined(HAVE_SNPRINTF)
int
snprintf(char *s, size_t n, const char *fmt, ...)
{
va_list ap;
int rv;

va_start(ap, fmt);
rv = vsnprintf(s, n, fmt, ap);
va_end(ap);
return rv;
}
#endif

#if !defined(HAVE_STRFTIME)
size_t
strftime(char *buf, size_t len, const char *fmt, const struct tm *tm)
{
static char months[][4] = {
"Jan", "Feb", "Mar", "Apr", "May", "Jun",
"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

size_t s;
char *b = buf;

while (*fmt) {
if (len == 0)
return buf - b;
if (*fmt != '%') {
*buf++ = *fmt++;
len--;
continue;
}
switch (*fmt++) {
case '%':
*buf++ = '%';
len--;
if (len == 0) return buf - b;

case '\0':
*buf = '%';
s = 1;
break;
case 'k':
s = snprintf(buf, len, "%d", tm->tm_hour);
break;
case 'M':
s = snprintf(buf, len, "%02d", tm->tm_min);
break;
case 'S':
s = snprintf(buf, len, "%02d", tm->tm_sec);
break;
case 'b':
if (tm->tm_mon >= 12)
return buf - b;
s = snprintf(buf, len, "%s", months[tm->tm_mon]);
break;
case 'd':
s = snprintf(buf, len, "%02d", tm->tm_mday);
break;
case 'Y':
s = snprintf(buf, len, "%d", 1900 + tm->tm_year);
break;
default:
s = snprintf(buf, len, "Unsupported format %c",
fmt[-1]);
break;
}
buf += s;
len -= s;
}
return buf - b;
}
#endif

#if !defined(HAVE_KILLPG)
#if !defined(__hpux__) && !defined(__hpux)
int
killpg(int pid, int sig)
{
return kill(-pid, sig);
}
#endif
#endif

#if !defined(HAVE_WARNX)
static void
vwarnx(const char *fmt, va_list args)
{
fprintf(stderr, "%s: ", progname);
if ((fmt)) {
vfprintf(stderr, fmt, args);
fprintf(stderr, ": ");
}
}
#endif

#if !defined(HAVE_WARN)
static void
vwarn(const char *fmt, va_list args)
{
vwarnx(fmt, args);
fprintf(stderr, "%s\n", strerror(errno));
}
#endif

#if !defined(HAVE_ERR)
static void
verr(int eval, const char *fmt, va_list args)
{
vwarn(fmt, args);
exit(eval);
}
#endif

#if !defined(HAVE_ERRX)
static void
verrx(int eval, const char *fmt, va_list args)
{
vwarnx(fmt, args);
exit(eval);
}
#endif

#if !defined(HAVE_ERR)
void
err(int eval, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
verr(eval, fmt, ap);
va_end(ap);
}
#endif

#if !defined(HAVE_ERRX)
void
errx(int eval, const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
verrx(eval, fmt, ap);
va_end(ap);
}
#endif

#if !defined(HAVE_WARN)
void
warn(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
vwarn(fmt, ap);
va_end(ap);
}
#endif

#if !defined(HAVE_WARNX)
void
warnx(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
vwarnx(fmt, ap);
va_end(ap);
}
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif

#if !defined(NUM_TYPE)
#if defined(HAVE_LONG_LONG_INT)
#define NUM_TYPE long long
#elif defined(_INT64_T_DECLARED) || defined(int64_t)
#define NUM_TYPE int64_t
#endif
#endif

#if defined(NUM_TYPE)
#if !defined(HAVE_STRTOLL)
#define BCS_ONLY
#define _FUNCNAME strtoll
#define __INT NUM_TYPE
#undef __INT_MIN
#undef __INT_MAX
#if defined(LLONG_MAX)
#define __INT_MIN LLONG_MIN
#define __INT_MAX LLONG_MAX
#elif defined(INT64_MAX)
#define __INT_MIN INT64_MIN
#define __INT_MAX INT64_MAX
#endif
#if !defined(_DIAGASSERT)
#define _DIAGASSERT(e)
#endif
#if !defined(__UNCONST)
#define __UNCONST(a) ((void *)(unsigned long)(const void *)(a))
#endif
#include "_strtol.h"
#endif

#endif

#if !defined(HAVE_STRTOL)
#define BCS_ONLY
#define _FUNCNAME strtol
#define __INT long
#undef __INT_MIN
#undef __INT_MAX
#define __INT_MIN LONG_MIN
#define __INT_MAX LONG_MAX
#if !defined(_DIAGASSERT)
#define _DIAGASSERT(e)
#endif
#if !defined(__UNCONST)
#define __UNCONST(a) ((void *)(unsigned long)(const void *)(a))
#endif
#include "_strtol.h"
#endif

#if !defined(HAVE_STRTOUL)
#define BCS_ONLY
#define _FUNCNAME strtoul
#define __INT unsigned long
#undef __INT_MIN
#undef __INT_MAX
#define __INT_MIN 0
#define __INT_MAX ULONG_MAX
#if !defined(_DIAGASSERT)
#define _DIAGASSERT(e)
#endif
#if !defined(__UNCONST)
#define __UNCONST(a) ((void *)(unsigned long)(const void *)(a))
#endif
#include "_strtol.h"
#endif
