







#if !defined(_LIBCPP___UTILITY_PIECEWISE_CONSTRUCT_H)
#define _LIBCPP___UTILITY_PIECEWISE_CONSTRUCT_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

struct _LIBCPP_TEMPLATE_VIS piecewise_construct_t { explicit piecewise_construct_t() = default; };
#if defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_BUILDING_LIBRARY)
extern _LIBCPP_EXPORTED_FROM_ABI const piecewise_construct_t piecewise_construct;
#else
constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();
#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
