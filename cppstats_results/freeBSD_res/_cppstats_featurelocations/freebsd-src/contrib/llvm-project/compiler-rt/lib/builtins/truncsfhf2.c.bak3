







#define SRC_SINGLE
#define DST_HALF
#include "fp_trunc_impl.inc"



COMPILER_RT_ABI NOINLINE dst_t __truncsfhf2(float a) {
return __truncXfYf2__(a);
}

COMPILER_RT_ABI dst_t __gnu_f2h_ieee(float a) { return __truncsfhf2(a); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI dst_t __aeabi_f2h(float a) { return __truncsfhf2(a); }
#else
COMPILER_RT_ALIAS(__truncsfhf2, __aeabi_f2h)
#endif
#endif
