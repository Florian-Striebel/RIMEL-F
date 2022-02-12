












#define DOUBLE_PRECISION

#define NUMBER_OF_HALF_ITERATIONS 3
#define NUMBER_OF_FULL_ITERATIONS 1

#include "fp_div_impl.inc"

COMPILER_RT_ABI fp_t __divdf3(fp_t a, fp_t b) { return __divXf3__(a, b); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_ddiv(fp_t a, fp_t b) { return __divdf3(a, b); }
#else
COMPILER_RT_ALIAS(__divdf3, __aeabi_ddiv)
#endif
#endif
