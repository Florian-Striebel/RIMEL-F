








#if !defined(_LIBCPP___ITERATOR_DEFAULT_SENTINEL_H)
#define _LIBCPP___ITERATOR_DEFAULT_SENTINEL_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_RANGES)

struct default_sentinel_t { };
inline constexpr default_sentinel_t default_sentinel{};

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
