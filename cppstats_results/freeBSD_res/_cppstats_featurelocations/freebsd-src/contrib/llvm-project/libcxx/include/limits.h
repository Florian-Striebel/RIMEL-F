








#if !defined(_LIBCPP_LIMITS_H)
#define _LIBCPP_LIMITS_H




























#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#if !defined(__GNUC__)
#include_next <limits.h>
#else






#define _GCC_LIMITS_H_
#define _GCC_NEXT_LIMITS_H
#include_next <limits.h>


#undef _GCC_LIMITS_H_
#include_next <limits.h>
#endif

#endif
