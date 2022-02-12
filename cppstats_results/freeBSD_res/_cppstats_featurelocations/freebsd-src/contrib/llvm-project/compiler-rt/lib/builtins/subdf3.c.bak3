











#define DOUBLE_PRECISION
#include "fp_lib.h"


COMPILER_RT_ABI fp_t __subdf3(fp_t a, fp_t b) {
return __adddf3(a, fromRep(toRep(b) ^ signBit));
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_dsub(fp_t a, fp_t b) { return __subdf3(a, b); }
#else
COMPILER_RT_ALIAS(__subdf3, __aeabi_dsub)
#endif
#endif
