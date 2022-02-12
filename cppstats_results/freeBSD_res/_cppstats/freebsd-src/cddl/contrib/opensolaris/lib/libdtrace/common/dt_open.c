#include <sys/types.h>
#if defined(illumos)
#include <sys/modctl.h>
#include <sys/systeminfo.h>
#else
#include <sys/param.h>
#include <sys/module.h>
#include <sys/linker.h>
#endif
#include <sys/resource.h>
#include <libelf.h>
#include <strings.h>
#if defined(illumos)
#include <alloca.h>
#endif
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#define _POSIX_PTHREAD_SEMANTICS
#include <dirent.h>
#undef _POSIX_PTHREAD_SEMANTICS
#include <dt_impl.h>
#include <dt_program.h>
#include <dt_module.h>
#include <dt_printf.h>
#include <dt_string.h>
#include <dt_provider.h>
#if !defined(illumos)
#include <sys/sysctl.h>
#include <string.h>
#endif
#if defined(__i386__)
#include <ieeefp.h>
#endif
#define DT_ATTR_STABCMN { DTRACE_STABILITY_STABLE, DTRACE_STABILITY_STABLE, DTRACE_CLASS_COMMON }
#define DT_ATTR_EVOLCMN { DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_COMMON }
#define DT_VERS_1_0 DT_VERSION_NUMBER(1, 0, 0)
#define DT_VERS_1_1 DT_VERSION_NUMBER(1, 1, 0)
#define DT_VERS_1_2 DT_VERSION_NUMBER(1, 2, 0)
#define DT_VERS_1_2_1 DT_VERSION_NUMBER(1, 2, 1)
#define DT_VERS_1_2_2 DT_VERSION_NUMBER(1, 2, 2)
#define DT_VERS_1_3 DT_VERSION_NUMBER(1, 3, 0)
#define DT_VERS_1_4 DT_VERSION_NUMBER(1, 4, 0)
#define DT_VERS_1_4_1 DT_VERSION_NUMBER(1, 4, 1)
#define DT_VERS_1_5 DT_VERSION_NUMBER(1, 5, 0)
#define DT_VERS_1_6 DT_VERSION_NUMBER(1, 6, 0)
#define DT_VERS_1_6_1 DT_VERSION_NUMBER(1, 6, 1)
#define DT_VERS_1_6_2 DT_VERSION_NUMBER(1, 6, 2)
#define DT_VERS_1_6_3 DT_VERSION_NUMBER(1, 6, 3)
#define DT_VERS_1_7 DT_VERSION_NUMBER(1, 7, 0)
#define DT_VERS_1_7_1 DT_VERSION_NUMBER(1, 7, 1)
#define DT_VERS_1_8 DT_VERSION_NUMBER(1, 8, 0)
#define DT_VERS_1_8_1 DT_VERSION_NUMBER(1, 8, 1)
#define DT_VERS_1_9 DT_VERSION_NUMBER(1, 9, 0)
#define DT_VERS_1_9_1 DT_VERSION_NUMBER(1, 9, 1)
#define DT_VERS_1_10 DT_VERSION_NUMBER(1, 10, 0)
#define DT_VERS_1_11 DT_VERSION_NUMBER(1, 11, 0)
#define DT_VERS_1_12 DT_VERSION_NUMBER(1, 12, 0)
#define DT_VERS_1_12_1 DT_VERSION_NUMBER(1, 12, 1)
#define DT_VERS_1_13 DT_VERSION_NUMBER(1, 13, 0)
#define DT_VERS_LATEST DT_VERS_1_13
#define DT_VERS_STRING "Sun D 1.13"
const dt_version_t _dtrace_versions[] = {
DT_VERS_1_0,
DT_VERS_1_1,
DT_VERS_1_2,
DT_VERS_1_2_1,
DT_VERS_1_2_2,
DT_VERS_1_3,
DT_VERS_1_4,
DT_VERS_1_4_1,
DT_VERS_1_5,
DT_VERS_1_6,
DT_VERS_1_6_1,
DT_VERS_1_6_2,
DT_VERS_1_6_3,
DT_VERS_1_7,
DT_VERS_1_7_1,
DT_VERS_1_8,
DT_VERS_1_8_1,
DT_VERS_1_9,
DT_VERS_1_9_1,
DT_VERS_1_10,
DT_VERS_1_11,
DT_VERS_1_12,
DT_VERS_1_12_1,
DT_VERS_1_13,
0
};
#if !defined(illumos)
static char curthread_str[MAXPATHLEN];
static char intmtx_str[MAXPATHLEN];
static char threadmtx_str[MAXPATHLEN];
static char rwlock_str[MAXPATHLEN];
static char sxlock_str[MAXPATHLEN];
#endif
static const dt_ident_t _dtrace_globals[] = {
{ "alloca", DT_IDENT_FUNC, 0, DIF_SUBR_ALLOCA, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void *(size_t)" },
{ "arg0", DT_IDENT_SCALAR, 0, DIF_VAR_ARG0, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg1", DT_IDENT_SCALAR, 0, DIF_VAR_ARG1, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg2", DT_IDENT_SCALAR, 0, DIF_VAR_ARG2, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg3", DT_IDENT_SCALAR, 0, DIF_VAR_ARG3, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg4", DT_IDENT_SCALAR, 0, DIF_VAR_ARG4, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg5", DT_IDENT_SCALAR, 0, DIF_VAR_ARG5, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg6", DT_IDENT_SCALAR, 0, DIF_VAR_ARG6, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg7", DT_IDENT_SCALAR, 0, DIF_VAR_ARG7, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg8", DT_IDENT_SCALAR, 0, DIF_VAR_ARG8, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "arg9", DT_IDENT_SCALAR, 0, DIF_VAR_ARG9, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "args", DT_IDENT_ARRAY, 0, DIF_VAR_ARGS, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_args, NULL },
{ "avg", DT_IDENT_AGGFUNC, 0, DTRACEAGG_AVG, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@)" },
{ "basename", DT_IDENT_FUNC, 0, DIF_SUBR_BASENAME, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "string(const char *)" },
{ "bcopy", DT_IDENT_FUNC, 0, DIF_SUBR_BCOPY, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(void *, void *, size_t)" },
{ "breakpoint", DT_IDENT_ACTFUNC, 0, DT_ACT_BREAKPOINT,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void()" },
{ "caller", DT_IDENT_SCALAR, 0, DIF_VAR_CALLER, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uintptr_t" },
{ "chill", DT_IDENT_ACTFUNC, 0, DT_ACT_CHILL, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(int)" },
{ "cleanpath", DT_IDENT_FUNC, 0, DIF_SUBR_CLEANPATH, DT_ATTR_STABCMN,
DT_VERS_1_0, &dt_idops_func, "string(const char *)" },
{ "clear", DT_IDENT_ACTFUNC, 0, DT_ACT_CLEAR, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(...)" },
{ "commit", DT_IDENT_ACTFUNC, 0, DT_ACT_COMMIT, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(int)" },
{ "copyin", DT_IDENT_FUNC, 0, DIF_SUBR_COPYIN, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void *(uintptr_t, size_t)" },
{ "copyinstr", DT_IDENT_FUNC, 0, DIF_SUBR_COPYINSTR,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "string(uintptr_t, [size_t])" },
{ "copyinto", DT_IDENT_FUNC, 0, DIF_SUBR_COPYINTO, DT_ATTR_STABCMN,
DT_VERS_1_0, &dt_idops_func, "void(uintptr_t, size_t, void *)" },
{ "copyout", DT_IDENT_FUNC, 0, DIF_SUBR_COPYOUT, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(void *, uintptr_t, size_t)" },
{ "copyoutstr", DT_IDENT_FUNC, 0, DIF_SUBR_COPYOUTSTR,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(char *, uintptr_t, size_t)" },
{ "count", DT_IDENT_AGGFUNC, 0, DTRACEAGG_COUNT, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void()" },
{ "curthread", DT_IDENT_SCALAR, 0, DIF_VAR_CURTHREAD,
{ DTRACE_STABILITY_STABLE, DTRACE_STABILITY_PRIVATE,
DTRACE_CLASS_COMMON }, DT_VERS_1_0,
#if defined(illumos)
&dt_idops_type, "genunix`kthread_t *" },
#else
&dt_idops_type, curthread_str },
#endif
{ "ddi_pathname", DT_IDENT_FUNC, 0, DIF_SUBR_DDI_PATHNAME,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "string(void *, int64_t)" },
{ "denormalize", DT_IDENT_ACTFUNC, 0, DT_ACT_DENORMALIZE, DT_ATTR_STABCMN,
DT_VERS_1_0, &dt_idops_func, "void(...)" },
{ "dirname", DT_IDENT_FUNC, 0, DIF_SUBR_DIRNAME, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "string(const char *)" },
{ "discard", DT_IDENT_ACTFUNC, 0, DT_ACT_DISCARD, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(int)" },
{ "epid", DT_IDENT_SCALAR, 0, DIF_VAR_EPID, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uint_t" },
{ "errno", DT_IDENT_SCALAR, 0, DIF_VAR_ERRNO, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int" },
{ "execargs", DT_IDENT_SCALAR, 0, DIF_VAR_EXECARGS,
DT_ATTR_STABCMN, DT_VERS_1_0, &dt_idops_type, "string" },
{ "execname", DT_IDENT_SCALAR, 0, DIF_VAR_EXECNAME,
DT_ATTR_STABCMN, DT_VERS_1_0, &dt_idops_type, "string" },
{ "exit", DT_IDENT_ACTFUNC, 0, DT_ACT_EXIT, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(int)" },
{ "freopen", DT_IDENT_ACTFUNC, 0, DT_ACT_FREOPEN, DT_ATTR_STABCMN,
DT_VERS_1_1, &dt_idops_func, "void(@, ...)" },
{ "ftruncate", DT_IDENT_ACTFUNC, 0, DT_ACT_FTRUNCATE, DT_ATTR_STABCMN,
DT_VERS_1_0, &dt_idops_func, "void()" },
{ "func", DT_IDENT_ACTFUNC, 0, DT_ACT_SYM, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "_symaddr(uintptr_t)" },
{ "getmajor", DT_IDENT_FUNC, 0, DIF_SUBR_GETMAJOR,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "genunix`major_t(genunix`dev_t)" },
{ "getminor", DT_IDENT_FUNC, 0, DIF_SUBR_GETMINOR,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "genunix`minor_t(genunix`dev_t)" },
{ "htonl", DT_IDENT_FUNC, 0, DIF_SUBR_HTONL, DT_ATTR_EVOLCMN, DT_VERS_1_3,
&dt_idops_func, "uint32_t(uint32_t)" },
{ "htonll", DT_IDENT_FUNC, 0, DIF_SUBR_HTONLL, DT_ATTR_EVOLCMN, DT_VERS_1_3,
&dt_idops_func, "uint64_t(uint64_t)" },
{ "htons", DT_IDENT_FUNC, 0, DIF_SUBR_HTONS, DT_ATTR_EVOLCMN, DT_VERS_1_3,
&dt_idops_func, "uint16_t(uint16_t)" },
{ "getf", DT_IDENT_FUNC, 0, DIF_SUBR_GETF, DT_ATTR_STABCMN, DT_VERS_1_10,
&dt_idops_func, "file_t *(int)" },
{ "gid", DT_IDENT_SCALAR, 0, DIF_VAR_GID, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "gid_t" },
{ "id", DT_IDENT_SCALAR, 0, DIF_VAR_ID, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uint_t" },
{ "index", DT_IDENT_FUNC, 0, DIF_SUBR_INDEX, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "int(const char *, const char *, [int])" },
{ "inet_ntoa", DT_IDENT_FUNC, 0, DIF_SUBR_INET_NTOA, DT_ATTR_STABCMN,
#if defined(illumos)
DT_VERS_1_5, &dt_idops_func, "string(ipaddr_t *)" },
#else
DT_VERS_1_5, &dt_idops_func, "string(in_addr_t *)" },
#endif
{ "inet_ntoa6", DT_IDENT_FUNC, 0, DIF_SUBR_INET_NTOA6, DT_ATTR_STABCMN,
#if defined(illumos)
DT_VERS_1_5, &dt_idops_func, "string(in6_addr_t *)" },
#else
DT_VERS_1_5, &dt_idops_func, "string(struct in6_addr *)" },
#endif
{ "inet_ntop", DT_IDENT_FUNC, 0, DIF_SUBR_INET_NTOP, DT_ATTR_STABCMN,
DT_VERS_1_5, &dt_idops_func, "string(int, void *)" },
{ "ipl", DT_IDENT_SCALAR, 0, DIF_VAR_IPL, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uint_t" },
#if defined(__FreeBSD__)
{ "jailname", DT_IDENT_SCALAR, 0, DIF_VAR_JAILNAME,
DT_ATTR_STABCMN, DT_VERS_1_13, &dt_idops_type, "string" },
{ "jid", DT_IDENT_SCALAR, 0, DIF_VAR_JID, DT_ATTR_STABCMN, DT_VERS_1_13,
&dt_idops_type, "int" },
#endif
{ "json", DT_IDENT_FUNC, 0, DIF_SUBR_JSON, DT_ATTR_STABCMN, DT_VERS_1_11,
&dt_idops_func, "string(const char *, const char *)" },
{ "jstack", DT_IDENT_ACTFUNC, 0, DT_ACT_JSTACK, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "stack(...)" },
{ "lltostr", DT_IDENT_FUNC, 0, DIF_SUBR_LLTOSTR, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "string(int64_t, [int])" },
{ "llquantize", DT_IDENT_AGGFUNC, 0, DTRACEAGG_LLQUANTIZE, DT_ATTR_STABCMN,
DT_VERS_1_7, &dt_idops_func,
"void(@, int32_t, int32_t, int32_t, int32_t, ...)" },
{ "lquantize", DT_IDENT_AGGFUNC, 0, DTRACEAGG_LQUANTIZE,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@, int32_t, int32_t, ...)" },
{ "max", DT_IDENT_AGGFUNC, 0, DTRACEAGG_MAX, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@)" },
{ "memref", DT_IDENT_FUNC, 0, DIF_SUBR_MEMREF, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "uintptr_t *(void *, size_t)" },
#if !defined(illumos)
{ "memstr", DT_IDENT_FUNC, 0, DIF_SUBR_MEMSTR, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "string(void *, char, size_t)" },
#endif
{ "min", DT_IDENT_AGGFUNC, 0, DTRACEAGG_MIN, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@)" },
{ "mod", DT_IDENT_ACTFUNC, 0, DT_ACT_MOD, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "_symaddr(uintptr_t)" },
#if defined(illumos)
{ "msgdsize", DT_IDENT_FUNC, 0, DIF_SUBR_MSGDSIZE,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "size_t(mblk_t *)" },
{ "msgsize", DT_IDENT_FUNC, 0, DIF_SUBR_MSGSIZE,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "size_t(mblk_t *)" },
{ "mutex_owned", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_OWNED,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "int(genunix`kmutex_t *)" },
{ "mutex_owner", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_OWNER,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "genunix`kthread_t *(genunix`kmutex_t *)" },
{ "mutex_type_adaptive", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_TYPE_ADAPTIVE,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "int(genunix`kmutex_t *)" },
{ "mutex_type_spin", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_TYPE_SPIN,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "int(genunix`kmutex_t *)" },
#else
{ "mutex_owned", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_OWNED,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, intmtx_str },
{ "mutex_owner", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_OWNER,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, threadmtx_str },
{ "mutex_type_adaptive", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_TYPE_ADAPTIVE,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, intmtx_str },
{ "mutex_type_spin", DT_IDENT_FUNC, 0, DIF_SUBR_MUTEX_TYPE_SPIN,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, intmtx_str },
#endif
{ "ntohl", DT_IDENT_FUNC, 0, DIF_SUBR_NTOHL, DT_ATTR_EVOLCMN, DT_VERS_1_3,
&dt_idops_func, "uint32_t(uint32_t)" },
{ "ntohll", DT_IDENT_FUNC, 0, DIF_SUBR_NTOHLL, DT_ATTR_EVOLCMN, DT_VERS_1_3,
&dt_idops_func, "uint64_t(uint64_t)" },
{ "ntohs", DT_IDENT_FUNC, 0, DIF_SUBR_NTOHS, DT_ATTR_EVOLCMN, DT_VERS_1_3,
&dt_idops_func, "uint16_t(uint16_t)" },
{ "normalize", DT_IDENT_ACTFUNC, 0, DT_ACT_NORMALIZE, DT_ATTR_STABCMN,
DT_VERS_1_0, &dt_idops_func, "void(...)" },
{ "panic", DT_IDENT_ACTFUNC, 0, DT_ACT_PANIC, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void()" },
{ "pid", DT_IDENT_SCALAR, 0, DIF_VAR_PID, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "pid_t" },
{ "ppid", DT_IDENT_SCALAR, 0, DIF_VAR_PPID, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "pid_t" },
{ "print", DT_IDENT_ACTFUNC, 0, DT_ACT_PRINT, DT_ATTR_STABCMN, DT_VERS_1_9,
&dt_idops_func, "void(@)" },
{ "printa", DT_IDENT_ACTFUNC, 0, DT_ACT_PRINTA, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@, ...)" },
{ "printf", DT_IDENT_ACTFUNC, 0, DT_ACT_PRINTF, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@, ...)" },
{ "printm", DT_IDENT_ACTFUNC, 0, DT_ACT_PRINTM, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(size_t, uintptr_t *)" },
{ "probefunc", DT_IDENT_SCALAR, 0, DIF_VAR_PROBEFUNC,
DT_ATTR_STABCMN, DT_VERS_1_0, &dt_idops_type, "string" },
{ "probemod", DT_IDENT_SCALAR, 0, DIF_VAR_PROBEMOD,
DT_ATTR_STABCMN, DT_VERS_1_0, &dt_idops_type, "string" },
{ "probename", DT_IDENT_SCALAR, 0, DIF_VAR_PROBENAME,
DT_ATTR_STABCMN, DT_VERS_1_0, &dt_idops_type, "string" },
{ "probeprov", DT_IDENT_SCALAR, 0, DIF_VAR_PROBEPROV,
DT_ATTR_STABCMN, DT_VERS_1_0, &dt_idops_type, "string" },
{ "progenyof", DT_IDENT_FUNC, 0, DIF_SUBR_PROGENYOF,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "int(pid_t)" },
{ "quantize", DT_IDENT_AGGFUNC, 0, DTRACEAGG_QUANTIZE,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@, ...)" },
{ "raise", DT_IDENT_ACTFUNC, 0, DT_ACT_RAISE, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(int)" },
{ "rand", DT_IDENT_FUNC, 0, DIF_SUBR_RAND, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "int()" },
{ "rindex", DT_IDENT_FUNC, 0, DIF_SUBR_RINDEX, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "int(const char *, const char *, [int])" },
#if defined(illumos)
{ "rw_iswriter", DT_IDENT_FUNC, 0, DIF_SUBR_RW_ISWRITER,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "int(genunix`krwlock_t *)" },
{ "rw_read_held", DT_IDENT_FUNC, 0, DIF_SUBR_RW_READ_HELD,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "int(genunix`krwlock_t *)" },
{ "rw_write_held", DT_IDENT_FUNC, 0, DIF_SUBR_RW_WRITE_HELD,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, "int(genunix`krwlock_t *)" },
#else
{ "rw_iswriter", DT_IDENT_FUNC, 0, DIF_SUBR_RW_ISWRITER,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, rwlock_str },
{ "rw_read_held", DT_IDENT_FUNC, 0, DIF_SUBR_RW_READ_HELD,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, rwlock_str },
{ "rw_write_held", DT_IDENT_FUNC, 0, DIF_SUBR_RW_WRITE_HELD,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, rwlock_str },
#endif
{ "self", DT_IDENT_PTR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "void" },
{ "setopt", DT_IDENT_ACTFUNC, 0, DT_ACT_SETOPT, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "void(const char *, [const char *])" },
{ "speculate", DT_IDENT_ACTFUNC, 0, DT_ACT_SPECULATE,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(int)" },
{ "speculation", DT_IDENT_FUNC, 0, DIF_SUBR_SPECULATION,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "int()" },
{ "stack", DT_IDENT_ACTFUNC, 0, DT_ACT_STACK, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "stack(...)" },
{ "stackdepth", DT_IDENT_SCALAR, 0, DIF_VAR_STACKDEPTH,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uint32_t" },
{ "stddev", DT_IDENT_AGGFUNC, 0, DTRACEAGG_STDDEV, DT_ATTR_STABCMN,
DT_VERS_1_6, &dt_idops_func, "void(@)" },
{ "stop", DT_IDENT_ACTFUNC, 0, DT_ACT_STOP, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void()" },
{ "strchr", DT_IDENT_FUNC, 0, DIF_SUBR_STRCHR, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "string(const char *, char)" },
{ "strlen", DT_IDENT_FUNC, 0, DIF_SUBR_STRLEN, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "size_t(const char *)" },
{ "strjoin", DT_IDENT_FUNC, 0, DIF_SUBR_STRJOIN, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "string(const char *, const char *)" },
{ "strrchr", DT_IDENT_FUNC, 0, DIF_SUBR_STRRCHR, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "string(const char *, char)" },
{ "strstr", DT_IDENT_FUNC, 0, DIF_SUBR_STRSTR, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "string(const char *, const char *)" },
{ "strtok", DT_IDENT_FUNC, 0, DIF_SUBR_STRTOK, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "string(const char *, const char *)" },
{ "strtoll", DT_IDENT_FUNC, 0, DIF_SUBR_STRTOLL, DT_ATTR_STABCMN, DT_VERS_1_11,
&dt_idops_func, "int64_t(const char *, [int])" },
{ "substr", DT_IDENT_FUNC, 0, DIF_SUBR_SUBSTR, DT_ATTR_STABCMN, DT_VERS_1_1,
&dt_idops_func, "string(const char *, int, [int])" },
{ "sum", DT_IDENT_AGGFUNC, 0, DTRACEAGG_SUM, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@)" },
#if !defined(illumos)
{ "sx_isexclusive", DT_IDENT_FUNC, 0, DIF_SUBR_SX_ISEXCLUSIVE,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, sxlock_str },
{ "sx_shared_held", DT_IDENT_FUNC, 0, DIF_SUBR_SX_SHARED_HELD,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, sxlock_str },
{ "sx_exclusive_held", DT_IDENT_FUNC, 0, DIF_SUBR_SX_EXCLUSIVE_HELD,
DT_ATTR_EVOLCMN, DT_VERS_1_0,
&dt_idops_func, sxlock_str },
#endif
{ "sym", DT_IDENT_ACTFUNC, 0, DT_ACT_SYM, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "_symaddr(uintptr_t)" },
{ "system", DT_IDENT_ACTFUNC, 0, DT_ACT_SYSTEM, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@, ...)" },
{ "this", DT_IDENT_PTR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "void" },
{ "tid", DT_IDENT_SCALAR, 0, DIF_VAR_TID, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "id_t" },
{ "timestamp", DT_IDENT_SCALAR, 0, DIF_VAR_TIMESTAMP,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uint64_t" },
{ "tolower", DT_IDENT_FUNC, 0, DIF_SUBR_TOLOWER, DT_ATTR_STABCMN, DT_VERS_1_8,
&dt_idops_func, "string(const char *)" },
{ "toupper", DT_IDENT_FUNC, 0, DIF_SUBR_TOUPPER, DT_ATTR_STABCMN, DT_VERS_1_8,
&dt_idops_func, "string(const char *)" },
{ "trace", DT_IDENT_ACTFUNC, 0, DT_ACT_TRACE, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@)" },
{ "tracemem", DT_IDENT_ACTFUNC, 0, DT_ACT_TRACEMEM,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "void(@, size_t, ...)" },
{ "trunc", DT_IDENT_ACTFUNC, 0, DT_ACT_TRUNC, DT_ATTR_STABCMN,
DT_VERS_1_0, &dt_idops_func, "void(...)" },
{ "uaddr", DT_IDENT_ACTFUNC, 0, DT_ACT_UADDR, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "_usymaddr(uintptr_t)" },
{ "ucaller", DT_IDENT_SCALAR, 0, DIF_VAR_UCALLER, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_type, "uint64_t" },
{ "ufunc", DT_IDENT_ACTFUNC, 0, DT_ACT_USYM, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "_usymaddr(uintptr_t)" },
{ "uid", DT_IDENT_SCALAR, 0, DIF_VAR_UID, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uid_t" },
{ "umod", DT_IDENT_ACTFUNC, 0, DT_ACT_UMOD, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "_usymaddr(uintptr_t)" },
{ "uregs", DT_IDENT_ARRAY, 0, DIF_VAR_UREGS, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_regs, NULL },
{ "ustack", DT_IDENT_ACTFUNC, 0, DT_ACT_USTACK, DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_func, "stack(...)" },
{ "ustackdepth", DT_IDENT_SCALAR, 0, DIF_VAR_USTACKDEPTH,
DT_ATTR_STABCMN, DT_VERS_1_2,
&dt_idops_type, "uint32_t" },
{ "usym", DT_IDENT_ACTFUNC, 0, DT_ACT_USYM, DT_ATTR_STABCMN,
DT_VERS_1_2, &dt_idops_func, "_usymaddr(uintptr_t)" },
{ "vtimestamp", DT_IDENT_SCALAR, 0, DIF_VAR_VTIMESTAMP,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "uint64_t" },
{ "walltimestamp", DT_IDENT_SCALAR, 0, DIF_VAR_WALLTIMESTAMP,
DT_ATTR_STABCMN, DT_VERS_1_0,
&dt_idops_type, "int64_t" },
{ "zonename", DT_IDENT_SCALAR, 0, DIF_VAR_ZONENAME,
DT_ATTR_STABCMN, DT_VERS_1_0, &dt_idops_type, "string" },
#if !defined(illumos)
{ "cpu", DT_IDENT_SCALAR, 0, DIF_VAR_CPU,
DT_ATTR_STABCMN, DT_VERS_1_6_3, &dt_idops_type, "int" },
#endif
{ NULL, 0, 0, 0, { 0, 0, 0 }, 0, NULL, NULL }
};
static const dt_intrinsic_t _dtrace_intrinsics_32[] = {
{ "void", { CTF_INT_SIGNED, 0, 0 }, CTF_K_INTEGER },
{ "signed", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "unsigned", { 0, 0, 32 }, CTF_K_INTEGER },
{ "char", { CTF_INT_SIGNED | CTF_INT_CHAR, 0, 8 }, CTF_K_INTEGER },
{ "short", { CTF_INT_SIGNED, 0, 16 }, CTF_K_INTEGER },
{ "int", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "long", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "long long", { CTF_INT_SIGNED, 0, 64 }, CTF_K_INTEGER },
{ "signed char", { CTF_INT_SIGNED | CTF_INT_CHAR, 0, 8 }, CTF_K_INTEGER },
{ "signed short", { CTF_INT_SIGNED, 0, 16 }, CTF_K_INTEGER },
{ "signed int", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "signed long", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "signed long long", { CTF_INT_SIGNED, 0, 64 }, CTF_K_INTEGER },
{ "unsigned char", { CTF_INT_CHAR, 0, 8 }, CTF_K_INTEGER },
{ "unsigned short", { 0, 0, 16 }, CTF_K_INTEGER },
{ "unsigned int", { 0, 0, 32 }, CTF_K_INTEGER },
{ "unsigned long", { 0, 0, 32 }, CTF_K_INTEGER },
{ "unsigned long long", { 0, 0, 64 }, CTF_K_INTEGER },
{ "_Bool", { CTF_INT_BOOL, 0, 8 }, CTF_K_INTEGER },
{ "float", { CTF_FP_SINGLE, 0, 32 }, CTF_K_FLOAT },
{ "double", { CTF_FP_DOUBLE, 0, 64 }, CTF_K_FLOAT },
{ "long double", { CTF_FP_LDOUBLE, 0, 128 }, CTF_K_FLOAT },
{ "float imaginary", { CTF_FP_IMAGRY, 0, 32 }, CTF_K_FLOAT },
{ "double imaginary", { CTF_FP_DIMAGRY, 0, 64 }, CTF_K_FLOAT },
{ "long double imaginary", { CTF_FP_LDIMAGRY, 0, 128 }, CTF_K_FLOAT },
{ "float complex", { CTF_FP_CPLX, 0, 64 }, CTF_K_FLOAT },
{ "double complex", { CTF_FP_DCPLX, 0, 128 }, CTF_K_FLOAT },
{ "long double complex", { CTF_FP_LDCPLX, 0, 256 }, CTF_K_FLOAT },
{ NULL, { 0, 0, 0 }, 0 }
};
static const dt_intrinsic_t _dtrace_intrinsics_64[] = {
{ "void", { CTF_INT_SIGNED, 0, 0 }, CTF_K_INTEGER },
{ "signed", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "unsigned", { 0, 0, 32 }, CTF_K_INTEGER },
{ "char", { CTF_INT_SIGNED | CTF_INT_CHAR, 0, 8 }, CTF_K_INTEGER },
{ "short", { CTF_INT_SIGNED, 0, 16 }, CTF_K_INTEGER },
{ "int", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "long", { CTF_INT_SIGNED, 0, 64 }, CTF_K_INTEGER },
{ "long long", { CTF_INT_SIGNED, 0, 64 }, CTF_K_INTEGER },
{ "signed char", { CTF_INT_SIGNED | CTF_INT_CHAR, 0, 8 }, CTF_K_INTEGER },
{ "signed short", { CTF_INT_SIGNED, 0, 16 }, CTF_K_INTEGER },
{ "signed int", { CTF_INT_SIGNED, 0, 32 }, CTF_K_INTEGER },
{ "signed long", { CTF_INT_SIGNED, 0, 64 }, CTF_K_INTEGER },
{ "signed long long", { CTF_INT_SIGNED, 0, 64 }, CTF_K_INTEGER },
{ "unsigned char", { CTF_INT_CHAR, 0, 8 }, CTF_K_INTEGER },
{ "unsigned short", { 0, 0, 16 }, CTF_K_INTEGER },
{ "unsigned int", { 0, 0, 32 }, CTF_K_INTEGER },
{ "unsigned long", { 0, 0, 64 }, CTF_K_INTEGER },
{ "unsigned long long", { 0, 0, 64 }, CTF_K_INTEGER },
{ "_Bool", { CTF_INT_BOOL, 0, 8 }, CTF_K_INTEGER },
{ "float", { CTF_FP_SINGLE, 0, 32 }, CTF_K_FLOAT },
{ "double", { CTF_FP_DOUBLE, 0, 64 }, CTF_K_FLOAT },
{ "long double", { CTF_FP_LDOUBLE, 0, 128 }, CTF_K_FLOAT },
{ "float imaginary", { CTF_FP_IMAGRY, 0, 32 }, CTF_K_FLOAT },
{ "double imaginary", { CTF_FP_DIMAGRY, 0, 64 }, CTF_K_FLOAT },
{ "long double imaginary", { CTF_FP_LDIMAGRY, 0, 128 }, CTF_K_FLOAT },
{ "float complex", { CTF_FP_CPLX, 0, 64 }, CTF_K_FLOAT },
{ "double complex", { CTF_FP_DCPLX, 0, 128 }, CTF_K_FLOAT },
{ "long double complex", { CTF_FP_LDCPLX, 0, 256 }, CTF_K_FLOAT },
{ NULL, { 0, 0, 0 }, 0 }
};
static const dt_typedef_t _dtrace_typedefs_32[] = {
{ "char", "int8_t" },
{ "short", "int16_t" },
{ "int", "int32_t" },
{ "long long", "int64_t" },
{ "int", "intptr_t" },
{ "int", "ssize_t" },
{ "unsigned char", "uint8_t" },
{ "unsigned short", "uint16_t" },
{ "unsigned", "uint32_t" },
{ "unsigned long long", "uint64_t" },
{ "unsigned char", "uchar_t" },
{ "unsigned short", "ushort_t" },
{ "unsigned", "uint_t" },
{ "unsigned long", "ulong_t" },
{ "unsigned long long", "u_longlong_t" },
{ "int", "ptrdiff_t" },
{ "unsigned", "uintptr_t" },
{ "unsigned", "size_t" },
{ "long", "id_t" },
{ "long", "pid_t" },
{ NULL, NULL }
};
static const dt_typedef_t _dtrace_typedefs_64[] = {
{ "char", "int8_t" },
{ "short", "int16_t" },
{ "int", "int32_t" },
{ "long", "int64_t" },
{ "long", "intptr_t" },
{ "long", "ssize_t" },
{ "unsigned char", "uint8_t" },
{ "unsigned short", "uint16_t" },
{ "unsigned", "uint32_t" },
{ "unsigned long", "uint64_t" },
{ "unsigned char", "uchar_t" },
{ "unsigned short", "ushort_t" },
{ "unsigned", "uint_t" },
{ "unsigned long", "ulong_t" },
{ "unsigned long long", "u_longlong_t" },
{ "long", "ptrdiff_t" },
{ "unsigned long", "uintptr_t" },
{ "unsigned long", "size_t" },
{ "int", "id_t" },
{ "int", "pid_t" },
{ NULL, NULL }
};
static const dt_intdesc_t _dtrace_ints_32[] = {
{ "int", NULL, CTF_ERR, 0x7fffffffULL },
{ "unsigned int", NULL, CTF_ERR, 0xffffffffULL },
{ "long", NULL, CTF_ERR, 0x7fffffffULL },
{ "unsigned long", NULL, CTF_ERR, 0xffffffffULL },
{ "long long", NULL, CTF_ERR, 0x7fffffffffffffffULL },
{ "unsigned long long", NULL, CTF_ERR, 0xffffffffffffffffULL }
};
static const dt_intdesc_t _dtrace_ints_64[] = {
{ "int", NULL, CTF_ERR, 0x7fffffffULL },
{ "unsigned int", NULL, CTF_ERR, 0xffffffffULL },
{ "long", NULL, CTF_ERR, 0x7fffffffffffffffULL },
{ "unsigned long", NULL, CTF_ERR, 0xffffffffffffffffULL },
{ "long long", NULL, CTF_ERR, 0x7fffffffffffffffULL },
{ "unsigned long long", NULL, CTF_ERR, 0xffffffffffffffffULL }
};
static const dt_ident_t _dtrace_macros[] = {
{ "egid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "euid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "gid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "pid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "pgid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "ppid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "projid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "sid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "taskid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "target", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ "uid", DT_IDENT_SCALAR, 0, 0, DT_ATTR_STABCMN, DT_VERS_1_0 },
{ NULL, 0, 0, 0, { 0, 0, 0 }, 0 }
};
static const char _dtrace_hardwire[] = "\
inline long NULL = 0; \n\
#pragma D binding \"1.0\" NULL\n";
static const dtrace_conf_t _dtrace_conf = {
DIF_VERSION,
DIF_DIR_NREGS,
DIF_DTR_NREGS,
CTF_MODEL_NATIVE
};
const dtrace_attribute_t _dtrace_maxattr = {
DTRACE_STABILITY_MAX,
DTRACE_STABILITY_MAX,
DTRACE_CLASS_MAX
};
const dtrace_attribute_t _dtrace_defattr = {
DTRACE_STABILITY_STABLE,
DTRACE_STABILITY_STABLE,
DTRACE_CLASS_COMMON
};
const dtrace_attribute_t _dtrace_symattr = {
DTRACE_STABILITY_PRIVATE,
DTRACE_STABILITY_PRIVATE,
DTRACE_CLASS_UNKNOWN
};
const dtrace_attribute_t _dtrace_typattr = {
DTRACE_STABILITY_PRIVATE,
DTRACE_STABILITY_PRIVATE,
DTRACE_CLASS_UNKNOWN
};
const dtrace_attribute_t _dtrace_prvattr = {
DTRACE_STABILITY_PRIVATE,
DTRACE_STABILITY_PRIVATE,
DTRACE_CLASS_UNKNOWN
};
const dtrace_pattr_t _dtrace_prvdesc = {
{ DTRACE_STABILITY_UNSTABLE, DTRACE_STABILITY_UNSTABLE, DTRACE_CLASS_COMMON },
{ DTRACE_STABILITY_UNSTABLE, DTRACE_STABILITY_UNSTABLE, DTRACE_CLASS_COMMON },
{ DTRACE_STABILITY_UNSTABLE, DTRACE_STABILITY_UNSTABLE, DTRACE_CLASS_COMMON },
{ DTRACE_STABILITY_UNSTABLE, DTRACE_STABILITY_UNSTABLE, DTRACE_CLASS_COMMON },
{ DTRACE_STABILITY_UNSTABLE, DTRACE_STABILITY_UNSTABLE, DTRACE_CLASS_COMMON },
};
#if defined(illumos)
const char *_dtrace_defcpp = "/usr/ccs/lib/cpp";
const char *_dtrace_defld = "/usr/ccs/bin/ld";
#else
const char *_dtrace_defcpp = "cpp";
const char *_dtrace_defld = "ld";
const char *_dtrace_defobjcopy = "objcopy";
#endif
const char *_dtrace_libdir = "/usr/lib/dtrace";
#if defined(illumos)
const char *_dtrace_provdir = "/dev/dtrace/provider";
#else
const char *_dtrace_libdir32 = "/usr/lib32/dtrace";
const char *_dtrace_provdir = "/dev/dtrace";
#endif
int _dtrace_strbuckets = 211;
int _dtrace_intbuckets = 256;
uint_t _dtrace_strsize = 256;
uint_t _dtrace_stkindent = 14;
uint_t _dtrace_pidbuckets = 64;
uint_t _dtrace_pidlrulim = 8;
size_t _dtrace_bufsize = 512;
int _dtrace_argmax = 32;
int _dtrace_debug = 0;
const char *const _dtrace_version = DT_VERS_STRING;
int _dtrace_rdvers = RD_VERSION;
typedef struct dt_fdlist {
int *df_fds;
uint_t df_ents;
uint_t df_size;
} dt_fdlist_t;
#if defined(illumos)
#pragma init(_dtrace_init)
#else
void _dtrace_init(void) __attribute__ ((constructor));
#endif
void
_dtrace_init(void)
{
_dtrace_debug = getenv("DTRACE_DEBUG") != NULL;
for (; _dtrace_rdvers > 0; _dtrace_rdvers--) {
if (rd_init(_dtrace_rdvers) == RD_OK)
break;
}
#if defined(__i386__)
(void) fpsetprec(FP_PE);
#endif
}
static dtrace_hdl_t *
set_open_errno(dtrace_hdl_t *dtp, int *errp, int err)
{
if (dtp != NULL)
dtrace_close(dtp);
if (errp != NULL)
*errp = err;
return (NULL);
}
static void
dt_provmod_open(dt_provmod_t **provmod, dt_fdlist_t *dfp)
{
dt_provmod_t *prov;
char path[PATH_MAX];
int fd;
#if defined(illumos)
struct dirent *dp, *ep;
DIR *dirp;
if ((dirp = opendir(_dtrace_provdir)) == NULL)
return;
ep = alloca(sizeof (struct dirent) + PATH_MAX + 1);
bzero(ep, sizeof (struct dirent) + PATH_MAX + 1);
while (readdir_r(dirp, ep, &dp) == 0 && dp != NULL) {
if (dp->d_name[0] == '.')
continue;
if (dfp->df_ents == dfp->df_size) {
uint_t size = dfp->df_size ? dfp->df_size * 2 : 16;
int *fds = realloc(dfp->df_fds, size * sizeof (int));
if (fds == NULL)
break;
dfp->df_fds = fds;
dfp->df_size = size;
}
(void) snprintf(path, sizeof (path), "%s/%s",
_dtrace_provdir, dp->d_name);
if ((fd = open(path, O_RDONLY)) == -1)
continue;
if (((prov = malloc(sizeof (dt_provmod_t))) == NULL) ||
(prov->dp_name = malloc(strlen(dp->d_name) + 1)) == NULL) {
free(prov);
(void) close(fd);
break;
}
(void) strcpy(prov->dp_name, dp->d_name);
prov->dp_next = *provmod;
*provmod = prov;
dt_dprintf("opened provider %s\n", dp->d_name);
dfp->df_fds[dfp->df_ents++] = fd;
}
(void) closedir(dirp);
#else
char *p;
char *p1;
char *p_providers = NULL;
int error;
size_t len = 0;
while(1) {
error = sysctlbyname("debug.dtrace.providers",p_providers,&len,NULL,0);
if (len == 0)
break;
else if (error == 0 && p_providers == NULL) {
if ((p_providers = malloc(len)) == NULL)
return;
} else if (error == -1 && errno == ENOMEM) {
if ((p = realloc(p_providers,len)) == NULL) {
free(p_providers);
return;
}
p_providers = p;
} else
break;
}
if (error == 0 && len > 0 && p_providers != NULL) {
p = p_providers;
while ((p1 = strsep(&p," ")) != NULL) {
if (dfp->df_ents == dfp->df_size) {
uint_t size = dfp->df_size ? dfp->df_size * 2 : 16;
int *fds = realloc(dfp->df_fds, size * sizeof (int));
if (fds == NULL)
break;
dfp->df_fds = fds;
dfp->df_size = size;
}
(void) snprintf(path, sizeof (path), "/dev/dtrace/%s", p1);
if ((fd = open(path, O_RDONLY | O_CLOEXEC)) == -1)
continue;
if (((prov = malloc(sizeof (dt_provmod_t))) == NULL) ||
(prov->dp_name = malloc(strlen(p1) + 1)) == NULL) {
free(prov);
(void) close(fd);
break;
}
(void) strcpy(prov->dp_name, p1);
prov->dp_next = *provmod;
*provmod = prov;
dt_dprintf("opened provider %s\n", p1);
dfp->df_fds[dfp->df_ents++] = fd;
}
}
if (p_providers != NULL)
free(p_providers);
#endif
}
static void
dt_provmod_destroy(dt_provmod_t **provmod)
{
dt_provmod_t *next, *current;
for (current = *provmod; current != NULL; current = next) {
next = current->dp_next;
free(current->dp_name);
free(current);
}
*provmod = NULL;
}
#if defined(illumos)
static const char *
dt_get_sysinfo(int cmd, char *buf, size_t len)
{
ssize_t rv = sysinfo(cmd, buf, len);
char *p = buf;
if (rv < 0 || rv > len)
(void) snprintf(buf, len, "%s", "Unknown");
while ((p = strchr(p, '.')) != NULL)
*p++ = '_';
return (buf);
}
#endif
static dtrace_hdl_t *
dt_vopen(int version, int flags, int *errp,
const dtrace_vector_t *vector, void *arg)
{
dtrace_hdl_t *dtp = NULL;
int dtfd = -1, ftfd = -1, fterr = 0;
dtrace_prog_t *pgp;
dt_module_t *dmp;
dt_provmod_t *provmod = NULL;
int i, err;
struct rlimit rl;
const dt_intrinsic_t *dinp;
const dt_typedef_t *dtyp;
const dt_ident_t *idp;
dtrace_typeinfo_t dtt;
ctf_funcinfo_t ctc;
ctf_arinfo_t ctr;
dt_fdlist_t df = { NULL, 0, 0 };
char isadef[32], utsdef[32];
char s1[64], s2[64];
if (version <= 0)
return (set_open_errno(dtp, errp, EINVAL));
if (version > DTRACE_VERSION)
return (set_open_errno(dtp, errp, EDT_VERSION));
if (version < DTRACE_VERSION) {
return (set_open_errno(dtp, errp, EDT_OVERSION));
}
if (flags & ~DTRACE_O_MASK)
return (set_open_errno(dtp, errp, EINVAL));
if ((flags & DTRACE_O_LP64) && (flags & DTRACE_O_ILP32))
return (set_open_errno(dtp, errp, EINVAL));
if (vector == NULL && arg != NULL)
return (set_open_errno(dtp, errp, EINVAL));
if (elf_version(EV_CURRENT) == EV_NONE)
return (set_open_errno(dtp, errp, EDT_ELFVERSION));
if (vector != NULL || (flags & DTRACE_O_NODEV))
goto alloc;
if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
rl.rlim_cur = rl.rlim_max;
(void) setrlimit(RLIMIT_NOFILE, &rl);
}
dt_provmod_open(&provmod, &df);
dtfd = open("/dev/dtrace/dtrace", O_RDWR | O_CLOEXEC);
err = dtfd == -1 ? errno : 0;
#if defined(__FreeBSD__)
if (err == ENOENT && modfind("dtraceall") < 0) {
kldload("dtraceall");
dtfd = open("/dev/dtrace/dtrace", O_RDWR | O_CLOEXEC);
err = errno;
}
#endif
#if defined(illumos)
ftfd = open("/dev/dtrace/provider/fasttrap", O_RDWR);
#else
ftfd = open("/dev/dtrace/fasttrap", O_RDWR | O_CLOEXEC);
#endif
fterr = ftfd == -1 ? errno : 0;
while (df.df_ents-- != 0)
(void) close(df.df_fds[df.df_ents]);
free(df.df_fds);
if (dtfd == -1) {
dt_provmod_destroy(&provmod);
switch (err) {
case ENOENT:
err = EDT_NOENT;
break;
case EBUSY:
err = EDT_BUSY;
break;
case EACCES:
err = EDT_ACCESS;
break;
}
return (set_open_errno(dtp, errp, err));
}
alloc:
if ((dtp = malloc(sizeof (dtrace_hdl_t))) == NULL) {
dt_provmod_destroy(&provmod);
return (set_open_errno(dtp, errp, EDT_NOMEM));
}
bzero(dtp, sizeof (dtrace_hdl_t));
dtp->dt_oflags = flags;
#if defined(illumos)
dtp->dt_prcmode = DT_PROC_STOP_PREINIT;
#else
dtp->dt_prcmode = DT_PROC_STOP_POSTINIT;
#endif
dtp->dt_linkmode = DT_LINK_KERNEL;
dtp->dt_linktype = DT_LTYP_ELF;
dtp->dt_xlatemode = DT_XL_STATIC;
dtp->dt_stdcmode = DT_STDC_XA;
dtp->dt_encoding = DT_ENCODING_UNSET;
dtp->dt_version = version;
dtp->dt_fd = dtfd;
dtp->dt_ftfd = ftfd;
dtp->dt_fterr = fterr;
dtp->dt_cdefs_fd = -1;
dtp->dt_ddefs_fd = -1;
#if defined(illumos)
dtp->dt_stdout_fd = -1;
#else
dtp->dt_freopen_fp = NULL;
#endif
dtp->dt_modbuckets = _dtrace_strbuckets;
dtp->dt_mods = calloc(dtp->dt_modbuckets, sizeof (dt_module_t *));
#if defined(__FreeBSD__)
dtp->dt_kmods = calloc(dtp->dt_modbuckets, sizeof (dt_module_t *));
#endif
dtp->dt_provbuckets = _dtrace_strbuckets;
dtp->dt_provs = calloc(dtp->dt_provbuckets, sizeof (dt_provider_t *));
dt_proc_init(dtp);
dtp->dt_vmax = DT_VERS_LATEST;
dtp->dt_cpp_path = strdup(_dtrace_defcpp);
dtp->dt_cpp_argv = malloc(sizeof (char *));
dtp->dt_cpp_argc = 1;
dtp->dt_cpp_args = 1;
dtp->dt_ld_path = strdup(_dtrace_defld);
#if defined(__FreeBSD__)
dtp->dt_objcopy_path = strdup(_dtrace_defobjcopy);
#endif
dtp->dt_provmod = provmod;
dtp->dt_vector = vector;
dtp->dt_varg = arg;
dt_dof_init(dtp);
(void) uname(&dtp->dt_uts);
if (dtp->dt_mods == NULL || dtp->dt_provs == NULL ||
dtp->dt_procs == NULL || dtp->dt_proc_env == NULL ||
dtp->dt_ld_path == NULL || dtp->dt_cpp_path == NULL ||
#if defined(__FreeBSD__)
dtp->dt_kmods == NULL ||
dtp->dt_objcopy_path == NULL ||
#endif
dtp->dt_cpp_argv == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
for (i = 0; i < DTRACEOPT_MAX; i++)
dtp->dt_options[i] = DTRACEOPT_UNSET;
dtp->dt_cpp_argv[0] = (char *)strbasename(dtp->dt_cpp_path);
#if defined(illumos)
(void) snprintf(isadef, sizeof (isadef), "-D__SUNW_D_%u",
(uint_t)(sizeof (void *) * NBBY));
(void) snprintf(utsdef, sizeof (utsdef), "-D__%s_%s",
dt_get_sysinfo(SI_SYSNAME, s1, sizeof (s1)),
dt_get_sysinfo(SI_RELEASE, s2, sizeof (s2)));
if (dt_cpp_add_arg(dtp, "-D__sun") == NULL ||
dt_cpp_add_arg(dtp, "-D__unix") == NULL ||
dt_cpp_add_arg(dtp, "-D__SVR4") == NULL ||
dt_cpp_add_arg(dtp, "-D__SUNW_D=1") == NULL ||
dt_cpp_add_arg(dtp, isadef) == NULL ||
dt_cpp_add_arg(dtp, utsdef) == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
#endif
if (flags & DTRACE_O_NODEV)
bcopy(&_dtrace_conf, &dtp->dt_conf, sizeof (_dtrace_conf));
else if (dt_ioctl(dtp, DTRACEIOC_CONF, &dtp->dt_conf) != 0)
return (set_open_errno(dtp, errp, errno));
if (flags & DTRACE_O_LP64)
dtp->dt_conf.dtc_ctfmodel = CTF_MODEL_LP64;
else if (flags & DTRACE_O_ILP32)
dtp->dt_conf.dtc_ctfmodel = CTF_MODEL_ILP32;
#if defined(__sparc)
if (dt_cpp_add_arg(dtp, "-D__sparc") == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
if (dtp->dt_conf.dtc_ctfmodel == CTF_MODEL_LP64 &&
dt_cpp_add_arg(dtp, "-D__sparcv9") == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
#endif
#if defined(illumos)
#if defined(__x86)
if (dtp->dt_conf.dtc_ctfmodel == CTF_MODEL_LP64) {
if (dt_cpp_add_arg(dtp, "-D__amd64") == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
} else {
if (dt_cpp_add_arg(dtp, "-D__i386") == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
}
#endif
#else
#if defined(__amd64__) || defined(__i386__)
if (dtp->dt_conf.dtc_ctfmodel == CTF_MODEL_LP64) {
if (dt_cpp_add_arg(dtp, "-m64") == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
} else {
if (dt_cpp_add_arg(dtp, "-m32") == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
}
#endif
#endif
if (dtp->dt_conf.dtc_difversion < DIF_VERSION)
return (set_open_errno(dtp, errp, EDT_DIFVERS));
if (dtp->dt_conf.dtc_ctfmodel == CTF_MODEL_ILP32)
bcopy(_dtrace_ints_32, dtp->dt_ints, sizeof (_dtrace_ints_32));
else
bcopy(_dtrace_ints_64, dtp->dt_ints, sizeof (_dtrace_ints_64));
#if !defined(illumos)
{
char bootfile[MAXPATHLEN];
char *p;
int i;
size_t len = sizeof(bootfile);
if (sysctlbyname("kern.bootfile", bootfile, &len, NULL, 0) != 0)
strlcpy(bootfile, "kernel", sizeof(bootfile));
if ((p = strrchr(bootfile, '/')) != NULL)
p++;
else
p = bootfile;
snprintf(curthread_str, sizeof(curthread_str), "%s`struct thread *",p);
snprintf(intmtx_str, sizeof(intmtx_str), "int(%s`struct mtx *)",p);
snprintf(threadmtx_str, sizeof(threadmtx_str), "struct thread *(%s`struct mtx *)",p);
snprintf(rwlock_str, sizeof(rwlock_str), "int(%s`struct rwlock *)",p);
snprintf(sxlock_str, sizeof(sxlock_str), "int(%s`struct sx *)",p);
}
#endif
dtp->dt_macros = dt_idhash_create("macro", NULL, 0, UINT_MAX);
dtp->dt_aggs = dt_idhash_create("aggregation", NULL,
DTRACE_AGGVARIDNONE + 1, UINT_MAX);
dtp->dt_globals = dt_idhash_create("global", _dtrace_globals,
DIF_VAR_OTHER_UBASE, DIF_VAR_OTHER_MAX);
dtp->dt_tls = dt_idhash_create("thread local", NULL,
DIF_VAR_OTHER_UBASE, DIF_VAR_OTHER_MAX);
if (dtp->dt_macros == NULL || dtp->dt_aggs == NULL ||
dtp->dt_globals == NULL || dtp->dt_tls == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
for (idp = _dtrace_macros; idp->di_name != NULL; idp++) {
if (dt_idhash_insert(dtp->dt_macros, idp->di_name,
idp->di_kind, idp->di_flags, idp->di_id, idp->di_attr,
idp->di_vers, idp->di_ops ? idp->di_ops : &dt_idops_thaw,
idp->di_iarg, 0) == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
}
dtrace_update(dtp);
if (dtp->dt_conf.dtc_ctfmodel == CTF_MODEL_ILP32) {
dinp = _dtrace_intrinsics_32;
dtyp = _dtrace_typedefs_32;
} else {
dinp = _dtrace_intrinsics_64;
dtyp = _dtrace_typedefs_64;
}
if ((dmp = dtp->dt_cdefs = dt_module_create(dtp, "C")) == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
if ((dmp->dm_ctfp = ctf_create(&dtp->dt_ctferr)) == NULL)
return (set_open_errno(dtp, errp, EDT_CTF));
dt_dprintf("created CTF container for %s (%p)\n",
dmp->dm_name, (void *)dmp->dm_ctfp);
(void) ctf_setmodel(dmp->dm_ctfp, dtp->dt_conf.dtc_ctfmodel);
ctf_setspecific(dmp->dm_ctfp, dmp);
dmp->dm_flags = DT_DM_LOADED;
dmp->dm_modid = -1;
for (; dinp->din_name != NULL; dinp++) {
if (dinp->din_kind == CTF_K_INTEGER) {
err = ctf_add_integer(dmp->dm_ctfp, CTF_ADD_ROOT,
dinp->din_name, &dinp->din_data);
} else {
err = ctf_add_float(dmp->dm_ctfp, CTF_ADD_ROOT,
dinp->din_name, &dinp->din_data);
}
if (err == CTF_ERR) {
dt_dprintf("failed to add %s to C container: %s\n",
dinp->din_name, ctf_errmsg(
ctf_errno(dmp->dm_ctfp)));
return (set_open_errno(dtp, errp, EDT_CTF));
}
}
if (ctf_update(dmp->dm_ctfp) != 0) {
dt_dprintf("failed to update C container: %s\n",
ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
return (set_open_errno(dtp, errp, EDT_CTF));
}
(void) ctf_add_pointer(dmp->dm_ctfp, CTF_ADD_ROOT,
ctf_lookup_by_name(dmp->dm_ctfp, "void"));
(void) ctf_add_pointer(dmp->dm_ctfp, CTF_ADD_ROOT,
ctf_lookup_by_name(dmp->dm_ctfp, "char"));
(void) ctf_add_pointer(dmp->dm_ctfp, CTF_ADD_ROOT,
ctf_lookup_by_name(dmp->dm_ctfp, "int"));
if (ctf_update(dmp->dm_ctfp) != 0) {
dt_dprintf("failed to update C container: %s\n",
ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
return (set_open_errno(dtp, errp, EDT_CTF));
}
if ((dmp = dtp->dt_ddefs = dt_module_create(dtp, "D")) == NULL)
return (set_open_errno(dtp, errp, EDT_NOMEM));
if ((dmp->dm_ctfp = ctf_create(&dtp->dt_ctferr)) == NULL)
return (set_open_errno(dtp, errp, EDT_CTF));
dt_dprintf("created CTF container for %s (%p)\n",
dmp->dm_name, (void *)dmp->dm_ctfp);
(void) ctf_setmodel(dmp->dm_ctfp, dtp->dt_conf.dtc_ctfmodel);
ctf_setspecific(dmp->dm_ctfp, dmp);
dmp->dm_flags = DT_DM_LOADED;
dmp->dm_modid = -1;
if (ctf_import(dmp->dm_ctfp, dtp->dt_cdefs->dm_ctfp) == CTF_ERR) {
dt_dprintf("failed to import D parent container: %s\n",
ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
return (set_open_errno(dtp, errp, EDT_CTF));
}
for (; dtyp->dty_src != NULL; dtyp++) {
if (ctf_add_typedef(dmp->dm_ctfp, CTF_ADD_ROOT,
dtyp->dty_dst, ctf_lookup_by_name(dmp->dm_ctfp,
dtyp->dty_src)) == CTF_ERR) {
dt_dprintf("failed to add typedef %s %s to D "
"container: %s", dtyp->dty_src, dtyp->dty_dst,
ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
return (set_open_errno(dtp, errp, EDT_CTF));
}
}
ctc.ctc_return = ctf_lookup_by_name(dmp->dm_ctfp, "int");
ctc.ctc_argc = 0;
ctc.ctc_flags = 0;
dtp->dt_type_func = ctf_add_function(dmp->dm_ctfp,
CTF_ADD_ROOT, &ctc, NULL);
dtp->dt_type_fptr = ctf_add_pointer(dmp->dm_ctfp,
CTF_ADD_ROOT, dtp->dt_type_func);
ctr.ctr_contents = ctf_lookup_by_name(dmp->dm_ctfp, "char");
ctr.ctr_index = ctf_lookup_by_name(dmp->dm_ctfp, "long");
ctr.ctr_nelems = _dtrace_strsize;
dtp->dt_type_str = ctf_add_typedef(dmp->dm_ctfp, CTF_ADD_ROOT,
"string", ctf_add_array(dmp->dm_ctfp, CTF_ADD_ROOT, &ctr));
dtp->dt_type_dyn = ctf_add_typedef(dmp->dm_ctfp, CTF_ADD_ROOT,
"<DYN>", ctf_lookup_by_name(dmp->dm_ctfp, "void"));
dtp->dt_type_stack = ctf_add_typedef(dmp->dm_ctfp, CTF_ADD_ROOT,
"stack", ctf_lookup_by_name(dmp->dm_ctfp, "void"));
dtp->dt_type_symaddr = ctf_add_typedef(dmp->dm_ctfp, CTF_ADD_ROOT,
"_symaddr", ctf_lookup_by_name(dmp->dm_ctfp, "void"));
dtp->dt_type_usymaddr = ctf_add_typedef(dmp->dm_ctfp, CTF_ADD_ROOT,
"_usymaddr", ctf_lookup_by_name(dmp->dm_ctfp, "void"));
if (dtp->dt_type_func == CTF_ERR || dtp->dt_type_fptr == CTF_ERR ||
dtp->dt_type_str == CTF_ERR || dtp->dt_type_dyn == CTF_ERR ||
dtp->dt_type_stack == CTF_ERR || dtp->dt_type_symaddr == CTF_ERR ||
dtp->dt_type_usymaddr == CTF_ERR) {
dt_dprintf("failed to add intrinsic to D container: %s\n",
ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
return (set_open_errno(dtp, errp, EDT_CTF));
}
if (ctf_update(dmp->dm_ctfp) != 0) {
dt_dprintf("failed update D container: %s\n",
ctf_errmsg(ctf_errno(dmp->dm_ctfp)));
return (set_open_errno(dtp, errp, EDT_CTF));
}
for (i = 0; i < sizeof (dtp->dt_ints) / sizeof (dtp->dt_ints[0]); i++) {
if (dtrace_lookup_by_type(dtp, DTRACE_OBJ_EVERY,
dtp->dt_ints[i].did_name, &dtt) != 0) {
dt_dprintf("failed to lookup integer type %s: %s\n",
dtp->dt_ints[i].did_name,
dtrace_errmsg(dtp, dtrace_errno(dtp)));
return (set_open_errno(dtp, errp, dtp->dt_errno));
}
dtp->dt_ints[i].did_ctfp = dtt.dtt_ctfp;
dtp->dt_ints[i].did_type = dtt.dtt_type;
}
dt_list_delete(&dtp->dt_modlist, dtp->dt_ddefs);
dt_list_prepend(&dtp->dt_modlist, dtp->dt_ddefs);
dt_list_delete(&dtp->dt_modlist, dtp->dt_cdefs);
dt_list_prepend(&dtp->dt_modlist, dtp->dt_cdefs);
if (dt_pfdict_create(dtp) == -1)
return (set_open_errno(dtp, errp, dtp->dt_errno));
if (flags & DTRACE_O_NODEV)
dtp->dt_cflags |= DTRACE_C_ZDEFS;
if ((pgp = dtrace_program_strcompile(dtp, _dtrace_hardwire,
DTRACE_PROBESPEC_NONE, DTRACE_C_EMPTY, 0, NULL)) == NULL) {
dt_dprintf("failed to load hard-wired definitions: %s\n",
dtrace_errmsg(dtp, dtrace_errno(dtp)));
return (set_open_errno(dtp, errp, EDT_HARDWIRE));
}
dt_program_destroy(dtp, pgp);
#if defined(__FreeBSD__)
#if defined(__LP64__)
if ((dtp->dt_oflags & DTRACE_O_ILP32) != 0) {
if (dtrace_setopt(dtp, "libdir", _dtrace_libdir32) != 0)
return (set_open_errno(dtp, errp, dtp->dt_errno));
}
#endif
if (dtrace_setopt(dtp, "libdir", _dtrace_libdir) != 0)
return (set_open_errno(dtp, errp, dtp->dt_errno));
#else
if (dtrace_setopt(dtp, "libdir", _dtrace_libdir) != 0)
return (set_open_errno(dtp, errp, dtp->dt_errno));
#endif
return (dtp);
}
dtrace_hdl_t *
dtrace_open(int version, int flags, int *errp)
{
return (dt_vopen(version, flags, errp, NULL, NULL));
}
dtrace_hdl_t *
dtrace_vopen(int version, int flags, int *errp,
const dtrace_vector_t *vector, void *arg)
{
return (dt_vopen(version, flags, errp, vector, arg));
}
void
dtrace_close(dtrace_hdl_t *dtp)
{
dt_ident_t *idp, *ndp;
dt_module_t *dmp;
dt_provider_t *pvp;
dtrace_prog_t *pgp;
dt_xlator_t *dxp;
dt_dirpath_t *dirp;
#if defined(__FreeBSD__)
dt_kmodule_t *dkm;
uint_t h;
#endif
int i;
if (dtp->dt_procs != NULL)
dt_proc_fini(dtp);
while ((pgp = dt_list_next(&dtp->dt_programs)) != NULL)
dt_program_destroy(dtp, pgp);
while ((dxp = dt_list_next(&dtp->dt_xlators)) != NULL)
dt_xlator_destroy(dtp, dxp);
dt_free(dtp, dtp->dt_xlatormap);
for (idp = dtp->dt_externs; idp != NULL; idp = ndp) {
ndp = idp->di_next;
dt_ident_destroy(idp);
}
if (dtp->dt_macros != NULL)
dt_idhash_destroy(dtp->dt_macros);
if (dtp->dt_aggs != NULL)
dt_idhash_destroy(dtp->dt_aggs);
if (dtp->dt_globals != NULL)
dt_idhash_destroy(dtp->dt_globals);
if (dtp->dt_tls != NULL)
dt_idhash_destroy(dtp->dt_tls);
#if defined(__FreeBSD__)
for (h = 0; h < dtp->dt_modbuckets; h++)
while ((dkm = dtp->dt_kmods[h]) != NULL) {
dtp->dt_kmods[h] = dkm->dkm_next;
free(dkm->dkm_name);
free(dkm);
}
#endif
while ((dmp = dt_list_next(&dtp->dt_modlist)) != NULL)
dt_module_destroy(dtp, dmp);
while ((pvp = dt_list_next(&dtp->dt_provlist)) != NULL)
dt_provider_destroy(dtp, pvp);
if (dtp->dt_fd != -1)
(void) close(dtp->dt_fd);
if (dtp->dt_ftfd != -1)
(void) close(dtp->dt_ftfd);
if (dtp->dt_cdefs_fd != -1)
(void) close(dtp->dt_cdefs_fd);
if (dtp->dt_ddefs_fd != -1)
(void) close(dtp->dt_ddefs_fd);
#if defined(illumos)
if (dtp->dt_stdout_fd != -1)
(void) close(dtp->dt_stdout_fd);
#else
if (dtp->dt_freopen_fp != NULL)
(void) fclose(dtp->dt_freopen_fp);
#endif
dt_epid_destroy(dtp);
dt_aggid_destroy(dtp);
dt_format_destroy(dtp);
dt_strdata_destroy(dtp);
dt_buffered_destroy(dtp);
dt_aggregate_destroy(dtp);
dt_pfdict_destroy(dtp);
dt_provmod_destroy(&dtp->dt_provmod);
dt_dof_fini(dtp);
for (i = 1; i < dtp->dt_cpp_argc; i++)
free(dtp->dt_cpp_argv[i]);
while ((dirp = dt_list_next(&dtp->dt_lib_path)) != NULL) {
dt_list_delete(&dtp->dt_lib_path, dirp);
free(dirp->dir_path);
free(dirp);
}
free(dtp->dt_cpp_argv);
free(dtp->dt_cpp_path);
free(dtp->dt_ld_path);
#if defined(__FreeBSD__)
free(dtp->dt_objcopy_path);
#endif
free(dtp->dt_mods);
#if defined(__FreeBSD__)
free(dtp->dt_kmods);
#endif
free(dtp->dt_provs);
free(dtp);
}
int
dtrace_provider_modules(dtrace_hdl_t *dtp, const char **mods, int nmods)
{
dt_provmod_t *prov;
int i = 0;
for (prov = dtp->dt_provmod; prov != NULL; prov = prov->dp_next, i++) {
if (i < nmods)
mods[i] = prov->dp_name;
}
return (i);
}
int
dtrace_ctlfd(dtrace_hdl_t *dtp)
{
return (dtp->dt_fd);
}
