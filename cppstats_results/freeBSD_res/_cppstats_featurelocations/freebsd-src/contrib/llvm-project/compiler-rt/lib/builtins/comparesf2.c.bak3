





































#define SINGLE_PRECISION
#include "fp_lib.h"

#include "fp_compare_impl.inc"

COMPILER_RT_ABI CMP_RESULT __lesf2(fp_t a, fp_t b) { return __leXf2__(a, b); }

#if defined(__ELF__)

COMPILER_RT_ALIAS(__lesf2, __cmpsf2)
#endif
COMPILER_RT_ALIAS(__lesf2, __eqsf2)
COMPILER_RT_ALIAS(__lesf2, __ltsf2)
COMPILER_RT_ALIAS(__lesf2, __nesf2)

COMPILER_RT_ABI CMP_RESULT __gesf2(fp_t a, fp_t b) { return __geXf2__(a, b); }

COMPILER_RT_ALIAS(__gesf2, __gtsf2)

COMPILER_RT_ABI CMP_RESULT __unordsf2(fp_t a, fp_t b) {
return __unordXf2__(a, b);
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI int __aeabi_fcmpun(fp_t a, fp_t b) { return __unordsf2(a, b); }
#else
COMPILER_RT_ALIAS(__unordsf2, __aeabi_fcmpun)
#endif
#endif

#if defined(_WIN32) && !defined(__MINGW32__)


int __eqsf2(fp_t a, fp_t b) { return __lesf2(a, b); }
int __ltsf2(fp_t a, fp_t b) { return __lesf2(a, b); }
int __nesf2(fp_t a, fp_t b) { return __lesf2(a, b); }
int __gtsf2(fp_t a, fp_t b) { return __gesf2(a, b); }
#endif
