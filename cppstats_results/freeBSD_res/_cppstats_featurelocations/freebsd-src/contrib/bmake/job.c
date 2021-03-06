



























































































































#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include "wait.h"

#include <errno.h>
#if !defined(USE_SELECT) && defined(HAVE_POLL_H)
#include <poll.h>
#else
#if !defined(USE_SELECT)
#define USE_SELECT
#endif
#if defined(HAVE_SYS_SELECT_H)
#include <sys/select.h>
#endif
#endif
#include <signal.h>
#include <utime.h>
#if defined(HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif

#include "make.h"
#include "dir.h"
#include "job.h"
#include "pathnames.h"
#include "trace.h"


MAKE_RCSID("$NetBSD: job.c,v 1.440 2021/11/28 19:51:06 rillig Exp $");



































typedef struct Shell {







const char *name;

bool hasEchoCtl;
const char *echoOff;
const char *echoOn;
const char *noPrint;

size_t noPrintLen;

bool hasErrCtl;

const char *errOn;
const char *errOff;

const char *echoTmpl;
const char *runIgnTmpl;

const char *runChkTmpl;




const char *newline;
char commentChar;

const char *echoFlag;
const char *errFlag;
} Shell;

typedef struct CommandFlags {

bool echo;


bool always;





bool ignerr;
} CommandFlags;







typedef struct ShellWriter {
FILE *f;


bool xtraced;

} ShellWriter;






#define MAKE_ALWAYS_PASS_JOB_QUEUE "${.MAKE.ALWAYS_PASS_JOB_QUEUE:U}"
static bool Always_pass_job_queue = true;






#define MAKE_JOB_ERROR_TOKEN "${MAKE_JOB_ERROR_TOKEN:U}"
static bool Job_error_token = true;




static int job_errors = 0;
static enum {
ABORT_NONE,
ABORT_ERROR,
ABORT_INTERRUPT,
ABORT_WAIT
} aborting = ABORT_NONE;
#define JOB_TOKENS "+EI+"




int jobTokensRunning = 0;

typedef enum JobStartResult {
JOB_RUNNING,
JOB_ERROR,
JOB_FINISHED
} JobStartResult;
















#if defined(DEFSHELL_CUSTOM)
#define DEFSHELL_INDEX_CUSTOM 0
#define DEFSHELL_INDEX_SH 1
#define DEFSHELL_INDEX_KSH 2
#define DEFSHELL_INDEX_CSH 3
#else
#define DEFSHELL_INDEX_SH 0
#define DEFSHELL_INDEX_KSH 1
#define DEFSHELL_INDEX_CSH 2
#endif

#if !defined(DEFSHELL_INDEX)
#define DEFSHELL_INDEX 0
#endif

static Shell shells[] = {
#if defined(DEFSHELL_CUSTOM)







{
DEFSHELL_CUSTOM,
false,
"",
"",
"",
0,
false,
"",
"",
"echo \"%s\"\n",
"%s\n",
"{ %s \n} || exit $?\n",
"'\n'",
'#',
"",
"",
},
#endif




{
"sh",
false,
"",
"",
"",
0,
false,
"",
"",
"echo \"%s\"\n",
"%s\n",
"{ %s \n} || exit $?\n",
"'\n'",
'#',
#if defined(MAKE_NATIVE) && defined(__NetBSD__)

"q",
#else
"",
#endif
"",
},



{
"ksh",
true,
"set +v",
"set -v",
"set +v",
6,
false,
"",
"",
"echo \"%s\"\n",
"%s\n",
"{ %s \n} || exit $?\n",
"'\n'",
'#',
"v",
"",
},





{
"csh",
true,
"unset verbose",
"set verbose",
"unset verbose",
13,
false,
"",
"",
"echo \"%s\"\n",
"csh -c \"%s || exit 0\"\n",
"",
"'\\\n'",
'#',
"v",
"e",
}
};





static Shell *shell = &shells[DEFSHELL_INDEX];
const char *shellPath = NULL;
const char *shellName = NULL;
char *shellErrFlag = NULL;
static char *shell_freeIt = NULL;


static Job *job_table;
static Job *job_table_end;
static unsigned int wantToken;
static bool lurking_children = false;
static bool make_suspended = false;





static struct pollfd *fds = NULL;
static Job **jobByFdIndex = NULL;
static nfds_t fdsLen = 0;
static void watchfd(Job *);
static void clearfd(Job *);
static bool readyfd(Job *);

static char *targPrefix = NULL;
static Job tokenWaitJob;

static Job childExitJob;
#define CHILD_EXIT "."
#define DO_JOB_RESUME "R"

enum {
npseudojobs = 2
};

static sigset_t caught_signals;
static volatile sig_atomic_t caught_sigchld;

static void CollectOutput(Job *, bool);
static void JobInterrupt(bool, int) MAKE_ATTR_DEAD;
static void JobRestartJobs(void);
static void JobSigReset(void);

static void
SwitchOutputTo(GNode *gn)
{

static GNode *lastNode = NULL;

if (gn == lastNode)
return;
lastNode = gn;

if (opts.maxJobs != 1 && targPrefix != NULL && targPrefix[0] != '\0')
(void)fprintf(stdout, "%s %s ---\n", targPrefix, gn->name);
}

static unsigned
nfds_per_job(void)
{
#if defined(USE_FILEMON) && !defined(USE_FILEMON_DEV)
if (useMeta)
return 2;
#endif
return 1;
}

void
Job_FlagsToString(const Job *job, char *buf, size_t bufsize)
{
snprintf(buf, bufsize, "%c%c%c",
job->ignerr ? 'i' : '-',
!job->echo ? 's' : '-',
job->special ? 'S' : '-');
}

static void
DumpJobs(const char *where)
{
Job *job;
char flags[4];

debug_printf("job table @ %s\n", where);
for (job = job_table; job < job_table_end; job++) {
Job_FlagsToString(job, flags, sizeof flags);
debug_printf("job %d, status %d, flags %s, pid %d\n",
(int)(job - job_table), job->status, flags, job->pid);
}
}





static void
JobDeleteTarget(GNode *gn)
{
const char *file;

if (gn->type & OP_JOIN)
return;
if (gn->type & OP_PHONY)
return;
if (Targ_Precious(gn))
return;
if (opts.noExecute)
return;

file = GNode_Path(gn);
if (eunlink(file) != -1)
Error("*** %s removed", file);
}







static void
JobSigLock(sigset_t *omaskp)
{
if (sigprocmask(SIG_BLOCK, &caught_signals, omaskp) != 0) {
Punt("JobSigLock: sigprocmask: %s", strerror(errno));
sigemptyset(omaskp);
}
}

static void
JobSigUnlock(sigset_t *omaskp)
{
(void)sigprocmask(SIG_SETMASK, omaskp, NULL);
}

static void
JobCreatePipe(Job *job, int minfd)
{
int i, fd, flags;
int pipe_fds[2];

if (pipe(pipe_fds) == -1)
Punt("Cannot create pipe: %s", strerror(errno));

for (i = 0; i < 2; i++) {

fd = fcntl(pipe_fds[i], F_DUPFD, minfd);
if (fd != -1) {
close(pipe_fds[i]);
pipe_fds[i] = fd;
}
}

job->inPipe = pipe_fds[0];
job->outPipe = pipe_fds[1];


if (fcntl(job->inPipe, F_SETFD, FD_CLOEXEC) == -1)
Punt("Cannot set close-on-exec: %s", strerror(errno));
if (fcntl(job->outPipe, F_SETFD, FD_CLOEXEC) == -1)
Punt("Cannot set close-on-exec: %s", strerror(errno));







flags = fcntl(job->inPipe, F_GETFL, 0);
if (flags == -1)
Punt("Cannot get flags: %s", strerror(errno));
flags |= O_NONBLOCK;
if (fcntl(job->inPipe, F_SETFL, flags) == -1)
Punt("Cannot set flags: %s", strerror(errno));
}


static void
JobCondPassSig(int signo)
{
Job *job;

DEBUG1(JOB, "JobCondPassSig(%d) called.\n", signo);

for (job = job_table; job < job_table_end; job++) {
if (job->status != JOB_ST_RUNNING)
continue;
DEBUG2(JOB, "JobCondPassSig passing signal %d to child %d.\n",
signo, job->pid);
KILLPG(job->pid, signo);
}
}







static void
JobChildSig(int signo MAKE_ATTR_UNUSED)
{
caught_sigchld = 1;
while (write(childExitJob.outPipe, CHILD_EXIT, 1) == -1 &&
errno == EAGAIN)
continue;
}




static void
JobContinueSig(int signo MAKE_ATTR_UNUSED)
{




while (write(childExitJob.outPipe, DO_JOB_RESUME, 1) == -1 &&
errno == EAGAIN)
continue;
}





MAKE_ATTR_DEAD static void
JobPassSig_int(int signo)
{

JobInterrupt(true, signo);
}





MAKE_ATTR_DEAD static void
JobPassSig_term(int signo)
{

JobInterrupt(false, signo);
}

static void
JobPassSig_suspend(int signo)
{
sigset_t nmask, omask;
struct sigaction act;


make_suspended = true;


JobCondPassSig(signo);







sigfillset(&nmask);
sigdelset(&nmask, signo);
(void)sigprocmask(SIG_SETMASK, &nmask, &omask);

act.sa_handler = SIG_DFL;
sigemptyset(&act.sa_mask);
act.sa_flags = 0;
(void)sigaction(signo, &act, NULL);

DEBUG1(JOB, "JobPassSig passing signal %d to self.\n", signo);

(void)kill(getpid(), signo);




















act.sa_handler = JobPassSig_suspend;
(void)sigaction(signo, &act, NULL);
(void)sigprocmask(SIG_SETMASK, &omask, NULL);
}

static Job *
JobFindPid(int pid, JobStatus status, bool isJobs)
{
Job *job;

for (job = job_table; job < job_table_end; job++) {
if (job->status == status && job->pid == pid)
return job;
}
if (DEBUG(JOB) && isJobs)
DumpJobs("no pid");
return NULL;
}


static void
ParseCommandFlags(char **pp, CommandFlags *out_cmdFlags)
{
char *p = *pp;
out_cmdFlags->echo = true;
out_cmdFlags->ignerr = false;
out_cmdFlags->always = false;

for (;;) {
if (*p == '@')
out_cmdFlags->echo = DEBUG(LOUD);
else if (*p == '-')
out_cmdFlags->ignerr = true;
else if (*p == '+')
out_cmdFlags->always = true;
else
break;
p++;
}

pp_skip_whitespace(&p);

*pp = p;
}


static char *
EscapeShellDblQuot(const char *cmd)
{
size_t i, j;


char *esc = bmake_malloc(strlen(cmd) * 2 + 1);
for (i = 0, j = 0; cmd[i] != '\0'; i++, j++) {
if (cmd[i] == '$' || cmd[i] == '`' || cmd[i] == '\\' ||
cmd[i] == '"')
esc[j++] = '\\';
esc[j] = cmd[i];
}
esc[j] = '\0';

return esc;
}

static void
ShellWriter_WriteFmt(ShellWriter *wr, const char *fmt, const char *arg)
{
DEBUG1(JOB, fmt, arg);

(void)fprintf(wr->f, fmt, arg);
if (wr->f == stdout)
(void)fflush(wr->f);
}

static void
ShellWriter_WriteLine(ShellWriter *wr, const char *line)
{
ShellWriter_WriteFmt(wr, "%s\n", line);
}

static void
ShellWriter_EchoOff(ShellWriter *wr)
{
if (shell->hasEchoCtl)
ShellWriter_WriteLine(wr, shell->echoOff);
}

static void
ShellWriter_EchoCmd(ShellWriter *wr, const char *escCmd)
{
ShellWriter_WriteFmt(wr, shell->echoTmpl, escCmd);
}

static void
ShellWriter_EchoOn(ShellWriter *wr)
{
if (shell->hasEchoCtl)
ShellWriter_WriteLine(wr, shell->echoOn);
}

static void
ShellWriter_TraceOn(ShellWriter *wr)
{
if (!wr->xtraced) {
ShellWriter_WriteLine(wr, "set -x");
wr->xtraced = true;
}
}

static void
ShellWriter_ErrOff(ShellWriter *wr, bool echo)
{
if (echo)
ShellWriter_EchoOff(wr);
ShellWriter_WriteLine(wr, shell->errOff);
if (echo)
ShellWriter_EchoOn(wr);
}

static void
ShellWriter_ErrOn(ShellWriter *wr, bool echo)
{
if (echo)
ShellWriter_EchoOff(wr);
ShellWriter_WriteLine(wr, shell->errOn);
if (echo)
ShellWriter_EchoOn(wr);
}






static void
JobWriteSpecialsEchoCtl(Job *job, ShellWriter *wr, CommandFlags *inout_cmdFlags,
const char *escCmd, const char **inout_cmdTemplate)
{

job->ignerr = true;

if (job->echo && inout_cmdFlags->echo) {
ShellWriter_EchoOff(wr);
ShellWriter_EchoCmd(wr, escCmd);





inout_cmdFlags->echo = false;
} else {
if (inout_cmdFlags->echo)
ShellWriter_EchoCmd(wr, escCmd);
}
*inout_cmdTemplate = shell->runIgnTmpl;






inout_cmdFlags->ignerr = false;
}

static void
JobWriteSpecials(Job *job, ShellWriter *wr, const char *escCmd, bool run,
CommandFlags *inout_cmdFlags, const char **inout_cmdTemplate)
{
if (!run) {




inout_cmdFlags->ignerr = false;
} else if (shell->hasErrCtl)
ShellWriter_ErrOff(wr, job->echo && inout_cmdFlags->echo);
else if (shell->runIgnTmpl != NULL && shell->runIgnTmpl[0] != '\0') {
JobWriteSpecialsEchoCtl(job, wr, inout_cmdFlags, escCmd,
inout_cmdTemplate);
} else
inout_cmdFlags->ignerr = false;
}




















static void
JobWriteCommand(Job *job, ShellWriter *wr, StringListNode *ln, const char *ucmd)
{
bool run;

CommandFlags cmdFlags;

const char *cmdTemplate;
char *xcmd;
char *xcmdStart;
char *escCmd;

run = GNode_ShouldExecute(job->node);

Var_Subst(ucmd, job->node, VARE_WANTRES, &xcmd);

xcmdStart = xcmd;

cmdTemplate = "%s\n";

ParseCommandFlags(&xcmd, &cmdFlags);


if (cmdFlags.always && !run) {




Compat_RunCommand(ucmd, job->node, ln);
free(xcmdStart);
return;
}






escCmd = shell->hasErrCtl ? NULL : EscapeShellDblQuot(xcmd);

if (!cmdFlags.echo) {
if (job->echo && run && shell->hasEchoCtl) {
ShellWriter_EchoOff(wr);
} else {
if (shell->hasErrCtl)
cmdFlags.echo = true;
}
}

if (cmdFlags.ignerr) {
JobWriteSpecials(job, wr, escCmd, run, &cmdFlags, &cmdTemplate);
} else {







if (!shell->hasErrCtl && shell->runChkTmpl != NULL &&
shell->runChkTmpl[0] != '\0') {
if (job->echo && cmdFlags.echo) {
ShellWriter_EchoOff(wr);
ShellWriter_EchoCmd(wr, escCmd);
cmdFlags.echo = false;
}




cmdTemplate = escCmd[0] == shell->commentChar ||
escCmd[0] == '\0'
? shell->runIgnTmpl
: shell->runChkTmpl;
cmdFlags.ignerr = false;
}
}

if (DEBUG(SHELL) && strcmp(shellName, "sh") == 0)
ShellWriter_TraceOn(wr);

ShellWriter_WriteFmt(wr, cmdTemplate, xcmd);
free(xcmdStart);
free(escCmd);

if (cmdFlags.ignerr)
ShellWriter_ErrOn(wr, cmdFlags.echo && job->echo);

if (!cmdFlags.echo)
ShellWriter_EchoOn(wr);
}









static bool
JobWriteCommands(Job *job)
{
StringListNode *ln;
bool seen = false;
ShellWriter wr;

wr.f = job->cmdFILE;
wr.xtraced = false;

for (ln = job->node->commands.first; ln != NULL; ln = ln->next) {
const char *cmd = ln->datum;

if (strcmp(cmd, "...") == 0) {
job->node->type |= OP_SAVE_CMDS;
job->tailCmds = ln->next;
break;
}

JobWriteCommand(job, &wr, ln, ln->datum);
seen = true;
}

return seen;
}





static void
JobSaveCommands(Job *job)
{
StringListNode *ln;

for (ln = job->tailCmds; ln != NULL; ln = ln->next) {
const char *cmd = ln->datum;
char *expanded_cmd;





(void)Var_Subst(cmd, job->node, VARE_WANTRES, &expanded_cmd);

Lst_Append(&Targ_GetEndNode()->commands, expanded_cmd);
}
}



static void
JobClosePipes(Job *job)
{
clearfd(job);
(void)close(job->outPipe);
job->outPipe = -1;

CollectOutput(job, true);
(void)close(job->inPipe);
job->inPipe = -1;
}

static void
DebugFailedJob(const Job *job)
{
const StringListNode *ln;

if (!DEBUG(ERROR))
return;

debug_printf("\n");
debug_printf("*** Failed target: %s\n", job->node->name);
debug_printf("*** Failed commands:\n");
for (ln = job->node->commands.first; ln != NULL; ln = ln->next) {
const char *cmd = ln->datum;
debug_printf("\t%s\n", cmd);

if (strchr(cmd, '$') != NULL) {
char *xcmd;
(void)Var_Subst(cmd, job->node, VARE_WANTRES, &xcmd);
debug_printf("\t=> %s\n", xcmd);
free(xcmd);
}
}
}

static void
JobFinishDoneExitedError(Job *job, WAIT_T *inout_status)
{
SwitchOutputTo(job->node);
#if defined(USE_META)
if (useMeta) {
meta_job_error(job, job->node,
job->ignerr, WEXITSTATUS(*inout_status));
}
#endif
if (!shouldDieQuietly(job->node, -1)) {
DebugFailedJob(job);
(void)printf("*** [%s] Error code %d%s\n",
job->node->name, WEXITSTATUS(*inout_status),
job->ignerr ? " (ignored)" : "");
}

if (job->ignerr)
WAIT_STATUS(*inout_status) = 0;
else {
if (deleteOnError)
JobDeleteTarget(job->node);
PrintOnError(job->node, NULL);
}
}

static void
JobFinishDoneExited(Job *job, WAIT_T *inout_status)
{
DEBUG2(JOB, "Process %d [%s] exited.\n", job->pid, job->node->name);

if (WEXITSTATUS(*inout_status) != 0)
JobFinishDoneExitedError(job, inout_status);
else if (DEBUG(JOB)) {
SwitchOutputTo(job->node);
(void)printf("*** [%s] Completed successfully\n",
job->node->name);
}
}

static void
JobFinishDoneSignaled(Job *job, WAIT_T status)
{
SwitchOutputTo(job->node);
DebugFailedJob(job);
(void)printf("*** [%s] Signal %d\n", job->node->name, WTERMSIG(status));
if (deleteOnError)
JobDeleteTarget(job->node);
}

static void
JobFinishDone(Job *job, WAIT_T *inout_status)
{
if (WIFEXITED(*inout_status))
JobFinishDoneExited(job, inout_status);
else
JobFinishDoneSignaled(job, *inout_status);

(void)fflush(stdout);
}














static void
JobFinish (Job *job, WAIT_T status)
{
bool done, return_job_token;

DEBUG3(JOB, "JobFinish: %d [%s], status %d\n",
job->pid, job->node->name, status);

if ((WIFEXITED(status) &&
((WEXITSTATUS(status) != 0 && !job->ignerr))) ||
WIFSIGNALED(status)) {


JobClosePipes(job);
if (job->cmdFILE != NULL && job->cmdFILE != stdout) {
if (fclose(job->cmdFILE) != 0)
Punt("Cannot write shell script for '%s': %s",
job->node->name, strerror(errno));
job->cmdFILE = NULL;
}
done = true;

} else if (WIFEXITED(status)) {





done = WEXITSTATUS(status) != 0;

JobClosePipes(job);

} else {

done = false;
}

if (done)
JobFinishDone(job, &status);

#if defined(USE_META)
if (useMeta) {
int meta_status = meta_job_finish(job);
if (meta_status != 0 && status == 0)
status = meta_status;
}
#endif

return_job_token = false;

Trace_Log(JOBEND, job);
if (!job->special) {
if (WAIT_STATUS(status) != 0 ||
(aborting == ABORT_ERROR) || aborting == ABORT_INTERRUPT)
return_job_token = true;
}

if (aborting != ABORT_ERROR && aborting != ABORT_INTERRUPT &&
(WAIT_STATUS(status) == 0)) {





JobSaveCommands(job);
job->node->made = MADE;
if (!job->special)
return_job_token = true;
Make_Update(job->node);
job->status = JOB_ST_FREE;
} else if (status != 0) {
job_errors++;
job->status = JOB_ST_FREE;
}

if (job_errors > 0 && !opts.keepgoing && aborting != ABORT_INTERRUPT) {

aborting = ABORT_ERROR;
}

if (return_job_token)
Job_TokenReturn();

if (aborting == ABORT_ERROR && jobTokensRunning == 0)
Finish(job_errors);
}

static void
TouchRegular(GNode *gn)
{
const char *file = GNode_Path(gn);
struct utimbuf times;
int fd;
char c;

times.actime = now;
times.modtime = now;
if (utime(file, &times) >= 0)
return;

fd = open(file, O_RDWR | O_CREAT, 0666);
if (fd < 0) {
(void)fprintf(stderr, "*** couldn't touch %s: %s\n",
file, strerror(errno));
(void)fflush(stderr);
return;
}




if (read(fd, &c, 1) == 1) {
(void)lseek(fd, 0, SEEK_SET);
while (write(fd, &c, 1) == -1 && errno == EAGAIN)
continue;
}
(void)close(fd);
}







void
Job_Touch(GNode *gn, bool echo)
{
if (gn->type &
(OP_JOIN | OP_USE | OP_USEBEFORE | OP_EXEC | OP_OPTIONAL |
OP_SPECIAL | OP_PHONY)) {




return;
}

if (echo || !GNode_ShouldExecute(gn)) {
(void)fprintf(stdout, "touch %s\n", gn->name);
(void)fflush(stdout);
}

if (!GNode_ShouldExecute(gn))
return;

if (gn->type & OP_ARCHV)
Arch_Touch(gn);
else if (gn->type & OP_LIB)
Arch_TouchLib(gn);
else
TouchRegular(gn);
}














bool
Job_CheckCommands(GNode *gn, void (*abortProc)(const char *, ...))
{
if (GNode_IsTarget(gn))
return true;
if (!Lst_IsEmpty(&gn->commands))
return true;
if ((gn->type & OP_LIB) && !Lst_IsEmpty(&gn->children))
return true;





if (defaultNode != NULL && !Lst_IsEmpty(&defaultNode->commands) &&
!(gn->type & OP_SPECIAL)) {









Make_HandleUse(defaultNode, gn);
Var_Set(gn, IMPSRC, GNode_VarTarget(gn));
return true;
}

Dir_UpdateMTime(gn, false);
if (gn->mtime != 0 || (gn->type & OP_SPECIAL))
return true;









if (gn->flags.fromDepend) {
if (!Job_RunTarget(".STALE", gn->fname))
fprintf(stdout,
"%s: %s, %d: ignoring stale %s for %s\n",
progname, gn->fname, gn->lineno, makeDependfile,
gn->name);
return true;
}

if (gn->type & OP_OPTIONAL) {
(void)fprintf(stdout, "%s: don't know how to make %s (%s)\n",
progname, gn->name, "ignored");
(void)fflush(stdout);
return true;
}

if (opts.keepgoing) {
(void)fprintf(stdout, "%s: don't know how to make %s (%s)\n",
progname, gn->name, "continuing");
(void)fflush(stdout);
return false;
}

abortProc("%s: don't know how to make %s. Stop", progname, gn->name);
return false;
}






static void
JobExec(Job *job, char **argv)
{
int cpid;
sigset_t mask;

if (DEBUG(JOB)) {
int i;

debug_printf("Running %s\n", job->node->name);
debug_printf("\tCommand: ");
for (i = 0; argv[i] != NULL; i++) {
debug_printf("%s ", argv[i]);
}
debug_printf("\n");
}







if (job->echo)
SwitchOutputTo(job->node);


JobSigLock(&mask);


job->status = JOB_ST_RUNNING;

Var_ReexportVars();

cpid = vfork();
if (cpid == -1)
Punt("Cannot vfork: %s", strerror(errno));

if (cpid == 0) {

sigset_t tmask;

#if defined(USE_META)
if (useMeta) {
meta_job_child(job);
}
#endif




JobSigReset();


sigemptyset(&tmask);
JobSigUnlock(&tmask);







if (dup2(fileno(job->cmdFILE), 0) == -1)
execDie("dup2", "job->cmdFILE");
if (fcntl(0, F_SETFD, 0) == -1)
execDie("fcntl clear close-on-exec", "stdin");
if (lseek(0, 0, SEEK_SET) == -1)
execDie("lseek to 0", "stdin");

if (Always_pass_job_queue ||
(job->node->type & (OP_MAKE | OP_SUBMAKE))) {



if (fcntl(tokenWaitJob.inPipe, F_SETFD, 0) == -1)
execDie("clear close-on-exec",
"tokenWaitJob.inPipe");
if (fcntl(tokenWaitJob.outPipe, F_SETFD, 0) == -1)
execDie("clear close-on-exec",
"tokenWaitJob.outPipe");
}





if (dup2(job->outPipe, 1) == -1)
execDie("dup2", "job->outPipe");







if (fcntl(1, F_SETFD, 0) == -1)
execDie("clear close-on-exec", "stdout");
if (dup2(1, 2) == -1)
execDie("dup2", "1, 2");







#if defined(HAVE_SETPGID)
(void)setpgid(0, getpid());
#else
#if defined(HAVE_SETSID)

(void)setsid();
#else
(void)setpgrp(0, getpid());
#endif
#endif

(void)execv(shellPath, argv);
execDie("exec", shellPath);
}


job->pid = cpid;

Trace_Log(JOBSTART, job);

#if defined(USE_META)
if (useMeta) {
meta_job_parent(job, cpid);
}
#endif





job->curPos = 0;

watchfd(job);

if (job->cmdFILE != NULL && job->cmdFILE != stdout) {
if (fclose(job->cmdFILE) != 0)
Punt("Cannot write shell script for '%s': %s",
job->node->name, strerror(errno));
job->cmdFILE = NULL;
}


if (DEBUG(JOB)) {
debug_printf("JobExec(%s): pid %d added to jobs table\n",
job->node->name, job->pid);
DumpJobs("job started");
}
JobSigUnlock(&mask);
}


static void
JobMakeArgv(Job *job, char **argv)
{
int argc;
static char args[10];

argv[0] = UNCONST(shellName);
argc = 1;

if ((shell->errFlag != NULL && shell->errFlag[0] != '-') ||
(shell->echoFlag != NULL && shell->echoFlag[0] != '-')) {










(void)snprintf(args, sizeof args, "-%s%s",
(job->ignerr ? "" :
(shell->errFlag != NULL ? shell->errFlag : "")),
(!job->echo ? "" :
(shell->echoFlag != NULL ? shell->echoFlag : "")));

if (args[1] != '\0') {
argv[argc] = args;
argc++;
}
} else {
if (!job->ignerr && shell->errFlag != NULL) {
argv[argc] = UNCONST(shell->errFlag);
argc++;
}
if (job->echo && shell->echoFlag != NULL) {
argv[argc] = UNCONST(shell->echoFlag);
argc++;
}
}
argv[argc] = NULL;
}

static void
JobWriteShellCommands(Job *job, GNode *gn, bool *out_run)
{





char tfile[MAXPATHLEN];
int tfd;

tfd = Job_TempFile(TMPPAT, tfile, sizeof tfile);

job->cmdFILE = fdopen(tfd, "w+");
if (job->cmdFILE == NULL)
Punt("Could not fdopen %s", tfile);

(void)fcntl(fileno(job->cmdFILE), F_SETFD, FD_CLOEXEC);

#if defined(USE_META)
if (useMeta) {
meta_job_start(job, gn);
if (gn->type & OP_SILENT)
job->echo = false;
}
#endif

*out_run = JobWriteCommands(job);
}















static JobStartResult
JobStart(GNode *gn, bool special)
{
Job *job;
char *argv[10];
bool cmdsOK;
bool run;

for (job = job_table; job < job_table_end; job++) {
if (job->status == JOB_ST_FREE)
break;
}
if (job >= job_table_end)
Punt("JobStart no job slots vacant");

memset(job, 0, sizeof *job);
job->node = gn;
job->tailCmds = NULL;
job->status = JOB_ST_SET_UP;

job->special = special || gn->type & OP_SPECIAL;
job->ignerr = opts.ignoreErrors || gn->type & OP_IGNORE;
job->echo = !(opts.beSilent || gn->type & OP_SILENT);





cmdsOK = Job_CheckCommands(gn, Error);

job->inPollfd = NULL;

if (Lst_IsEmpty(&gn->commands)) {
job->cmdFILE = stdout;
run = false;





if (!cmdsOK) {
PrintOnError(gn, NULL);
DieHorribly();
}
} else if (((gn->type & OP_MAKE) && !opts.noRecursiveExecute) ||
(!opts.noExecute && !opts.touchFlag)) {











if (!cmdsOK) {
PrintOnError(gn, NULL);
DieHorribly();
}

JobWriteShellCommands(job, gn, &run);
(void)fflush(job->cmdFILE);
} else if (!GNode_ShouldExecute(gn)) {




SwitchOutputTo(gn);
job->cmdFILE = stdout;
if (cmdsOK)
JobWriteCommands(job);
run = false;
(void)fflush(job->cmdFILE);
} else {
Job_Touch(gn, job->echo);
run = false;
}


if (!run) {
if (!job->special)
Job_TokenReturn();

if (job->cmdFILE != NULL && job->cmdFILE != stdout) {
(void)fclose(job->cmdFILE);
job->cmdFILE = NULL;
}





if (cmdsOK && aborting == ABORT_NONE) {
JobSaveCommands(job);
job->node->made = MADE;
Make_Update(job->node);
}
job->status = JOB_ST_FREE;
return cmdsOK ? JOB_FINISHED : JOB_ERROR;
}





JobMakeArgv(job, argv);


JobCreatePipe(job, 3);

JobExec(job, argv);
return JOB_RUNNING;
}









static char *
PrintFilteredOutput(char *cp, char *endp)
{
char *ecp;

if (shell->noPrint == NULL || shell->noPrint[0] == '\0')
return cp;







while ((ecp = strstr(cp, shell->noPrint)) != NULL) {
if (ecp != cp) {
*ecp = '\0';







(void)fprintf(stdout, "%s", cp);
(void)fflush(stdout);
}
cp = ecp + shell->noPrintLen;
if (cp == endp)
break;
cp++;
pp_skip_whitespace(&cp);
}
return cp;
}
















static void
CollectOutput(Job *job, bool finish)
{
bool gotNL;
bool fbuf;
size_t nr;
size_t i;
size_t max;
ssize_t nRead;


again:
gotNL = false;
fbuf = false;

nRead = read(job->inPipe, &job->outBuf[job->curPos],
JOB_BUFSIZE - job->curPos);
if (nRead < 0) {
if (errno == EAGAIN)
return;
if (DEBUG(JOB)) {
perror("CollectOutput(piperead)");
}
nr = 0;
} else {
nr = (size_t)nRead;
}







if (nr == 0 && job->curPos != 0) {
job->outBuf[job->curPos] = '\n';
nr = 1;
finish = false;
} else if (nr == 0) {
finish = false;
}






max = job->curPos + nr;
for (i = job->curPos + nr - 1;
i >= job->curPos && i != (size_t)-1; i--) {
if (job->outBuf[i] == '\n') {
gotNL = true;
break;
} else if (job->outBuf[i] == '\0') {





job->outBuf[i] = ' ';
}
}

if (!gotNL) {
job->curPos += nr;
if (job->curPos == JOB_BUFSIZE) {




fbuf = true;
i = job->curPos;
}
}
if (gotNL || fbuf) {










job->outBuf[i] = '\0';
if (i >= job->curPos) {
char *cp;







cp = PrintFilteredOutput(job->outBuf, &job->outBuf[i]);






if (*cp != '\0') {
if (!opts.beSilent)
SwitchOutputTo(job->node);
#if defined(USE_META)
if (useMeta) {
meta_job_output(job, cp,
gotNL ? "\n" : "");
}
#endif
(void)fprintf(stdout, "%s%s", cp,
gotNL ? "\n" : "");
(void)fflush(stdout);
}
}





if (i < max) {
(void)memmove(job->outBuf, &job->outBuf[i + 1],
max - (i + 1));
job->curPos = max - (i + 1);
} else {
assert(i == max);
job->curPos = 0;
}
}
if (finish) {








goto again;
}
}

static void
JobRun(GNode *targ)
{
#if 0








GNodeList lst = LST_INIT;
Lst_Append(&lst, targ);
(void)Make_Run(&lst);
Lst_Done(&lst);
JobStart(targ, true);
while (jobTokensRunning != 0) {
Job_CatchOutput();
}
#else
Compat_Make(targ, targ);

if (targ->made == ERROR) {
PrintOnError(targ, "\n\nStop.");
exit(1);
}
#endif
}











void
Job_CatchChildren(void)
{
int pid;
WAIT_T status;


if (jobTokensRunning == 0)
return;


if (caught_sigchld == 0)
return;
caught_sigchld = 0;

while ((pid = waitpid((pid_t)-1, &status, WNOHANG | WUNTRACED)) > 0) {
DEBUG2(JOB, "Process %d exited/stopped status %x.\n",
pid, WAIT_STATUS(status));
JobReapChild(pid, status, true);
}
}





void
JobReapChild(pid_t pid, WAIT_T status, bool isJobs)
{
Job *job;


if (jobTokensRunning == 0)
return;

job = JobFindPid(pid, JOB_ST_RUNNING, isJobs);
if (job == NULL) {
if (isJobs) {
if (!lurking_children)
Error("Child (%d) status %x not in table?",
pid, status);
}
return;
}
if (WIFSTOPPED(status)) {
DEBUG2(JOB, "Process %d (%s) stopped.\n",
job->pid, job->node->name);
if (!make_suspended) {
switch (WSTOPSIG(status)) {
case SIGTSTP:
(void)printf("*** [%s] Suspended\n",
job->node->name);
break;
case SIGSTOP:
(void)printf("*** [%s] Stopped\n",
job->node->name);
break;
default:
(void)printf("*** [%s] Stopped -- signal %d\n",
job->node->name, WSTOPSIG(status));
}
job->suspended = true;
}
(void)fflush(stdout);
return;
}

job->status = JOB_ST_FINISHED;
job->exit_status = WAIT_STATUS(status);

JobFinish(job, status);
}







void
Job_CatchOutput(void)
{
int nready;
Job *job;
unsigned int i;

(void)fflush(stdout);


do {
nready = poll(fds + 1 - wantToken, fdsLen - 1 + wantToken,
POLL_MSEC);
} while (nready < 0 && errno == EINTR);

if (nready < 0)
Punt("poll: %s", strerror(errno));

if (nready > 0 && readyfd(&childExitJob)) {
char token = 0;
ssize_t count;
count = read(childExitJob.inPipe, &token, 1);
if (count == 1) {
if (token == DO_JOB_RESUME[0])




JobRestartJobs();
} else if (count == 0)
Punt("unexpected eof on token pipe");
else if (errno != EAGAIN)
Punt("token pipe read: %s", strerror(errno));
nready--;
}

Job_CatchChildren();
if (nready == 0)
return;

for (i = npseudojobs * nfds_per_job(); i < fdsLen; i++) {
if (fds[i].revents == 0)
continue;
job = jobByFdIndex[i];
if (job->status == JOB_ST_RUNNING)
CollectOutput(job, false);
#if defined(USE_FILEMON) && !defined(USE_FILEMON_DEV)





if (useMeta && job->inPollfd != &fds[i]) {
if (meta_job_event(job) <= 0) {
fds[i].events = 0;
}
}
#endif
if (--nready == 0)
return;
}
}





void
Job_Make(GNode *gn)
{
(void)JobStart(gn, false);
}

static void
InitShellNameAndPath(void)
{
shellName = shell->name;

#if defined(DEFSHELL_CUSTOM)
if (shellName[0] == '/') {
shellPath = shellName;
shellName = str_basename(shellPath);
return;
}
#endif
#if defined(DEFSHELL_PATH)
shellPath = DEFSHELL_PATH;
#else
shellPath = str_concat3(_PATH_DEFSHELLDIR, "/", shellName);
#endif
}

void
Shell_Init(void)
{
if (shellPath == NULL)
InitShellNameAndPath();

Var_SetWithFlags(SCOPE_CMDLINE, ".SHELL", shellPath, VAR_SET_READONLY);
if (shell->errFlag == NULL)
shell->errFlag = "";
if (shell->echoFlag == NULL)
shell->echoFlag = "";
if (shell->hasErrCtl && shell->errFlag[0] != '\0') {
if (shellErrFlag != NULL &&
strcmp(shell->errFlag, &shellErrFlag[1]) != 0) {
free(shellErrFlag);
shellErrFlag = NULL;
}
if (shellErrFlag == NULL) {
size_t n = strlen(shell->errFlag) + 2;

shellErrFlag = bmake_malloc(n);
if (shellErrFlag != NULL)
snprintf(shellErrFlag, n, "-%s",
shell->errFlag);
}
} else if (shellErrFlag != NULL) {
free(shellErrFlag);
shellErrFlag = NULL;
}
}





const char *
Shell_GetNewline(void)
{
return shell->newline;
}

void
Job_SetPrefix(void)
{
if (targPrefix != NULL) {
free(targPrefix);
} else if (!Var_Exists(SCOPE_GLOBAL, MAKE_JOB_PREFIX)) {
Global_Set(MAKE_JOB_PREFIX, "---");
}

(void)Var_Subst("${" MAKE_JOB_PREFIX "}",
SCOPE_GLOBAL, VARE_WANTRES, &targPrefix);

}

static void
AddSig(int sig, SignalProc handler)
{
if (bmake_signal(sig, SIG_IGN) != SIG_IGN) {
sigaddset(&caught_signals, sig);
(void)bmake_signal(sig, handler);
}
}


void
Job_Init(void)
{
Job_SetPrefix();

job_table = bmake_malloc((size_t)opts.maxJobs * sizeof *job_table);
memset(job_table, 0, (size_t)opts.maxJobs * sizeof *job_table);
job_table_end = job_table + opts.maxJobs;
wantToken = 0;
caught_sigchld = 0;

aborting = ABORT_NONE;
job_errors = 0;

Always_pass_job_queue = GetBooleanExpr(MAKE_ALWAYS_PASS_JOB_QUEUE,
Always_pass_job_queue);

Job_error_token = GetBooleanExpr(MAKE_JOB_ERROR_TOKEN, Job_error_token);









for (;;) {
int rval;
WAIT_T status;

rval = waitpid((pid_t)-1, &status, WNOHANG);
if (rval > 0)
continue;
if (rval == 0)
lurking_children = true;
break;
}

Shell_Init();

JobCreatePipe(&childExitJob, 3);

{

size_t nfds = (npseudojobs + (size_t)opts.maxJobs) *
nfds_per_job();
fds = bmake_malloc(sizeof *fds * nfds);
jobByFdIndex = bmake_malloc(sizeof *jobByFdIndex * nfds);
}


watchfd(&tokenWaitJob);
watchfd(&childExitJob);

sigemptyset(&caught_signals);



(void)bmake_signal(SIGCHLD, JobChildSig);
sigaddset(&caught_signals, SIGCHLD);





AddSig(SIGINT, JobPassSig_int);
AddSig(SIGHUP, JobPassSig_term);
AddSig(SIGTERM, JobPassSig_term);
AddSig(SIGQUIT, JobPassSig_term);







AddSig(SIGTSTP, JobPassSig_suspend);
AddSig(SIGTTOU, JobPassSig_suspend);
AddSig(SIGTTIN, JobPassSig_suspend);
AddSig(SIGWINCH, JobCondPassSig);
AddSig(SIGCONT, JobContinueSig);

(void)Job_RunTarget(".BEGIN", NULL);


(void)Targ_GetEndNode();
}

static void
DelSig(int sig)
{
if (sigismember(&caught_signals, sig) != 0)
(void)bmake_signal(sig, SIG_DFL);
}

static void
JobSigReset(void)
{
DelSig(SIGINT);
DelSig(SIGHUP);
DelSig(SIGQUIT);
DelSig(SIGTERM);
DelSig(SIGTSTP);
DelSig(SIGTTOU);
DelSig(SIGTTIN);
DelSig(SIGWINCH);
DelSig(SIGCONT);
(void)bmake_signal(SIGCHLD, SIG_DFL);
}


static Shell *
FindShellByName(const char *name)
{
Shell *sh = shells;
const Shell *shellsEnd = sh + sizeof shells / sizeof shells[0];

for (sh = shells; sh < shellsEnd; sh++) {
if (strcmp(name, sh->name) == 0)
return sh;
}
return NULL;
}












































bool
Job_ParseShell(char *line)
{
Words wordsList;
char **words;
char **argv;
size_t argc;
char *path;
Shell newShell;
bool fullSpec = false;
Shell *sh;


pp_skip_whitespace(&line);

free(shell_freeIt);

memset(&newShell, 0, sizeof newShell);




wordsList = Str_Words(line, true);
words = wordsList.words;
argc = wordsList.len;
path = wordsList.freeIt;
if (words == NULL) {
Error("Unterminated quoted string [%s]", line);
return false;
}
shell_freeIt = path;

for (path = NULL, argv = words; argc != 0; argc--, argv++) {
char *arg = *argv;
if (strncmp(arg, "path=", 5) == 0) {
path = arg + 5;
} else if (strncmp(arg, "name=", 5) == 0) {
newShell.name = arg + 5;
} else {
if (strncmp(arg, "quiet=", 6) == 0) {
newShell.echoOff = arg + 6;
} else if (strncmp(arg, "echo=", 5) == 0) {
newShell.echoOn = arg + 5;
} else if (strncmp(arg, "filter=", 7) == 0) {
newShell.noPrint = arg + 7;
newShell.noPrintLen = strlen(newShell.noPrint);
} else if (strncmp(arg, "echoFlag=", 9) == 0) {
newShell.echoFlag = arg + 9;
} else if (strncmp(arg, "errFlag=", 8) == 0) {
newShell.errFlag = arg + 8;
} else if (strncmp(arg, "hasErrCtl=", 10) == 0) {
char c = arg[10];
newShell.hasErrCtl = c == 'Y' || c == 'y' ||
c == 'T' || c == 't';
} else if (strncmp(arg, "newline=", 8) == 0) {
newShell.newline = arg + 8;
} else if (strncmp(arg, "check=", 6) == 0) {


newShell.errOn = arg + 6;
newShell.echoTmpl = arg + 6;
} else if (strncmp(arg, "ignore=", 7) == 0) {


newShell.errOff = arg + 7;
newShell.runIgnTmpl = arg + 7;
} else if (strncmp(arg, "errout=", 7) == 0) {
newShell.runChkTmpl = arg + 7;
} else if (strncmp(arg, "comment=", 8) == 0) {
newShell.commentChar = arg[8];
} else {
Parse_Error(PARSE_FATAL,
"Unknown keyword \"%s\"", arg);
free(words);
return false;
}
fullSpec = true;
}
}

if (path == NULL) {






if (newShell.name == NULL) {
Parse_Error(PARSE_FATAL,
"Neither path nor name specified");
free(words);
return false;
} else {
if ((sh = FindShellByName(newShell.name)) == NULL) {
Parse_Error(PARSE_WARNING,
"%s: No matching shell", newShell.name);
free(words);
return false;
}
shell = sh;
shellName = newShell.name;
if (shellPath != NULL) {




free(UNCONST(shellPath));
shellPath = NULL;
Shell_Init();
}
}
} else {







shellPath = path;
path = strrchr(path, '/');
if (path == NULL) {
path = UNCONST(shellPath);
} else {
path++;
}
if (newShell.name != NULL) {
shellName = newShell.name;
} else {
shellName = path;
}
if (!fullSpec) {
if ((sh = FindShellByName(shellName)) == NULL) {
Parse_Error(PARSE_WARNING,
"%s: No matching shell", shellName);
free(words);
return false;
}
shell = sh;
} else {
shell = bmake_malloc(sizeof *shell);
*shell = newShell;
}

Shell_Init();
}

if (shell->echoOn != NULL && shell->echoOff != NULL)
shell->hasEchoCtl = true;

if (!shell->hasErrCtl) {
if (shell->echoTmpl == NULL)
shell->echoTmpl = "";
if (shell->runIgnTmpl == NULL)
shell->runIgnTmpl = "%s\n";
}





free(words);
return true;
}












static void
JobInterrupt(bool runINTERRUPT, int signo)
{
Job *job;
GNode *interrupt;
sigset_t mask;
GNode *gn;

aborting = ABORT_INTERRUPT;

JobSigLock(&mask);

for (job = job_table; job < job_table_end; job++) {
if (job->status != JOB_ST_RUNNING)
continue;

gn = job->node;

JobDeleteTarget(gn);
if (job->pid != 0) {
DEBUG2(JOB,
"JobInterrupt passing signal %d to child %d.\n",
signo, job->pid);
KILLPG(job->pid, signo);
}
}

JobSigUnlock(&mask);

if (runINTERRUPT && !opts.touchFlag) {
interrupt = Targ_FindNode(".INTERRUPT");
if (interrupt != NULL) {
opts.ignoreErrors = false;
JobRun(interrupt);
}
}
Trace_Log(MAKEINTR, NULL);
exit(signo);
}






int
Job_Finish(void)
{
GNode *endNode = Targ_GetEndNode();
if (!Lst_IsEmpty(&endNode->commands) ||
!Lst_IsEmpty(&endNode->children)) {
if (job_errors != 0) {
Error("Errors reported so .END ignored");
} else {
JobRun(endNode);
}
}
return job_errors;
}


void
Job_End(void)
{
#if defined(CLEANUP)
free(shell_freeIt);
#endif
}





void
Job_Wait(void)
{
aborting = ABORT_WAIT;
while (jobTokensRunning != 0) {
Job_CatchOutput();
}
aborting = ABORT_NONE;
}








void
Job_AbortAll(void)
{
Job *job;
WAIT_T foo;

aborting = ABORT_ERROR;

if (jobTokensRunning != 0) {
for (job = job_table; job < job_table_end; job++) {
if (job->status != JOB_ST_RUNNING)
continue;




KILLPG(job->pid, SIGINT);
KILLPG(job->pid, SIGKILL);
}
}




while (waitpid((pid_t)-1, &foo, WNOHANG) > 0)
continue;
}





static void
JobRestartJobs(void)
{
Job *job;

for (job = job_table; job < job_table_end; job++) {
if (job->status == JOB_ST_RUNNING &&
(make_suspended || job->suspended)) {
DEBUG1(JOB, "Restarting stopped job pid %d.\n",
job->pid);
if (job->suspended) {
(void)printf("*** [%s] Continued\n",
job->node->name);
(void)fflush(stdout);
}
job->suspended = false;
if (KILLPG(job->pid, SIGCONT) != 0 && DEBUG(JOB)) {
debug_printf("Failed to send SIGCONT to %d\n",
job->pid);
}
}
if (job->status == JOB_ST_FINISHED) {




JobFinish(job, job->exit_status);
}
}
make_suspended = false;
}

static void
watchfd(Job *job)
{
if (job->inPollfd != NULL)
Punt("Watching watched job");

fds[fdsLen].fd = job->inPipe;
fds[fdsLen].events = POLLIN;
jobByFdIndex[fdsLen] = job;
job->inPollfd = &fds[fdsLen];
fdsLen++;
#if defined(USE_FILEMON) && !defined(USE_FILEMON_DEV)
if (useMeta) {
fds[fdsLen].fd = meta_job_fd(job);
fds[fdsLen].events = fds[fdsLen].fd == -1 ? 0 : POLLIN;
jobByFdIndex[fdsLen] = job;
fdsLen++;
}
#endif
}

static void
clearfd(Job *job)
{
size_t i;
if (job->inPollfd == NULL)
Punt("Unwatching unwatched job");
i = (size_t)(job->inPollfd - fds);
fdsLen--;
#if defined(USE_FILEMON) && !defined(USE_FILEMON_DEV)
if (useMeta) {




assert(nfds_per_job() == 2);
if (i % 2 != 0)
Punt("odd-numbered fd with meta");
fdsLen--;
}
#endif



if (fdsLen != i) {
fds[i] = fds[fdsLen];
jobByFdIndex[i] = jobByFdIndex[fdsLen];
jobByFdIndex[i]->inPollfd = &fds[i];
#if defined(USE_FILEMON) && !defined(USE_FILEMON_DEV)
if (useMeta) {
fds[i + 1] = fds[fdsLen + 1];
jobByFdIndex[i + 1] = jobByFdIndex[fdsLen + 1];
}
#endif
}
job->inPollfd = NULL;
}

static bool
readyfd(Job *job)
{
if (job->inPollfd == NULL)
Punt("Polling unwatched job");
return (job->inPollfd->revents & POLLIN) != 0;
}





static void
JobTokenAdd(void)
{
char tok = JOB_TOKENS[aborting], tok1;

if (!Job_error_token && aborting == ABORT_ERROR) {
if (jobTokensRunning == 0)
return;
tok = '+';
}


while (tok != '+' && read(tokenWaitJob.inPipe, &tok1, 1) == 1)
continue;

DEBUG3(JOB, "(%d) aborting %d, deposit token %c\n",
getpid(), aborting, tok);
while (write(tokenWaitJob.outPipe, &tok, 1) == -1 && errno == EAGAIN)
continue;
}


int
Job_TempFile(const char *pattern, char *tfile, size_t tfile_sz)
{
int fd;
sigset_t mask;

JobSigLock(&mask);
fd = mkTempFile(pattern, tfile, tfile_sz);
if (tfile != NULL && !DEBUG(SCRIPT))
unlink(tfile);
JobSigUnlock(&mask);

return fd;
}


void
Job_ServerStart(int max_tokens, int jp_0, int jp_1)
{
int i;
char jobarg[64];

if (jp_0 >= 0 && jp_1 >= 0) {

tokenWaitJob.inPipe = jp_0;
tokenWaitJob.outPipe = jp_1;
(void)fcntl(jp_0, F_SETFD, FD_CLOEXEC);
(void)fcntl(jp_1, F_SETFD, FD_CLOEXEC);
return;
}

JobCreatePipe(&tokenWaitJob, 15);

snprintf(jobarg, sizeof jobarg, "%d,%d",
tokenWaitJob.inPipe, tokenWaitJob.outPipe);

Global_Append(MAKEFLAGS, "-J");
Global_Append(MAKEFLAGS, jobarg);









for (i = 1; i < max_tokens; i++)
JobTokenAdd();
}


void
Job_TokenReturn(void)
{
jobTokensRunning--;
if (jobTokensRunning < 0)
Punt("token botch");
if (jobTokensRunning != 0 || JOB_TOKENS[aborting] != '+')
JobTokenAdd();
}










bool
Job_TokenWithdraw(void)
{
char tok, tok1;
ssize_t count;

wantToken = 0;
DEBUG3(JOB, "Job_TokenWithdraw(%d): aborting %d, running %d\n",
getpid(), aborting, jobTokensRunning);

if (aborting != ABORT_NONE || (jobTokensRunning >= opts.maxJobs))
return false;

count = read(tokenWaitJob.inPipe, &tok, 1);
if (count == 0)
Fatal("eof on job pipe!");
if (count < 0 && jobTokensRunning != 0) {
if (errno != EAGAIN) {
Fatal("job pipe read: %s", strerror(errno));
}
DEBUG1(JOB, "(%d) blocked for token\n", getpid());
wantToken = 1;
return false;
}

if (count == 1 && tok != '+') {

DEBUG2(JOB, "(%d) aborted by token %c\n", getpid(), tok);
while (read(tokenWaitJob.inPipe, &tok1, 1) == 1)
continue;

while (write(tokenWaitJob.outPipe, &tok, 1) == -1 &&
errno == EAGAIN)
continue;
if (shouldDieQuietly(NULL, 1))
exit(6);
Fatal("A failure has been detected "
"in another branch of the parallel make");
}

if (count == 1 && jobTokensRunning == 0)

while (write(tokenWaitJob.outPipe, &tok, 1) == -1 &&
errno == EAGAIN)
continue;

jobTokensRunning++;
DEBUG1(JOB, "(%d) withdrew token\n", getpid());
return true;
}







bool
Job_RunTarget(const char *target, const char *fname)
{
GNode *gn = Targ_FindNode(target);
if (gn == NULL)
return false;

if (fname != NULL)
Var_Set(gn, ALLSRC, fname);

JobRun(gn);

if (gn->made == ERROR) {
PrintOnError(gn, "\n\nStop.");
exit(1);
}
return true;
}

#if defined(USE_SELECT)
int
emul_poll(struct pollfd *fd, int nfd, int timeout)
{
fd_set rfds, wfds;
int i, maxfd, nselect, npoll;
struct timeval tv, *tvp;
long usecs;

FD_ZERO(&rfds);
FD_ZERO(&wfds);

maxfd = -1;
for (i = 0; i < nfd; i++) {
fd[i].revents = 0;

if (fd[i].events & POLLIN)
FD_SET(fd[i].fd, &rfds);

if (fd[i].events & POLLOUT)
FD_SET(fd[i].fd, &wfds);

if (fd[i].fd > maxfd)
maxfd = fd[i].fd;
}

if (maxfd >= FD_SETSIZE) {
Punt("Ran out of fd_set slots; "
"recompile with a larger FD_SETSIZE.");
}

if (timeout < 0) {
tvp = NULL;
} else {
usecs = timeout * 1000;
tv.tv_sec = usecs / 1000000;
tv.tv_usec = usecs % 1000000;
tvp = &tv;
}

nselect = select(maxfd + 1, &rfds, &wfds, NULL, tvp);

if (nselect <= 0)
return nselect;

npoll = 0;
for (i = 0; i < nfd; i++) {
if (FD_ISSET(fd[i].fd, &rfds))
fd[i].revents |= POLLIN;

if (FD_ISSET(fd[i].fd, &wfds))
fd[i].revents |= POLLOUT;

if (fd[i].revents)
npoll++;
}

return npoll;
}
#endif
