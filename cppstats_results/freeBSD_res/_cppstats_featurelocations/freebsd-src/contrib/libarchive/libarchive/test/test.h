


























#define KNOWNREF "test_compat_gtar_1.tar.uu"
#define ENVBASE "LIBARCHIVE"
#undef PROGRAM
#define LIBRARY "libarchive"
#define EXTRA_DUMP(x) archive_error_string((struct archive *)(x))
#define EXTRA_ERRNO(x) archive_errno((struct archive *)(x))
#define EXTRA_VERSION archive_version_details()

#if defined(__GNUC__) && (__GNUC__ >= 7)
#define __LA_FALLTHROUGH __attribute__((fallthrough))
#else
#define __LA_FALLTHROUGH
#endif

#include "test_common.h"
