





































#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
#include "fp_compare_impl.inc"

COMPILER_RT_ABI CMP_RESULT __letf2(fp_t a, fp_t b) { return __leXf2__(a, b); }

#if defined(__ELF__)

COMPILER_RT_ALIAS(__letf2, __cmptf2)
#endif
COMPILER_RT_ALIAS(__letf2, __eqtf2)
COMPILER_RT_ALIAS(__letf2, __lttf2)
COMPILER_RT_ALIAS(__letf2, __netf2)

COMPILER_RT_ABI CMP_RESULT __getf2(fp_t a, fp_t b) { return __geXf2__(a, b); }

COMPILER_RT_ALIAS(__getf2, __gttf2)

COMPILER_RT_ABI CMP_RESULT __unordtf2(fp_t a, fp_t b) {
return __unordXf2__(a, b);
}

#endif
