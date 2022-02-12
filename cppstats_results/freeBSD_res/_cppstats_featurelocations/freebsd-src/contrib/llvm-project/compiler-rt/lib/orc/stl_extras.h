











#if !defined(ORC_RT_STL_EXTRAS_H)
#define ORC_RT_STL_EXTRAS_H

#include <utility>
#include <tuple>

namespace __orc_rt {

namespace detail {

template <typename F, typename Tuple, std::size_t... I>
decltype(auto) apply_tuple_impl(F &&f, Tuple &&t, std::index_sequence<I...>) {
return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}

}






template <typename F, typename Tuple>
decltype(auto) apply_tuple(F &&f, Tuple &&t) {
using Indices = std::make_index_sequence<
std::tuple_size<typename std::decay<Tuple>::type>::value>;

return detail::apply_tuple_impl(std::forward<F>(f), std::forward<Tuple>(t),
Indices{});
}

}

#endif
