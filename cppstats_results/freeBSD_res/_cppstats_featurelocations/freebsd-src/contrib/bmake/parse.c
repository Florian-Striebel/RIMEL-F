


































































































#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>

#include "make.h"

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#if defined(HAVE_MMAP)
#include <sys/mman.h>

#if !defined(MAP_COPY)
#define MAP_COPY MAP_PRIVATE
#endif
#if !defined(MAP_FILE)
#define MAP_FILE 0
#endif
#endif

#include "dir.h"
#include "job.h"
#include "pathnames.h"


MAKE_RCSID("$NetBSD: parse.c,v 1.574 2021/12/12 15:44:41 rillig Exp $");






typedef struct IFile {
char *fname;
bool fromForLoop;
int lineno;
int first_lineno;
unsigned int cond_depth;
bool depending;






char *buf_freeIt;
char *buf_ptr;
char *buf_end;


ReadMoreProc readMore;
void *readMoreArg;

struct loadedfile *lf;
} IFile;




typedef enum ParseSpecial {
SP_ATTRIBUTE,
SP_BEGIN,
SP_DEFAULT,
SP_DELETE_ON_ERROR,
SP_END,
SP_ERROR,
SP_IGNORE,
SP_INCLUDES,
SP_INTERRUPT,
SP_LIBS,

SP_MAIN,
SP_META,
SP_MFLAGS,
SP_NOMETA,
SP_NOMETA_CMP,
SP_NOPATH,
SP_NOT,
SP_NOTPARALLEL,
SP_NULL,
SP_OBJDIR,
SP_ORDER,
SP_PARALLEL,
SP_PATH,
SP_PHONY,
#if defined(POSIX)
SP_POSIX,
#endif
SP_PRECIOUS,
SP_SHELL,
SP_SILENT,
SP_SINGLESHELL,
SP_STALE,
SP_SUFFIXES,
SP_WAIT
} ParseSpecial;

typedef List SearchPathList;
typedef ListNode SearchPathListNode;







static GNode *mainNode;










static GNodeList *targets;

#if defined(CLEANUP)






static StringList targCmds = LST_INIT;
#endif





static GNode *order_pred;




static int parseErrors = 0;












static Vector includes;

static IFile *
GetInclude(size_t i)
{
return Vector_Get(&includes, i);
}


static IFile *
CurFile(void)
{
return GetInclude(includes.len - 1);
}


SearchPath *parseIncPath;
SearchPath *sysIncPath;
SearchPath *defSysIncPath;










static const struct {
const char name[17];
ParseSpecial spec;
GNodeType op;
} parseKeywords[] = {
{ ".BEGIN", SP_BEGIN, OP_NONE },
{ ".DEFAULT", SP_DEFAULT, OP_NONE },
{ ".DELETE_ON_ERROR", SP_DELETE_ON_ERROR, OP_NONE },
{ ".END", SP_END, OP_NONE },
{ ".ERROR", SP_ERROR, OP_NONE },
{ ".EXEC", SP_ATTRIBUTE, OP_EXEC },
{ ".IGNORE", SP_IGNORE, OP_IGNORE },
{ ".INCLUDES", SP_INCLUDES, OP_NONE },
{ ".INTERRUPT", SP_INTERRUPT, OP_NONE },
{ ".INVISIBLE", SP_ATTRIBUTE, OP_INVISIBLE },
{ ".JOIN", SP_ATTRIBUTE, OP_JOIN },
{ ".LIBS", SP_LIBS, OP_NONE },
{ ".MADE", SP_ATTRIBUTE, OP_MADE },
{ ".MAIN", SP_MAIN, OP_NONE },
{ ".MAKE", SP_ATTRIBUTE, OP_MAKE },
{ ".MAKEFLAGS", SP_MFLAGS, OP_NONE },
{ ".META", SP_META, OP_META },
{ ".MFLAGS", SP_MFLAGS, OP_NONE },
{ ".NOMETA", SP_NOMETA, OP_NOMETA },
{ ".NOMETA_CMP", SP_NOMETA_CMP, OP_NOMETA_CMP },
{ ".NOPATH", SP_NOPATH, OP_NOPATH },
{ ".NOTMAIN", SP_ATTRIBUTE, OP_NOTMAIN },
{ ".NOTPARALLEL", SP_NOTPARALLEL, OP_NONE },
{ ".NO_PARALLEL", SP_NOTPARALLEL, OP_NONE },
{ ".NULL", SP_NULL, OP_NONE },
{ ".OBJDIR", SP_OBJDIR, OP_NONE },
{ ".OPTIONAL", SP_ATTRIBUTE, OP_OPTIONAL },
{ ".ORDER", SP_ORDER, OP_NONE },
{ ".PARALLEL", SP_PARALLEL, OP_NONE },
{ ".PATH", SP_PATH, OP_NONE },
{ ".PHONY", SP_PHONY, OP_PHONY },
#if defined(POSIX)
{ ".POSIX", SP_POSIX, OP_NONE },
#endif
{ ".PRECIOUS", SP_PRECIOUS, OP_PRECIOUS },
{ ".RECURSIVE", SP_ATTRIBUTE, OP_MAKE },
{ ".SHELL", SP_SHELL, OP_NONE },
{ ".SILENT", SP_SILENT, OP_SILENT },
{ ".SINGLESHELL", SP_SINGLESHELL, OP_NONE },
{ ".STALE", SP_STALE, OP_NONE },
{ ".SUFFIXES", SP_SUFFIXES, OP_NONE },
{ ".USE", SP_ATTRIBUTE, OP_USE },
{ ".USEBEFORE", SP_ATTRIBUTE, OP_USEBEFORE },
{ ".WAIT", SP_WAIT, OP_NONE },
};



struct loadedfile {
char *buf;
size_t len;
bool used;
};


static struct loadedfile *
loadedfile_create(char *buf, size_t buflen)
{
struct loadedfile *lf;

lf = bmake_malloc(sizeof *lf);
lf->buf = buf;
lf->len = buflen;
lf->used = false;
return lf;
}

static void
loadedfile_destroy(struct loadedfile *lf)
{
free(lf->buf);
free(lf);
}





static char *
loadedfile_readMore(void *x, size_t *len)
{
struct loadedfile *lf = x;

if (lf->used)
return NULL;

lf->used = true;
*len = lf->len;
return lf->buf;
}




static bool
load_getsize(int fd, size_t *ret)
{
struct stat st;

if (fstat(fd, &st) < 0)
return false;

if (!S_ISREG(st.st_mode))
return false;










if (st.st_size < 0 || st.st_size > 0x3fffffff)
return false;

*ret = (size_t)st.st_size;
return true;
}










static struct loadedfile *
loadfile(const char *path, int fd)
{
ssize_t n;
Buffer buf;
size_t filesize;


if (path == NULL) {
assert(fd == -1);
fd = STDIN_FILENO;
}

if (load_getsize(fd, &filesize)) {






filesize++;
} else
filesize = 1024;
Buf_InitSize(&buf, filesize);

for (;;) {
assert(buf.len <= buf.cap);
if (buf.len == buf.cap) {
if (buf.cap > 0x1fffffff) {
errno = EFBIG;
Error("%s: file too large", path);
exit(2);
}
Buf_Expand(&buf);
}
assert(buf.len < buf.cap);
n = read(fd, buf.data + buf.len, buf.cap - buf.len);
if (n < 0) {
Error("%s: read error: %s", path, strerror(errno));
exit(2);
}
if (n == 0)
break;

buf.len += (size_t)n;
}
assert(buf.len <= buf.cap);

if (!Buf_EndsWith(&buf, '\n'))
Buf_AddByte(&buf, '\n');

if (path != NULL)
close(fd);

{
struct loadedfile *lf = loadedfile_create(buf.data, buf.len);
Buf_DoneData(&buf);
return lf;
}
}

static void
PrintStackTrace(void)
{
const IFile *entries;
size_t i, n;

if (!(DEBUG(PARSE)))
return;

entries = GetInclude(0);
n = includes.len;
if (n == 0)
return;
n--;















for (i = n; i-- > 0;) {
const IFile *entry = entries + i;
const char *fname = entry->fname;
bool printLineno;
char dirbuf[MAXPATHLEN + 1];

if (fname[0] != '/' && strcmp(fname, "(stdin)") != 0)
fname = realpath(fname, dirbuf);

printLineno = !entry->fromForLoop;
if (i + 1 < n && entries[i + 1].fromForLoop == printLineno)
printLineno = entry->fromForLoop;

if (printLineno)
debug_printf("\tin .include from %s:%d\n",
fname, entry->lineno);
if (entry->fromForLoop)
debug_printf("\tin .for loop from %s:%d\n",
fname, entry->first_lineno);
}
}


static bool
ParseIsEscaped(const char *line, const char *c)
{
bool active = false;
for (;;) {
if (line == c)
return active;
if (*--c != '\\')
return active;
active = !active;
}
}





static void
RememberLocation(GNode *gn)
{
IFile *curFile = CurFile();
gn->fname = curFile->fname;
gn->lineno = curFile->lineno;
}





static int
ParseFindKeyword(const char *str)
{
int start = 0;
int end = sizeof parseKeywords / sizeof parseKeywords[0] - 1;

do {
int curr = start + (end - start) / 2;
int diff = strcmp(str, parseKeywords[curr].name);

if (diff == 0)
return curr;
if (diff < 0)
end = curr - 1;
else
start = curr + 1;
} while (start <= end);

return -1;
}

static void
PrintLocation(FILE *f, const char *fname, size_t lineno)
{
char dirbuf[MAXPATHLEN + 1];
FStr dir, base;

if (*fname == '/' || strcmp(fname, "(stdin)") == 0) {
(void)fprintf(f, "\"%s\" line %u: ", fname, (unsigned)lineno);
return;
}




dir = Var_Value(SCOPE_GLOBAL, ".PARSEDIR");
if (dir.str == NULL)
dir.str = ".";
if (dir.str[0] != '/')
dir.str = realpath(dir.str, dirbuf);

base = Var_Value(SCOPE_GLOBAL, ".PARSEFILE");
if (base.str == NULL)
base.str = str_basename(fname);

(void)fprintf(f, "\"%s/%s\" line %u: ",
dir.str, base.str, (unsigned)lineno);

FStr_Done(&base);
FStr_Done(&dir);
}

static void
ParseVErrorInternal(FILE *f, const char *fname, size_t lineno,
ParseErrorLevel type, const char *fmt, va_list ap)
{
static bool fatal_warning_error_printed = false;

(void)fprintf(f, "%s: ", progname);

if (fname != NULL)
PrintLocation(f, fname, lineno);
if (type == PARSE_WARNING)
(void)fprintf(f, "warning: ");
(void)vfprintf(f, fmt, ap);
(void)fprintf(f, "\n");
(void)fflush(f);

if (type == PARSE_INFO)
goto print_stack_trace;
if (type == PARSE_WARNING && !opts.parseWarnFatal)
goto print_stack_trace;
parseErrors++;
if (type == PARSE_WARNING && !fatal_warning_error_printed) {
Error("parsing warnings being treated as errors");
fatal_warning_error_printed = true;
}

print_stack_trace:
PrintStackTrace();
}

static void
ParseErrorInternal(const char *fname, size_t lineno,
ParseErrorLevel type, const char *fmt, ...)
{
va_list ap;

(void)fflush(stdout);
va_start(ap, fmt);
ParseVErrorInternal(stderr, fname, lineno, type, fmt, ap);
va_end(ap);

if (opts.debug_file != stderr && opts.debug_file != stdout) {
va_start(ap, fmt);
ParseVErrorInternal(opts.debug_file, fname, lineno, type,
fmt, ap);
va_end(ap);
}
}









void
Parse_Error(ParseErrorLevel type, const char *fmt, ...)
{
va_list ap;
const char *fname;
size_t lineno;

if (includes.len == 0) {
fname = NULL;
lineno = 0;
} else {
IFile *curFile = CurFile();
fname = curFile->fname;
lineno = (size_t)curFile->lineno;
}

va_start(ap, fmt);
(void)fflush(stdout);
ParseVErrorInternal(stderr, fname, lineno, type, fmt, ap);
va_end(ap);

if (opts.debug_file != stderr && opts.debug_file != stdout) {
va_start(ap, fmt);
ParseVErrorInternal(opts.debug_file, fname, lineno, type,
fmt, ap);
va_end(ap);
}
}






static void
ParseMessage(ParseErrorLevel level, const char *levelName, const char *umsg)
{
char *xmsg;

if (umsg[0] == '\0') {
Parse_Error(PARSE_FATAL, "Missing argument for \".%s\"",
levelName);
return;
}

(void)Var_Subst(umsg, SCOPE_CMDLINE, VARE_WANTRES, &xmsg);


Parse_Error(level, "%s", xmsg);
free(xmsg);

if (level == PARSE_FATAL) {
PrintOnError(NULL, NULL);
exit(1);
}
}








static void
LinkSource(GNode *pgn, GNode *cgn, bool isSpecial)
{
if ((pgn->type & OP_DOUBLEDEP) && !Lst_IsEmpty(&pgn->cohorts))
pgn = pgn->cohorts.last->datum;

Lst_Append(&pgn->children, cgn);
pgn->unmade++;


if (!isSpecial)
Lst_Append(&cgn->parents, pgn);

if (DEBUG(PARSE)) {
debug_printf("#%s: added child %s - %s\n",
__func__, pgn->name, cgn->name);
Targ_PrintNode(pgn, 0);
Targ_PrintNode(cgn, 0);
}
}


static void
LinkToTargets(GNode *gn, bool isSpecial)
{
GNodeListNode *ln;

for (ln = targets->first; ln != NULL; ln = ln->next)
LinkSource(ln->datum, gn, isSpecial);
}

static bool
TryApplyDependencyOperator(GNode *gn, GNodeType op)
{




if ((op & OP_OPMASK) && (gn->type & OP_OPMASK) &&
((op & OP_OPMASK) != (gn->type & OP_OPMASK))) {
Parse_Error(PARSE_FATAL, "Inconsistent operator for %s",
gn->name);
return false;
}

if (op == OP_DOUBLEDEP && (gn->type & OP_OPMASK) == OP_DOUBLEDEP) {












GNode *cohort;





gn->type |= op & ~OP_OPMASK;

cohort = Targ_NewInternalNode(gn->name);
if (doing_depend)
RememberLocation(cohort);









cohort->type = op | OP_INVISIBLE;
Lst_Append(&gn->cohorts, cohort);
cohort->centurion = gn;
gn->unmade_cohorts++;
snprintf(cohort->cohort_num, sizeof cohort->cohort_num, "#%d",
(unsigned int)gn->unmade_cohorts % 1000000);
} else {




gn->type |= op;
}

return true;
}

static void
ApplyDependencyOperator(GNodeType op)
{
GNodeListNode *ln;

for (ln = targets->first; ln != NULL; ln = ln->next)
if (!TryApplyDependencyOperator(ln->datum, op))
break;
}









static void
ParseDependencySourceWait(bool isSpecial)
{
static int wait_number = 0;
char wait_src[16];
GNode *gn;

snprintf(wait_src, sizeof wait_src, ".WAIT_%u", ++wait_number);
gn = Targ_NewInternalNode(wait_src);
if (doing_depend)
RememberLocation(gn);
gn->type = OP_WAIT | OP_PHONY | OP_DEPENDS | OP_NOTMAIN;
LinkToTargets(gn, isSpecial);

}

static bool
ParseDependencySourceKeyword(const char *src, ParseSpecial specType)
{
int keywd;
GNodeType op;

if (*src != '.' || !ch_isupper(src[1]))
return false;

keywd = ParseFindKeyword(src);
if (keywd == -1)
return false;

op = parseKeywords[keywd].op;
if (op != OP_NONE) {
ApplyDependencyOperator(op);
return true;
}
if (parseKeywords[keywd].spec == SP_WAIT) {
ParseDependencySourceWait(specType != SP_NOT);
return true;
}
return false;
}

static void
ParseDependencySourceMain(const char *src)
{








Lst_Append(&opts.create, bmake_strdup(src));




Global_Append(".TARGETS", src);
}

static void
ParseDependencySourceOrder(const char *src)
{
GNode *gn;




gn = Targ_GetNode(src);
if (doing_depend)
RememberLocation(gn);
if (order_pred != NULL) {
Lst_Append(&order_pred->order_succ, gn);
Lst_Append(&gn->order_pred, order_pred);
if (DEBUG(PARSE)) {
debug_printf("#%s: added Order dependency %s - %s\n",
__func__, order_pred->name, gn->name);
Targ_PrintNode(order_pred, 0);
Targ_PrintNode(gn, 0);
}
}



order_pred = gn;
}

static void
ParseDependencySourceOther(const char *src, GNodeType tOp,
ParseSpecial specType)
{
GNode *gn;













gn = Targ_GetNode(src);
if (doing_depend)
RememberLocation(gn);
if (tOp != OP_NONE)
gn->type |= tOp;
else
LinkToTargets(gn, specType != SP_NOT);
}












static void
ParseDependencySource(GNodeType tOp, const char *src, ParseSpecial specType)
{
if (ParseDependencySourceKeyword(src, specType))
return;

if (specType == SP_MAIN)
ParseDependencySourceMain(src);
else if (specType == SP_ORDER)
ParseDependencySourceOrder(src);
else
ParseDependencySourceOther(src, tOp, specType);
}






static void
FindMainTarget(void)
{
GNodeListNode *ln;

if (mainNode != NULL)
return;

for (ln = targets->first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;
if (!(gn->type & OP_NOTARGET)) {
DEBUG1(MAKE, "Setting main node to \"%s\"\n", gn->name);
mainNode = gn;
Targ_SetMain(gn);
return;
}
}
}








static void
ParseErrorNoDependency(const char *lstart)
{
if ((strncmp(lstart, "<<<<<<", 6) == 0) ||
(strncmp(lstart, "======", 6) == 0) ||
(strncmp(lstart, ">>>>>>", 6) == 0))
Parse_Error(PARSE_FATAL,
"Makefile appears to contain unresolved CVS/RCS/??? merge conflicts");
else if (lstart[0] == '.') {
const char *dirstart = lstart + 1;
const char *dirend;
cpp_skip_whitespace(&dirstart);
dirend = dirstart;
while (ch_isalnum(*dirend) || *dirend == '-')
dirend++;
Parse_Error(PARSE_FATAL, "Unknown directive \"%.*s\"",
(int)(dirend - dirstart), dirstart);
} else
Parse_Error(PARSE_FATAL, "Invalid line type");
}

static void
ParseDependencyTargetWord(const char **pp, const char *lstart)
{
const char *cp = *pp;

while (*cp != '\0') {
if ((ch_isspace(*cp) || *cp == '!' || *cp == ':' ||
*cp == '(') &&
!ParseIsEscaped(lstart, cp))
break;

if (*cp == '$') {









const char *nested_p = cp;
FStr nested_val;

(void)Var_Parse(&nested_p, SCOPE_CMDLINE,
VARE_PARSE_ONLY, &nested_val);

FStr_Done(&nested_val);
cp += nested_p - cp;
} else
cp++;
}

*pp = cp;
}






static void
ParseDependencyTargetSpecial(ParseSpecial *inout_specType,
const char *targetName,
SearchPathList **inout_paths)
{
switch (*inout_specType) {
case SP_PATH:
if (*inout_paths == NULL)
*inout_paths = Lst_New();
Lst_Append(*inout_paths, &dirSearchPath);
break;
case SP_MAIN:




if (!Lst_IsEmpty(&opts.create))
*inout_specType = SP_NOT;
break;
case SP_BEGIN:
case SP_END:
case SP_STALE:
case SP_ERROR:
case SP_INTERRUPT: {
GNode *gn = Targ_GetNode(targetName);
if (doing_depend)
RememberLocation(gn);
gn->type |= OP_NOTMAIN | OP_SPECIAL;
Lst_Append(targets, gn);
break;
}
case SP_DEFAULT: {







GNode *gn = GNode_New(".DEFAULT");
gn->type |= OP_NOTMAIN | OP_TRANSFORM;
Lst_Append(targets, gn);
defaultNode = gn;
break;
}
case SP_DELETE_ON_ERROR:
deleteOnError = true;
break;
case SP_NOTPARALLEL:
opts.maxJobs = 1;
break;
case SP_SINGLESHELL:
opts.compatMake = true;
break;
case SP_ORDER:
order_pred = NULL;
break;
default:
break;
}
}





static bool
ParseDependencyTargetPath(const char *suffixName,
SearchPathList **inout_paths)
{
SearchPath *path;

path = Suff_GetPath(suffixName);
if (path == NULL) {
Parse_Error(PARSE_FATAL,
"Suffix '%s' not defined (yet)", suffixName);
return false;
}

if (*inout_paths == NULL)
*inout_paths = Lst_New();
Lst_Append(*inout_paths, path);

return true;
}




static bool
ParseDependencyTarget(const char *targetName,
ParseSpecial *inout_specType,
GNodeType *out_tOp, SearchPathList **inout_paths)
{
int keywd;

if (!(targetName[0] == '.' && ch_isupper(targetName[1])))
return true;





keywd = ParseFindKeyword(targetName);
if (keywd != -1) {
if (*inout_specType == SP_PATH &&
parseKeywords[keywd].spec != SP_PATH) {
Parse_Error(PARSE_FATAL, "Mismatched special targets");
return false;
}

*inout_specType = parseKeywords[keywd].spec;
*out_tOp = parseKeywords[keywd].op;

ParseDependencyTargetSpecial(inout_specType, targetName,
inout_paths);

} else if (strncmp(targetName, ".PATH", 5) == 0) {
*inout_specType = SP_PATH;
if (!ParseDependencyTargetPath(targetName + 5, inout_paths))
return false;
}
return true;
}

static void
ParseDependencyTargetMundane(char *targetName, StringList *curTargs)
{
if (Dir_HasWildcards(targetName)) {






SearchPath *emptyPath = SearchPath_New();

SearchPath_Expand(emptyPath, targetName, curTargs);

SearchPath_Free(emptyPath);
} else {




Lst_Append(curTargs, targetName);
}



while (!Lst_IsEmpty(curTargs)) {
char *targName = Lst_Dequeue(curTargs);
GNode *gn = Suff_IsTransform(targName)
? Suff_AddTransform(targName)
: Targ_GetNode(targName);
if (doing_depend)
RememberLocation(gn);

Lst_Append(targets, gn);
}
}

static void
ParseDependencyTargetExtraWarn(char **pp, const char *lstart)
{
bool warning = false;
char *cp = *pp;

while (*cp != '\0') {
if (!ParseIsEscaped(lstart, cp) && (*cp == '!' || *cp == ':'))
break;
if (ParseIsEscaped(lstart, cp) || (*cp != ' ' && *cp != '\t'))
warning = true;
cp++;
}
if (warning)
Parse_Error(PARSE_WARNING, "Extra target ignored");

*pp = cp;
}

static void
ParseDependencyCheckSpec(ParseSpecial specType)
{
switch (specType) {
default:
Parse_Error(PARSE_WARNING,
"Special and mundane targets don't mix. "
"Mundane ones ignored");
break;
case SP_DEFAULT:
case SP_STALE:
case SP_BEGIN:
case SP_END:
case SP_ERROR:
case SP_INTERRUPT:




case SP_NOT:

break;
}
}





static bool
ParseDependencyOp(char **pp, const char *lstart, GNodeType *out_op)
{
const char *cp = *pp;

if (*cp == '!') {
*out_op = OP_FORCE;
(*pp)++;
return true;
}

if (*cp == ':') {
if (cp[1] == ':') {
*out_op = OP_DOUBLEDEP;
(*pp) += 2;
} else {
*out_op = OP_DEPENDS;
(*pp)++;
}
return true;
}

{
const char *msg = lstart[0] == '.'
? "Unknown directive" : "Missing dependency operator";
Parse_Error(PARSE_FATAL, "%s", msg);
return false;
}
}

static void
ClearPaths(SearchPathList *paths)
{
if (paths != NULL) {
SearchPathListNode *ln;
for (ln = paths->first; ln != NULL; ln = ln->next)
SearchPath_Clear(ln->datum);
}

Dir_SetPATH();
}










static void
ParseDependencySourcesEmpty(ParseSpecial specType, SearchPathList *paths)
{
switch (specType) {
case SP_SUFFIXES:
Suff_ClearSuffixes();
break;
case SP_PRECIOUS:
allPrecious = true;
break;
case SP_IGNORE:
opts.ignoreErrors = true;
break;
case SP_SILENT:
opts.beSilent = true;
break;
case SP_PATH:
ClearPaths(paths);
break;
#if defined(POSIX)
case SP_POSIX:
Global_Set("%POSIX", "1003.2");
break;
#endif
default:
break;
}
}

static void
AddToPaths(const char *dir, SearchPathList *paths)
{
if (paths != NULL) {
SearchPathListNode *ln;
for (ln = paths->first; ln != NULL; ln = ln->next)
(void)SearchPath_Add(ln->datum, dir);
}
}




























static void
ParseDependencySourceSpecial(ParseSpecial specType, char *word,
SearchPathList *paths)
{
switch (specType) {
case SP_SUFFIXES:
Suff_AddSuffix(word, &mainNode);
break;
case SP_PATH:
AddToPaths(word, paths);
break;
case SP_INCLUDES:
Suff_AddInclude(word);
break;
case SP_LIBS:
Suff_AddLib(word);
break;
case SP_NULL:
Suff_SetNull(word);
break;
case SP_OBJDIR:
Main_SetObjdir(false, "%s", word);
break;
default:
break;
}
}

static bool
ParseDependencyTargets(char **inout_cp,
char **inout_line,
const char *lstart,
ParseSpecial *inout_specType,
GNodeType *inout_tOp,
SearchPathList **inout_paths,
StringList *curTargs)
{
char *cp;
char *tgt = *inout_line;
char savec;
const char *p;

for (;;) {






cp = tgt;
p = cp;
ParseDependencyTargetWord(&p, lstart);
cp += p - cp;





if (!ParseIsEscaped(lstart, cp) && *cp == '(') {













if (!Arch_ParseArchive(&tgt, targets, SCOPE_CMDLINE)) {
Parse_Error(PARSE_FATAL,
"Error in archive specification: \"%s\"",
tgt);
return false;
}

cp = tgt;
continue;
}

if (*cp == '\0') {
ParseErrorNoDependency(lstart);
return false;
}


savec = *cp;
*cp = '\0';

if (!ParseDependencyTarget(tgt, inout_specType, inout_tOp,
inout_paths))
return false;





if (*inout_specType == SP_NOT && *tgt != '\0')
ParseDependencyTargetMundane(tgt, curTargs);
else if (*inout_specType == SP_PATH && *tgt != '.' &&
*tgt != '\0')
Parse_Error(PARSE_WARNING, "Extra target (%s) ignored",
tgt);


*cp = savec;





if (*inout_specType != SP_NOT && *inout_specType != SP_PATH)
ParseDependencyTargetExtraWarn(&cp, lstart);
else
pp_skip_whitespace(&cp);

tgt = cp;
if (*tgt == '\0')
break;
if ((*tgt == '!' || *tgt == ':') &&
!ParseIsEscaped(lstart, tgt))
break;
}

*inout_cp = cp;
*inout_line = tgt;
return true;
}

static void
ParseDependencySourcesSpecial(char *start, char *end,
ParseSpecial specType, SearchPathList *paths)
{
char savec;

while (*start != '\0') {
while (*end != '\0' && !ch_isspace(*end))
end++;
savec = *end;
*end = '\0';
ParseDependencySourceSpecial(specType, start, paths);
*end = savec;
if (savec != '\0')
end++;
pp_skip_whitespace(&end);
start = end;
}
}

static bool
ParseDependencySourcesMundane(char *start, char *end,
ParseSpecial specType, GNodeType tOp)
{
while (*start != '\0') {





for (; *end != '\0' && !ch_isspace(*end); end++) {
if (*end == '(' && end > start && end[-1] != '$') {







break;
}
}

if (*end == '(') {
GNodeList sources = LST_INIT;
if (!Arch_ParseArchive(&start, &sources,
SCOPE_CMDLINE)) {
Parse_Error(PARSE_FATAL,
"Error in source archive spec \"%s\"",
start);
return false;
}

while (!Lst_IsEmpty(&sources)) {
GNode *gn = Lst_Dequeue(&sources);
ParseDependencySource(tOp, gn->name, specType);
}
Lst_Done(&sources);
end = start;
} else {
if (*end != '\0') {
*end = '\0';
end++;
}

ParseDependencySource(tOp, start, specType);
}
pp_skip_whitespace(&end);
start = end;
}
return true;
}






static void
ParseDependencySources(char *line, char *cp, GNodeType tOp,
ParseSpecial specType, SearchPathList **inout_paths)
{
if (line[0] == '\0') {
ParseDependencySourcesEmpty(specType, *inout_paths);
} else if (specType == SP_MFLAGS) {
Main_ParseArgLine(line);




*line = '\0';
} else if (specType == SP_SHELL) {
if (!Job_ParseShell(line)) {
Parse_Error(PARSE_FATAL,
"improper shell specification");
return;
}
*line = '\0';
} else if (specType == SP_NOTPARALLEL || specType == SP_SINGLESHELL ||
specType == SP_DELETE_ON_ERROR) {
*line = '\0';
}


if (specType == SP_SUFFIXES || specType == SP_PATH ||
specType == SP_INCLUDES || specType == SP_LIBS ||
specType == SP_NULL || specType == SP_OBJDIR) {
ParseDependencySourcesSpecial(line, cp, specType,
*inout_paths);
if (*inout_paths != NULL) {
Lst_Free(*inout_paths);
*inout_paths = NULL;
}
if (specType == SP_PATH)
Dir_SetPATH();
} else {
assert(*inout_paths == NULL);
if (!ParseDependencySourcesMundane(line, cp, specType, tOp))
return;
}

FindMainTarget();
}




























static void
ParseDependency(char *line)
{
char *cp;
GNodeType op;
SearchPathList *paths;

GNodeType tOp;

StringList curTargs = LST_INIT;
char *lstart = line;






ParseSpecial specType = SP_NOT;

DEBUG1(PARSE, "ParseDependency(%s)\n", line);
tOp = OP_NONE;

paths = NULL;





if (!ParseDependencyTargets(&cp, &line, lstart, &specType, &tOp,
&paths, &curTargs))
goto out;





Lst_Done(&curTargs);
Lst_Init(&curTargs);

if (!Lst_IsEmpty(targets))
ParseDependencyCheckSpec(specType);




if (!ParseDependencyOp(&cp, lstart, &op))
goto out;






ApplyDependencyOperator(op);







pp_skip_whitespace(&cp);
line = cp;

ParseDependencySources(line, cp, tOp, specType, &paths);

out:
if (paths != NULL)
Lst_Free(paths);
Lst_Done(&curTargs);
}

typedef struct VarAssignParsed {
const char *nameStart;
const char *nameEnd;
const char *eq;
} VarAssignParsed;





static void
AdjustVarassignOp(const VarAssignParsed *pvar, const char *value,
VarAssign *out_var)
{
const char *op = pvar->eq;
const char *const name = pvar->nameStart;
VarAssignOp type;

if (op > name && op[-1] == '+') {
type = VAR_APPEND;
op--;

} else if (op > name && op[-1] == '?') {
op--;
type = VAR_DEFAULT;

} else if (op > name && op[-1] == ':') {
op--;
type = VAR_SUBST;

} else if (op > name && op[-1] == '!') {
op--;
type = VAR_SHELL;

} else {
type = VAR_NORMAL;
#if defined(SUNSHCMD)
while (op > name && ch_isspace(op[-1]))
op--;

if (op >= name + 3 && op[-3] == ':' && op[-2] == 's' &&
op[-1] == 'h') {
type = VAR_SHELL;
op -= 3;
}
#endif
}

{
const char *nameEnd = pvar->nameEnd < op ? pvar->nameEnd : op;
out_var->varname = bmake_strsedup(pvar->nameStart, nameEnd);
out_var->op = type;
out_var->value = value;
}
}














bool
Parse_IsVar(const char *p, VarAssign *out_var)
{
VarAssignParsed pvar;
const char *firstSpace = NULL;
int level = 0;

cpp_skip_hspace(&p);







pvar.nameStart = p;
#if defined(CLEANUP)
pvar.nameEnd = NULL;
pvar.eq = NULL;
#endif





while (*p != '\0') {
char ch = *p++;
if (ch == '(' || ch == '{') {
level++;
continue;
}
if (ch == ')' || ch == '}') {
level--;
continue;
}

if (level != 0)
continue;

if (ch == ' ' || ch == '\t')
if (firstSpace == NULL)
firstSpace = p - 1;
while (ch == ' ' || ch == '\t')
ch = *p++;

#if defined(SUNSHCMD)
if (ch == ':' && p[0] == 's' && p[1] == 'h') {
p += 2;
continue;
}
#endif
if (ch == '=') {
pvar.eq = p - 1;
pvar.nameEnd = firstSpace != NULL ? firstSpace : p - 1;
cpp_skip_whitespace(&p);
AdjustVarassignOp(&pvar, p, out_var);
return true;
}
if (*p == '=' &&
(ch == '+' || ch == ':' || ch == '?' || ch == '!')) {
pvar.eq = p;
pvar.nameEnd = firstSpace != NULL ? firstSpace : p;
p++;
cpp_skip_whitespace(&p);
AdjustVarassignOp(&pvar, p, out_var);
return true;
}
if (firstSpace != NULL)
return false;
}

return false;
}




static void
VarCheckSyntax(VarAssignOp type, const char *uvalue, GNode *scope)
{
if (opts.strict) {
if (type != VAR_SUBST && strchr(uvalue, '$') != NULL) {
char *expandedValue;

(void)Var_Subst(uvalue, scope, VARE_PARSE_ONLY,
&expandedValue);

free(expandedValue);
}
}
}

static void
VarAssign_EvalSubst(GNode *scope, const char *name, const char *uvalue,
FStr *out_avalue)
{
char *evalue;








if (!Var_ExistsExpand(scope, name))
Var_SetExpand(scope, name, "");

(void)Var_Subst(uvalue, scope, VARE_KEEP_DOLLAR_UNDEF, &evalue);


Var_SetExpand(scope, name, evalue);

*out_avalue = FStr_InitOwn(evalue);
}

static void
VarAssign_EvalShell(const char *name, const char *uvalue, GNode *scope,
FStr *out_avalue)
{
FStr cmd;
const char *errfmt;
char *cmdOut;

cmd = FStr_InitRefer(uvalue);
if (strchr(cmd.str, '$') != NULL) {
char *expanded;
(void)Var_Subst(cmd.str, SCOPE_CMDLINE, VARE_UNDEFERR,
&expanded);

cmd = FStr_InitOwn(expanded);
}

cmdOut = Cmd_Exec(cmd.str, &errfmt);
Var_SetExpand(scope, name, cmdOut);
*out_avalue = FStr_InitOwn(cmdOut);

if (errfmt != NULL)
Parse_Error(PARSE_WARNING, errfmt, cmd.str);

FStr_Done(&cmd);
}












static bool
VarAssign_Eval(const char *name, VarAssignOp op, const char *uvalue,
GNode *scope, FStr *out_true_avalue)
{
FStr avalue = FStr_InitRefer(uvalue);

if (op == VAR_APPEND)
Var_AppendExpand(scope, name, uvalue);
else if (op == VAR_SUBST)
VarAssign_EvalSubst(scope, name, uvalue, &avalue);
else if (op == VAR_SHELL)
VarAssign_EvalShell(name, uvalue, scope, &avalue);
else {
if (op == VAR_DEFAULT && Var_ExistsExpand(scope, name))
return false;


Var_SetExpand(scope, name, uvalue);
}

*out_true_avalue = avalue;
return true;
}

static void
VarAssignSpecial(const char *name, const char *avalue)
{
if (strcmp(name, MAKEOVERRIDES) == 0)
Main_ExportMAKEFLAGS(false);
else if (strcmp(name, ".CURDIR") == 0) {





Dir_InitCur(avalue);
Dir_SetPATH();
} else if (strcmp(name, MAKE_JOB_PREFIX) == 0)
Job_SetPrefix();
else if (strcmp(name, MAKE_EXPORTED) == 0)
Var_ExportVars(avalue);
}


void
Parse_Var(VarAssign *var, GNode *scope)
{
FStr avalue;

VarCheckSyntax(var->op, var->value, scope);
if (VarAssign_Eval(var->varname, var->op, var->value, scope, &avalue)) {
VarAssignSpecial(var->varname, avalue.str);
FStr_Done(&avalue);
}

free(var->varname);
}






static bool
MaybeSubMake(const char *cmd)
{
const char *start;

for (start = cmd; *start != '\0'; start++) {
const char *p = start;
char endc;


if (p[0] == 'm' && p[1] == 'a' && p[2] == 'k' && p[3] == 'e')
if (start == cmd || !ch_isalnum(p[-1]))
if (!ch_isalnum(p[4]))
return true;

if (*p != '$')
continue;
p++;

if (*p == '{')
endc = '}';
else if (*p == '(')
endc = ')';
else
continue;
p++;

if (*p == '.')
p++;

if (p[0] == 'M' && p[1] == 'A' && p[2] == 'K' && p[3] == 'E')
if (p[4] == endc)
return true;
}
return false;
}







static void
ParseAddCmd(GNode *gn, char *cmd)
{

if ((gn->type & OP_DOUBLEDEP) && gn->cohorts.last != NULL)
gn = gn->cohorts.last->datum;


if (!(gn->type & OP_HAS_COMMANDS)) {
Lst_Append(&gn->commands, cmd);
if (MaybeSubMake(cmd))
gn->type |= OP_SUBMAKE;
RememberLocation(gn);
} else {
#if 0

Lst_Append(&gn->commands, cmd);
Parse_Error(PARSE_WARNING,
"overriding commands for target \"%s\"; "
"previous commands defined at %s: %d ignored",
gn->name, gn->fname, gn->lineno);
#else
Parse_Error(PARSE_WARNING,
"duplicate script for target \"%s\" ignored",
gn->name);
ParseErrorInternal(gn->fname, (size_t)gn->lineno, PARSE_WARNING,
"using previous script for \"%s\" defined here",
gn->name);
#endif
}
}





void
Parse_AddIncludeDir(const char *dir)
{
(void)SearchPath_Add(parseIncPath, dir);
}











static void
IncludeFile(const char *file, bool isSystem, bool depinc, bool silent)
{
struct loadedfile *lf;
char *fullname;
char *newName;
char *slash, *incdir;
int fd;
int i;

fullname = file[0] == '/' ? bmake_strdup(file) : NULL;

if (fullname == NULL && !isSystem) {








incdir = bmake_strdup(CurFile()->fname);
slash = strrchr(incdir, '/');
if (slash != NULL) {
*slash = '\0';




for (i = 0; strncmp(file + i, "../", 3) == 0; i += 3) {
slash = strrchr(incdir + 1, '/');
if (slash == NULL || strcmp(slash, "/..") == 0)
break;
*slash = '\0';
}
newName = str_concat3(incdir, "/", file + i);
fullname = Dir_FindFile(newName, parseIncPath);
if (fullname == NULL)
fullname = Dir_FindFile(newName,
&dirSearchPath);
free(newName);
}
free(incdir);

if (fullname == NULL) {









const char *suff;
SearchPath *suffPath = NULL;

if ((suff = strrchr(file, '.')) != NULL) {
suffPath = Suff_GetPath(suff);
if (suffPath != NULL)
fullname = Dir_FindFile(file, suffPath);
}
if (fullname == NULL) {
fullname = Dir_FindFile(file, parseIncPath);
if (fullname == NULL)
fullname = Dir_FindFile(file,
&dirSearchPath);
}
}
}


if (fullname == NULL) {



SearchPath *path = Lst_IsEmpty(&sysIncPath->dirs)
? defSysIncPath : sysIncPath;
fullname = Dir_FindFile(file, path);
}

if (fullname == NULL) {
if (!silent)
Parse_Error(PARSE_FATAL, "Could not find %s", file);
return;
}


fd = open(fullname, O_RDONLY);
if (fd == -1) {
if (!silent)
Parse_Error(PARSE_FATAL, "Cannot open %s", fullname);
free(fullname);
return;
}


lf = loadfile(fullname, fd);


Parse_PushInput(fullname, 0, -1, loadedfile_readMore, lf);
CurFile()->lf = lf;
if (depinc)
doing_depend = depinc;

}







static void
ParseInclude(char *directive)
{
char endc;
char *p;
bool silent = directive[0] != 'i';
FStr file;

p = directive + (silent ? 8 : 7);
pp_skip_hspace(&p);

if (*p != '"' && *p != '<') {
Parse_Error(PARSE_FATAL,
".include filename must be delimited by '\"' or '<'");
return;
}

if (*p++ == '<')
endc = '>';
else
endc = '"';
file = FStr_InitRefer(p);


while (*p != '\0' && *p != endc)
p++;

if (*p != endc) {
Parse_Error(PARSE_FATAL,
"Unclosed .include filename. '%c' expected", endc);
return;
}

*p = '\0';

if (strchr(file.str, '$') != NULL) {
char *xfile;
Var_Subst(file.str, SCOPE_CMDLINE, VARE_WANTRES, &xfile);

file = FStr_InitOwn(xfile);
}

IncludeFile(file.str, endc == '>', directive[0] == 'd', silent);
FStr_Done(&file);
}





static void
SetFilenameVars(const char *filename, const char *dirvar, const char *filevar)
{
const char *slash, *basename;
FStr dirname;

slash = strrchr(filename, '/');
if (slash == NULL) {
dirname = FStr_InitRefer(curdir);
basename = filename;
} else {
dirname = FStr_InitOwn(bmake_strsedup(filename, slash));
basename = slash + 1;
}

Global_Set(dirvar, dirname.str);
Global_Set(filevar, basename);

DEBUG5(PARSE, "%s: ${%s} = `%s' ${%s} = `%s'\n",
__func__, dirvar, dirname.str, filevar, basename);
FStr_Done(&dirname);
}







static const char *
GetActuallyIncludingFile(void)
{
size_t i;
const IFile *incs = GetInclude(0);

for (i = includes.len; i >= 2; i--)
if (!incs[i - 1].fromForLoop)
return incs[i - 2].fname;
return NULL;
}


static void
ParseSetParseFile(const char *filename)
{
const char *including;

SetFilenameVars(filename, ".PARSEDIR", ".PARSEFILE");

including = GetActuallyIncludingFile();
if (including != NULL) {
SetFilenameVars(including,
".INCLUDEDFROMDIR", ".INCLUDEDFROMFILE");
} else {
Global_Delete(".INCLUDEDFROMDIR");
Global_Delete(".INCLUDEDFROMFILE");
}
}

static bool
StrContainsWord(const char *str, const char *word)
{
size_t strLen = strlen(str);
size_t wordLen = strlen(word);
const char *p, *end;

if (strLen < wordLen)
return false;

end = str + strLen - wordLen;
for (p = str; p != NULL; p = strchr(p, ' ')) {
if (*p == ' ')
p++;
if (p > end)
return false;

if (memcmp(p, word, wordLen) == 0 &&
(p[wordLen] == '\0' || p[wordLen] == ' '))
return true;
}
return false;
}







static bool
VarContainsWord(const char *varname, const char *word)
{
FStr val = Var_Value(SCOPE_GLOBAL, varname);
bool found = val.str != NULL && StrContainsWord(val.str, word);
FStr_Done(&val);
return found;
}








static void
ParseTrackInput(const char *name)
{
if (!VarContainsWord(MAKE_MAKEFILES, name))
Global_Append(MAKE_MAKEFILES, name);
}







void
Parse_PushInput(const char *name, int lineno, int fd,
ReadMoreProc readMore, void *readMoreArg)
{
IFile *curFile;
char *buf;
size_t len;
bool fromForLoop = name == NULL;

if (fromForLoop)
name = CurFile()->fname;
else
ParseTrackInput(name);

DEBUG3(PARSE, "Parse_PushInput: %s %s, line %d\n",
readMore == loadedfile_readMore ? "file" : ".for loop in",
name, lineno);

if (fd == -1 && readMore == NULL)

return;

curFile = Vector_Push(&includes);
curFile->fname = bmake_strdup(name);
curFile->fromForLoop = fromForLoop;
curFile->lineno = lineno;
curFile->first_lineno = lineno;
curFile->readMore = readMore;
curFile->readMoreArg = readMoreArg;
curFile->lf = NULL;
curFile->depending = doing_depend;

assert(readMore != NULL);


buf = curFile->readMore(curFile->readMoreArg, &len);
if (buf == NULL) {

if (curFile->fname != NULL)
free(curFile->fname);
free(curFile);
return;
}
curFile->buf_freeIt = buf;
curFile->buf_ptr = buf;
curFile->buf_end = buf + len;

curFile->cond_depth = Cond_save_depth();
ParseSetParseFile(name);
}


static bool
IsInclude(const char *dir, bool sysv)
{
if (dir[0] == 's' || dir[0] == '-' || (dir[0] == 'd' && !sysv))
dir++;

if (strncmp(dir, "include", 7) != 0)
return false;


return !sysv || ch_isspace(dir[7]);
}


#if defined(SYSVINCLUDE)

static bool
IsSysVInclude(const char *line)
{
const char *p;

if (!IsInclude(line, true))
return false;


for (p = line; (p = strchr(p, ':')) != NULL;) {


if (*++p == '\0')
return false;


if (*p == ':' || ch_isspace(*p))
return false;
}
return true;
}


static void
ParseTraditionalInclude(char *line)
{
char *cp;
bool done = false;
bool silent = line[0] != 'i';
char *file = line + (silent ? 8 : 7);
char *all_files;

DEBUG2(PARSE, "%s: %s\n", __func__, file);

pp_skip_whitespace(&file);





(void)Var_Subst(file, SCOPE_CMDLINE, VARE_WANTRES, &all_files);


if (*file == '\0') {
Parse_Error(PARSE_FATAL, "Filename missing from \"include\"");
goto out;
}

for (file = all_files; !done; file = cp + 1) {

for (cp = file; *cp != '\0' && !ch_isspace(*cp); cp++)
continue;

if (*cp != '\0')
*cp = '\0';
else
done = true;

IncludeFile(file, false, false, silent);
}
out:
free(all_files);
}
#endif

#if defined(GMAKEEXPORT)

static void
ParseGmakeExport(char *line)
{
char *variable = line + 6;
char *value;

DEBUG2(PARSE, "%s: %s\n", __func__, variable);

pp_skip_whitespace(&variable);

for (value = variable; *value != '\0' && *value != '='; value++)
continue;

if (*value != '=') {
Parse_Error(PARSE_FATAL,
"Variable/Value missing from \"export\"");
return;
}
*value++ = '\0';




(void)Var_Subst(value, SCOPE_CMDLINE, VARE_WANTRES, &value);


setenv(variable, value, 1);
free(value);
}
#endif










static bool
ParseEOF(void)
{
char *ptr;
size_t len;
IFile *curFile = CurFile();

assert(curFile->readMore != NULL);

doing_depend = curFile->depending;

ptr = curFile->readMore(curFile->readMoreArg, &len);
curFile->buf_ptr = ptr;
curFile->buf_freeIt = ptr;
curFile->buf_end = ptr == NULL ? NULL : ptr + len;
curFile->lineno = curFile->first_lineno;
if (ptr != NULL)
return true;


Cond_restore_depth(curFile->cond_depth);

if (curFile->lf != NULL) {
loadedfile_destroy(curFile->lf);
curFile->lf = NULL;
}



free(curFile->buf_freeIt);
Vector_Pop(&includes);

if (includes.len == 0) {

Global_Delete(".PARSEDIR");
Global_Delete(".PARSEFILE");
Global_Delete(".INCLUDEDFROMDIR");
Global_Delete(".INCLUDEDFROMFILE");
return false;
}

curFile = CurFile();
DEBUG2(PARSE, "ParseEOF: returning to file %s, line %d\n",
curFile->fname, curFile->lineno);

ParseSetParseFile(curFile->fname);
return true;
}

typedef enum ParseRawLineResult {
PRLR_LINE,
PRLR_EOF,
PRLR_ERROR
} ParseRawLineResult;






static ParseRawLineResult
ParseRawLine(IFile *curFile, char **out_line, char **out_line_end,
char **out_firstBackslash, char **out_firstComment)
{
char *line = curFile->buf_ptr;
char *buf_end = curFile->buf_end;
char *p = line;
char *line_end = line;
char *firstBackslash = NULL;
char *firstComment = NULL;
ParseRawLineResult res = PRLR_LINE;

curFile->lineno++;

for (;;) {
char ch;

if (p == buf_end) {
res = PRLR_EOF;
break;
}

ch = *p;
if (ch == '\0' ||
(ch == '\\' && p + 1 < buf_end && p[1] == '\0')) {
Parse_Error(PARSE_FATAL, "Zero byte read from file");
return PRLR_ERROR;
}


if (ch == '\\') {
if (firstBackslash == NULL)
firstBackslash = p;
if (p[1] == '\n') {
curFile->lineno++;
if (p + 2 == buf_end) {
line_end = p;
*line_end = '\n';
p += 2;
continue;
}
}
p += 2;
line_end = p;
assert(p <= buf_end);
continue;
}





if (ch == '#' && firstComment == NULL &&
!(p > line && p[-1] == '['))
firstComment = line_end;

p++;
if (ch == '\n')
break;


if (!ch_isspace(ch))
line_end = p;
}

*out_line = line;
curFile->buf_ptr = p;
*out_line_end = line_end;
*out_firstBackslash = firstBackslash;
*out_firstComment = firstComment;
return res;
}





static void
UnescapeBackslash(char *line, char *start)
{
char *src = start;
char *dst = start;
char *spaceStart = line;

for (;;) {
char ch = *src++;
if (ch != '\\') {
if (ch == '\0')
break;
*dst++ = ch;
continue;
}

ch = *src++;
if (ch == '\0') {

dst--;
break;
}


if (ch == '#' && line[0] != '\t') {
*dst++ = ch;
continue;
}

if (ch != '\n') {

*dst++ = '\\';




spaceStart = dst + 1;
*dst++ = ch;
continue;
}





pp_skip_hspace(&src);
*dst++ = ' ';
}


while (dst > spaceStart && ch_isspace(dst[-1]))
dst--;
*dst = '\0';
}

typedef enum GetLineMode {





GLM_NONEMPTY,








GLM_FOR_BODY,









GLM_DOT
} GetLineMode;


static char *
ParseGetLine(GetLineMode mode)
{
IFile *curFile = CurFile();
char *line;
char *line_end;
char *firstBackslash;
char *firstComment;

for (;;) {
ParseRawLineResult res = ParseRawLine(curFile,
&line, &line_end, &firstBackslash, &firstComment);
if (res == PRLR_ERROR)
return NULL;

if (line_end == line || firstComment == line) {
if (res == PRLR_EOF)
return NULL;
if (mode != GLM_FOR_BODY)
continue;
}


assert(ch_isspace(*line_end));
*line_end = '\0';

if (mode == GLM_FOR_BODY)
return line;

if (mode == GLM_DOT && line[0] != '.')
continue;
break;
}


if (firstComment != NULL && line[0] != '\t')
*firstComment = '\0';


if (firstBackslash == NULL)
return line;


UnescapeBackslash(line, firstBackslash);

return line;
}

static bool
ParseSkippedBranches(void)
{
char *line;

while ((line = ParseGetLine(GLM_DOT)) != NULL) {
if (Cond_EvalLine(line) == COND_PARSE)
break;













}

return line != NULL;
}

static bool
ParseForLoop(const char *line)
{
int rval;
int firstLineno;

rval = For_Eval(line);
if (rval == 0)
return false;
if (rval < 0)
return true;

firstLineno = CurFile()->lineno;


do {
line = ParseGetLine(GLM_FOR_BODY);
if (line == NULL) {
Parse_Error(PARSE_FATAL,
"Unexpected end of file in .for loop");
break;
}
} while (For_Accum(line));

For_Run(firstLineno);

return true;
}












static char *
ParseReadLine(void)
{
char *line;

for (;;) {
line = ParseGetLine(GLM_NONEMPTY);
if (line == NULL)
return NULL;

if (line[0] != '.')
return line;





switch (Cond_EvalLine(line)) {
case COND_SKIP:
if (!ParseSkippedBranches())
return NULL;
continue;
case COND_PARSE:
continue;
case COND_INVALID:
if (ParseForLoop(line))
continue;
break;
}
return line;
}
}

static void
FinishDependencyGroup(void)
{
GNodeListNode *ln;

if (targets == NULL)
return;

for (ln = targets->first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;

Suff_EndTransform(gn);






if (!Lst_IsEmpty(&gn->commands))
gn->type |= OP_HAS_COMMANDS;
}

Lst_Free(targets);
targets = NULL;
}


static void
ParseLine_ShellCommand(const char *p)
{
cpp_skip_whitespace(&p);
if (*p == '\0')
return;

if (targets == NULL) {
Parse_Error(PARSE_FATAL,
"Unassociated shell command \"%s\"", p);
return;
}

{
char *cmd = bmake_strdup(p);
GNodeListNode *ln;

for (ln = targets->first; ln != NULL; ln = ln->next) {
GNode *gn = ln->datum;
ParseAddCmd(gn, cmd);
}
#if defined(CLEANUP)
Lst_Append(&targCmds, cmd);
#endif
}
}





static bool
ParseDirective(char *line)
{
char *cp = line + 1;
const char *arg;
Substring dir;

pp_skip_whitespace(&cp);
if (IsInclude(cp, false)) {
ParseInclude(cp);
return true;
}

dir.start = cp;
while (ch_isalpha(*cp) || *cp == '-')
cp++;
dir.end = cp;

if (*cp != '\0' && !ch_isspace(*cp))
return false;

pp_skip_whitespace(&cp);
arg = cp;

if (Substring_Equals(dir, "undef")) {
Var_Undef(arg);
return true;
} else if (Substring_Equals(dir, "export")) {
Var_Export(VEM_PLAIN, arg);
return true;
} else if (Substring_Equals(dir, "export-env")) {
Var_Export(VEM_ENV, arg);
return true;
} else if (Substring_Equals(dir, "export-literal")) {
Var_Export(VEM_LITERAL, arg);
return true;
} else if (Substring_Equals(dir, "unexport")) {
Var_UnExport(false, arg);
return true;
} else if (Substring_Equals(dir, "unexport-env")) {
Var_UnExport(true, arg);
return true;
} else if (Substring_Equals(dir, "info")) {
ParseMessage(PARSE_INFO, "info", arg);
return true;
} else if (Substring_Equals(dir, "warning")) {
ParseMessage(PARSE_WARNING, "warning", arg);
return true;
} else if (Substring_Equals(dir, "error")) {
ParseMessage(PARSE_FATAL, "error", arg);
return true;
}
return false;
}

static bool
ParseVarassign(const char *line)
{
VarAssign var;

if (!Parse_IsVar(line, &var))
return false;

FinishDependencyGroup();
Parse_Var(&var, SCOPE_GLOBAL);
return true;
}

static char *
FindSemicolon(char *p)
{
int level = 0;

for (; *p != '\0'; p++) {
if (*p == '\\' && p[1] != '\0') {
p++;
continue;
}

if (*p == '$' && (p[1] == '(' || p[1] == '{'))
level++;
else if (level > 0 && (*p == ')' || *p == '}'))
level--;
else if (level == 0 && *p == ';')
break;
}
return p;
}





static void
ParseDependencyLine(char *line)
{
VarEvalMode emode;
char *expanded_line;
const char *shellcmd = NULL;






{
char *semicolon = FindSemicolon(line);
if (*semicolon != '\0') {

*semicolon = '\0';
shellcmd = semicolon + 1;
}
}
































emode = opts.strict ? VARE_WANTRES : VARE_UNDEFERR;
(void)Var_Subst(line, SCOPE_CMDLINE, emode, &expanded_line);



if (targets != NULL)
Lst_Free(targets);
targets = Lst_New();

ParseDependency(expanded_line);
free(expanded_line);

if (shellcmd != NULL)
ParseLine_ShellCommand(shellcmd);
}

static void
ParseLine(char *line)
{







if (line[0] == '.' && ParseDirective(line))
return;

if (line[0] == '\t') {
ParseLine_ShellCommand(line + 1);
return;
}

#if defined(SYSVINCLUDE)
if (IsSysVInclude(line)) {



ParseTraditionalInclude(line);
return;
}
#endif

#if defined(GMAKEEXPORT)
if (strncmp(line, "export", 6) == 0 && ch_isspace(line[6]) &&
strchr(line, ':') == NULL) {



ParseGmakeExport(line);
return;
}
#endif

if (ParseVarassign(line))
return;

FinishDependencyGroup();

ParseDependencyLine(line);
}









void
Parse_File(const char *name, int fd)
{
char *line;
struct loadedfile *lf;

lf = loadfile(name, fd);

assert(targets == NULL);

if (name == NULL)
name = "(stdin)";

Parse_PushInput(name, 0, -1, loadedfile_readMore, lf);
CurFile()->lf = lf;

do {
while ((line = ParseReadLine()) != NULL) {
DEBUG2(PARSE, "ParseReadLine (%d): '%s'\n",
CurFile()->lineno, line);
ParseLine(line);
}

} while (ParseEOF());

FinishDependencyGroup();

if (parseErrors != 0) {
(void)fflush(stdout);
(void)fprintf(stderr,
"%s: Fatal errors encountered -- cannot continue",
progname);
PrintOnError(NULL, NULL);
exit(1);
}
}


void
Parse_Init(void)
{
mainNode = NULL;
parseIncPath = SearchPath_New();
sysIncPath = SearchPath_New();
defSysIncPath = SearchPath_New();
Vector_Init(&includes, sizeof(IFile));
}


void
Parse_End(void)
{
#if defined(CLEANUP)
Lst_DoneCall(&targCmds, free);
assert(targets == NULL);
SearchPath_Free(defSysIncPath);
SearchPath_Free(sysIncPath);
SearchPath_Free(parseIncPath);
assert(includes.len == 0);
Vector_Done(&includes);
#endif
}






void
Parse_MainName(GNodeList *mainList)
{
if (mainNode == NULL)
Punt("no target to make.");

Lst_Append(mainList, mainNode);
if (mainNode->type & OP_DOUBLEDEP)
Lst_AppendAll(mainList, &mainNode->cohorts);

Global_Append(".TARGETS", mainNode->name);
}

int
Parse_NumErrors(void)
{
return parseErrors;
}
