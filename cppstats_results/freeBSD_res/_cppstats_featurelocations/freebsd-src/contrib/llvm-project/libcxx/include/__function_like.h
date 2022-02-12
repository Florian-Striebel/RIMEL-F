








#if !defined(_LIBCPP___ITERATOR_FUNCTION_LIKE_H)
#define _LIBCPP___ITERATOR_FUNCTION_LIKE_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_RANGES)

namespace ranges {








struct __function_like {
__function_like() = delete;
__function_like(__function_like const&) = delete;
__function_like& operator=(__function_like const&) = delete;

void operator&() const = delete;

struct __tag { };

protected:
constexpr explicit __function_like(__tag) noexcept {}
~__function_like() = default;
};
}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
