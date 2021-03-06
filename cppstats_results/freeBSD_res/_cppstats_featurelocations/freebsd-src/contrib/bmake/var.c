





























































































































#include <sys/stat.h>
#include <sys/types.h>
#if !defined(NO_REGEX)
#include <regex.h>
#endif

#include "make.h"

#include <errno.h>
#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#include <time.h>

#include "dir.h"
#include "job.h"
#include "metachar.h"


MAKE_RCSID("$NetBSD: var.c,v 1.973 2021/12/12 20:45:48 sjg Exp $");




















typedef struct Var {





FStr name;


Buffer val;


bool fromCmd: 1;






bool fromEnv: 1;








bool readOnly: 1;





bool inUse: 1;





bool exported: 1;







bool reexport: 1;
} Var;








typedef enum VarExportedMode {
VAR_EXPORTED_NONE,
VAR_EXPORTED_SOME,
VAR_EXPORTED_ALL
} VarExportedMode;

typedef enum UnexportWhat {

UNEXPORT_NAMED,




UNEXPORT_ALL,




UNEXPORT_ENV
} UnexportWhat;


typedef struct PatternFlags {
bool subGlobal: 1;
bool subOnce: 1;
bool anchorStart: 1;
bool anchorEnd: 1;
} PatternFlags;


typedef struct SepBuf {
Buffer buf;
bool needSep;

char sep;
} SepBuf;






char **savedEnv = NULL;






char var_Error[] = "";









static char varUndefined[] = "";









#define MAKE_SAVE_DOLLARS ".MAKE.SAVE_DOLLARS"
static bool save_dollars = false;






















GNode *SCOPE_CMDLINE;
GNode *SCOPE_GLOBAL;
GNode *SCOPE_INTERNAL;

static VarExportedMode var_exportedVars = VAR_EXPORTED_NONE;

static const char VarEvalMode_Name[][32] = {
"parse-only",
"eval",
"eval-defined",
"eval-keep-dollar",
"eval-keep-undefined",
"eval-keep-dollar-and-undefined",
};


static Var *
VarNew(FStr name, const char *value, bool fromEnv, bool readOnly)
{
size_t value_len = strlen(value);
Var *var = bmake_malloc(sizeof *var);
var->name = name;
Buf_InitSize(&var->val, value_len + 1);
Buf_AddBytes(&var->val, value, value_len);
var->fromCmd = false;
var->fromEnv = fromEnv;
var->readOnly = readOnly;
var->inUse = false;
var->exported = false;
var->reexport = false;
return var;
}

static Substring
CanonicalVarname(Substring name)
{

if (!(Substring_Length(name) > 0 && name.start[0] == '.'))
return name;

if (Substring_Equals(name, ".ALLSRC"))
return Substring_InitStr(ALLSRC);
if (Substring_Equals(name, ".ARCHIVE"))
return Substring_InitStr(ARCHIVE);
if (Substring_Equals(name, ".IMPSRC"))
return Substring_InitStr(IMPSRC);
if (Substring_Equals(name, ".MEMBER"))
return Substring_InitStr(MEMBER);
if (Substring_Equals(name, ".OODATE"))
return Substring_InitStr(OODATE);
if (Substring_Equals(name, ".PREFIX"))
return Substring_InitStr(PREFIX);
if (Substring_Equals(name, ".TARGET"))
return Substring_InitStr(TARGET);

if (Substring_Equals(name, ".SHELL") && shellPath == NULL)
Shell_Init();



return name;
}

static Var *
GNode_FindVar(GNode *scope, Substring varname, unsigned int hash)
{
return HashTable_FindValueBySubstringHash(&scope->vars, varname, hash);
}














static Var *
VarFindSubstring(Substring name, GNode *scope, bool elsewhere)
{
Var *var;
unsigned int nameHash;


name = CanonicalVarname(name);
nameHash = Hash_Substring(name);

var = GNode_FindVar(scope, name, nameHash);
if (!elsewhere)
return var;

if (var == NULL && scope != SCOPE_CMDLINE)
var = GNode_FindVar(SCOPE_CMDLINE, name, nameHash);

if (!opts.checkEnvFirst && var == NULL && scope != SCOPE_GLOBAL) {
var = GNode_FindVar(SCOPE_GLOBAL, name, nameHash);
if (var == NULL && scope != SCOPE_INTERNAL) {

var = GNode_FindVar(SCOPE_INTERNAL, name, nameHash);
}
}

if (var == NULL) {
FStr envName;
const char *envValue;






envName = Substring_Str(name);
envValue = getenv(envName.str);
if (envValue != NULL)
return VarNew(envName, envValue, true, false);
FStr_Done(&envName);

if (opts.checkEnvFirst && scope != SCOPE_GLOBAL) {
var = GNode_FindVar(SCOPE_GLOBAL, name, nameHash);
if (var == NULL && scope != SCOPE_INTERNAL)
var = GNode_FindVar(SCOPE_INTERNAL, name,
nameHash);
return var;
}

return NULL;
}

return var;
}


static Var *
VarFind(const char *name, GNode *scope, bool elsewhere)
{
return VarFindSubstring(Substring_InitStr(name), scope, elsewhere);
}


static void
VarFreeEnv(Var *v)
{
if (!v->fromEnv)
return;

FStr_Done(&v->name);
Buf_Done(&v->val);
free(v);
}


static Var *
VarAdd(const char *name, const char *value, GNode *scope, VarSetFlags flags)
{
HashEntry *he = HashTable_CreateEntry(&scope->vars, name, NULL);
Var *v = VarNew(FStr_InitRefer( he->key), value,
false, (flags & VAR_SET_READONLY) != 0);
HashEntry_Set(he, v);
DEBUG3(VAR, "%s: %s = %s\n", scope->name, name, value);
return v;
}





void
Var_Delete(GNode *scope, const char *varname)
{
HashEntry *he = HashTable_FindEntry(&scope->vars, varname);
Var *v;

if (he == NULL) {
DEBUG2(VAR, "%s:delete %s (not found)\n", scope->name, varname);
return;
}

DEBUG2(VAR, "%s:delete %s\n", scope->name, varname);
v = he->value;
if (v->inUse) {
Parse_Error(PARSE_FATAL,
"Cannot delete variable \"%s\" while it is used",
v->name.str);
return;
}
if (v->exported)
unsetenv(v->name.str);
if (strcmp(v->name.str, MAKE_EXPORTED) == 0)
var_exportedVars = VAR_EXPORTED_NONE;
assert(v->name.freeIt == NULL);
HashTable_DeleteEntry(&scope->vars, he);
Buf_Done(&v->val);
free(v);
}





void
Var_DeleteExpand(GNode *scope, const char *name)
{
FStr varname = FStr_InitRefer(name);

if (strchr(varname.str, '$') != NULL) {
char *expanded;
(void)Var_Subst(varname.str, SCOPE_GLOBAL, VARE_WANTRES,
&expanded);

varname = FStr_InitOwn(expanded);
}

Var_Delete(scope, varname.str);
FStr_Done(&varname);
}





void
Var_Undef(const char *arg)
{
VarParseResult vpr;
char *expanded;
Words varnames;
size_t i;

if (arg[0] == '\0') {
Parse_Error(PARSE_FATAL,
"The .undef directive requires an argument");
return;
}

vpr = Var_Subst(arg, SCOPE_GLOBAL, VARE_WANTRES, &expanded);
if (vpr != VPR_OK) {
Parse_Error(PARSE_FATAL,
"Error in variable names to be undefined");
return;
}

varnames = Str_Words(expanded, false);
if (varnames.len == 1 && varnames.words[0][0] == '\0')
varnames.len = 0;

for (i = 0; i < varnames.len; i++) {
const char *varname = varnames.words[i];
Global_Delete(varname);
}

Words_Free(varnames);
free(expanded);
}

static bool
MayExport(const char *name)
{
if (name[0] == '.')
return false;
if (name[0] == '-')
return false;
if (name[1] == '\0') {






switch (name[0]) {
case '@':
case '%':
case '*':
case '!':
return false;
}
}
return true;
}

static bool
ExportVarEnv(Var *v)
{
const char *name = v->name.str;
char *val = v->val.data;
char *expr;

if (v->exported && !v->reexport)
return false;

if (strchr(val, '$') == NULL) {
if (!v->exported)
setenv(name, val, 1);
return true;
}

if (v->inUse) {




return false;
}


expr = str_concat3("${", name, "}");
(void)Var_Subst(expr, SCOPE_GLOBAL, VARE_WANTRES, &val);

setenv(name, val, 1);
free(val);
free(expr);
return true;
}

static bool
ExportVarPlain(Var *v)
{
if (strchr(v->val.data, '$') == NULL) {
setenv(v->name.str, v->val.data, 1);
v->exported = true;
v->reexport = false;
return true;
}







v->exported = true;
v->reexport = true;
return true;
}

static bool
ExportVarLiteral(Var *v)
{
if (v->exported && !v->reexport)
return false;

if (!v->exported)
setenv(v->name.str, v->val.data, 1);

return true;
}






static bool
ExportVar(const char *name, VarExportMode mode)
{
Var *v;

if (!MayExport(name))
return false;

v = VarFind(name, SCOPE_GLOBAL, false);
if (v == NULL)
return false;

if (mode == VEM_ENV)
return ExportVarEnv(v);
else if (mode == VEM_PLAIN)
return ExportVarPlain(v);
else
return ExportVarLiteral(v);
}





void
Var_ReexportVars(void)
{
char *xvarnames;







char tmp[21];
snprintf(tmp, sizeof tmp, "%d", makelevel + 1);
setenv(MAKE_LEVEL_ENV, tmp, 1);

if (var_exportedVars == VAR_EXPORTED_NONE)
return;

if (var_exportedVars == VAR_EXPORTED_ALL) {
HashIter hi;


HashIter_Init(&hi, &SCOPE_GLOBAL->vars);
while (HashIter_Next(&hi) != NULL) {
Var *var = hi.entry->value;
ExportVar(var->name.str, VEM_ENV);
}
return;
}

(void)Var_Subst("${" MAKE_EXPORTED ":O:u}", SCOPE_GLOBAL, VARE_WANTRES,
&xvarnames);

if (xvarnames[0] != '\0') {
Words varnames = Str_Words(xvarnames, false);
size_t i;

for (i = 0; i < varnames.len; i++)
ExportVar(varnames.words[i], VEM_ENV);
Words_Free(varnames);
}
free(xvarnames);
}

static void
ExportVars(const char *varnames, bool isExport, VarExportMode mode)

{
Words words = Str_Words(varnames, false);
size_t i;

if (words.len == 1 && words.words[0][0] == '\0')
words.len = 0;

for (i = 0; i < words.len; i++) {
const char *varname = words.words[i];
if (!ExportVar(varname, mode))
continue;

if (var_exportedVars == VAR_EXPORTED_NONE)
var_exportedVars = VAR_EXPORTED_SOME;

if (isExport && mode == VEM_PLAIN)
Global_Append(MAKE_EXPORTED, varname);
}
Words_Free(words);
}

static void
ExportVarsExpand(const char *uvarnames, bool isExport, VarExportMode mode)
{
char *xvarnames;

(void)Var_Subst(uvarnames, SCOPE_GLOBAL, VARE_WANTRES, &xvarnames);

ExportVars(xvarnames, isExport, mode);
free(xvarnames);
}


void
Var_Export(VarExportMode mode, const char *varnames)
{
if (mode == VEM_PLAIN && varnames[0] == '\0') {
var_exportedVars = VAR_EXPORTED_ALL;
return;
}

ExportVarsExpand(varnames, true, mode);
}

void
Var_ExportVars(const char *varnames)
{
ExportVarsExpand(varnames, false, VEM_PLAIN);
}


extern char **environ;

static void
ClearEnv(void)
{
const char *cp;
char **newenv;

cp = getenv(MAKE_LEVEL_ENV);
if (environ == savedEnv) {

newenv = bmake_realloc(environ, 2 * sizeof(char *));
} else {
if (savedEnv != NULL) {
free(savedEnv);
savedEnv = NULL;
}
newenv = bmake_malloc(2 * sizeof(char *));
}


environ = savedEnv = newenv;
newenv[0] = NULL;
newenv[1] = NULL;
if (cp != NULL && *cp != '\0')
setenv(MAKE_LEVEL_ENV, cp, 1);
}

static void
GetVarnamesToUnexport(bool isEnv, const char *arg,
FStr *out_varnames, UnexportWhat *out_what)
{
UnexportWhat what;
FStr varnames = FStr_InitRefer("");

if (isEnv) {
if (arg[0] != '\0') {
Parse_Error(PARSE_FATAL,
"The directive .unexport-env does not take "
"arguments");

}
what = UNEXPORT_ENV;

} else {
what = arg[0] != '\0' ? UNEXPORT_NAMED : UNEXPORT_ALL;
if (what == UNEXPORT_NAMED)
varnames = FStr_InitRefer(arg);
}

if (what != UNEXPORT_NAMED) {
char *expanded;

(void)Var_Subst("${" MAKE_EXPORTED ":O:u}", SCOPE_GLOBAL,
VARE_WANTRES, &expanded);

varnames = FStr_InitOwn(expanded);
}

*out_varnames = varnames;
*out_what = what;
}

static void
UnexportVar(Substring varname, UnexportWhat what)
{
Var *v = VarFindSubstring(varname, SCOPE_GLOBAL, false);
if (v == NULL) {
DEBUG2(VAR, "Not unexporting \"%.*s\" (not found)\n",
(int)Substring_Length(varname), varname.start);
return;
}

DEBUG2(VAR, "Unexporting \"%.*s\"\n",
(int)Substring_Length(varname), varname.start);
if (what != UNEXPORT_ENV && v->exported && !v->reexport)
unsetenv(v->name.str);
v->exported = false;
v->reexport = false;

if (what == UNEXPORT_NAMED) {


char *expr = str_concat3("${" MAKE_EXPORTED ":N",
v->name.str, "}");
char *cp;
(void)Var_Subst(expr, SCOPE_GLOBAL, VARE_WANTRES, &cp);

Global_Set(MAKE_EXPORTED, cp);
free(cp);
free(expr);
}
}

static void
UnexportVars(FStr *varnames, UnexportWhat what)
{
size_t i;
SubstringWords words;

if (what == UNEXPORT_ENV)
ClearEnv();

words = Substring_Words(varnames->str, false);
for (i = 0; i < words.len; i++)
UnexportVar(words.words[i], what);
SubstringWords_Free(words);

if (what != UNEXPORT_NAMED)
Global_Delete(MAKE_EXPORTED);
}






void
Var_UnExport(bool isEnv, const char *arg)
{
UnexportWhat what;
FStr varnames;

GetVarnamesToUnexport(isEnv, arg, &varnames, &what);
UnexportVars(&varnames, what);
FStr_Done(&varnames);
}








static bool
ExistsInCmdline(const char *name, const char *val)
{
Var *v;

v = VarFind(name, SCOPE_CMDLINE, false);
if (v == NULL)
return false;

if (v->fromCmd) {
DEBUG3(VAR, "%s: %s = %s ignored!\n",
SCOPE_GLOBAL->name, name, val);
return true;
}

VarFreeEnv(v);
return false;
}


void
Var_SetWithFlags(GNode *scope, const char *name, const char *val,
VarSetFlags flags)
{
Var *v;

assert(val != NULL);
if (name[0] == '\0') {
DEBUG0(VAR, "SetVar: variable name is empty - ignored\n");
return;
}

if (scope == SCOPE_GLOBAL && ExistsInCmdline(name, val))
return;






v = VarFind(name, scope, false);
if (v == NULL) {
if (scope == SCOPE_CMDLINE && !(flags & VAR_SET_NO_EXPORT)) {







Var_Delete(SCOPE_GLOBAL, name);
}
if (strcmp(name, ".SUFFIXES") == 0) {

DEBUG3(VAR, "%s: %s = %s ignored (read-only)\n",
scope->name, name, val);
return;
}
v = VarAdd(name, val, scope, flags);
} else {
if (v->readOnly && !(flags & VAR_SET_READONLY)) {
DEBUG3(VAR, "%s: %s = %s ignored (read-only)\n",
scope->name, name, val);
return;
}
Buf_Empty(&v->val);
Buf_AddStr(&v->val, val);

DEBUG3(VAR, "%s: %s = %s\n", scope->name, name, val);
if (v->exported)
ExportVar(name, VEM_PLAIN);
}





if (scope == SCOPE_CMDLINE && !(flags & VAR_SET_NO_EXPORT) &&
name[0] != '.') {
v->fromCmd = true;







if (!opts.varNoExportEnv)
setenv(name, val, 1);




Global_Append(MAKEOVERRIDES, name);
}

if (name[0] == '.' && strcmp(name, MAKE_SAVE_DOLLARS) == 0)
save_dollars = ParseBoolean(val, save_dollars);

if (v != NULL)
VarFreeEnv(v);
}


void
Var_SetExpandWithFlags(GNode *scope, const char *name, const char *val,
VarSetFlags flags)
{
const char *unexpanded_name = name;
FStr varname = FStr_InitRefer(name);

assert(val != NULL);

if (strchr(varname.str, '$') != NULL) {
char *expanded;
(void)Var_Subst(varname.str, scope, VARE_WANTRES, &expanded);

varname = FStr_InitOwn(expanded);
}

if (varname.str[0] == '\0') {
DEBUG2(VAR,
"Var_SetExpand: variable name \"%s\" expands "
"to empty string, with value \"%s\" - ignored\n",
unexpanded_name, val);
} else
Var_SetWithFlags(scope, varname.str, val, flags);

FStr_Done(&varname);
}

void
Var_Set(GNode *scope, const char *name, const char *val)
{
Var_SetWithFlags(scope, name, val, VAR_SET_NONE);
}












void
Var_SetExpand(GNode *scope, const char *name, const char *val)
{
Var_SetExpandWithFlags(scope, name, val, VAR_SET_NONE);
}

void
Global_Set(const char *name, const char *value)
{
Var_Set(SCOPE_GLOBAL, name, value);
}

void
Global_SetExpand(const char *name, const char *value)
{
Var_SetExpand(SCOPE_GLOBAL, name, value);
}

void
Global_Delete(const char *name)
{
Var_Delete(SCOPE_GLOBAL, name);
}







void
Var_Append(GNode *scope, const char *name, const char *val)
{
Var *v;

v = VarFind(name, scope, scope == SCOPE_GLOBAL);

if (v == NULL) {
Var_SetWithFlags(scope, name, val, VAR_SET_NONE);
} else if (v->readOnly) {
DEBUG1(VAR, "Ignoring append to %s since it is read-only\n",
name);
} else if (scope == SCOPE_CMDLINE || !v->fromCmd) {
Buf_AddByte(&v->val, ' ');
Buf_AddStr(&v->val, val);

DEBUG3(VAR, "%s: %s = %s\n", scope->name, name, v->val.data);

if (v->fromEnv) {






v->fromEnv = false;





HashTable_Set(&scope->vars, name, v);
}
}
}





















void
Var_AppendExpand(GNode *scope, const char *name, const char *val)
{
FStr xname = FStr_InitRefer(name);

assert(val != NULL);

if (strchr(name, '$') != NULL) {
char *expanded;
(void)Var_Subst(name, scope, VARE_WANTRES, &expanded);

xname = FStr_InitOwn(expanded);
if (expanded[0] == '\0') {
DEBUG2(VAR,
"Var_AppendExpand: variable name \"%s\" expands "
"to empty string, with value \"%s\" - ignored\n",
name, val);
FStr_Done(&xname);
return;
}
}

Var_Append(scope, xname.str, val);

FStr_Done(&xname);
}

void
Global_Append(const char *name, const char *value)
{
Var_Append(SCOPE_GLOBAL, name, value);
}

bool
Var_Exists(GNode *scope, const char *name)
{
Var *v = VarFind(name, scope, true);
if (v == NULL)
return false;

VarFreeEnv(v);
return true;
}









bool
Var_ExistsExpand(GNode *scope, const char *name)
{
FStr varname = FStr_InitRefer(name);
bool exists;

if (strchr(varname.str, '$') != NULL) {
char *expanded;
(void)Var_Subst(varname.str, scope, VARE_WANTRES, &expanded);

varname = FStr_InitOwn(expanded);
}

exists = Var_Exists(scope, varname.str);
FStr_Done(&varname);
return exists;
}













FStr
Var_Value(GNode *scope, const char *name)
{
Var *v = VarFind(name, scope, true);
char *value;

if (v == NULL)
return FStr_InitRefer(NULL);

if (!v->fromEnv)
return FStr_InitRefer(v->val.data);


FStr_Done(&v->name);
value = Buf_DoneData(&v->val);
free(v);
return FStr_InitOwn(value);
}





const char *
GNode_ValueDirect(GNode *gn, const char *name)
{
Var *v = VarFind(name, gn, false);
return v != NULL ? v->val.data : NULL;
}

static VarEvalMode
VarEvalMode_WithoutKeepDollar(VarEvalMode emode)
{
if (emode == VARE_KEEP_DOLLAR_UNDEF)
return VARE_EVAL_KEEP_UNDEF;
if (emode == VARE_EVAL_KEEP_DOLLAR)
return VARE_WANTRES;
return emode;
}

static VarEvalMode
VarEvalMode_UndefOk(VarEvalMode emode)
{
return emode == VARE_UNDEFERR ? VARE_WANTRES : emode;
}

static bool
VarEvalMode_ShouldEval(VarEvalMode emode)
{
return emode != VARE_PARSE_ONLY;
}

static bool
VarEvalMode_ShouldKeepUndef(VarEvalMode emode)
{
return emode == VARE_EVAL_KEEP_UNDEF ||
emode == VARE_KEEP_DOLLAR_UNDEF;
}

static bool
VarEvalMode_ShouldKeepDollar(VarEvalMode emode)
{
return emode == VARE_EVAL_KEEP_DOLLAR ||
emode == VARE_KEEP_DOLLAR_UNDEF;
}


static void
SepBuf_Init(SepBuf *buf, char sep)
{
Buf_InitSize(&buf->buf, 32);
buf->needSep = false;
buf->sep = sep;
}

static void
SepBuf_Sep(SepBuf *buf)
{
buf->needSep = true;
}

static void
SepBuf_AddBytes(SepBuf *buf, const char *mem, size_t mem_size)
{
if (mem_size == 0)
return;
if (buf->needSep && buf->sep != '\0') {
Buf_AddByte(&buf->buf, buf->sep);
buf->needSep = false;
}
Buf_AddBytes(&buf->buf, mem, mem_size);
}

static void
SepBuf_AddBytesBetween(SepBuf *buf, const char *start, const char *end)
{
SepBuf_AddBytes(buf, start, (size_t)(end - start));
}

static void
SepBuf_AddStr(SepBuf *buf, const char *str)
{
SepBuf_AddBytes(buf, str, strlen(str));
}

static void
SepBuf_AddSubstring(SepBuf *buf, Substring sub)
{
SepBuf_AddBytesBetween(buf, sub.start, sub.end);
}

static char *
SepBuf_DoneData(SepBuf *buf)
{
return Buf_DoneData(&buf->buf);
}














typedef void (*ModifyWordProc)(Substring word, SepBuf *buf, void *data);







static void
ModifyWord_Head(Substring word, SepBuf *buf, void *dummy MAKE_ATTR_UNUSED)
{
SepBuf_AddSubstring(buf, Substring_Dirname(word));
}






static void
ModifyWord_Tail(Substring word, SepBuf *buf, void *dummy MAKE_ATTR_UNUSED)
{
SepBuf_AddSubstring(buf, Substring_Basename(word));
}






static void
ModifyWord_Suffix(Substring word, SepBuf *buf, void *dummy MAKE_ATTR_UNUSED)
{
const char *lastDot = Substring_LastIndex(word, '.');
if (lastDot != NULL)
SepBuf_AddBytesBetween(buf, lastDot + 1, word.end);
}






static void
ModifyWord_Root(Substring word, SepBuf *buf, void *dummy MAKE_ATTR_UNUSED)
{
const char *lastDot, *end;

lastDot = Substring_LastIndex(word, '.');
end = lastDot != NULL ? lastDot : word.end;
SepBuf_AddBytesBetween(buf, word.start, end);
}





static void
ModifyWord_Match(Substring word, SepBuf *buf, void *data)
{
const char *pattern = data;

assert(word.end[0] == '\0');
if (Str_Match(word.start, pattern))
SepBuf_AddSubstring(buf, word);
}





static void
ModifyWord_NoMatch(Substring word, SepBuf *buf, void *data)
{
const char *pattern = data;

assert(word.end[0] == '\0');
if (!Str_Match(word.start, pattern))
SepBuf_AddSubstring(buf, word);
}

#if defined(SYSVVARSUB)
struct ModifyWord_SysVSubstArgs {
GNode *scope;
Substring lhsPrefix;
bool lhsPercent;
Substring lhsSuffix;
const char *rhs;
};


static void
ModifyWord_SysVSubst(Substring word, SepBuf *buf, void *data)
{
const struct ModifyWord_SysVSubstArgs *args = data;
FStr rhs;
char *rhsExp;
const char *percent;

if (Substring_IsEmpty(word))
return;

if (!Substring_HasPrefix(word, args->lhsPrefix))
goto no_match;
if (!Substring_HasSuffix(word, args->lhsSuffix))
goto no_match;

rhs = FStr_InitRefer(args->rhs);
if (strchr(rhs.str, '$') != NULL) {
(void)Var_Subst(args->rhs, args->scope, VARE_WANTRES, &rhsExp);

rhs = FStr_InitOwn(rhsExp);
}

percent = args->lhsPercent ? strchr(rhs.str, '%') : NULL;

if (percent != NULL)
SepBuf_AddBytesBetween(buf, rhs.str, percent);
if (percent != NULL || !args->lhsPercent)
SepBuf_AddBytesBetween(buf,
word.start + Substring_Length(args->lhsPrefix),
word.end - Substring_Length(args->lhsSuffix));
SepBuf_AddStr(buf, percent != NULL ? percent + 1 : rhs.str);

FStr_Done(&rhs);
return;

no_match:
SepBuf_AddSubstring(buf, word);
}
#endif


struct ModifyWord_SubstArgs {
Substring lhs;
Substring rhs;
PatternFlags pflags;
bool matched;
};

static const char *
Substring_Find(Substring haystack, Substring needle)
{
size_t len, needleLen, i;

len = Substring_Length(haystack);
needleLen = Substring_Length(needle);
for (i = 0; i + needleLen <= len; i++)
if (memcmp(haystack.start + i, needle.start, needleLen) == 0)
return haystack.start + i;
return NULL;
}





static void
ModifyWord_Subst(Substring word, SepBuf *buf, void *data)
{
struct ModifyWord_SubstArgs *args = data;
size_t wordLen, lhsLen;
const char *wordEnd, *match;

wordLen = Substring_Length(word);
wordEnd = word.end;
if (args->pflags.subOnce && args->matched)
goto nosub;

lhsLen = Substring_Length(args->lhs);
if (args->pflags.anchorStart) {
if (wordLen < lhsLen ||
memcmp(word.start, args->lhs.start, lhsLen) != 0)
goto nosub;

if (args->pflags.anchorEnd && wordLen != lhsLen)
goto nosub;


SepBuf_AddSubstring(buf, args->rhs);
SepBuf_AddBytesBetween(buf, word.start + lhsLen, wordEnd);
args->matched = true;
return;
}

if (args->pflags.anchorEnd) {
if (wordLen < lhsLen)
goto nosub;
if (memcmp(wordEnd - lhsLen, args->lhs.start, lhsLen) != 0)
goto nosub;


SepBuf_AddBytesBetween(buf, word.start, wordEnd - lhsLen);
SepBuf_AddSubstring(buf, args->rhs);
args->matched = true;
return;
}

if (Substring_IsEmpty(args->lhs))
goto nosub;


while ((match = Substring_Find(word, args->lhs)) != NULL) {
SepBuf_AddBytesBetween(buf, word.start, match);
SepBuf_AddSubstring(buf, args->rhs);
args->matched = true;
word.start = match + lhsLen;
if (Substring_IsEmpty(word) || !args->pflags.subGlobal)
break;
}
nosub:
SepBuf_AddSubstring(buf, word);
}

#if !defined(NO_REGEX)

static void
VarREError(int reerr, const regex_t *pat, const char *str)
{
size_t errlen = regerror(reerr, pat, NULL, 0);
char *errbuf = bmake_malloc(errlen);
regerror(reerr, pat, errbuf, errlen);
Error("%s: %s", str, errbuf);
free(errbuf);
}





static void
RegexReplace(const char *replace, SepBuf *buf, const char *wp,
const regmatch_t *m, size_t nsub)
{
const char *rp;
unsigned int n;

for (rp = replace; *rp != '\0'; rp++) {
if (*rp == '\\' && (rp[1] == '&' || rp[1] == '\\')) {
SepBuf_AddBytes(buf, rp + 1, 1);
rp++;
continue;
}

if (*rp == '&') {
SepBuf_AddBytesBetween(buf,
wp + (size_t)m[0].rm_so,
wp + (size_t)m[0].rm_eo);
continue;
}

if (*rp != '\\' || !ch_isdigit(rp[1])) {
SepBuf_AddBytes(buf, rp, 1);
continue;
}


n = rp[1] - '0';
rp++;

if (n >= nsub) {
Error("No subexpression \\%u", n);
} else if (m[n].rm_so == -1) {
if (opts.strict) {
Error("No match for subexpression \\%u", n);
}
} else {
SepBuf_AddBytesBetween(buf,
wp + (size_t)m[n].rm_so,
wp + (size_t)m[n].rm_eo);
}
}
}

struct ModifyWord_SubstRegexArgs {
regex_t re;
size_t nsub;
const char *replace;
PatternFlags pflags;
bool matched;
};





static void
ModifyWord_SubstRegex(Substring word, SepBuf *buf, void *data)
{
struct ModifyWord_SubstRegexArgs *args = data;
int xrv;
const char *wp;
int flags = 0;
regmatch_t m[10];

assert(word.end[0] == '\0');
wp = word.start;
if (args->pflags.subOnce && args->matched)
goto no_match;

again:
xrv = regexec(&args->re, wp, args->nsub, m, flags);
if (xrv == 0)
goto ok;
if (xrv != REG_NOMATCH)
VarREError(xrv, &args->re, "Unexpected regex error");
no_match:
SepBuf_AddStr(buf, wp);
return;

ok:
args->matched = true;
SepBuf_AddBytes(buf, wp, (size_t)m[0].rm_so);

RegexReplace(args->replace, buf, wp, m, args->nsub);

wp += (size_t)m[0].rm_eo;
if (args->pflags.subGlobal) {
flags |= REG_NOTBOL;
if (m[0].rm_so == 0 && m[0].rm_eo == 0) {
SepBuf_AddBytes(buf, wp, 1);
wp++;
}
if (*wp != '\0')
goto again;
}
if (*wp != '\0')
SepBuf_AddStr(buf, wp);
}
#endif


struct ModifyWord_LoopArgs {
GNode *scope;
const char *var;
const char *body;
VarEvalMode emode;
};


static void
ModifyWord_Loop(Substring word, SepBuf *buf, void *data)
{
const struct ModifyWord_LoopArgs *args;
char *s;

if (Substring_IsEmpty(word))
return;

args = data;
assert(word.end[0] == '\0');
Var_SetWithFlags(args->scope, args->var, word.start,
VAR_SET_NO_EXPORT);
(void)Var_Subst(args->body, args->scope, args->emode, &s);


assert(word.end[0] == '\0');
DEBUG4(VAR, "ModifyWord_Loop: "
"in \"%s\", replace \"%s\" with \"%s\" to \"%s\"\n",
word.start, args->var, args->body, s);

if (s[0] == '\n' || Buf_EndsWith(&buf->buf, '\n'))
buf->needSep = false;
SepBuf_AddStr(buf, s);
free(s);
}






static char *
VarSelectWords(const char *str, int first, int last,
char sep, bool oneBigWord)
{
SubstringWords words;
int len, start, end, step;
int i;

SepBuf buf;
SepBuf_Init(&buf, sep);

if (oneBigWord) {

words.len = 1;
words.words = bmake_malloc(sizeof(words.words[0]));
words.freeIt = NULL;
words.words[0] = Substring_InitStr(str);
} else {
words = Substring_Words(str, false);
}






len = (int)words.len;
if (first < 0)
first += len + 1;
if (last < 0)
last += len + 1;


if (first > last) {
start = (first > len ? len : first) - 1;
end = last < 1 ? 0 : last - 1;
step = -1;
} else {
start = first < 1 ? 0 : first - 1;
end = last > len ? len : last;
step = 1;
}

for (i = start; (step < 0) == (i >= end); i += step) {
SepBuf_AddSubstring(&buf, words.words[i]);
SepBuf_Sep(&buf);
}

SubstringWords_Free(words);

return SepBuf_DoneData(&buf);
}







static void
ModifyWord_Realpath(Substring word, SepBuf *buf, void *data MAKE_ATTR_UNUSED)
{
struct stat st;
char rbuf[MAXPATHLEN];
const char *rp;

assert(word.end[0] == '\0');
rp = cached_realpath(word.start, rbuf);
if (rp != NULL && *rp == '/' && stat(rp, &st) == 0)
SepBuf_AddStr(buf, rp);
else
SepBuf_AddSubstring(buf, word);
}


static char *
SubstringWords_JoinFree(SubstringWords words)
{
Buffer buf;
size_t i;

Buf_Init(&buf);

for (i = 0; i < words.len; i++) {
if (i != 0) {

Buf_AddByte(&buf, ' ');
}
Buf_AddBytesBetween(&buf,
words.words[i].start, words.words[i].end);
}

SubstringWords_Free(words);

return Buf_DoneData(&buf);
}






static void
VarQuote(const char *str, bool quoteDollar, LazyBuf *buf)
{
const char *p;

LazyBuf_Init(buf, str);
for (p = str; *p != '\0'; p++) {
if (*p == '\n') {
const char *newline = Shell_GetNewline();
if (newline == NULL)
newline = "\\\n";
LazyBuf_AddStr(buf, newline);
continue;
}
if (ch_isspace(*p) || is_shell_metachar((unsigned char)*p))
LazyBuf_Add(buf, '\\');
LazyBuf_Add(buf, *p);
if (quoteDollar && *p == '$')
LazyBuf_AddStr(buf, "\\$");
}
}





static char *
VarHash(const char *str)
{
static const char hexdigits[16] = "0123456789abcdef";
const unsigned char *ustr = (const unsigned char *)str;

uint32_t h = 0x971e137bU;
uint32_t c1 = 0x95543787U;
uint32_t c2 = 0x2ad7eb25U;
size_t len2 = strlen(str);

char *buf;
size_t i;

size_t len;
for (len = len2; len != 0;) {
uint32_t k = 0;
switch (len) {
default:
k = ((uint32_t)ustr[3] << 24) |
((uint32_t)ustr[2] << 16) |
((uint32_t)ustr[1] << 8) |
(uint32_t)ustr[0];
len -= 4;
ustr += 4;
break;
case 3:
k |= (uint32_t)ustr[2] << 16;

case 2:
k |= (uint32_t)ustr[1] << 8;

case 1:
k |= (uint32_t)ustr[0];
len = 0;
}
c1 = c1 * 5 + 0x7b7d159cU;
c2 = c2 * 5 + 0x6bce6396U;
k *= c1;
k = (k << 11) ^ (k >> 21);
k *= c2;
h = (h << 13) ^ (h >> 19);
h = h * 5 + 0x52dce729U;
h ^= k;
}
h ^= (uint32_t)len2;
h *= 0x85ebca6b;
h ^= h >> 13;
h *= 0xc2b2ae35;
h ^= h >> 16;

buf = bmake_malloc(9);
for (i = 0; i < 8; i++) {
buf[i] = hexdigits[h & 0x0f];
h >>= 4;
}
buf[8] = '\0';
return buf;
}

static char *
VarStrftime(const char *fmt, bool zulu, time_t tim)
{
char buf[BUFSIZ];

if (tim == 0)
time(&tim);
if (*fmt == '\0')
fmt = "%c";
strftime(buf, sizeof buf, fmt, zulu ? gmtime(&tim) : localtime(&tim));

buf[sizeof buf - 1] = '\0';
return bmake_strdup(buf);
}































































typedef enum ExprDefined {

DEF_REGULAR,

DEF_UNDEF,





DEF_DEFINED
} ExprDefined;

static const char ExprDefined_Name[][10] = {
"regular",
"undefined",
"defined"
};

#if __STDC_VERSION__ >= 199901L
#define const_member const
#else
#define const_member
#endif


typedef struct Expr {
const char *name;
FStr value;
VarEvalMode const_member emode;
GNode *const_member scope;
ExprDefined defined;
} Expr;



























typedef struct ModChain {
Expr *expr;

char const_member startc;

char const_member endc;

char sep;





bool oneBigWord;
} ModChain;

static void
Expr_Define(Expr *expr)
{
if (expr->defined == DEF_UNDEF)
expr->defined = DEF_DEFINED;
}

static const char *
Expr_Str(const Expr *expr)
{
return expr->value.str;
}

static SubstringWords
Expr_Words(const Expr *expr)
{
return Substring_Words(Expr_Str(expr), false);
}

static void
Expr_SetValue(Expr *expr, FStr value)
{
FStr_Done(&expr->value);
expr->value = value;
}

static void
Expr_SetValueOwn(Expr *expr, char *value)
{
Expr_SetValue(expr, FStr_InitOwn(value));
}

static void
Expr_SetValueRefer(Expr *expr, const char *value)
{
Expr_SetValue(expr, FStr_InitRefer(value));
}

static bool
Expr_ShouldEval(const Expr *expr)
{
return VarEvalMode_ShouldEval(expr->emode);
}

static bool
ModChain_ShouldEval(const ModChain *ch)
{
return Expr_ShouldEval(ch->expr);
}


typedef enum ApplyModifierResult {

AMR_OK,

AMR_UNKNOWN,

AMR_BAD,

AMR_CLEANUP
} ApplyModifierResult;





static bool
IsEscapedModifierPart(const char *p, char delim,
struct ModifyWord_SubstArgs *subst)
{
if (p[0] != '\\')
return false;
if (p[1] == delim || p[1] == '\\' || p[1] == '$')
return true;
return p[1] == '&' && subst != NULL;
}




static void
ParseModifierPartExpr(const char **pp, LazyBuf *part, const ModChain *ch,
VarEvalMode emode)
{
const char *p = *pp;
FStr nested_val;

(void)Var_Parse(&p, ch->expr->scope,
VarEvalMode_WithoutKeepDollar(emode), &nested_val);

LazyBuf_AddStr(part, nested_val.str);
FStr_Done(&nested_val);
*pp = p;
}













static void
ParseModifierPartDollar(const char **pp, LazyBuf *part)
{
const char *p = *pp;
const char *start = *pp;

if (p[1] == '(' || p[1] == '{') {
char startc = p[1];
int endc = startc == '(' ? ')' : '}';
int depth = 1;

for (p += 2; *p != '\0' && depth > 0; p++) {
if (p[-1] != '\\') {
if (*p == startc)
depth++;
if (*p == endc)
depth--;
}
}
LazyBuf_AddBytesBetween(part, start, p);
*pp = p;
} else {
LazyBuf_Add(part, *start);
*pp = p + 1;
}
}


static VarParseResult
ParseModifierPartSubst(
const char **pp,
char delim,
VarEvalMode emode,
ModChain *ch,
LazyBuf *part,


PatternFlags *out_pflags,


struct ModifyWord_SubstArgs *subst
)
{
const char *p;

p = *pp;
LazyBuf_Init(part, p);

while (*p != '\0' && *p != delim) {
if (IsEscapedModifierPart(p, delim, subst)) {
LazyBuf_Add(part, p[1]);
p += 2;
} else if (*p != '$') {
if (subst != NULL && *p == '&')
LazyBuf_AddSubstring(part, subst->lhs);
else
LazyBuf_Add(part, *p);
p++;
} else if (p[1] == delim) {
if (out_pflags != NULL)
out_pflags->anchorEnd = true;
else
LazyBuf_Add(part, *p);
p++;
} else if (VarEvalMode_ShouldEval(emode))
ParseModifierPartExpr(&p, part, ch, emode);
else
ParseModifierPartDollar(&p, part);
}

if (*p != delim) {
*pp = p;
Error("Unfinished modifier for \"%s\" ('%c' missing)",
ch->expr->name, delim);
LazyBuf_Done(part);
return VPR_ERR;
}

*pp = p + 1;

{
Substring sub = LazyBuf_Get(part);
DEBUG2(VAR, "Modifier part: \"%.*s\"\n",
(int)Substring_Length(sub), sub.start);
}

return VPR_OK;
}











static VarParseResult
ParseModifierPart(

const char **pp,

char delim,

VarEvalMode emode,
ModChain *ch,
LazyBuf *part
)
{
return ParseModifierPartSubst(pp, delim, emode, ch, part, NULL, NULL);
}

MAKE_INLINE bool
IsDelimiter(char c, const ModChain *ch)
{
return c == ':' || c == ch->endc;
}


MAKE_INLINE bool
ModMatch(const char *mod, const char *modname, const ModChain *ch)
{
size_t n = strlen(modname);
return strncmp(mod, modname, n) == 0 && IsDelimiter(mod[n], ch);
}


MAKE_INLINE bool
ModMatchEq(const char *mod, const char *modname, const ModChain *ch)
{
size_t n = strlen(modname);
return strncmp(mod, modname, n) == 0 &&
(IsDelimiter(mod[n], ch) || mod[n] == '=');
}

static bool
TryParseIntBase0(const char **pp, int *out_num)
{
char *end;
long n;

errno = 0;
n = strtol(*pp, &end, 0);

if (end == *pp)
return false;
if ((n == LONG_MIN || n == LONG_MAX) && errno == ERANGE)
return false;
if (n < INT_MIN || n > INT_MAX)
return false;

*pp = end;
*out_num = (int)n;
return true;
}

static bool
TryParseSize(const char **pp, size_t *out_num)
{
char *end;
unsigned long n;

if (!ch_isdigit(**pp))
return false;

errno = 0;
n = strtoul(*pp, &end, 10);
if (n == ULONG_MAX && errno == ERANGE)
return false;
if (n > SIZE_MAX)
return false;

*pp = end;
*out_num = (size_t)n;
return true;
}

static bool
TryParseChar(const char **pp, int base, char *out_ch)
{
char *end;
unsigned long n;

if (!ch_isalnum(**pp))
return false;

errno = 0;
n = strtoul(*pp, &end, base);
if (n == ULONG_MAX && errno == ERANGE)
return false;
if (n > UCHAR_MAX)
return false;

*pp = end;
*out_ch = (char)n;
return true;
}





static void
ModifyWords(ModChain *ch,
ModifyWordProc modifyWord, void *modifyWord_args,
bool oneBigWord)
{
Expr *expr = ch->expr;
const char *val = Expr_Str(expr);
SepBuf result;
SubstringWords words;
size_t i;
Substring word;

if (oneBigWord) {
SepBuf_Init(&result, ch->sep);

word = Substring_InitStr(val);
modifyWord(word, &result, modifyWord_args);
goto done;
}

words = Substring_Words(val, false);

DEBUG3(VAR, "ModifyWords: split \"%s\" into %u %s\n",
val, (unsigned)words.len, words.len != 1 ? "words" : "word");

SepBuf_Init(&result, ch->sep);
for (i = 0; i < words.len; i++) {
modifyWord(words.words[i], &result, modifyWord_args);
if (result.buf.len > 0)
SepBuf_Sep(&result);
}

SubstringWords_Free(words);

done:
Expr_SetValueOwn(expr, SepBuf_DoneData(&result));
}


static ApplyModifierResult
ApplyModifier_Loop(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
struct ModifyWord_LoopArgs args;
char prev_sep;
VarParseResult res;
LazyBuf tvarBuf, strBuf;
FStr tvar, str;

args.scope = expr->scope;

(*pp)++;
res = ParseModifierPart(pp, '@', VARE_PARSE_ONLY, ch, &tvarBuf);
if (res != VPR_OK)
return AMR_CLEANUP;
tvar = LazyBuf_DoneGet(&tvarBuf);
args.var = tvar.str;
if (strchr(args.var, '$') != NULL) {
Parse_Error(PARSE_FATAL,
"In the :@ modifier of \"%s\", the variable name \"%s\" "
"must not contain a dollar",
expr->name, args.var);
return AMR_CLEANUP;
}

res = ParseModifierPart(pp, '@', VARE_PARSE_ONLY, ch, &strBuf);
if (res != VPR_OK)
return AMR_CLEANUP;
str = LazyBuf_DoneGet(&strBuf);
args.body = str.str;

if (!Expr_ShouldEval(expr))
goto done;

args.emode = VarEvalMode_WithoutKeepDollar(expr->emode);
prev_sep = ch->sep;
ch->sep = ' ';
ModifyWords(ch, ModifyWord_Loop, &args, ch->oneBigWord);
ch->sep = prev_sep;

Var_Delete(expr->scope, args.var);

done:
FStr_Done(&tvar);
FStr_Done(&str);
return AMR_OK;
}


static ApplyModifierResult
ApplyModifier_Defined(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
LazyBuf buf;
const char *p;

VarEvalMode emode = VARE_PARSE_ONLY;
if (Expr_ShouldEval(expr))
if ((**pp == 'D') == (expr->defined == DEF_REGULAR))
emode = expr->emode;

p = *pp + 1;
LazyBuf_Init(&buf, p);
while (!IsDelimiter(*p, ch) && *p != '\0') {







if (*p == '\\') {
char c = p[1];
if (IsDelimiter(c, ch) || c == '$' || c == '\\') {
LazyBuf_Add(&buf, c);
p += 2;
continue;
}
}


if (*p == '$') {
FStr nested_val;

(void)Var_Parse(&p, expr->scope, emode, &nested_val);

if (Expr_ShouldEval(expr))
LazyBuf_AddStr(&buf, nested_val.str);
FStr_Done(&nested_val);
continue;
}


LazyBuf_Add(&buf, *p);
p++;
}
*pp = p;

Expr_Define(expr);

if (VarEvalMode_ShouldEval(emode))
Expr_SetValue(expr, Substring_Str(LazyBuf_Get(&buf)));
else
LazyBuf_Done(&buf);

return AMR_OK;
}


static ApplyModifierResult
ApplyModifier_Literal(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;

(*pp)++;

if (Expr_ShouldEval(expr)) {
Expr_Define(expr);
Expr_SetValueOwn(expr, bmake_strdup(expr->name));
}

return AMR_OK;
}

static bool
TryParseTime(const char **pp, time_t *out_time)
{
char *end;
unsigned long n;

if (!ch_isdigit(**pp))
return false;

errno = 0;
n = strtoul(*pp, &end, 10);
if (n == ULONG_MAX && errno == ERANGE)
return false;

*pp = end;
*out_time = (time_t)n;
return true;
}


static ApplyModifierResult
ApplyModifier_Gmtime(const char **pp, ModChain *ch)
{
Expr *expr;
time_t utc;

const char *mod = *pp;
if (!ModMatchEq(mod, "gmtime", ch))
return AMR_UNKNOWN;

if (mod[6] == '=') {
const char *p = mod + 7;
if (!TryParseTime(&p, &utc)) {
Parse_Error(PARSE_FATAL,
"Invalid time value at \"%s\"", mod + 7);
return AMR_CLEANUP;
}
*pp = p;
} else {
utc = 0;
*pp = mod + 6;
}

expr = ch->expr;
if (Expr_ShouldEval(expr))
Expr_SetValueOwn(expr,
VarStrftime(Expr_Str(expr), true, utc));

return AMR_OK;
}


static ApplyModifierResult
ApplyModifier_Localtime(const char **pp, ModChain *ch)
{
Expr *expr;
time_t utc;

const char *mod = *pp;
if (!ModMatchEq(mod, "localtime", ch))
return AMR_UNKNOWN;

if (mod[9] == '=') {
const char *p = mod + 10;
if (!TryParseTime(&p, &utc)) {
Parse_Error(PARSE_FATAL,
"Invalid time value at \"%s\"", mod + 10);
return AMR_CLEANUP;
}
*pp = p;
} else {
utc = 0;
*pp = mod + 9;
}

expr = ch->expr;
if (Expr_ShouldEval(expr))
Expr_SetValueOwn(expr,
VarStrftime(Expr_Str(expr), false, utc));

return AMR_OK;
}


static ApplyModifierResult
ApplyModifier_Hash(const char **pp, ModChain *ch)
{
if (!ModMatch(*pp, "hash", ch))
return AMR_UNKNOWN;
*pp += 4;

if (ModChain_ShouldEval(ch))
Expr_SetValueOwn(ch->expr, VarHash(Expr_Str(ch->expr)));

return AMR_OK;
}


static ApplyModifierResult
ApplyModifier_Path(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
GNode *gn;
char *path;

(*pp)++;

if (!Expr_ShouldEval(expr))
return AMR_OK;

Expr_Define(expr);

gn = Targ_FindNode(expr->name);
if (gn == NULL || gn->type & OP_NOPATH) {
path = NULL;
} else if (gn->path != NULL) {
path = bmake_strdup(gn->path);
} else {
SearchPath *searchPath = Suff_FindPath(gn);
path = Dir_FindFile(expr->name, searchPath);
}
if (path == NULL)
path = bmake_strdup(expr->name);
Expr_SetValueOwn(expr, path);

return AMR_OK;
}


static ApplyModifierResult
ApplyModifier_ShellCommand(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
const char *errfmt;
VarParseResult res;
LazyBuf cmdBuf;
FStr cmd;

(*pp)++;
res = ParseModifierPart(pp, '!', expr->emode, ch, &cmdBuf);
if (res != VPR_OK)
return AMR_CLEANUP;
cmd = LazyBuf_DoneGet(&cmdBuf);


errfmt = NULL;
if (Expr_ShouldEval(expr))
Expr_SetValueOwn(expr, Cmd_Exec(cmd.str, &errfmt));
else
Expr_SetValueRefer(expr, "");
if (errfmt != NULL)
Error(errfmt, cmd.str);
FStr_Done(&cmd);
Expr_Define(expr);

return AMR_OK;
}





static ApplyModifierResult
ApplyModifier_Range(const char **pp, ModChain *ch)
{
size_t n;
Buffer buf;
size_t i;

const char *mod = *pp;
if (!ModMatchEq(mod, "range", ch))
return AMR_UNKNOWN;

if (mod[5] == '=') {
const char *p = mod + 6;
if (!TryParseSize(&p, &n)) {
Parse_Error(PARSE_FATAL,
"Invalid number \"%s\" for ':range' modifier",
mod + 6);
return AMR_CLEANUP;
}
*pp = p;
} else {
n = 0;
*pp = mod + 5;
}

if (!ModChain_ShouldEval(ch))
return AMR_OK;

if (n == 0) {
SubstringWords words = Expr_Words(ch->expr);
n = words.len;
SubstringWords_Free(words);
}

Buf_Init(&buf);

for (i = 0; i < n; i++) {
if (i != 0) {

Buf_AddByte(&buf, ' ');
}
Buf_AddInt(&buf, 1 + (int)i);
}

Expr_SetValueOwn(ch->expr, Buf_DoneData(&buf));
return AMR_OK;
}


static void
ParseModifier_Match(const char **pp, const ModChain *ch,
char **out_pattern)
{
const char *mod = *pp;
Expr *expr = ch->expr;
bool copy = false;
bool needSubst = false;
const char *endpat;
char *pattern;











int nest = 0;
const char *p;
for (p = mod + 1; *p != '\0' && !(*p == ':' && nest == 0); p++) {
if (*p == '\\' &&
(IsDelimiter(p[1], ch) || p[1] == ch->startc)) {
if (!needSubst)
copy = true;
p++;
continue;
}
if (*p == '$')
needSubst = true;
if (*p == '(' || *p == '{')
nest++;
if (*p == ')' || *p == '}') {
nest--;
if (nest < 0)
break;
}
}
*pp = p;
endpat = p;

if (copy) {
char *dst;
const char *src;


pattern = bmake_malloc((size_t)(endpat - (mod + 1)) + 1);
dst = pattern;
src = mod + 1;
for (; src < endpat; src++, dst++) {
if (src[0] == '\\' && src + 1 < endpat &&

IsDelimiter(src[1], ch))
src++;
*dst = *src;
}
*dst = '\0';
} else {
pattern = bmake_strsedup(mod + 1, endpat);
}

if (needSubst) {
char *old_pattern = pattern;
(void)Var_Subst(pattern, expr->scope, expr->emode, &pattern);

free(old_pattern);
}

DEBUG2(VAR, "Pattern for ':%c' is \"%s\"\n", mod[0], pattern);

*out_pattern = pattern;
}


static ApplyModifierResult
ApplyModifier_Match(const char **pp, ModChain *ch)
{
const char mod = **pp;
char *pattern;

ParseModifier_Match(pp, ch, &pattern);

if (ModChain_ShouldEval(ch)) {
ModifyWordProc modifyWord =
mod == 'M' ? ModifyWord_Match : ModifyWord_NoMatch;
ModifyWords(ch, modifyWord, pattern, ch->oneBigWord);
}

free(pattern);
return AMR_OK;
}

static void
ParsePatternFlags(const char **pp, PatternFlags *pflags, bool *oneBigWord)
{
for (;; (*pp)++) {
if (**pp == 'g')
pflags->subGlobal = true;
else if (**pp == '1')
pflags->subOnce = true;
else if (**pp == 'W')
*oneBigWord = true;
else
break;
}
}

MAKE_INLINE PatternFlags
PatternFlags_None(void)
{
PatternFlags pflags = { false, false, false, false };
return pflags;
}


static ApplyModifierResult
ApplyModifier_Subst(const char **pp, ModChain *ch)
{
struct ModifyWord_SubstArgs args;
bool oneBigWord;
VarParseResult res;
LazyBuf lhsBuf, rhsBuf;

char delim = (*pp)[1];
if (delim == '\0') {
Error("Missing delimiter for modifier ':S'");
(*pp)++;
return AMR_CLEANUP;
}

*pp += 2;

args.pflags = PatternFlags_None();
args.matched = false;

if (**pp == '^') {
args.pflags.anchorStart = true;
(*pp)++;
}

res = ParseModifierPartSubst(pp, delim, ch->expr->emode, ch, &lhsBuf,
&args.pflags, NULL);
if (res != VPR_OK)
return AMR_CLEANUP;
args.lhs = LazyBuf_Get(&lhsBuf);

res = ParseModifierPartSubst(pp, delim, ch->expr->emode, ch, &rhsBuf,
NULL, &args);
if (res != VPR_OK) {
LazyBuf_Done(&lhsBuf);
return AMR_CLEANUP;
}
args.rhs = LazyBuf_Get(&rhsBuf);

oneBigWord = ch->oneBigWord;
ParsePatternFlags(pp, &args.pflags, &oneBigWord);

ModifyWords(ch, ModifyWord_Subst, &args, oneBigWord);

LazyBuf_Done(&lhsBuf);
LazyBuf_Done(&rhsBuf);
return AMR_OK;
}

#if !defined(NO_REGEX)


static ApplyModifierResult
ApplyModifier_Regex(const char **pp, ModChain *ch)
{
struct ModifyWord_SubstRegexArgs args;
bool oneBigWord;
int error;
VarParseResult res;
LazyBuf reBuf, replaceBuf;
FStr re, replace;

char delim = (*pp)[1];
if (delim == '\0') {
Error("Missing delimiter for :C modifier");
(*pp)++;
return AMR_CLEANUP;
}

*pp += 2;

res = ParseModifierPart(pp, delim, ch->expr->emode, ch, &reBuf);
if (res != VPR_OK)
return AMR_CLEANUP;
re = LazyBuf_DoneGet(&reBuf);

res = ParseModifierPart(pp, delim, ch->expr->emode, ch, &replaceBuf);
if (res != VPR_OK) {
FStr_Done(&re);
return AMR_CLEANUP;
}
replace = LazyBuf_DoneGet(&replaceBuf);
args.replace = replace.str;

args.pflags = PatternFlags_None();
args.matched = false;
oneBigWord = ch->oneBigWord;
ParsePatternFlags(pp, &args.pflags, &oneBigWord);

if (!ModChain_ShouldEval(ch)) {
FStr_Done(&replace);
FStr_Done(&re);
return AMR_OK;
}

error = regcomp(&args.re, re.str, REG_EXTENDED);
if (error != 0) {
VarREError(error, &args.re, "Regex compilation error");
FStr_Done(&replace);
FStr_Done(&re);
return AMR_CLEANUP;
}

args.nsub = args.re.re_nsub + 1;
if (args.nsub > 10)
args.nsub = 10;

ModifyWords(ch, ModifyWord_SubstRegex, &args, oneBigWord);

regfree(&args.re);
FStr_Done(&replace);
FStr_Done(&re);
return AMR_OK;
}

#endif


static ApplyModifierResult
ApplyModifier_Quote(const char **pp, ModChain *ch)
{
LazyBuf buf;
bool quoteDollar;

quoteDollar = **pp == 'q';
if (!IsDelimiter((*pp)[1], ch))
return AMR_UNKNOWN;
(*pp)++;

if (!ModChain_ShouldEval(ch))
return AMR_OK;

VarQuote(Expr_Str(ch->expr), quoteDollar, &buf);
if (buf.data != NULL)
Expr_SetValue(ch->expr, LazyBuf_DoneGet(&buf));
else
LazyBuf_Done(&buf);

return AMR_OK;
}


static void
ModifyWord_Copy(Substring word, SepBuf *buf, void *data MAKE_ATTR_UNUSED)
{
SepBuf_AddSubstring(buf, word);
}


static ApplyModifierResult
ApplyModifier_ToSep(const char **pp, ModChain *ch)
{
const char *sep = *pp + 2;









if (sep[0] != ch->endc && IsDelimiter(sep[1], ch)) {
*pp = sep + 1;
ch->sep = sep[0];
goto ok;
}


if (IsDelimiter(sep[0], ch)) {
*pp = sep;
ch->sep = '\0';
goto ok;
}


if (sep[0] != '\\') {
(*pp)++;
return AMR_BAD;
}


if (sep[1] == 'n') {
*pp = sep + 2;
ch->sep = '\n';
goto ok;
}


if (sep[1] == 't') {
*pp = sep + 2;
ch->sep = '\t';
goto ok;
}


{
const char *p = sep + 1;
int base = 8;

if (sep[1] == 'x') {
base = 16;
p++;
} else if (!ch_isdigit(sep[1])) {
(*pp)++;
return AMR_BAD;
}

if (!TryParseChar(&p, base, &ch->sep)) {
Parse_Error(PARSE_FATAL,
"Invalid character number at \"%s\"", p);
return AMR_CLEANUP;
}
if (!IsDelimiter(*p, ch)) {
(*pp)++;
return AMR_BAD;
}

*pp = p;
}

ok:
ModifyWords(ch, ModifyWord_Copy, NULL, ch->oneBigWord);
return AMR_OK;
}

static char *
str_toupper(const char *str)
{
char *res;
size_t i, len;

len = strlen(str);
res = bmake_malloc(len + 1);
for (i = 0; i < len + 1; i++)
res[i] = ch_toupper(str[i]);

return res;
}

static char *
str_tolower(const char *str)
{
char *res;
size_t i, len;

len = strlen(str);
res = bmake_malloc(len + 1);
for (i = 0; i < len + 1; i++)
res[i] = ch_tolower(str[i]);

return res;
}


static ApplyModifierResult
ApplyModifier_To(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
const char *mod = *pp;
assert(mod[0] == 't');

if (IsDelimiter(mod[1], ch) || mod[1] == '\0') {
*pp = mod + 1;
return AMR_BAD;
}

if (mod[1] == 's')
return ApplyModifier_ToSep(pp, ch);

if (!IsDelimiter(mod[2], ch)) {
*pp = mod + 1;
return AMR_BAD;
}

if (mod[1] == 'A') {
*pp = mod + 2;
ModifyWords(ch, ModifyWord_Realpath, NULL, ch->oneBigWord);
return AMR_OK;
}

if (mod[1] == 'u') {
*pp = mod + 2;
if (Expr_ShouldEval(expr))
Expr_SetValueOwn(expr, str_toupper(Expr_Str(expr)));
return AMR_OK;
}

if (mod[1] == 'l') {
*pp = mod + 2;
if (Expr_ShouldEval(expr))
Expr_SetValueOwn(expr, str_tolower(Expr_Str(expr)));
return AMR_OK;
}

if (mod[1] == 'W' || mod[1] == 'w') {
*pp = mod + 2;
ch->oneBigWord = mod[1] == 'W';
return AMR_OK;
}


*pp = mod + 1;
return AMR_BAD;
}


static ApplyModifierResult
ApplyModifier_Words(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
const char *estr;
int first, last;
VarParseResult res;
const char *p;
LazyBuf estrBuf;
FStr festr;

(*pp)++;
res = ParseModifierPart(pp, ']', expr->emode, ch, &estrBuf);
if (res != VPR_OK)
return AMR_CLEANUP;
festr = LazyBuf_DoneGet(&estrBuf);
estr = festr.str;

if (!IsDelimiter(**pp, ch))
goto bad_modifier;

if (!ModChain_ShouldEval(ch))
goto ok;

if (estr[0] == '\0')
goto bad_modifier;

if (estr[0] == '#' && estr[1] == '\0') {
if (ch->oneBigWord) {
Expr_SetValueRefer(expr, "1");
} else {
Buffer buf;

SubstringWords words = Expr_Words(expr);
size_t ac = words.len;
SubstringWords_Free(words);


Buf_InitSize(&buf, 4);
Buf_AddInt(&buf, (int)ac);
Expr_SetValueOwn(expr, Buf_DoneData(&buf));
}
goto ok;
}

if (estr[0] == '*' && estr[1] == '\0') {
ch->oneBigWord = true;
goto ok;
}

if (estr[0] == '@' && estr[1] == '\0') {
ch->oneBigWord = false;
goto ok;
}





p = estr;
if (!TryParseIntBase0(&p, &first))
goto bad_modifier;

if (p[0] == '\0') {
last = first;
} else if (p[0] == '.' && p[1] == '.' && p[2] != '\0') {

p += 2;
if (!TryParseIntBase0(&p, &last) || *p != '\0')
goto bad_modifier;
} else
goto bad_modifier;





if (first == 0 && last == 0) {

ch->oneBigWord = true;
goto ok;
}


if (first == 0 || last == 0)
goto bad_modifier;


Expr_SetValueOwn(expr,
VarSelectWords(Expr_Str(expr), first, last,
ch->sep, ch->oneBigWord));

ok:
FStr_Done(&festr);
return AMR_OK;

bad_modifier:
FStr_Done(&festr);
return AMR_BAD;
}

#if !defined(NUM_TYPE)
#if defined(HAVE_LONG_LONG_INT)
#define NUM_TYPE long long
#elif defined(_INT64_T_DECLARED) || defined(int64_t)
#define NUM_TYPE int64_t
#else
#define NUM_TYPE long
#define strtoll strtol
#endif
#endif

static NUM_TYPE
num_val(Substring s)
{
NUM_TYPE val;
char *ep;

val = strtoll(s.start, &ep, 0);
if (ep != s.start) {
switch (*ep) {
case 'K':
case 'k':
val <<= 10;
break;
case 'M':
case 'm':
val <<= 20;
break;
case 'G':
case 'g':
val <<= 30;
break;
}
}
return val;
}

static int
SubNumAsc(const void *sa, const void *sb)
{
NUM_TYPE a, b;

a = num_val(*((const Substring *)sa));
b = num_val(*((const Substring *)sb));
return (a > b) ? 1 : (b > a) ? -1 : 0;
}

static int
SubNumDesc(const void *sa, const void *sb)
{
return SubNumAsc(sb, sa);
}

static int
SubStrAsc(const void *sa, const void *sb)
{
return strcmp(
((const Substring *)sa)->start, ((const Substring *)sb)->start);
}

static int
SubStrDesc(const void *sa, const void *sb)
{
return SubStrAsc(sb, sa);
}

static void
ShuffleSubstrings(Substring *strs, size_t n)
{
size_t i;

for (i = n - 1; i > 0; i--) {
size_t rndidx = (size_t)random() % (i + 1);
Substring t = strs[i];
strs[i] = strs[rndidx];
strs[rndidx] = t;
}
}








static ApplyModifierResult
ApplyModifier_Order(const char **pp, ModChain *ch)
{
const char *mod = *pp;
SubstringWords words;
int (*cmp)(const void *, const void *);

if (IsDelimiter(mod[1], ch) || mod[1] == '\0') {
cmp = SubStrAsc;
(*pp)++;
} else if (IsDelimiter(mod[2], ch) || mod[2] == '\0') {
if (mod[1] == 'n')
cmp = SubNumAsc;
else if (mod[1] == 'r')
cmp = SubStrDesc;
else if (mod[1] == 'x')
cmp = NULL;
else
goto bad;
*pp += 2;
} else if (IsDelimiter(mod[3], ch) || mod[3] == '\0') {
if ((mod[1] == 'n' && mod[2] == 'r') ||
(mod[1] == 'r' && mod[2] == 'n'))
cmp = SubNumDesc;
else
goto bad;
*pp += 3;
} else {
goto bad;
}

if (!ModChain_ShouldEval(ch))
return AMR_OK;

words = Expr_Words(ch->expr);
if (cmp == NULL)
ShuffleSubstrings(words.words, words.len);
else {
assert(words.words[0].end[0] == '\0');
qsort(words.words, words.len, sizeof(words.words[0]), cmp);
}
Expr_SetValueOwn(ch->expr, SubstringWords_JoinFree(words));

return AMR_OK;

bad:
(*pp)++;
return AMR_BAD;
}


static ApplyModifierResult
ApplyModifier_IfElse(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
VarParseResult res;
LazyBuf buf;
FStr then_expr, else_expr;

bool value = false;
VarEvalMode then_emode = VARE_PARSE_ONLY;
VarEvalMode else_emode = VARE_PARSE_ONLY;

CondEvalResult cond_rc = COND_PARSE;
if (Expr_ShouldEval(expr)) {
cond_rc = Cond_EvalCondition(expr->name, &value);
if (cond_rc != COND_INVALID && value)
then_emode = expr->emode;
if (cond_rc != COND_INVALID && !value)
else_emode = expr->emode;
}

(*pp)++;
res = ParseModifierPart(pp, ':', then_emode, ch, &buf);
if (res != VPR_OK)
return AMR_CLEANUP;
then_expr = LazyBuf_DoneGet(&buf);

res = ParseModifierPart(pp, ch->endc, else_emode, ch, &buf);
if (res != VPR_OK) {
FStr_Done(&then_expr);
return AMR_CLEANUP;
}
else_expr = LazyBuf_DoneGet(&buf);

(*pp)--;

if (cond_rc == COND_INVALID) {
Error("Bad conditional expression '%s' in '%s?%s:%s'",
expr->name, expr->name, then_expr.str, else_expr.str);
FStr_Done(&then_expr);
FStr_Done(&else_expr);
return AMR_CLEANUP;
}

if (!Expr_ShouldEval(expr)) {
FStr_Done(&then_expr);
FStr_Done(&else_expr);
} else if (value) {
Expr_SetValue(expr, then_expr);
FStr_Done(&else_expr);
} else {
FStr_Done(&then_expr);
Expr_SetValue(expr, else_expr);
}
Expr_Define(expr);
return AMR_OK;
}























static ApplyModifierResult
ApplyModifier_Assign(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
GNode *scope;
FStr val;
VarParseResult res;
LazyBuf buf;

const char *mod = *pp;
const char *op = mod + 1;

if (op[0] == '=')
goto ok;
if ((op[0] == '!' || op[0] == '+' || op[0] == '?') && op[1] == '=')
goto ok;
return AMR_UNKNOWN;

ok:
if (expr->name[0] == '\0') {
*pp = mod + 1;
return AMR_BAD;
}

switch (op[0]) {
case '+':
case '?':
case '!':
*pp = mod + 3;
break;
default:
*pp = mod + 2;
break;
}

res = ParseModifierPart(pp, ch->endc, expr->emode, ch, &buf);
if (res != VPR_OK)
return AMR_CLEANUP;
val = LazyBuf_DoneGet(&buf);

(*pp)--;

if (!Expr_ShouldEval(expr))
goto done;

scope = expr->scope;
if (expr->defined == DEF_REGULAR && expr->scope != SCOPE_GLOBAL) {
Var *gv = VarFind(expr->name, expr->scope, false);
if (gv == NULL)
scope = SCOPE_GLOBAL;
else
VarFreeEnv(gv);
}

switch (op[0]) {
case '+':
Var_Append(scope, expr->name, val.str);
break;
case '!': {
const char *errfmt;
char *cmd_output = Cmd_Exec(val.str, &errfmt);
if (errfmt != NULL)
Error(errfmt, val.str);
else
Var_Set(scope, expr->name, cmd_output);
free(cmd_output);
break;
}
case '?':
if (expr->defined == DEF_REGULAR)
break;

default:
Var_Set(scope, expr->name, val.str);
break;
}
Expr_SetValueRefer(expr, "");

done:
FStr_Done(&val);
return AMR_OK;
}





static ApplyModifierResult
ApplyModifier_Remember(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
const char *mod = *pp;
FStr name;

if (!ModMatchEq(mod, "_", ch))
return AMR_UNKNOWN;

name = FStr_InitRefer("_");
if (mod[1] == '=') {





const char *arg = mod + 2;
size_t argLen = strcspn(arg, ":)}");
*pp = arg + argLen;
name = FStr_InitOwn(bmake_strldup(arg, argLen));
} else
*pp = mod + 1;

if (Expr_ShouldEval(expr))
Var_Set(expr->scope, name.str, Expr_Str(expr));
FStr_Done(&name);

return AMR_OK;
}





static ApplyModifierResult
ApplyModifier_WordFunc(const char **pp, ModChain *ch,
ModifyWordProc modifyWord)
{
if (!IsDelimiter((*pp)[1], ch))
return AMR_UNKNOWN;
(*pp)++;

if (ModChain_ShouldEval(ch))
ModifyWords(ch, modifyWord, NULL, ch->oneBigWord);

return AMR_OK;
}


static ApplyModifierResult
ApplyModifier_Unique(const char **pp, ModChain *ch)
{
SubstringWords words;

if (!IsDelimiter((*pp)[1], ch))
return AMR_UNKNOWN;
(*pp)++;

if (!ModChain_ShouldEval(ch))
return AMR_OK;

words = Expr_Words(ch->expr);

if (words.len > 1) {
size_t si, di;

di = 0;
for (si = 1; si < words.len; si++) {
if (!Substring_Eq(words.words[si], words.words[di])) {
di++;
if (di != si)
words.words[di] = words.words[si];
}
}
words.len = di + 1;
}

Expr_SetValueOwn(ch->expr, SubstringWords_JoinFree(words));

return AMR_OK;
}

#if defined(SYSVVARSUB)

static ApplyModifierResult
ApplyModifier_SysV(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
VarParseResult res;
LazyBuf lhsBuf, rhsBuf;
FStr rhs;
struct ModifyWord_SysVSubstArgs args;
Substring lhs;
const char *lhsSuffix;

const char *mod = *pp;
bool eqFound = false;





int depth = 1;
const char *p = mod;
while (*p != '\0' && depth > 0) {
if (*p == '=') {
eqFound = true;

} else if (*p == ch->endc)
depth--;
else if (*p == ch->startc)
depth++;
if (depth > 0)
p++;
}
if (*p != ch->endc || !eqFound)
return AMR_UNKNOWN;

res = ParseModifierPart(pp, '=', expr->emode, ch, &lhsBuf);
if (res != VPR_OK)
return AMR_CLEANUP;


res = ParseModifierPart(pp, ch->endc, expr->emode, ch, &rhsBuf);
if (res != VPR_OK) {
LazyBuf_Done(&lhsBuf);
return AMR_CLEANUP;
}
rhs = LazyBuf_DoneGet(&rhsBuf);

(*pp)--;


if (lhsBuf.len == 0 && Expr_Str(expr)[0] == '\0')
goto done;

lhs = LazyBuf_Get(&lhsBuf);
lhsSuffix = Substring_SkipFirst(lhs, '%');

args.scope = expr->scope;
args.lhsPrefix = Substring_Init(lhs.start,
lhsSuffix != lhs.start ? lhsSuffix - 1 : lhs.start);
args.lhsPercent = lhsSuffix != lhs.start;
args.lhsSuffix = Substring_Init(lhsSuffix, lhs.end);
args.rhs = rhs.str;

ModifyWords(ch, ModifyWord_SysVSubst, &args, ch->oneBigWord);

done:
LazyBuf_Done(&lhsBuf);
return AMR_OK;
}
#endif

#if defined(SUNSHCMD)

static ApplyModifierResult
ApplyModifier_SunShell(const char **pp, ModChain *ch)
{
Expr *expr = ch->expr;
const char *p = *pp;
if (!(p[1] == 'h' && IsDelimiter(p[2], ch)))
return AMR_UNKNOWN;
*pp = p + 2;

if (Expr_ShouldEval(expr)) {
const char *errfmt;
char *output = Cmd_Exec(Expr_Str(expr), &errfmt);
if (errfmt != NULL)
Error(errfmt, Expr_Str(expr));
Expr_SetValueOwn(expr, output);
}

return AMR_OK;
}
#endif

static void
LogBeforeApply(const ModChain *ch, const char *mod)
{
const Expr *expr = ch->expr;
bool is_single_char = mod[0] != '\0' && IsDelimiter(mod[1], ch);






if (!Expr_ShouldEval(expr)) {
debug_printf("Parsing modifier ${%s:%c%s}\n",
expr->name, mod[0], is_single_char ? "" : "...");
return;
}

if ((expr->emode == VARE_WANTRES || expr->emode == VARE_UNDEFERR) &&
expr->defined == DEF_REGULAR) {
debug_printf(
"Evaluating modifier ${%s:%c%s} on value \"%s\"\n",
expr->name, mod[0], is_single_char ? "" : "...",
Expr_Str(expr));
return;
}

debug_printf(
"Evaluating modifier ${%s:%c%s} on value \"%s\" (%s, %s)\n",
expr->name, mod[0], is_single_char ? "" : "...", Expr_Str(expr),
VarEvalMode_Name[expr->emode], ExprDefined_Name[expr->defined]);
}

static void
LogAfterApply(const ModChain *ch, const char *p, const char *mod)
{
const Expr *expr = ch->expr;
const char *value = Expr_Str(expr);
const char *quot = value == var_Error ? "" : "\"";

if ((expr->emode == VARE_WANTRES || expr->emode == VARE_UNDEFERR) &&
expr->defined == DEF_REGULAR) {

debug_printf("Result of ${%s:%.*s} is %s%s%s\n",
expr->name, (int)(p - mod), mod,
quot, value == var_Error ? "error" : value, quot);
return;
}

debug_printf("Result of ${%s:%.*s} is %s%s%s (%s, %s)\n",
expr->name, (int)(p - mod), mod,
quot, value == var_Error ? "error" : value, quot,
VarEvalMode_Name[expr->emode],
ExprDefined_Name[expr->defined]);
}

static ApplyModifierResult
ApplyModifier(const char **pp, ModChain *ch)
{
switch (**pp) {
case '!':
return ApplyModifier_ShellCommand(pp, ch);
case ':':
return ApplyModifier_Assign(pp, ch);
case '?':
return ApplyModifier_IfElse(pp, ch);
case '@':
return ApplyModifier_Loop(pp, ch);
case '[':
return ApplyModifier_Words(pp, ch);
case '_':
return ApplyModifier_Remember(pp, ch);
#if !defined(NO_REGEX)
case 'C':
return ApplyModifier_Regex(pp, ch);
#endif
case 'D':
case 'U':
return ApplyModifier_Defined(pp, ch);
case 'E':
return ApplyModifier_WordFunc(pp, ch, ModifyWord_Suffix);
case 'g':
return ApplyModifier_Gmtime(pp, ch);
case 'H':
return ApplyModifier_WordFunc(pp, ch, ModifyWord_Head);
case 'h':
return ApplyModifier_Hash(pp, ch);
case 'L':
return ApplyModifier_Literal(pp, ch);
case 'l':
return ApplyModifier_Localtime(pp, ch);
case 'M':
case 'N':
return ApplyModifier_Match(pp, ch);
case 'O':
return ApplyModifier_Order(pp, ch);
case 'P':
return ApplyModifier_Path(pp, ch);
case 'Q':
case 'q':
return ApplyModifier_Quote(pp, ch);
case 'R':
return ApplyModifier_WordFunc(pp, ch, ModifyWord_Root);
case 'r':
return ApplyModifier_Range(pp, ch);
case 'S':
return ApplyModifier_Subst(pp, ch);
#if defined(SUNSHCMD)
case 's':
return ApplyModifier_SunShell(pp, ch);
#endif
case 'T':
return ApplyModifier_WordFunc(pp, ch, ModifyWord_Tail);
case 't':
return ApplyModifier_To(pp, ch);
case 'u':
return ApplyModifier_Unique(pp, ch);
default:
return AMR_UNKNOWN;
}
}

static void ApplyModifiers(Expr *, const char **, char, char);

typedef enum ApplyModifiersIndirectResult {

AMIR_CONTINUE,

AMIR_SYSV,

AMIR_OUT
} ApplyModifiersIndirectResult;














static ApplyModifiersIndirectResult
ApplyModifiersIndirect(ModChain *ch, const char **pp)
{
Expr *expr = ch->expr;
const char *p = *pp;
FStr mods;

(void)Var_Parse(&p, expr->scope, expr->emode, &mods);


if (mods.str[0] != '\0' && *p != '\0' && !IsDelimiter(*p, ch)) {
FStr_Done(&mods);
return AMIR_SYSV;
}

DEBUG3(VAR, "Indirect modifier \"%s\" from \"%.*s\"\n",
mods.str, (int)(p - *pp), *pp);

if (mods.str[0] != '\0') {
const char *modsp = mods.str;
ApplyModifiers(expr, &modsp, '\0', '\0');
if (Expr_Str(expr) == var_Error || *modsp != '\0') {
FStr_Done(&mods);
*pp = p;
return AMIR_OUT;
}
}
FStr_Done(&mods);

if (*p == ':')
p++;
else if (*p == '\0' && ch->endc != '\0') {
Error("Unclosed variable expression after indirect "
"modifier, expecting '%c' for variable \"%s\"",
ch->endc, expr->name);
*pp = p;
return AMIR_OUT;
}

*pp = p;
return AMIR_CONTINUE;
}

static ApplyModifierResult
ApplySingleModifier(const char **pp, ModChain *ch)
{
ApplyModifierResult res;
const char *mod = *pp;
const char *p = *pp;

if (DEBUG(VAR))
LogBeforeApply(ch, mod);

res = ApplyModifier(&p, ch);

#if defined(SYSVVARSUB)
if (res == AMR_UNKNOWN) {
assert(p == mod);
res = ApplyModifier_SysV(&p, ch);
}
#endif

if (res == AMR_UNKNOWN) {






for (p++; !IsDelimiter(*p, ch) && *p != '\0'; p++)
continue;
Parse_Error(PARSE_FATAL, "Unknown modifier \"%.*s\"",
(int)(p - mod), mod);
Expr_SetValueRefer(ch->expr, var_Error);
}
if (res == AMR_CLEANUP || res == AMR_BAD) {
*pp = p;
return res;
}

if (DEBUG(VAR))
LogAfterApply(ch, p, mod);

if (*p == '\0' && ch->endc != '\0') {
Error(
"Unclosed variable expression, expecting '%c' for "
"modifier \"%.*s\" of variable \"%s\" with value \"%s\"",
ch->endc,
(int)(p - mod), mod,
ch->expr->name, Expr_Str(ch->expr));
} else if (*p == ':') {
p++;
} else if (opts.strict && *p != '\0' && *p != ch->endc) {
Parse_Error(PARSE_FATAL,
"Missing delimiter ':' after modifier \"%.*s\"",
(int)(p - mod), mod);




}
*pp = p;
return AMR_OK;
}

#if __STDC_VERSION__ >= 199901L
#define ModChain_Literal(expr, startc, endc, sep, oneBigWord) (ModChain) { expr, startc, endc, sep, oneBigWord }

#else
MAKE_INLINE ModChain
ModChain_Literal(Expr *expr, char startc, char endc, char sep, bool oneBigWord)
{
ModChain ch;
ch.expr = expr;
ch.startc = startc;
ch.endc = endc;
ch.sep = sep;
ch.oneBigWord = oneBigWord;
return ch;
}
#endif


static void
ApplyModifiers(
Expr *expr,
const char **pp,
char startc,
char endc
)
{
ModChain ch = ModChain_Literal(expr, startc, endc, ' ', false);
const char *p;
const char *mod;

assert(startc == '(' || startc == '{' || startc == '\0');
assert(endc == ')' || endc == '}' || endc == '\0');
assert(Expr_Str(expr) != NULL);

p = *pp;

if (*p == '\0' && endc != '\0') {
Error(
"Unclosed variable expression (expecting '%c') for \"%s\"",
ch.endc, expr->name);
goto cleanup;
}

while (*p != '\0' && *p != endc) {
ApplyModifierResult res;

if (*p == '$') {
ApplyModifiersIndirectResult amir =
ApplyModifiersIndirect(&ch, &p);
if (amir == AMIR_CONTINUE)
continue;
if (amir == AMIR_OUT)
break;





}

mod = p;

res = ApplySingleModifier(&p, &ch);
if (res == AMR_CLEANUP)
goto cleanup;
if (res == AMR_BAD)
goto bad_modifier;
}

*pp = p;
assert(Expr_Str(expr) != NULL);
return;

bad_modifier:

Error("Bad modifier \":%.*s\" for variable \"%s\"",
(int)strcspn(mod, ":)}"), mod, expr->name);

cleanup:










*pp = p;
Expr_SetValueRefer(expr, var_Error);
}





static bool
VarnameIsDynamic(Substring varname)
{
const char *name;
size_t len;

name = varname.start;
len = Substring_Length(varname);
if (len == 1 || (len == 2 && (name[1] == 'F' || name[1] == 'D'))) {
switch (name[0]) {
case '@':
case '%':
case '*':
case '!':
return true;
}
return false;
}

if ((len == 7 || len == 8) && name[0] == '.' && ch_isupper(name[1])) {
return Substring_Equals(varname, ".TARGET") ||
Substring_Equals(varname, ".ARCHIVE") ||
Substring_Equals(varname, ".PREFIX") ||
Substring_Equals(varname, ".MEMBER");
}

return false;
}

static const char *
UndefinedShortVarValue(char varname, const GNode *scope)
{
if (scope == SCOPE_CMDLINE || scope == SCOPE_GLOBAL) {









switch (varname) {
case '@':
return "$(.TARGET)";
case '%':
return "$(.MEMBER)";
case '*':
return "$(.PREFIX)";
case '!':
return "$(.ARCHIVE)";
}
}
return NULL;
}





static void
ParseVarname(const char **pp, char startc, char endc,
GNode *scope, VarEvalMode emode,
LazyBuf *buf)
{
const char *p = *pp;
int depth = 0;

LazyBuf_Init(buf, p);

while (*p != '\0') {
if ((*p == endc || *p == ':') && depth == 0)
break;
if (*p == startc)
depth++;
if (*p == endc)
depth--;


if (*p == '$') {
FStr nested_val;
(void)Var_Parse(&p, scope, emode, &nested_val);

LazyBuf_AddStr(buf, nested_val.str);
FStr_Done(&nested_val);
} else {
LazyBuf_Add(buf, *p);
p++;
}
}
*pp = p;
}

static VarParseResult
ValidShortVarname(char varname, const char *start)
{
if (varname != '$' && varname != ':' && varname != '}' &&
varname != ')' && varname != '\0')
return VPR_OK;

if (!opts.strict)
return VPR_ERR;

if (varname == '$')
Parse_Error(PARSE_FATAL,
"To escape a dollar, use \\$, not $$, at \"%s\"", start);
else if (varname == '\0')
Parse_Error(PARSE_FATAL, "Dollar followed by nothing");
else
Parse_Error(PARSE_FATAL,
"Invalid variable name '%c', at \"%s\"", varname, start);

return VPR_ERR;
}





static bool
ParseVarnameShort(char varname, const char **pp, GNode *scope,
VarEvalMode emode,
VarParseResult *out_false_res, const char **out_false_val,
Var **out_true_var)
{
char name[2];
Var *v;
VarParseResult vpr;

vpr = ValidShortVarname(varname, *pp);
if (vpr != VPR_OK) {
(*pp)++;
*out_false_res = vpr;
*out_false_val = var_Error;
return false;
}

name[0] = varname;
name[1] = '\0';
v = VarFind(name, scope, true);
if (v == NULL) {
const char *val;
*pp += 2;

val = UndefinedShortVarValue(varname, scope);
if (val == NULL)
val = emode == VARE_UNDEFERR
? var_Error : varUndefined;

if (opts.strict && val == var_Error) {
Parse_Error(PARSE_FATAL,
"Variable \"%s\" is undefined", name);
*out_false_res = VPR_ERR;
*out_false_val = val;
return false;
}











*out_false_res = emode == VARE_UNDEFERR
? VPR_UNDEF : VPR_OK;
*out_false_val = val;
return false;
}

*out_true_var = v;
return true;
}


static Var *
FindLocalLegacyVar(Substring varname, GNode *scope,
const char **out_extraModifiers)
{
Var *v;


if (scope == SCOPE_CMDLINE || scope == SCOPE_GLOBAL)
return NULL;

if (Substring_Length(varname) != 2)
return NULL;
if (varname.start[1] != 'F' && varname.start[1] != 'D')
return NULL;
if (strchr("@%?*!<>", varname.start[0]) == NULL)
return NULL;

v = VarFindSubstring(Substring_Sub(varname, 0, 1), scope, false);
if (v == NULL)
return NULL;

*out_extraModifiers = varname.start[1] == 'D' ? "H:" : "T:";
return v;
}

static VarParseResult
EvalUndefined(bool dynamic, const char *start, const char *p,
Substring varname, VarEvalMode emode, FStr *out_val)
{
if (dynamic) {
*out_val = FStr_InitOwn(bmake_strsedup(start, p));
return VPR_OK;
}

if (emode == VARE_UNDEFERR && opts.strict) {
Parse_Error(PARSE_FATAL,
"Variable \"%.*s\" is undefined",
(int)Substring_Length(varname), varname.start);
*out_val = FStr_InitRefer(var_Error);
return VPR_ERR;
}

if (emode == VARE_UNDEFERR) {
*out_val = FStr_InitRefer(var_Error);
return VPR_UNDEF;
}

*out_val = FStr_InitRefer(varUndefined);
return VPR_OK;
}







static bool
ParseVarnameLong(
const char **pp,
char startc,
GNode *scope,
VarEvalMode emode,

const char **out_false_pp,
VarParseResult *out_false_res,
FStr *out_false_val,

char *out_true_endc,
Var **out_true_v,
bool *out_true_haveModifier,
const char **out_true_extraModifiers,
bool *out_true_dynamic,
ExprDefined *out_true_exprDefined
)
{
LazyBuf varname;
Substring name;
Var *v;
bool haveModifier;
bool dynamic = false;

const char *p = *pp;
const char *const start = p;
char endc = startc == '(' ? ')' : '}';

p += 2;
ParseVarname(&p, startc, endc, scope, emode, &varname);
name = LazyBuf_Get(&varname);

if (*p == ':') {
haveModifier = true;
} else if (*p == endc) {
haveModifier = false;
} else {
Parse_Error(PARSE_FATAL, "Unclosed variable \"%.*s\"",
(int)Substring_Length(name), name.start);
LazyBuf_Done(&varname);
*out_false_pp = p;
*out_false_val = FStr_InitRefer(var_Error);
*out_false_res = VPR_ERR;
return false;
}

v = VarFindSubstring(name, scope, true);




if (v == NULL) {
if (Substring_Equals(name, ".SUFFIXES"))
v = VarNew(Substring_Str(name),
Suff_NamesStr(), false, true);
else
v = FindLocalLegacyVar(name, scope,
out_true_extraModifiers);
}

if (v == NULL) {




dynamic = VarnameIsDynamic(name) &&
(scope == SCOPE_CMDLINE || scope == SCOPE_GLOBAL);

if (!haveModifier) {
p++;
*out_false_pp = p;
*out_false_res = EvalUndefined(dynamic, start, p,
name, emode, out_false_val);
return false;
}















v = VarNew(LazyBuf_DoneGet(&varname), "", false, false);
*out_true_exprDefined = DEF_UNDEF;
} else
LazyBuf_Done(&varname);

*pp = p;
*out_true_endc = endc;
*out_true_v = v;
*out_true_haveModifier = haveModifier;
*out_true_dynamic = dynamic;
return true;
}


static void
FreeEnvVar(Var *v, Expr *expr)
{
char *varValue = Buf_DoneData(&v->val);
if (expr->value.str == varValue)
expr->value.freeIt = varValue;
else
free(varValue);

FStr_Done(&v->name);
free(v);
}

#if __STDC_VERSION__ >= 199901L
#define Expr_Literal(name, value, emode, scope, defined) { name, value, emode, scope, defined }

#else
MAKE_INLINE Expr
Expr_Literal(const char *name, FStr value,
VarEvalMode emode, GNode *scope, ExprDefined defined)
{
Expr expr;

expr.name = name;
expr.value = value;
expr.emode = emode;
expr.scope = scope;
expr.defined = defined;
return expr;
}
#endif






static bool
Var_Parse_FastLane(const char **pp, VarEvalMode emode, FStr *out_value)
{
const char *p;

p = *pp;
if (!(p[0] == '$' && p[1] == '{' && p[2] == ':' && p[3] == 'U'))
return false;

p += 4;
while (*p != '$' && *p != '{' && *p != ':' && *p != '\\' &&
*p != '}' && *p != '\0')
p++;
if (*p != '}')
return false;

if (emode == VARE_PARSE_ONLY)
*out_value = FStr_InitRefer("");
else
*out_value = FStr_InitOwn(bmake_strsedup(*pp + 4, p));
*pp = p + 1;
return true;
}






































VarParseResult
Var_Parse(const char **pp, GNode *scope, VarEvalMode emode, FStr *out_val)
{
const char *p = *pp;
const char *const start = p;

bool haveModifier;

char startc;

char endc;





bool dynamic;
const char *extramodifiers;
Var *v;
Expr expr = Expr_Literal(NULL, FStr_InitRefer(NULL), emode,
scope, DEF_REGULAR);

if (Var_Parse_FastLane(pp, emode, out_val))
return VPR_OK;

DEBUG2(VAR, "Var_Parse: %s (%s)\n", start, VarEvalMode_Name[emode]);

*out_val = FStr_InitRefer(NULL);
extramodifiers = NULL;
dynamic = false;





endc = '\0';

startc = p[1];
if (startc != '(' && startc != '{') {
VarParseResult res;
if (!ParseVarnameShort(startc, pp, scope, emode, &res,
&out_val->str, &v))
return res;
haveModifier = false;
p++;
} else {
VarParseResult res;
if (!ParseVarnameLong(&p, startc, scope, emode,
pp, &res, out_val,
&endc, &v, &haveModifier, &extramodifiers,
&dynamic, &expr.defined))
return res;
}

expr.name = v->name.str;
if (v->inUse)
Fatal("Variable %s is recursive.", v->name.str);










expr.value = FStr_InitRefer(v->val.data);





if (strchr(Expr_Str(&expr), '$') != NULL &&
VarEvalMode_ShouldEval(emode)) {
char *expanded;
VarEvalMode nested_emode = emode;
if (opts.strict)
nested_emode = VarEvalMode_UndefOk(nested_emode);
v->inUse = true;
(void)Var_Subst(Expr_Str(&expr), scope, nested_emode,
&expanded);
v->inUse = false;

Expr_SetValueOwn(&expr, expanded);
}

if (extramodifiers != NULL) {
const char *em = extramodifiers;
ApplyModifiers(&expr, &em, '\0', '\0');
}

if (haveModifier) {
p++;
ApplyModifiers(&expr, &p, startc, endc);
}

if (*p != '\0')
p++;

*pp = p;

if (v->fromEnv) {
FreeEnvVar(v, &expr);

} else if (expr.defined != DEF_REGULAR) {
if (expr.defined == DEF_UNDEF) {
if (dynamic) {
Expr_SetValueOwn(&expr,
bmake_strsedup(start, p));
} else {





Expr_SetValueRefer(&expr,
emode == VARE_UNDEFERR
? var_Error : varUndefined);
}
}

if (expr.value.str != v->val.data)
Buf_Done(&v->val);
FStr_Done(&v->name);
free(v);
}
*out_val = expr.value;
return VPR_OK;
}

static void
VarSubstDollarDollar(const char **pp, Buffer *res, VarEvalMode emode)
{

if (save_dollars && VarEvalMode_ShouldKeepDollar(emode))
Buf_AddByte(res, '$');
Buf_AddByte(res, '$');
*pp += 2;
}

static void
VarSubstExpr(const char **pp, Buffer *buf, GNode *scope,
VarEvalMode emode, bool *inout_errorReported)
{
const char *p = *pp;
const char *nested_p = p;
FStr val;

(void)Var_Parse(&nested_p, scope, emode, &val);


if (val.str == var_Error || val.str == varUndefined) {
if (!VarEvalMode_ShouldKeepUndef(emode)) {
p = nested_p;
} else if (emode == VARE_UNDEFERR || val.str == var_Error) {













if (!*inout_errorReported) {
Parse_Error(PARSE_FATAL,
"Undefined variable \"%.*s\"",
(int)(size_t)(nested_p - p), p);
}
p = nested_p;
*inout_errorReported = true;
} else {




Buf_AddByte(buf, *p);
p++;
}
} else {
p = nested_p;
Buf_AddStr(buf, val.str);
}

FStr_Done(&val);

*pp = p;
}





static void
VarSubstPlain(const char **pp, Buffer *res)
{
const char *p = *pp;
const char *start = p;

for (p++; *p != '$' && *p != '\0'; p++)
continue;
Buf_AddBytesBetween(res, start, p);
*pp = p;
}












VarParseResult
Var_Subst(const char *str, GNode *scope, VarEvalMode emode, char **out_res)
{
const char *p = str;
Buffer res;




static bool errorReported;

Buf_Init(&res);
errorReported = false;

while (*p != '\0') {
if (p[0] == '$' && p[1] == '$')
VarSubstDollarDollar(&p, &res, emode);
else if (p[0] == '$')
VarSubstExpr(&p, &res, scope, emode, &errorReported);
else
VarSubstPlain(&p, &res);
}

*out_res = Buf_DoneDataCompact(&res);
return VPR_OK;
}


void
Var_Init(void)
{
SCOPE_INTERNAL = GNode_New("Internal");
SCOPE_GLOBAL = GNode_New("Global");
SCOPE_CMDLINE = GNode_New("Command");
}


void
Var_End(void)
{
Var_Stats();
}

void
Var_Stats(void)
{
HashTable_DebugStats(&SCOPE_GLOBAL->vars, "Global variables");
}

static int
StrAsc(const void *sa, const void *sb)
{
return strcmp(
*((const char *const *)sa), *((const char *const *)sb));
}



void
Var_Dump(GNode *scope)
{
Vector vec;
HashIter hi;
size_t i;
const char **varnames;

Vector_Init(&vec, sizeof(const char *));

HashIter_Init(&hi, &scope->vars);
while (HashIter_Next(&hi) != NULL)
*(const char **)Vector_Push(&vec) = hi.entry->key;
varnames = vec.items;

qsort(varnames, vec.len, sizeof varnames[0], StrAsc);

for (i = 0; i < vec.len; i++) {
const char *varname = varnames[i];
Var *var = HashTable_FindValue(&scope->vars, varname);
debug_printf("%-16s = %s\n", varname, var->val.data);
}

Vector_Done(&vec);
}
