






#include <float.h>
#include <math.h>
#include <stdint.h>
#include "math_config.h"

#define N (1 << EXP_TABLE_BITS)
#define InvLn2N __exp_data.invln2N
#define NegLn2hiN __exp_data.negln2hiN
#define NegLn2loN __exp_data.negln2loN
#define Shift __exp_data.shift
#define T __exp_data.tab
#define C2 __exp_data.poly[5 - EXP_POLY_ORDER]
#define C3 __exp_data.poly[6 - EXP_POLY_ORDER]
#define C4 __exp_data.poly[7 - EXP_POLY_ORDER]
#define C5 __exp_data.poly[8 - EXP_POLY_ORDER]
#define C6 __exp_data.poly[9 - EXP_POLY_ORDER]








static inline double
specialcase (double_t tmp, uint64_t sbits, uint64_t ki)
{
double_t scale, y;

if ((ki & 0x80000000) == 0)
{

sbits -= 1009ull << 52;
scale = asdouble (sbits);
y = 0x1p1009 * (scale + scale * tmp);
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



static inline double
exp_inline (double x, double xtail, int hastail)
{
uint32_t abstop;
uint64_t ki, idx, top, sbits;

double_t kd, z, r, r2, scale, tail, tmp;

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
if (asuint64 (x) >> 63)
return __math_uflow (0);
else
return __math_oflow (0);
}

abstop = 0;
}



z = InvLn2N * x;
#if TOINT_INTRINSICS
kd = roundtoint (z);
ki = converttoint (z);
#elif EXP_USE_TOINT_NARROW

kd = eval_as_double (z + Shift);
ki = asuint64 (kd) >> 16;
kd = (double_t) (int32_t) ki;
#else

kd = eval_as_double (z + Shift);
ki = asuint64 (kd);
kd -= Shift;
#endif
r = x + kd * NegLn2hiN + kd * NegLn2loN;

if (hastail)
r += xtail;

idx = 2 * (ki % N);
top = ki << (52 - EXP_TABLE_BITS);
tail = asdouble (T[idx]);

sbits = T[idx + 1] + top;


r2 = r * r;


#if EXP_POLY_ORDER == 4
tmp = tail + r + r2 * C2 + r * r2 * (C3 + r * C4);
#elif EXP_POLY_ORDER == 5
tmp = tail + r + r2 * (C2 + r * C3) + r2 * r2 * (C4 + r * C5);
#elif EXP_POLY_ORDER == 6
tmp = tail + r + r2 * (0.5 + r * C3) + r2 * r2 * (C4 + r * C5 + r2 * C6);
#endif
if (unlikely (abstop == 0))
return specialcase (tmp, sbits, ki);
scale = asdouble (sbits);


return eval_as_double (scale + scale * tmp);
}

double
exp (double x)
{
return exp_inline (x, 0, 0);
}



double
__exp_dd (double x, double xtail)
{
return exp_inline (x, xtail, 1);
}
#if USE_GLIBC_ABI
strong_alias (exp, __exp_finite)
hidden_alias (exp, __ieee754_exp)
hidden_alias (__exp_dd, __exp1)
#if LDBL_MANT_DIG == 53
long double expl (long double x) { return exp (x); }
#endif
#endif
