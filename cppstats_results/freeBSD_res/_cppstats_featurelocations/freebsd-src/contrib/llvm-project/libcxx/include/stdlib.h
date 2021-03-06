








#if defined(__need_malloc_and_calloc)

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <stdlib.h>

#elif !defined(_LIBCPP_STDLIB_H)
#define _LIBCPP_STDLIB_H



































































#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <stdlib.h>

#if defined(__cplusplus)
extern "C++" {


#undef abs
#undef labs
#if !defined(_LIBCPP_HAS_NO_LONG_LONG)
#undef llabs
#endif


#if !defined(_LIBCPP_MSVCRT) && !defined(__sun__)
inline _LIBCPP_INLINE_VISIBILITY long abs(long __x) _NOEXCEPT {
return __builtin_labs(__x);
}
#if !defined(_LIBCPP_HAS_NO_LONG_LONG)
inline _LIBCPP_INLINE_VISIBILITY long long abs(long long __x) _NOEXCEPT {
return __builtin_llabs(__x);
}
#endif
#endif

#if !defined(__sun__)
inline _LIBCPP_INLINE_VISIBILITY float abs(float __lcpp_x) _NOEXCEPT {
return __builtin_fabsf(__lcpp_x);
}

inline _LIBCPP_INLINE_VISIBILITY double abs(double __lcpp_x) _NOEXCEPT {
return __builtin_fabs(__lcpp_x);
}

inline _LIBCPP_INLINE_VISIBILITY long double
abs(long double __lcpp_x) _NOEXCEPT {
return __builtin_fabsl(__lcpp_x);
}
#endif



#undef div
#undef ldiv
#if !defined(_LIBCPP_HAS_NO_LONG_LONG)
#undef lldiv
#endif


#if !defined(_LIBCPP_MSVCRT) && !defined(__sun__)
inline _LIBCPP_INLINE_VISIBILITY ldiv_t div(long __x, long __y) _NOEXCEPT {
return ::ldiv(__x, __y);
}
#if !defined(_LIBCPP_HAS_NO_LONG_LONG)
inline _LIBCPP_INLINE_VISIBILITY lldiv_t div(long long __x,
long long __y) _NOEXCEPT {
return ::lldiv(__x, __y);
}
#endif
#endif
}
#endif

#endif
