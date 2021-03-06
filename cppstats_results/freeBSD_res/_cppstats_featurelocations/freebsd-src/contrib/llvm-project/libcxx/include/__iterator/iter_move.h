








#if !defined(_LIBCPP___ITERATOR_ITER_MOVE_H)
#define _LIBCPP___ITERATOR_ITER_MOVE_H

#include <__config>
#include <__iterator/iterator_traits.h>
#include <__utility/forward.h>
#include <concepts>
#include <type_traits>
#include <utility>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_RANGES)

namespace ranges::__iter_move {
void iter_move();

template<class _Ip>
concept __unqualified_iter_move = requires(_Ip&& __i) {
iter_move(_VSTD::forward<_Ip>(__i));
};





struct __fn {



template<class _Ip>
requires __class_or_enum<remove_cvref_t<_Ip>> && __unqualified_iter_move<_Ip>
[[nodiscard]] _LIBCPP_HIDE_FROM_ABI constexpr decltype(auto) operator()(_Ip&& __i) const
noexcept(noexcept(iter_move(_VSTD::forward<_Ip>(__i))))
{
return iter_move(_VSTD::forward<_Ip>(__i));
}





template<class _Ip>
requires (!(__class_or_enum<remove_cvref_t<_Ip>> && __unqualified_iter_move<_Ip>)) &&
requires(_Ip&& __i) { *_VSTD::forward<_Ip>(__i); }
[[nodiscard]] _LIBCPP_HIDE_FROM_ABI constexpr decltype(auto) operator()(_Ip&& __i) const
noexcept(noexcept(*_VSTD::forward<_Ip>(__i)))
{
if constexpr (is_lvalue_reference_v<decltype(*_VSTD::forward<_Ip>(__i))>) {
return _VSTD::move(*_VSTD::forward<_Ip>(__i));
} else {
return *_VSTD::forward<_Ip>(__i);
}
}



};
}

namespace ranges::inline __cpo {
inline constexpr auto iter_move = __iter_move::__fn{};
}

template<__dereferenceable _Tp>
requires requires(_Tp& __t) { { ranges::iter_move(__t) } -> __referenceable; }
using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<_Tp&>()));

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
