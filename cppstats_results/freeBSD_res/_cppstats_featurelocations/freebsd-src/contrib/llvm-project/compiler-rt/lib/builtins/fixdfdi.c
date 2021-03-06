







#define DOUBLE_PRECISION
#include "fp_lib.h"

#if !defined(__SOFTFP__)



COMPILER_RT_ABI du_int __fixunsdfdi(double a);

COMPILER_RT_ABI di_int __fixdfdi(double a) {
if (a < 0.0) {
return -__fixunsdfdi(-a);
}
return __fixunsdfdi(a);
}

#else




typedef di_int fixint_t;
typedef du_int fixuint_t;
#include "fp_fixint_impl.inc"

COMPILER_RT_ABI di_int __fixdfdi(fp_t a) { return __fixint(a); }

#endif

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI di_int __aeabi_d2lz(fp_t a) { return __fixdfdi(a); }
#else
COMPILER_RT_ALIAS(__fixdfdi, __aeabi_d2lz)
#endif
#endif
