#include <math.h>
#if defined(_WIN32)
#include <windows.h>
#include "msvc_compat/windows_extra.h"
#if defined(_WIN64)
#if LG_VADDR <= 32
#error Generate the headers using x64 vcargs
#endif
#else
#if LG_VADDR > 32
#undef LG_VADDR
#define LG_VADDR 32
#endif
#endif
#else
#include <sys/param.h>
#include <sys/mman.h>
#if !defined(__pnacl__) && !defined(__native_client__)
#include <sys/syscall.h>
#if !defined(SYS_write) && defined(__NR_write)
#define SYS_write __NR_write
#endif
#if defined(SYS_open) && defined(__aarch64__)
#undef SYS_open
#endif
#include <sys/uio.h>
#endif
#include <pthread.h>
#if defined(__FreeBSD__)
#include <pthread_np.h>
#endif
#include <signal.h>
#if defined(JEMALLOC_OS_UNFAIR_LOCK)
#include <os/lock.h>
#endif
#if defined(JEMALLOC_GLIBC_MALLOC_HOOK)
#include <sched.h>
#endif
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#if defined(JEMALLOC_HAVE_MACH_ABSOLUTE_TIME)
#include <mach/mach_time.h>
#endif
#endif
#include <sys/types.h>
#include <limits.h>
#if !defined(SIZE_T_MAX)
#define SIZE_T_MAX SIZE_MAX
#endif
#if !defined(SSIZE_MAX)
#define SSIZE_MAX ((ssize_t)(SIZE_T_MAX >> 1))
#endif
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#if !defined(offsetof)
#define offsetof(type, member) ((size_t)&(((type *)NULL)->member))
#endif
#include <string.h>
#include <strings.h>
#include <ctype.h>
#if defined(_MSC_VER)
#include <io.h>
typedef intptr_t ssize_t;
#define PATH_MAX 1024
#define STDERR_FILENO 2
#define __func__ __FUNCTION__
#if defined(JEMALLOC_HAS_RESTRICT)
#define restrict __restrict
#endif
#pragma warning(disable: 4996)
#if _MSC_VER < 1800
static int
isblank(int c) {
return (c == '\t' || c == ' ');
}
#endif
#else
#include <unistd.h>
#endif
#include <fcntl.h>
