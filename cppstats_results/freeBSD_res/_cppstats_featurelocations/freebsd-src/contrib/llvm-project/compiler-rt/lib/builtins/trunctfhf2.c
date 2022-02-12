








#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT) && defined(COMPILER_RT_HAS_FLOAT16)

#define SRC_QUAD
#define DST_HALF
#include "fp_trunc_impl.inc"

COMPILER_RT_ABI _Float16 __trunctfhf2(long double a) {
return __truncXfYf2__(a);
}

#endif
