








#if !defined(_LIBCPP_INTTYPES_H)



#if !defined(_AIX) || !defined(_STD_TYPES_T)
#define _LIBCPP_INTTYPES_H
#endif





























































































































































































































#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif




#if defined(__cplusplus) && !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS
#endif

#include_next <inttypes.h>

#if defined(__cplusplus)

#include <stdint.h>

#undef imaxabs
#undef imaxdiv

#endif

#endif
