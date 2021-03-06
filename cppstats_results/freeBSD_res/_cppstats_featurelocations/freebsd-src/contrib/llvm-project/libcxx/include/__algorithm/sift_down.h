







#if !defined(_LIBCPP___ALGORITHM_SIFT_DOWN_H)
#define _LIBCPP___ALGORITHM_SIFT_DOWN_H

#include <__config>
#include <__iterator/iterator_traits.h>
#include <__utility/move.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Compare, class _RandomAccessIterator>
_LIBCPP_CONSTEXPR_AFTER_CXX11 void
__sift_down(_RandomAccessIterator __first, _RandomAccessIterator ,
_Compare __comp,
typename iterator_traits<_RandomAccessIterator>::difference_type __len,
_RandomAccessIterator __start)
{
typedef typename iterator_traits<_RandomAccessIterator>::difference_type difference_type;
typedef typename iterator_traits<_RandomAccessIterator>::value_type value_type;


difference_type __child = __start - __first;

if (__len < 2 || (__len - 2) / 2 < __child)
return;

__child = 2 * __child + 1;
_RandomAccessIterator __child_i = __first + __child;

if ((__child + 1) < __len && __comp(*__child_i, *(__child_i + 1))) {

++__child_i;
++__child;
}


if (__comp(*__child_i, *__start))

return;

value_type __top(_VSTD::move(*__start));
do
{

*__start = _VSTD::move(*__child_i);
__start = __child_i;

if ((__len - 2) / 2 < __child)
break;


__child = 2 * __child + 1;
__child_i = __first + __child;

if ((__child + 1) < __len && __comp(*__child_i, *(__child_i + 1))) {

++__child_i;
++__child;
}


} while (!__comp(*__child_i, __top));
*__start = _VSTD::move(__top);
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
