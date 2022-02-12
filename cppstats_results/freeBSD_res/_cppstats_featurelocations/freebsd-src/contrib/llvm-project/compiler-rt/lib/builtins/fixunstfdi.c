







#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
typedef du_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI du_int __fixunstfdi(fp_t a) { return __fixuint(a); }
#endif
