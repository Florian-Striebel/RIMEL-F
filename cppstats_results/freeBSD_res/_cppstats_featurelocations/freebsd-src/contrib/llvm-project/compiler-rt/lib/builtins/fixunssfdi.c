







#define SINGLE_PRECISION
#include "fp_lib.h"

#if !defined(__SOFTFP__)



COMPILER_RT_ABI du_int __fixunssfdi(float a) {
if (a <= 0.0f)
return 0;
double da = a;
su_int high = da / 4294967296.f;
su_int low = da - (double)high * 4294967296.f;
return ((du_int)high << 32) | low;
}

#else




typedef du_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI du_int __fixunssfdi(fp_t a) { return __fixuint(a); }

#endif

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI du_int __aeabi_f2ulz(fp_t a) { return __fixunssfdi(a); }
#else
COMPILER_RT_ALIAS(__fixunssfdi, __aeabi_f2ulz)
#endif
#endif
