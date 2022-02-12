











#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
COMPILER_RT_ABI fp_t __addtf3(fp_t a, fp_t b);


COMPILER_RT_ABI fp_t __subtf3(fp_t a, fp_t b) {
return __addtf3(a, fromRep(toRep(b) ^ signBit));
}

#endif
