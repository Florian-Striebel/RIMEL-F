




































#if !defined(ARCHIVE_PLATFORM_H_INCLUDED)
#define ARCHIVE_PLATFORM_H_INCLUDED


#define __LIBARCHIVE_BUILD 1

#if defined(PLATFORM_CONFIG_H)

#include PLATFORM_CONFIG_H
#elif defined(HAVE_CONFIG_H)

#include "config.h"
#else

#error Oops: No config.h and no pre-built configuration in archive_platform.h.
#endif


#if defined(__APPLE__)
#undef HAVE_FUTIMENS
#undef HAVE_UTIMENSAT
#include <AvailabilityMacros.h>
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 101300
#define HAVE_FUTIMENS 1
#define HAVE_UTIMENSAT 1
#endif
#endif





#if (defined(__WIN32__) || defined(_WIN32) || defined(__WIN32)) && !defined(__CYGWIN__)
#include "archive_windows.h"
#else
#define la_stat(path,stref) stat(path,stref)
#endif








#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif


#if !defined(__FBSDID)
#define __FBSDID(a) struct _undefined_hack
#endif
#if !defined(__RCSID)
#define __RCSID(a) struct _undefined_hack
#endif


#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#if HAVE_STDINT_H
#include <stdint.h>
#endif


#if defined(__BORLANDC__)
#if HAVE_DECL_UINT64_MAX
#undef UINT64_MAX
#undef HAVE_DECL_UINT64_MAX
#endif
#if HAVE_DECL_UINT64_MIN
#undef UINT64_MIN
#undef HAVE_DECL_UINT64_MIN
#endif
#if HAVE_DECL_INT64_MAX
#undef INT64_MAX
#undef HAVE_DECL_INT64_MAX
#endif
#if HAVE_DECL_INT64_MIN
#undef INT64_MIN
#undef HAVE_DECL_INT64_MIN
#endif
#endif


#if !HAVE_DECL_SIZE_MAX
#define SIZE_MAX (~(size_t)0)
#endif
#if !HAVE_DECL_SSIZE_MAX
#define SSIZE_MAX ((ssize_t)(SIZE_MAX >> 1))
#endif
#if !HAVE_DECL_UINT32_MAX
#define UINT32_MAX (~(uint32_t)0)
#endif
#if !HAVE_DECL_INT32_MAX
#define INT32_MAX ((int32_t)(UINT32_MAX >> 1))
#endif
#if !HAVE_DECL_INT32_MIN
#define INT32_MIN ((int32_t)(~INT32_MAX))
#endif
#if !HAVE_DECL_UINT64_MAX
#define UINT64_MAX (~(uint64_t)0)
#endif
#if !HAVE_DECL_INT64_MAX
#define INT64_MAX ((int64_t)(UINT64_MAX >> 1))
#endif
#if !HAVE_DECL_INT64_MIN
#define INT64_MIN ((int64_t)(~INT64_MAX))
#endif
#if !HAVE_DECL_UINTMAX_MAX
#define UINTMAX_MAX (~(uintmax_t)0)
#endif
#if !HAVE_DECL_INTMAX_MAX
#define INTMAX_MAX ((intmax_t)(UINTMAX_MAX >> 1))
#endif
#if !HAVE_DECL_INTMAX_MIN
#define INTMAX_MIN ((intmax_t)(~INTMAX_MAX))
#endif





#if defined(HAVE_FCHMOD) || defined(HAVE_FUTIMES) || defined(HAVE_ACL_SET_FD) || defined(HAVE_ACL_SET_FD_NP) || defined(HAVE_FCHOWN)
#define CAN_RESTORE_METADATA_FD
#endif




#if defined(HAVE_READDIR_R) && (!defined(__GLIBC__) || !defined(__GLIBC_MINOR__) || __GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 24))
#define USE_READDIR_R 1
#else
#undef USE_READDIR_R
#endif


#if !defined(ARCHIVE_ERRNO_FILE_FORMAT)
#if HAVE_EFTYPE
#define ARCHIVE_ERRNO_FILE_FORMAT EFTYPE
#else
#if HAVE_EILSEQ
#define ARCHIVE_ERRNO_FILE_FORMAT EILSEQ
#else
#define ARCHIVE_ERRNO_FILE_FORMAT EINVAL
#endif
#endif
#endif

#if !defined(ARCHIVE_ERRNO_PROGRAMMER)
#define ARCHIVE_ERRNO_PROGRAMMER EINVAL
#endif

#if !defined(ARCHIVE_ERRNO_MISC)
#define ARCHIVE_ERRNO_MISC (-1)
#endif

#if defined(__GNUC__) && (__GNUC__ >= 7)
#define __LA_FALLTHROUGH __attribute__((fallthrough))
#else
#define __LA_FALLTHROUGH
#endif

#endif
