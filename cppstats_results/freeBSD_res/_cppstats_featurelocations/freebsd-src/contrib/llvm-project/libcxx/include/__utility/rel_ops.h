







#if !defined(_LIBCPP___UTILITY_REL_OPS_H)
#define _LIBCPP___UTILITY_REL_OPS_H

#include <__config>
#include <__utility/forward.h>
#include <__utility/move.h>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

namespace rel_ops
{

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const _Tp& __x, const _Tp& __y)
{
return !(__x == __y);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator> (const _Tp& __x, const _Tp& __y)
{
return __y < __x;
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const _Tp& __x, const _Tp& __y)
{
return !(__y < __x);
}

template<class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const _Tp& __x, const _Tp& __y)
{
return !(__x < __y);
}

}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
