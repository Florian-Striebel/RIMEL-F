







#define SRC_DOUBLE
#define DST_HALF
#include "fp_trunc_impl.inc"

COMPILER_RT_ABI dst_t __truncdfhf2(double a) { return __truncXfYf2__(a); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI dst_t __aeabi_d2h(double a) { return __truncdfhf2(a); }
#else
COMPILER_RT_ALIAS(__truncdfhf2, __aeabi_d2h)
#endif
#endif
