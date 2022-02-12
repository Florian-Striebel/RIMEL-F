












#if !defined(ASAN_INIT_VERSION_H)
#define ASAN_INIT_VERSION_H

#include "sanitizer_common/sanitizer_platform.h"

extern "C" {
















#if SANITIZER_WORDSIZE == 32 && SANITIZER_ANDROID

#define __asan_version_mismatch_check __asan_version_mismatch_check_v9
#else
#define __asan_version_mismatch_check __asan_version_mismatch_check_v8
#endif
}

#endif
