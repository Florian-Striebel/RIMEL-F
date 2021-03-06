







#if !defined(_LIBCPP___ALGORITHM_STABLE_PARTITION_H)
#define _LIBCPP___ALGORITHM_STABLE_PARTITION_H

#include <__config>
#include <__algorithm/rotate.h>
#include <__iterator/iterator_traits.h>
#include <__utility/swap.h>
#include <memory>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Predicate, class _ForwardIterator, class _Distance, class _Pair>
_ForwardIterator
__stable_partition(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred,
_Distance __len, _Pair __p, forward_iterator_tag __fit)
{


if (__len == 1)
return __first;
if (__len == 2)
{
_ForwardIterator __m = __first;
if (__pred(*++__m))
{
swap(*__first, *__m);
return __m;
}
return __first;
}
if (__len <= __p.second)
{
typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
__destruct_n __d(0);
unique_ptr<value_type, __destruct_n&> __h(__p.first, __d);


value_type* __t = __p.first;
::new ((void*)__t) value_type(_VSTD::move(*__first));
__d.template __incr<value_type>();
++__t;
_ForwardIterator __i = __first;
while (++__i != __last)
{
if (__pred(*__i))
{
*__first = _VSTD::move(*__i);
++__first;
}
else
{
::new ((void*)__t) value_type(_VSTD::move(*__i));
__d.template __incr<value_type>();
++__t;
}
}


__i = __first;
for (value_type* __t2 = __p.first; __t2 < __t; ++__t2, (void) ++__i)
*__i = _VSTD::move(*__t2);

return __first;
}


_ForwardIterator __m = __first;
_Distance __len2 = __len / 2;
_VSTD::advance(__m, __len2);



typedef typename add_lvalue_reference<_Predicate>::type _PredRef;
_ForwardIterator __first_false = _VSTD::__stable_partition<_PredRef>(__first, __m, __pred, __len2, __p, __fit);



_ForwardIterator __m1 = __m;
_ForwardIterator __second_false = __last;
_Distance __len_half = __len - __len2;
while (__pred(*__m1))
{
if (++__m1 == __last)
goto __second_half_done;
--__len_half;
}


__second_false = _VSTD::__stable_partition<_PredRef>(__m1, __last, __pred, __len_half, __p, __fit);
__second_half_done:


return _VSTD::rotate(__first_false, __m, __second_false);


}

template <class _Predicate, class _ForwardIterator>
_ForwardIterator
__stable_partition(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred,
forward_iterator_tag)
{
const unsigned __alloc_limit = 3;

while (true)
{
if (__first == __last)
return __first;
if (!__pred(*__first))
break;
++__first;
}


typedef typename iterator_traits<_ForwardIterator>::difference_type difference_type;
typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
difference_type __len = _VSTD::distance(__first, __last);
pair<value_type*, ptrdiff_t> __p(0, 0);
unique_ptr<value_type, __return_temporary_buffer> __h;
if (__len >= __alloc_limit)
{
__p = _VSTD::get_temporary_buffer<value_type>(__len);
__h.reset(__p.first);
}
return _VSTD::__stable_partition<typename add_lvalue_reference<_Predicate>::type>
(__first, __last, __pred, __len, __p, forward_iterator_tag());
}

template <class _Predicate, class _BidirectionalIterator, class _Distance, class _Pair>
_BidirectionalIterator
__stable_partition(_BidirectionalIterator __first, _BidirectionalIterator __last, _Predicate __pred,
_Distance __len, _Pair __p, bidirectional_iterator_tag __bit)
{



if (__len == 2)
{
swap(*__first, *__last);
return __last;
}
if (__len == 3)
{
_BidirectionalIterator __m = __first;
if (__pred(*++__m))
{
swap(*__first, *__m);
swap(*__m, *__last);
return __last;
}
swap(*__m, *__last);
swap(*__first, *__m);
return __m;
}
if (__len <= __p.second)
{
typedef typename iterator_traits<_BidirectionalIterator>::value_type value_type;
__destruct_n __d(0);
unique_ptr<value_type, __destruct_n&> __h(__p.first, __d);


value_type* __t = __p.first;
::new ((void*)__t) value_type(_VSTD::move(*__first));
__d.template __incr<value_type>();
++__t;
_BidirectionalIterator __i = __first;
while (++__i != __last)
{
if (__pred(*__i))
{
*__first = _VSTD::move(*__i);
++__first;
}
else
{
::new ((void*)__t) value_type(_VSTD::move(*__i));
__d.template __incr<value_type>();
++__t;
}
}

*__first = _VSTD::move(*__i);
__i = ++__first;


for (value_type* __t2 = __p.first; __t2 < __t; ++__t2, (void) ++__i)
*__i = _VSTD::move(*__t2);

return __first;
}


_BidirectionalIterator __m = __first;
_Distance __len2 = __len / 2;
_VSTD::advance(__m, __len2);



_BidirectionalIterator __m1 = __m;
_BidirectionalIterator __first_false = __first;
_Distance __len_half = __len2;
while (!__pred(*--__m1))
{
if (__m1 == __first)
goto __first_half_done;
--__len_half;
}


typedef typename add_lvalue_reference<_Predicate>::type _PredRef;
__first_false = _VSTD::__stable_partition<_PredRef>(__first, __m1, __pred, __len_half, __p, __bit);
__first_half_done:



__m1 = __m;
_BidirectionalIterator __second_false = __last;
++__second_false;
__len_half = __len - __len2;
while (__pred(*__m1))
{
if (++__m1 == __last)
goto __second_half_done;
--__len_half;
}


__second_false = _VSTD::__stable_partition<_PredRef>(__m1, __last, __pred, __len_half, __p, __bit);
__second_half_done:


return _VSTD::rotate(__first_false, __m, __second_false);


}

template <class _Predicate, class _BidirectionalIterator>
_BidirectionalIterator
__stable_partition(_BidirectionalIterator __first, _BidirectionalIterator __last, _Predicate __pred,
bidirectional_iterator_tag)
{
typedef typename iterator_traits<_BidirectionalIterator>::difference_type difference_type;
typedef typename iterator_traits<_BidirectionalIterator>::value_type value_type;
const difference_type __alloc_limit = 4;

while (true)
{
if (__first == __last)
return __first;
if (!__pred(*__first))
break;
++__first;
}


do
{
if (__first == --__last)
return __first;
} while (!__pred(*__last));




difference_type __len = _VSTD::distance(__first, __last) + 1;
pair<value_type*, ptrdiff_t> __p(0, 0);
unique_ptr<value_type, __return_temporary_buffer> __h;
if (__len >= __alloc_limit)
{
__p = _VSTD::get_temporary_buffer<value_type>(__len);
__h.reset(__p.first);
}
return _VSTD::__stable_partition<typename add_lvalue_reference<_Predicate>::type>
(__first, __last, __pred, __len, __p, bidirectional_iterator_tag());
}

template <class _ForwardIterator, class _Predicate>
inline _LIBCPP_INLINE_VISIBILITY
_ForwardIterator
stable_partition(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred)
{
return _VSTD::__stable_partition<typename add_lvalue_reference<_Predicate>::type>
(__first, __last, __pred, typename iterator_traits<_ForwardIterator>::iterator_category());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
