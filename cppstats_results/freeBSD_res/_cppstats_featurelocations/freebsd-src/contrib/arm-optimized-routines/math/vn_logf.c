





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_logf, _ZGVnN4v_logf)
#include "v_logf.c"
#endif
