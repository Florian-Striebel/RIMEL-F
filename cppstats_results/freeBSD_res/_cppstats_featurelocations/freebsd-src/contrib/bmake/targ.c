












































































































#include <time.h>

#include "make.h"
#include "dir.h"


MAKE_RCSID("$NetBSD: targ.c,v 1.173 2021/11/28 19:51:06 rillig Exp $");





static GNodeList allTargets = LST_INIT;
static HashTable allTargetsByName;

#if defined(CLEANUP)
static GNodeList allNodes = LST_INIT;

static void GNode_Free(void *);
#endif

void
Targ_Init(void)
{
HashTable_Init(&allTargetsByName);
}

void
Targ_End(void)
{
Targ_Stats();
#if defined(CLEANUP)
Lst_Done(&allTargets);
HashTable_Done(&allTargetsByName);
Lst_DoneCall(&allNodes, GNode_Free);
#endif
}

void
Targ_Stats(void)
{
HashTable_DebugStats(&allTargetsByName, "targets");
}






GNodeList *
Targ_List(void)
{
return &allTargets;
}
















GNode *
GNode_New(const char *name)
{
GNode *gn;

gn = bmake_malloc(sizeof *gn);
gn->name = bmake_strdup(name);
gn->uname = NULL;
gn->path = NULL;
gn->type = name[0] == '-' && name[1] == 'l' ? OP_LIB : OP_NONE;
memset(&gn->flags, 0, sizeof(gn->flags));
gn->made = UNMADE;
gn->unmade = 0;
gn->mtime = 0;
gn->youngestChild = NULL;
Lst_Init(&gn->implicitParents);
Lst_Init(&gn->parents);
Lst_Init(&gn->children);
Lst_Init(&gn->order_pred);
Lst_Init(&gn->order_succ);
Lst_Init(&gn->cohorts);
gn->cohort_num[0] = '\0';
gn->unmade_cohorts = 0;
gn->centurion = NULL;
gn->checked_seqno = 0;
HashTable_Init(&gn->vars);
Lst_Init(&gn->commands);
gn->suffix = NULL;
gn->fname = NULL;
gn->lineno = 0;

#if defined(CLEANUP)
Lst_Append(&allNodes, gn);
#endif

return gn;
}

#if defined(CLEANUP)
static void
GNode_Free(void *gnp)
{
GNode *gn = gnp;

free(gn->name);
free(gn->uname);
free(gn->path);







Lst_Done(&gn->implicitParents);
Lst_Done(&gn->parents);
Lst_Done(&gn->children);
Lst_Done(&gn->order_pred);
Lst_Done(&gn->order_succ);
Lst_Done(&gn->cohorts);















HashTable_Done(&gn->vars);





Lst_Done(&gn->commands);









free(gn);
}
#endif


GNode *
Targ_FindNode(const char *name)
{
return HashTable_FindValue(&allTargetsByName, name);
}


GNode *
Targ_GetNode(const char *name)
{
bool isNew;
HashEntry *he = HashTable_CreateEntry(&allTargetsByName, name, &isNew);
if (!isNew)
return HashEntry_Get(he);

{
GNode *gn = Targ_NewInternalNode(name);
HashEntry_Set(he, gn);
return gn;
}
}







GNode *
Targ_NewInternalNode(const char *name)
{
GNode *gn = GNode_New(name);
Global_Append(".ALLTARGETS", name);
Lst_Append(&allTargets, gn);
DEBUG1(TARG, "Adding \"%s\" to all targets.\n", gn->name);
if (doing_depend)
gn->flags.fromDepend = true;
return gn;
}





GNode *
Targ_GetEndNode(void)
{




static GNode *endNode = NULL;

if (endNode == NULL) {
endNode = Targ_GetNode(".END");
endNode->type = OP_SPECIAL;
}
return endNode;
}


void
Targ_FindList(GNodeList *gns, StringList *names)
{
StringListNode *ln;

for (ln = names->first; ln != NULL; ln = ln->next) {
const char *name = ln->datum;
GNode *gn = Targ_GetNode(name);
Lst_Append(gns, gn);
}
}


bool
Targ_Precious(const GNode *gn)
{

return allPrecious || gn->type & (OP_PRECIOUS | OP_DOUBLEDEP);
}





static GNode *mainTarg;


void
Targ_SetMain(GNode *gn)
{
mainTarg = gn;
}

static void
PrintNodeNames(GNodeList *gnodes)
{
GNodeListNode *ln;

for (ln = gnodes->first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;
debug_printf(" %s%s", gn->name, gn->cohort_num);
}
}

static void
PrintNodeNamesLine(const char *label, GNodeList *gnodes)
{
if (Lst_IsEmpty(gnodes))
return;
debug_printf("#%s:", label);
PrintNodeNames(gnodes);
debug_printf("\n");
}

void
Targ_PrintCmds(GNode *gn)
{
StringListNode *ln;

for (ln = gn->commands.first; ln != NULL; ln = ln->next) {
const char *cmd = ln->datum;
debug_printf("\t%s\n", cmd);
}
}






const char *
Targ_FmtTime(time_t tm)
{
static char buf[128];

struct tm *parts = localtime(&tm);
(void)strftime(buf, sizeof buf, "%H:%M:%S %b %d, %Y", parts);
return buf;
}


void
Targ_PrintType(GNodeType type)
{
static const struct {
GNodeType bit;
bool internal;
const char name[10];
} names[] = {
{ OP_MEMBER, true, "MEMBER" },
{ OP_LIB, true, "LIB" },
{ OP_ARCHV, true, "ARCHV" },
{ OP_PHONY, true, "PHONY" },
{ OP_NOTMAIN, false, "NOTMAIN" },
{ OP_INVISIBLE, false, "INVISIBLE" },
{ OP_MADE, true, "MADE" },
{ OP_JOIN, false, "JOIN" },
{ OP_MAKE, false, "MAKE" },
{ OP_SILENT, false, "SILENT" },
{ OP_PRECIOUS, false, "PRECIOUS" },
{ OP_IGNORE, false, "IGNORE" },
{ OP_EXEC, false, "EXEC" },
{ OP_USE, false, "USE" },
{ OP_OPTIONAL, false, "OPTIONAL" },
};
size_t i;

for (i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
if (type & names[i].bit) {
if (names[i].internal)
DEBUG1(TARG, " .%s", names[i].name);
else
debug_printf(" .%s", names[i].name);
}
}
}

const char *
GNodeMade_Name(GNodeMade made)
{
switch (made) {
case UNMADE: return "unmade";
case DEFERRED: return "deferred";
case REQUESTED: return "requested";
case BEINGMADE: return "being made";
case MADE: return "made";
case UPTODATE: return "up-to-date";
case ERROR: return "error when made";
case ABORTED: return "aborted";
default: return "unknown enum_made value";
}
}

static const char *
GNode_OpName(const GNode *gn)
{
switch (gn->type & OP_OPMASK) {
case OP_DEPENDS:
return ":";
case OP_FORCE:
return "!";
case OP_DOUBLEDEP:
return "::";
}
return "";
}

static bool
GNodeFlags_IsNone(GNodeFlags flags)
{
return !flags.remake
&& !flags.childMade
&& !flags.force
&& !flags.doneWait
&& !flags.doneOrder
&& !flags.fromDepend
&& !flags.doneAllsrc
&& !flags.cycle
&& !flags.doneCycle;
}


void
Targ_PrintNode(GNode *gn, int pass)
{
debug_printf("#%s%s", gn->name, gn->cohort_num);
GNode_FprintDetails(opts.debug_file, ", ", gn, "\n");
if (GNodeFlags_IsNone(gn->flags))
return;

if (!GNode_IsTarget(gn))
return;

debug_printf("#\n");
if (gn == mainTarg)
debug_printf("#*** MAIN TARGET ***\n");

if (pass >= 2) {
if (gn->unmade > 0)
debug_printf("#%d unmade children\n", gn->unmade);
else
debug_printf("#No unmade children\n");
if (!(gn->type & (OP_JOIN | OP_USE | OP_USEBEFORE | OP_EXEC))) {
if (gn->mtime != 0) {
debug_printf("#last modified %s: %s\n",
Targ_FmtTime(gn->mtime),
GNodeMade_Name(gn->made));
} else if (gn->made != UNMADE) {
debug_printf("#nonexistent (maybe): %s\n",
GNodeMade_Name(gn->made));
} else
debug_printf("#unmade\n");
}
PrintNodeNamesLine("implicit parents", &gn->implicitParents);
} else {
if (gn->unmade != 0)
debug_printf("#%d unmade children\n", gn->unmade);
}

PrintNodeNamesLine("parents", &gn->parents);
PrintNodeNamesLine("order_pred", &gn->order_pred);
PrintNodeNamesLine("order_succ", &gn->order_succ);

debug_printf("%-16s%s", gn->name, GNode_OpName(gn));
Targ_PrintType(gn->type);
PrintNodeNames(&gn->children);
debug_printf("\n");
Targ_PrintCmds(gn);
debug_printf("\n\n");
if (gn->type & OP_DOUBLEDEP)
Targ_PrintNodes(&gn->cohorts, pass);
}

void
Targ_PrintNodes(GNodeList *gnodes, int pass)
{
GNodeListNode *ln;

for (ln = gnodes->first; ln != NULL; ln = ln->next)
Targ_PrintNode(ln->datum, pass);
}


static void
PrintOnlySources(void)
{
GNodeListNode *ln;

for (ln = allTargets.first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;
if (GNode_IsTarget(gn))
continue;

debug_printf("#\t%s [%s]", gn->name, GNode_Path(gn));
Targ_PrintType(gn->type);
debug_printf("\n");
}
}







void
Targ_PrintGraph(int pass)
{
debug_printf("#*** Input graph:\n");
Targ_PrintNodes(&allTargets, pass);
debug_printf("\n");
debug_printf("\n");

debug_printf("#\n");
debug_printf("#Files that are only sources:\n");
PrintOnlySources();

debug_printf("#*** Global Variables:\n");
Var_Dump(SCOPE_GLOBAL);

debug_printf("#*** Command-line Variables:\n");
Var_Dump(SCOPE_CMDLINE);

debug_printf("\n");
Dir_PrintDirectories();
debug_printf("\n");

Suff_PrintAll();
}








void
Targ_Propagate(void)
{
GNodeListNode *ln, *cln;

for (ln = allTargets.first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;
GNodeType type = gn->type;

if (!(type & OP_DOUBLEDEP))
continue;

for (cln = gn->cohorts.first; cln != NULL; cln = cln->next) {
GNode *cohort = cln->datum;

cohort->type |= type & ~OP_OPMASK;
}
}
}
