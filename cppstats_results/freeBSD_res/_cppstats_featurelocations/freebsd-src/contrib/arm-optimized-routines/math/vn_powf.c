





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_powf, _ZGVnN4vv_powf)
#include "v_powf.c"
#endif
