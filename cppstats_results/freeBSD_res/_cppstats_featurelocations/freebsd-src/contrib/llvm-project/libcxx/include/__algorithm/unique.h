







#if !defined(_LIBCPP___ALGORITHM_UNIQUE_H)
#define _LIBCPP___ALGORITHM_UNIQUE_H

#include <__config>
#include <__algorithm/comp.h>
#include <__algorithm/adjacent_find.h>
#include <__iterator/iterator_traits.h>
#include <__utility/move.h>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD



template <class _ForwardIterator, class _BinaryPredicate>
_LIBCPP_NODISCARD_EXT _LIBCPP_CONSTEXPR_AFTER_CXX17 _ForwardIterator
unique(_ForwardIterator __first, _ForwardIterator __last, _BinaryPredicate __pred)
{
__first = _VSTD::adjacent_find<_ForwardIterator, typename add_lvalue_reference<_BinaryPredicate>::type>
(__first, __last, __pred);
if (__first != __last)
{


_ForwardIterator __i = __first;
for (++__i; ++__i != __last;)
if (!__pred(*__first, *__i))
*++__first = _VSTD::move(*__i);
++__first;
}
return __first;
}

template <class _ForwardIterator>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
_ForwardIterator
unique(_ForwardIterator __first, _ForwardIterator __last)
{
typedef typename iterator_traits<_ForwardIterator>::value_type __v;
return _VSTD::unique(__first, __last, __equal_to<__v>());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
