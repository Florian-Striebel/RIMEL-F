
































































































































#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>

#include "make.h"
#include "dir.h"
#include "job.h"


MAKE_RCSID("$NetBSD: dir.c,v 1.275 2021/11/28 21:46:17 rillig Exp $");







































































struct CachedDir {








char *name;






int refCount;


int hits;


HashSet files;
};

typedef List CachedDirList;
typedef ListNode CachedDirListNode;

typedef ListNode SearchPathNode;


typedef struct OpenDirs {
CachedDirList list;
HashTable table;
} OpenDirs;


SearchPath dirSearchPath = { LST_INIT };

static OpenDirs openDirs;





static int hits;
static int misses;
static int nearmisses;
static int bigmisses;


static CachedDir *dot = NULL;

static CachedDir *cur = NULL;

static CachedDir *dotLast = NULL;










static HashTable mtimes;

static HashTable lmtimes;


static void OpenDirs_Remove(OpenDirs *, const char *);


static CachedDir *
CachedDir_New(const char *name)
{
CachedDir *dir = bmake_malloc(sizeof *dir);

dir->name = bmake_strdup(name);
dir->refCount = 0;
dir->hits = 0;
HashSet_Init(&dir->files);

#if defined(DEBUG_REFCNT)
DEBUG2(DIR, "CachedDir %p new for \"%s\"\n", dir, dir->name);
#endif

return dir;
}

static CachedDir *
CachedDir_Ref(CachedDir *dir)
{
dir->refCount++;

#if defined(DEBUG_REFCNT)
DEBUG3(DIR, "CachedDir %p ++ %d for \"%s\"\n",
dir, dir->refCount, dir->name);
#endif

return dir;
}

static void
CachedDir_Unref(CachedDir *dir)
{
dir->refCount--;

#if defined(DEBUG_REFCNT)
DEBUG3(DIR, "CachedDir %p -- %d for \"%s\"\n",
dir, dir->refCount, dir->name);
#endif

if (dir->refCount > 0)
return;

#if defined(DEBUG_REFCNT)
DEBUG2(DIR, "CachedDir %p free for \"%s\"\n", dir, dir->name);
#endif

OpenDirs_Remove(&openDirs, dir->name);

free(dir->name);
HashSet_Done(&dir->files);
free(dir);
}


static void
CachedDir_Assign(CachedDir **var, CachedDir *dir)
{
CachedDir *prev;

prev = *var;
*var = dir;
if (dir != NULL)
CachedDir_Ref(dir);
if (prev != NULL)
CachedDir_Unref(prev);
}

static void
OpenDirs_Init(OpenDirs *odirs)
{
Lst_Init(&odirs->list);
HashTable_Init(&odirs->table);
}

#if defined(CLEANUP)
static void
OpenDirs_Done(OpenDirs *odirs)
{
CachedDirListNode *ln = odirs->list.first;
DEBUG1(DIR, "OpenDirs_Done: %u entries to remove\n",
odirs->table.numEntries);
while (ln != NULL) {
CachedDirListNode *next = ln->next;
CachedDir *dir = ln->datum;
DEBUG2(DIR, "OpenDirs_Done: refCount %d for \"%s\"\n",
dir->refCount, dir->name);
CachedDir_Unref(dir);
ln = next;
}
Lst_Done(&odirs->list);
HashTable_Done(&odirs->table);
}
#endif

static CachedDir *
OpenDirs_Find(OpenDirs *odirs, const char *name)
{
CachedDirListNode *ln = HashTable_FindValue(&odirs->table, name);
return ln != NULL ? ln->datum : NULL;
}

static void
OpenDirs_Add(OpenDirs *odirs, CachedDir *cdir)
{
if (HashTable_FindEntry(&odirs->table, cdir->name) != NULL)
return;
Lst_Append(&odirs->list, cdir);
HashTable_Set(&odirs->table, cdir->name, odirs->list.last);
}

static void
OpenDirs_Remove(OpenDirs *odirs, const char *name)
{
HashEntry *he = HashTable_FindEntry(&odirs->table, name);
CachedDirListNode *ln;
if (he == NULL)
return;
ln = HashEntry_Get(he);
HashTable_DeleteEntry(&odirs->table, he);
Lst_Remove(&odirs->list, ln);
}





static int
cached_stats(const char *pathname, struct cached_stat *out_cst,
bool useLstat, bool forceRefresh)
{
HashTable *tbl = useLstat ? &lmtimes : &mtimes;
struct stat sys_st;
struct cached_stat *cst;
int rc;

if (pathname == NULL || pathname[0] == '\0')
return -1;

cst = HashTable_FindValue(tbl, pathname);
if (cst != NULL && !forceRefresh) {
*out_cst = *cst;
DEBUG2(DIR, "Using cached time %s for %s\n",
Targ_FmtTime(cst->cst_mtime), pathname);
return 0;
}

rc = (useLstat ? lstat : stat)(pathname, &sys_st);
if (rc == -1)
return -1;

if (sys_st.st_mtime == 0)
sys_st.st_mtime = 1;

if (cst == NULL) {
cst = bmake_malloc(sizeof *cst);
HashTable_Set(tbl, pathname, cst);
}

cst->cst_mtime = sys_st.st_mtime;
cst->cst_mode = sys_st.st_mode;

*out_cst = *cst;
DEBUG2(DIR, " Caching %s for %s\n",
Targ_FmtTime(sys_st.st_mtime), pathname);

return 0;
}

int
cached_stat(const char *pathname, struct cached_stat *cst)
{
return cached_stats(pathname, cst, false, false);
}

int
cached_lstat(const char *pathname, struct cached_stat *cst)
{
return cached_stats(pathname, cst, true, false);
}


void
Dir_Init(void)
{
OpenDirs_Init(&openDirs);
HashTable_Init(&mtimes);
HashTable_Init(&lmtimes);
CachedDir_Assign(&dotLast, CachedDir_New(".DOTLAST"));
}




void
Dir_InitCur(const char *newCurdir)
{
CachedDir *dir;

if (newCurdir == NULL)
return;





dir = SearchPath_Add(NULL, newCurdir);
if (dir == NULL)
return;

CachedDir_Assign(&cur, dir);
}





void
Dir_InitDot(void)
{
CachedDir *dir;

dir = SearchPath_Add(NULL, ".");
if (dir == NULL) {
Error("Cannot open `.' (%s)", strerror(errno));
exit(2);
}

CachedDir_Assign(&dot, dir);

Dir_SetPATH();
}


void
Dir_End(void)
{
#if defined(CLEANUP)
CachedDir_Assign(&cur, NULL);
CachedDir_Assign(&dot, NULL);
CachedDir_Assign(&dotLast, NULL);
SearchPath_Clear(&dirSearchPath);
OpenDirs_Done(&openDirs);
HashTable_Done(&mtimes);
HashTable_Done(&lmtimes);
#endif
}






void
Dir_SetPATH(void)
{
CachedDirListNode *ln;
bool seenDotLast = false;

Global_Delete(".PATH");

if ((ln = dirSearchPath.dirs.first) != NULL) {
CachedDir *dir = ln->datum;
if (dir == dotLast) {
seenDotLast = true;
Global_Append(".PATH", dotLast->name);
}
}

if (!seenDotLast) {
if (dot != NULL)
Global_Append(".PATH", dot->name);
if (cur != NULL)
Global_Append(".PATH", cur->name);
}

for (ln = dirSearchPath.dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
if (dir == dotLast)
continue;
if (dir == dot && seenDotLast)
continue;
Global_Append(".PATH", dir->name);
}

if (seenDotLast) {
if (dot != NULL)
Global_Append(".PATH", dot->name);
if (cur != NULL)
Global_Append(".PATH", cur->name);
}
}











bool
Dir_HasWildcards(const char *name)
{
const char *p;
bool wild = false;
int braces = 0, brackets = 0;

for (p = name; *p != '\0'; p++) {
switch (*p) {
case '{':
braces++;
wild = true;
break;
case '}':
braces--;
break;
case '[':
brackets++;
wild = true;
break;
case ']':
brackets--;
break;
case '?':
case '*':
wild = true;
break;
default:
break;
}
}
return wild && brackets == 0 && braces == 0;
}

















static void
DirMatchFiles(const char *pattern, CachedDir *dir, StringList *expansions)
{
const char *dirName = dir->name;
bool isDot = dirName[0] == '.' && dirName[1] == '\0';
HashIter hi;







HashIter_InitSet(&hi, &dir->files);
while (HashIter_Next(&hi) != NULL) {
const char *base = hi.entry->key;

if (!Str_Match(base, pattern))
continue;











if (base[0] == '.' && pattern[0] != '.')
continue;

{
char *fullName = isDot
? bmake_strdup(base)
: str_concat3(dirName, "/", base);
Lst_Append(expansions, fullName);
}
}
}





static const char *
closing_brace(const char *p)
{
int nest = 0;
while (*p != '\0') {
if (*p == '}' && nest == 0)
break;
if (*p == '{')
nest++;
if (*p == '}')
nest--;
p++;
}
return p;
}





static const char *
separator_comma(const char *p)
{
int nest = 0;
while (*p != '\0') {
if ((*p == '}' || *p == ',') && nest == 0)
break;
if (*p == '{')
nest++;
if (*p == '}')
nest--;
p++;
}
return p;
}

static bool
contains_wildcard(const char *p)
{
for (; *p != '\0'; p++) {
switch (*p) {
case '*':
case '?':
case '{':
case '[':
return true;
}
}
return false;
}

static char *
concat3(const char *a, size_t a_len, const char *b, size_t b_len,
const char *c, size_t c_len)
{
size_t s_len = a_len + b_len + c_len;
char *s = bmake_malloc(s_len + 1);
memcpy(s, a, a_len);
memcpy(s + a_len, b, b_len);
memcpy(s + a_len + b_len, c, c_len);
s[s_len] = '\0';
return s;
}

















static void
DirExpandCurly(const char *word, const char *brace, SearchPath *path,
StringList *expansions)
{
const char *prefix, *middle, *piece, *middle_end, *suffix;
size_t prefix_len, suffix_len;



middle = brace + 1;
middle_end = closing_brace(middle);
if (*middle_end == '\0') {
Error("Unterminated {} clause \"%s\"", middle);
return;
}

prefix = word;
prefix_len = (size_t)(brace - prefix);
suffix = middle_end + 1;
suffix_len = strlen(suffix);



piece = middle;
while (piece < middle_end + 1) {
const char *piece_end = separator_comma(piece);
size_t piece_len = (size_t)(piece_end - piece);

char *file = concat3(prefix, prefix_len, piece, piece_len,
suffix, suffix_len);

if (contains_wildcard(file)) {
SearchPath_Expand(path, file, expansions);
free(file);
} else {
Lst_Append(expansions, file);
}


piece = piece_end + 1;
}
}



static void
DirExpandPath(const char *word, SearchPath *path, StringList *expansions)
{
SearchPathNode *ln;
for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
DirMatchFiles(word, dir, expansions);
}
}

static void
PrintExpansions(StringList *expansions)
{
const char *sep = "";
StringListNode *ln;
for (ln = expansions->first; ln != NULL; ln = ln->next) {
const char *word = ln->datum;
debug_printf("%s%s", sep, word);
sep = " ";
}
debug_printf("\n");
}





static void
SearchPath_ExpandMiddle(SearchPath *path, const char *pattern,
const char *wildcardComponent, StringList *expansions)
{
char *prefix, *dirpath, *end;
SearchPath *partPath;

prefix = bmake_strsedup(pattern, wildcardComponent + 1);










dirpath = Dir_FindFile(prefix, path);
free(prefix);










if (dirpath == NULL)
return;

end = &dirpath[strlen(dirpath) - 1];

if (*end == '/')
*end = '\0';

partPath = SearchPath_New();
(void)SearchPath_Add(partPath, dirpath);
DirExpandPath(wildcardComponent + 1, partPath, expansions);
SearchPath_Free(partPath);
}










void
SearchPath_Expand(SearchPath *path, const char *pattern, StringList *expansions)
{
const char *brace, *slash, *wildcard, *wildcardComponent;

assert(path != NULL);
assert(expansions != NULL);

DEBUG1(DIR, "Expanding \"%s\"... ", pattern);

brace = strchr(pattern, '{');
if (brace != NULL) {
DirExpandCurly(pattern, brace, path, expansions);
goto done;
}



slash = strchr(pattern, '/');
if (slash == NULL) {



DirMatchFiles(pattern, dot, expansions);

DirExpandPath(pattern, path, expansions);
goto done;
}




for (wildcard = pattern; *wildcard != '\0'; wildcard++)
if (*wildcard == '?' || *wildcard == '[' || *wildcard == '*')
break;

if (*wildcard == '\0') {





DirExpandPath(pattern, path, expansions);
goto done;
}



wildcardComponent = wildcard;
while (wildcardComponent > pattern && *wildcardComponent != '/')
wildcardComponent--;

if (wildcardComponent == pattern) {


DirExpandPath(pattern, path, expansions);
} else {
SearchPath_ExpandMiddle(path, pattern, wildcardComponent,
expansions);
}

done:
if (DEBUG(DIR))
PrintExpansions(expansions);
}





static char *
DirLookup(CachedDir *dir, const char *base)
{
char *file;

DEBUG1(DIR, " %s ...\n", dir->name);

if (!HashSet_Contains(&dir->files, base))
return NULL;

file = str_concat3(dir->name, "/", base);
DEBUG1(DIR, " returning %s\n", file);
dir->hits++;
hits++;
return file;
}






static char *
DirLookupSubdir(CachedDir *dir, const char *name)
{
struct cached_stat cst;
char *file = dir == dot ? bmake_strdup(name)
: str_concat3(dir->name, "/", name);

DEBUG1(DIR, "checking %s ...\n", file);

if (cached_stat(file, &cst) == 0) {
nearmisses++;
return file;
}
free(file);
return NULL;
}






static char *
DirLookupAbs(CachedDir *dir, const char *name, const char *cp)
{
const char *dnp;
const char *np;

DEBUG1(DIR, " %s ...\n", dir->name);







for (dnp = dir->name, np = name;
*dnp != '\0' && *dnp == *np; dnp++, np++)
continue;
if (*dnp != '\0' || np != cp - 1)
return NULL;

if (!HashSet_Contains(&dir->files, cp)) {
DEBUG0(DIR, " must be here but isn't -- returning\n");
return bmake_strdup("");
}

dir->hits++;
hits++;
DEBUG1(DIR, " returning %s\n", name);
return bmake_strdup(name);
}





static char *
DirFindDot(const char *name, const char *base)
{

if (HashSet_Contains(&dot->files, base)) {
DEBUG0(DIR, " in '.'\n");
hits++;
dot->hits++;
return bmake_strdup(name);
}

if (cur != NULL && HashSet_Contains(&cur->files, base)) {
DEBUG1(DIR, " in ${.CURDIR} = %s\n", cur->name);
hits++;
cur->hits++;
return str_concat3(cur->name, "/", base);
}

return NULL;
}

static bool
FindFileRelative(SearchPath *path, bool seenDotLast,
const char *name, char **out_file)
{
SearchPathNode *ln;
bool checkedDot = false;
char *file;

DEBUG0(DIR, " Trying subdirectories...\n");

if (!seenDotLast) {
if (dot != NULL) {
checkedDot = true;
if ((file = DirLookupSubdir(dot, name)) != NULL)
goto found;
}
if (cur != NULL &&
(file = DirLookupSubdir(cur, name)) != NULL)
goto found;
}

for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
if (dir == dotLast)
continue;
if (dir == dot) {
if (checkedDot)
continue;
checkedDot = true;
}
if ((file = DirLookupSubdir(dir, name)) != NULL)
goto found;
}

if (seenDotLast) {
if (dot != NULL && !checkedDot) {
checkedDot = true;
if ((file = DirLookupSubdir(dot, name)) != NULL)
goto found;
}
if (cur != NULL &&
(file = DirLookupSubdir(cur, name)) != NULL)
goto found;
}

if (checkedDot) {




DEBUG0(DIR, " Checked . already, returning NULL\n");
file = NULL;
goto found;
}

return false;

found:
*out_file = file;
return true;
}

static bool
FindFileAbsolute(SearchPath *path, bool seenDotLast,
const char *name, const char *base, char **out_file)
{
char *file;
SearchPathNode *ln;











DEBUG0(DIR, " Trying exact path matches...\n");

if (!seenDotLast && cur != NULL &&
((file = DirLookupAbs(cur, name, base)) != NULL))
goto found;

for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
if (dir == dotLast)
continue;
if ((file = DirLookupAbs(dir, name, base)) != NULL)
goto found;
}

if (seenDotLast && cur != NULL &&
((file = DirLookupAbs(cur, name, base)) != NULL))
goto found;

return false;

found:
if (file[0] == '\0') {
free(file);
file = NULL;
}
*out_file = file;
return true;
}


















char *
Dir_FindFile(const char *name, SearchPath *path)
{
char *file;
bool seenDotLast = false;
struct cached_stat cst;
const char *trailing_dot = ".";
const char *base = str_basename(name);

DEBUG1(DIR, "Searching for %s ...", name);

if (path == NULL) {
DEBUG0(DIR, "couldn't open path, file not found\n");
misses++;
return NULL;
}

if (path->dirs.first != NULL) {
CachedDir *dir = path->dirs.first->datum;
if (dir == dotLast) {
seenDotLast = true;
DEBUG0(DIR, "[dot last]...");
}
}
DEBUG0(DIR, "\n");






if (base == name || (base - name == 2 && *name == '.')) {
SearchPathNode *ln;















if (!seenDotLast && (file = DirFindDot(name, base)) != NULL)
return file;

for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
if (dir == dotLast)
continue;
if ((file = DirLookup(dir, base)) != NULL)
return file;
}

if (seenDotLast && (file = DirFindDot(name, base)) != NULL)
return file;
}















if (base == name) {
DEBUG0(DIR, " failed.\n");
misses++;
return NULL;
}

if (*base == '\0') {

base = trailing_dot;
}

if (name[0] != '/') {
if (FindFileRelative(path, seenDotLast, name, &file))
return file;
} else {
if (FindFileAbsolute(path, seenDotLast, name, base, &file))
return file;
}



















#if 0
{
CachedDir *dir;
char *prefix;

if (base == trailing_dot) {
base = strrchr(name, '/');
base++;
}
prefix = bmake_strsedup(name, base - 1);
(void)SearchPath_Add(path, prefix);
free(prefix);

bigmisses++;
if (path->last == NULL)
return NULL;

dir = path->last->datum;
if (HashSet_Contains(&dir->files, base))
return bmake_strdup(name);
return NULL;
}
#else
DEBUG1(DIR, " Looking for \"%s\" ...\n", name);

bigmisses++;
if (cached_stat(name, &cst) == 0) {
return bmake_strdup(name);
}

DEBUG0(DIR, " failed. Returning NULL\n");
return NULL;
#endif
}













char *
Dir_FindHereOrAbove(const char *here, const char *search_path)
{
struct cached_stat cst;
char *dirbase, *dirbase_end;
char *try, *try_end;


dirbase = bmake_strdup(here);
dirbase_end = dirbase + strlen(dirbase);


for (;;) {


try = str_concat3(dirbase, "/", search_path);
if (cached_stat(try, &cst) != -1) {




if ((cst.cst_mode & S_IFMT) != S_IFDIR) {
try_end = try + strlen(try);
while (try_end > try && *try_end != '/')
try_end--;
if (try_end > try)
*try_end = '\0';
}

free(dirbase);
return try;
}
free(try);





if (dirbase_end == dirbase)
break;




while (dirbase_end > dirbase && *dirbase_end != '/')
dirbase_end--;
*dirbase_end = '\0';
}

free(dirbase);
return NULL;
}





static char *
ResolveMovedDepends(GNode *gn)
{
char *fullName;

const char *base = str_basename(gn->name);
if (base == gn->name)
return NULL;

fullName = Dir_FindFile(base, Suff_FindPath(gn));
if (fullName == NULL)
return NULL;








gn->path = bmake_strdup(fullName);
if (!Job_RunTarget(".STALE", gn->fname))
fprintf(stdout,
"%s: %s, %d: ignoring stale %s for %s, found %s\n",
progname, gn->fname, gn->lineno,
makeDependfile, gn->name, fullName);

return fullName;
}

static char *
ResolveFullName(GNode *gn)
{
char *fullName;

fullName = gn->path;
if (fullName == NULL && !(gn->type & OP_NOPATH)) {

fullName = Dir_FindFile(gn->name, Suff_FindPath(gn));

if (fullName == NULL && gn->flags.fromDepend &&
!Lst_IsEmpty(&gn->implicitParents))
fullName = ResolveMovedDepends(gn);

DEBUG2(DIR, "Found '%s' as '%s'\n",
gn->name, fullName != NULL ? fullName : "(not found)");
}

if (fullName == NULL)
fullName = bmake_strdup(gn->name);



return fullName;
}







void
Dir_UpdateMTime(GNode *gn, bool forceRefresh)
{
char *fullName;
struct cached_stat cst;

if (gn->type & OP_ARCHV) {
Arch_UpdateMTime(gn);
return;
}

if (gn->type & OP_PHONY) {
gn->mtime = 0;
return;
}

fullName = ResolveFullName(gn);

if (cached_stats(fullName, &cst, false, forceRefresh) < 0) {
if (gn->type & OP_MEMBER) {
if (fullName != gn->path)
free(fullName);
Arch_UpdateMemberMTime(gn);
return;
}

cst.cst_mtime = 0;
}

if (fullName != NULL && gn->path == NULL)
gn->path = fullName;


gn->mtime = cst.cst_mtime;
}





static CachedDir *
CacheNewDir(const char *name, SearchPath *path)
{
CachedDir *dir = NULL;
DIR *d;
struct dirent *dp;

if ((d = opendir(name)) == NULL) {
DEBUG1(DIR, "Caching %s ... not found\n", name);
return dir;
}

DEBUG1(DIR, "Caching %s ...\n", name);

dir = CachedDir_New(name);

while ((dp = readdir(d)) != NULL) {

#if defined(sun) && defined(d_ino)





if (dp->d_fileno == 0)
continue;
#endif

(void)HashSet_Add(&dir->files, dp->d_name);
}
(void)closedir(d);

OpenDirs_Add(&openDirs, dir);
if (path != NULL)
Lst_Append(&path->dirs, CachedDir_Ref(dir));

DEBUG1(DIR, "Caching %s done\n", name);
return dir;
}



















CachedDir *
SearchPath_Add(SearchPath *path, const char *name)
{

if (path != NULL && strcmp(name, ".DOTLAST") == 0) {
SearchPathNode *ln;


for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *pathDir = ln->datum;
if (strcmp(pathDir->name, name) == 0)
return pathDir;
}

Lst_Prepend(&path->dirs, CachedDir_Ref(dotLast));
}

if (path != NULL) {

CachedDir *dir = OpenDirs_Find(&openDirs, name);
if (dir != NULL) {
if (Lst_FindDatum(&path->dirs, dir) == NULL)
Lst_Append(&path->dirs, CachedDir_Ref(dir));
return dir;
}
}

return CacheNewDir(name, path);
}





SearchPath *
Dir_CopyDirSearchPath(void)
{
SearchPath *path = SearchPath_New();
SearchPathNode *ln;
for (ln = dirSearchPath.dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
Lst_Append(&path->dirs, CachedDir_Ref(dir));
}
return path;
}















char *
SearchPath_ToFlags(SearchPath *path, const char *flag)
{
Buffer buf;
SearchPathNode *ln;

Buf_Init(&buf);

if (path != NULL) {
for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
Buf_AddStr(&buf, " ");
Buf_AddStr(&buf, flag);
Buf_AddStr(&buf, dir->name);
}
}

return Buf_DoneData(&buf);
}


void
SearchPath_Free(SearchPath *path)
{
SearchPathNode *ln;

for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
CachedDir_Unref(dir);
}
Lst_Done(&path->dirs);
free(path);
}





void
SearchPath_Clear(SearchPath *path)
{
while (!Lst_IsEmpty(&path->dirs)) {
CachedDir *dir = Lst_Dequeue(&path->dirs);
CachedDir_Unref(dir);
}
}






void
SearchPath_AddAll(SearchPath *dst, SearchPath *src)
{
SearchPathNode *ln;

for (ln = src->dirs.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
if (Lst_FindDatum(&dst->dirs, dir) == NULL)
Lst_Append(&dst->dirs, CachedDir_Ref(dir));
}
}

static int
percentage(int num, int den)
{
return den != 0 ? num * 100 / den : 0;
}


void
Dir_PrintDirectories(void)
{
CachedDirListNode *ln;

debug_printf("#*** Directory Cache:\n");
debug_printf(
"#Stats: %d hits %d misses %d near misses %d losers (%d%%)\n",
hits, misses, nearmisses, bigmisses,
percentage(hits, hits + bigmisses + nearmisses));
debug_printf("#refs hits directory\n");

for (ln = openDirs.list.first; ln != NULL; ln = ln->next) {
CachedDir *dir = ln->datum;
debug_printf("#%4d %4d %s\n",
dir->refCount, dir->hits, dir->name);
}
}

void
SearchPath_Print(const SearchPath *path)
{
SearchPathNode *ln;

for (ln = path->dirs.first; ln != NULL; ln = ln->next) {
const CachedDir *dir = ln->datum;
debug_printf("%s ", dir->name);
}
}
