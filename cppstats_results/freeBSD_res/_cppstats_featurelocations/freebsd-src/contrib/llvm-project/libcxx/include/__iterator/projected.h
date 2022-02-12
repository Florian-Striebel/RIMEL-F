







#if !defined(_LIBCPP___ITERATOR_PROJECTED_H)
#define _LIBCPP___ITERATOR_PROJECTED_H

#include <__config>
#include <__iterator/concepts.h>
#include <__iterator/incrementable_traits.h>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_RANGES)

template<indirectly_readable _It, indirectly_regular_unary_invocable<_It> _Proj>
struct projected {
using value_type = remove_cvref_t<indirect_result_t<_Proj&, _It>>;
indirect_result_t<_Proj&, _It> operator*() const;
};

template<weakly_incrementable _It, class _Proj>
struct incrementable_traits<projected<_It, _Proj>> {
using difference_type = iter_difference_t<_It>;
};

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
