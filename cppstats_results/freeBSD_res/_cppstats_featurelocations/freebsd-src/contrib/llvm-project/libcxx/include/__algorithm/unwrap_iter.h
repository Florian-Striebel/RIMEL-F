







#if !defined(_LIBCPP___ALGORITHM_UNWRAP_ITER_H)
#define _LIBCPP___ALGORITHM_UNWRAP_ITER_H

#include <__config>
#include <iterator>
#include <__memory/pointer_traits.h>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD
















template <class _Iter, bool = __is_cpp17_contiguous_iterator<_Iter>::value>
struct __unwrap_iter_impl {
static _LIBCPP_CONSTEXPR _Iter
__apply(_Iter __i) _NOEXCEPT {
return __i;
}
};

#if _LIBCPP_DEBUG_LEVEL < 2

template <class _Iter>
struct __unwrap_iter_impl<_Iter, true> {
static _LIBCPP_CONSTEXPR decltype(_VSTD::__to_address(declval<_Iter>()))
__apply(_Iter __i) _NOEXCEPT {
return _VSTD::__to_address(__i);
}
};

#endif

template<class _Iter, class _Impl = __unwrap_iter_impl<_Iter> >
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
decltype(_Impl::__apply(declval<_Iter>()))
__unwrap_iter(_Iter __i) _NOEXCEPT
{
return _Impl::__apply(__i);
}

template<class _OrigIter>
_OrigIter __rewrap_iter(_OrigIter, _OrigIter __result)
{
return __result;
}

template<class _OrigIter, class _UnwrappedIter>
_OrigIter __rewrap_iter(_OrigIter __first, _UnwrappedIter __result)
{


return __first + (__result - _VSTD::__unwrap_iter(__first));
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
