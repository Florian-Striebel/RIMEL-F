








#if !defined(_LIBCPP___FUNCTIONAL_BINARY_FUNCTION_H)
#define _LIBCPP___FUNCTIONAL_BINARY_FUNCTION_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Arg1, class _Arg2, class _Result>
struct _LIBCPP_TEMPLATE_VIS binary_function
{
typedef _Arg1 first_argument_type;
typedef _Arg2 second_argument_type;
typedef _Result result_type;
};

_LIBCPP_END_NAMESPACE_STD

#endif
