








#if !defined(_LIBCPP___ITERATOR_ITERATOR_H)
#define _LIBCPP___ITERATOR_ITERATOR_H

#include <__config>
#include <cstddef>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template<class _Category, class _Tp, class _Distance = ptrdiff_t,
class _Pointer = _Tp*, class _Reference = _Tp&>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_DEPRECATED_IN_CXX17 iterator
{
typedef _Tp value_type;
typedef _Distance difference_type;
typedef _Pointer pointer;
typedef _Reference reference;
typedef _Category iterator_category;
};

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
