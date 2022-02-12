






#include "v_math.h"
#if WANT_VMATH

#define V_EXP_TABLE_BITS 7

extern const u64_t __v_exp_data[1 << V_EXP_TABLE_BITS] HIDDEN;
#endif
