








#if !defined(_LIBCPP___ALGORITHM_ALL_OF_H)
#define _LIBCPP___ALGORITHM_ALL_OF_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _InputIterator, class _Predicate>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 bool
all_of(_InputIterator __first, _InputIterator __last, _Predicate __pred) {
for (; __first != __last; ++__first)
if (!__pred(*__first))
return false;
return true;
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
