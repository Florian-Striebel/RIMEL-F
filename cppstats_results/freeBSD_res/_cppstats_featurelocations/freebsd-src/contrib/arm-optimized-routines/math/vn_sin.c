





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_sin, _ZGVnN2v_sin)
#include "v_sin.c"
#endif
