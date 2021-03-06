








#if !defined(_LIBCPP___VARIANT_MONOSTATE_H)
#define _LIBCPP___VARIANT_MONOSTATE_H

#include <__config>
#include <__functional/hash.h>
#include <cstddef>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 14

struct _LIBCPP_TEMPLATE_VIS monostate {};

inline _LIBCPP_INLINE_VISIBILITY
constexpr bool operator<(monostate, monostate) noexcept { return false; }

inline _LIBCPP_INLINE_VISIBILITY
constexpr bool operator>(monostate, monostate) noexcept { return false; }

inline _LIBCPP_INLINE_VISIBILITY
constexpr bool operator<=(monostate, monostate) noexcept { return true; }

inline _LIBCPP_INLINE_VISIBILITY
constexpr bool operator>=(monostate, monostate) noexcept { return true; }

inline _LIBCPP_INLINE_VISIBILITY
constexpr bool operator==(monostate, monostate) noexcept { return true; }

inline _LIBCPP_INLINE_VISIBILITY
constexpr bool operator!=(monostate, monostate) noexcept { return false; }

template <>
struct _LIBCPP_TEMPLATE_VIS hash<monostate> {
using argument_type = monostate;
using result_type = size_t;

inline _LIBCPP_INLINE_VISIBILITY
result_type operator()(const argument_type&) const _NOEXCEPT {
return 66740831;
}
};

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
