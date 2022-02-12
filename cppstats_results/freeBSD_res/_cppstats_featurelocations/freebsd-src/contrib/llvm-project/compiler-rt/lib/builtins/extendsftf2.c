







#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
#define SRC_SINGLE
#define DST_QUAD
#include "fp_extend_impl.inc"

COMPILER_RT_ABI fp_t __extendsftf2(float a) {
return __extendXfYf2__(a);
}

#endif
