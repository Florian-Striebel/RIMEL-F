




























#if !defined(_COMPAT_OPENSOLARIS_UNISTD_H_)
#define _COMPAT_OPENSOLARIS_UNISTD_H_

#include_next <unistd.h>

#define fork1 fork
#define ftruncate64 ftruncate
#define pread64 pread

#endif
