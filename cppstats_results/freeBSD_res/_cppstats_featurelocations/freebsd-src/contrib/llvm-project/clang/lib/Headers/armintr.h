









#if !defined(_MSC_VER)
#include_next <armintr.h>
#else

#if !defined(__ARMINTR_H)
#define __ARMINTR_H

typedef enum
{
_ARM_BARRIER_SY = 0xF,
_ARM_BARRIER_ST = 0xE,
_ARM_BARRIER_ISH = 0xB,
_ARM_BARRIER_ISHST = 0xA,
_ARM_BARRIER_NSH = 0x7,
_ARM_BARRIER_NSHST = 0x6,
_ARM_BARRIER_OSH = 0x3,
_ARM_BARRIER_OSHST = 0x2
} _ARMINTR_BARRIER_TYPE;

#endif
#endif
