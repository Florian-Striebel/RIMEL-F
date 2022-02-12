





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_sinf, _ZGVnN4v_sinf)
#include "v_sinf.c"
#endif
