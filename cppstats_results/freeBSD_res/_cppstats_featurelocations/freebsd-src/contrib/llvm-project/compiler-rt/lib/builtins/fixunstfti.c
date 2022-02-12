







#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
typedef tu_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI tu_int __fixunstfti(fp_t a) { return __fixuint(a); }
#endif
