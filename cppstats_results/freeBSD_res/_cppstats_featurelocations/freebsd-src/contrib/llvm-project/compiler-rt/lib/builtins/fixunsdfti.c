







#include "int_lib.h"

#if defined(CRT_HAS_128BIT)
#define DOUBLE_PRECISION
#include "fp_lib.h"
typedef tu_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI tu_int __fixunsdfti(fp_t a) { return __fixuint(a); }
#endif
