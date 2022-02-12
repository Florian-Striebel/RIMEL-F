












#if !defined(LLVM_CLANG_C_PLATFORM_H)
#define LLVM_CLANG_C_PLATFORM_H

#include "clang-c/ExternC.h"

LLVM_CLANG_C_EXTERN_C_BEGIN


#if !defined(CINDEX_NO_EXPORTS)
#define CINDEX_EXPORTS
#endif
#if defined(_WIN32)
#if defined(CINDEX_EXPORTS)
#if defined(_CINDEX_LIB_)
#define CINDEX_LINKAGE __declspec(dllexport)
#else
#define CINDEX_LINKAGE __declspec(dllimport)
#endif
#endif
#elif defined(CINDEX_EXPORTS) && defined(__GNUC__)
#define CINDEX_LINKAGE __attribute__((visibility("default")))
#endif

#if !defined(CINDEX_LINKAGE)
#define CINDEX_LINKAGE
#endif

#if defined(__GNUC__)
#define CINDEX_DEPRECATED __attribute__((deprecated))
#else
#if defined(_MSC_VER)
#define CINDEX_DEPRECATED __declspec(deprecated)
#else
#define CINDEX_DEPRECATED
#endif
#endif

LLVM_CLANG_C_EXTERN_C_END

#endif
