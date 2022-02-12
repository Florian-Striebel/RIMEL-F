








#if !defined(_LIBCPP___ITERATOR_ERASE_IF_CONTAINER_H)
#define _LIBCPP___ITERATOR_ERASE_IF_CONTAINER_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Container, class _Predicate>
_LIBCPP_HIDE_FROM_ABI
typename _Container::size_type
__libcpp_erase_if_container(_Container& __c, _Predicate& __pred) {
typename _Container::size_type __old_size = __c.size();

const typename _Container::iterator __last = __c.end();
for (typename _Container::iterator __iter = __c.begin(); __iter != __last;) {
if (__pred(*__iter))
__iter = __c.erase(__iter);
else
++__iter;
}

return __old_size - __c.size();
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
