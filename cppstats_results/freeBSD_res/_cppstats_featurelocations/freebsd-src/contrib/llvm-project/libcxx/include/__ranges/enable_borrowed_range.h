








#if !defined(_LIBCPP___RANGES_ENABLE_BORROWED_RANGE_H)
#define _LIBCPP___RANGES_ENABLE_BORROWED_RANGE_H





#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_RANGES)

namespace ranges
{



template <class>
inline constexpr bool enable_borrowed_range = false;

}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
