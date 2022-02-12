








#if !defined(_LIBCPP_FLOAT_H)
#define _LIBCPP_FLOAT_H





























































#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <float.h>

#if defined(__cplusplus)

#if !defined(FLT_EVAL_METHOD)
#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
#endif

#if !defined(DECIMAL_DIG)
#define DECIMAL_DIG __DECIMAL_DIG__
#endif

#endif

#endif
