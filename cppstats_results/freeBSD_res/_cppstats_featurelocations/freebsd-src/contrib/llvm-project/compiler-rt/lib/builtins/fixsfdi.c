







#define SINGLE_PRECISION
#include "fp_lib.h"

#if !defined(__SOFTFP__)



COMPILER_RT_ABI du_int __fixunssfdi(float a);

COMPILER_RT_ABI di_int __fixsfdi(float a) {
if (a < 0.0f) {
return -__fixunssfdi(-a);
}
return __fixunssfdi(a);
}

#else




typedef di_int fixint_t;
typedef du_int fixuint_t;
#include "fp_fixint_impl.inc"

COMPILER_RT_ABI di_int __fixsfdi(fp_t a) { return __fixint(a); }

#endif

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI di_int __aeabi_f2lz(fp_t a) { return __fixsfdi(a); }
#else
COMPILER_RT_ALIAS(__fixsfdi, __aeabi_f2lz)
#endif
#endif
