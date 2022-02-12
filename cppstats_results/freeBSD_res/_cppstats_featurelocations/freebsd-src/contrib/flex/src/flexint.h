

#if !defined(FLEXINT_H)
#define FLEXINT_H



#if defined(__FreeBSD__) || (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)





#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS 1
#endif

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t;
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;


#if !defined(INT8_MIN)
#define INT8_MIN (-128)
#endif
#if !defined(INT16_MIN)
#define INT16_MIN (-32767-1)
#endif
#if !defined(INT32_MIN)
#define INT32_MIN (-2147483647-1)
#endif
#if !defined(INT8_MAX)
#define INT8_MAX (127)
#endif
#if !defined(INT16_MAX)
#define INT16_MAX (32767)
#endif
#if !defined(INT32_MAX)
#define INT32_MAX (2147483647)
#endif
#if !defined(UINT8_MAX)
#define UINT8_MAX (255U)
#endif
#if !defined(UINT16_MAX)
#define UINT16_MAX (65535U)
#endif
#if !defined(UINT32_MAX)
#define UINT32_MAX (4294967295U)
#endif

#if !defined(SIZE_MAX)
#define SIZE_MAX (~(size_t)0)
#endif

#endif

#endif
