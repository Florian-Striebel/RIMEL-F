





#include "mathlib.h"
#if defined(__vpcs)
#define VPCS 1
#define VPCS_ALIAS strong_alias (__vn_log, _ZGVnN2v_log)
#include "v_log.c"
#endif
