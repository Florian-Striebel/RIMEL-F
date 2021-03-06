
















































































































#include "make.h"
#include "dir.h"


MAKE_RCSID("$NetBSD: suff.c,v 1.357 2021/12/12 20:45:48 sjg Exp $");

typedef List SuffixList;
typedef ListNode SuffixListNode;

typedef List CandidateList;
typedef ListNode CandidateListNode;


static SuffixList sufflist = LST_INIT;
#if defined(CLEANUP)

static SuffixList suffClean = LST_INIT;
#endif





static GNodeList transforms = LST_INIT;





static int sNum = 0;

typedef List SuffixListList;





typedef struct Suffix {

char *name;

size_t nameLen;




bool include:1;




bool library:1;









bool isNull:1;

SearchPath *searchPath;


int sNum;

int refCount;


SuffixList parents;

SuffixList children;







SuffixListList ref;
} Suffix;










typedef struct Candidate {

char *file;


char *prefix;

Suffix *suff;



struct Candidate *parent;

GNode *node;



int numChildren;
#if defined(DEBUG_SRC)
CandidateList childrenList;
#endif
} Candidate;

typedef struct CandidateSearcher {

CandidateList list;






} CandidateSearcher;




static Suffix *nullSuff;

static Suffix *emptySuff;


static Suffix *
Suffix_Ref(Suffix *suff)
{
suff->refCount++;
return suff;
}


static void
Suffix_Reassign(Suffix **var, Suffix *suff)
{
if (*var != NULL)
(*var)->refCount--;
*var = suff;
suff->refCount++;
}


static void
Suffix_Unassign(Suffix **var)
{
if (*var != NULL)
(*var)->refCount--;
*var = NULL;
}





static const char *
StrTrimPrefix(const char *pref, const char *str)
{
while (*str != '\0' && *pref == *str) {
pref++;
str++;
}

return *pref != '\0' ? NULL : str;
}





static const char *
StrTrimSuffix(const char *str, size_t strLen, const char *suff, size_t suffLen)
{
const char *suffInStr;
size_t i;

if (strLen < suffLen)
return NULL;

suffInStr = str + strLen - suffLen;
for (i = 0; i < suffLen; i++)
if (suff[i] != suffInStr[i])
return NULL;

return suffInStr;
}





static const char *
Suffix_TrimSuffix(const Suffix *suff, size_t nameLen, const char *nameEnd)
{
return StrTrimSuffix(nameEnd - nameLen, nameLen,
suff->name, suff->nameLen);
}

static bool
Suffix_IsSuffix(const Suffix *suff, size_t nameLen, const char *nameEnd)
{
return Suffix_TrimSuffix(suff, nameLen, nameEnd) != NULL;
}

static Suffix *
FindSuffixByNameLen(const char *name, size_t nameLen)
{
SuffixListNode *ln;

for (ln = sufflist.first; ln != NULL; ln = ln->next) {
Suffix *suff = ln->datum;
if (suff->nameLen == nameLen &&
memcmp(suff->name, name, nameLen) == 0)
return suff;
}
return NULL;
}

static Suffix *
FindSuffixByName(const char *name)
{
return FindSuffixByNameLen(name, strlen(name));
}

static GNode *
FindTransformByName(const char *name)
{
GNodeListNode *ln;

for (ln = transforms.first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;
if (strcmp(gn->name, name) == 0)
return gn;
}
return NULL;
}

static void
SuffixList_Unref(SuffixList *list, Suffix *suff)
{
SuffixListNode *ln = Lst_FindDatum(list, suff);
if (ln != NULL) {
Lst_Remove(list, ln);
suff->refCount--;
}
}


static void
Suffix_Free(Suffix *suff)
{

if (suff == nullSuff)
nullSuff = NULL;

if (suff == emptySuff)
emptySuff = NULL;

#if 0

if (suff->refCount != 0)
Punt("Internal error deleting suffix `%s' with refcount = %d",
suff->name, suff->refCount);
#endif

Lst_Done(&suff->ref);
Lst_Done(&suff->children);
Lst_Done(&suff->parents);
SearchPath_Free(suff->searchPath);

free(suff->name);
free(suff);
}

static void
SuffFree(void *p)
{
Suffix_Free(p);
}


static void
SuffixList_Remove(SuffixList *list, Suffix *suff)
{
SuffixList_Unref(list, suff);
if (suff->refCount == 0) {

SuffixList_Unref(&sufflist, suff);
DEBUG1(SUFF, "Removing suffix \"%s\"\n", suff->name);
SuffFree(suff);
}
}





static void
SuffixList_Insert(SuffixList *list, Suffix *suff)
{
SuffixListNode *ln;
Suffix *listSuff = NULL;

for (ln = list->first; ln != NULL; ln = ln->next) {
listSuff = ln->datum;
if (listSuff->sNum >= suff->sNum)
break;
}

if (ln == NULL) {
DEBUG2(SUFF, "inserting \"%s\" (%d) at end of list\n",
suff->name, suff->sNum);
Lst_Append(list, Suffix_Ref(suff));
Lst_Append(&suff->ref, list);
} else if (listSuff->sNum != suff->sNum) {
DEBUG4(SUFF, "inserting \"%s\" (%d) before \"%s\" (%d)\n",
suff->name, suff->sNum, listSuff->name, listSuff->sNum);
Lst_InsertBefore(list, ln, Suffix_Ref(suff));
Lst_Append(&suff->ref, list);
} else {
DEBUG2(SUFF, "\"%s\" (%d) is already there\n",
suff->name, suff->sNum);
}
}

static void
Relate(Suffix *srcSuff, Suffix *targSuff)
{
SuffixList_Insert(&targSuff->children, srcSuff);
SuffixList_Insert(&srcSuff->parents, targSuff);
}

static Suffix *
Suffix_New(const char *name)
{
Suffix *suff = bmake_malloc(sizeof *suff);

suff->name = bmake_strdup(name);
suff->nameLen = strlen(suff->name);
suff->searchPath = SearchPath_New();
Lst_Init(&suff->children);
Lst_Init(&suff->parents);
Lst_Init(&suff->ref);
suff->sNum = sNum++;
suff->include = false;
suff->library = false;
suff->isNull = false;
suff->refCount = 1;

return suff;
}








void
Suff_ClearSuffixes(void)
{
#if defined(CLEANUP)
Lst_MoveAll(&suffClean, &sufflist);
#endif
DEBUG0(SUFF, "Clearing all suffixes\n");
Lst_Init(&sufflist);
sNum = 0;
if (nullSuff != NULL)
SuffFree(nullSuff);
emptySuff = nullSuff = Suffix_New("");

SearchPath_AddAll(nullSuff->searchPath, &dirSearchPath);
nullSuff->include = false;
nullSuff->library = false;
nullSuff->isNull = true;
}








static bool
ParseTransform(const char *str, Suffix **out_src, Suffix **out_targ)
{
SuffixListNode *ln;
Suffix *single = NULL;







for (ln = sufflist.first; ln != NULL; ln = ln->next) {
Suffix *src = ln->datum;

if (StrTrimPrefix(src->name, str) == NULL)
continue;

if (str[src->nameLen] == '\0') {
single = src;
} else {
Suffix *targ = FindSuffixByName(str + src->nameLen);
if (targ != NULL) {
*out_src = src;
*out_targ = targ;
return true;
}
}
}

if (single != NULL) {










*out_src = single;
*out_targ = nullSuff;
return true;
}
return false;
}






bool
Suff_IsTransform(const char *str)
{
Suffix *src, *targ;

return ParseTransform(str, &src, &targ);
}













GNode *
Suff_AddTransform(const char *name)
{
Suffix *srcSuff;
Suffix *targSuff;

GNode *gn = FindTransformByName(name);
if (gn == NULL) {




gn = GNode_New(name);
Lst_Append(&transforms, gn);
} else {







Lst_Done(&gn->commands);
Lst_Init(&gn->commands);
Lst_Done(&gn->children);
Lst_Init(&gn->children);
}

gn->type = OP_TRANSFORM;

{

bool ok = ParseTransform(name, &srcSuff, &targSuff);
assert(ok);

(void)ok;
}


DEBUG2(SUFF, "defining transformation from `%s' to `%s'\n",
srcSuff->name, targSuff->name);
Relate(srcSuff, targSuff);

return gn;
}











void
Suff_EndTransform(GNode *gn)
{
Suffix *srcSuff, *targSuff;
SuffixList *srcSuffParents;

if ((gn->type & OP_DOUBLEDEP) && !Lst_IsEmpty(&gn->cohorts))
gn = gn->cohorts.last->datum;

if (!(gn->type & OP_TRANSFORM))
return;

if (!Lst_IsEmpty(&gn->commands) || !Lst_IsEmpty(&gn->children)) {
DEBUG1(SUFF, "transformation %s complete\n", gn->name);
return;
}





if (!ParseTransform(gn->name, &srcSuff, &targSuff))
return;

DEBUG2(SUFF, "deleting incomplete transformation from `%s' to `%s'\n",
srcSuff->name, targSuff->name);





srcSuffParents = &srcSuff->parents;
SuffixList_Remove(&targSuff->children, srcSuff);
SuffixList_Remove(srcSuffParents, targSuff);
}















static void
RebuildGraph(GNode *transform, Suffix *suff)
{
const char *name = transform->name;
size_t nameLen = strlen(name);
const char *toName;


toName = StrTrimPrefix(suff->name, name);
if (toName != NULL) {
Suffix *to = FindSuffixByName(toName);
if (to != NULL) {
Relate(suff, to);
return;
}
}


toName = Suffix_TrimSuffix(suff, nameLen, name + nameLen);
if (toName != NULL) {
Suffix *from = FindSuffixByNameLen(name,
(size_t)(toName - name));
if (from != NULL)
Relate(from, suff);
}
}












static bool
UpdateTarget(GNode *target, GNode **inout_main, Suffix *suff,
bool *inout_removedMain)
{
Suffix *srcSuff, *targSuff;
char *ptr;

if (*inout_main == NULL && *inout_removedMain &&
!(target->type & OP_NOTARGET)) {
DEBUG1(MAKE, "Setting main node to \"%s\"\n", target->name);
*inout_main = target;
Targ_SetMain(target);










return true;
}

if (target->type == OP_TRANSFORM)
return false;






ptr = strstr(target->name, suff->name);
if (ptr == NULL)
return false;









if (ptr == target->name)
return false;

if (ParseTransform(target->name, &srcSuff, &targSuff)) {
if (*inout_main == target) {
DEBUG1(MAKE,
"Setting main node from \"%s\" back to null\n",
target->name);
*inout_removedMain = true;
*inout_main = NULL;
Targ_SetMain(NULL);
}
Lst_Done(&target->children);
Lst_Init(&target->children);
target->type = OP_TRANSFORM;




DEBUG2(SUFF, "defining transformation from `%s' to `%s'\n",
srcSuff->name, targSuff->name);
Relate(srcSuff, targSuff);
}
return false;
}








static void
UpdateTargets(GNode **inout_main, Suffix *suff)
{
bool removedMain = false;
GNodeListNode *ln;

for (ln = Targ_List()->first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;
if (UpdateTarget(gn, inout_main, suff, &removedMain))
break;
}
}














void
Suff_AddSuffix(const char *name, GNode **inout_main)
{
GNodeListNode *ln;

Suffix *suff = FindSuffixByName(name);
if (suff != NULL)
return;

suff = Suffix_New(name);
Lst_Append(&sufflist, suff);
DEBUG1(SUFF, "Adding suffix \"%s\"\n", suff->name);

UpdateTargets(inout_main, suff);





for (ln = transforms.first; ln != NULL; ln = ln->next)
RebuildGraph(ln->datum, suff);
}


SearchPath *
Suff_GetPath(const char *sname)
{
Suffix *suff = FindSuffixByName(sname);
return suff != NULL ? suff->searchPath : NULL;
}














void
Suff_ExtendPaths(void)
{
SuffixListNode *ln;
char *flags;
SearchPath *includesPath = SearchPath_New();
SearchPath *libsPath = SearchPath_New();

for (ln = sufflist.first; ln != NULL; ln = ln->next) {
Suffix *suff = ln->datum;
if (!Lst_IsEmpty(&suff->searchPath->dirs)) {
#if defined(INCLUDES)
if (suff->include)
SearchPath_AddAll(includesPath,
suff->searchPath);
#endif
#if defined(LIBRARIES)
if (suff->library)
SearchPath_AddAll(libsPath, suff->searchPath);
#endif
SearchPath_AddAll(suff->searchPath, &dirSearchPath);
} else {
SearchPath_Free(suff->searchPath);
suff->searchPath = Dir_CopyDirSearchPath();
}
}

flags = SearchPath_ToFlags(includesPath, "-I");
Global_Set(".INCLUDES", flags);
free(flags);

flags = SearchPath_ToFlags(libsPath, "-L");
Global_Set(".LIBS", flags);
free(flags);

SearchPath_Free(includesPath);
SearchPath_Free(libsPath);
}







void
Suff_AddInclude(const char *suffName)
{
Suffix *suff = FindSuffixByName(suffName);
if (suff != NULL)
suff->include = true;
}







void
Suff_AddLib(const char *suffName)
{
Suffix *suff = FindSuffixByName(suffName);
if (suff != NULL)
suff->library = true;
}



static void
CandidateSearcher_Init(CandidateSearcher *cs)
{
Lst_Init(&cs->list);
}

static void
CandidateSearcher_Done(CandidateSearcher *cs)
{
Lst_Done(&cs->list);
}

static void
CandidateSearcher_Add(CandidateSearcher *cs, Candidate *cand)
{

Lst_Append(&cs->list, cand);
}

static void
CandidateSearcher_AddIfNew(CandidateSearcher *cs, Candidate *cand)
{

if (Lst_FindDatum(&cs->list, cand) == NULL)
Lst_Append(&cs->list, cand);
}

static void
CandidateSearcher_MoveAll(CandidateSearcher *cs, CandidateList *list)
{

Lst_MoveAll(&cs->list, list);
}


#if defined(DEBUG_SRC)
static void
CandidateList_PrintAddrs(CandidateList *list)
{
CandidateListNode *ln;

for (ln = list->first; ln != NULL; ln = ln->next) {
Candidate *cand = ln->datum;
debug_printf(" %p:%s", cand, cand->file);
}
debug_printf("\n");
}
#endif

static Candidate *
Candidate_New(char *name, char *prefix, Suffix *suff, Candidate *parent,
GNode *gn)
{
Candidate *cand = bmake_malloc(sizeof *cand);

cand->file = name;
cand->prefix = prefix;
cand->suff = Suffix_Ref(suff);
cand->parent = parent;
cand->node = gn;
cand->numChildren = 0;
#if defined(DEBUG_SRC)
Lst_Init(&cand->childrenList);
#endif

return cand;
}



static void
CandidateList_Add(CandidateList *list, char *srcName, Candidate *targ,
Suffix *suff, const char *debug_tag)
{
Candidate *cand = Candidate_New(srcName, targ->prefix, suff, targ,
NULL);
targ->numChildren++;
Lst_Append(list, cand);

#if defined(DEBUG_SRC)
Lst_Append(&targ->childrenList, cand);
debug_printf("%s add suff %p:%s candidate %p:%s to list %p:",
debug_tag, targ, targ->file, cand, cand->file, list);
CandidateList_PrintAddrs(list);
#endif
}





static void
CandidateList_AddCandidatesFor(CandidateList *list, Candidate *cand)
{
SuffixListNode *ln;
for (ln = cand->suff->children.first; ln != NULL; ln = ln->next) {
Suffix *suff = ln->datum;

if (suff->isNull && suff->name[0] != '\0') {





CandidateList_Add(list, bmake_strdup(cand->prefix),
cand, suff, "1");
}

CandidateList_Add(list, str_concat2(cand->prefix, suff->name),
cand, suff, "2");
}
}





static bool
RemoveCandidate(CandidateList *srcs)
{
CandidateListNode *ln;

#if defined(DEBUG_SRC)
debug_printf("cleaning list %p:", srcs);
CandidateList_PrintAddrs(srcs);
#endif

for (ln = srcs->first; ln != NULL; ln = ln->next) {
Candidate *src = ln->datum;

if (src->numChildren == 0) {
if (src->parent == NULL)
free(src->prefix);
else {
#if defined(DEBUG_SRC)

CandidateListNode *ln2;
ln2 = Lst_FindDatum(&src->parent->childrenList,
src);
if (ln2 != NULL)
Lst_Remove(&src->parent->childrenList,
ln2);
#endif
src->parent->numChildren--;
}
#if defined(DEBUG_SRC)
debug_printf("free: list %p src %p:%s children %d\n",
srcs, src, src->file, src->numChildren);
Lst_Done(&src->childrenList);
#endif
Lst_Remove(srcs, ln);
free(src->file);
free(src);
return true;
}
#if defined(DEBUG_SRC)
else {
debug_printf("keep: list %p src %p:%s children %d:",
srcs, src, src->file, src->numChildren);
CandidateList_PrintAddrs(&src->childrenList);
}
#endif
}

return false;
}


static Candidate *
FindThem(CandidateList *srcs, CandidateSearcher *cs)
{
HashSet seen;

HashSet_Init(&seen);

while (!Lst_IsEmpty(srcs)) {
Candidate *src = Lst_Dequeue(srcs);

#if defined(DEBUG_SRC)
debug_printf("remove from list %p src %p:%s\n",
srcs, src, src->file);
#endif
DEBUG1(SUFF, "\ttrying %s...", src->file);





if (Targ_FindNode(src->file) != NULL) {
found:
HashSet_Done(&seen);
DEBUG0(SUFF, "got it\n");
return src;
}

{
char *file = Dir_FindFile(src->file,
src->suff->searchPath);
if (file != NULL) {
free(file);
goto found;
}
}

DEBUG0(SUFF, "not there\n");

if (HashSet_Add(&seen, src->file))
CandidateList_AddCandidatesFor(srcs, src);
else {
DEBUG1(SUFF, "FindThem: skipping duplicate \"%s\"\n",
src->file);
}

CandidateSearcher_Add(cs, src);
}

HashSet_Done(&seen);
return NULL;
}






static Candidate *
FindCmds(Candidate *targ, CandidateSearcher *cs)
{
GNodeListNode *gln;
GNode *tgn;
GNode *sgn;
size_t prefLen;
Suffix *suff;
Candidate *ret;

tgn = targ->node;
prefLen = strlen(targ->prefix);

for (gln = tgn->children.first; gln != NULL; gln = gln->next) {
const char *base;

sgn = gln->datum;

if (sgn->type & OP_OPTIONAL && Lst_IsEmpty(&tgn->commands)) {







continue;
}

base = str_basename(sgn->name);
if (strncmp(base, targ->prefix, prefLen) != 0)
continue;

suff = FindSuffixByName(base + prefLen);
if (suff == NULL)
continue;








if (Lst_FindDatum(&suff->parents, targ->suff) != NULL)
break;
}

if (gln == NULL)
return NULL;

ret = Candidate_New(bmake_strdup(sgn->name), targ->prefix, suff, targ,
sgn);
targ->numChildren++;
#if defined(DEBUG_SRC)
debug_printf("3 add targ %p:%s ret %p:%s\n",
targ, targ->file, ret, ret->file);
Lst_Append(&targ->childrenList, ret);
#endif
CandidateSearcher_Add(cs, ret);
DEBUG1(SUFF, "\tusing existing source %s\n", sgn->name);
return ret;
}

static void
ExpandWildcards(GNodeListNode *cln, GNode *pgn)
{
GNode *cgn = cln->datum;
StringList expansions;

if (!Dir_HasWildcards(cgn->name))
return;




Lst_Init(&expansions);
SearchPath_Expand(Suff_FindPath(cgn), cgn->name, &expansions);

while (!Lst_IsEmpty(&expansions)) {
GNode *gn;



char *cp = Lst_Dequeue(&expansions);

DEBUG1(SUFF, "%s...", cp);
gn = Targ_GetNode(cp);


Lst_InsertBefore(&pgn->children, cln, gn);
Lst_Append(&gn->parents, pgn);
pgn->unmade++;
}

Lst_Done(&expansions);

DEBUG0(SUFF, "\n");





pgn->unmade--;
Lst_Remove(&pgn->children, cln);
Lst_Remove(&cgn->parents, Lst_FindDatum(&cgn->parents, pgn));
}








static void
ExpandChildrenRegular(char *cp, GNode *pgn, GNodeList *members)
{
char *start;

pp_skip_hspace(&cp);
start = cp;
while (*cp != '\0') {
if (*cp == ' ' || *cp == '\t') {
GNode *gn;




*cp++ = '\0';
gn = Targ_GetNode(start);
Lst_Append(members, gn);
pp_skip_hspace(&cp);

start = cp;
} else if (*cp == '$') {

const char *nested_p = cp;
FStr junk;

(void)Var_Parse(&nested_p, pgn, VARE_PARSE_ONLY, &junk);

if (junk.str == var_Error) {
Parse_Error(PARSE_FATAL,
"Malformed variable expression at \"%s\"",
cp);
cp++;
} else {
cp += nested_p - cp;
}

FStr_Done(&junk);
} else if (cp[0] == '\\' && cp[1] != '\0') {






cp += 2;
} else {
cp++;
}
}

if (cp != start) {



GNode *gn = Targ_GetNode(start);
Lst_Append(members, gn);
}
}












static void
ExpandChildren(GNodeListNode *cln, GNode *pgn)
{
GNode *cgn = cln->datum;
char *cp;

if (!Lst_IsEmpty(&cgn->order_pred) || !Lst_IsEmpty(&cgn->order_succ))

return;

if (cgn->type & OP_WAIT)

return;







if (strchr(cgn->name, '$') == NULL) {
ExpandWildcards(cln, pgn);
return;
}

DEBUG1(SUFF, "Expanding \"%s\"...", cgn->name);
(void)Var_Subst(cgn->name, pgn, VARE_UNDEFERR, &cp);


{
GNodeList members = LST_INIT;

if (cgn->type & OP_ARCHV) {





char *p = cp;
(void)Arch_ParseArchive(&p, &members, pgn);
} else {
ExpandChildrenRegular(cp, pgn, &members);
}




while (!Lst_IsEmpty(&members)) {
GNode *gn = Lst_Dequeue(&members);

DEBUG1(SUFF, "%s...", gn->name);




Lst_InsertBefore(&pgn->children, cln, gn);
Lst_Append(&gn->parents, pgn);
pgn->unmade++;

ExpandWildcards(cln->prev, pgn);
}
Lst_Done(&members);

free(cp);
}

DEBUG0(SUFF, "\n");





pgn->unmade--;
Lst_Remove(&pgn->children, cln);
Lst_Remove(&cgn->parents, Lst_FindDatum(&cgn->parents, pgn));
}

static void
ExpandAllChildren(GNode *gn)
{
GNodeListNode *ln, *nln;

for (ln = gn->children.first; ln != NULL; ln = nln) {
nln = ln->next;
ExpandChildren(ln, gn);
}
}













SearchPath *
Suff_FindPath(GNode *gn)
{
Suffix *suff = gn->suffix;

if (suff == NULL) {
char *name = gn->name;
size_t nameLen = strlen(gn->name);
SuffixListNode *ln;
for (ln = sufflist.first; ln != NULL; ln = ln->next)
if (Suffix_IsSuffix(ln->datum, nameLen, name + nameLen))
break;

DEBUG1(SUFF, "Wildcard expanding \"%s\"...", gn->name);
if (ln != NULL)
suff = ln->datum;




}

if (suff != NULL) {
DEBUG1(SUFF, "suffix is \"%s\"...\n", suff->name);
return suff->searchPath;
} else {
DEBUG0(SUFF, "\n");
return &dirSearchPath;
}
}












static bool
ApplyTransform(GNode *tgn, GNode *sgn, Suffix *tsuff, Suffix *ssuff)
{
GNodeListNode *ln;
char *tname;
GNode *gn;


Lst_Append(&tgn->children, sgn);
Lst_Append(&sgn->parents, tgn);
tgn->unmade++;


tname = str_concat2(ssuff->name, tsuff->name);
gn = FindTransformByName(tname);
free(tname);


if (gn == NULL)
return false;

DEBUG3(SUFF, "\tapplying %s -> %s to \"%s\"\n",
ssuff->name, tsuff->name, tgn->name);


ln = tgn->children.last;


Make_HandleUse(gn, tgn);


ln = ln != NULL ? ln->next : NULL;
while (ln != NULL) {
GNodeListNode *nln = ln->next;
ExpandChildren(ln, tgn);
ln = nln;
}





Lst_Append(&sgn->implicitParents, tgn);

return true;
}








static void
ExpandMember(GNode *gn, const char *eoarch, GNode *mem, Suffix *memSuff)
{
GNodeListNode *ln;
size_t nameLen = (size_t)(eoarch - gn->name);


for (ln = memSuff->parents.first; ln != NULL; ln = ln->next)
if (Suffix_IsSuffix(ln->datum, nameLen, eoarch))
break;

if (ln != NULL) {

Suffix *suff = ln->datum;
if (!ApplyTransform(gn, mem, suff, memSuff)) {
DEBUG2(SUFF, "\tNo transformation from %s -> %s\n",
memSuff->name, suff->name);
}
}
}

static void FindDeps(GNode *, CandidateSearcher *);










static void
FindDepsArchive(GNode *gn, CandidateSearcher *cs)
{
char *eoarch;
char *eoname;
GNode *mem;
Suffix *memSuff;
const char *name;





eoarch = strchr(gn->name, '(');
eoname = strchr(eoarch, ')');





assert(eoarch != NULL);
assert(eoname != NULL);

*eoname = '\0';
*eoarch = '\0';

name = eoarch + 1;








mem = Targ_GetNode(name);
FindDeps(mem, cs);


Lst_Append(&gn->children, mem);
Lst_Append(&mem->parents, gn);
gn->unmade++;


Var_Set(gn, PREFIX, GNode_VarPrefix(mem));
Var_Set(gn, TARGET, GNode_VarTarget(mem));

memSuff = mem->suffix;
if (memSuff == NULL) {
DEBUG0(SUFF, "using null suffix\n");
memSuff = nullSuff;
}



Var_Set(gn, MEMBER, name);
Var_Set(gn, ARCHIVE, gn->name);

Var_Set(gn, TARGET, gn->name);





ExpandAllChildren(gn);

if (memSuff != NULL)
ExpandMember(gn, eoarch, mem, memSuff);





*eoarch = '(';
*eoname = ')';






if (!GNode_IsTarget(gn))
gn->type |= OP_DEPENDS;






mem->type |= OP_MEMBER | OP_JOIN | OP_MADE;
}









static void
FindDepsLib(GNode *gn)
{
Suffix *suff = FindSuffixByName(LIBSUFF);
if (suff != NULL) {
Suffix_Reassign(&gn->suffix, suff);
Arch_FindLib(gn, suff->searchPath);
} else {
Suffix_Unassign(&gn->suffix);
Var_Set(gn, TARGET, gn->name);
}






Var_Set(gn, PREFIX, "");
}

static void
FindDepsRegularKnown(const char *name, size_t nameLen, GNode *gn,
CandidateList *srcs, CandidateList *targs)
{
SuffixListNode *ln;
Candidate *targ;
char *pref;

for (ln = sufflist.first; ln != NULL; ln = ln->next) {
Suffix *suff = ln->datum;
if (!Suffix_IsSuffix(suff, nameLen, name + nameLen))
continue;

pref = bmake_strldup(name, (size_t)(nameLen - suff->nameLen));
targ = Candidate_New(bmake_strdup(gn->name), pref, suff, NULL,
gn);

CandidateList_AddCandidatesFor(srcs, targ);


Lst_Append(targs, targ);
}
}

static void
FindDepsRegularUnknown(GNode *gn, const char *sopref,
CandidateList *srcs, CandidateList *targs)
{
Candidate *targ;

if (!Lst_IsEmpty(targs) || nullSuff == NULL)
return;

DEBUG1(SUFF, "\tNo known suffix on %s. Using .NULL suffix\n", gn->name);

targ = Candidate_New(bmake_strdup(gn->name), bmake_strdup(sopref),
nullSuff, NULL, gn);







if (Lst_IsEmpty(&gn->commands))
CandidateList_AddCandidatesFor(srcs, targ);
else {
DEBUG0(SUFF, "not ");
}

DEBUG0(SUFF, "adding suffix rules\n");

Lst_Append(targs, targ);
}







static void
FindDepsRegularPath(GNode *gn, Candidate *targ)
{
if (gn->type & (OP_PHONY | OP_NOPATH))
return;

free(gn->path);
gn->path = Dir_FindFile(gn->name,
(targ == NULL ? &dirSearchPath :
targ->suff->searchPath));
if (gn->path == NULL)
return;

Var_Set(gn, TARGET, gn->path);

if (targ != NULL) {




size_t savep = strlen(gn->path) - targ->suff->nameLen;
char savec;

Suffix_Reassign(&gn->suffix, targ->suff);

savec = gn->path[savep];
gn->path[savep] = '\0';

Var_Set(gn, PREFIX, str_basename(gn->path));

gn->path[savep] = savec;
} else {




Suffix_Unassign(&gn->suffix);
Var_Set(gn, PREFIX, str_basename(gn->path));
}
}










static void
FindDepsRegular(GNode *gn, CandidateSearcher *cs)
{

CandidateList srcs = LST_INIT;




CandidateList targs = LST_INIT;
Candidate *bottom;
Candidate *src;
Candidate *targ;

const char *name = gn->name;
size_t nameLen = strlen(name);

#if defined(DEBUG_SRC)
DEBUG1(SUFF, "FindDepsRegular \"%s\"\n", gn->name);
#endif




















bottom = NULL;
targ = NULL;

if (!(gn->type & OP_PHONY)) {

FindDepsRegularKnown(name, nameLen, gn, &srcs, &targs);


FindDepsRegularUnknown(gn, name, &srcs, &targs);






bottom = FindThem(&srcs, cs);

if (bottom == NULL) {




if (targs.first != NULL)
targ = targs.first->datum;
else
targ = NULL;
} else {




for (targ = bottom;
targ->parent != NULL; targ = targ->parent)
continue;
}
}

Var_Set(gn, TARGET, GNode_Path(gn));
Var_Set(gn, PREFIX, targ != NULL ? targ->prefix : gn->name);





{
GNodeListNode *ln, *nln;
for (ln = gn->children.first; ln != NULL; ln = nln) {
nln = ln->next;
ExpandChildren(ln, gn);
}
}

if (targ == NULL) {
DEBUG1(SUFF, "\tNo valid suffix on %s\n", gn->name);

sfnd_abort:
FindDepsRegularPath(gn, targ);
goto sfnd_return;
}





if (targ->suff->library)
gn->type |= OP_LIB;




if (!Lst_IsEmpty(&gn->children)) {
src = FindCmds(targ, cs);

if (src != NULL) {




while (bottom != NULL && bottom->parent != NULL) {
CandidateSearcher_AddIfNew(cs, bottom);
bottom = bottom->parent;
}
bottom = src;
}
}

if (bottom == NULL) {

goto sfnd_abort;
}













if (bottom->node == NULL)
bottom->node = Targ_GetNode(bottom->file);

for (src = bottom; src->parent != NULL; src = src->parent) {
targ = src->parent;

Suffix_Reassign(&src->node->suffix, src->suff);

if (targ->node == NULL)
targ->node = Targ_GetNode(targ->file);

ApplyTransform(targ->node, src->node,
targ->suff, src->suff);

if (targ->node != gn) {









targ->node->type |= OP_DEPS_FOUND;
Var_Set(targ->node, PREFIX, targ->prefix);
Var_Set(targ->node, TARGET, targ->node->name);
}
}

Suffix_Reassign(&gn->suffix, src->suff);





sfnd_return:
if (bottom != NULL)
CandidateSearcher_AddIfNew(cs, bottom);

while (RemoveCandidate(&srcs) || RemoveCandidate(&targs))
continue;

CandidateSearcher_MoveAll(cs, &srcs);
CandidateSearcher_MoveAll(cs, &targs);
}

static void
CandidateSearcher_CleanUp(CandidateSearcher *cs)
{
while (RemoveCandidate(&cs->list))
continue;
assert(Lst_IsEmpty(&cs->list));
}

















void
Suff_FindDeps(GNode *gn)
{
CandidateSearcher cs;

CandidateSearcher_Init(&cs);

FindDeps(gn, &cs);

CandidateSearcher_CleanUp(&cs);
CandidateSearcher_Done(&cs);
}

static void
FindDeps(GNode *gn, CandidateSearcher *cs)
{
if (gn->type & OP_DEPS_FOUND)
return;
gn->type |= OP_DEPS_FOUND;


Var_Set(gn, TARGET, GNode_Path(gn));
Var_Set(gn, PREFIX, gn->name);

DEBUG1(SUFF, "SuffFindDeps \"%s\"\n", gn->name);

if (gn->type & OP_ARCHV)
FindDepsArchive(gn, cs);
else if (gn->type & OP_LIB)
FindDepsLib(gn);
else
FindDepsRegular(gn, cs);
}










void
Suff_SetNull(const char *name)
{
Suffix *suff = FindSuffixByName(name);
if (suff == NULL) {
Parse_Error(PARSE_WARNING,
"Desired null suffix %s not defined",
name);
return;
}

if (nullSuff != NULL)
nullSuff->isNull = false;
suff->isNull = true;

nullSuff = suff;
}


void
Suff_Init(void)
{





Suff_ClearSuffixes();
}



void
Suff_End(void)
{
#if defined(CLEANUP)
Lst_DoneCall(&sufflist, SuffFree);
Lst_DoneCall(&suffClean, SuffFree);
if (nullSuff != NULL)
SuffFree(nullSuff);
Lst_Done(&transforms);
#endif
}


static void
PrintSuffNames(const char *prefix, const SuffixList *suffs)
{
SuffixListNode *ln;

debug_printf("#\t%s: ", prefix);
for (ln = suffs->first; ln != NULL; ln = ln->next) {
const Suffix *suff = ln->datum;
debug_printf("%s ", suff->name);
}
debug_printf("\n");
}

static void
Suffix_Print(const Suffix *suff)
{
Buffer buf;

Buf_InitSize(&buf, 16);
Buf_AddFlag(&buf, suff->include, "SUFF_INCLUDE");
Buf_AddFlag(&buf, suff->library, "SUFF_LIBRARY");
Buf_AddFlag(&buf, suff->isNull, "SUFF_NULL");

debug_printf("#\"%s\" (num %d, ref %d)",
suff->name, suff->sNum, suff->refCount);
if (buf.len > 0)
debug_printf(" (%s)", buf.data);
debug_printf("\n");

Buf_Done(&buf);

PrintSuffNames("To", &suff->parents);
PrintSuffNames("From", &suff->children);

debug_printf("#\tSearch Path: ");
SearchPath_Print(suff->searchPath);
debug_printf("\n");
}

static void
PrintTransformation(GNode *t)
{
debug_printf("%-16s:", t->name);
Targ_PrintType(t->type);
debug_printf("\n");
Targ_PrintCmds(t);
debug_printf("\n");
}

void
Suff_PrintAll(void)
{
debug_printf("#*** Suffixes:\n");
{
SuffixListNode *ln;
for (ln = sufflist.first; ln != NULL; ln = ln->next)
Suffix_Print(ln->datum);
}

debug_printf("#*** Transformations:\n");
{
GNodeListNode *ln;
for (ln = transforms.first; ln != NULL; ln = ln->next)
PrintTransformation(ln->datum);
}
}

const char *
Suff_NamesStr(void)
{
Buffer buf;
SuffixListNode *ln;
Suffix *suff;

Buf_InitSize(&buf, 16);
for (ln = sufflist.first; ln != NULL; ln = ln->next) {
suff = ln->datum;
if (ln != sufflist.first)
Buf_AddByte(&buf, ' ');
Buf_AddStr(&buf, suff->name);
}
return Buf_DoneData(&buf);
}
