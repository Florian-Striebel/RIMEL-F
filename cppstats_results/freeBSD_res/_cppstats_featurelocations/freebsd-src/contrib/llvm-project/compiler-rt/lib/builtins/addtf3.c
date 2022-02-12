











#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
#include "fp_add_impl.inc"

COMPILER_RT_ABI fp_t __addtf3(fp_t a, fp_t b) {
return __addXf3__(a, b);
}

#endif
