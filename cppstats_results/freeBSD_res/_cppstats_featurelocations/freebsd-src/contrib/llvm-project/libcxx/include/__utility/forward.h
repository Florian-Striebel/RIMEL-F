








#if !defined(_LIBCPP___UTILITY_FORWARD_H)
#define _LIBCPP___UTILITY_FORWARD_H

#include <__config>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR _Tp&&
forward(typename remove_reference<_Tp>::type& __t) _NOEXCEPT {
return static_cast<_Tp&&>(__t);
}

template <class _Tp>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR _Tp&&
forward(typename remove_reference<_Tp>::type&& __t) _NOEXCEPT {
static_assert(!is_lvalue_reference<_Tp>::value, "cannot forward an rvalue as an lvalue");
return static_cast<_Tp&&>(__t);
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
