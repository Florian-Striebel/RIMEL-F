































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)jobs.c 8.5 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "shell.h"
#if JOBS
#include <termios.h>
#undef CEOF
#endif
#include "redir.h"
#include "exec.h"
#include "show.h"
#include "main.h"
#include "parser.h"
#include "nodes.h"
#include "jobs.h"
#include "options.h"
#include "trap.h"
#include "syntax.h"
#include "input.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "mystring.h"
#include "var.h"
#include "builtins.h"
#include "eval.h"









struct procstat {
pid_t pid;
int status;
char *cmd;
};



#define JOBSTOPPED 1
#define JOBDONE 2


struct job {
struct procstat ps0;
struct procstat *ps;
short nprocs;
pid_t pgrp;
char state;
char used;
char changed;
char foreground;
char remembered;
char pipefail;
#if JOBS
char jobctl;
struct job *next;
#endif
};


static struct job *jobtab;
static int njobs;
static pid_t backgndpid = -1;
static struct job *bgjob = NULL;
#if JOBS
static struct job *jobmru;
static pid_t initialpgrp;
#endif
static int ttyfd = -1;


#define DOWAIT_BLOCK 0x1
#define DOWAIT_SIG 0x2
#define DOWAIT_SIG_TRAP 0x4

#if JOBS
static void restartjob(struct job *);
#endif
static void freejob(struct job *);
static int waitcmdloop(struct job *);
static struct job *getjob_nonotfound(const char *);
static struct job *getjob(const char *);
pid_t killjob(const char *, int);
static pid_t dowait(int, struct job *);
static void checkzombies(void);
static void cmdtxt(union node *);
static void cmdputs(const char *);
#if JOBS
static void setcurjob(struct job *);
static void deljob(struct job *);
static struct job *getcurjob(struct job *);
#endif
static int getjobstatus(const struct job *);
static void printjobcmd(struct job *);
static void showjob(struct job *, int);






static int jobctl;

#if JOBS
static void
jobctl_notty(void)
{
if (ttyfd >= 0) {
close(ttyfd);
ttyfd = -1;
}
if (!iflag) {
setsignal(SIGTSTP);
setsignal(SIGTTOU);
setsignal(SIGTTIN);
jobctl = 1;
return;
}
out2fmt_flush("sh: can't access tty; job control turned off\n");
mflag = 0;
}

void
setjobctl(int on)
{
int i;

if (on == jobctl || rootshell == 0)
return;
if (on) {
if (ttyfd != -1)
close(ttyfd);
if ((ttyfd = open(_PATH_TTY, O_RDWR | O_CLOEXEC)) < 0) {
i = 0;
while (i <= 2 && !isatty(i))
i++;
if (i > 2 ||
(ttyfd = fcntl(i, F_DUPFD_CLOEXEC, 10)) < 0) {
jobctl_notty();
return;
}
}
if (ttyfd < 10) {




if ((i = fcntl(ttyfd, F_DUPFD_CLOEXEC, 10)) < 0) {
jobctl_notty();
return;
}
close(ttyfd);
ttyfd = i;
}
do {
initialpgrp = tcgetpgrp(ttyfd);
if (initialpgrp < 0) {
jobctl_notty();
return;
}
if (initialpgrp != getpgrp()) {
if (!iflag) {
initialpgrp = -1;
jobctl_notty();
return;
}
kill(0, SIGTTIN);
continue;
}
} while (0);
setsignal(SIGTSTP);
setsignal(SIGTTOU);
setsignal(SIGTTIN);
setpgid(0, rootpid);
tcsetpgrp(ttyfd, rootpid);
} else {
setpgid(0, initialpgrp);
if (ttyfd >= 0) {
tcsetpgrp(ttyfd, initialpgrp);
close(ttyfd);
ttyfd = -1;
}
setsignal(SIGTSTP);
setsignal(SIGTTOU);
setsignal(SIGTTIN);
}
jobctl = on;
}
#endif


#if JOBS
int
fgcmd(int argc __unused, char **argv __unused)
{
struct job *jp;
pid_t pgrp;
int status;

nextopt("");
jp = getjob(*argptr);
if (jp->jobctl == 0)
error("job not created under job control");
printjobcmd(jp);
flushout(&output);
pgrp = jp->ps[0].pid;
if (ttyfd >= 0)
tcsetpgrp(ttyfd, pgrp);
restartjob(jp);
jp->foreground = 1;
INTOFF;
status = waitforjob(jp, (int *)NULL);
INTON;
return status;
}


int
bgcmd(int argc __unused, char **argv __unused)
{
struct job *jp;

nextopt("");
do {
jp = getjob(*argptr);
if (jp->jobctl == 0)
error("job not created under job control");
if (jp->state == JOBDONE)
continue;
restartjob(jp);
jp->foreground = 0;
out1fmt("[%td] ", jp - jobtab + 1);
printjobcmd(jp);
} while (*argptr != NULL && *++argptr != NULL);
return 0;
}


static void
restartjob(struct job *jp)
{
struct procstat *ps;
int i;

if (jp->state == JOBDONE)
return;
setcurjob(jp);
INTOFF;
kill(-jp->ps[0].pid, SIGCONT);
for (ps = jp->ps, i = jp->nprocs ; --i >= 0 ; ps++) {
if (WIFSTOPPED(ps->status)) {
ps->status = -1;
jp->state = 0;
}
}
INTON;
}
#endif


int
jobscmd(int argc __unused, char *argv[] __unused)
{
char *id;
int ch, mode;

mode = SHOWJOBS_DEFAULT;
while ((ch = nextopt("lps")) != '\0') {
switch (ch) {
case 'l':
mode = SHOWJOBS_VERBOSE;
break;
case 'p':
mode = SHOWJOBS_PGIDS;
break;
case 's':
mode = SHOWJOBS_PIDS;
break;
}
}

if (*argptr == NULL)
showjobs(0, mode);
else
while ((id = *argptr++) != NULL)
showjob(getjob(id), mode);

return (0);
}

static int getjobstatus(const struct job *jp)
{
int i, status;

if (!jp->pipefail)
return (jp->ps[jp->nprocs - 1].status);
for (i = jp->nprocs - 1; i >= 0; i--) {
status = jp->ps[i].status;
if (status != 0)
return (status);
}
return (0);
}

static void
printjobcmd(struct job *jp)
{
struct procstat *ps;
int i;

for (ps = jp->ps, i = jp->nprocs ; --i >= 0 ; ps++) {
out1str(ps->cmd);
if (i > 0)
out1str(" | ");
}
out1c('\n');
}

static void
showjob(struct job *jp, int mode)
{
char s[64];
char statebuf[16];
const char *statestr, *coredump;
struct procstat *ps;
struct job *j;
int col, curr, i, jobno, prev, procno, status;
char c;

procno = (mode == SHOWJOBS_PGIDS) ? 1 : jp->nprocs;
jobno = jp - jobtab + 1;
curr = prev = 0;
#if JOBS
if ((j = getcurjob(NULL)) != NULL) {
curr = j - jobtab + 1;
if ((j = getcurjob(j)) != NULL)
prev = j - jobtab + 1;
}
#endif
coredump = "";
status = getjobstatus(jp);
if (jp->state == 0) {
statestr = "Running";
#if JOBS
} else if (jp->state == JOBSTOPPED) {
ps = jp->ps + jp->nprocs - 1;
while (!WIFSTOPPED(ps->status) && ps > jp->ps)
ps--;
if (WIFSTOPPED(ps->status))
i = WSTOPSIG(ps->status);
else
i = -1;
statestr = strsignal(i);
if (statestr == NULL)
statestr = "Suspended";
#endif
} else if (WIFEXITED(status)) {
if (WEXITSTATUS(status) == 0)
statestr = "Done";
else {
fmtstr(statebuf, sizeof(statebuf), "Done(%d)",
WEXITSTATUS(status));
statestr = statebuf;
}
} else {
i = WTERMSIG(status);
statestr = strsignal(i);
if (statestr == NULL)
statestr = "Unknown signal";
if (WCOREDUMP(status))
coredump = " (core dumped)";
}

for (ps = jp->ps ; procno > 0 ; ps++, procno--) {
if (mode == SHOWJOBS_PIDS || mode == SHOWJOBS_PGIDS) {
out1fmt("%d\n", (int)ps->pid);
continue;
}
if (mode != SHOWJOBS_VERBOSE && ps != jp->ps)
continue;
if (jobno == curr && ps == jp->ps)
c = '+';
else if (jobno == prev && ps == jp->ps)
c = '-';
else
c = ' ';
if (ps == jp->ps)
fmtstr(s, 64, "[%d] %c ", jobno, c);
else
fmtstr(s, 64, " %c ", c);
out1str(s);
col = strlen(s);
if (mode == SHOWJOBS_VERBOSE) {
fmtstr(s, 64, "%d ", (int)ps->pid);
out1str(s);
col += strlen(s);
}
if (ps == jp->ps) {
out1str(statestr);
out1str(coredump);
col += strlen(statestr) + strlen(coredump);
}
do {
out1c(' ');
col++;
} while (col < 30);
if (mode == SHOWJOBS_VERBOSE) {
out1str(ps->cmd);
out1c('\n');
} else
printjobcmd(jp);
}
}










void
showjobs(int change, int mode)
{
int jobno;
struct job *jp;

TRACE(("showjobs(%d) called\n", change));
checkzombies();
for (jobno = 1, jp = jobtab ; jobno <= njobs ; jobno++, jp++) {
if (! jp->used)
continue;
if (jp->nprocs == 0) {
freejob(jp);
continue;
}
if (change && ! jp->changed)
continue;
showjob(jp, mode);
if (mode == SHOWJOBS_DEFAULT || mode == SHOWJOBS_VERBOSE) {
jp->changed = 0;



if (jp->state == JOBDONE && !jp->remembered &&
(iflag || jp != bgjob)) {
freejob(jp);
}
}
}
}






static void
freejob(struct job *jp)
{
struct procstat *ps;
int i;

INTOFF;
if (bgjob == jp)
bgjob = NULL;
for (i = jp->nprocs, ps = jp->ps ; --i >= 0 ; ps++) {
if (ps->cmd != nullstr)
ckfree(ps->cmd);
}
if (jp->ps != &jp->ps0)
ckfree(jp->ps);
jp->used = 0;
#if JOBS
deljob(jp);
#endif
INTON;
}



int
waitcmd(int argc __unused, char **argv __unused)
{
struct job *job;
int retval;

nextopt("");
if (*argptr == NULL)
return (waitcmdloop(NULL));

do {
job = getjob_nonotfound(*argptr);
if (job == NULL)
retval = 127;
else
retval = waitcmdloop(job);
argptr++;
} while (*argptr != NULL);

return (retval);
}

static int
waitcmdloop(struct job *job)
{
int status, retval, sig;
struct job *jp;






do {
if (job != NULL) {
if (job->state == JOBDONE) {
status = getjobstatus(job);
if (WIFEXITED(status))
retval = WEXITSTATUS(status);
else
retval = WTERMSIG(status) + 128;
if (! iflag || ! job->changed)
freejob(job);
else {
job->remembered = 0;
if (job == bgjob)
bgjob = NULL;
}
return retval;
}
} else {
for (jp = jobtab ; jp < jobtab + njobs; jp++)
if (jp->used && jp->state == JOBDONE) {
if (! iflag || ! jp->changed)
freejob(jp);
else {
jp->remembered = 0;
if (jp == bgjob)
bgjob = NULL;
}
}
for (jp = jobtab ; ; jp++) {
if (jp >= jobtab + njobs) {
return 0;
}
if (jp->used && jp->state == 0)
break;
}
}
} while (dowait(DOWAIT_BLOCK | DOWAIT_SIG, (struct job *)NULL) != -1);

sig = pendingsig_waitcmd;
pendingsig_waitcmd = 0;
return sig + 128;
}



int
jobidcmd(int argc __unused, char **argv __unused)
{
struct job *jp;
int i;

nextopt("");
jp = getjob(*argptr);
for (i = 0 ; i < jp->nprocs ; ) {
out1fmt("%d", (int)jp->ps[i].pid);
out1c(++i < jp->nprocs? ' ' : '\n');
}
return 0;
}







static struct job *
getjob_nonotfound(const char *name)
{
int jobno;
struct job *found, *jp;
size_t namelen;
pid_t pid;
int i;

if (name == NULL) {
#if JOBS
name = "%+";
#else
error("No current job");
#endif
}
if (name[0] == '%') {
if (is_digit(name[1])) {
jobno = number(name + 1);
if (jobno > 0 && jobno <= njobs
&& jobtab[jobno - 1].used != 0)
return &jobtab[jobno - 1];
#if JOBS
} else if ((name[1] == '%' || name[1] == '+') &&
name[2] == '\0') {
if ((jp = getcurjob(NULL)) == NULL)
error("No current job");
return (jp);
} else if (name[1] == '-' && name[2] == '\0') {
if ((jp = getcurjob(NULL)) == NULL ||
(jp = getcurjob(jp)) == NULL)
error("No previous job");
return (jp);
#endif
} else if (name[1] == '?') {
found = NULL;
for (jp = jobtab, i = njobs ; --i >= 0 ; jp++) {
if (jp->used && jp->nprocs > 0
&& strstr(jp->ps[0].cmd, name + 2) != NULL) {
if (found)
error("%s: ambiguous", name);
found = jp;
}
}
if (found != NULL)
return (found);
} else {
namelen = strlen(name);
found = NULL;
for (jp = jobtab, i = njobs ; --i >= 0 ; jp++) {
if (jp->used && jp->nprocs > 0
&& strncmp(jp->ps[0].cmd, name + 1,
namelen - 1) == 0) {
if (found)
error("%s: ambiguous", name);
found = jp;
}
}
if (found)
return found;
}
} else if (is_number(name)) {
pid = (pid_t)number(name);
for (jp = jobtab, i = njobs ; --i >= 0 ; jp++) {
if (jp->used && jp->nprocs > 0
&& jp->ps[jp->nprocs - 1].pid == pid)
return jp;
}
}
return NULL;
}


static struct job *
getjob(const char *name)
{
struct job *jp;

jp = getjob_nonotfound(name);
if (jp == NULL)
error("No such job: %s", name);
return (jp);
}


int
killjob(const char *name, int sig)
{
struct job *jp;
int i, ret;

jp = getjob(name);
if (jp->state == JOBDONE)
return 0;
if (jp->jobctl)
return kill(-jp->ps[0].pid, sig);
ret = -1;
errno = ESRCH;
for (i = 0; i < jp->nprocs; i++)
if (jp->ps[i].status == -1 || WIFSTOPPED(jp->ps[i].status)) {
if (kill(jp->ps[i].pid, sig) == 0)
ret = 0;
} else
ret = 0;
return ret;
}





struct job *
makejob(union node *node __unused, int nprocs)
{
int i;
struct job *jp;

for (i = njobs, jp = jobtab ; ; jp++) {
if (--i < 0) {
INTOFF;
if (njobs == 0) {
jobtab = ckmalloc(4 * sizeof jobtab[0]);
#if JOBS
jobmru = NULL;
#endif
} else {
jp = ckmalloc((njobs + 4) * sizeof jobtab[0]);
memcpy(jp, jobtab, njobs * sizeof jp[0]);
#if JOBS

if (jobmru != NULL)
jobmru = &jp[jobmru - jobtab];
for (i = 0; i < njobs; i++)
if (jp[i].next != NULL)
jp[i].next = &jp[jp[i].next -
jobtab];
#endif
if (bgjob != NULL)
bgjob = &jp[bgjob - jobtab];

for (i = 0; i < njobs; i++)
if (jp[i].ps == &jobtab[i].ps0)
jp[i].ps = &jp[i].ps0;
ckfree(jobtab);
jobtab = jp;
}
jp = jobtab + njobs;
for (i = 4 ; --i >= 0 ; jobtab[njobs++].used = 0)
;
INTON;
break;
}
if (jp->used == 0)
break;
}
INTOFF;
jp->state = 0;
jp->used = 1;
jp->changed = 0;
jp->nprocs = 0;
jp->foreground = 0;
jp->remembered = 0;
jp->pipefail = pipefailflag;
#if JOBS
jp->jobctl = jobctl;
jp->next = NULL;
#endif
if (nprocs > 1) {
jp->ps = ckmalloc(nprocs * sizeof (struct procstat));
} else {
jp->ps = &jp->ps0;
}
INTON;
TRACE(("makejob(%p, %d) returns %%%td\n", (void *)node, nprocs,
jp - jobtab + 1));
return jp;
}

#if JOBS
static void
setcurjob(struct job *cj)
{
struct job *jp, *prev;

for (prev = NULL, jp = jobmru; jp != NULL; prev = jp, jp = jp->next) {
if (jp == cj) {
if (prev != NULL)
prev->next = jp->next;
else
jobmru = jp->next;
jp->next = jobmru;
jobmru = cj;
return;
}
}
cj->next = jobmru;
jobmru = cj;
}

static void
deljob(struct job *j)
{
struct job *jp, *prev;

for (prev = NULL, jp = jobmru; jp != NULL; prev = jp, jp = jp->next) {
if (jp == j) {
if (prev != NULL)
prev->next = jp->next;
else
jobmru = jp->next;
return;
}
}
}





static struct job *
getcurjob(struct job *nj)
{
struct job *jp;


for (jp = jobmru; jp != NULL; jp = jp->next)
if (jp->used && jp != nj && jp->state == JOBSTOPPED)
return (jp);

for (jp = jobmru; jp != NULL; jp = jp->next)
if (jp->used && jp != nj)
return (jp);

return (NULL);
}

#endif
















pid_t
forkshell(struct job *jp, union node *n, int mode)
{
pid_t pid;
pid_t pgrp;

TRACE(("forkshell(%%%td, %p, %d) called\n", jp - jobtab, (void *)n,
mode));
INTOFF;
if (mode == FORK_BG && (jp == NULL || jp->nprocs == 0))
checkzombies();
flushall();
pid = fork();
if (pid == -1) {
TRACE(("Fork failed, errno=%d\n", errno));
INTON;
error("Cannot fork: %s", strerror(errno));
}
if (pid == 0) {
struct job *p;
int wasroot;
int i;

TRACE(("Child shell %d\n", (int)getpid()));
wasroot = rootshell;
rootshell = 0;
handler = &main_handler;
closescript();
INTON;
forcelocal = 0;
clear_traps();
#if JOBS
jobctl = 0;
if (wasroot && mode != FORK_NOJOB && mflag) {
if (jp == NULL || jp->nprocs == 0)
pgrp = getpid();
else
pgrp = jp->ps[0].pid;
if (setpgid(0, pgrp) == 0 && mode == FORK_FG &&
ttyfd >= 0) {






if (tcsetpgrp(ttyfd, pgrp) < 0)
error("tcsetpgrp failed, errno=%d", errno);
}
setsignal(SIGTSTP);
setsignal(SIGTTOU);
} else if (mode == FORK_BG) {
ignoresig(SIGINT);
ignoresig(SIGQUIT);
if ((jp == NULL || jp->nprocs == 0) &&
! fd0_redirected_p ()) {
close(0);
if (open(_PATH_DEVNULL, O_RDONLY) != 0)
error("cannot open %s: %s",
_PATH_DEVNULL, strerror(errno));
}
}
#else
if (mode == FORK_BG) {
ignoresig(SIGINT);
ignoresig(SIGQUIT);
if ((jp == NULL || jp->nprocs == 0) &&
! fd0_redirected_p ()) {
close(0);
if (open(_PATH_DEVNULL, O_RDONLY) != 0)
error("cannot open %s: %s",
_PATH_DEVNULL, strerror(errno));
}
}
#endif
INTOFF;
for (i = njobs, p = jobtab ; --i >= 0 ; p++)
if (p->used)
freejob(p);
INTON;
if (wasroot && iflag) {
setsignal(SIGINT);
setsignal(SIGQUIT);
setsignal(SIGTERM);
}
return pid;
}
if (rootshell && mode != FORK_NOJOB && mflag) {
if (jp == NULL || jp->nprocs == 0)
pgrp = pid;
else
pgrp = jp->ps[0].pid;
setpgid(pid, pgrp);
}
if (mode == FORK_BG) {
if (bgjob != NULL && bgjob->state == JOBDONE &&
!bgjob->remembered && !iflag)
freejob(bgjob);
backgndpid = pid;
bgjob = jp;
}
if (jp) {
struct procstat *ps = &jp->ps[jp->nprocs++];
ps->pid = pid;
ps->status = -1;
ps->cmd = nullstr;
if (iflag && rootshell && n)
ps->cmd = commandtext(n);
jp->foreground = mode == FORK_FG;
#if JOBS
setcurjob(jp);
#endif
}
INTON;
TRACE(("In parent shell: child = %d\n", (int)pid));
return pid;
}


pid_t
vforkexecshell(struct job *jp, char **argv, char **envp, const char *path, int idx, int pip[2])
{
pid_t pid;
struct jmploc jmploc;
struct jmploc *savehandler;
int inton;

TRACE(("vforkexecshell(%%%td, %s, %p) called\n", jp - jobtab, argv[0],
(void *)pip));
inton = is_int_on();
INTOFF;
flushall();
savehandler = handler;
pid = vfork();
if (pid == -1) {
TRACE(("Vfork failed, errno=%d\n", errno));
INTON;
error("Cannot fork: %s", strerror(errno));
}
if (pid == 0) {
TRACE(("Child shell %d\n", (int)getpid()));
if (setjmp(jmploc.loc))
_exit(exitstatus);
if (pip != NULL) {
close(pip[0]);
if (pip[1] != 1) {
dup2(pip[1], 1);
close(pip[1]);
}
}
handler = &jmploc;
shellexec(argv, envp, path, idx);
}
handler = savehandler;
if (jp) {
struct procstat *ps = &jp->ps[jp->nprocs++];
ps->pid = pid;
ps->status = -1;
ps->cmd = nullstr;
jp->foreground = 1;
#if JOBS
setcurjob(jp);
#endif
}
SETINTON(inton);
TRACE(("In parent shell: child = %d\n", (int)pid));
return pid;
}





















int
waitforjob(struct job *jp, int *signaled)
{
#if JOBS
int propagate_int = jp->jobctl && jp->foreground;
#endif
int status;
int st;

INTOFF;
TRACE(("waitforjob(%%%td) called\n", jp - jobtab + 1));
while (jp->state == 0)
if (dowait(DOWAIT_BLOCK | (Tflag ? DOWAIT_SIG |
DOWAIT_SIG_TRAP : 0), jp) == -1)
dotrap();
#if JOBS
if (jp->jobctl) {
if (ttyfd >= 0 && tcsetpgrp(ttyfd, rootpid) < 0)
error("tcsetpgrp failed, errno=%d\n", errno);
}
if (jp->state == JOBSTOPPED)
setcurjob(jp);
#endif
status = getjobstatus(jp);
if (signaled != NULL)
*signaled = WIFSIGNALED(status);

if (WIFEXITED(status))
st = WEXITSTATUS(status);
#if JOBS
else if (WIFSTOPPED(status))
st = WSTOPSIG(status) + 128;
#endif
else
st = WTERMSIG(status) + 128;
if (! JOBS || jp->state == JOBDONE)
freejob(jp);
if (int_pending()) {
if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGINT)
CLEAR_PENDING_INT;
}
#if JOBS
else if (rootshell && propagate_int &&
WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
kill(getpid(), SIGINT);
#endif
INTON;
return st;
}


static void
dummy_handler(int sig __unused)
{
}





static pid_t
dowait(int mode, struct job *job)
{
struct sigaction sa, osa;
sigset_t mask, omask;
pid_t pid;
int status;
struct procstat *sp;
struct job *jp;
struct job *thisjob;
const char *sigstr;
int done;
int stopped;
int sig;
int coredump;
int wflags;
int restore_sigchld;

TRACE(("dowait(%d, %p) called\n", mode, job));
restore_sigchld = 0;
if ((mode & DOWAIT_SIG) != 0) {
sigfillset(&mask);
sigprocmask(SIG_BLOCK, &mask, &omask);
INTOFF;
if (!issigchldtrapped()) {
restore_sigchld = 1;
sa.sa_handler = dummy_handler;
sa.sa_flags = 0;
sigemptyset(&sa.sa_mask);
sigaction(SIGCHLD, &sa, &osa);
}
}
do {
#if JOBS
if (iflag)
wflags = WUNTRACED | WCONTINUED;
else
#endif
wflags = 0;
if ((mode & (DOWAIT_BLOCK | DOWAIT_SIG)) != DOWAIT_BLOCK)
wflags |= WNOHANG;
pid = wait3(&status, wflags, (struct rusage *)NULL);
TRACE(("wait returns %d, status=%d\n", (int)pid, status));
if (pid == 0 && (mode & DOWAIT_SIG) != 0) {
pid = -1;
if (((mode & DOWAIT_SIG_TRAP) != 0 ?
pendingsig : pendingsig_waitcmd) != 0) {
errno = EINTR;
break;
}
sigsuspend(&omask);
if (int_pending())
break;
}
} while (pid == -1 && errno == EINTR);
if (pid == -1 && errno == ECHILD && job != NULL)
job->state = JOBDONE;
if ((mode & DOWAIT_SIG) != 0) {
if (restore_sigchld)
sigaction(SIGCHLD, &osa, NULL);
sigprocmask(SIG_SETMASK, &omask, NULL);
INTON;
}
if (pid <= 0)
return pid;
INTOFF;
thisjob = NULL;
for (jp = jobtab ; jp < jobtab + njobs ; jp++) {
if (jp->used && jp->nprocs > 0) {
done = 1;
stopped = 1;
for (sp = jp->ps ; sp < jp->ps + jp->nprocs ; sp++) {
if (sp->pid == -1)
continue;
if (sp->pid == pid && (sp->status == -1 ||
WIFSTOPPED(sp->status))) {
TRACE(("Changing status of proc %d from 0x%x to 0x%x\n",
(int)pid, sp->status,
status));
if (WIFCONTINUED(status)) {
sp->status = -1;
jp->state = 0;
} else
sp->status = status;
thisjob = jp;
}
if (sp->status == -1)
stopped = 0;
else if (WIFSTOPPED(sp->status))
done = 0;
}
if (stopped) {
int state = done? JOBDONE : JOBSTOPPED;
if (jp->state != state) {
TRACE(("Job %td: changing state from %d to %d\n", jp - jobtab + 1, jp->state, state));
jp->state = state;
if (jp != job) {
if (done && !jp->remembered &&
!iflag && jp != bgjob)
freejob(jp);
#if JOBS
else if (done)
deljob(jp);
#endif
}
}
}
}
}
INTON;
if (!thisjob || thisjob->state == 0)
;
else if ((!rootshell || !iflag || thisjob == job) &&
thisjob->foreground && thisjob->state != JOBSTOPPED) {
sig = 0;
coredump = 0;
for (sp = thisjob->ps; sp < thisjob->ps + thisjob->nprocs; sp++)
if (WIFSIGNALED(sp->status)) {
sig = WTERMSIG(sp->status);
coredump = WCOREDUMP(sp->status);
}
if (sig > 0 && sig != SIGINT && sig != SIGPIPE) {
sigstr = strsignal(sig);
if (sigstr != NULL)
out2str(sigstr);
else
out2str("Unknown signal");
if (coredump)
out2str(" (core dumped)");
out2c('\n');
flushout(out2);
}
} else {
TRACE(("Not printing status, rootshell=%d, job=%p\n", rootshell, job));
thisjob->changed = 1;
}
return pid;
}






int job_warning = 0;
int
stoppedjobs(void)
{
int jobno;
struct job *jp;

if (job_warning)
return (0);
for (jobno = 1, jp = jobtab; jobno <= njobs; jobno++, jp++) {
if (jp->used == 0)
continue;
if (jp->state == JOBSTOPPED) {
out2fmt_flush("You have stopped jobs.\n");
job_warning = 2;
return (1);
}
}

return (0);
}


static void
checkzombies(void)
{
while (njobs > 0 && dowait(0, NULL) > 0)
;
}


int
backgndpidset(void)
{
return backgndpid != -1;
}


pid_t
backgndpidval(void)
{
if (bgjob != NULL && !forcelocal)
bgjob->remembered = 1;
return backgndpid;
}






static char *cmdnextc;
static int cmdnleft;
#define MAXCMDTEXT 200

char *
commandtext(union node *n)
{
char *name;

cmdnextc = name = ckmalloc(MAXCMDTEXT);
cmdnleft = MAXCMDTEXT - 4;
cmdtxt(n);
*cmdnextc = '\0';
return name;
}


static void
cmdtxtdogroup(union node *n)
{
cmdputs("; do ");
cmdtxt(n);
cmdputs("; done");
}


static void
cmdtxtredir(union node *n, const char *op, int deffd)
{
char s[2];

if (n->nfile.fd != deffd) {
s[0] = n->nfile.fd + '0';
s[1] = '\0';
cmdputs(s);
}
cmdputs(op);
if (n->type == NTOFD || n->type == NFROMFD) {
if (n->ndup.dupfd >= 0)
s[0] = n->ndup.dupfd + '0';
else
s[0] = '-';
s[1] = '\0';
cmdputs(s);
} else {
cmdtxt(n->nfile.fname);
}
}


static void
cmdtxt(union node *n)
{
union node *np;
struct nodelist *lp;

if (n == NULL)
return;
switch (n->type) {
case NSEMI:
cmdtxt(n->nbinary.ch1);
cmdputs("; ");
cmdtxt(n->nbinary.ch2);
break;
case NAND:
cmdtxt(n->nbinary.ch1);
cmdputs(" && ");
cmdtxt(n->nbinary.ch2);
break;
case NOR:
cmdtxt(n->nbinary.ch1);
cmdputs(" || ");
cmdtxt(n->nbinary.ch2);
break;
case NPIPE:
for (lp = n->npipe.cmdlist ; lp ; lp = lp->next) {
cmdtxt(lp->n);
if (lp->next)
cmdputs(" | ");
}
break;
case NSUBSHELL:
cmdputs("(");
cmdtxt(n->nredir.n);
cmdputs(")");
break;
case NREDIR:
case NBACKGND:
cmdtxt(n->nredir.n);
break;
case NIF:
cmdputs("if ");
cmdtxt(n->nif.test);
cmdputs("; then ");
cmdtxt(n->nif.ifpart);
cmdputs("...");
break;
case NWHILE:
cmdputs("while ");
cmdtxt(n->nbinary.ch1);
cmdtxtdogroup(n->nbinary.ch2);
break;
case NUNTIL:
cmdputs("until ");
cmdtxt(n->nbinary.ch1);
cmdtxtdogroup(n->nbinary.ch2);
break;
case NFOR:
cmdputs("for ");
cmdputs(n->nfor.var);
cmdputs(" in ...");
break;
case NCASE:
cmdputs("case ");
cmdputs(n->ncase.expr->narg.text);
cmdputs(" in ...");
break;
case NDEFUN:
cmdputs(n->narg.text);
cmdputs("() ...");
break;
case NNOT:
cmdputs("! ");
cmdtxt(n->nnot.com);
break;
case NCMD:
for (np = n->ncmd.args ; np ; np = np->narg.next) {
cmdtxt(np);
if (np->narg.next)
cmdputs(" ");
}
for (np = n->ncmd.redirect ; np ; np = np->nfile.next) {
cmdputs(" ");
cmdtxt(np);
}
break;
case NARG:
cmdputs(n->narg.text);
break;
case NTO:
cmdtxtredir(n, ">", 1);
break;
case NAPPEND:
cmdtxtredir(n, ">>", 1);
break;
case NTOFD:
cmdtxtredir(n, ">&", 1);
break;
case NCLOBBER:
cmdtxtredir(n, ">|", 1);
break;
case NFROM:
cmdtxtredir(n, "<", 0);
break;
case NFROMTO:
cmdtxtredir(n, "<>", 0);
break;
case NFROMFD:
cmdtxtredir(n, "<&", 0);
break;
case NHERE:
case NXHERE:
cmdputs("<<...");
break;
default:
cmdputs("???");
break;
}
}



static void
cmdputs(const char *s)
{
const char *p;
char *q;
char c;
int subtype = 0;

if (cmdnleft <= 0)
return;
p = s;
q = cmdnextc;
while ((c = *p++) != '\0') {
if (c == CTLESC)
*q++ = *p++;
else if (c == CTLVAR) {
*q++ = '$';
if (--cmdnleft > 0)
*q++ = '{';
subtype = *p++;
if ((subtype & VSTYPE) == VSLENGTH && --cmdnleft > 0)
*q++ = '#';
} else if (c == '=' && subtype != 0) {
*q = "}-+?=##%%\0X"[(subtype & VSTYPE) - VSNORMAL];
if (*q)
q++;
else
cmdnleft++;
if (((subtype & VSTYPE) == VSTRIMLEFTMAX ||
(subtype & VSTYPE) == VSTRIMRIGHTMAX) &&
--cmdnleft > 0)
*q = q[-1], q++;
subtype = 0;
} else if (c == CTLENDVAR) {
*q++ = '}';
} else if (c == CTLBACKQ || c == CTLBACKQ+CTLQUOTE) {
cmdnleft -= 5;
if (cmdnleft > 0) {
*q++ = '$';
*q++ = '(';
*q++ = '.';
*q++ = '.';
*q++ = '.';
*q++ = ')';
}
} else if (c == CTLARI) {
cmdnleft -= 2;
if (cmdnleft > 0) {
*q++ = '$';
*q++ = '(';
*q++ = '(';
}
p++;
} else if (c == CTLENDARI) {
if (--cmdnleft > 0) {
*q++ = ')';
*q++ = ')';
}
} else if (c == CTLQUOTEMARK || c == CTLQUOTEEND)
cmdnleft++;
else
*q++ = c;
if (--cmdnleft <= 0) {
*q++ = '.';
*q++ = '.';
*q++ = '.';
break;
}
}
cmdnextc = q;
}
