












#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)

#define NUMBER_OF_HALF_ITERATIONS 4
#define NUMBER_OF_FULL_ITERATIONS 1

#include "fp_div_impl.inc"

COMPILER_RT_ABI fp_t __divtf3(fp_t a, fp_t b) { return __divXf3__(a, b); }

#endif
