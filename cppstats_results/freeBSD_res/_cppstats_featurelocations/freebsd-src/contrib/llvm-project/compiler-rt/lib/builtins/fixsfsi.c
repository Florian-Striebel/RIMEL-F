







#define SINGLE_PRECISION
#include "fp_lib.h"
typedef si_int fixint_t;
typedef su_int fixuint_t;
#include "fp_fixint_impl.inc"

COMPILER_RT_ABI si_int __fixsfsi(fp_t a) { return __fixint(a); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI si_int __aeabi_f2iz(fp_t a) { return __fixsfsi(a); }
#else
COMPILER_RT_ALIAS(__fixsfsi, __aeabi_f2iz)
#endif
#endif
