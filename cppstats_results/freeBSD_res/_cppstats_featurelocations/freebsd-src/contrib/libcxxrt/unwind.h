





















#if !defined(UNWIND_H_INCLUDED)
#define UNWIND_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__arm__) && !defined(__ARM_DWARF_EH__)
#include "unwind-arm.h"
#else
#include "unwind-itanium.h"
#endif

#if defined(__cplusplus)
}
#endif

#endif
