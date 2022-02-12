







#if !defined(_LIBCPP_STDBOOL_H)
#define _LIBCPP_STDBOOL_H











#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <stdbool.h>

#if defined(__cplusplus)
#undef bool
#undef true
#undef false
#undef __bool_true_false_are_defined
#define __bool_true_false_are_defined 1
#endif

#endif
