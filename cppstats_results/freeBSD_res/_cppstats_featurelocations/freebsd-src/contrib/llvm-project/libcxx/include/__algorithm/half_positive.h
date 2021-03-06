







#if !defined(_LIBCPP___ALGORITHM_HALF_POSITIVE_H)
#define _LIBCPP___ALGORITHM_HALF_POSITIVE_H

#include <__config>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD



template <typename _Integral>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
typename enable_if
<
is_integral<_Integral>::value,
_Integral
>::type
__half_positive(_Integral __value)
{
return static_cast<_Integral>(static_cast<typename make_unsigned<_Integral>::type>(__value) / 2);
}

template <typename _Tp>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
typename enable_if
<
!is_integral<_Tp>::value,
_Tp
>::type
__half_positive(_Tp __value)
{
return __value / 2;
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
