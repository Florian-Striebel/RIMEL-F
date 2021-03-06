








#if !defined(_LIBCPP___ALGORITHM_SEARCH_H)
#define _LIBCPP___ALGORITHM_SEARCH_H

#include <__algorithm/comp.h>
#include <__config>
#include <__iterator/iterator_traits.h>
#include <type_traits>
#include <utility>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _BinaryPredicate, class _ForwardIterator1, class _ForwardIterator2>
pair<_ForwardIterator1, _ForwardIterator1>
_LIBCPP_CONSTEXPR_AFTER_CXX11 __search(_ForwardIterator1 __first1, _ForwardIterator1 __last1,
_ForwardIterator2 __first2, _ForwardIterator2 __last2,
_BinaryPredicate __pred, forward_iterator_tag, forward_iterator_tag) {
if (__first2 == __last2)
return _VSTD::make_pair(__first1, __first1);
while (true) {

while (true) {
if (__first1 == __last1)
return _VSTD::make_pair(__last1, __last1);
if (__pred(*__first1, *__first2))
break;
++__first1;
}

_ForwardIterator1 __m1 = __first1;
_ForwardIterator2 __m2 = __first2;
while (true) {
if (++__m2 == __last2)
return _VSTD::make_pair(__first1, __m1);
if (++__m1 == __last1)
return _VSTD::make_pair(__last1, __last1);
if (!__pred(*__m1, *__m2))
{
++__first1;
break;
}
}
}
}

template <class _BinaryPredicate, class _RandomAccessIterator1, class _RandomAccessIterator2>
_LIBCPP_CONSTEXPR_AFTER_CXX11 pair<_RandomAccessIterator1, _RandomAccessIterator1>
__search(_RandomAccessIterator1 __first1, _RandomAccessIterator1 __last1, _RandomAccessIterator2 __first2,
_RandomAccessIterator2 __last2, _BinaryPredicate __pred, random_access_iterator_tag,
random_access_iterator_tag) {
typedef typename iterator_traits<_RandomAccessIterator1>::difference_type _D1;
typedef typename iterator_traits<_RandomAccessIterator2>::difference_type _D2;

const _D2 __len2 = __last2 - __first2;
if (__len2 == 0)
return _VSTD::make_pair(__first1, __first1);
const _D1 __len1 = __last1 - __first1;
if (__len1 < __len2)
return _VSTD::make_pair(__last1, __last1);
const _RandomAccessIterator1 __s = __last1 - (__len2 - 1);

while (true) {
while (true) {
if (__first1 == __s)
return _VSTD::make_pair(__last1, __last1);
if (__pred(*__first1, *__first2))
break;
++__first1;
}

_RandomAccessIterator1 __m1 = __first1;
_RandomAccessIterator2 __m2 = __first2;
while (true) {
if (++__m2 == __last2)
return _VSTD::make_pair(__first1, __first1 + __len2);
++__m1;
if (!__pred(*__m1, *__m2)) {
++__first1;
break;
}
}
}
}

template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 _ForwardIterator1
search(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2,
_BinaryPredicate __pred) {
return _VSTD::__search<typename add_lvalue_reference<_BinaryPredicate>::type>(
__first1, __last1, __first2, __last2, __pred,
typename iterator_traits<_ForwardIterator1>::iterator_category(),
typename iterator_traits<_ForwardIterator2>::iterator_category()).first;
}

template <class _ForwardIterator1, class _ForwardIterator2>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 _ForwardIterator1
search(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2) {
typedef typename iterator_traits<_ForwardIterator1>::value_type __v1;
typedef typename iterator_traits<_ForwardIterator2>::value_type __v2;
return _VSTD::search(__first1, __last1, __first2, __last2, __equal_to<__v1, __v2>());
}

#if _LIBCPP_STD_VER > 14
template <class _ForwardIterator, class _Searcher>
_LIBCPP_NODISCARD_EXT _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 _ForwardIterator
search(_ForwardIterator __f, _ForwardIterator __l, const _Searcher& __s) {
return __s(__f, __l).first;
}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
