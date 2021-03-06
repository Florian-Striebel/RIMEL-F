








#if !defined(_LIBCPP___UTILITY_MOVE_H)
#define _LIBCPP___UTILITY_MOVE_H

#include <__config>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR typename remove_reference<_Tp>::type&&
move(_Tp&& __t) _NOEXCEPT {
typedef _LIBCPP_NODEBUG_TYPE typename remove_reference<_Tp>::type _Up;
return static_cast<_Up&&>(__t);
}

#if !defined(_LIBCPP_CXX03_LANG)
template <class _Tp>
using __move_if_noexcept_result_t =
typename conditional<!is_nothrow_move_constructible<_Tp>::value && is_copy_constructible<_Tp>::value, const _Tp&,
_Tp&&>::type;
#else
template <class _Tp>
using __move_if_noexcept_result_t = const _Tp&;
#endif

template <class _Tp>
_LIBCPP_NODISCARD_EXT inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11 __move_if_noexcept_result_t<_Tp>
move_if_noexcept(_Tp& __x) _NOEXCEPT {
return _VSTD::move(__x);
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
