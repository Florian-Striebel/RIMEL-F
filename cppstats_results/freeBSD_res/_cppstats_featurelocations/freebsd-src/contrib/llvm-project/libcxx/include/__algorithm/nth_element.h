







#if !defined(_LIBCPP___ALGORITHM_NTH_ELEMENT_H)
#define _LIBCPP___ALGORITHM_NTH_ELEMENT_H

#include <__config>
#include <__algorithm/comp.h>
#include <__algorithm/comp_ref_type.h>
#include <__algorithm/sort.h>
#include <__iterator/iterator_traits.h>
#include <__utility/swap.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template<class _Compare, class _RandomAccessIterator>
_LIBCPP_CONSTEXPR_AFTER_CXX11 bool
__nth_element_find_guard(_RandomAccessIterator& __i, _RandomAccessIterator& __j,
_RandomAccessIterator __m, _Compare __comp)
{

while (true) {
if (__i == --__j) {
return false;
}
if (__comp(*__j, *__m)) {
return true;
}
}
}

template <class _Compare, class _RandomAccessIterator>
_LIBCPP_CONSTEXPR_AFTER_CXX11 void
__nth_element(_RandomAccessIterator __first, _RandomAccessIterator __nth, _RandomAccessIterator __last, _Compare __comp)
{

typedef typename iterator_traits<_RandomAccessIterator>::difference_type difference_type;
const difference_type __limit = 7;
while (true)
{
if (__nth == __last)
return;
difference_type __len = __last - __first;
switch (__len)
{
case 0:
case 1:
return;
case 2:
if (__comp(*--__last, *__first))
swap(*__first, *__last);
return;
case 3:
{
_RandomAccessIterator __m = __first;
_VSTD::__sort3<_Compare>(__first, ++__m, --__last, __comp);
return;
}
}
if (__len <= __limit)
{
_VSTD::__selection_sort<_Compare>(__first, __last, __comp);
return;
}

_RandomAccessIterator __m = __first + __len/2;
_RandomAccessIterator __lm1 = __last;
unsigned __n_swaps = _VSTD::__sort3<_Compare>(__first, __m, --__lm1, __comp);



_RandomAccessIterator __i = __first;
_RandomAccessIterator __j = __lm1;



if (!__comp(*__i, *__m))
{

if (_VSTD::__nth_element_find_guard<_Compare>(__i, __j, __m, __comp)) {
swap(*__i, *__j);
++__n_swaps;
} else {


++__i;
__j = __last;
if (!__comp(*__first, *--__j)) {
while (true) {
if (__i == __j) {
return;
} else if (__comp(*__first, *__i)) {
swap(*__i, *__j);
++__n_swaps;
++__i;
break;
}
++__i;
}
}

if (__i == __j) {
return;
}
while (true) {
while (!__comp(*__first, *__i))
++__i;
while (__comp(*__first, *--__j))
;
if (__i >= __j)
break;
swap(*__i, *__j);
++__n_swaps;
++__i;
}


if (__nth < __i) {
return;
}


__first = __i;
continue;
}
}
++__i;


if (__i < __j)
{

while (true)
{

while (__comp(*__i, *__m))
++__i;

while (!__comp(*--__j, *__m))
;
if (__i >= __j)
break;
swap(*__i, *__j);
++__n_swaps;


if (__m == __i)
__m = __j;
++__i;
}
}

if (__i != __m && __comp(*__m, *__i))
{
swap(*__i, *__m);
++__n_swaps;
}

if (__nth == __i)
return;
if (__n_swaps == 0)
{

if (__nth < __i)
{

__j = __m = __first;
while (true) {
if (++__j == __i) {

return;
}
if (__comp(*__j, *__m)) {

break;
}
__m = __j;
}
}
else
{

__j = __m = __i;
while (true) {
if (++__j == __last) {

return;
}
if (__comp(*__j, *__m)) {

break;
}
__m = __j;
}
}
}

if (__nth < __i)
{

__last = __i;
}
else
{

__first = ++__i;
}
}
}

template <class _RandomAccessIterator, class _Compare>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
nth_element(_RandomAccessIterator __first, _RandomAccessIterator __nth, _RandomAccessIterator __last, _Compare __comp)
{
typedef typename __comp_ref_type<_Compare>::type _Comp_ref;
_VSTD::__nth_element<_Comp_ref>(__first, __nth, __last, __comp);
}

template <class _RandomAccessIterator>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
nth_element(_RandomAccessIterator __first, _RandomAccessIterator __nth, _RandomAccessIterator __last)
{
_VSTD::nth_element(__first, __nth, __last, __less<typename iterator_traits<_RandomAccessIterator>::value_type>());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
