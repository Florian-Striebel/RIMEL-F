





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_exp2f, _ZGVnN4v_exp2f)
#include "v_exp2f.c"
#endif
