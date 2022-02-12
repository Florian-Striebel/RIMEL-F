












#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
#include "fp_mul_impl.inc"

COMPILER_RT_ABI fp_t __multf3(fp_t a, fp_t b) { return __mulXf3__(a, b); }

#endif
