








#if !defined(_LIBCPP_SETJMP_H)
#define _LIBCPP_SETJMP_H
















#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <setjmp.h>

#if defined(__cplusplus)

#if !defined(setjmp)
#define setjmp(env) setjmp(env)
#endif

#endif

#endif
