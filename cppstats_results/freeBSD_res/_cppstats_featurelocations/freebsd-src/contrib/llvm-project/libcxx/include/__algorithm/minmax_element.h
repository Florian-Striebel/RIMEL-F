







#if !defined(_LIBCPP___ALGORITHM_MINMAX_ELEMENT_H)
#define _LIBCPP___ALGORITHM_MINMAX_ELEMENT_H

#include <__config>
#include <__algorithm/comp.h>
#include <__iterator/iterator_traits.h>
#include <utility>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ForwardIterator, class _Compare>
_LIBCPP_NODISCARD_EXT _LIBCPP_CONSTEXPR_AFTER_CXX11
pair<_ForwardIterator, _ForwardIterator>
minmax_element(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
static_assert(__is_cpp17_forward_iterator<_ForwardIterator>::value,
"std::minmax_element requires a ForwardIterator");
pair<_ForwardIterator, _ForwardIterator> __result(__first, __first);
if (__first != __last)
{
if (++__first != __last)
{
if (__comp(*__first, *__result.first))
__result.first = __first;
else
__result.second = __first;
while (++__first != __last)
{
_ForwardIterator __i = __first;
if (++__first == __last)
{
if (__comp(*__i, *__result.first))
__result.first = __i;
else if (!__comp(*__i, *__result.second))
__result.second = __i;
break;
}
else
{
if (__comp(*__first, *__i))
{
if (__comp(*__first, *__result.first))
__result.first = __first;
if (!__comp(*__i, *__result.second))
__result.second = __i;
}
else
{
if (__comp(*__i, *__result.first))
__result.first = __i;
if (!__comp(*__first, *__result.second))
__result.second = __first;
}
}
}
}
}
return __result;
}

template <class _ForwardIterator>
_LIBCPP_NODISCARD_EXT inline
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
pair<_ForwardIterator, _ForwardIterator>
minmax_element(_ForwardIterator __first, _ForwardIterator __last)
{
return _VSTD::minmax_element(__first, __last,
__less<typename iterator_traits<_ForwardIterator>::value_type>());
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
