







#if !defined(_LIBCPP___ALGORITHM_MIN_H)
#define _LIBCPP___ALGORITHM_MIN_H

#include <__config>
#include <__algorithm/comp.h>
#include <__algorithm/min_element.h>
#include <initializer_list>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp, class _Compare>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
const _Tp&
min(const _Tp& __a, const _Tp& __b, _Compare __comp)
{
return __comp(__b, __a) ? __b : __a;
}

template <class _Tp>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
const _Tp&
min(const _Tp& __a, const _Tp& __b)
{
return _VSTD::min(__a, __b, __less<_Tp>());
}

#if !defined(_LIBCPP_CXX03_LANG)

template<class _Tp, class _Compare>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
_Tp
min(initializer_list<_Tp> __t, _Compare __comp)
{
return *_VSTD::min_element(__t.begin(), __t.end(), __comp);
}

template<class _Tp>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
_Tp
min(initializer_list<_Tp> __t)
{
return *_VSTD::min_element(__t.begin(), __t.end(), __less<_Tp>());
}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
