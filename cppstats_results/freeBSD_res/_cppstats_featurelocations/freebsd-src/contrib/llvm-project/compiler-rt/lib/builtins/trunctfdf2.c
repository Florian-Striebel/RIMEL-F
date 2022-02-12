







#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
#define SRC_QUAD
#define DST_DOUBLE
#include "fp_trunc_impl.inc"

COMPILER_RT_ABI double __trunctfdf2(long double a) { return __truncXfYf2__(a); }

#endif
