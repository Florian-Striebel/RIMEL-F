







#if !defined(_LIBCPP___ALGORITHM_GENERATE_H)
#define _LIBCPP___ALGORITHM_GENERATE_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _ForwardIterator, class _Generator>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void
generate(_ForwardIterator __first, _ForwardIterator __last, _Generator __gen)
{
for (; __first != __last; ++__first)
*__first = __gen();
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
