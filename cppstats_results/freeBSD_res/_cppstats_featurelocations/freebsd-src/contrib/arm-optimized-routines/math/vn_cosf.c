





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_cosf, _ZGVnN4v_cosf)
#include "v_cosf.c"
#endif
