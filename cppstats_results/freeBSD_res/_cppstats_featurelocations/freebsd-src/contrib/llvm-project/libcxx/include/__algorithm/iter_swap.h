







#if !defined(_LIBCPP___ALGORITHM_ITER_SWAP_H)
#define _LIBCPP___ALGORITHM_ITER_SWAP_H

#include <__config>
#include <__utility/declval.h>
#include <__utility/swap.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ForwardIterator1, class _ForwardIterator2>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 void iter_swap(_ForwardIterator1 __a,
_ForwardIterator2 __b)

_NOEXCEPT_(_NOEXCEPT_(swap(*declval<_ForwardIterator1>(), *declval<_ForwardIterator2>()))) {
swap(*__a, *__b);
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
