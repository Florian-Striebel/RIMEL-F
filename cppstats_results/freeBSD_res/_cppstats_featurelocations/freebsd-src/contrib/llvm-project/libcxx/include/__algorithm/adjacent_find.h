








#if !defined(_LIBCPP___ALGORITHM_ADJACENT_FIND_H)
#define _LIBCPP___ALGORITHM_ADJACENT_FIND_H

#include <__config>
#include <__algorithm/comp.h>
#include <__iterator/iterator_traits.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ForwardIterator, class _BinaryPredicate>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 _ForwardIterator
adjacent_find(_ForwardIterator __first, _ForwardIterator __last, _BinaryPredicate __pred) {
if (__first != __last) {
_ForwardIterator __i = __first;
while (++__i != __last) {
if (__pred(*__first, *__i))
return __first;
__first = __i;
}
}
return __last;
}

template <class _ForwardIterator>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 _ForwardIterator
adjacent_find(_ForwardIterator __first, _ForwardIterator __last) {
typedef typename iterator_traits<_ForwardIterator>::value_type __v;
return _VSTD::adjacent_find(__first, __last, __equal_to<__v>());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
