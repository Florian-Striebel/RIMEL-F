

























































































#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/stat.h>
#if defined(MAKE_NATIVE) && defined(HAVE_SYSCTL)
#include <sys/sysctl.h>
#endif
#include <sys/utsname.h>
#include "wait.h"

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>

#include "make.h"
#include "dir.h"
#include "job.h"
#include "pathnames.h"
#include "trace.h"


MAKE_RCSID("$NetBSD: main.c,v 1.541 2021/08/14 13:32:12 rillig Exp $");
#if defined(MAKE_NATIVE) && !defined(lint)
__COPYRIGHT("@(#) Copyright (c) 1988, 1989, 1990, 1993 "
"The Regents of the University of California. "
"All rights reserved.");
#endif

#if !defined(__arraycount)
#define __arraycount(__x) (sizeof(__x) / sizeof(__x[0]))
#endif

CmdOpts opts;
time_t now;
GNode *defaultNode;
bool allPrecious;
bool deleteOnError;

static int maxJobTokens;
bool enterFlagObj;

static int jp_0 = -1, jp_1 = -1;
bool doing_depend;
static bool jobsRunning;
static const char *tracefile;
static int ReadMakefile(const char *);
static void purge_relative_cached_realpaths(void);

static bool ignorePWD;
static char objdir[MAXPATHLEN + 1];
char curdir[MAXPATHLEN + 1];
const char *progname;
char *makeDependfile;
pid_t myPid;
int makelevel;

bool forceJobs = false;
static int main_errors = 0;
static HashTable cached_realpaths;





static char *
explode(const char *flags)
{
size_t len;
char *nf, *st;
const char *f;

if (flags == NULL)
return NULL;

for (f = flags; *f != '\0'; f++)
if (!ch_isalpha(*f))
break;

if (*f != '\0')
return bmake_strdup(flags);

len = strlen(flags);
st = nf = bmake_malloc(len * 3 + 1);
while (*flags != '\0') {
*nf++ = '-';
*nf++ = *flags++;
*nf++ = ' ';
}
*nf = '\0';
return st;
}





MAKE_ATTR_DEAD static void
usage(void)
{
size_t prognameLen = strcspn(progname, "[");

(void)fprintf(stderr,
"usage: %.*s [-BeikNnqrSstWwX]\n"
" [-C directory] [-D variable] [-d flags] [-f makefile]\n"
" [-I directory] [-J private] [-j max_jobs] [-m directory] [-T file]\n"
" [-V variable] [-v variable] [variable=value] [target ...]\n",
(int)prognameLen, progname);
exit(2);
}

static void
MainParseArgDebugFile(const char *arg)
{
const char *mode;
size_t len;
char *fname;

if (opts.debug_file != stdout && opts.debug_file != stderr)
fclose(opts.debug_file);

if (*arg == '+') {
arg++;
mode = "a";
} else
mode = "w";

if (strcmp(arg, "stdout") == 0) {
opts.debug_file = stdout;
return;
}
if (strcmp(arg, "stderr") == 0) {
opts.debug_file = stderr;
return;
}

len = strlen(arg);
fname = bmake_malloc(len + 20);
memcpy(fname, arg, len + 1);


if (strcmp(fname + len - 3, ".%d") == 0)
snprintf(fname + len - 2, 20, "%d", getpid());

opts.debug_file = fopen(fname, mode);
if (opts.debug_file == NULL) {
fprintf(stderr, "Cannot open debug file %s\n",
fname);
usage();
}
free(fname);
}

static void
MainParseArgDebug(const char *argvalue)
{
const char *modules;
DebugFlags debug = opts.debug;

for (modules = argvalue; *modules != '\0'; modules++) {
switch (*modules) {
case '0':
debug = DEBUG_NONE;
break;
case 'A':
debug = DEBUG_ALL;
break;
case 'a':
debug |= DEBUG_ARCH;
break;
case 'C':
debug |= DEBUG_CWD;
break;
case 'c':
debug |= DEBUG_COND;
break;
case 'd':
debug |= DEBUG_DIR;
break;
case 'e':
debug |= DEBUG_ERROR;
break;
case 'f':
debug |= DEBUG_FOR;
break;
case 'g':
if (modules[1] == '1') {
debug |= DEBUG_GRAPH1;
modules++;
} else if (modules[1] == '2') {
debug |= DEBUG_GRAPH2;
modules++;
} else if (modules[1] == '3') {
debug |= DEBUG_GRAPH3;
modules++;
}
break;
case 'h':
debug |= DEBUG_HASH;
break;
case 'j':
debug |= DEBUG_JOB;
break;
case 'L':
opts.strict = true;
break;
case 'l':
debug |= DEBUG_LOUD;
break;
case 'M':
debug |= DEBUG_META;
break;
case 'm':
debug |= DEBUG_MAKE;
break;
case 'n':
debug |= DEBUG_SCRIPT;
break;
case 'p':
debug |= DEBUG_PARSE;
break;
case 's':
debug |= DEBUG_SUFF;
break;
case 't':
debug |= DEBUG_TARG;
break;
case 'V':
opts.debugVflag = true;
break;
case 'v':
debug |= DEBUG_VAR;
break;
case 'x':
debug |= DEBUG_SHELL;
break;
case 'F':
MainParseArgDebugFile(modules + 1);
goto debug_setbuf;
default:
(void)fprintf(stderr,
"%s: illegal argument to d option -- %c\n",
progname, *modules);
usage();
}
}

debug_setbuf:
opts.debug = debug;





setvbuf(opts.debug_file, NULL, _IONBF, 0);
if (opts.debug_file != stdout) {
setvbuf(stdout, NULL, _IOLBF, 0);
}
}


static bool
IsRelativePath(const char *path)
{
const char *p;

if (path[0] != '/')
return true;
p = path;
while ((p = strstr(p, "/.")) != NULL) {
p += 2;
if (*p == '.')
p++;
if (*p == '/' || *p == '\0')
return true;
}
return false;
}

static void
MainParseArgChdir(const char *argvalue)
{
struct stat sa, sb;

if (chdir(argvalue) == -1) {
(void)fprintf(stderr, "%s: chdir %s: %s\n",
progname, argvalue, strerror(errno));
exit(2);
}
if (getcwd(curdir, MAXPATHLEN) == NULL) {
(void)fprintf(stderr, "%s: %s.\n", progname, strerror(errno));
exit(2);
}
if (!IsRelativePath(argvalue) &&
stat(argvalue, &sa) != -1 &&
stat(curdir, &sb) != -1 &&
sa.st_ino == sb.st_ino &&
sa.st_dev == sb.st_dev)
strncpy(curdir, argvalue, MAXPATHLEN);
ignorePWD = true;
}

static void
MainParseArgJobsInternal(const char *argvalue)
{
char end;
if (sscanf(argvalue, "%d,%d%c", &jp_0, &jp_1, &end) != 2) {
(void)fprintf(stderr,
"%s: internal error -- J option malformed (%s)\n",
progname, argvalue);
usage();
}
if ((fcntl(jp_0, F_GETFD, 0) < 0) ||
(fcntl(jp_1, F_GETFD, 0) < 0)) {
#if 0
(void)fprintf(stderr,
"%s: ######warning -- J descriptors were closed!\n",
progname);
exit(2);
#endif
jp_0 = -1;
jp_1 = -1;
opts.compatMake = true;
} else {
Global_Append(MAKEFLAGS, "-J");
Global_Append(MAKEFLAGS, argvalue);
}
}

static void
MainParseArgJobs(const char *argvalue)
{
char *p;

forceJobs = true;
opts.maxJobs = (int)strtol(argvalue, &p, 0);
if (*p != '\0' || opts.maxJobs < 1) {
(void)fprintf(stderr,
"%s: illegal argument to -j -- must be positive integer!\n",
progname);
exit(2);
}
Global_Append(MAKEFLAGS, "-j");
Global_Append(MAKEFLAGS, argvalue);
Global_Set(".MAKE.JOBS", argvalue);
maxJobTokens = opts.maxJobs;
}

static void
MainParseArgSysInc(const char *argvalue)
{

if (strncmp(".../", argvalue, 4) == 0) {
char *found_path = Dir_FindHereOrAbove(curdir, argvalue + 4);
if (found_path == NULL)
return;
(void)SearchPath_Add(sysIncPath, found_path);
free(found_path);
} else {
(void)SearchPath_Add(sysIncPath, argvalue);
}
Global_Append(MAKEFLAGS, "-m");
Global_Append(MAKEFLAGS, argvalue);
}

static bool
MainParseArg(char c, const char *argvalue)
{
switch (c) {
case '\0':
break;
case 'B':
opts.compatMake = true;
Global_Append(MAKEFLAGS, "-B");
Global_Set(MAKE_MODE, "compat");
break;
case 'C':
MainParseArgChdir(argvalue);
break;
case 'D':
if (argvalue[0] == '\0') return false;
Global_SetExpand(argvalue, "1");
Global_Append(MAKEFLAGS, "-D");
Global_Append(MAKEFLAGS, argvalue);
break;
case 'I':
Parse_AddIncludeDir(argvalue);
Global_Append(MAKEFLAGS, "-I");
Global_Append(MAKEFLAGS, argvalue);
break;
case 'J':
MainParseArgJobsInternal(argvalue);
break;
case 'N':
opts.noExecute = true;
opts.noRecursiveExecute = true;
Global_Append(MAKEFLAGS, "-N");
break;
case 'S':
opts.keepgoing = false;
Global_Append(MAKEFLAGS, "-S");
break;
case 'T':
tracefile = bmake_strdup(argvalue);
Global_Append(MAKEFLAGS, "-T");
Global_Append(MAKEFLAGS, argvalue);
break;
case 'V':
case 'v':
opts.printVars = c == 'v' ? PVM_EXPANDED : PVM_UNEXPANDED;
Lst_Append(&opts.variables, bmake_strdup(argvalue));

Global_Append(MAKEFLAGS, "-V");
Global_Append(MAKEFLAGS, argvalue);
break;
case 'W':
opts.parseWarnFatal = true;

break;
case 'X':
opts.varNoExportEnv = true;
Global_Append(MAKEFLAGS, "-X");
break;
case 'd':

if (argvalue[0] == '-')
argvalue++;
else {
Global_Append(MAKEFLAGS, "-d");
Global_Append(MAKEFLAGS, argvalue);
}
MainParseArgDebug(argvalue);
break;
case 'e':
opts.checkEnvFirst = true;
Global_Append(MAKEFLAGS, "-e");
break;
case 'f':
Lst_Append(&opts.makefiles, bmake_strdup(argvalue));
break;
case 'i':
opts.ignoreErrors = true;
Global_Append(MAKEFLAGS, "-i");
break;
case 'j':
MainParseArgJobs(argvalue);
break;
case 'k':
opts.keepgoing = true;
Global_Append(MAKEFLAGS, "-k");
break;
case 'm':
MainParseArgSysInc(argvalue);

break;
case 'n':
opts.noExecute = true;
Global_Append(MAKEFLAGS, "-n");
break;
case 'q':
opts.queryFlag = true;

Global_Append(MAKEFLAGS, "-q");
break;
case 'r':
opts.noBuiltins = true;
Global_Append(MAKEFLAGS, "-r");
break;
case 's':
opts.beSilent = true;
Global_Append(MAKEFLAGS, "-s");
break;
case 't':
opts.touchFlag = true;
Global_Append(MAKEFLAGS, "-t");
break;
case 'w':
opts.enterFlag = true;
Global_Append(MAKEFLAGS, "-w");
break;
default:
case '?':
usage();
}
return true;
}










static void
MainParseArgs(int argc, char **argv)
{
char c;
int arginc;
char *argvalue;
char *optscan;
bool inOption, dashDash = false;

const char *optspecs = "BC:D:I:J:NST:V:WXd:ef:ij:km:nqrstv:w";


rearg:
inOption = false;
optscan = NULL;
while (argc > 1) {
const char *optspec;
if (!inOption)
optscan = argv[1];
c = *optscan++;
arginc = 0;
if (inOption) {
if (c == '\0') {
argv++;
argc--;
inOption = false;
continue;
}
} else {
if (c != '-' || dashDash)
break;
inOption = true;
c = *optscan++;
}

optspec = strchr(optspecs, c);
if (c != '\0' && optspec != NULL && optspec[1] == ':') {

inOption = false;
arginc = 1;
argvalue = optscan;
if (*argvalue == '\0') {
if (argc < 3)
goto noarg;
argvalue = argv[2];
arginc = 2;
}
} else {
argvalue = NULL;
}
switch (c) {
case '\0':
arginc = 1;
inOption = false;
break;
case '-':
dashDash = true;
break;
default:
if (!MainParseArg(c, argvalue))
goto noarg;
}
argv += arginc;
argc -= arginc;
}






for (; argc > 1; argv++, argc--) {
VarAssign var;
if (Parse_IsVar(argv[1], &var)) {
Parse_Var(&var, SCOPE_CMDLINE);
} else {
if (argv[1][0] == '\0')
Punt("illegal (null) argument.");
if (argv[1][0] == '-' && !dashDash)
goto rearg;
Lst_Append(&opts.create, bmake_strdup(argv[1]));
}
}

return;
noarg:
(void)fprintf(stderr, "%s: option requires an argument -- %c\n",
progname, c);
usage();
}







void
Main_ParseArgLine(const char *line)
{
Words words;
char *buf;

if (line == NULL)
return;

for (; *line == ' '; line++)
continue;
if (line[0] == '\0')
return;

#if !defined(POSIX)
{



char *cp;

if (!(cp = strrchr(line, '/')))
cp = line;
if ((cp = strstr(cp, "make")) &&
strcmp(cp, "make") == 0)
return;
}
#endif
{
FStr argv0 = Var_Value(SCOPE_GLOBAL, ".MAKE");
buf = str_concat3(argv0.str, " ", line);
FStr_Done(&argv0);
}

words = Str_Words(buf, true);
if (words.words == NULL) {
Error("Unterminated quoted string [%s]", buf);
free(buf);
return;
}
free(buf);
MainParseArgs((int)words.len, words.words);

Words_Free(words);
}

bool
Main_SetObjdir(bool writable, const char *fmt, ...)
{
struct stat sb;
char *path;
char buf[MAXPATHLEN + 1];
char buf2[MAXPATHLEN + 1];
bool rc = false;
va_list ap;

va_start(ap, fmt);
vsnprintf(path = buf, MAXPATHLEN, fmt, ap);
va_end(ap);

if (path[0] != '/') {
snprintf(buf2, MAXPATHLEN, "%s/%s", curdir, path);
path = buf2;
}


if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
if ((writable && access(path, W_OK) != 0) ||
(chdir(path) != 0)) {
(void)fprintf(stderr, "%s warning: %s: %s.\n",
progname, path, strerror(errno));
} else {
snprintf(objdir, sizeof objdir, "%s", path);
Global_Set(".OBJDIR", objdir);
setenv("PWD", objdir, 1);
Dir_InitDot();
purge_relative_cached_realpaths();
rc = true;
if (opts.enterFlag && strcmp(objdir, curdir) != 0)
enterFlagObj = true;
}
}

return rc;
}

static bool
SetVarObjdir(bool writable, const char *var, const char *suffix)
{
FStr path = Var_Value(SCOPE_CMDLINE, var);
FStr xpath;

if (path.str == NULL || path.str[0] == '\0') {
FStr_Done(&path);
return false;
}


xpath = FStr_InitRefer(path.str);
if (strchr(path.str, '$') != 0) {
char *expanded;
(void)Var_Subst(path.str, SCOPE_GLOBAL, VARE_WANTRES, &expanded);

xpath = FStr_InitOwn(expanded);
}

(void)Main_SetObjdir(writable, "%s%s", xpath.str, suffix);

FStr_Done(&xpath);
FStr_Done(&path);
return true;
}





int
str2Lst_Append(StringList *lp, char *str)
{
char *cp;
int n;

const char *sep = " \t";

for (n = 0, cp = strtok(str, sep); cp != NULL; cp = strtok(NULL, sep)) {
Lst_Append(lp, cp);
n++;
}
return n;
}

#if defined(SIGINFO)

static void
siginfo(int signo MAKE_ATTR_UNUSED)
{
char dir[MAXPATHLEN];
char str[2 * MAXPATHLEN];
int len;
if (getcwd(dir, sizeof dir) == NULL)
return;
len = snprintf(str, sizeof str, "%s: Working in: %s\n", progname, dir);
if (len > 0)
(void)write(STDERR_FILENO, str, (size_t)len);
}
#endif


static void
MakeMode(void)
{
char *mode;

(void)Var_Subst("${" MAKE_MODE ":tl}", SCOPE_GLOBAL, VARE_WANTRES, &mode);


if (mode[0] != '\0') {
if (strstr(mode, "compat") != NULL) {
opts.compatMake = true;
forceJobs = false;
}
#if USE_META
if (strstr(mode, "meta") != NULL)
meta_mode_init(mode);
#endif
}

free(mode);
}

static void
PrintVar(const char *varname, bool expandVars)
{
if (strchr(varname, '$') != NULL) {
char *evalue;
(void)Var_Subst(varname, SCOPE_GLOBAL, VARE_WANTRES, &evalue);

printf("%s\n", evalue);
bmake_free(evalue);

} else if (expandVars) {
char *expr = str_concat3("${", varname, "}");
char *evalue;
(void)Var_Subst(expr, SCOPE_GLOBAL, VARE_WANTRES, &evalue);

free(expr);
printf("%s\n", evalue);
bmake_free(evalue);

} else {
FStr value = Var_Value(SCOPE_GLOBAL, varname);
printf("%s\n", value.str != NULL ? value.str : "");
FStr_Done(&value);
}
}








bool
GetBooleanExpr(const char *expr, bool fallback)
{
char *value;
bool res;

(void)Var_Subst(expr, SCOPE_GLOBAL, VARE_WANTRES, &value);

res = ParseBoolean(value, fallback);
free(value);
return res;
}

static void
doPrintVars(void)
{
StringListNode *ln;
bool expandVars;

if (opts.printVars == PVM_EXPANDED)
expandVars = true;
else if (opts.debugVflag)
expandVars = false;
else
expandVars = GetBooleanExpr("${.MAKE.EXPAND_VARIABLES}",
false);

for (ln = opts.variables.first; ln != NULL; ln = ln->next) {
const char *varname = ln->datum;
PrintVar(varname, expandVars);
}
}

static bool
runTargets(void)
{
GNodeList targs = LST_INIT;
bool outOfDate;







if (Lst_IsEmpty(&opts.create))
Parse_MainName(&targs);
else
Targ_FindList(&targs, &opts.create);

if (!opts.compatMake) {







if (!opts.queryFlag) {
Job_Init();
jobsRunning = true;
}


outOfDate = Make_Run(&targs);
} else {




Compat_Run(&targs);
outOfDate = false;
}
Lst_Done(&targs);
return outOfDate;
}






static void
InitVarTargets(void)
{
StringListNode *ln;

if (Lst_IsEmpty(&opts.create)) {
Global_Set(".TARGETS", "");
return;
}

for (ln = opts.create.first; ln != NULL; ln = ln->next) {
const char *name = ln->datum;
Global_Append(".TARGETS", name);
}
}

static void
InitRandom(void)
{
struct timeval tv;

gettimeofday(&tv, NULL);
srandom((unsigned int)(tv.tv_sec + tv.tv_usec));
}

static const char *
InitVarMachine(const struct utsname *utsname MAKE_ATTR_UNUSED)
{
#if defined(FORCE_MACHINE)
return FORCE_MACHINE;
#else
const char *machine = getenv("MACHINE");

if (machine != NULL)
return machine;

#if defined(MAKE_NATIVE)
return utsname->machine;
#elif defined(MAKE_MACHINE)
return MAKE_MACHINE;
#else
return "unknown";
#endif
#endif
}

static const char *
InitVarMachineArch(void)
{
#if defined(FORCE_MACHINE_ARCH)
return FORCE_MACHINE_ARCH;
#else
const char *env = getenv("MACHINE_ARCH");
if (env != NULL)
return env;

#if defined(MAKE_NATIVE) && defined(CTL_HW)
{
struct utsname utsname;
static char machine_arch_buf[sizeof utsname.machine];
const int mib[2] = { CTL_HW, HW_MACHINE_ARCH };
size_t len = sizeof machine_arch_buf;

if (sysctl(mib, (unsigned int)__arraycount(mib),
machine_arch_buf, &len, NULL, 0) < 0) {
(void)fprintf(stderr, "%s: sysctl failed (%s).\n",
progname, strerror(errno));
exit(2);
}

return machine_arch_buf;
}
#elif defined(MACHINE_ARCH)
return MACHINE_ARCH;
#elif defined(MAKE_MACHINE_ARCH)
return MAKE_MACHINE_ARCH;
#else
return "unknown";
#endif
#endif
}

#if !defined(NO_PWD_OVERRIDE)
















static void
HandlePWD(const struct stat *curdir_st)
{
char *pwd;
FStr prefix, makeobjdir;
struct stat pwd_st;

if (ignorePWD || (pwd = getenv("PWD")) == NULL)
return;

prefix = Var_Value(SCOPE_CMDLINE, "MAKEOBJDIRPREFIX");
if (prefix.str != NULL) {
FStr_Done(&prefix);
return;
}

makeobjdir = Var_Value(SCOPE_CMDLINE, "MAKEOBJDIR");
if (makeobjdir.str != NULL && strchr(makeobjdir.str, '$') != NULL)
goto ignore_pwd;

if (stat(pwd, &pwd_st) == 0 &&
curdir_st->st_ino == pwd_st.st_ino &&
curdir_st->st_dev == pwd_st.st_dev)
(void)strncpy(curdir, pwd, MAXPATHLEN);

ignore_pwd:
FStr_Done(&makeobjdir);
}
#endif










static void
InitObjdir(const char *machine, const char *machine_arch)
{
bool writable;

Dir_InitCur(curdir);
writable = GetBooleanExpr("${MAKE_OBJDIR_CHECK_WRITABLE}", true);
(void)Main_SetObjdir(false, "%s", curdir);

if (!SetVarObjdir(writable, "MAKEOBJDIRPREFIX", curdir) &&
!SetVarObjdir(writable, "MAKEOBJDIR", "") &&
!Main_SetObjdir(writable, "%s.%s-%s", _PATH_OBJDIR, machine, machine_arch) &&
!Main_SetObjdir(writable, "%s.%s", _PATH_OBJDIR, machine) &&
!Main_SetObjdir(writable, "%s", _PATH_OBJDIR))
(void)Main_SetObjdir(writable, "%s%s", _PATH_OBJDIRPREFIX, curdir);
}


static void
UnlimitFiles(void)
{
#if defined(HAVE_SETRLIMIT) && defined(RLIMIT_NOFILE)
struct rlimit rl;
if (getrlimit(RLIMIT_NOFILE, &rl) != -1 &&
rl.rlim_cur != rl.rlim_max) {
rl.rlim_cur = rl.rlim_max;
(void)setrlimit(RLIMIT_NOFILE, &rl);
}
#endif
}

static void
CmdOpts_Init(void)
{
opts.compatMake = false;
opts.debug = DEBUG_NONE;

opts.strict = false;
opts.debugVflag = false;
opts.checkEnvFirst = false;
Lst_Init(&opts.makefiles);
opts.ignoreErrors = false;
opts.maxJobs = 1;
opts.keepgoing = false;
opts.noRecursiveExecute = false;
opts.noExecute = false;
opts.queryFlag = false;
opts.noBuiltins = false;
opts.beSilent = false;
opts.touchFlag = false;
opts.printVars = PVM_NONE;
Lst_Init(&opts.variables);
opts.parseWarnFatal = false;
opts.enterFlag = false;
opts.varNoExportEnv = false;
Lst_Init(&opts.create);
}








static void
InitVarMake(const char *argv0)
{
const char *make = argv0;

if (argv0[0] != '/' && strchr(argv0, '/') != NULL) {
char pathbuf[MAXPATHLEN];
const char *abspath = cached_realpath(argv0, pathbuf);
struct stat st;
if (abspath != NULL && abspath[0] == '/' &&
stat(make, &st) == 0)
make = abspath;
}

Global_Set("MAKE", make);
Global_Set(".MAKE", make);
}





static void
InitDefSysIncPath(char *syspath)
{
static char defsyspath[] = _PATH_DEFSYSPATH;
char *start, *cp;






if (syspath == NULL || syspath[0] == '\0')
syspath = defsyspath;
else
syspath = bmake_strdup(syspath);

for (start = syspath; *start != '\0'; start = cp) {
for (cp = start; *cp != '\0' && *cp != ':'; cp++)
continue;
if (*cp == ':')
*cp++ = '\0';


if (strncmp(start, ".../", 4) == 0) {
char *dir = Dir_FindHereOrAbove(curdir, start + 4);
if (dir != NULL) {
(void)SearchPath_Add(defSysIncPath, dir);
free(dir);
}
} else {
(void)SearchPath_Add(defSysIncPath, start);
}
}

if (syspath != defsyspath)
free(syspath);
}

static void
ReadBuiltinRules(void)
{
StringListNode *ln;
StringList sysMkFiles = LST_INIT;

SearchPath_Expand(
Lst_IsEmpty(&sysIncPath->dirs) ? defSysIncPath : sysIncPath,
_PATH_DEFSYSMK,
&sysMkFiles);
if (Lst_IsEmpty(&sysMkFiles))
Fatal("%s: no system rules (%s).", progname, _PATH_DEFSYSMK);

for (ln = sysMkFiles.first; ln != NULL; ln = ln->next)
if (ReadMakefile(ln->datum) == 0)
break;

if (ln == NULL)
Fatal("%s: cannot open %s.",
progname, (const char *)sysMkFiles.first->datum);



Lst_Done(&sysMkFiles);
}

static void
InitMaxJobs(void)
{
char *value;
int n;

if (forceJobs || opts.compatMake ||
!Var_Exists(SCOPE_GLOBAL, ".MAKE.JOBS"))
return;

(void)Var_Subst("${.MAKE.JOBS}", SCOPE_GLOBAL, VARE_WANTRES, &value);

n = (int)strtol(value, NULL, 0);
if (n < 1) {
(void)fprintf(stderr,
"%s: illegal value for .MAKE.JOBS "
"-- must be positive integer!\n",
progname);
exit(2);
}

if (n != opts.maxJobs) {
Global_Append(MAKEFLAGS, "-j");
Global_Append(MAKEFLAGS, value);
}

opts.maxJobs = n;
maxJobTokens = opts.maxJobs;
forceJobs = true;
free(value);
}







static void
InitVpath(void)
{
char *vpath, savec, *path;
if (!Var_Exists(SCOPE_CMDLINE, "VPATH"))
return;

(void)Var_Subst("${VPATH}", SCOPE_CMDLINE, VARE_WANTRES, &vpath);

path = vpath;
do {
char *cp;

for (cp = path; *cp != ':' && *cp != '\0'; cp++)
continue;

savec = *cp;
*cp = '\0';

(void)SearchPath_Add(&dirSearchPath, path);
*cp = savec;
path = cp + 1;
} while (savec == ':');
free(vpath);
}

static void
ReadAllMakefiles(StringList *makefiles)
{
StringListNode *ln;

for (ln = makefiles->first; ln != NULL; ln = ln->next) {
const char *fname = ln->datum;
if (ReadMakefile(fname) != 0)
Fatal("%s: cannot open %s.", progname, fname);
}
}

static void
ReadFirstDefaultMakefile(void)
{
StringListNode *ln;
char *prefs;

(void)Var_Subst("${" MAKE_MAKEFILE_PREFERENCE "}",
SCOPE_CMDLINE, VARE_WANTRES, &prefs);






(void)str2Lst_Append(&opts.makefiles, prefs);

for (ln = opts.makefiles.first; ln != NULL; ln = ln->next)
if (ReadMakefile(ln->datum) == 0)
break;

free(prefs);
}






static void
main_Init(int argc, char **argv)
{
struct stat sa;
const char *machine;
const char *machine_arch;
char *syspath = getenv("MAKESYSPATH");
struct utsname utsname;


opts.debug_file = stderr;

HashTable_Init(&cached_realpaths);

#if defined(SIGINFO)
(void)bmake_signal(SIGINFO, siginfo);
#endif

InitRandom();

progname = str_basename(argv[0]);

UnlimitFiles();

if (uname(&utsname) == -1) {
(void)fprintf(stderr, "%s: uname failed (%s).\n", progname,
strerror(errno));
exit(2);
}









machine = InitVarMachine(&utsname);
machine_arch = InitVarMachineArch();

myPid = getpid();




Targ_Init();
Var_Init();
Global_Set(".MAKE.OS", utsname.sysname);
Global_Set("MACHINE", machine);
Global_Set("MACHINE_ARCH", machine_arch);
#if defined(MAKE_VERSION)
Global_Set("MAKE_VERSION", MAKE_VERSION);
#endif
Global_Set(".newline", "\n");



#if !defined(MAKEFILE_PREFERENCE_LIST)
#define MAKEFILE_PREFERENCE_LIST "makefile Makefile"
#endif
Global_Set(MAKE_MAKEFILE_PREFERENCE, MAKEFILE_PREFERENCE_LIST);
Global_Set(MAKE_DEPENDFILE, ".depend");

CmdOpts_Init();
allPrecious = false;
deleteOnError = false;
jobsRunning = false;

maxJobTokens = opts.maxJobs;
ignorePWD = false;













Parse_Init();
InitVarMake(argv[0]);
Global_Set(MAKEFLAGS, "");
Global_Set(MAKEOVERRIDES, "");
Global_Set("MFLAGS", "");
Global_Set(".ALLTARGETS", "");

Var_Set(SCOPE_CMDLINE, MAKE_LEVEL ".ENV", MAKE_LEVEL_ENV);


{
char tmp[64], *ep = getenv(MAKE_LEVEL_ENV);

makelevel = ep != NULL && ep[0] != '\0' ? atoi(ep) : 0;
if (makelevel < 0)
makelevel = 0;
snprintf(tmp, sizeof tmp, "%d", makelevel);
Global_Set(MAKE_LEVEL, tmp);
snprintf(tmp, sizeof tmp, "%u", myPid);
Global_Set(".MAKE.PID", tmp);
snprintf(tmp, sizeof tmp, "%u", getppid());
Global_Set(".MAKE.PPID", tmp);
snprintf(tmp, sizeof tmp, "%u", getuid());
Global_Set(".MAKE.UID", tmp);
snprintf(tmp, sizeof tmp, "%u", getgid());
Global_Set(".MAKE.GID", tmp);
}
if (makelevel > 0) {
char pn[1024];
snprintf(pn, sizeof pn, "%s[%d]", progname, makelevel);
progname = bmake_strdup(pn);
}

#if defined(USE_META)
meta_init();
#endif
Dir_Init();






#if defined(POSIX)
{
char *p1 = explode(getenv("MAKEFLAGS"));
Main_ParseArgLine(p1);
free(p1);
}
#else
Main_ParseArgLine(getenv("MAKE"));
#endif





if (getcwd(curdir, MAXPATHLEN) == NULL) {
(void)fprintf(stderr, "%s: getcwd: %s.\n",
progname, strerror(errno));
exit(2);
}

MainParseArgs(argc, argv);

if (opts.enterFlag)
printf("%s: Entering directory `%s'\n", progname, curdir);




if (stat(curdir, &sa) == -1) {
(void)fprintf(stderr, "%s: %s: %s.\n",
progname, curdir, strerror(errno));
exit(2);
}

#if !defined(NO_PWD_OVERRIDE)
HandlePWD(&sa);
#endif
Global_Set(".CURDIR", curdir);

InitObjdir(machine, machine_arch);





Arch_Init();
Suff_Init();
Trace_Init(tracefile);

defaultNode = NULL;
(void)time(&now);

Trace_Log(MAKESTART, NULL);

InitVarTargets();

InitDefSysIncPath(syspath);
}





static void
main_ReadFiles(void)
{

if (!opts.noBuiltins)
ReadBuiltinRules();

if (!Lst_IsEmpty(&opts.makefiles))
ReadAllMakefiles(&opts.makefiles);
else
ReadFirstDefaultMakefile();
}


static void
main_PrepareMaking(void)
{

if (!opts.noBuiltins || opts.printVars == PVM_NONE) {
(void)Var_Subst("${.MAKE.DEPENDFILE}",
SCOPE_CMDLINE, VARE_WANTRES, &makeDependfile);
if (makeDependfile[0] != '\0') {

doing_depend = true;
(void)ReadMakefile(makeDependfile);
doing_depend = false;
}
}

if (enterFlagObj)
printf("%s: Entering directory `%s'\n", progname, objdir);

MakeMode();

{
FStr makeflags = Var_Value(SCOPE_GLOBAL, MAKEFLAGS);
Global_Append("MFLAGS", makeflags.str);
FStr_Done(&makeflags);
}

InitMaxJobs();





if (!opts.compatMake && !forceJobs)
opts.compatMake = true;

if (!opts.compatMake)
Job_ServerStart(maxJobTokens, jp_0, jp_1);
DEBUG5(JOB, "job_pipe %d %d, maxjobs %d, tokens %d, compat %d\n",
jp_0, jp_1, opts.maxJobs, maxJobTokens, opts.compatMake ? 1 : 0);

if (opts.printVars == PVM_NONE)
Main_ExportMAKEFLAGS(true);

InitVpath();





Suff_ExtendPaths();




Targ_Propagate();


if (DEBUG(GRAPH1))
Targ_PrintGraph(1);
}






static bool
main_Run(void)
{
if (opts.printVars != PVM_NONE) {

doPrintVars();
return false;
} else {
return runTargets();
}
}


static void
main_CleanUp(void)
{
#if defined(CLEANUP)
Lst_DoneCall(&opts.variables, free);




Lst_Done(&opts.makefiles);
Lst_DoneCall(&opts.create, free);
#endif


if (DEBUG(GRAPH2))
Targ_PrintGraph(2);

Trace_Log(MAKEEND, NULL);

if (enterFlagObj)
printf("%s: Leaving directory `%s'\n", progname, objdir);
if (opts.enterFlag)
printf("%s: Leaving directory `%s'\n", progname, curdir);

#if defined(USE_META)
meta_finish();
#endif
Suff_End();
Targ_End();
Arch_End();
Var_End();
Parse_End();
Dir_End();
Job_End();
Trace_End();
}


static int
main_Exit(bool outOfDate)
{
if (opts.strict && (main_errors > 0 || Parse_NumErrors() > 0))
return 2;
return outOfDate ? 1 : 0;
}

int
main(int argc, char **argv)
{
bool outOfDate;

main_Init(argc, argv);
main_ReadFiles();
main_PrepareMaking();
outOfDate = main_Run();
main_CleanUp();
return main_Exit(outOfDate);
}







static int
ReadMakefile(const char *fname)
{
int fd;
char *name, *path = NULL;

if (strcmp(fname, "-") == 0) {
Parse_File(NULL , -1);
Var_Set(SCOPE_INTERNAL, "MAKEFILE", "");
} else {

if (strcmp(curdir, objdir) != 0 && *fname != '/') {
path = str_concat3(curdir, "/", fname);
fd = open(path, O_RDONLY);
if (fd != -1) {
fname = path;
goto found;
}
free(path);


path = str_concat3(objdir, "/", fname);
fd = open(path, O_RDONLY);
if (fd != -1) {
fname = path;
goto found;
}
} else {
fd = open(fname, O_RDONLY);
if (fd != -1)
goto found;
}

name = Dir_FindFile(fname, parseIncPath);
if (name == NULL) {
SearchPath *sysInc = Lst_IsEmpty(&sysIncPath->dirs)
? defSysIncPath : sysIncPath;
name = Dir_FindFile(fname, sysInc);
}
if (name == NULL || (fd = open(name, O_RDONLY)) == -1) {
free(name);
free(path);
return -1;
}
fname = name;





found:
if (!doing_depend)
Var_Set(SCOPE_INTERNAL, "MAKEFILE", fname);
Parse_File(fname, fd);
}
free(path);
return 0;
}














char *
Cmd_Exec(const char *cmd, const char **errfmt)
{
const char *args[4];
int pipefds[2];
int cpid;
int pid;
int status;
Buffer buf;
ssize_t bytes_read;
char *res;
size_t res_len;
char *cp;
int savederr;

*errfmt = NULL;

if (shellName == NULL)
Shell_Init();



args[0] = shellName;
args[1] = "-c";
args[2] = cmd;
args[3] = NULL;




if (pipe(pipefds) == -1) {
*errfmt = "Couldn't create pipe for \"%s\"";
goto bad;
}

Var_ReexportVars();




switch (cpid = vfork()) {
case 0:
(void)close(pipefds[0]);






(void)dup2(pipefds[1], 1);
(void)close(pipefds[1]);

(void)execv(shellPath, UNCONST(args));
_exit(1);


case -1:
*errfmt = "Couldn't exec \"%s\"";
goto bad;

default:
(void)close(pipefds[1]);

savederr = 0;
Buf_Init(&buf);

do {
char result[BUFSIZ];
bytes_read = read(pipefds[0], result, sizeof result);
if (bytes_read > 0)
Buf_AddBytes(&buf, result, (size_t)bytes_read);
} while (bytes_read > 0 ||
(bytes_read == -1 && errno == EINTR));
if (bytes_read == -1)
savederr = errno;

(void)close(pipefds[0]);


while ((pid = waitpid(cpid, &status, 0)) != cpid && pid >= 0)
JobReapChild(pid, status, false);

res_len = buf.len;
res = Buf_DoneData(&buf);

if (savederr != 0)
*errfmt = "Couldn't read shell's output for \"%s\"";

if (WIFSIGNALED(status))
*errfmt = "\"%s\" exited on a signal";
else if (WEXITSTATUS(status) != 0)
*errfmt = "\"%s\" returned non-zero status";


if (res_len > 0 && res[res_len - 1] == '\n')
res[res_len - 1] = '\0';
for (cp = res; *cp != '\0'; cp++)
if (*cp == '\n')
*cp = ' ';
break;
}
return res;
bad:
return bmake_strdup("");
}







void
Error(const char *fmt, ...)
{
va_list ap;
FILE *err_file;

err_file = opts.debug_file;
if (err_file == stdout)
err_file = stderr;
(void)fflush(stdout);
for (;;) {
va_start(ap, fmt);
fprintf(err_file, "%s: ", progname);
(void)vfprintf(err_file, fmt, ap);
va_end(ap);
(void)fprintf(err_file, "\n");
(void)fflush(err_file);
if (err_file == stderr)
break;
err_file = stderr;
}
main_errors++;
}








void
Fatal(const char *fmt, ...)
{
va_list ap;

if (jobsRunning)
Job_Wait();

(void)fflush(stdout);
va_start(ap, fmt);
(void)vfprintf(stderr, fmt, ap);
va_end(ap);
(void)fprintf(stderr, "\n");
(void)fflush(stderr);

PrintOnError(NULL, NULL);

if (DEBUG(GRAPH2) || DEBUG(GRAPH3))
Targ_PrintGraph(2);
Trace_Log(MAKEERROR, NULL);
exit(2);
}





void
Punt(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
(void)fflush(stdout);
(void)fprintf(stderr, "%s: ", progname);
(void)vfprintf(stderr, fmt, ap);
va_end(ap);
(void)fprintf(stderr, "\n");
(void)fflush(stderr);

PrintOnError(NULL, NULL);

DieHorribly();
}


void
DieHorribly(void)
{
if (jobsRunning)
Job_AbortAll();
if (DEBUG(GRAPH2))
Targ_PrintGraph(2);
Trace_Log(MAKEERROR, NULL);
exit(2);
}






void
Finish(int errs)
{
if (shouldDieQuietly(NULL, -1))
exit(2);
Fatal("%d error%s", errs, errs == 1 ? "" : "s");
}





int
eunlink(const char *file)
{
struct stat st;

if (lstat(file, &st) == -1)
return -1;

if (S_ISDIR(st.st_mode)) {
errno = EISDIR;
return -1;
}
return unlink(file);
}

static void
write_all(int fd, const void *data, size_t n)
{
const char *mem = data;

while (n > 0) {
ssize_t written = write(fd, mem, n);
if (written == -1 && errno == EAGAIN)
continue;
if (written == -1)
break;
mem += written;
n -= (size_t)written;
}
}





void MAKE_ATTR_DEAD
execDie(const char *af, const char *av)
{
Buffer buf;

Buf_Init(&buf);
Buf_AddStr(&buf, progname);
Buf_AddStr(&buf, ": ");
Buf_AddStr(&buf, af);
Buf_AddStr(&buf, "(");
Buf_AddStr(&buf, av);
Buf_AddStr(&buf, ") failed (");
Buf_AddStr(&buf, strerror(errno));
Buf_AddStr(&buf, ")\n");

write_all(STDERR_FILENO, buf.data, buf.len);

Buf_Done(&buf);
_exit(1);
}


static void
purge_relative_cached_realpaths(void)
{
HashEntry *he, *nhe;
HashIter hi;

HashIter_Init(&hi, &cached_realpaths);
he = HashIter_Next(&hi);
while (he != NULL) {
nhe = HashIter_Next(&hi);
if (he->key[0] != '/') {
DEBUG1(DIR, "cached_realpath: purging %s\n", he->key);
HashTable_DeleteEntry(&cached_realpaths, he);


}
he = nhe;
}
}

char *
cached_realpath(const char *pathname, char *resolved)
{
const char *rp;

if (pathname == NULL || pathname[0] == '\0')
return NULL;

rp = HashTable_FindValue(&cached_realpaths, pathname);
if (rp != NULL) {

strncpy(resolved, rp, MAXPATHLEN);
resolved[MAXPATHLEN - 1] = '\0';
return resolved;
}

rp = realpath(pathname, resolved);
if (rp != NULL) {
HashTable_Set(&cached_realpaths, pathname, bmake_strdup(rp));
DEBUG2(DIR, "cached_realpath: %s -> %s\n", pathname, rp);
return resolved;
}


return NULL;
}





bool
shouldDieQuietly(GNode *gn, int bf)
{
static int quietly = -1;

if (quietly < 0) {
if (DEBUG(JOB) ||
!GetBooleanExpr("${.MAKE.DIE_QUIETLY}", true))
quietly = 0;
else if (bf >= 0)
quietly = bf;
else
quietly = (gn != NULL && (gn->type & OP_MAKE)) ? 1 : 0;
}
return quietly != 0;
}

static void
SetErrorVars(GNode *gn)
{
StringListNode *ln;




Global_Set(".ERROR_TARGET", gn->name);
Global_Delete(".ERROR_CMD");

for (ln = gn->commands.first; ln != NULL; ln = ln->next) {
const char *cmd = ln->datum;

if (cmd == NULL)
break;
Global_Append(".ERROR_CMD", cmd);
}
}





void
PrintOnError(GNode *gn, const char *msg)
{
static GNode *errorNode = NULL;

if (DEBUG(HASH)) {
Targ_Stats();
Var_Stats();
}

if (errorNode != NULL)
return;

if (msg != NULL)
printf("%s", msg);
printf("\n%s: stopped in %s\n", progname, curdir);


if (shouldDieQuietly(gn, -1))
return;

if (gn != NULL)
SetErrorVars(gn);

{
char *errorVarsValues;
(void)Var_Subst("${MAKE_PRINT_VAR_ON_ERROR:@v@$v='${$v}'\n@}",
SCOPE_GLOBAL, VARE_WANTRES, &errorVarsValues);

printf("%s", errorVarsValues);
free(errorVarsValues);
}

fflush(stdout);




errorNode = Targ_FindNode(".ERROR");
if (errorNode != NULL) {
errorNode->type |= OP_SPECIAL;
Compat_Make(errorNode, errorNode);
}
}

void
Main_ExportMAKEFLAGS(bool first)
{
static bool once = true;
const char *expr;
char *s;

if (once != first)
return;
once = false;

expr = "${.MAKEFLAGS} ${.MAKEOVERRIDES:O:u:@v@$v=${$v:Q}@}";
(void)Var_Subst(expr, SCOPE_CMDLINE, VARE_WANTRES, &s);

if (s[0] != '\0') {
#if defined(POSIX)
setenv("MAKEFLAGS", s, 1);
#else
setenv("MAKE", s, 1);
#endif
}
}

char *
getTmpdir(void)
{
static char *tmpdir = NULL;
struct stat st;

if (tmpdir != NULL)
return tmpdir;


(void)Var_Subst("${TMPDIR:tA:U" _PATH_TMP ":S,/$,,W}/",
SCOPE_GLOBAL, VARE_WANTRES, &tmpdir);


if (stat(tmpdir, &st) < 0 || !S_ISDIR(st.st_mode)) {
free(tmpdir);
tmpdir = bmake_strdup(_PATH_TMP);
}
return tmpdir;
}






int
mkTempFile(const char *pattern, char *tfile, size_t tfile_sz)
{
static char *tmpdir = NULL;
char tbuf[MAXPATHLEN];
int fd;

if (pattern == NULL)
pattern = TMPPAT;
if (tmpdir == NULL)
tmpdir = getTmpdir();
if (tfile == NULL) {
tfile = tbuf;
tfile_sz = sizeof tbuf;
}
if (pattern[0] == '/') {
snprintf(tfile, tfile_sz, "%s", pattern);
} else {
snprintf(tfile, tfile_sz, "%s%s", tmpdir, pattern);
}
if ((fd = mkstemp(tfile)) < 0)
Punt("Could not create temporary file %s: %s", tfile,
strerror(errno));
if (tfile == tbuf) {
unlink(tfile);
}
return fd;
}






bool
ParseBoolean(const char *s, bool fallback)
{
char ch = ch_tolower(s[0]);
if (ch == '\0')
return fallback;
if (ch == '0' || ch == 'f' || ch == 'n')
return false;
if (ch == 'o')
return ch_tolower(s[1]) != 'f';
return true;
}
