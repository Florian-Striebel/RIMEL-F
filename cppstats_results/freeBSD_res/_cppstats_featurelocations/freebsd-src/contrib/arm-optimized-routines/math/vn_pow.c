





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_pow, _ZGVnN2vv_pow)
#include "v_pow.c"
#endif
