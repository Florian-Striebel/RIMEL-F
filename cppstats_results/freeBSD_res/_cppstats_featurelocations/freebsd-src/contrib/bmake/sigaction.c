

























































































































#if !defined(lint)
static char *RCSid = "$Id: sigact.c,v 1.8 2021/10/14 19:39:17 sjg Exp $";
#endif

#undef _ANSI_SOURCE

#include <signal.h>
#include <sys/cdefs.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#if defined(NO_SIGSET)
#undef HAVE_SIGSET
#endif
#if !defined(HAVE_SIGACTION)
#if defined(HAVE_SIGSETMASK)
#define USE_SIGMASK
#else
#if defined(HAVE_SIGSET)
#define USE_SIGSET
#else
#define USE_SIGNAL
#endif
#endif
#endif
#endif







#if !defined(SA_NOCLDSTOP) || defined(_SIGACT_H) || defined(USE_SIGNAL) || defined(USE_SIGSET) || defined(USE_SIGMASK)





#if !defined(USE_SIGSET) && !defined(USE_SIGMASK) && !defined(USE_SIGNAL)
#if defined(sigmask) || defined(BSD) || defined(_BSD) && !defined(BSD41)
#define USE_SIGMASK
#else
#if !defined(NO_SIGSET)
#define USE_SIGSET
#else
#define USE_SIGNAL
#endif
#endif
#endif



#if !defined(USE_SIGSET) && !defined(USE_SIGMASK) && !defined(USE_SIGNAL)
error must know what to implement with
#endif

#include "sigact.h"




#undef signal


int
sigaction(int sig,
const struct sigaction *act,
struct sigaction *oact)
{
SIG_HDLR(*oldh) ();

if (act) {
#if defined(USE_SIGSET)
oldh = sigset(sig, act->sa_handler);
#else
oldh = signal(sig, act->sa_handler);
#endif
} else {
if (oact) {
#if defined(USE_SIGSET)
oldh = sigset(sig, SIG_IGN);
#else
oldh = signal(sig, SIG_IGN);
#endif
if (oldh != SIG_IGN && oldh != SIG_ERR) {
#if defined(USE_SIGSET)
(void) sigset(sig, oldh);
#else
(void) signal(sig, oldh);
#endif
}
}
}
if (oact) {
oact->sa_handler = oldh;
}
return 0;
}

#if !defined(HAVE_SIGADDSET)
int
sigaddset(sigset_t *mask, int sig)
{
*mask |= sigmask(sig);
return 0;
}


int
sigdelset(sigset_t *mask, int sig)
{
*mask &= ~(sigmask(sig));
return 0;
}


int
sigemptyset(sigset_t *mask)
{
*mask = 0;
return 0;
}


int
sigfillset(sigset_t *mask)
{
*mask = ~0;
return 0;
}


int
sigismember(const sigset_t *mask, int sig)
{
return ((*mask) & sigmask(sig));
}
#endif

#if !defined(HAVE_SIGPENDING)
int
sigpending(sigset_t *set)
{
return 0;
}
#endif

#if !defined(HAVE_SIGPROCMASK)
int
sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
#if defined(USE_SIGSET)
int i;
#endif
static sigset_t sm;
static int once = 0;

if (!once) {





once++;
#if defined(USE_SIGMASK)
sm = sigblock(0);
#else
sm = 0;
#endif
}
if (oset)
*oset = sm;
if (set) {
switch (how) {
case SIG_BLOCK:
sm |= *set;
break;
case SIG_UNBLOCK:
sm &= ~(*set);
break;
case SIG_SETMASK:
sm = *set;
break;
}
#if defined(USE_SIGMASK)
(void) sigsetmask(sm);
#else
#if defined(USE_SIGSET)
for (i = 1; i < NSIG; i++) {
if (how == SIG_UNBLOCK) {
if (*set & sigmask(i))
sigrelse(i);
} else
if (sm & sigmask(i)) {
sighold(i);
}
}
#endif
#endif
}
return 0;
}
#endif

#if !defined(HAVE_SIGSUSPEND)
int
sigsuspend(sigset_t *mask)
{
#if defined(USE_SIGMASK)
sigpause(*mask);
#else
int i;

#if defined(USE_SIGSET)

for (i = 1; i < NSIG; i++) {
if (*mask & sigmask(i)) {

sigpause(i);
break;
}
}
#else
SIG_HDLR(*oldh) ();





for (i = 1; i < NSIG; i++) {
if (*mask & sigmask(i)) {
if ((oldh = signal(i, SIG_DFL)) != SIG_ERR &&
oldh != SIG_IGN &&
oldh != SIG_DFL)
(void) signal(i, oldh);
}
}
pause();
#endif
#endif
return 0;
}
#endif
#endif

#if 0
#if !defined(SIG_HDLR)
#define SIG_HDLR void
#endif
#if !defined(SIG_ERR)
#define SIG_ERR (SIG_HDLR (*)())-1
#endif

#if !defined(USE_SIGNAL) && !defined(USE_SIGMASK) && !defined(NO_SIGNAL)




extern void (*Signal (int sig, void (*handler) (int)))(int);

SIG_HDLR(*signal(int sig, SIG_HDLR(*handler)(int))
{
return (Signal(sig, handler));
}
#endif
#endif








