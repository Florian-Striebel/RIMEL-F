





































#define DOUBLE_PRECISION
#include "fp_lib.h"

#include "fp_compare_impl.inc"

COMPILER_RT_ABI CMP_RESULT __ledf2(fp_t a, fp_t b) { return __leXf2__(a, b); }

#if defined(__ELF__)

COMPILER_RT_ALIAS(__ledf2, __cmpdf2)
#endif
COMPILER_RT_ALIAS(__ledf2, __eqdf2)
COMPILER_RT_ALIAS(__ledf2, __ltdf2)
COMPILER_RT_ALIAS(__ledf2, __nedf2)

COMPILER_RT_ABI CMP_RESULT __gedf2(fp_t a, fp_t b) { return __geXf2__(a, b); }

COMPILER_RT_ALIAS(__gedf2, __gtdf2)

COMPILER_RT_ABI CMP_RESULT __unorddf2(fp_t a, fp_t b) {
return __unordXf2__(a, b);
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI int __aeabi_dcmpun(fp_t a, fp_t b) { return __unorddf2(a, b); }
#else
COMPILER_RT_ALIAS(__unorddf2, __aeabi_dcmpun)
#endif
#endif

#if defined(_WIN32) && !defined(__MINGW32__)


int __eqdf2(fp_t a, fp_t b) { return __ledf2(a, b); }
int __ltdf2(fp_t a, fp_t b) { return __ledf2(a, b); }
int __nedf2(fp_t a, fp_t b) { return __ledf2(a, b); }
int __gtdf2(fp_t a, fp_t b) { return __gedf2(a, b); }
#endif
