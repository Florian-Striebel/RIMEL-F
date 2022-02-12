











#if !defined(SANITIZER_GLIBC_VERSION_H)
#define SANITIZER_GLIBC_VERSION_H

#include "sanitizer_platform.h"

#if SANITIZER_LINUX || SANITIZER_FUCHSIA
#include <features.h>
#endif

#if !defined(__GLIBC_PREREQ)
#define __GLIBC_PREREQ(x, y) 0
#endif

#endif
