







#if !defined(_LIBCPP___ALGORITHM_REPLACE_COPY_IF_H)
#define _LIBCPP___ALGORITHM_REPLACE_COPY_IF_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _InputIterator, class _OutputIterator, class _Predicate, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
_OutputIterator
replace_copy_if(_InputIterator __first, _InputIterator __last, _OutputIterator __result,
_Predicate __pred, const _Tp& __new_value)
{
for (; __first != __last; ++__first, (void) ++__result)
if (__pred(*__first))
*__result = __new_value;
else
*__result = *__first;
return __result;
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
