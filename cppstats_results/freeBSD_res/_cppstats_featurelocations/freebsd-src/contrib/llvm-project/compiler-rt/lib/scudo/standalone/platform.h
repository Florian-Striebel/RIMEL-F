







#if !defined(SCUDO_PLATFORM_H_)
#define SCUDO_PLATFORM_H_


#include <stdint.h>

#if defined(__linux__) && !defined(__TRUSTY__)
#define SCUDO_LINUX 1
#else
#define SCUDO_LINUX 0
#endif


#if defined(__BIONIC__)
#define SCUDO_ANDROID 1
#else
#define SCUDO_ANDROID 0
#endif

#if defined(__Fuchsia__)
#define SCUDO_FUCHSIA 1
#else
#define SCUDO_FUCHSIA 0
#endif

#if defined(__TRUSTY__)
#define SCUDO_TRUSTY 1
#else
#define SCUDO_TRUSTY 0
#endif

#if __LP64__
#define SCUDO_WORDSIZE 64U
#else
#define SCUDO_WORDSIZE 32U
#endif

#if SCUDO_WORDSIZE == 64U
#define FIRST_32_SECOND_64(a, b) (b)
#else
#define FIRST_32_SECOND_64(a, b) (a)
#endif

#if !defined(SCUDO_CAN_USE_PRIMARY64)
#define SCUDO_CAN_USE_PRIMARY64 (SCUDO_WORDSIZE == 64U)
#endif

#if !defined(SCUDO_MIN_ALIGNMENT_LOG)




#define SCUDO_MIN_ALIGNMENT_LOG FIRST_32_SECOND_64(3, 4)
#endif

#if defined(__aarch64__)
#define SCUDO_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 48)
#else
#define SCUDO_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 47)
#endif



#if defined(__powerpc__) || defined(__powerpc64__)
#define SCUDO_CACHE_LINE_SIZE 128
#else
#define SCUDO_CACHE_LINE_SIZE 64
#endif

#define SCUDO_POINTER_FORMAT_LENGTH FIRST_32_SECOND_64(8, 12)

#endif
