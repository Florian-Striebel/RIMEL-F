








#if !defined(_LIBCPP___RANGES_DANGLING_H)
#define _LIBCPP___RANGES_DANGLING_H

#include <__config>
#include <__ranges/access.h>
#include <__ranges/concepts.h>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_RANGES)

namespace ranges {
struct dangling {
dangling() = default;
_LIBCPP_HIDE_FROM_ABI constexpr dangling(auto&&...) noexcept {}
};

template <range _Rp>
using borrowed_iterator_t = _If<borrowed_range<_Rp>, iterator_t<_Rp>, dangling>;


}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
