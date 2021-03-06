







#if !defined(_LIBCPP___ALGORITHM_IS_SORTED_H)
#define _LIBCPP___ALGORITHM_IS_SORTED_H

#include <__algorithm/comp.h>
#include <__algorithm/is_sorted_until.h>
#include <__config>
#include <__iterator/iterator_traits.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ForwardIterator, class _Compare>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
bool
is_sorted(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
return _VSTD::is_sorted_until(__first, __last, __comp) == __last;
}

template<class _ForwardIterator>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
bool
is_sorted(_ForwardIterator __first, _ForwardIterator __last)
{
return _VSTD::is_sorted(__first, __last, __less<typename iterator_traits<_ForwardIterator>::value_type>());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
