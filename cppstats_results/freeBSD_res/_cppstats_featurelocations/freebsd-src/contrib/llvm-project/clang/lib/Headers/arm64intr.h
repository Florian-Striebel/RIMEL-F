









#if !defined(_MSC_VER)
#include_next <arm64intr.h>
#else

#if !defined(__ARM64INTR_H)
#define __ARM64INTR_H

typedef enum
{
_ARM64_BARRIER_SY = 0xF,
_ARM64_BARRIER_ST = 0xE,
_ARM64_BARRIER_LD = 0xD,
_ARM64_BARRIER_ISH = 0xB,
_ARM64_BARRIER_ISHST = 0xA,
_ARM64_BARRIER_ISHLD = 0x9,
_ARM64_BARRIER_NSH = 0x7,
_ARM64_BARRIER_NSHST = 0x6,
_ARM64_BARRIER_NSHLD = 0x5,
_ARM64_BARRIER_OSH = 0x3,
_ARM64_BARRIER_OSHST = 0x2,
_ARM64_BARRIER_OSHLD = 0x1
} _ARM64INTR_BARRIER_TYPE;

#endif
#endif
