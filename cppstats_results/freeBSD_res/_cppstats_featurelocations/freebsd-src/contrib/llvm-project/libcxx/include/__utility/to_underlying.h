








#if !defined(_LIBCPP___UTILITY_TO_UNDERLYING_H)
#define _LIBCPP___UTILITY_TO_UNDERLYING_H

#include <__config>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_CXX03_LANG)
template <class _Tp>
_LIBCPP_INLINE_VISIBILITY constexpr typename underlying_type<_Tp>::type
__to_underlying(_Tp __val) noexcept {
return static_cast<typename underlying_type<_Tp>::type>(__val);
}
#endif

#if _LIBCPP_STD_VER > 20
template <class _Tp>
_LIBCPP_NODISCARD_EXT _LIBCPP_INLINE_VISIBILITY constexpr underlying_type_t<_Tp>
to_underlying(_Tp __val) noexcept {
return _VSTD::__to_underlying(__val);
}
#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
