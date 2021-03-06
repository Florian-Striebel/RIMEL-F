

































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)trap.c 8.5 (Berkeley) 6/5/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "shell.h"
#include "main.h"
#include "nodes.h"
#include "eval.h"
#include "jobs.h"
#include "show.h"
#include "options.h"
#include "syntax.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "trap.h"
#include "mystring.h"
#include "builtins.h"
#if !defined(NO_HISTORY)
#include "myhistedit.h"
#endif








#define S_DFL 1
#define S_CATCH 2
#define S_IGN 3
#define S_HARD_IGN 4
#define S_RESET 5


static char sigmode[NSIG];
volatile sig_atomic_t pendingsig;
volatile sig_atomic_t pendingsig_waitcmd;
static int in_dotrap;
static char *volatile trap[NSIG];
static volatile sig_atomic_t gotsig[NSIG];

static int ignore_sigchld;
static int last_trapsig;

static int exiting;
static int exiting_exitstatus;

static int getsigaction(int, sig_t *);







static int
sigstring_to_signum(char *sig)
{

if (is_number(sig)) {
int signo;

signo = atoi(sig);
return ((signo >= 0 && signo < NSIG) ? signo : (-1));
} else if (strcasecmp(sig, "EXIT") == 0) {
return (0);
} else {
int n;

if (strncasecmp(sig, "SIG", 3) == 0)
sig += 3;
for (n = 1; n < sys_nsig; n++)
if (sys_signame[n] &&
strcasecmp(sys_signame[n], sig) == 0)
return (n);
}
return (-1);
}





static void
printsignals(void)
{
int n, outlen;

outlen = 0;
for (n = 1; n < sys_nsig; n++) {
if (sys_signame[n]) {
out1fmt("%s", sys_signame[n]);
outlen += strlen(sys_signame[n]);
} else {
out1fmt("%d", n);
outlen += 3;
}
++outlen;
if (outlen > 71 || n == sys_nsig - 1) {
out1str("\n");
outlen = 0;
} else {
out1c(' ');
}
}
}





int
trapcmd(int argc __unused, char **argv)
{
char *action;
int signo;
int errors = 0;
int i;

while ((i = nextopt("l")) != '\0') {
switch (i) {
case 'l':
printsignals();
return (0);
}
}
argv = argptr;

if (*argv == NULL) {
for (signo = 0 ; signo < sys_nsig ; signo++) {
if (signo < NSIG && trap[signo] != NULL) {
out1str("trap -- ");
out1qstr(trap[signo]);
if (signo == 0) {
out1str(" EXIT\n");
} else if (sys_signame[signo]) {
out1fmt(" %s\n", sys_signame[signo]);
} else {
out1fmt(" %d\n", signo);
}
}
}
return 0;
}
action = NULL;
if (*argv && !is_number(*argv)) {
if (strcmp(*argv, "-") == 0)
argv++;
else {
action = *argv;
argv++;
}
}
for (; *argv; argv++) {
if ((signo = sigstring_to_signum(*argv)) == -1) {
warning("bad signal %s", *argv);
errors = 1;
continue;
}
INTOFF;
if (action)
action = savestr(action);
if (trap[signo])
ckfree(trap[signo]);
trap[signo] = action;
if (signo != 0)
setsignal(signo);
INTON;
}
return errors;
}





void
clear_traps(void)
{
char *volatile *tp;

for (tp = trap ; tp <= &trap[NSIG - 1] ; tp++) {
if (*tp && **tp) {
INTOFF;
ckfree(*tp);
*tp = NULL;
if (tp != &trap[0])
setsignal(tp - trap);
INTON;
}
}
}





int
have_traps(void)
{
char *volatile *tp;

for (tp = trap ; tp <= &trap[NSIG - 1] ; tp++) {
if (*tp && **tp)
return 1;
}
return 0;
}





void
setsignal(int signo)
{
int action;
sig_t sigact = SIG_DFL;
struct sigaction sa;
char *t;

if ((t = trap[signo]) == NULL)
action = S_DFL;
else if (*t != '\0')
action = S_CATCH;
else
action = S_IGN;
if (action == S_DFL) {
switch (signo) {
case SIGINT:
action = S_CATCH;
break;
case SIGQUIT:
#if defined(DEBUG)
if (debug)
break;
#endif
action = S_CATCH;
break;
case SIGTERM:
if (rootshell && iflag)
action = S_IGN;
break;
#if JOBS
case SIGTSTP:
case SIGTTOU:
if (rootshell && mflag)
action = S_IGN;
break;
#endif
}
}

t = &sigmode[signo];
if (*t == 0) {



if (!getsigaction(signo, &sigact)) {





return;
}
if (sigact == SIG_IGN) {
if (mflag && (signo == SIGTSTP ||
signo == SIGTTIN || signo == SIGTTOU)) {
*t = S_IGN;
} else
*t = S_HARD_IGN;
} else {
*t = S_RESET;
}
}
if (*t == S_HARD_IGN || *t == action)
return;
switch (action) {
case S_DFL: sigact = SIG_DFL; break;
case S_CATCH: sigact = onsig; break;
case S_IGN: sigact = SIG_IGN; break;
}
*t = action;
sa.sa_handler = sigact;
sa.sa_flags = 0;
sigemptyset(&sa.sa_mask);
sigaction(signo, &sa, NULL);
}





static int
getsigaction(int signo, sig_t *sigact)
{
struct sigaction sa;

if (sigaction(signo, (struct sigaction *)0, &sa) == -1)
return 0;
*sigact = (sig_t) sa.sa_handler;
return 1;
}





void
ignoresig(int signo)
{

if (sigmode[signo] == 0)
setsignal(signo);
if (sigmode[signo] != S_IGN && sigmode[signo] != S_HARD_IGN) {
signal(signo, SIG_IGN);
sigmode[signo] = S_IGN;
}
}


int
issigchldtrapped(void)
{

return (trap[SIGCHLD] != NULL && *trap[SIGCHLD] != '\0');
}





void
onsig(int signo)
{

if (signo == SIGINT && trap[SIGINT] == NULL) {
if (suppressint)
SET_PENDING_INT;
else
onint();
return;
}


if (signo == SIGINT || signo == SIGQUIT)
pendingsig_waitcmd = signo;

if (trap[signo] != NULL && trap[signo][0] != '\0' &&
(signo != SIGCHLD || !ignore_sigchld)) {
gotsig[signo] = 1;
pendingsig = signo;
pendingsig_waitcmd = signo;
}
}






void
dotrap(void)
{
struct stackmark smark;
int i;
int savestatus, prev_evalskip, prev_skipcount;

in_dotrap++;
for (;;) {
pendingsig = 0;
pendingsig_waitcmd = 0;
for (i = 1; i < NSIG; i++) {
if (gotsig[i]) {
gotsig[i] = 0;
if (trap[i]) {





if (i == SIGCHLD)
ignore_sigchld++;









prev_evalskip = evalskip;
prev_skipcount = skipcount;
evalskip = 0;

last_trapsig = i;
savestatus = exitstatus;
setstackmark(&smark);
evalstring(stsavestr(trap[i]), 0);
popstackmark(&smark);








if (evalskip == 0 ||
prev_evalskip != 0) {
evalskip = prev_evalskip;
skipcount = prev_skipcount;
exitstatus = savestatus;
}

if (i == SIGCHLD)
ignore_sigchld--;
}
break;
}
}
if (i >= NSIG)
break;
}
in_dotrap--;
}


void
trap_init(void)
{
setsignal(SIGINT);
setsignal(SIGQUIT);
}





void
setinteractive(void)
{
setsignal(SIGTERM);
}





void
exitshell(int status)
{
TRACE(("exitshell(%d) pid=%d\n", status, getpid()));
exiting = 1;
exiting_exitstatus = status;
exitshell_savedstatus();
}

void
exitshell_savedstatus(void)
{
struct jmploc loc1, loc2;
char *p;
int sig = 0;
sigset_t sigs;

if (!exiting) {
if (in_dotrap && last_trapsig) {
sig = last_trapsig;
exiting_exitstatus = sig + 128;
} else
exiting_exitstatus = oexitstatus;
}
exitstatus = oexitstatus = exiting_exitstatus;
if (!setjmp(loc1.loc)) {
handler = &loc1;
if ((p = trap[0]) != NULL && *p != '\0') {




evalskip = 0;
trap[0] = NULL;
FORCEINTON;
evalstring(p, 0);
}
}
if (!setjmp(loc2.loc)) {
handler = &loc2;
FORCEINTON;
flushall();
#if JOBS
setjobctl(0);
#endif
#if !defined(NO_HISTORY)
histsave();
#endif
}
if (sig != 0 && sig != SIGSTOP && sig != SIGTSTP && sig != SIGTTIN &&
sig != SIGTTOU) {
signal(sig, SIG_DFL);
sigemptyset(&sigs);
sigaddset(&sigs, sig);
sigprocmask(SIG_UNBLOCK, &sigs, NULL);
kill(getpid(), sig);

}
_exit(exiting_exitstatus);
}
