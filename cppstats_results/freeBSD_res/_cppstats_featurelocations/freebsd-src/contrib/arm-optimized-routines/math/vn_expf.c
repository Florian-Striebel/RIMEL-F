





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_expf, _ZGVnN4v_expf)
#include "v_expf.c"
#endif
