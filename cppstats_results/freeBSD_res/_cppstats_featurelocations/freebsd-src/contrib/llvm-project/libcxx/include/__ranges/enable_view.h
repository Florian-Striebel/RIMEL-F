








#if !defined(_LIBCPP___RANGES_ENABLE_VIEW_H)
#define _LIBCPP___RANGES_ENABLE_VIEW_H

#include <__config>
#include <concepts>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_RANGES)

namespace ranges {

struct view_base { };

template <class _Tp>
inline constexpr bool enable_view = derived_from<_Tp, view_base>;

}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
