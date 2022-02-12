





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_cos, _ZGVnN2v_cos)
#include "v_cos.c"
#endif
