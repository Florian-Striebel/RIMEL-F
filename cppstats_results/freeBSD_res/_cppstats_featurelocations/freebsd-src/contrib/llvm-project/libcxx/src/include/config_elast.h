







#if !defined(_LIBCPP_CONFIG_ELAST)
#define _LIBCPP_CONFIG_ELAST

#include <__config>

#if defined(_LIBCPP_MSVCRT_LIKE)
#include <stdlib.h>
#else
#include <errno.h>
#endif



#if defined(ELAST)
#define _LIBCPP_ELAST ELAST
#elif defined(_NEWLIB_VERSION)
#define _LIBCPP_ELAST __ELASTERROR
#elif defined(__NuttX__)

#elif defined(__Fuchsia__)

#elif defined(__wasi__)

#elif defined(__linux__) || defined(_LIBCPP_HAS_MUSL_LIBC)
#define _LIBCPP_ELAST 4095
#elif defined(__APPLE__)

#elif defined(__sun__)
#define _LIBCPP_ELAST ESTALE
#elif defined(__MVS__)
#define _LIBCPP_ELAST 1160
#elif defined(_LIBCPP_MSVCRT_LIKE)
#define _LIBCPP_ELAST (_sys_nerr - 1)
#elif defined(_AIX)
#define _LIBCPP_ELAST 127
#else

#warning ELAST for this platform not yet implemented
#endif

#endif
