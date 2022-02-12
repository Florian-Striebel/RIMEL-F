





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_exp, _ZGVnN2v_exp)
#include "v_exp.c"
#endif
