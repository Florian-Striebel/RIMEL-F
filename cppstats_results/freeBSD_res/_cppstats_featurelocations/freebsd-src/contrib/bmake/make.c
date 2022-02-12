




































































































#include "make.h"
#include "dir.h"
#include "job.h"


MAKE_RCSID("$NetBSD: make.c,v 1.248 2021/11/28 23:12:51 rillig Exp $");


static unsigned int checked_seqno = 1;






static GNodeList toBeMade = LST_INIT;


void
debug_printf(const char *fmt, ...)
{
va_list args;

va_start(args, fmt);
vfprintf(opts.debug_file, fmt, args);
va_end(args);
}

MAKE_ATTR_DEAD static void
make_abort(GNode *gn, int lineno)
{

debug_printf("make_abort from line %d\n", lineno);
Targ_PrintNode(gn, 2);
Targ_PrintNodes(&toBeMade, 2);
Targ_PrintGraph(3);
abort();
}

static const char *
GNodeType_ToString(GNodeType type, void **freeIt)
{
Buffer buf;

Buf_InitSize(&buf, 32);
#define ADD(flag) Buf_AddFlag(&buf, (type & (flag)) != OP_NONE, #flag)
ADD(OP_DEPENDS);
ADD(OP_FORCE);
ADD(OP_DOUBLEDEP);
ADD(OP_OPTIONAL);
ADD(OP_USE);
ADD(OP_EXEC);
ADD(OP_IGNORE);
ADD(OP_PRECIOUS);
ADD(OP_SILENT);
ADD(OP_MAKE);
ADD(OP_JOIN);
ADD(OP_MADE);
ADD(OP_SPECIAL);
ADD(OP_USEBEFORE);
ADD(OP_INVISIBLE);
ADD(OP_NOTMAIN);
ADD(OP_PHONY);
ADD(OP_NOPATH);
ADD(OP_WAIT);
ADD(OP_NOMETA);
ADD(OP_META);
ADD(OP_NOMETA_CMP);
ADD(OP_SUBMAKE);
ADD(OP_TRANSFORM);
ADD(OP_MEMBER);
ADD(OP_LIB);
ADD(OP_ARCHV);
ADD(OP_HAS_COMMANDS);
ADD(OP_SAVE_CMDS);
ADD(OP_DEPS_FOUND);
ADD(OP_MARK);
#undef ADD
return buf.len == 0 ? "none" : (*freeIt = Buf_DoneData(&buf));
}

static const char *
GNodeFlags_ToString(GNodeFlags flags, void **freeIt)
{
Buffer buf;

Buf_InitSize(&buf, 32);
#define ADD(flag, name) Buf_AddFlag(&buf, flags.flag, name)
ADD(remake, "REMAKE");
ADD(childMade, "CHILDMADE");
ADD(force, "FORCE");
ADD(doneWait, "DONE_WAIT");
ADD(doneOrder, "DONE_ORDER");
ADD(fromDepend, "FROM_DEPEND");
ADD(doneAllsrc, "DONE_ALLSRC");
ADD(cycle, "CYCLE");
ADD(doneCycle, "DONECYCLE");
#undef ADD
return buf.len == 0 ? "none" : (*freeIt = Buf_DoneData(&buf));
}

void
GNode_FprintDetails(FILE *f, const char *prefix, const GNode *gn,
const char *suffix)
{
void *type_freeIt = NULL;
void *flags_freeIt = NULL;

fprintf(f, "%s%s, type %s, flags %s%s",
prefix,
GNodeMade_Name(gn->made),
GNodeType_ToString(gn->type, &type_freeIt),
GNodeFlags_ToString(gn->flags, &flags_freeIt),
suffix);
free(type_freeIt);
free(flags_freeIt);
}

bool
GNode_ShouldExecute(GNode *gn)
{
return !((gn->type & OP_MAKE)
? opts.noRecursiveExecute
: opts.noExecute);
}


void
GNode_UpdateYoungestChild(GNode *gn, GNode *cgn)
{
if (gn->youngestChild == NULL || cgn->mtime > gn->youngestChild->mtime)
gn->youngestChild = cgn;
}

static bool
IsOODateRegular(GNode *gn)
{


if (gn->youngestChild != NULL) {
if (gn->mtime < gn->youngestChild->mtime) {
DEBUG1(MAKE, "modified before source \"%s\"...",
GNode_Path(gn->youngestChild));
return true;
}
return false;
}

if (gn->mtime == 0 && !(gn->type & OP_OPTIONAL)) {
DEBUG0(MAKE, "nonexistent and no sources...");
return true;
}

if (gn->type & OP_DOUBLEDEP) {
DEBUG0(MAKE, ":: operator and no sources...");
return true;
}

return false;
}













bool
GNode_IsOODate(GNode *gn)
{
bool oodate;





if (!(gn->type & (OP_JOIN | OP_USE | OP_USEBEFORE | OP_EXEC))) {
Dir_UpdateMTime(gn, true);
if (DEBUG(MAKE)) {
if (gn->mtime != 0)
debug_printf("modified %s...",
Targ_FmtTime(gn->mtime));
else
debug_printf("nonexistent...");
}
}



















if (gn->type & (OP_USE | OP_USEBEFORE)) {




DEBUG0(MAKE, ".USE node...");
oodate = false;
} else if ((gn->type & OP_LIB) && (gn->mtime == 0 || Arch_IsLib(gn))) {
DEBUG0(MAKE, "library...");





oodate = (gn->mtime == 0 || Arch_LibOODate(gn) ||
(gn->youngestChild == NULL &&
(gn->type & OP_DOUBLEDEP)));
} else if (gn->type & OP_JOIN) {




DEBUG0(MAKE, ".JOIN node...");
DEBUG1(MAKE, "source %smade...",
gn->flags.childMade ? "" : "not ");
oodate = gn->flags.childMade;
} else if (gn->type & (OP_FORCE | OP_EXEC | OP_PHONY)) {





if (DEBUG(MAKE)) {
if (gn->type & OP_FORCE) {
debug_printf("! operator...");
} else if (gn->type & OP_PHONY) {
debug_printf(".PHONY node...");
} else {
debug_printf(".EXEC node...");
}
}
oodate = true;
} else if (IsOODateRegular(gn)) {
oodate = true;
} else {







if (DEBUG(MAKE)) {
if (gn->flags.force)
debug_printf("non existing child...");
}
oodate = gn->flags.force;
}

#if defined(USE_META)
if (useMeta) {
oodate = meta_oodate(gn, oodate);
}
#endif








if (!oodate) {
GNodeListNode *ln;
for (ln = gn->parents.first; ln != NULL; ln = ln->next)
GNode_UpdateYoungestChild(ln->datum, gn);
}

return oodate;
}

static void
PretendAllChildrenAreMade(GNode *pgn)
{
GNodeListNode *ln;

for (ln = pgn->children.first; ln != NULL; ln = ln->next) {
GNode *cgn = ln->datum;


Dir_UpdateMTime(cgn, false);
GNode_UpdateYoungestChild(pgn, cgn);
pgn->unmade--;
}
}















void
Make_HandleUse(GNode *cgn, GNode *pgn)
{
GNodeListNode *ln;

#if defined(DEBUG_SRC)
if (!(cgn->type & (OP_USE | OP_USEBEFORE | OP_TRANSFORM))) {
debug_printf("Make_HandleUse: called for plain node %s\n",
cgn->name);

return;
}
#endif

if ((cgn->type & (OP_USE | OP_USEBEFORE)) ||
Lst_IsEmpty(&pgn->commands)) {
if (cgn->type & OP_USEBEFORE) {

Lst_PrependAll(&pgn->commands, &cgn->commands);
} else {

Lst_AppendAll(&pgn->commands, &cgn->commands);
}
}

for (ln = cgn->children.first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;







if (gn->uname == NULL) {
gn->uname = gn->name;
} else {
free(gn->name);
}
(void)Var_Subst(gn->uname, pgn, VARE_WANTRES, &gn->name);

if (gn->uname != NULL && strcmp(gn->name, gn->uname) != 0) {

GNode *tgn = Targ_FindNode(gn->name);
if (tgn != NULL)
gn = tgn;
}

Lst_Append(&pgn->children, gn);
Lst_Append(&gn->parents, pgn);
pgn->unmade++;
}

pgn->type |=
cgn->type & ~(OP_OPMASK | OP_USE | OP_USEBEFORE | OP_TRANSFORM);
}













static void
MakeHandleUse(GNode *cgn, GNode *pgn, GNodeListNode *ln)
{
bool unmarked;

unmarked = !(cgn->type & OP_MARK);
cgn->type |= OP_MARK;

if (!(cgn->type & (OP_USE | OP_USEBEFORE)))
return;

if (unmarked)
Make_HandleUse(cgn, pgn);








Lst_Remove(&pgn->children, ln);
pgn->unmade--;
}

static void
HandleUseNodes(GNode *gn)
{
GNodeListNode *ln, *nln;
for (ln = gn->children.first; ln != NULL; ln = nln) {
nln = ln->next;
MakeHandleUse(ln->datum, gn, ln);
}
}






time_t
Make_Recheck(GNode *gn)
{
time_t mtime;

Dir_UpdateMTime(gn, true);
mtime = gn->mtime;

#if !defined(RECHECK)






















if (!Lst_IsEmpty(gn->commands) || Lst_IsEmpty(gn->children)) {
gn->mtime = now;
}
#else























if (!GNode_ShouldExecute(gn) || (gn->type & OP_SAVE_CMDS) ||
(mtime == 0 && !(gn->type & OP_WAIT))) {
DEBUG2(MAKE, " recheck(%s): update time from %s to now\n",
gn->name,
gn->mtime == 0 ? "nonexistent" : Targ_FmtTime(gn->mtime));
gn->mtime = now;
} else {
DEBUG2(MAKE, " recheck(%s): current update time: %s\n",
gn->name, Targ_FmtTime(gn->mtime));
}
#endif



return mtime;
}





static void
UpdateImplicitParentsVars(GNode *cgn, const char *cname)
{
GNodeListNode *ln;
const char *cpref = GNode_VarPrefix(cgn);

for (ln = cgn->implicitParents.first; ln != NULL; ln = ln->next) {
GNode *pgn = ln->datum;
if (pgn->flags.remake) {
Var_Set(pgn, IMPSRC, cname);
if (cpref != NULL)
Var_Set(pgn, PREFIX, cpref);
}
}
}


static bool
IsWaitingForOrder(GNode *gn)
{
GNodeListNode *ln;

for (ln = gn->order_pred.first; ln != NULL; ln = ln->next) {
GNode *ogn = ln->datum;

if (GNode_IsDone(ogn) || !ogn->flags.remake)
continue;

DEBUG2(MAKE,
"IsWaitingForOrder: Waiting for .ORDER node \"%s%s\"\n",
ogn->name, ogn->cohort_num);
return true;
}
return false;
}

static void MakeBuildParent(GNode *, GNodeListNode *);

static void
ScheduleOrderSuccessors(GNode *gn)
{
GNodeListNode *toBeMadeNext = toBeMade.first;
GNodeListNode *ln;

for (ln = gn->order_succ.first; ln != NULL; ln = ln->next)
MakeBuildParent(ln->datum, toBeMadeNext);
}





















void
Make_Update(GNode *cgn)
{
const char *cname;
time_t mtime = -1;
GNodeList *parents;
GNodeListNode *ln;
GNode *centurion;


checked_seqno++;

cname = GNode_VarTarget(cgn);

DEBUG2(MAKE, "Make_Update: %s%s\n", cgn->name, cgn->cohort_num);






if (cgn->made != UPTODATE) {
mtime = Make_Recheck(cgn);
}





if ((centurion = cgn->centurion) != NULL) {
if (!Lst_IsEmpty(&cgn->parents))
Punt("%s%s: cohort has parents", cgn->name,
cgn->cohort_num);
centurion->unmade_cohorts--;
if (centurion->unmade_cohorts < 0)
Error("Graph cycles through centurion %s",
centurion->name);
} else {
centurion = cgn;
}
parents = &centurion->parents;


ScheduleOrderSuccessors(centurion);


for (ln = parents->first; ln != NULL; ln = ln->next) {
GNode *pgn = ln->datum;

if (DEBUG(MAKE)) {
debug_printf("inspect parent %s%s: ", pgn->name,
pgn->cohort_num);
GNode_FprintDetails(opts.debug_file, "", pgn, "");
debug_printf(", unmade %d ", pgn->unmade - 1);
}

if (!pgn->flags.remake) {

DEBUG0(MAKE, "- not needed\n");
continue;
}
if (mtime == 0 && !(cgn->type & OP_WAIT))
pgn->flags.force = true;









if (pgn->type & OP_MADE) {
DEBUG0(MAKE, "- .MADE\n");
continue;
}

if (!(cgn->type & (OP_EXEC | OP_USE | OP_USEBEFORE))) {
if (cgn->made == MADE)
pgn->flags.childMade = true;
GNode_UpdateYoungestChild(pgn, cgn);
}





if (centurion->unmade_cohorts != 0 ||
!GNode_IsDone(centurion)) {
DEBUG2(MAKE,
"- centurion made %d, %d unmade cohorts\n",
centurion->made, centurion->unmade_cohorts);
continue;
}


pgn->unmade--;
if (pgn->unmade < 0) {
if (DEBUG(MAKE)) {
debug_printf("Graph cycles through %s%s\n",
pgn->name, pgn->cohort_num);
Targ_PrintGraph(2);
}
Error("Graph cycles through %s%s", pgn->name,
pgn->cohort_num);
}





if (pgn->unmade != 0 && !(centurion->type & OP_WAIT)
&& !centurion->flags.doneOrder) {
DEBUG0(MAKE, "- unmade children\n");
continue;
}
if (pgn->made != DEFERRED) {





DEBUG0(MAKE, "- not deferred\n");
continue;
}

if (IsWaitingForOrder(pgn))
continue;

if (DEBUG(MAKE)) {
debug_printf("- %s%s made, schedule %s%s (made %d)\n",
cgn->name, cgn->cohort_num,
pgn->name, pgn->cohort_num, pgn->made);
Targ_PrintNode(pgn, 2);
}

pgn->made = REQUESTED;
Lst_Enqueue(&toBeMade, pgn);
}

UpdateImplicitParentsVars(cgn, cname);
}

static void
UnmarkChildren(GNode *gn)
{
GNodeListNode *ln;

for (ln = gn->children.first; ln != NULL; ln = ln->next) {
GNode *child = ln->datum;
child->type &= ~OP_MARK;
}
}




















static void
MakeAddAllSrc(GNode *cgn, GNode *pgn)
{
const char *child, *allsrc;

if (cgn->type & OP_MARK)
return;
cgn->type |= OP_MARK;

if (cgn->type & (OP_EXEC | OP_USE | OP_USEBEFORE | OP_INVISIBLE))
return;

if (cgn->type & OP_ARCHV)
child = GNode_VarMember(cgn);
else
child = GNode_Path(cgn);

if (cgn->type & OP_JOIN)
allsrc = GNode_VarAllsrc(cgn);
else
allsrc = child;

if (allsrc != NULL)
Var_Append(pgn, ALLSRC, allsrc);

if (pgn->type & OP_JOIN) {
if (cgn->made == MADE)
Var_Append(pgn, OODATE, child);

} else if ((pgn->mtime < cgn->mtime) ||
(cgn->mtime >= now && cgn->made == MADE)) {


















Var_Append(pgn, OODATE, child);
}
}















void
GNode_SetLocalVars(GNode *gn)
{
GNodeListNode *ln;

if (gn->flags.doneAllsrc)
return;

UnmarkChildren(gn);
for (ln = gn->children.first; ln != NULL; ln = ln->next)
MakeAddAllSrc(ln->datum, gn);

if (!Var_Exists(gn, OODATE))
Var_Set(gn, OODATE, "");
if (!Var_Exists(gn, ALLSRC))
Var_Set(gn, ALLSRC, "");

if (gn->type & OP_JOIN)
Var_Set(gn, TARGET, GNode_VarAllsrc(gn));
gn->flags.doneAllsrc = true;
}

static bool
MakeBuildChild(GNode *cn, GNodeListNode *toBeMadeNext)
{

if (DEBUG(MAKE)) {
debug_printf("MakeBuildChild: inspect %s%s, ",
cn->name, cn->cohort_num);
GNode_FprintDetails(opts.debug_file, "", cn, "\n");
}
if (GNode_IsReady(cn))
return false;


if (IsWaitingForOrder(cn)) {

cn->made = DEFERRED;
return false;
}

DEBUG2(MAKE, "MakeBuildChild: schedule %s%s\n",
cn->name, cn->cohort_num);

cn->made = REQUESTED;
if (toBeMadeNext == NULL)
Lst_Append(&toBeMade, cn);
else
Lst_InsertBefore(&toBeMade, toBeMadeNext, cn);

if (cn->unmade_cohorts != 0) {
ListNode *ln;

for (ln = cn->cohorts.first; ln != NULL; ln = ln->next)
if (MakeBuildChild(ln->datum, toBeMadeNext))
break;
}





return cn->type & OP_WAIT && cn->unmade > 0;
}


static void
MakeBuildParent(GNode *pn, GNodeListNode *toBeMadeNext)
{
if (pn->made != DEFERRED)
return;

if (!MakeBuildChild(pn, toBeMadeNext)) {

pn->flags.doneOrder = true;
}
}

static void
MakeChildren(GNode *gn)
{
GNodeListNode *toBeMadeNext = toBeMade.first;
GNodeListNode *ln;

for (ln = gn->children.first; ln != NULL; ln = ln->next)
if (MakeBuildChild(ln->datum, toBeMadeNext))
break;
}








static bool
MakeStartJobs(void)
{
GNode *gn;
bool have_token = false;

while (!Lst_IsEmpty(&toBeMade)) {




if (!have_token && !Job_TokenWithdraw())
break;
have_token = true;

gn = Lst_Dequeue(&toBeMade);
DEBUG2(MAKE, "Examining %s%s...\n", gn->name, gn->cohort_num);

if (gn->made != REQUESTED) {




DEBUG1(MAKE, "state %d\n", gn->made);

make_abort(gn, __LINE__);
}

if (gn->checked_seqno == checked_seqno) {




DEBUG2(MAKE, "already checked %s%s\n", gn->name,
gn->cohort_num);
gn->made = DEFERRED;
continue;
}
gn->checked_seqno = checked_seqno;

if (gn->unmade != 0) {




gn->made = DEFERRED;

MakeChildren(gn);


DEBUG2(MAKE, "dropped %s%s\n", gn->name,
gn->cohort_num);
continue;
}

gn->made = BEINGMADE;
if (GNode_IsOODate(gn)) {
DEBUG0(MAKE, "out-of-date\n");
if (opts.queryFlag)
return true;
GNode_SetLocalVars(gn);
Job_Make(gn);
have_token = false;
} else {
DEBUG0(MAKE, "up-to-date\n");
gn->made = UPTODATE;
if (gn->type & OP_JOIN) {







GNode_SetLocalVars(gn);
}
Make_Update(gn);
}
}

if (have_token)
Job_TokenReturn();

return false;
}


static void
MakePrintStatusOrderNode(GNode *ogn, GNode *gn)
{
if (!GNode_IsWaitingFor(ogn))
return;

printf(" `%s%s' has .ORDER dependency against %s%s ",
gn->name, gn->cohort_num, ogn->name, ogn->cohort_num);
GNode_FprintDetails(stdout, "(", ogn, ")\n");

if (DEBUG(MAKE) && opts.debug_file != stdout) {
debug_printf(" `%s%s' has .ORDER dependency against %s%s ",
gn->name, gn->cohort_num, ogn->name, ogn->cohort_num);
GNode_FprintDetails(opts.debug_file, "(", ogn, ")\n");
}
}

static void
MakePrintStatusOrder(GNode *gn)
{
GNodeListNode *ln;
for (ln = gn->order_pred.first; ln != NULL; ln = ln->next)
MakePrintStatusOrderNode(ln->datum, gn);
}

static void MakePrintStatusList(GNodeList *, int *);





static bool
MakePrintStatus(GNode *gn, int *errors)
{
if (gn->flags.doneCycle) {




return false;
}

if (gn->unmade == 0) {
gn->flags.doneCycle = true;
switch (gn->made) {
case UPTODATE:
printf("`%s%s' is up to date.\n", gn->name,
gn->cohort_num);
break;
case MADE:
break;
case UNMADE:
case DEFERRED:
case REQUESTED:
case BEINGMADE:
(*errors)++;
printf("`%s%s' was not built", gn->name,
gn->cohort_num);
GNode_FprintDetails(stdout, " (", gn, ")!\n");
if (DEBUG(MAKE) && opts.debug_file != stdout) {
debug_printf("`%s%s' was not built", gn->name,
gn->cohort_num);
GNode_FprintDetails(opts.debug_file, " (", gn,
")!\n");
}

MakePrintStatusOrder(gn);
break;
default:

printf("`%s%s' not remade because of errors.\n",
gn->name, gn->cohort_num);
if (DEBUG(MAKE) && opts.debug_file != stdout)
debug_printf(
"`%s%s' not remade because of errors.\n",
gn->name, gn->cohort_num);
break;
}
return false;
}

DEBUG3(MAKE, "MakePrintStatus: %s%s has %d unmade children\n",
gn->name, gn->cohort_num, gn->unmade);




if (!gn->flags.cycle) {

gn->flags.cycle = true;
MakePrintStatusList(&gn->children, errors);

gn->flags.doneCycle = true;
return false;
}


gn->flags.doneCycle = true;
Error("Graph cycles through `%s%s'", gn->name, gn->cohort_num);
if ((*errors)++ > 100)

return true;


MakePrintStatusList(&gn->children, errors);
return false;
}

static void
MakePrintStatusList(GNodeList *gnodes, int *errors)
{
GNodeListNode *ln;

for (ln = gnodes->first; ln != NULL; ln = ln->next)
if (MakePrintStatus(ln->datum, errors))
break;
}

static void
ExamineLater(GNodeList *examine, GNodeList *toBeExamined)
{
ListNode *ln;

for (ln = toBeExamined->first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;

if (gn->flags.remake)
continue;
if (gn->type & (OP_USE | OP_USEBEFORE))
continue;

DEBUG2(MAKE, "ExamineLater: need to examine \"%s%s\"\n",
gn->name, gn->cohort_num);
Lst_Enqueue(examine, gn);
}
}







void
Make_ExpandUse(GNodeList *targs)
{
GNodeList examine = LST_INIT;
Lst_AppendAll(&examine, targs);











while (!Lst_IsEmpty(&examine)) {
GNode *gn = Lst_Dequeue(&examine);

if (gn->flags.remake)

continue;
gn->flags.remake = true;
DEBUG2(MAKE, "Make_ExpandUse: examine %s%s\n",
gn->name, gn->cohort_num);

if (gn->type & OP_DOUBLEDEP)
Lst_PrependAll(&examine, &gn->cohorts);









if (gn->type & OP_ARCHV) {
char *eoa = strchr(gn->name, '(');
char *eon = strchr(gn->name, ')');
if (eoa == NULL || eon == NULL)
continue;
*eoa = '\0';
*eon = '\0';
Var_Set(gn, MEMBER, eoa + 1);
Var_Set(gn, ARCHIVE, gn->name);
*eoa = '(';
*eon = ')';
}

Dir_UpdateMTime(gn, false);
Var_Set(gn, TARGET, GNode_Path(gn));
UnmarkChildren(gn);
HandleUseNodes(gn);

if (!(gn->type & OP_MADE))
Suff_FindDeps(gn);
else {
PretendAllChildrenAreMade(gn);
if (gn->unmade != 0) {
printf(
"Warning: "
"%s%s still has %d unmade children\n",
gn->name, gn->cohort_num, gn->unmade);
}
}

if (gn->unmade != 0)
ExamineLater(&examine, &gn->children);
}

Lst_Done(&examine);
}


static void
add_wait_dependency(GNodeListNode *owln, GNode *wn)
{
GNodeListNode *cln;
GNode *cn;

for (cln = owln; (cn = cln->datum) != wn; cln = cln->next) {
DEBUG3(MAKE, ".WAIT: add dependency %s%s -> %s\n",
cn->name, cn->cohort_num, wn->name);


Lst_Append(&wn->children, cn);
wn->unmade++;
Lst_Append(&cn->parents, wn);
}
}


static void
Make_ProcessWait(GNodeList *targs)
{
GNode *pgn;
GNodeListNode *owln;
GNodeList examine;







pgn = GNode_New(".MAIN");
pgn->flags.remake = true;
pgn->type = OP_PHONY | OP_DEPENDS;

Lst_Prepend(Targ_List(), pgn);

{
GNodeListNode *ln;
for (ln = targs->first; ln != NULL; ln = ln->next) {
GNode *cgn = ln->datum;

Lst_Append(&pgn->children, cgn);
Lst_Append(&cgn->parents, pgn);
pgn->unmade++;
}
}


MakeBuildChild(pgn, NULL);

Lst_Init(&examine);
Lst_Append(&examine, pgn);

while (!Lst_IsEmpty(&examine)) {
GNodeListNode *ln;

pgn = Lst_Dequeue(&examine);


if (pgn->flags.doneWait)
continue;
pgn->flags.doneWait = true;
DEBUG1(MAKE, "Make_ProcessWait: examine %s\n", pgn->name);

if (pgn->type & OP_DOUBLEDEP)
Lst_PrependAll(&examine, &pgn->cohorts);

owln = pgn->children.first;
for (ln = pgn->children.first; ln != NULL; ln = ln->next) {
GNode *cgn = ln->datum;
if (cgn->type & OP_WAIT) {
add_wait_dependency(owln, cgn);
owln = ln;
} else {
Lst_Append(&examine, cgn);
}
}
}

Lst_Done(&examine);
}





















bool
Make_Run(GNodeList *targs)
{
int errors;


Lst_Init(&toBeMade);

Make_ExpandUse(targs);
Make_ProcessWait(targs);

if (DEBUG(MAKE)) {
debug_printf("#***#full graph\n");
Targ_PrintGraph(1);
}

if (opts.queryFlag) {






return MakeStartJobs();
}







(void)MakeStartJobs();











while (!Lst_IsEmpty(&toBeMade) || jobTokensRunning > 0) {
Job_CatchOutput();
(void)MakeStartJobs();
}

errors = Job_Finish();





DEBUG1(MAKE, "done: errors %d\n", errors);
if (errors == 0) {
MakePrintStatusList(targs, &errors);
if (DEBUG(MAKE)) {
debug_printf("done: errors %d\n", errors);
if (errors > 0)
Targ_PrintGraph(4);
}
}
return errors > 0;
}
