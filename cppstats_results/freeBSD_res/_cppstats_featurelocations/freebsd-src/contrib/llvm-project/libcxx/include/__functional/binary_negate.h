








#if !defined(_LIBCPP___FUNCTIONAL_BINARY_NEGATE_H)
#define _LIBCPP___FUNCTIONAL_BINARY_NEGATE_H

#include <__config>
#include <__functional/binary_function.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER <= 17 || defined(_LIBCPP_ENABLE_CXX20_REMOVED_NEGATORS)

template <class _Predicate>
class _LIBCPP_TEMPLATE_VIS _LIBCPP_DEPRECATED_IN_CXX17 binary_negate
: public binary_function<typename _Predicate::first_argument_type,
typename _Predicate::second_argument_type,
bool>
{
_Predicate __pred_;
public:
_LIBCPP_INLINE_VISIBILITY explicit _LIBCPP_CONSTEXPR_AFTER_CXX11
binary_negate(const _Predicate& __pred) : __pred_(__pred) {}

_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
bool operator()(const typename _Predicate::first_argument_type& __x,
const typename _Predicate::second_argument_type& __y) const
{return !__pred_(__x, __y);}
};

template <class _Predicate>
_LIBCPP_DEPRECATED_IN_CXX17 inline _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
binary_negate<_Predicate>
not2(const _Predicate& __pred) {return binary_negate<_Predicate>(__pred);}

#endif

_LIBCPP_END_NAMESPACE_STD

#endif
