







#if !defined(_LIBCPP___FUNCTIONAL_UNARY_FUNCTION_H)
#define _LIBCPP___FUNCTIONAL_UNARY_FUNCTION_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Arg, class _Result>
struct _LIBCPP_TEMPLATE_VIS unary_function
{
typedef _Arg argument_type;
typedef _Result result_type;
};

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
