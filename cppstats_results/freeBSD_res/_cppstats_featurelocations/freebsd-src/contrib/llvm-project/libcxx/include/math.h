








#if !defined(_LIBCPP_MATH_H)
#define _LIBCPP_MATH_H


























































































































































































































































































#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <math.h>

#if defined(__cplusplus)



extern "C++" {

#include <stdlib.h>
#include <type_traits>
#include <limits>



#if defined(signbit)

template <class _A1>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_signbit(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_signbit)
return __builtin_signbit(__lcpp_x);
#else
return signbit(__lcpp_x);
#endif
}

#undef signbit

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_floating_point<_A1>::value, bool>::type
signbit(_A1 __lcpp_x) _NOEXCEPT
{
return __libcpp_signbit((typename std::__promote<_A1>::type)__lcpp_x);
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_integral<_A1>::value && std::is_signed<_A1>::value, bool>::type
signbit(_A1 __lcpp_x) _NOEXCEPT
{ return __lcpp_x < 0; }

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_integral<_A1>::value && !std::is_signed<_A1>::value, bool>::type
signbit(_A1) _NOEXCEPT
{ return false; }

#elif defined(_LIBCPP_MSVCRT)

template <typename _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_floating_point<_A1>::value, bool>::type
signbit(_A1 __lcpp_x) _NOEXCEPT
{
return ::signbit(static_cast<typename std::__promote<_A1>::type>(__lcpp_x));
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_integral<_A1>::value && std::is_signed<_A1>::value, bool>::type
signbit(_A1 __lcpp_x) _NOEXCEPT
{ return __lcpp_x < 0; }

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_integral<_A1>::value && !std::is_signed<_A1>::value, bool>::type
signbit(_A1) _NOEXCEPT
{ return false; }

#endif



#if defined(fpclassify)

template <class _A1>
_LIBCPP_INLINE_VISIBILITY
int
__libcpp_fpclassify(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_fpclassify)
return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL,
FP_ZERO, __lcpp_x);
#else
return fpclassify(__lcpp_x);
#endif
}

#undef fpclassify

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_floating_point<_A1>::value, int>::type
fpclassify(_A1 __lcpp_x) _NOEXCEPT
{
return __libcpp_fpclassify((typename std::__promote<_A1>::type)__lcpp_x);
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, int>::type
fpclassify(_A1 __lcpp_x) _NOEXCEPT
{ return __lcpp_x == 0 ? FP_ZERO : FP_NORMAL; }

#elif defined(_LIBCPP_MSVCRT)

template <typename _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_floating_point<_A1>::value, bool>::type
fpclassify(_A1 __lcpp_x) _NOEXCEPT
{
return ::fpclassify(static_cast<typename std::__promote<_A1>::type>(__lcpp_x));
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, int>::type
fpclassify(_A1 __lcpp_x) _NOEXCEPT
{ return __lcpp_x == 0 ? FP_ZERO : FP_NORMAL; }

#endif



#if defined(isfinite)

template <class _A1>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isfinite(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_isfinite)
return __builtin_isfinite(__lcpp_x);
#else
return isfinite(__lcpp_x);
#endif
}

#undef isfinite

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_arithmetic<_A1>::value && std::numeric_limits<_A1>::has_infinity,
bool>::type
isfinite(_A1 __lcpp_x) _NOEXCEPT
{
return __libcpp_isfinite((typename std::__promote<_A1>::type)__lcpp_x);
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_arithmetic<_A1>::value && !std::numeric_limits<_A1>::has_infinity,
bool>::type
isfinite(_A1) _NOEXCEPT
{ return true; }

#endif



#if defined(isinf)

template <class _A1>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isinf(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_isinf)
return __builtin_isinf(__lcpp_x);
#else
return isinf(__lcpp_x);
#endif
}

#undef isinf

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_arithmetic<_A1>::value && std::numeric_limits<_A1>::has_infinity,
bool>::type
isinf(_A1 __lcpp_x) _NOEXCEPT
{
return __libcpp_isinf((typename std::__promote<_A1>::type)__lcpp_x);
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<
std::is_arithmetic<_A1>::value && !std::numeric_limits<_A1>::has_infinity,
bool>::type
isinf(_A1) _NOEXCEPT
{ return false; }

#if defined(_LIBCPP_PREFERRED_OVERLOAD)
inline _LIBCPP_INLINE_VISIBILITY
bool
isinf(float __lcpp_x) _NOEXCEPT { return __libcpp_isinf(__lcpp_x); }

inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
bool
isinf(double __lcpp_x) _NOEXCEPT { return __libcpp_isinf(__lcpp_x); }

inline _LIBCPP_INLINE_VISIBILITY
bool
isinf(long double __lcpp_x) _NOEXCEPT { return __libcpp_isinf(__lcpp_x); }
#endif

#endif



#if defined(isnan)

template <class _A1>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isnan(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_isnan)
return __builtin_isnan(__lcpp_x);
#else
return isnan(__lcpp_x);
#endif
}

#undef isnan

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_floating_point<_A1>::value, bool>::type
isnan(_A1 __lcpp_x) _NOEXCEPT
{
return __libcpp_isnan((typename std::__promote<_A1>::type)__lcpp_x);
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, bool>::type
isnan(_A1) _NOEXCEPT
{ return false; }

#if defined(_LIBCPP_PREFERRED_OVERLOAD)
inline _LIBCPP_INLINE_VISIBILITY
bool
isnan(float __lcpp_x) _NOEXCEPT { return __libcpp_isnan(__lcpp_x); }

inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
bool
isnan(double __lcpp_x) _NOEXCEPT { return __libcpp_isnan(__lcpp_x); }

inline _LIBCPP_INLINE_VISIBILITY
bool
isnan(long double __lcpp_x) _NOEXCEPT { return __libcpp_isnan(__lcpp_x); }
#endif

#endif



#if defined(isnormal)

template <class _A1>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isnormal(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_isnormal)
return __builtin_isnormal(__lcpp_x);
#else
return isnormal(__lcpp_x);
#endif
}

#undef isnormal

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_floating_point<_A1>::value, bool>::type
isnormal(_A1 __lcpp_x) _NOEXCEPT
{
return __libcpp_isnormal((typename std::__promote<_A1>::type)__lcpp_x);
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, bool>::type
isnormal(_A1 __lcpp_x) _NOEXCEPT
{ return __lcpp_x != 0; }

#endif



#if defined(isgreater)

template <class _A1, class _A2>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isgreater(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
return isgreater(__lcpp_x, __lcpp_y);
}

#undef isgreater

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
bool
>::type
isgreater(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type type;
return __libcpp_isgreater((type)__lcpp_x, (type)__lcpp_y);
}

#endif



#if defined(isgreaterequal)

template <class _A1, class _A2>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isgreaterequal(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
return isgreaterequal(__lcpp_x, __lcpp_y);
}

#undef isgreaterequal

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
bool
>::type
isgreaterequal(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type type;
return __libcpp_isgreaterequal((type)__lcpp_x, (type)__lcpp_y);
}

#endif



#if defined(isless)

template <class _A1, class _A2>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isless(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
return isless(__lcpp_x, __lcpp_y);
}

#undef isless

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
bool
>::type
isless(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type type;
return __libcpp_isless((type)__lcpp_x, (type)__lcpp_y);
}

#endif



#if defined(islessequal)

template <class _A1, class _A2>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_islessequal(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
return islessequal(__lcpp_x, __lcpp_y);
}

#undef islessequal

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
bool
>::type
islessequal(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type type;
return __libcpp_islessequal((type)__lcpp_x, (type)__lcpp_y);
}

#endif



#if defined(islessgreater)

template <class _A1, class _A2>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_islessgreater(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
return islessgreater(__lcpp_x, __lcpp_y);
}

#undef islessgreater

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
bool
>::type
islessgreater(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type type;
return __libcpp_islessgreater((type)__lcpp_x, (type)__lcpp_y);
}

#endif



#if defined(isunordered)

template <class _A1, class _A2>
_LIBCPP_INLINE_VISIBILITY
bool
__libcpp_isunordered(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
return isunordered(__lcpp_x, __lcpp_y);
}

#undef isunordered

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
bool
>::type
isunordered(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type type;
return __libcpp_isunordered((type)__lcpp_x, (type)__lcpp_y);
}

#endif











#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float acos(float __lcpp_x) _NOEXCEPT {return ::acosf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double acos(long double __lcpp_x) _NOEXCEPT {return ::acosl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
acos(_A1 __lcpp_x) _NOEXCEPT {return ::acos((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float asin(float __lcpp_x) _NOEXCEPT {return ::asinf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double asin(long double __lcpp_x) _NOEXCEPT {return ::asinl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
asin(_A1 __lcpp_x) _NOEXCEPT {return ::asin((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float atan(float __lcpp_x) _NOEXCEPT {return ::atanf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double atan(long double __lcpp_x) _NOEXCEPT {return ::atanl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
atan(_A1 __lcpp_x) _NOEXCEPT {return ::atan((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float atan2(float __lcpp_y, float __lcpp_x) _NOEXCEPT {return ::atan2f(__lcpp_y, __lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double atan2(long double __lcpp_y, long double __lcpp_x) _NOEXCEPT {return ::atan2l(__lcpp_y, __lcpp_x);}
#endif

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
atan2(_A1 __lcpp_y, _A2 __lcpp_x) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::atan2((__result_type)__lcpp_y, (__result_type)__lcpp_x);
}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float ceil(float __lcpp_x) _NOEXCEPT {return ::ceilf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double ceil(long double __lcpp_x) _NOEXCEPT {return ::ceill(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
ceil(_A1 __lcpp_x) _NOEXCEPT {return ::ceil((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float cos(float __lcpp_x) _NOEXCEPT {return ::cosf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double cos(long double __lcpp_x) _NOEXCEPT {return ::cosl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
cos(_A1 __lcpp_x) _NOEXCEPT {return ::cos((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float cosh(float __lcpp_x) _NOEXCEPT {return ::coshf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double cosh(long double __lcpp_x) _NOEXCEPT {return ::coshl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
cosh(_A1 __lcpp_x) _NOEXCEPT {return ::cosh((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float exp(float __lcpp_x) _NOEXCEPT {return ::expf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double exp(long double __lcpp_x) _NOEXCEPT {return ::expl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
exp(_A1 __lcpp_x) _NOEXCEPT {return ::exp((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float fabs(float __lcpp_x) _NOEXCEPT {return ::fabsf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double fabs(long double __lcpp_x) _NOEXCEPT {return ::fabsl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
fabs(_A1 __lcpp_x) _NOEXCEPT {return ::fabs((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float floor(float __lcpp_x) _NOEXCEPT {return ::floorf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double floor(long double __lcpp_x) _NOEXCEPT {return ::floorl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
floor(_A1 __lcpp_x) _NOEXCEPT {return ::floor((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float fmod(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::fmodf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double fmod(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::fmodl(__lcpp_x, __lcpp_y);}
#endif

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
fmod(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::fmod((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float frexp(float __lcpp_x, int* __lcpp_e) _NOEXCEPT {return ::frexpf(__lcpp_x, __lcpp_e);}
inline _LIBCPP_INLINE_VISIBILITY long double frexp(long double __lcpp_x, int* __lcpp_e) _NOEXCEPT {return ::frexpl(__lcpp_x, __lcpp_e);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
frexp(_A1 __lcpp_x, int* __lcpp_e) _NOEXCEPT {return ::frexp((double)__lcpp_x, __lcpp_e);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float ldexp(float __lcpp_x, int __lcpp_e) _NOEXCEPT {return ::ldexpf(__lcpp_x, __lcpp_e);}
inline _LIBCPP_INLINE_VISIBILITY long double ldexp(long double __lcpp_x, int __lcpp_e) _NOEXCEPT {return ::ldexpl(__lcpp_x, __lcpp_e);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
ldexp(_A1 __lcpp_x, int __lcpp_e) _NOEXCEPT {return ::ldexp((double)__lcpp_x, __lcpp_e);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float log(float __lcpp_x) _NOEXCEPT {return ::logf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double log(long double __lcpp_x) _NOEXCEPT {return ::logl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
log(_A1 __lcpp_x) _NOEXCEPT {return ::log((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float log10(float __lcpp_x) _NOEXCEPT {return ::log10f(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double log10(long double __lcpp_x) _NOEXCEPT {return ::log10l(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
log10(_A1 __lcpp_x) _NOEXCEPT {return ::log10((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float modf(float __lcpp_x, float* __lcpp_y) _NOEXCEPT {return ::modff(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double modf(long double __lcpp_x, long double* __lcpp_y) _NOEXCEPT {return ::modfl(__lcpp_x, __lcpp_y);}
#endif



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float pow(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::powf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double pow(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::powl(__lcpp_x, __lcpp_y);}
#endif

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
pow(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::pow((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float sin(float __lcpp_x) _NOEXCEPT {return ::sinf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double sin(long double __lcpp_x) _NOEXCEPT {return ::sinl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
sin(_A1 __lcpp_x) _NOEXCEPT {return ::sin((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float sinh(float __lcpp_x) _NOEXCEPT {return ::sinhf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double sinh(long double __lcpp_x) _NOEXCEPT {return ::sinhl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
sinh(_A1 __lcpp_x) _NOEXCEPT {return ::sinh((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float sqrt(float __lcpp_x) _NOEXCEPT {return ::sqrtf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double sqrt(long double __lcpp_x) _NOEXCEPT {return ::sqrtl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
sqrt(_A1 __lcpp_x) _NOEXCEPT {return ::sqrt((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float tan(float __lcpp_x) _NOEXCEPT {return ::tanf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double tan(long double __lcpp_x) _NOEXCEPT {return ::tanl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
tan(_A1 __lcpp_x) _NOEXCEPT {return ::tan((double)__lcpp_x);}



#if !(defined(_AIX) || defined(__sun__))
inline _LIBCPP_INLINE_VISIBILITY float tanh(float __lcpp_x) _NOEXCEPT {return ::tanhf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double tanh(long double __lcpp_x) _NOEXCEPT {return ::tanhl(__lcpp_x);}
#endif

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
tanh(_A1 __lcpp_x) _NOEXCEPT {return ::tanh((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float acosh(float __lcpp_x) _NOEXCEPT {return ::acoshf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double acosh(long double __lcpp_x) _NOEXCEPT {return ::acoshl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
acosh(_A1 __lcpp_x) _NOEXCEPT {return ::acosh((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float asinh(float __lcpp_x) _NOEXCEPT {return ::asinhf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double asinh(long double __lcpp_x) _NOEXCEPT {return ::asinhl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
asinh(_A1 __lcpp_x) _NOEXCEPT {return ::asinh((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float atanh(float __lcpp_x) _NOEXCEPT {return ::atanhf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double atanh(long double __lcpp_x) _NOEXCEPT {return ::atanhl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
atanh(_A1 __lcpp_x) _NOEXCEPT {return ::atanh((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float cbrt(float __lcpp_x) _NOEXCEPT {return ::cbrtf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double cbrt(long double __lcpp_x) _NOEXCEPT {return ::cbrtl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
cbrt(_A1 __lcpp_x) _NOEXCEPT {return ::cbrt((double)__lcpp_x);}



#if __has_builtin(__builtin_copysignf)
_LIBCPP_CONSTEXPR
#endif
inline _LIBCPP_INLINE_VISIBILITY float __libcpp_copysign(float __lcpp_x, float __lcpp_y) _NOEXCEPT {
#if __has_builtin(__builtin_copysignf)
return __builtin_copysignf(__lcpp_x, __lcpp_y);
#else
return ::copysignf(__lcpp_x, __lcpp_y);
#endif
}

#if __has_builtin(__builtin_copysign)
_LIBCPP_CONSTEXPR
#endif
inline _LIBCPP_INLINE_VISIBILITY double __libcpp_copysign(double __lcpp_x, double __lcpp_y) _NOEXCEPT {
#if __has_builtin(__builtin_copysign)
return __builtin_copysign(__lcpp_x, __lcpp_y);
#else
return ::copysign(__lcpp_x, __lcpp_y);
#endif
}

#if __has_builtin(__builtin_copysignl)
_LIBCPP_CONSTEXPR
#endif
inline _LIBCPP_INLINE_VISIBILITY long double __libcpp_copysign(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {
#if __has_builtin(__builtin_copysignl)
return __builtin_copysignl(__lcpp_x, __lcpp_y);
#else
return ::copysignl(__lcpp_x, __lcpp_y);
#endif
}

template <class _A1, class _A2>
#if __has_builtin(__builtin_copysign)
_LIBCPP_CONSTEXPR
#endif
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
__libcpp_copysign(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT {
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
#if __has_builtin(__builtin_copysign)
return __builtin_copysign((__result_type)__lcpp_x, (__result_type)__lcpp_y);
#else
return ::copysign((__result_type)__lcpp_x, (__result_type)__lcpp_y);
#endif
}

inline _LIBCPP_INLINE_VISIBILITY float copysign(float __lcpp_x, float __lcpp_y) _NOEXCEPT {
return ::__libcpp_copysign(__lcpp_x, __lcpp_y);
}

inline _LIBCPP_INLINE_VISIBILITY long double copysign(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {
return ::__libcpp_copysign(__lcpp_x, __lcpp_y);
}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
copysign(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT {
return ::__libcpp_copysign(__lcpp_x, __lcpp_y);
}



inline _LIBCPP_INLINE_VISIBILITY float erf(float __lcpp_x) _NOEXCEPT {return ::erff(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double erf(long double __lcpp_x) _NOEXCEPT {return ::erfl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
erf(_A1 __lcpp_x) _NOEXCEPT {return ::erf((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float erfc(float __lcpp_x) _NOEXCEPT {return ::erfcf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double erfc(long double __lcpp_x) _NOEXCEPT {return ::erfcl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
erfc(_A1 __lcpp_x) _NOEXCEPT {return ::erfc((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float exp2(float __lcpp_x) _NOEXCEPT {return ::exp2f(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double exp2(long double __lcpp_x) _NOEXCEPT {return ::exp2l(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
exp2(_A1 __lcpp_x) _NOEXCEPT {return ::exp2((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float expm1(float __lcpp_x) _NOEXCEPT {return ::expm1f(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double expm1(long double __lcpp_x) _NOEXCEPT {return ::expm1l(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
expm1(_A1 __lcpp_x) _NOEXCEPT {return ::expm1((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float fdim(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::fdimf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double fdim(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::fdiml(__lcpp_x, __lcpp_y);}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
fdim(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::fdim((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



inline _LIBCPP_INLINE_VISIBILITY float fma(float __lcpp_x, float __lcpp_y, float __lcpp_z) _NOEXCEPT
{
#if __has_builtin(__builtin_fmaf)
return __builtin_fmaf(__lcpp_x, __lcpp_y, __lcpp_z);
#else
return ::fmaf(__lcpp_x, __lcpp_y, __lcpp_z);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long double fma(long double __lcpp_x, long double __lcpp_y, long double __lcpp_z) _NOEXCEPT
{
#if __has_builtin(__builtin_fmal)
return __builtin_fmal(__lcpp_x, __lcpp_y, __lcpp_z);
#else
return ::fmal(__lcpp_x, __lcpp_y, __lcpp_z);
#endif
}

template <class _A1, class _A2, class _A3>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value &&
std::is_arithmetic<_A3>::value,
std::__promote<_A1, _A2, _A3>
>::type
fma(_A1 __lcpp_x, _A2 __lcpp_y, _A3 __lcpp_z) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2, _A3>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value &&
std::_IsSame<_A3, __result_type>::value)), "");
#if __has_builtin(__builtin_fma)
return __builtin_fma((__result_type)__lcpp_x, (__result_type)__lcpp_y, (__result_type)__lcpp_z);
#else
return ::fma((__result_type)__lcpp_x, (__result_type)__lcpp_y, (__result_type)__lcpp_z);
#endif
}



inline _LIBCPP_INLINE_VISIBILITY float fmax(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::fmaxf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double fmax(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::fmaxl(__lcpp_x, __lcpp_y);}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
fmax(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::fmax((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



inline _LIBCPP_INLINE_VISIBILITY float fmin(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::fminf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double fmin(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::fminl(__lcpp_x, __lcpp_y);}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
fmin(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::fmin((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



inline _LIBCPP_INLINE_VISIBILITY float hypot(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::hypotf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double hypot(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::hypotl(__lcpp_x, __lcpp_y);}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
hypot(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::hypot((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



inline _LIBCPP_INLINE_VISIBILITY int ilogb(float __lcpp_x) _NOEXCEPT {return ::ilogbf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY int ilogb(long double __lcpp_x) _NOEXCEPT {return ::ilogbl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, int>::type
ilogb(_A1 __lcpp_x) _NOEXCEPT {return ::ilogb((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float lgamma(float __lcpp_x) _NOEXCEPT {return ::lgammaf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double lgamma(long double __lcpp_x) _NOEXCEPT {return ::lgammal(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
lgamma(_A1 __lcpp_x) _NOEXCEPT {return ::lgamma((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY long long llrint(float __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_llrintf)
return __builtin_llrintf(__lcpp_x);
#else
return ::llrintf(__lcpp_x);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long long llrint(long double __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_llrintl)
return __builtin_llrintl(__lcpp_x);
#else
return ::llrintl(__lcpp_x);
#endif
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, long long>::type
llrint(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_llrint)
return __builtin_llrint((double)__lcpp_x);
#else
return ::llrint((double)__lcpp_x);
#endif
}



inline _LIBCPP_INLINE_VISIBILITY long long llround(float __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_llroundf)
return __builtin_llroundf(__lcpp_x);
#else
return ::llroundf(__lcpp_x);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long long llround(long double __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_llroundl)
return __builtin_llroundl(__lcpp_x);
#else
return ::llroundl(__lcpp_x);
#endif
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, long long>::type
llround(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_llround)
return __builtin_llround((double)__lcpp_x);
#else
return ::llround((double)__lcpp_x);
#endif
}



inline _LIBCPP_INLINE_VISIBILITY float log1p(float __lcpp_x) _NOEXCEPT {return ::log1pf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double log1p(long double __lcpp_x) _NOEXCEPT {return ::log1pl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
log1p(_A1 __lcpp_x) _NOEXCEPT {return ::log1p((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float log2(float __lcpp_x) _NOEXCEPT {return ::log2f(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double log2(long double __lcpp_x) _NOEXCEPT {return ::log2l(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
log2(_A1 __lcpp_x) _NOEXCEPT {return ::log2((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float logb(float __lcpp_x) _NOEXCEPT {return ::logbf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double logb(long double __lcpp_x) _NOEXCEPT {return ::logbl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
logb(_A1 __lcpp_x) _NOEXCEPT {return ::logb((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY long lrint(float __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_lrintf)
return __builtin_lrintf(__lcpp_x);
#else
return ::lrintf(__lcpp_x);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long lrint(long double __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_lrintl)
return __builtin_lrintl(__lcpp_x);
#else
return ::lrintl(__lcpp_x);
#endif
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, long>::type
lrint(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_lrint)
return __builtin_lrint((double)__lcpp_x);
#else
return ::lrint((double)__lcpp_x);
#endif
}



inline _LIBCPP_INLINE_VISIBILITY long lround(float __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_lroundf)
return __builtin_lroundf(__lcpp_x);
#else
return ::lroundf(__lcpp_x);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long lround(long double __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_lroundl)
return __builtin_lroundl(__lcpp_x);
#else
return ::lroundl(__lcpp_x);
#endif
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, long>::type
lround(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_lround)
return __builtin_lround((double)__lcpp_x);
#else
return ::lround((double)__lcpp_x);
#endif
}





inline _LIBCPP_INLINE_VISIBILITY float nearbyint(float __lcpp_x) _NOEXCEPT {return ::nearbyintf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double nearbyint(long double __lcpp_x) _NOEXCEPT {return ::nearbyintl(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
nearbyint(_A1 __lcpp_x) _NOEXCEPT {return ::nearbyint((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float nextafter(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::nextafterf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double nextafter(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::nextafterl(__lcpp_x, __lcpp_y);}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
nextafter(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::nextafter((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



inline _LIBCPP_INLINE_VISIBILITY float nexttoward(float __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::nexttowardf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double nexttoward(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::nexttowardl(__lcpp_x, __lcpp_y);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
nexttoward(_A1 __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::nexttoward((double)__lcpp_x, __lcpp_y);}



inline _LIBCPP_INLINE_VISIBILITY float remainder(float __lcpp_x, float __lcpp_y) _NOEXCEPT {return ::remainderf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double remainder(long double __lcpp_x, long double __lcpp_y) _NOEXCEPT {return ::remainderl(__lcpp_x, __lcpp_y);}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
remainder(_A1 __lcpp_x, _A2 __lcpp_y) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::remainder((__result_type)__lcpp_x, (__result_type)__lcpp_y);
}



inline _LIBCPP_INLINE_VISIBILITY float remquo(float __lcpp_x, float __lcpp_y, int* __lcpp_z) _NOEXCEPT {return ::remquof(__lcpp_x, __lcpp_y, __lcpp_z);}
inline _LIBCPP_INLINE_VISIBILITY long double remquo(long double __lcpp_x, long double __lcpp_y, int* __lcpp_z) _NOEXCEPT {return ::remquol(__lcpp_x, __lcpp_y, __lcpp_z);}

template <class _A1, class _A2>
inline _LIBCPP_INLINE_VISIBILITY
typename std::_EnableIf
<
std::is_arithmetic<_A1>::value &&
std::is_arithmetic<_A2>::value,
std::__promote<_A1, _A2>
>::type
remquo(_A1 __lcpp_x, _A2 __lcpp_y, int* __lcpp_z) _NOEXCEPT
{
typedef typename std::__promote<_A1, _A2>::type __result_type;
static_assert((!(std::_IsSame<_A1, __result_type>::value &&
std::_IsSame<_A2, __result_type>::value)), "");
return ::remquo((__result_type)__lcpp_x, (__result_type)__lcpp_y, __lcpp_z);
}



inline _LIBCPP_INLINE_VISIBILITY float rint(float __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_rintf)
return __builtin_rintf(__lcpp_x);
#else
return ::rintf(__lcpp_x);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long double rint(long double __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_rintl)
return __builtin_rintl(__lcpp_x);
#else
return ::rintl(__lcpp_x);
#endif
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
rint(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_rint)
return __builtin_rint((double)__lcpp_x);
#else
return ::rint((double)__lcpp_x);
#endif
}



inline _LIBCPP_INLINE_VISIBILITY float round(float __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_round)
return __builtin_round(__lcpp_x);
#else
return ::round(__lcpp_x);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long double round(long double __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_roundl)
return __builtin_roundl(__lcpp_x);
#else
return ::roundl(__lcpp_x);
#endif
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
round(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_round)
return __builtin_round((double)__lcpp_x);
#else
return ::round((double)__lcpp_x);
#endif
}



inline _LIBCPP_INLINE_VISIBILITY float scalbln(float __lcpp_x, long __lcpp_y) _NOEXCEPT {return ::scalblnf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double scalbln(long double __lcpp_x, long __lcpp_y) _NOEXCEPT {return ::scalblnl(__lcpp_x, __lcpp_y);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
scalbln(_A1 __lcpp_x, long __lcpp_y) _NOEXCEPT {return ::scalbln((double)__lcpp_x, __lcpp_y);}



inline _LIBCPP_INLINE_VISIBILITY float scalbn(float __lcpp_x, int __lcpp_y) _NOEXCEPT {return ::scalbnf(__lcpp_x, __lcpp_y);}
inline _LIBCPP_INLINE_VISIBILITY long double scalbn(long double __lcpp_x, int __lcpp_y) _NOEXCEPT {return ::scalbnl(__lcpp_x, __lcpp_y);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
scalbn(_A1 __lcpp_x, int __lcpp_y) _NOEXCEPT {return ::scalbn((double)__lcpp_x, __lcpp_y);}



inline _LIBCPP_INLINE_VISIBILITY float tgamma(float __lcpp_x) _NOEXCEPT {return ::tgammaf(__lcpp_x);}
inline _LIBCPP_INLINE_VISIBILITY long double tgamma(long double __lcpp_x) _NOEXCEPT {return ::tgammal(__lcpp_x);}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
tgamma(_A1 __lcpp_x) _NOEXCEPT {return ::tgamma((double)__lcpp_x);}



inline _LIBCPP_INLINE_VISIBILITY float trunc(float __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_trunc)
return __builtin_trunc(__lcpp_x);
#else
return ::trunc(__lcpp_x);
#endif
}
inline _LIBCPP_INLINE_VISIBILITY long double trunc(long double __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_truncl)
return __builtin_truncl(__lcpp_x);
#else
return ::truncl(__lcpp_x);
#endif
}

template <class _A1>
inline _LIBCPP_INLINE_VISIBILITY
typename std::enable_if<std::is_integral<_A1>::value, double>::type
trunc(_A1 __lcpp_x) _NOEXCEPT
{
#if __has_builtin(__builtin_trunc)
return __builtin_trunc((double)__lcpp_x);
#else
return ::trunc((double)__lcpp_x);
#endif
}

}

#endif

#else









#if defined(_LIBCPP_MSVCRT) && defined(_USE_MATH_DEFINES)
#include_next <math.h>
#endif

#endif
