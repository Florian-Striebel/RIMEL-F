

























#include <sys/wait.h>

#if defined(sun386)
#define UNION_WAIT
#define WEXITSTATUS(x) ((&x)->w_retcode)
#define WTERMSIG(x) ((&x)->w_termsig)
#define WSTOPSIG(x) ((&x)->w_stopsig)
#define HAVE_WAIT4
#endif

#if !defined(WAIT_T)
#if defined(UNION_WAIT)
#define WAIT_T union wait
#define WAIT_STATUS(x) (x).w_status
#else
#define WAIT_T int
#define WAIT_STATUS(x) x
#endif
#endif

#if !defined(WEXITSTATUS)
#define WEXITSTATUS(_X) (((int)(_X)>>8)&0377)
#endif
#if !defined(WSTOPPED)
#define WSTOPPED 0177
#endif
#if !defined(WSTOPSIG)
#define WSTOPSIG(x) WSTOPPED
#endif

#if defined(UNION_WAIT)
#if !defined(WSET_STOPCODE)
#define WSET_STOPCODE(x, sig) ((&x)->w_stopsig = (sig))
#endif
#if !defined(WSET_EXITCODE)
#define WSET_EXITCODE(x, ret, sig) ((&x)->w_termsig = (sig), (&x)->w_retcode = (ret))
#endif
#else
#if !defined(WSET_STOPCODE)
#define WSET_STOPCODE(x, sig) ((x) = ((sig) << 8) | 0177)
#endif
#if !defined(WSET_EXITCODE)
#define WSET_EXITCODE(x, ret, sig) ((x) = (ret << 8) | (sig))
#endif
#endif

#if !defined(HAVE_WAITPID)
#if defined(HAVE_WAIT4)
#define waitpid(pid, statusp, flags) wait4(pid, statusp, flags, (char *)0)
#else
#if defined(HAVE_WAIT3)
#define waitpid(pid, statusp, flags) wait3(statusp, flags, (char *)0)
#endif
#endif
#endif
