







#if !defined(_LIBCPP___ALGORITHM_ROTATE_COPY_H)
#define _LIBCPP___ALGORITHM_ROTATE_COPY_H

#include <__config>
#include <__algorithm/copy.h>
#include <iterator>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ForwardIterator, class _OutputIterator>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
_OutputIterator
rotate_copy(_ForwardIterator __first, _ForwardIterator __middle, _ForwardIterator __last, _OutputIterator __result)
{
return _VSTD::copy(__first, __middle, _VSTD::copy(__middle, __last, __result));
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
