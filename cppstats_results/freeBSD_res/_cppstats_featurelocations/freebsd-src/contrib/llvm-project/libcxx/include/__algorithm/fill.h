







#if !defined(_LIBCPP___ALGORITHM_FILL_H)
#define _LIBCPP___ALGORITHM_FILL_H

#include <__config>
#include <__algorithm/fill_n.h>
#include <__iterator/iterator_traits.h>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ForwardIterator, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
__fill(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value_, forward_iterator_tag)
{
for (; __first != __last; ++__first)
*__first = __value_;
}

template <class _RandomAccessIterator, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
__fill(_RandomAccessIterator __first, _RandomAccessIterator __last, const _Tp& __value_, random_access_iterator_tag)
{
_VSTD::fill_n(__first, __last - __first, __value_);
}

template <class _ForwardIterator, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
fill(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value_)
{
_VSTD::__fill(__first, __last, __value_, typename iterator_traits<_ForwardIterator>::iterator_category());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
