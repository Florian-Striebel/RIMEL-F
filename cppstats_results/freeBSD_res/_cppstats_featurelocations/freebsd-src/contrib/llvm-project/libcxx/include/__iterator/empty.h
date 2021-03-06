








#if !defined(_LIBCPP___ITERATOR_EMPTY_H)
#define _LIBCPP___ITERATOR_EMPTY_H

#include <__config>
#include <cstddef>
#include <initializer_list>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 14

template <class _Cont>
_LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY
constexpr auto empty(const _Cont& __c)
_NOEXCEPT_(noexcept(__c.empty()))
-> decltype (__c.empty())
{ return __c.empty(); }

template <class _Tp, size_t _Sz>
_LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY
constexpr bool empty(const _Tp (&)[_Sz]) noexcept { return false; }

template <class _Ep>
_LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY
constexpr bool empty(initializer_list<_Ep> __il) noexcept { return __il.size() == 0; }

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
