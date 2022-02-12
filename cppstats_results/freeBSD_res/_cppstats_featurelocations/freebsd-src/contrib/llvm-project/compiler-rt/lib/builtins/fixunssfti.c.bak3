











#define SINGLE_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT)
typedef tu_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI tu_int __fixunssfti(fp_t a) { return __fixuint(a); }
#endif
