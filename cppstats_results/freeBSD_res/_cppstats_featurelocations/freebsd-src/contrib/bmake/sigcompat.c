






















































































#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <signal.h>
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(sun) && !(defined(__svr4__) || defined(__SVR4))
#define NO_SIGCOMPAT
#endif
#if defined(__MINT__)
#define NO_SIGCOMPAT
#endif

#if !defined(NO_SIGCOMPAT) && (defined(HAVE_SIGACTION) || defined(SA_NOCLDSTOP))

#if defined(LIBC_SCCS) && !defined(lint)

static char *rcsid = "$Id: sigcompat.c,v 1.23 2011/02/14 00:07:11 sjg Exp $";
#endif

#undef signal
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/cdefs.h>
#include "assert.h"

#if !defined(ASSERT)
#define ASSERT assert
#endif

#if defined(NDEBUG)
#define _DBUG(x)
#else
#define _DBUG(x) x
#endif

#if !defined(SA_RESTART)
#define SA_RESTART 2
#endif
#if !defined(SV_INTERRUPT)
#define SV_INTERRUPT SA_RESTART
#endif

#if !defined(MASK_T)
#if defined(__hpux__) || defined(__hpux)
#define MASK_T long
#else
#define MASK_T int
#endif
#endif

#if (defined(__HPUX_VERSION) && __HPUX_VERSION > 9) || defined(__hpux)
#define PAUSE_MASK_T int
#else
#define PAUSE_MASK_T MASK_T
#endif

#if !defined(SIG_HDLR)
#define SIG_HDLR void
#endif

#if defined(FORCE_POSIX_SIGNALS)
#if !(defined(libsig) || defined(libsjg))







#if !defined(SIGNAL_FLAGS)
#define SIGNAL_FLAGS 0
#endif
int _signalFlags = SIGNAL_FLAGS;

SIG_HDLR(*signal(int sig, SIG_HDLR(*handler)(int)))(int)
{
_DBUG(static int depth_signal = 0);
struct sigaction act, oact;
int n;

_DBUG(++depth_signal);
ASSERT(depth_signal < 2);
act.sa_handler = handler;
sigemptyset(&act.sa_mask);
act.sa_flags = _signalFlags;
n = sigaction(sig, &act, &oact);
_DBUG(--depth_signal);
if (n < 0)
return (SIG_ERR);
return (oact.sa_handler);
}
#else
SIG_HDLR(*signal(int sig, SIG_HDLR(*handler)(int)))(int)
{
extern SIG_HDLR(*Signal(int, void (*)(int)))(int);
_DBUG(static int depth_signal = 0);
SIG_HDLR(*old) __P((int));

_DBUG(++depth_signal);
ASSERT(depth_signal < 2);
old = Signal(sig, handler);
_DBUG(--depth_signal);
return old;
}
#endif
#endif






#if defined(SIGSET_T_INT)
#define ss2m(ss) (MASK_T) *(ss)
#define m2ss(ss, m) *ss = (sigset_t) *(m)
#else
static MASK_T
ss2m(sigset_t *ss)
{
MASK_T ma[(sizeof(sigset_t) / sizeof(MASK_T)) + 1];

memcpy((char *) ma, (char *) ss, sizeof(sigset_t));
return ma[0];
}

static void
m2ss(sigset_t *ss, MASK_T *m)
{
if (sizeof(sigset_t) > sizeof(MASK_T))
memset((char *) ss, 0, sizeof(sigset_t));

memcpy((char *) ss, (char *) m, sizeof(MASK_T));
}
#endif

#if !defined(HAVE_SIGSETMASK) || defined(FORCE_POSIX_SIGNALS)
MASK_T
sigsetmask(MASK_T mask)
{
_DBUG(static int depth_sigsetmask = 0);
sigset_t m, omask;
int n;

_DBUG(++depth_sigsetmask);
ASSERT(depth_sigsetmask < 2);
m2ss(&m, &mask);
n = sigprocmask(SIG_SETMASK, (sigset_t *) & m, (sigset_t *) & omask);
_DBUG(--depth_sigsetmask);
if (n)
return (n);

return ss2m(&omask);
}


MASK_T
sigblock(MASK_T mask)
{
_DBUG(static int depth_sigblock = 0);
sigset_t m, omask;
int n;

_DBUG(++depth_sigblock);
ASSERT(depth_sigblock < 2);
if (mask)
m2ss(&m, &mask);
n = sigprocmask(SIG_BLOCK, (sigset_t *) ((mask) ? &m : 0), (sigset_t *) & omask);
_DBUG(--depth_sigblock);
if (n)
return (n);
return ss2m(&omask);
}

#undef sigpause

PAUSE_MASK_T
sigpause(PAUSE_MASK_T mask)
{
_DBUG(static int depth_sigpause = 0);
sigset_t m;
PAUSE_MASK_T n;

_DBUG(++depth_sigpause);
ASSERT(depth_sigpause < 2);
m2ss(&m, &mask);
n = sigsuspend(&m);
_DBUG(--depth_sigpause);
return n;
}
#endif

#if defined(HAVE_SIGVEC) && defined(FORCE_POSIX_SIGNALS)
int
sigvec(int signo, struct sigvec *sv, struct sigvec *osv)
{
_DBUG(static int depth_sigvec = 0);
int ret;
struct sigvec nsv;

_DBUG(++depth_sigvec);
ASSERT(depth_sigvec < 2);
if (sv) {
nsv = *sv;
nsv.sv_flags ^= SV_INTERRUPT;
}
ret = sigaction(signo, sv ? (struct sigaction *) & nsv : NULL,
(struct sigaction *) osv);
_DBUG(--depth_sigvec);
if (ret == 0 && osv)
osv->sv_flags ^= SV_INTERRUPT;
return (ret);
}
#endif

#if defined(MAIN)
#if !defined(sigmask)
#define sigmask(n) ((unsigned int)1 << (((n) - 1) & (32 - 1)))
#endif

int
main(int argc, char *argv[])
{
MASK_T old = 0;

printf("expect: old=0,old=2\n");
fflush(stdout);
signal(SIGQUIT, SIG_IGN);
old = sigblock(sigmask(SIGINT));
printf("old=%d,", old);
old = sigsetmask(sigmask(SIGALRM));
printf("old=%d\n", old);
}
#endif
#endif
