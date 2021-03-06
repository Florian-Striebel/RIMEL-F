












#if !defined(INT_ENDIANNESS_H)
#define INT_ENDIANNESS_H

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)



#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _YUGA_LITTLE_ENDIAN 0
#define _YUGA_BIG_ENDIAN 1
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN 0
#endif

#else

#if defined(__SVR4) && defined(__sun)
#include <sys/byteorder.h>

#if defined(_BIG_ENDIAN)
#define _YUGA_LITTLE_ENDIAN 0
#define _YUGA_BIG_ENDIAN 1
#elif defined(_LITTLE_ENDIAN)
#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN 0
#else
#error "unknown endianness"
#endif

#endif



#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__minix)

#include <sys/endian.h>

#if _BYTE_ORDER == _BIG_ENDIAN
#define _YUGA_LITTLE_ENDIAN 0
#define _YUGA_BIG_ENDIAN 1
#elif _BYTE_ORDER == _LITTLE_ENDIAN
#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN 0
#endif

#endif

#if defined(__OpenBSD__)
#include <machine/endian.h>

#if _BYTE_ORDER == _BIG_ENDIAN
#define _YUGA_LITTLE_ENDIAN 0
#define _YUGA_BIG_ENDIAN 1
#elif _BYTE_ORDER == _LITTLE_ENDIAN
#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN 0
#endif

#endif





#if defined(__APPLE__) || defined(__ellcc__)

#if defined(__BIG_ENDIAN__)
#if __BIG_ENDIAN__
#define _YUGA_LITTLE_ENDIAN 0
#define _YUGA_BIG_ENDIAN 1
#endif
#endif

#if defined(__LITTLE_ENDIAN__)
#if __LITTLE_ENDIAN__
#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN 0
#endif
#endif

#endif



#if defined(_WIN32)

#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN 0

#endif

#endif



#if !defined(_YUGA_LITTLE_ENDIAN) || !defined(_YUGA_BIG_ENDIAN)
#error Unable to determine endian
#endif

#endif
