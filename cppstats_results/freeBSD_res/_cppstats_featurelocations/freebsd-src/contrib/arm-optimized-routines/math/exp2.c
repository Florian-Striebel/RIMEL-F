






#include <float.h>
#include <math.h>
#include <stdint.h>
#include "math_config.h"

#define N (1 << EXP_TABLE_BITS)
#define Shift __exp_data.exp2_shift
#define T __exp_data.tab
#define C1 __exp_data.exp2_poly[0]
#define C2 __exp_data.exp2_poly[1]
#define C3 __exp_data.exp2_poly[2]
#define C4 __exp_data.exp2_poly[3]
#define C5 __exp_data.exp2_poly[4]
#define C6 __exp_data.exp2_poly[5]








static inline double
specialcase (double_t tmp, uint64_t sbits, uint64_t ki)
{
double_t scale, y;

if ((ki & 0x80000000) == 0)
{

sbits -= 1ull << 52;
scale = asdouble (sbits);
y = 2 * (scale + scale * tmp);
return check_oflow (eval_as_double (y));
}

sbits += 1022ull << 52;
scale = asdouble (sbits);
y = scale + scale * tmp;
if (y < 1.0)
{




double_t hi, lo;
lo = scale - y + scale * tmp;
hi = 1.0 + y;
lo = 1.0 - hi + y + lo;
y = eval_as_double (hi + lo) - 1.0;

if (WANT_ROUNDING && y == 0.0)
y = 0.0;

force_eval_double (opt_barrier_double (0x1p-1022) * 0x1p-1022);
}
y = 0x1p-1022 * y;
return check_uflow (eval_as_double (y));
}


static inline uint32_t
top12 (double x)
{
return asuint64 (x) >> 52;
}

double
exp2 (double x)
{
uint32_t abstop;
uint64_t ki, idx, top, sbits;

double_t kd, r, r2, scale, tail, tmp;

abstop = top12 (x) & 0x7ff;
if (unlikely (abstop - top12 (0x1p-54) >= top12 (512.0) - top12 (0x1p-54)))
{
if (abstop - top12 (0x1p-54) >= 0x80000000)


return WANT_ROUNDING ? 1.0 + x : 1.0;
if (abstop >= top12 (1024.0))
{
if (asuint64 (x) == asuint64 (-INFINITY))
return 0.0;
if (abstop >= top12 (INFINITY))
return 1.0 + x;
if (!(asuint64 (x) >> 63))
return __math_oflow (0);
else if (asuint64 (x) >= asuint64 (-1075.0))
return __math_uflow (0);
}
if (2 * asuint64 (x) > 2 * asuint64 (928.0))

abstop = 0;
}



kd = eval_as_double (x + Shift);
ki = asuint64 (kd);
kd -= Shift;
r = x - kd;

idx = 2 * (ki % N);
top = ki << (52 - EXP_TABLE_BITS);
tail = asdouble (T[idx]);

sbits = T[idx + 1] + top;


r2 = r * r;


#if EXP2_POLY_ORDER == 4
tmp = tail + r * C1 + r2 * C2 + r * r2 * (C3 + r * C4);
#elif EXP2_POLY_ORDER == 5
tmp = tail + r * C1 + r2 * (C2 + r * C3) + r2 * r2 * (C4 + r * C5);
#elif EXP2_POLY_ORDER == 6
tmp = tail + r * C1 + r2 * (0.5 + r * C3) + r2 * r2 * (C4 + r * C5 + r2 * C6);
#endif
if (unlikely (abstop == 0))
return specialcase (tmp, sbits, ki);
scale = asdouble (sbits);


return eval_as_double (scale + scale * tmp);
}
#if USE_GLIBC_ABI
strong_alias (exp2, __exp2_finite)
hidden_alias (exp2, __ieee754_exp2)
#if LDBL_MANT_DIG == 53
long double exp2l (long double x) { return exp2 (x); }
#endif
#endif
