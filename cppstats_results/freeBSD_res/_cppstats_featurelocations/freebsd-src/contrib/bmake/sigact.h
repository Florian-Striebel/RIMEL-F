























#if !defined(_SIGACT_H)
#define _SIGACT_H

#include <sys/cdefs.h>





#if !defined(SIG_HDLR)
#define SIG_HDLR void
#endif





#if !defined(SIGKILL)
#include <signal.h>
#endif
#if !defined(SIGKILL)
#include <sys/signal.h>
#endif

#if !defined(SIG_ERR)
#define SIG_ERR (SIG_HDLR (*)())-1
#endif
#if !defined(BADSIG)
#define BADSIG SIG_ERR
#endif

#if !defined(SA_NOCLDSTOP)


#define SA_NOCLDSTOP 1
#define SA_RESTART 2


#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 4




#if defined(_SIGSET_T_)
typedef _SIGSET_T_ sigset_t;
#endif






struct sigaction
{
SIG_HDLR (*sa_handler)();
sigset_t sa_mask;
int sa_flags;
};


int sigaction ( int , const struct sigaction *, struct sigaction * );
int sigaddset ( sigset_t *, int );
int sigdelset ( sigset_t *, int );
int sigemptyset ( sigset_t * );
int sigfillset ( sigset_t * );
int sigismember ( const sigset_t *, int );
int sigpending ( sigset_t * );
int sigprocmask ( int how, const sigset_t *, sigset_t * );
int sigsuspend ( sigset_t * );

#if !defined(sigmask)
#define sigmask(s) (1<<((s)-1) & (32 - 1))
#endif
#if !defined(NSIG) && defined(_NSIG)
#define NSIG _NSIG
#endif
#endif
#endif
