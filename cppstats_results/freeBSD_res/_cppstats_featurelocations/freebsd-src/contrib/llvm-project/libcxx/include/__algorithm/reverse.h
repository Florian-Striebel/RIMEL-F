







#if !defined(_LIBCPP___ALGORITHM_REVERSE_H)
#define _LIBCPP___ALGORITHM_REVERSE_H

#include <__config>
#include <__algorithm/iter_swap.h>
#include <__iterator/iterator_traits.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _BidirectionalIterator>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
__reverse(_BidirectionalIterator __first, _BidirectionalIterator __last, bidirectional_iterator_tag)
{
while (__first != __last)
{
if (__first == --__last)
break;
_VSTD::iter_swap(__first, __last);
++__first;
}
}

template <class _RandomAccessIterator>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
__reverse(_RandomAccessIterator __first, _RandomAccessIterator __last, random_access_iterator_tag)
{
if (__first != __last)
for (; __first < --__last; ++__first)
_VSTD::iter_swap(__first, __last);
}

template <class _BidirectionalIterator>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
reverse(_BidirectionalIterator __first, _BidirectionalIterator __last)
{
_VSTD::__reverse(__first, __last, typename iterator_traits<_BidirectionalIterator>::iterator_category());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
