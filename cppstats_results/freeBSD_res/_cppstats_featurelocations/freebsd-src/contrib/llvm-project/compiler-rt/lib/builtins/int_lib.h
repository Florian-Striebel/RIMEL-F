












#if !defined(INT_LIB_H)
#define INT_LIB_H







#if __ARM_EABI__
#if defined(COMPILER_RT_ARMHF_TARGET) || (!defined(__clang__) && defined(__GNUC__) && (__GNUC__ < 4 || __GNUC__ == 4 && __GNUC_MINOR__ < 5))


#define COMPILER_RT_ABI
#else
#define COMPILER_RT_ABI __attribute__((__pcs__("aapcs")))
#endif
#else
#define COMPILER_RT_ABI
#endif

#define AEABI_RTABI __attribute__((__pcs__("aapcs")))

#if defined(_MSC_VER) && !defined(__clang__)
#define ALWAYS_INLINE __forceinline
#define NOINLINE __declspec(noinline)
#define NORETURN __declspec(noreturn)
#define UNUSED
#else
#define ALWAYS_INLINE __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#define NORETURN __attribute__((noreturn))
#define UNUSED __attribute__((unused))
#endif

#define STR(a) #a
#define XSTR(a) STR(a)
#define SYMBOL_NAME(name) XSTR(__USER_LABEL_PREFIX__) #name

#if defined(__ELF__) || defined(__MINGW32__) || defined(__wasm__) || defined(_AIX)

#define COMPILER_RT_ALIAS(name, aliasname) COMPILER_RT_ABI __typeof(name) aliasname __attribute__((__alias__(#name)));

#elif defined(__APPLE__)
#if defined(VISIBILITY_HIDDEN)
#define COMPILER_RT_ALIAS_VISIBILITY(name) __asm__(".private_extern " SYMBOL_NAME(name));

#else
#define COMPILER_RT_ALIAS_VISIBILITY(name)
#endif
#define COMPILER_RT_ALIAS(name, aliasname) __asm__(".globl " SYMBOL_NAME(aliasname)); COMPILER_RT_ALIAS_VISIBILITY(aliasname) __asm__(SYMBOL_NAME(aliasname) " = " SYMBOL_NAME(name)); COMPILER_RT_ABI __typeof(name) aliasname;




#elif defined(_WIN32)
#define COMPILER_RT_ALIAS(name, aliasname)
#else
#error Unsupported target
#endif

#if (defined(__FreeBSD__) || defined(__NetBSD__)) && (defined(_KERNEL) || defined(_STANDALONE))









#if defined(__has_include) && __has_include(<sys/limits.h>)
#include <sys/limits.h>
#else
#include <machine/limits.h>
#endif
#include <sys/stdint.h>
#include <sys/types.h>
#else

#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#endif


#include "int_types.h"


#include "int_util.h"

COMPILER_RT_ABI int __paritysi2(si_int a);
COMPILER_RT_ABI int __paritydi2(di_int a);

COMPILER_RT_ABI di_int __divdi3(di_int a, di_int b);
COMPILER_RT_ABI si_int __divsi3(si_int a, si_int b);
COMPILER_RT_ABI su_int __udivsi3(su_int n, su_int d);

COMPILER_RT_ABI su_int __udivmodsi4(su_int a, su_int b, su_int *rem);
COMPILER_RT_ABI du_int __udivmoddi4(du_int a, du_int b, du_int *rem);
#if defined(CRT_HAS_128BIT)
COMPILER_RT_ABI int __clzti2(ti_int a);
COMPILER_RT_ABI tu_int __udivmodti4(tu_int a, tu_int b, tu_int *rem);
#endif


#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>

int __inline __builtin_ctz(uint32_t value) {
unsigned long trailing_zero = 0;
if (_BitScanForward(&trailing_zero, value))
return trailing_zero;
return 32;
}

int __inline __builtin_clz(uint32_t value) {
unsigned long leading_zero = 0;
if (_BitScanReverse(&leading_zero, value))
return 31 - leading_zero;
return 32;
}

#if defined(_M_ARM) || defined(_M_X64)
int __inline __builtin_clzll(uint64_t value) {
unsigned long leading_zero = 0;
if (_BitScanReverse64(&leading_zero, value))
return 63 - leading_zero;
return 64;
}
#else
int __inline __builtin_clzll(uint64_t value) {
if (value == 0)
return 64;
uint32_t msh = (uint32_t)(value >> 32);
uint32_t lsh = (uint32_t)(value & 0xFFFFFFFF);
if (msh != 0)
return __builtin_clz(msh);
return 32 + __builtin_clz(lsh);
}
#endif

#define __builtin_clzl __builtin_clzll

bool __inline __builtin_sadd_overflow(int x, int y, int *result) {
if ((x < 0) != (y < 0)) {
*result = x + y;
return false;
}
int tmp = (unsigned int)x + (unsigned int)y;
if ((tmp < 0) != (x < 0))
return true;
*result = tmp;
return false;
}

#endif

#endif
