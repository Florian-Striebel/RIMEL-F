
















#if !defined(SANITIZER_ERRNO_H)
#define SANITIZER_ERRNO_H

#include "sanitizer_errno_codes.h"
#include "sanitizer_platform.h"

#if SANITIZER_FREEBSD || SANITIZER_MAC
#define __errno_location __error
#elif SANITIZER_ANDROID || SANITIZER_NETBSD
#define __errno_location __errno
#elif SANITIZER_SOLARIS
#define __errno_location ___errno
#elif SANITIZER_WINDOWS
#define __errno_location _errno
#endif

extern "C" int *__errno_location();

#define errno (*__errno_location())

#endif
