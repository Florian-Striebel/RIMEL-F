







#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
typedef su_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI su_int __fixunstfsi(fp_t a) { return __fixuint(a); }
#endif
