







#if !defined(SCUDO_INTERNAL_DEFS_H_)
#define SCUDO_INTERNAL_DEFS_H_

#include "platform.h"

#include <stdint.h>

#if !defined(SCUDO_DEBUG)
#define SCUDO_DEBUG 0
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))



#define STRINGIFY_(S) #S
#define STRINGIFY(S) STRINGIFY_(S)
#define CONCATENATE_(S, C) S##C
#define CONCATENATE(S, C) CONCATENATE_(S, C)



#define INTERFACE __attribute__((visibility("default")))
#define HIDDEN __attribute__((visibility("hidden")))
#define WEAK __attribute__((weak))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define ALIAS(X) __attribute__((alias(X)))
#define FORMAT(F, A) __attribute__((format(printf, F, A)))
#define NOINLINE __attribute__((noinline))
#define NORETURN __attribute__((noreturn))
#define LIKELY(X) __builtin_expect(!!(X), 1)
#define UNLIKELY(X) __builtin_expect(!!(X), 0)
#if defined(__i386__) || defined(__x86_64__)

#define PREFETCH(X) __asm__("prefetchnta (%0)" : : "r"(X))
#else
#define PREFETCH(X) __builtin_prefetch(X)
#endif
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
#define NOEXCEPT noexcept




















#if defined(__has_attribute)
#if __has_attribute(require_constant_initialization)
#define SCUDO_REQUIRE_CONSTANT_INITIALIZATION __attribute__((__require_constant_initialization__))

#else
#define SCUDO_REQUIRE_CONSTANT_INITIALIZATION
#endif
#endif

namespace scudo {

typedef unsigned long uptr;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed long sptr;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;


void outputRaw(const char *Buffer);
void NORETURN die();

#define RAW_CHECK_MSG(Expr, Msg) do { if (UNLIKELY(!(Expr))) { outputRaw(Msg); die(); } } while (false)







#define RAW_CHECK(Expr) RAW_CHECK_MSG(Expr, #Expr)

void NORETURN reportCheckFailed(const char *File, int Line,
const char *Condition, u64 Value1, u64 Value2);
#define CHECK_IMPL(C1, Op, C2) do { if (UNLIKELY(!(C1 Op C2))) { scudo::reportCheckFailed(__FILE__, __LINE__, #C1 " " #Op " " #C2, (scudo::u64)C1, (scudo::u64)C2); scudo::die(); } } while (false)








#define CHECK(A) CHECK_IMPL((A), !=, 0)
#define CHECK_EQ(A, B) CHECK_IMPL((A), ==, (B))
#define CHECK_NE(A, B) CHECK_IMPL((A), !=, (B))
#define CHECK_LT(A, B) CHECK_IMPL((A), <, (B))
#define CHECK_LE(A, B) CHECK_IMPL((A), <=, (B))
#define CHECK_GT(A, B) CHECK_IMPL((A), >, (B))
#define CHECK_GE(A, B) CHECK_IMPL((A), >=, (B))

#if SCUDO_DEBUG
#define DCHECK(A) CHECK(A)
#define DCHECK_EQ(A, B) CHECK_EQ(A, B)
#define DCHECK_NE(A, B) CHECK_NE(A, B)
#define DCHECK_LT(A, B) CHECK_LT(A, B)
#define DCHECK_LE(A, B) CHECK_LE(A, B)
#define DCHECK_GT(A, B) CHECK_GT(A, B)
#define DCHECK_GE(A, B) CHECK_GE(A, B)
#else
#define DCHECK(A) do { } while (false)


#define DCHECK_EQ(A, B) do { } while (false)


#define DCHECK_NE(A, B) do { } while (false)


#define DCHECK_LT(A, B) do { } while (false)


#define DCHECK_LE(A, B) do { } while (false)


#define DCHECK_GT(A, B) do { } while (false)


#define DCHECK_GE(A, B) do { } while (false)


#endif


#define UNREACHABLE(Msg) do { CHECK(0 && Msg); die(); } while (0)





}

#endif
