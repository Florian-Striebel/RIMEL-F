







#if !defined(__CLANG_STDINT_H)



#if !defined(_AIX) || !defined(_STD_TYPES_T) || !defined(__STDC_HOSTED__)
#define __CLANG_STDINT_H
#endif




#if __STDC_HOSTED__ && __has_include_next(<stdint.h>)




















#if defined(__cplusplus)
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS_DEFINED_BY_CLANG
#endif
#if !defined(__STDC_CONSTANT_MACROS)
#define __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS_DEFINED_BY_CLANG
#endif
#endif

#include_next <stdint.h>

#if defined(__STDC_LIMIT_MACROS_DEFINED_BY_CLANG)
#undef __STDC_LIMIT_MACROS
#undef __STDC_LIMIT_MACROS_DEFINED_BY_CLANG
#endif
#if defined(__STDC_CONSTANT_MACROS_DEFINED_BY_CLANG)
#undef __STDC_CONSTANT_MACROS
#undef __STDC_CONSTANT_MACROS_DEFINED_BY_CLANG
#endif

#else






























#if defined(__INT64_TYPE__)
#if !defined(__int8_t_defined)
typedef __INT64_TYPE__ int64_t;
#endif
typedef __UINT64_TYPE__ uint64_t;
#define __int_least64_t int64_t
#define __uint_least64_t uint64_t
#define __int_least32_t int64_t
#define __uint_least32_t uint64_t
#define __int_least16_t int64_t
#define __uint_least16_t uint64_t
#define __int_least8_t int64_t
#define __uint_least8_t uint64_t
#endif

#if defined(__int_least64_t)
typedef __int_least64_t int_least64_t;
typedef __uint_least64_t uint_least64_t;
typedef __int_least64_t int_fast64_t;
typedef __uint_least64_t uint_fast64_t;
#endif

#if defined(__INT56_TYPE__)
typedef __INT56_TYPE__ int56_t;
typedef __UINT56_TYPE__ uint56_t;
typedef int56_t int_least56_t;
typedef uint56_t uint_least56_t;
typedef int56_t int_fast56_t;
typedef uint56_t uint_fast56_t;
#define __int_least32_t int56_t
#define __uint_least32_t uint56_t
#define __int_least16_t int56_t
#define __uint_least16_t uint56_t
#define __int_least8_t int56_t
#define __uint_least8_t uint56_t
#endif


#if defined(__INT48_TYPE__)
typedef __INT48_TYPE__ int48_t;
typedef __UINT48_TYPE__ uint48_t;
typedef int48_t int_least48_t;
typedef uint48_t uint_least48_t;
typedef int48_t int_fast48_t;
typedef uint48_t uint_fast48_t;
#define __int_least32_t int48_t
#define __uint_least32_t uint48_t
#define __int_least16_t int48_t
#define __uint_least16_t uint48_t
#define __int_least8_t int48_t
#define __uint_least8_t uint48_t
#endif


#if defined(__INT40_TYPE__)
typedef __INT40_TYPE__ int40_t;
typedef __UINT40_TYPE__ uint40_t;
typedef int40_t int_least40_t;
typedef uint40_t uint_least40_t;
typedef int40_t int_fast40_t;
typedef uint40_t uint_fast40_t;
#define __int_least32_t int40_t
#define __uint_least32_t uint40_t
#define __int_least16_t int40_t
#define __uint_least16_t uint40_t
#define __int_least8_t int40_t
#define __uint_least8_t uint40_t
#endif


#if defined(__INT32_TYPE__)

#if !defined(__int8_t_defined)
typedef __INT32_TYPE__ int32_t;
#endif

#if !defined(__uint32_t_defined)
#define __uint32_t_defined
typedef __UINT32_TYPE__ uint32_t;
#endif

#define __int_least32_t int32_t
#define __uint_least32_t uint32_t
#define __int_least16_t int32_t
#define __uint_least16_t uint32_t
#define __int_least8_t int32_t
#define __uint_least8_t uint32_t
#endif

#if defined(__int_least32_t)
typedef __int_least32_t int_least32_t;
typedef __uint_least32_t uint_least32_t;
typedef __int_least32_t int_fast32_t;
typedef __uint_least32_t uint_fast32_t;
#endif

#if defined(__INT24_TYPE__)
typedef __INT24_TYPE__ int24_t;
typedef __UINT24_TYPE__ uint24_t;
typedef int24_t int_least24_t;
typedef uint24_t uint_least24_t;
typedef int24_t int_fast24_t;
typedef uint24_t uint_fast24_t;
#define __int_least16_t int24_t
#define __uint_least16_t uint24_t
#define __int_least8_t int24_t
#define __uint_least8_t uint24_t
#endif

#if defined(__INT16_TYPE__)
#if !defined(__int8_t_defined)
typedef __INT16_TYPE__ int16_t;
#endif
typedef __UINT16_TYPE__ uint16_t;
#define __int_least16_t int16_t
#define __uint_least16_t uint16_t
#define __int_least8_t int16_t
#define __uint_least8_t uint16_t
#endif

#if defined(__int_least16_t)
typedef __int_least16_t int_least16_t;
typedef __uint_least16_t uint_least16_t;
typedef __int_least16_t int_fast16_t;
typedef __uint_least16_t uint_fast16_t;
#endif


#if defined(__INT8_TYPE__)
#if !defined(__int8_t_defined)
typedef __INT8_TYPE__ int8_t;
#endif
typedef __UINT8_TYPE__ uint8_t;
#define __int_least8_t int8_t
#define __uint_least8_t uint8_t
#endif

#if defined(__int_least8_t)
typedef __int_least8_t int_least8_t;
typedef __uint_least8_t uint_least8_t;
typedef __int_least8_t int_fast8_t;
typedef __uint_least8_t uint_fast8_t;
#endif


#if !defined(__int8_t_defined)
#define __int8_t_defined
#endif



#define __stdint_join3(a,b,c) a ##b ##c

#if !defined(_INTPTR_T)
#if !defined(__intptr_t_defined)
typedef __INTPTR_TYPE__ intptr_t;
#define __intptr_t_defined
#define _INTPTR_T
#endif
#endif

#if !defined(_UINTPTR_T)
typedef __UINTPTR_TYPE__ uintptr_t;
#define _UINTPTR_T
#endif



typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;


















#define __int_c_join(a, b) a ##b
#define __int_c(v, suffix) __int_c_join(v, suffix)
#define __uint_c(v, suffix) __int_c_join(v##U, suffix)


#if defined(__INT64_TYPE__)
#if defined(__INT64_C_SUFFIX__)
#define __int64_c_suffix __INT64_C_SUFFIX__
#define __int32_c_suffix __INT64_C_SUFFIX__
#define __int16_c_suffix __INT64_C_SUFFIX__
#define __int8_c_suffix __INT64_C_SUFFIX__
#else
#undef __int64_c_suffix
#undef __int32_c_suffix
#undef __int16_c_suffix
#undef __int8_c_suffix
#endif
#endif

#if defined(__int_least64_t)
#if defined(__int64_c_suffix)
#define INT64_C(v) __int_c(v, __int64_c_suffix)
#define UINT64_C(v) __uint_c(v, __int64_c_suffix)
#else
#define INT64_C(v) v
#define UINT64_C(v) v ##U
#endif
#endif


#if defined(__INT56_TYPE__)
#if defined(__INT56_C_SUFFIX__)
#define INT56_C(v) __int_c(v, __INT56_C_SUFFIX__)
#define UINT56_C(v) __uint_c(v, __INT56_C_SUFFIX__)
#define __int32_c_suffix __INT56_C_SUFFIX__
#define __int16_c_suffix __INT56_C_SUFFIX__
#define __int8_c_suffix __INT56_C_SUFFIX__
#else
#define INT56_C(v) v
#define UINT56_C(v) v ##U
#undef __int32_c_suffix
#undef __int16_c_suffix
#undef __int8_c_suffix
#endif
#endif


#if defined(__INT48_TYPE__)
#if defined(__INT48_C_SUFFIX__)
#define INT48_C(v) __int_c(v, __INT48_C_SUFFIX__)
#define UINT48_C(v) __uint_c(v, __INT48_C_SUFFIX__)
#define __int32_c_suffix __INT48_C_SUFFIX__
#define __int16_c_suffix __INT48_C_SUFFIX__
#define __int8_c_suffix __INT48_C_SUFFIX__
#else
#define INT48_C(v) v
#define UINT48_C(v) v ##U
#undef __int32_c_suffix
#undef __int16_c_suffix
#undef __int8_c_suffix
#endif
#endif


#if defined(__INT40_TYPE__)
#if defined(__INT40_C_SUFFIX__)
#define INT40_C(v) __int_c(v, __INT40_C_SUFFIX__)
#define UINT40_C(v) __uint_c(v, __INT40_C_SUFFIX__)
#define __int32_c_suffix __INT40_C_SUFFIX__
#define __int16_c_suffix __INT40_C_SUFFIX__
#define __int8_c_suffix __INT40_C_SUFFIX__
#else
#define INT40_C(v) v
#define UINT40_C(v) v ##U
#undef __int32_c_suffix
#undef __int16_c_suffix
#undef __int8_c_suffix
#endif
#endif


#if defined(__INT32_TYPE__)
#if defined(__INT32_C_SUFFIX__)
#define __int32_c_suffix __INT32_C_SUFFIX__
#define __int16_c_suffix __INT32_C_SUFFIX__
#define __int8_c_suffix __INT32_C_SUFFIX__
#else
#undef __int32_c_suffix
#undef __int16_c_suffix
#undef __int8_c_suffix
#endif
#endif

#if defined(__int_least32_t)
#if defined(__int32_c_suffix)
#define INT32_C(v) __int_c(v, __int32_c_suffix)
#define UINT32_C(v) __uint_c(v, __int32_c_suffix)
#else
#define INT32_C(v) v
#define UINT32_C(v) v ##U
#endif
#endif


#if defined(__INT24_TYPE__)
#if defined(__INT24_C_SUFFIX__)
#define INT24_C(v) __int_c(v, __INT24_C_SUFFIX__)
#define UINT24_C(v) __uint_c(v, __INT24_C_SUFFIX__)
#define __int16_c_suffix __INT24_C_SUFFIX__
#define __int8_c_suffix __INT24_C_SUFFIX__
#else
#define INT24_C(v) v
#define UINT24_C(v) v ##U
#undef __int16_c_suffix
#undef __int8_c_suffix
#endif
#endif


#if defined(__INT16_TYPE__)
#if defined(__INT16_C_SUFFIX__)
#define __int16_c_suffix __INT16_C_SUFFIX__
#define __int8_c_suffix __INT16_C_SUFFIX__
#else
#undef __int16_c_suffix
#undef __int8_c_suffix
#endif
#endif

#if defined(__int_least16_t)
#if defined(__int16_c_suffix)
#define INT16_C(v) __int_c(v, __int16_c_suffix)
#define UINT16_C(v) __uint_c(v, __int16_c_suffix)
#else
#define INT16_C(v) v
#define UINT16_C(v) v ##U
#endif
#endif


#if defined(__INT8_TYPE__)
#if defined(__INT8_C_SUFFIX__)
#define __int8_c_suffix __INT8_C_SUFFIX__
#else
#undef __int8_c_suffix
#endif
#endif

#if defined(__int_least8_t)
#if defined(__int8_c_suffix)
#define INT8_C(v) __int_c(v, __int8_c_suffix)
#define UINT8_C(v) __uint_c(v, __int8_c_suffix)
#else
#define INT8_C(v) v
#define UINT8_C(v) v ##U
#endif
#endif





















#if defined(__INT64_TYPE__)
#define INT64_MAX INT64_C( 9223372036854775807)
#define INT64_MIN (-INT64_C( 9223372036854775807)-1)
#define UINT64_MAX UINT64_C(18446744073709551615)
#define __INT_LEAST64_MIN INT64_MIN
#define __INT_LEAST64_MAX INT64_MAX
#define __UINT_LEAST64_MAX UINT64_MAX
#define __INT_LEAST32_MIN INT64_MIN
#define __INT_LEAST32_MAX INT64_MAX
#define __UINT_LEAST32_MAX UINT64_MAX
#define __INT_LEAST16_MIN INT64_MIN
#define __INT_LEAST16_MAX INT64_MAX
#define __UINT_LEAST16_MAX UINT64_MAX
#define __INT_LEAST8_MIN INT64_MIN
#define __INT_LEAST8_MAX INT64_MAX
#define __UINT_LEAST8_MAX UINT64_MAX
#endif

#if defined(__INT_LEAST64_MIN)
#define INT_LEAST64_MIN __INT_LEAST64_MIN
#define INT_LEAST64_MAX __INT_LEAST64_MAX
#define UINT_LEAST64_MAX __UINT_LEAST64_MAX
#define INT_FAST64_MIN __INT_LEAST64_MIN
#define INT_FAST64_MAX __INT_LEAST64_MAX
#define UINT_FAST64_MAX __UINT_LEAST64_MAX
#endif


#if defined(__INT56_TYPE__)
#define INT56_MAX INT56_C(36028797018963967)
#define INT56_MIN (-INT56_C(36028797018963967)-1)
#define UINT56_MAX UINT56_C(72057594037927935)
#define INT_LEAST56_MIN INT56_MIN
#define INT_LEAST56_MAX INT56_MAX
#define UINT_LEAST56_MAX UINT56_MAX
#define INT_FAST56_MIN INT56_MIN
#define INT_FAST56_MAX INT56_MAX
#define UINT_FAST56_MAX UINT56_MAX
#define __INT_LEAST32_MIN INT56_MIN
#define __INT_LEAST32_MAX INT56_MAX
#define __UINT_LEAST32_MAX UINT56_MAX
#define __INT_LEAST16_MIN INT56_MIN
#define __INT_LEAST16_MAX INT56_MAX
#define __UINT_LEAST16_MAX UINT56_MAX
#define __INT_LEAST8_MIN INT56_MIN
#define __INT_LEAST8_MAX INT56_MAX
#define __UINT_LEAST8_MAX UINT56_MAX
#endif


#if defined(__INT48_TYPE__)
#define INT48_MAX INT48_C(140737488355327)
#define INT48_MIN (-INT48_C(140737488355327)-1)
#define UINT48_MAX UINT48_C(281474976710655)
#define INT_LEAST48_MIN INT48_MIN
#define INT_LEAST48_MAX INT48_MAX
#define UINT_LEAST48_MAX UINT48_MAX
#define INT_FAST48_MIN INT48_MIN
#define INT_FAST48_MAX INT48_MAX
#define UINT_FAST48_MAX UINT48_MAX
#define __INT_LEAST32_MIN INT48_MIN
#define __INT_LEAST32_MAX INT48_MAX
#define __UINT_LEAST32_MAX UINT48_MAX
#define __INT_LEAST16_MIN INT48_MIN
#define __INT_LEAST16_MAX INT48_MAX
#define __UINT_LEAST16_MAX UINT48_MAX
#define __INT_LEAST8_MIN INT48_MIN
#define __INT_LEAST8_MAX INT48_MAX
#define __UINT_LEAST8_MAX UINT48_MAX
#endif


#if defined(__INT40_TYPE__)
#define INT40_MAX INT40_C(549755813887)
#define INT40_MIN (-INT40_C(549755813887)-1)
#define UINT40_MAX UINT40_C(1099511627775)
#define INT_LEAST40_MIN INT40_MIN
#define INT_LEAST40_MAX INT40_MAX
#define UINT_LEAST40_MAX UINT40_MAX
#define INT_FAST40_MIN INT40_MIN
#define INT_FAST40_MAX INT40_MAX
#define UINT_FAST40_MAX UINT40_MAX
#define __INT_LEAST32_MIN INT40_MIN
#define __INT_LEAST32_MAX INT40_MAX
#define __UINT_LEAST32_MAX UINT40_MAX
#define __INT_LEAST16_MIN INT40_MIN
#define __INT_LEAST16_MAX INT40_MAX
#define __UINT_LEAST16_MAX UINT40_MAX
#define __INT_LEAST8_MIN INT40_MIN
#define __INT_LEAST8_MAX INT40_MAX
#define __UINT_LEAST8_MAX UINT40_MAX
#endif


#if defined(__INT32_TYPE__)
#define INT32_MAX INT32_C(2147483647)
#define INT32_MIN (-INT32_C(2147483647)-1)
#define UINT32_MAX UINT32_C(4294967295)
#define __INT_LEAST32_MIN INT32_MIN
#define __INT_LEAST32_MAX INT32_MAX
#define __UINT_LEAST32_MAX UINT32_MAX
#define __INT_LEAST16_MIN INT32_MIN
#define __INT_LEAST16_MAX INT32_MAX
#define __UINT_LEAST16_MAX UINT32_MAX
#define __INT_LEAST8_MIN INT32_MIN
#define __INT_LEAST8_MAX INT32_MAX
#define __UINT_LEAST8_MAX UINT32_MAX
#endif

#if defined(__INT_LEAST32_MIN)
#define INT_LEAST32_MIN __INT_LEAST32_MIN
#define INT_LEAST32_MAX __INT_LEAST32_MAX
#define UINT_LEAST32_MAX __UINT_LEAST32_MAX
#define INT_FAST32_MIN __INT_LEAST32_MIN
#define INT_FAST32_MAX __INT_LEAST32_MAX
#define UINT_FAST32_MAX __UINT_LEAST32_MAX
#endif


#if defined(__INT24_TYPE__)
#define INT24_MAX INT24_C(8388607)
#define INT24_MIN (-INT24_C(8388607)-1)
#define UINT24_MAX UINT24_C(16777215)
#define INT_LEAST24_MIN INT24_MIN
#define INT_LEAST24_MAX INT24_MAX
#define UINT_LEAST24_MAX UINT24_MAX
#define INT_FAST24_MIN INT24_MIN
#define INT_FAST24_MAX INT24_MAX
#define UINT_FAST24_MAX UINT24_MAX
#define __INT_LEAST16_MIN INT24_MIN
#define __INT_LEAST16_MAX INT24_MAX
#define __UINT_LEAST16_MAX UINT24_MAX
#define __INT_LEAST8_MIN INT24_MIN
#define __INT_LEAST8_MAX INT24_MAX
#define __UINT_LEAST8_MAX UINT24_MAX
#endif


#if defined(__INT16_TYPE__)
#define INT16_MAX INT16_C(32767)
#define INT16_MIN (-INT16_C(32767)-1)
#define UINT16_MAX UINT16_C(65535)
#define __INT_LEAST16_MIN INT16_MIN
#define __INT_LEAST16_MAX INT16_MAX
#define __UINT_LEAST16_MAX UINT16_MAX
#define __INT_LEAST8_MIN INT16_MIN
#define __INT_LEAST8_MAX INT16_MAX
#define __UINT_LEAST8_MAX UINT16_MAX
#endif

#if defined(__INT_LEAST16_MIN)
#define INT_LEAST16_MIN __INT_LEAST16_MIN
#define INT_LEAST16_MAX __INT_LEAST16_MAX
#define UINT_LEAST16_MAX __UINT_LEAST16_MAX
#define INT_FAST16_MIN __INT_LEAST16_MIN
#define INT_FAST16_MAX __INT_LEAST16_MAX
#define UINT_FAST16_MAX __UINT_LEAST16_MAX
#endif


#if defined(__INT8_TYPE__)
#define INT8_MAX INT8_C(127)
#define INT8_MIN (-INT8_C(127)-1)
#define UINT8_MAX UINT8_C(255)
#define __INT_LEAST8_MIN INT8_MIN
#define __INT_LEAST8_MAX INT8_MAX
#define __UINT_LEAST8_MAX UINT8_MAX
#endif

#if defined(__INT_LEAST8_MIN)
#define INT_LEAST8_MIN __INT_LEAST8_MIN
#define INT_LEAST8_MAX __INT_LEAST8_MAX
#define UINT_LEAST8_MAX __UINT_LEAST8_MAX
#define INT_FAST8_MIN __INT_LEAST8_MIN
#define INT_FAST8_MAX __INT_LEAST8_MAX
#define UINT_FAST8_MAX __UINT_LEAST8_MAX
#endif


#define __INTN_MIN(n) __stdint_join3( INT, n, _MIN)
#define __INTN_MAX(n) __stdint_join3( INT, n, _MAX)
#define __UINTN_MAX(n) __stdint_join3(UINT, n, _MAX)
#define __INTN_C(n, v) __stdint_join3( INT, n, _C(v))
#define __UINTN_C(n, v) __stdint_join3(UINT, n, _C(v))




#define INTPTR_MIN (-__INTPTR_MAX__-1)
#define INTPTR_MAX __INTPTR_MAX__
#define UINTPTR_MAX __UINTPTR_MAX__
#define PTRDIFF_MIN (-__PTRDIFF_MAX__-1)
#define PTRDIFF_MAX __PTRDIFF_MAX__
#define SIZE_MAX __SIZE_MAX__



#if defined(__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__ >= 1
#define RSIZE_MAX (SIZE_MAX >> 1)
#endif


#define INTMAX_MIN (-__INTMAX_MAX__-1)
#define INTMAX_MAX __INTMAX_MAX__
#define UINTMAX_MAX __UINTMAX_MAX__


#define SIG_ATOMIC_MIN __INTN_MIN(__SIG_ATOMIC_WIDTH__)
#define SIG_ATOMIC_MAX __INTN_MAX(__SIG_ATOMIC_WIDTH__)
#if defined(__WINT_UNSIGNED__)
#define WINT_MIN __UINTN_C(__WINT_WIDTH__, 0)
#define WINT_MAX __UINTN_MAX(__WINT_WIDTH__)
#else
#define WINT_MIN __INTN_MIN(__WINT_WIDTH__)
#define WINT_MAX __INTN_MAX(__WINT_WIDTH__)
#endif

#if !defined(WCHAR_MAX)
#define WCHAR_MAX __WCHAR_MAX__
#endif
#if !defined(WCHAR_MIN)
#if __WCHAR_MAX__ == __INTN_MAX(__WCHAR_WIDTH__)
#define WCHAR_MIN __INTN_MIN(__WCHAR_WIDTH__)
#else
#define WCHAR_MIN __UINTN_C(__WCHAR_WIDTH__, 0)
#endif
#endif


#define INTMAX_C(v) __int_c(v, __INTMAX_C_SUFFIX__)
#define UINTMAX_C(v) __int_c(v, __UINTMAX_C_SUFFIX__)

#endif
#endif
