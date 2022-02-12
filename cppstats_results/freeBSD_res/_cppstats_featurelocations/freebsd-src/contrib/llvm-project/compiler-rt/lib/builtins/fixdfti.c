







#include "int_lib.h"

#if defined(CRT_HAS_128BIT)
#define DOUBLE_PRECISION
#include "fp_lib.h"

typedef ti_int fixint_t;
typedef tu_int fixuint_t;
#include "fp_fixint_impl.inc"

COMPILER_RT_ABI ti_int __fixdfti(fp_t a) { return __fixint(a); }

#endif
