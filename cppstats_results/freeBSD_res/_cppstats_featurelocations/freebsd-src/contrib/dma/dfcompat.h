#if !defined(DFCOMPAT_H)
#define DFCOMPAT_H

#define _GNU_SOURCE

#include <sys/types.h>

#if !defined(__DECONST)
#define __DECONST(type, var) ((type)(uintptr_t)(const void *)(var))
#endif

#if !defined(HAVE_STRLCPY)
size_t strlcpy(char *, const char *, size_t);
#endif

#if !defined(HAVE_REALLOCF)
void *reallocf(void *, size_t);
#endif

#if !defined(HAVE_GETPROGNAME)
const char *getprogname(void);
#endif

#endif
