








#if !defined(_LIBCPP___ALGORITHM_COUNT_H)
#define _LIBCPP___ALGORITHM_COUNT_H

#include <__config>
#include <__iterator/iterator_traits.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _InputIterator, class _Tp>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
typename iterator_traits<_InputIterator>::difference_type
count(_InputIterator __first, _InputIterator __last, const _Tp& __value_) {
typename iterator_traits<_InputIterator>::difference_type __r(0);
for (; __first != __last; ++__first)
if (*__first == __value_)
++__r;
return __r;
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
