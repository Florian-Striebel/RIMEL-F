






#include "v_math.h"
#if WANT_VMATH

#define V_LOG_TABLE_BITS 7

extern const struct v_log_data
{
f64_t invc;
f64_t logc;
} __v_log_data[1 << V_LOG_TABLE_BITS] HIDDEN;
#endif
