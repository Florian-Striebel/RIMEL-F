





















#include "less.h"
#include <signal.h>
#include <setjmp.h>
#if MSDOS_COMPILER==WIN32C
#include <windows.h>
#endif
#if HAVE_TIME_H
#include <time.h>
#endif
#if HAVE_ERRNO_H
#include <errno.h>
#endif
#if HAVE_VALUES_H
#include <values.h>
#endif

#if HAVE_POLL && !MSDOS_COMPILER && !defined(__APPLE__)
#define USE_POLL 1
#else
#define USE_POLL 0
#endif
#if USE_POLL
#include <poll.h>
#endif








#if HAVE__SETJMP && HAVE_SIGSETMASK
#define SET_JUMP _setjmp
#define LONG_JUMP _longjmp
#else
#define SET_JUMP setjmp
#define LONG_JUMP longjmp
#endif

public int reading;

static jmp_buf read_label;

extern int sigs;
extern int ignore_eoi;
#if !MSDOS_COMPILER
extern int tty;
#endif

#if USE_POLL



static int
poll_events(fd, events)
int fd;
int events;
{
struct pollfd poller = { fd, events, 0 };
int n = poll(&poller, 1, 0);
if (n <= 0)
return 0;
return (poller.revents & events);
}
#endif






public int
iread(fd, buf, len)
int fd;
unsigned char *buf;
unsigned int len;
{
int n;

start:
#if MSDOS_COMPILER==WIN32C
if (ABORT_SIGS())
return (READ_INTR);
#else
#if MSDOS_COMPILER && MSDOS_COMPILER != DJGPPC
if (kbhit())
{
int c;

c = getch();
if (c == '\003')
return (READ_INTR);
ungetch(c);
}
#endif
#endif
if (SET_JUMP(read_label))
{



reading = 0;
#if HAVE_SIGPROCMASK
{
sigset_t mask;
sigemptyset(&mask);
sigprocmask(SIG_SETMASK, &mask, NULL);
}
#else
#if HAVE_SIGSETMASK
sigsetmask(0);
#else
#if defined(_OSK)
sigmask(~0);
#endif
#endif
#endif
return (READ_INTR);
}

flush();
reading = 1;
#if MSDOS_COMPILER==DJGPPC
if (isatty(fd))
{






fd_set readfds;

FD_ZERO(&readfds);
FD_SET(fd, &readfds);
if (select(fd+1, &readfds, 0, 0, 0) == -1)
return (-1);
}
#endif
#if USE_POLL
if (ignore_eoi && fd != tty)
{
if (poll_events(tty, POLLIN) && getchr() == CONTROL('X'))
{
sigs |= S_INTERRUPT;
return (READ_INTR);
}
if (poll_events(fd, POLLERR|POLLHUP))
{
sigs |= S_INTERRUPT;
return (READ_INTR);
}
}
#else
#if MSDOS_COMPILER==WIN32C
if (win32_kbhit() && WIN32getch() == CONTROL('X'))
{
sigs |= S_INTERRUPT;
return (READ_INTR);
}
#endif
#endif
n = read(fd, buf, len);
#if 1





{
if (!ignore_eoi)
{
static int consecutive_nulls = 0;
if (n == 0)
consecutive_nulls++;
else
consecutive_nulls = 0;
if (consecutive_nulls > 20)
quit(QUIT_ERROR);
}
}
#endif
reading = 0;
if (n < 0)
{
#if HAVE_ERRNO



#if MUST_DEFINE_ERRNO
extern int errno;
#endif
#if defined(EINTR)
if (errno == EINTR)
goto start;
#endif
#if defined(EAGAIN)
if (errno == EAGAIN)
goto start;
#endif
#endif
return (-1);
}
return (n);
}




public void
intread(VOID_PARAM)
{
LONG_JUMP(read_label, 1);
}




#if HAVE_TIME
public time_type
get_time(VOID_PARAM)
{
time_type t;

time(&t);
return (t);
}
#endif


#if !HAVE_STRERROR



static char *
strerror(err)
int err;
{
static char buf[16];
#if HAVE_SYS_ERRLIST
extern char *sys_errlist[];
extern int sys_nerr;

if (err < sys_nerr)
return sys_errlist[err];
#endif
sprintf(buf, "Error %d", err);
return buf;
}
#endif




public char *
errno_message(filename)
char *filename;
{
char *p;
char *m;
int len;
#if HAVE_ERRNO
#if MUST_DEFINE_ERRNO
extern int errno;
#endif
p = strerror(errno);
#else
p = "cannot open";
#endif
len = (int) (strlen(filename) + strlen(p) + 3);
m = (char *) ecalloc(len, sizeof(char));
SNPRINTF2(m, len, "%s: %s", filename, p);
return (m);
}



static POSITION
muldiv(val, num, den)
POSITION val, num, den;
{
#if HAVE_FLOAT
double v = (((double) val) * num) / den;
return ((POSITION) (v + 0.5));
#else
POSITION v = ((POSITION) val) * num;

if (v / num == val)

return (POSITION) (v / den);
else


return (POSITION) (val / (den / num));
#endif
}





public int
percentage(num, den)
POSITION num;
POSITION den;
{
return (int) muldiv(num, (POSITION) 100, den);
}




public POSITION
percent_pos(pos, percent, fraction)
POSITION pos;
int percent;
long fraction;
{

POSITION perden = (percent * (NUM_FRAC_DENOM / 100)) + (fraction / 100);

if (perden == 0)
return (0);
return (POSITION) muldiv(pos, perden, (POSITION) NUM_FRAC_DENOM);
}

#if !HAVE_STRCHR



char *
strchr(s, c)
char *s;
int c;
{
for ( ; *s != '\0'; s++)
if (*s == c)
return (s);
if (c == '\0')
return (s);
return (NULL);
}
#endif

#if !HAVE_MEMCPY
VOID_POINTER
memcpy(dst, src, len)
VOID_POINTER dst;
VOID_POINTER src;
int len;
{
char *dstp = (char *) dst;
char *srcp = (char *) src;
int i;

for (i = 0; i < len; i++)
dstp[i] = srcp[i];
return (dst);
}
#endif

#if defined(_OSK_MWC32)




public int
os9_signal(type, handler)
int type;
RETSIGTYPE (*handler)();
{
intercept(handler);
}

#include <sgstat.h>

int
isatty(f)
int f;
{
struct sgbuf sgbuf;

if (_gs_opt(f, &sgbuf) < 0)
return -1;
return (sgbuf.sg_class == 0);
}

#endif

public void
sleep_ms(ms)
int ms;
{
#if MSDOS_COMPILER==WIN32C
Sleep(ms);
#else
#if HAVE_NANOSLEEP
int sec = ms / 1000;
struct timespec t = { sec, (ms - sec*1000) * 1000000 };
nanosleep(&t, NULL);
#else
#if HAVE_USLEEP
usleep(ms);
#else
sleep((ms+999) / 1000);
#endif
#endif
#endif
}
