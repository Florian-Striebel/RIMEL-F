



















































































#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "wait.h"

#include <errno.h>
#include <signal.h>

#include "make.h"
#include "dir.h"
#include "job.h"
#include "metachar.h"
#include "pathnames.h"


MAKE_RCSID("$NetBSD: compat.c,v 1.229 2021/11/28 23:12:51 rillig Exp $");

static GNode *curTarg = NULL;
static pid_t compatChild;
static int compatSigno;





static void
CompatDeleteTarget(GNode *gn)
{
if (gn != NULL && !Targ_Precious(gn)) {
const char *file = GNode_VarTarget(gn);

if (!opts.noExecute && eunlink(file) != -1) {
Error("*** %s removed", file);
}
}
}










static void
CompatInterrupt(int signo)
{
CompatDeleteTarget(curTarg);

if (curTarg != NULL && !Targ_Precious(curTarg)) {



if (signo == SIGINT) {
GNode *gn = Targ_FindNode(".INTERRUPT");
if (gn != NULL) {
Compat_Make(gn, gn);
}
}
}

if (signo == SIGQUIT)
_exit(signo);





compatSigno = signo;
if (compatChild > 0) {
KILLPG(compatChild, signo);
} else {
bmake_signal(signo, SIG_DFL);
kill(myPid, signo);
}
}

static void
DebugFailedTarget(const char *cmd, const GNode *gn)
{
const char *p = cmd;
debug_printf("\n*** Failed target: %s\n*** Failed command: ",
gn->name);



while (*p != '\0') {
if (ch_isspace(*p)) {
debug_printf(" ");
cpp_skip_whitespace(&p);
} else {
debug_printf("%c", *p);
p++;
}
}
debug_printf("\n");
}

static bool
UseShell(const char *cmd MAKE_ATTR_UNUSED)
{
#if defined(FORCE_USE_SHELL) || !defined(MAKE_NATIVE)






return true;
#else










return needshell(cmd);
#endif
}













int
Compat_RunCommand(const char *cmdp, GNode *gn, StringListNode *ln)
{
char *cmdStart;
char *bp;
bool silent;
bool doIt;
volatile bool errCheck;
WAIT_T reason;
WAIT_T status;
pid_t cpid;
pid_t retstat;
const char **volatile av;
char **volatile mav;
bool useShell;

const char *volatile cmd = cmdp;

silent = (gn->type & OP_SILENT) != OP_NONE;
errCheck = !(gn->type & OP_IGNORE);
doIt = false;

(void)Var_Subst(cmd, gn, VARE_WANTRES, &cmdStart);


if (cmdStart[0] == '\0') {
free(cmdStart);
return 0;
}
cmd = cmdStart;
LstNode_Set(ln, cmdStart);

if (gn->type & OP_SAVE_CMDS) {
GNode *endNode = Targ_GetEndNode();
if (gn != endNode) {











Lst_Append(&endNode->commands, cmdStart);
return 0;
}
}
if (strcmp(cmdStart, "...") == 0) {
gn->type |= OP_SAVE_CMDS;
return 0;
}

for (;;) {
if (*cmd == '@')
silent = !DEBUG(LOUD);
else if (*cmd == '-')
errCheck = false;
else if (*cmd == '+') {
doIt = true;
if (shellName == NULL)
Shell_Init();
} else
break;
cmd++;
}

while (ch_isspace(*cmd))
cmd++;




if (cmd[0] == '\0')
return 0;

useShell = UseShell(cmd);




if (!silent || !GNode_ShouldExecute(gn)) {
printf("%s\n", cmd);
fflush(stdout);
}





if (!doIt && !GNode_ShouldExecute(gn))
return 0;

DEBUG1(JOB, "Execute: '%s'\n", cmd);

if (useShell) {




static const char *shargv[5];


int shargc = 0;
shargv[shargc++] = shellPath;
if (errCheck && shellErrFlag != NULL)
shargv[shargc++] = shellErrFlag;
shargv[shargc++] = DEBUG(SHELL) ? "-xc" : "-c";
shargv[shargc++] = cmd;
shargv[shargc] = NULL;
av = shargv;
bp = NULL;
mav = NULL;
} else {





Words words = Str_Words(cmd, false);
mav = words.words;
bp = words.freeIt;
av = (void *)mav;
}

#if defined(USE_META)
if (useMeta) {
meta_compat_start();
}
#endif

Var_ReexportVars();




compatChild = cpid = vfork();
if (cpid < 0) {
Fatal("Could not fork");
}
if (cpid == 0) {
#if defined(USE_META)
if (useMeta) {
meta_compat_child();
}
#endif
(void)execvp(av[0], (char *const *)UNCONST(av));
execDie("exec", av[0]);
}

free(mav);
free(bp);



LstNode_SetNull(ln);

#if defined(USE_META)
if (useMeta) {
meta_compat_parent(cpid);
}
#endif




while ((retstat = wait(&reason)) != cpid) {
if (retstat > 0)
JobReapChild(retstat, reason, false);
if (retstat == -1 && errno != EINTR) {
break;
}
}

if (retstat < 0)
Fatal("error in wait: %d: %s", retstat, strerror(errno));

if (WIFSTOPPED(reason)) {
status = WSTOPSIG(reason);
} else if (WIFEXITED(reason)) {
status = WEXITSTATUS(reason);
#if defined(USE_META) && defined(USE_FILEMON_ONCE)
if (useMeta) {
meta_cmd_finish(NULL);
}
#endif
if (status != 0) {
if (DEBUG(ERROR))
DebugFailedTarget(cmd, gn);
printf("*** Error code %d", status);
}
} else {
status = WTERMSIG(reason);
printf("*** Signal %d", status);
}


if (!WIFEXITED(reason) || status != 0) {
if (errCheck) {
#if defined(USE_META)
if (useMeta) {
meta_job_error(NULL, gn, false, status);
}
#endif
gn->made = ERROR;
if (opts.keepgoing) {




printf(" (continuing)\n");
} else {
printf("\n");
}
if (deleteOnError)
CompatDeleteTarget(gn);
} else {




printf(" (ignored)\n");
status = 0;
}
}

free(cmdStart);
compatChild = 0;
if (compatSigno != 0) {
bmake_signal(compatSigno, SIG_DFL);
kill(myPid, compatSigno);
}

return status;
}

static void
RunCommands(GNode *gn)
{
StringListNode *ln;

for (ln = gn->commands.first; ln != NULL; ln = ln->next) {
const char *cmd = ln->datum;
if (Compat_RunCommand(cmd, gn, ln) != 0)
break;
}
}

static void
MakeNodes(GNodeList *gnodes, GNode *pgn)
{
GNodeListNode *ln;

for (ln = gnodes->first; ln != NULL; ln = ln->next) {
GNode *cohort = ln->datum;
Compat_Make(cohort, pgn);
}
}

static bool
MakeUnmade(GNode *gn, GNode *pgn)
{

assert(gn->made == UNMADE);









gn->flags.remake = true;
gn->made = BEINGMADE;

if (!(gn->type & OP_MADE))
Suff_FindDeps(gn);

MakeNodes(&gn->children, gn);

if (!gn->flags.remake) {
gn->made = ABORTED;
pgn->flags.remake = false;
return false;
}

if (Lst_FindDatum(&gn->implicitParents, pgn) != NULL)
Var_Set(pgn, IMPSRC, GNode_VarTarget(gn));







DEBUG1(MAKE, "Examining %s...", gn->name);
if (!GNode_IsOODate(gn)) {
gn->made = UPTODATE;
DEBUG0(MAKE, "up-to-date.\n");
return false;
}





DEBUG0(MAKE, "out-of-date.\n");
if (opts.queryFlag)
exit(1);





GNode_SetLocalVars(gn);





if (opts.ignoreErrors)
gn->type |= OP_IGNORE;
if (opts.beSilent)
gn->type |= OP_SILENT;

if (Job_CheckCommands(gn, Fatal)) {




if (!opts.touchFlag || (gn->type & OP_MAKE)) {
curTarg = gn;
#if defined(USE_META)
if (useMeta && GNode_ShouldExecute(gn)) {
meta_job_start(NULL, gn);
}
#endif
RunCommands(gn);
curTarg = NULL;
} else {
Job_Touch(gn, (gn->type & OP_SILENT) != OP_NONE);
}
} else {
gn->made = ERROR;
}
#if defined(USE_META)
if (useMeta && GNode_ShouldExecute(gn)) {
if (meta_job_finish(NULL) != 0)
gn->made = ERROR;
}
#endif

if (gn->made != ERROR) {





gn->made = MADE;
if (Make_Recheck(gn) == 0)
pgn->flags.force = true;
if (!(gn->type & OP_EXEC)) {
pgn->flags.childMade = true;
GNode_UpdateYoungestChild(pgn, gn);
}
} else if (opts.keepgoing) {
pgn->flags.remake = false;
} else {
PrintOnError(gn, "\nStop.");
exit(1);
}
return true;
}

static void
MakeOther(GNode *gn, GNode *pgn)
{

if (Lst_FindDatum(&gn->implicitParents, pgn) != NULL) {
const char *target = GNode_VarTarget(gn);
Var_Set(pgn, IMPSRC, target != NULL ? target : "");
}

switch (gn->made) {
case BEINGMADE:
Error("Graph cycles through %s", gn->name);
gn->made = ERROR;
pgn->flags.remake = false;
break;
case MADE:
if (!(gn->type & OP_EXEC)) {
pgn->flags.childMade = true;
GNode_UpdateYoungestChild(pgn, gn);
}
break;
case UPTODATE:
if (!(gn->type & OP_EXEC))
GNode_UpdateYoungestChild(pgn, gn);
break;
default:
break;
}
}



















void
Compat_Make(GNode *gn, GNode *pgn)
{
if (shellName == NULL)
Shell_Init();

if (gn->made == UNMADE && (gn == pgn || !(pgn->type & OP_MADE))) {
if (!MakeUnmade(gn, pgn))
goto cohorts;


} else if (gn->made == ERROR) {




pgn->flags.remake = false;
} else {
MakeOther(gn, pgn);
}

cohorts:
MakeNodes(&gn->cohorts, pgn);
}

static void
MakeBeginNode(void)
{
GNode *gn = Targ_FindNode(".BEGIN");
if (gn == NULL)
return;

Compat_Make(gn, gn);
if (GNode_IsError(gn)) {
PrintOnError(gn, "\nStop.");
exit(1);
}
}

static void
InitSignals(void)
{
if (bmake_signal(SIGINT, SIG_IGN) != SIG_IGN)
bmake_signal(SIGINT, CompatInterrupt);
if (bmake_signal(SIGTERM, SIG_IGN) != SIG_IGN)
bmake_signal(SIGTERM, CompatInterrupt);
if (bmake_signal(SIGHUP, SIG_IGN) != SIG_IGN)
bmake_signal(SIGHUP, CompatInterrupt);
if (bmake_signal(SIGQUIT, SIG_IGN) != SIG_IGN)
bmake_signal(SIGQUIT, CompatInterrupt);
}







void
Compat_Run(GNodeList *targs)
{
GNode *errorNode = NULL;

if (shellName == NULL)
Shell_Init();

InitSignals();




(void)Targ_GetEndNode();

if (!opts.queryFlag)
MakeBeginNode();





Make_ExpandUse(targs);

while (!Lst_IsEmpty(targs)) {
GNode *gn = Lst_Dequeue(targs);
Compat_Make(gn, gn);

if (gn->made == UPTODATE) {
printf("`%s' is up to date.\n", gn->name);
} else if (gn->made == ABORTED) {
printf("`%s' not remade because of errors.\n",
gn->name);
}
if (GNode_IsError(gn) && errorNode == NULL)
errorNode = gn;
}


if (errorNode == NULL) {
GNode *endNode = Targ_GetEndNode();
Compat_Make(endNode, endNode);
if (GNode_IsError(endNode))
errorNode = endNode;
}

if (errorNode != NULL) {
if (DEBUG(GRAPH2))
Targ_PrintGraph(2);
else if (DEBUG(GRAPH3))
Targ_PrintGraph(3);
PrintOnError(errorNode, "\nStop.");
exit(1);
}
}
