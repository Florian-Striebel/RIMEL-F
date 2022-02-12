












#define SINGLE_PRECISION

#define NUMBER_OF_HALF_ITERATIONS 0
#define NUMBER_OF_FULL_ITERATIONS 3
#define USE_NATIVE_FULL_ITERATIONS

#include "fp_div_impl.inc"

COMPILER_RT_ABI fp_t __divsf3(fp_t a, fp_t b) { return __divXf3__(a, b); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_fdiv(fp_t a, fp_t b) { return __divsf3(a, b); }
#else
COMPILER_RT_ALIAS(__divsf3, __aeabi_fdiv)
#endif
#endif
