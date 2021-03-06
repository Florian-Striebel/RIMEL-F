







#if !defined(_LIBCPP___ALGORITHM_TRANSFORM_H)
#define _LIBCPP___ALGORITHM_TRANSFORM_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _InputIterator, class _OutputIterator, class _UnaryOperation>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
_OutputIterator
transform(_InputIterator __first, _InputIterator __last, _OutputIterator __result, _UnaryOperation __op)
{
for (; __first != __last; ++__first, (void) ++__result)
*__result = __op(*__first);
return __result;
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator, class _BinaryOperation>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
_OutputIterator
transform(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2,
_OutputIterator __result, _BinaryOperation __binary_op)
{
for (; __first1 != __last1; ++__first1, (void) ++__first2, ++__result)
*__result = __binary_op(*__first1, *__first2);
return __result;
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
