







#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
typedef di_int fixint_t;
typedef du_int fixuint_t;
#include "fp_fixint_impl.inc"

COMPILER_RT_ABI di_int __fixtfdi(fp_t a) { return __fixint(a); }
#endif
