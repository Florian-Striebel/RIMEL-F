







#if !defined(_LIBCPP___UTILITY_DECLVAL_H)
#define _LIBCPP___UTILITY_DECLVAL_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD



_LIBCPP_SUPPRESS_DEPRECATED_PUSH
template <class _Tp>
_Tp&& __declval(int);
template <class _Tp>
_Tp __declval(long);
_LIBCPP_SUPPRESS_DEPRECATED_POP

template <class _Tp>
decltype(__declval<_Tp>(0)) declval() _NOEXCEPT;

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
