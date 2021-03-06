










#if !defined(SANITIZER_DEFS_H)
#define SANITIZER_DEFS_H

#include "sanitizer_platform.h"

#if !defined(SANITIZER_DEBUG)
#define SANITIZER_DEBUG 0
#endif

#define SANITIZER_STRINGIFY_(S) #S
#define SANITIZER_STRINGIFY(S) SANITIZER_STRINGIFY_(S)


#if SANITIZER_WINDOWS
#if SANITIZER_IMPORT_INTERFACE
#define SANITIZER_INTERFACE_ATTRIBUTE __declspec(dllimport)
#else
#define SANITIZER_INTERFACE_ATTRIBUTE __declspec(dllexport)
#endif
#define SANITIZER_WEAK_ATTRIBUTE
#elif SANITIZER_GO
#define SANITIZER_INTERFACE_ATTRIBUTE
#define SANITIZER_WEAK_ATTRIBUTE
#else
#define SANITIZER_INTERFACE_ATTRIBUTE __attribute__((visibility("default")))
#define SANITIZER_WEAK_ATTRIBUTE __attribute__((weak))
#endif


#if SANITIZER_LINUX || SANITIZER_NETBSD || SANITIZER_FREEBSD

#define SANITIZER_TLS_INITIAL_EXEC_ATTRIBUTE __attribute__((tls_model("initial-exec"))) thread_local

#else
#define SANITIZER_TLS_INITIAL_EXEC_ATTRIBUTE
#endif










#if SANITIZER_WINDOWS
#include "sanitizer_win_defs.h"
#define SANITIZER_INTERFACE_WEAK_DEF(ReturnType, Name, ...) WIN_WEAK_EXPORT_DEF(ReturnType, Name, __VA_ARGS__)

#else
#define SANITIZER_INTERFACE_WEAK_DEF(ReturnType, Name, ...) extern "C" SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE ReturnType Name(__VA_ARGS__)


#endif



#if !defined(SANITIZER_SUPPORTS_WEAK_HOOKS)
#if (SANITIZER_LINUX || SANITIZER_SOLARIS) && !SANITIZER_GO
#define SANITIZER_SUPPORTS_WEAK_HOOKS 1



#elif SANITIZER_MAC && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1090 && !SANITIZER_GO

#define SANITIZER_SUPPORTS_WEAK_HOOKS 1
#else
#define SANITIZER_SUPPORTS_WEAK_HOOKS 0
#endif
#endif























#if !defined(SANITIZER_CAN_USE_PREINIT_ARRAY)
#if (SANITIZER_LINUX || SANITIZER_FUCHSIA || SANITIZER_NETBSD) && !defined(PIC)
#define SANITIZER_CAN_USE_PREINIT_ARRAY 1


#elif SANITIZER_SOLARIS && !defined(PIC)
#define SANITIZER_CAN_USE_PREINIT_ARRAY 1
#else
#define SANITIZER_CAN_USE_PREINIT_ARRAY 0
#endif
#endif


#if !defined(__has_feature)
#define __has_feature(x) 0
#endif


#if !defined(__has_attribute)
#define __has_attribute(x) 0
#endif




namespace __sanitizer {

#if defined(_WIN64)

typedef unsigned long long uptr;
typedef signed long long sptr;
#else
typedef unsigned long uptr;
typedef signed long sptr;
#endif
#if defined(__x86_64__)


typedef unsigned long long uhwptr;
#else
typedef uptr uhwptr;
#endif
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
#if SANITIZER_WINDOWS


typedef void* fd_t;
typedef unsigned error_t;
#else
typedef int fd_t;
typedef int error_t;
#endif
#if SANITIZER_SOLARIS && !defined(_LP64)
typedef long pid_t;
#else
typedef int pid_t;
#endif

#if SANITIZER_FREEBSD || SANITIZER_NETBSD || SANITIZER_MAC || (SANITIZER_SOLARIS && (defined(_LP64) || _FILE_OFFSET_BITS == 64)) || (SANITIZER_LINUX && defined(__x86_64__))



typedef u64 OFF_T;
#else
typedef uptr OFF_T;
#endif
typedef u64 OFF64_T;

#if (SANITIZER_WORDSIZE == 64) || SANITIZER_MAC
typedef uptr operator_new_size_type;
#else
#if defined(__s390__) && !defined(__s390x__)

typedef unsigned long operator_new_size_type;
#else
typedef u32 operator_new_size_type;
#endif
#endif

typedef u64 tid_t;





#define INTERFACE_ATTRIBUTE SANITIZER_INTERFACE_ATTRIBUTE
#define SANITIZER_WEAK_DEFAULT_IMPL extern "C" SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE NOINLINE

#define SANITIZER_WEAK_CXX_DEFAULT_IMPL extern "C++" SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE NOINLINE



#if defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline

#define ALIAS(x)
#define ALIGNED(x) __declspec(align(x))
#define FORMAT(f, a)
#define NOINLINE __declspec(noinline)
#define NORETURN __declspec(noreturn)
#define THREADLOCAL __declspec(thread)
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#define PREFETCH(x) (void)0
#define WARN_UNUSED_RESULT
#else
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define ALIAS(x) __attribute__((alias(x)))


#define ALIGNED(x) __attribute__((aligned(x)))
#define FORMAT(f, a) __attribute__((format(printf, f, a)))
#define NOINLINE __attribute__((noinline))
#define NORETURN __attribute__((noreturn))
#define THREADLOCAL __thread
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#if defined(__i386__) || defined(__x86_64__)

#define PREFETCH(x) __asm__("prefetchnta (%0)" : : "r" (x))
#else
#define PREFETCH(x) __builtin_prefetch(x)
#endif
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#endif

#if !defined(_MSC_VER) || defined(__clang__)
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
#else
#define UNUSED
#define USED
#endif

#if !defined(_MSC_VER) || defined(__clang__) || MSC_PREREQ(1900)
#define NOEXCEPT noexcept
#else
#define NOEXCEPT throw()
#endif


typedef ALIGNED(1) u16 uu16;
typedef ALIGNED(1) u32 uu32;
typedef ALIGNED(1) u64 uu64;
typedef ALIGNED(1) s16 us16;
typedef ALIGNED(1) s32 us32;
typedef ALIGNED(1) s64 us64;

#if SANITIZER_WINDOWS
}
typedef unsigned long DWORD;
namespace __sanitizer {
typedef DWORD thread_return_t;
#define THREAD_CALLING_CONV __stdcall
#else
typedef void* thread_return_t;
#define THREAD_CALLING_CONV
#endif
typedef thread_return_t (THREAD_CALLING_CONV *thread_callback_t)(void* arg);


void NORETURN Die();

void NORETURN CheckFailed(const char *file, int line, const char *cond,
u64 v1, u64 v2);


#define RAW_CHECK_MSG(expr, msg) do { if (UNLIKELY(!(expr))) { RawWrite(msg); Die(); } } while (0)






#define RAW_CHECK(expr) RAW_CHECK_MSG(expr, #expr)

#define CHECK_IMPL(c1, op, c2) do { __sanitizer::u64 v1 = (__sanitizer::u64)(c1); __sanitizer::u64 v2 = (__sanitizer::u64)(c2); if (UNLIKELY(!(v1 op v2))) __sanitizer::CheckFailed(__FILE__, __LINE__, "(" #c1 ") " #op " (" #c2 ")", v1, v2); } while (false)









#define CHECK(a) CHECK_IMPL((a), !=, 0)
#define CHECK_EQ(a, b) CHECK_IMPL((a), ==, (b))
#define CHECK_NE(a, b) CHECK_IMPL((a), !=, (b))
#define CHECK_LT(a, b) CHECK_IMPL((a), <, (b))
#define CHECK_LE(a, b) CHECK_IMPL((a), <=, (b))
#define CHECK_GT(a, b) CHECK_IMPL((a), >, (b))
#define CHECK_GE(a, b) CHECK_IMPL((a), >=, (b))

#if SANITIZER_DEBUG
#define DCHECK(a) CHECK(a)
#define DCHECK_EQ(a, b) CHECK_EQ(a, b)
#define DCHECK_NE(a, b) CHECK_NE(a, b)
#define DCHECK_LT(a, b) CHECK_LT(a, b)
#define DCHECK_LE(a, b) CHECK_LE(a, b)
#define DCHECK_GT(a, b) CHECK_GT(a, b)
#define DCHECK_GE(a, b) CHECK_GE(a, b)
#else
#define DCHECK(a)
#define DCHECK_EQ(a, b)
#define DCHECK_NE(a, b)
#define DCHECK_LT(a, b)
#define DCHECK_LE(a, b)
#define DCHECK_GT(a, b)
#define DCHECK_GE(a, b)
#endif

#define UNREACHABLE(msg) do { CHECK(0 && msg); Die(); } while (0)




#define UNIMPLEMENTED() UNREACHABLE("unimplemented")

#define COMPILER_CHECK(pred) static_assert(pred, "")

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))



#undef __INT64_C
#undef __UINT64_C
#if SANITIZER_WORDSIZE == 64
#define __INT64_C(c) c ##L
#define __UINT64_C(c) c ##UL
#else
#define __INT64_C(c) c ##LL
#define __UINT64_C(c) c ##ULL
#endif
#undef INT32_MIN
#define INT32_MIN (-2147483647-1)
#undef INT32_MAX
#define INT32_MAX (2147483647)
#undef UINT32_MAX
#define UINT32_MAX (4294967295U)
#undef INT64_MIN
#define INT64_MIN (-__INT64_C(9223372036854775807)-1)
#undef INT64_MAX
#define INT64_MAX (__INT64_C(9223372036854775807))
#undef UINT64_MAX
#define UINT64_MAX (__UINT64_C(18446744073709551615))
#undef UINTPTR_MAX
#if SANITIZER_WORDSIZE == 64
#define UINTPTR_MAX (18446744073709551615UL)
#else
#define UINTPTR_MAX (4294967295U)
#endif

enum LinkerInitialized { LINKER_INITIALIZED = 0 };

#if !defined(_MSC_VER) || defined(__clang__)
#if SANITIZER_S390_31
#define GET_CALLER_PC() (__sanitizer::uptr) __builtin_extract_return_addr(__builtin_return_address(0))

#else
#define GET_CALLER_PC() (__sanitizer::uptr) __builtin_return_address(0)
#endif
#define GET_CURRENT_FRAME() (__sanitizer::uptr) __builtin_frame_address(0)
inline void Trap() {
__builtin_trap();
}
#else
extern "C" void* _ReturnAddress(void);
extern "C" void* _AddressOfReturnAddress(void);
#pragma intrinsic(_ReturnAddress)
#pragma intrinsic(_AddressOfReturnAddress)
#define GET_CALLER_PC() (__sanitizer::uptr) _ReturnAddress()

#define GET_CURRENT_FRAME() (((__sanitizer::uptr)_AddressOfReturnAddress()) + sizeof(__sanitizer::uptr))


extern "C" void __ud2(void);
#pragma intrinsic(__ud2)
inline void Trap() {
__ud2();
}
#endif

#define HANDLE_EINTR(res, f) { int rverrno; do { res = (f); } while (internal_iserror(res, &rverrno) && rverrno == EINTR); }








#define ENABLE_FRAME_POINTER do { volatile __sanitizer::uptr enable_fp; enable_fp = GET_CURRENT_FRAME(); (void)enable_fp; } while (0)






constexpr u32 kInvalidTid = -1;
constexpr u32 kMainTid = 0;

}

namespace __asan {
using namespace __sanitizer;
}
namespace __dsan {
using namespace __sanitizer;
}
namespace __dfsan {
using namespace __sanitizer;
}
namespace __lsan {
using namespace __sanitizer;
}
namespace __msan {
using namespace __sanitizer;
}
namespace __hwasan {
using namespace __sanitizer;
}
namespace __tsan {
using namespace __sanitizer;
}
namespace __scudo {
using namespace __sanitizer;
}
namespace __ubsan {
using namespace __sanitizer;
}
namespace __xray {
using namespace __sanitizer;
}
namespace __interception {
using namespace __sanitizer;
}
namespace __hwasan {
using namespace __sanitizer;
}
namespace __memprof {
using namespace __sanitizer;
}

#endif
