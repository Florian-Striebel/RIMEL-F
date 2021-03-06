







#define DOUBLE_PRECISION
#include "fp_lib.h"
typedef si_int fixint_t;
typedef su_int fixuint_t;
#include "fp_fixint_impl.inc"

COMPILER_RT_ABI si_int __fixdfsi(fp_t a) { return __fixint(a); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI si_int __aeabi_d2iz(fp_t a) { return __fixdfsi(a); }
#else
COMPILER_RT_ALIAS(__fixdfsi, __aeabi_d2iz)
#endif
#endif
