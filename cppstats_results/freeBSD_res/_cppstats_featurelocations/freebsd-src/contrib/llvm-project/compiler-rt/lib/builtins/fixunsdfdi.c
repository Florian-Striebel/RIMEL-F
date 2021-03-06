







#define DOUBLE_PRECISION
#include "fp_lib.h"

#if !defined(__SOFTFP__)



COMPILER_RT_ABI du_int __fixunsdfdi(double a) {
if (a <= 0.0)
return 0;
su_int high = a / 4294967296.f;
su_int low = a - (double)high * 4294967296.f;
return ((du_int)high << 32) | low;
}

#else




typedef du_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI du_int __fixunsdfdi(fp_t a) { return __fixuint(a); }

#endif

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI du_int __aeabi_d2ulz(fp_t a) { return __fixunsdfdi(a); }
#else
COMPILER_RT_ALIAS(__fixunsdfdi, __aeabi_d2ulz)
#endif
#endif
