














































































#if !defined(MAKE_MAKE_H)
#define MAKE_MAKE_H

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(HAVE_STRING_H)
#include <string.h>
#else
#include <strings.h>
#endif
#include <unistd.h>
#include <sys/cdefs.h>

#if !defined(FD_CLOEXEC)
#define FD_CLOEXEC 1
#endif

#if defined(__GNUC__)
#define MAKE_GNUC_PREREQ(x, y) ((__GNUC__ == (x) && __GNUC_MINOR__ >= (y)) || (__GNUC__ > (x)))


#else
#define MAKE_GNUC_PREREQ(x, y) 0
#endif

#if MAKE_GNUC_PREREQ(2, 7)
#define MAKE_ATTR_UNUSED __attribute__((__unused__))
#else
#define MAKE_ATTR_UNUSED
#endif

#if MAKE_GNUC_PREREQ(2, 5)
#define MAKE_ATTR_DEAD __attribute__((__noreturn__))
#elif defined(__GNUC__)
#define MAKE_ATTR_DEAD __volatile
#else
#define MAKE_ATTR_DEAD
#endif

#if MAKE_GNUC_PREREQ(2, 7)
#define MAKE_ATTR_PRINTFLIKE(fmtarg, firstvararg) __attribute__((__format__ (__printf__, fmtarg, firstvararg)))

#else
#define MAKE_ATTR_PRINTFLIKE(fmtarg, firstvararg)
#endif

#define MAKE_INLINE static inline MAKE_ATTR_UNUSED


#if defined(lint)

#define MAKE_STATIC MAKE_INLINE
#else
#define MAKE_STATIC static MAKE_ATTR_UNUSED
#endif

#if __STDC_VERSION__ >= 199901L || defined(lint) || defined(USE_C99_BOOLEAN)
#include <stdbool.h>
#elif defined(__bool_true_false_are_defined)





#error "<stdbool.h> is included in pre-C99 mode"
#elif defined(bool) || defined(true) || defined(false)





#error "bool/true/false is defined in pre-C99 mode"
#else
typedef unsigned char bool;
#define true 1
#define false 0
#endif

#include "lst.h"
#include "make_malloc.h"
#include "str.h"
#include "hash.h"
#include "make-conf.h"
#include "buf.h"




#if defined(S_IFDIR) && !defined(S_ISDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#if defined(sun) && (defined(__svr4__) || defined(__SVR4))
#define POSIX_SIGNALS
#endif



















typedef enum GNodeMade {

UNMADE,


DEFERRED,


REQUESTED,



BEINGMADE,


MADE,

UPTODATE,


ERROR,


ABORTED
} GNodeMade;










typedef enum GNodeType {
OP_NONE = 0,



OP_DEPENDS = 1 << 0,


OP_FORCE = 1 << 1,




OP_DOUBLEDEP = 1 << 2,


OP_OPMASK = OP_DEPENDS | OP_FORCE | OP_DOUBLEDEP,


OP_OPTIONAL = 1 << 3,

OP_USE = 1 << 4,


OP_EXEC = 1 << 5,


OP_IGNORE = 1 << 6,

OP_PRECIOUS = 1 << 7,

OP_SILENT = 1 << 8,



OP_MAKE = 1 << 9,

OP_JOIN = 1 << 10,

OP_MADE = 1 << 11,

OP_SPECIAL = 1 << 12,

OP_USEBEFORE = 1 << 13,


OP_INVISIBLE = 1 << 14,


OP_NOTMAIN = 1 << 15,

OP_PHONY = 1 << 16,

OP_NOPATH = 1 << 17,




OP_WAIT = 1 << 18,

OP_NOMETA = 1 << 19,

OP_META = 1 << 20,

OP_NOMETA_CMP = 1 << 21,

OP_SUBMAKE = 1 << 22,




OP_TRANSFORM = 1 << 30,


OP_MEMBER = 1 << 29,


OP_LIB = 1 << 28,



OP_ARCHV = 1 << 27,



OP_HAS_COMMANDS = 1 << 26,



OP_SAVE_CMDS = 1 << 25,


OP_DEPS_FOUND = 1 << 24,

OP_MARK = 1 << 23,

OP_NOTARGET = OP_NOTMAIN | OP_USE | OP_EXEC | OP_TRANSFORM
} GNodeType;

typedef struct GNodeFlags {

bool remake:1;

bool childMade:1;

bool force:1;

bool doneWait:1;

bool doneOrder:1;

bool fromDepend:1;

bool doneAllsrc:1;

bool cycle:1;

bool doneCycle:1;
} GNodeFlags;

typedef struct List StringList;
typedef struct ListNode StringListNode;

typedef struct List GNodeList;
typedef struct ListNode GNodeListNode;

typedef struct SearchPath {
List dirs;
} SearchPath;





typedef struct GNode {

char *name;

char *uname;



char *path;




GNodeType type;
GNodeFlags flags;


GNodeMade made;

int unmade;



time_t mtime;
struct GNode *youngestChild;




GNodeList implicitParents;



GNodeList parents;

GNodeList children;




GNodeList order_pred;



GNodeList order_succ;





GNodeList cohorts;

char cohort_num[8];

int unmade_cohorts;


struct GNode *centurion;


unsigned int checked_seqno;









HashTable vars;


StringList commands;



struct Suffix *suffix;



const char *fname;

int lineno;
} GNode;


typedef enum ParseErrorLevel {


PARSE_FATAL = 1,

PARSE_WARNING,

PARSE_INFO
} ParseErrorLevel;




typedef enum CondEvalResult {
COND_PARSE,
COND_SKIP,
COND_INVALID
} CondEvalResult;


#define TARGET "@"
#define OODATE "?"
#define ALLSRC ">"
#define IMPSRC "<"
#define PREFIX "*"
#define ARCHIVE "!"
#define MEMBER "%"






extern bool allPrecious;

extern bool deleteOnError;

extern bool doing_depend;

extern GNode *defaultNode;





extern GNode *SCOPE_INTERNAL;

extern GNode *SCOPE_GLOBAL;

extern GNode *SCOPE_CMDLINE;





extern char var_Error[];


extern time_t now;





extern SearchPath dirSearchPath;

extern SearchPath *parseIncPath;




extern SearchPath *sysIncPath;

extern SearchPath *defSysIncPath;


extern char curdir[];

extern const char *progname;
extern int makelevel;

extern char *makeDependfile;

extern char **savedEnv;

extern pid_t myPid;

#define MAKEFLAGS ".MAKEFLAGS"
#define MAKEOVERRIDES ".MAKEOVERRIDES"

#define MAKE_JOB_PREFIX ".MAKE.JOB.PREFIX"
#define MAKE_EXPORTED ".MAKE.EXPORTED"
#define MAKE_MAKEFILES ".MAKE.MAKEFILES"
#define MAKE_LEVEL ".MAKE.LEVEL"
#define MAKE_MAKEFILE_PREFERENCE ".MAKE.MAKEFILE_PREFERENCE"
#define MAKE_DEPENDFILE ".MAKE.DEPENDFILE"
#define MAKE_MODE ".MAKE.MODE"
#if !defined(MAKE_LEVEL_ENV)
#define MAKE_LEVEL_ENV "MAKELEVEL"
#endif

typedef enum DebugFlags {
DEBUG_NONE = 0,
DEBUG_ARCH = 1 << 0,
DEBUG_COND = 1 << 1,
DEBUG_CWD = 1 << 2,
DEBUG_DIR = 1 << 3,
DEBUG_ERROR = 1 << 4,
DEBUG_FOR = 1 << 5,
DEBUG_GRAPH1 = 1 << 6,
DEBUG_GRAPH2 = 1 << 7,
DEBUG_GRAPH3 = 1 << 8,
DEBUG_HASH = 1 << 9,
DEBUG_JOB = 1 << 10,
DEBUG_LOUD = 1 << 11,
DEBUG_MAKE = 1 << 12,
DEBUG_META = 1 << 13,
DEBUG_PARSE = 1 << 14,
DEBUG_SCRIPT = 1 << 15,
DEBUG_SHELL = 1 << 16,
DEBUG_SUFF = 1 << 17,
DEBUG_TARG = 1 << 18,
DEBUG_VAR = 1 << 19,
DEBUG_ALL = (1 << 20) - 1
} DebugFlags;

#define CONCAT(a, b) a##b

#define DEBUG(module) ((opts.debug & CONCAT(DEBUG_, module)) != 0)

void debug_printf(const char *, ...) MAKE_ATTR_PRINTFLIKE(1, 2);

#define DEBUG_IMPL(module, args) do { if (DEBUG(module)) debug_printf args; } while (false)





#define DEBUG0(module, text) DEBUG_IMPL(module, ("%s", text))

#define DEBUG1(module, fmt, arg1) DEBUG_IMPL(module, (fmt, arg1))

#define DEBUG2(module, fmt, arg1, arg2) DEBUG_IMPL(module, (fmt, arg1, arg2))

#define DEBUG3(module, fmt, arg1, arg2, arg3) DEBUG_IMPL(module, (fmt, arg1, arg2, arg3))

#define DEBUG4(module, fmt, arg1, arg2, arg3, arg4) DEBUG_IMPL(module, (fmt, arg1, arg2, arg3, arg4))

#define DEBUG5(module, fmt, arg1, arg2, arg3, arg4, arg5) DEBUG_IMPL(module, (fmt, arg1, arg2, arg3, arg4, arg5))


typedef enum PrintVarsMode {
PVM_NONE,
PVM_UNEXPANDED,
PVM_EXPANDED
} PrintVarsMode;


typedef struct CmdOpts {

bool compatMake;



DebugFlags debug;


FILE *debug_file;





bool strict;


bool debugVflag;


bool checkEnvFirst;


StringList makefiles;


bool ignoreErrors;



int maxJobs;



bool keepgoing;


bool noRecursiveExecute;


bool noExecute;





bool queryFlag;


bool noBuiltins;


bool beSilent;



bool touchFlag;


PrintVarsMode printVars;

StringList variables;


bool parseWarnFatal;


bool enterFlag;



bool varNoExportEnv;



StringList create;

} CmdOpts;

extern CmdOpts opts;

#include "nonints.h"

void GNode_UpdateYoungestChild(GNode *, GNode *);
bool GNode_IsOODate(GNode *);
void Make_ExpandUse(GNodeList *);
time_t Make_Recheck(GNode *);
void Make_HandleUse(GNode *, GNode *);
void Make_Update(GNode *);
void GNode_SetLocalVars(GNode *);
bool Make_Run(GNodeList *);
bool shouldDieQuietly(GNode *, int);
void PrintOnError(GNode *, const char *);
void Main_ExportMAKEFLAGS(bool);
bool Main_SetObjdir(bool, const char *, ...) MAKE_ATTR_PRINTFLIKE(2, 3);
int mkTempFile(const char *, char *, size_t);
int str2Lst_Append(StringList *, char *);
void GNode_FprintDetails(FILE *, const char *, const GNode *, const char *);
bool GNode_ShouldExecute(GNode *gn);


MAKE_INLINE bool
GNode_IsTarget(const GNode *gn)
{
return (gn->type & OP_OPMASK) != OP_NONE;
}

MAKE_INLINE const char *
GNode_Path(const GNode *gn)
{
return gn->path != NULL ? gn->path : gn->name;
}

MAKE_INLINE bool
GNode_IsWaitingFor(const GNode *gn)
{
return gn->flags.remake && gn->made <= REQUESTED;
}

MAKE_INLINE bool
GNode_IsReady(const GNode *gn)
{
return gn->made > DEFERRED;
}

MAKE_INLINE bool
GNode_IsDone(const GNode *gn)
{
return gn->made >= MADE;
}

MAKE_INLINE bool
GNode_IsError(const GNode *gn)
{
return gn->made == ERROR || gn->made == ABORTED;
}

MAKE_INLINE const char *
GNode_VarTarget(GNode *gn) { return GNode_ValueDirect(gn, TARGET); }
MAKE_INLINE const char *
GNode_VarOodate(GNode *gn) { return GNode_ValueDirect(gn, OODATE); }
MAKE_INLINE const char *
GNode_VarAllsrc(GNode *gn) { return GNode_ValueDirect(gn, ALLSRC); }
MAKE_INLINE const char *
GNode_VarImpsrc(GNode *gn) { return GNode_ValueDirect(gn, IMPSRC); }
MAKE_INLINE const char *
GNode_VarPrefix(GNode *gn) { return GNode_ValueDirect(gn, PREFIX); }
MAKE_INLINE const char *
GNode_VarArchive(GNode *gn) { return GNode_ValueDirect(gn, ARCHIVE); }
MAKE_INLINE const char *
GNode_VarMember(GNode *gn) { return GNode_ValueDirect(gn, MEMBER); }

MAKE_INLINE void *
UNCONST(const void *ptr)
{
void *ret;
memcpy(&ret, &ptr, sizeof(ret));
return ret;
}


#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if !defined(MAXPATHLEN)
#define MAXPATHLEN BMAKE_PATH_MAX
#endif
#if !defined(PATH_MAX)
#define PATH_MAX MAXPATHLEN
#endif

#if defined(SYSV)
#define KILLPG(pid, sig) kill(-(pid), (sig))
#else
#define KILLPG(pid, sig) killpg((pid), (sig))
#endif

MAKE_INLINE bool
ch_isalnum(char ch) { return isalnum((unsigned char)ch) != 0; }
MAKE_INLINE bool
ch_isalpha(char ch) { return isalpha((unsigned char)ch) != 0; }
MAKE_INLINE bool
ch_isdigit(char ch) { return isdigit((unsigned char)ch) != 0; }
MAKE_INLINE bool
ch_isspace(char ch) { return isspace((unsigned char)ch) != 0; }
MAKE_INLINE bool
ch_isupper(char ch) { return isupper((unsigned char)ch) != 0; }
MAKE_INLINE char
ch_tolower(char ch) { return (char)tolower((unsigned char)ch); }
MAKE_INLINE char
ch_toupper(char ch) { return (char)toupper((unsigned char)ch); }

MAKE_INLINE void
cpp_skip_whitespace(const char **pp)
{
while (ch_isspace(**pp))
(*pp)++;
}

MAKE_INLINE void
cpp_skip_hspace(const char **pp)
{
while (**pp == ' ' || **pp == '\t')
(*pp)++;
}

MAKE_INLINE void
pp_skip_whitespace(char **pp)
{
while (ch_isspace(**pp))
(*pp)++;
}

MAKE_INLINE void
pp_skip_hspace(char **pp)
{
while (**pp == ' ' || **pp == '\t')
(*pp)++;
}

#if defined(lint)
#define MAKE_RCSID(id) extern void do_not_define_rcsid(void)
#elif defined(MAKE_NATIVE)
#include <sys/cdefs.h>
#if !defined(__IDSTRING)
#define __IDSTRING(name,string) static const char name[] MAKE_ATTR_UNUSED = string

#endif
#if !defined(__RCSID)
#define __RCSID(s) __IDSTRING(rcsid,s)
#endif
#if !defined(__COPYRIGHT)
#define __COPYRIGHT(s) __IDSTRING(copyright,s)
#endif
#define MAKE_RCSID(id) __RCSID(id)
#elif defined(MAKE_ALL_IN_ONE) && defined(__COUNTER__)
#define MAKE_RCSID_CONCAT(x, y) CONCAT(x, y)
#define MAKE_RCSID(id) static volatile char MAKE_RCSID_CONCAT(rcsid_, __COUNTER__)[] = id

#elif defined(MAKE_ALL_IN_ONE)
#define MAKE_RCSID(id) extern void do_not_define_rcsid(void)
#else
#define MAKE_RCSID(id) static volatile char rcsid[] = id
#endif

#endif
