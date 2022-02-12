











#define SINGLE_PRECISION
#include "fp_add_impl.inc"

COMPILER_RT_ABI float __addsf3(float a, float b) { return __addXf3__(a, b); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI float __aeabi_fadd(float a, float b) { return __addsf3(a, b); }
#else
COMPILER_RT_ALIAS(__addsf3, __aeabi_fadd)
#endif
#endif
