






#include <float.h>
#include <math.h>
#include <stdint.h>
#include "math_config.h"

#define T __log2_data.tab
#define T2 __log2_data.tab2
#define B __log2_data.poly1
#define A __log2_data.poly
#define InvLn2hi __log2_data.invln2hi
#define InvLn2lo __log2_data.invln2lo
#define N (1 << LOG2_TABLE_BITS)
#define OFF 0x3fe6000000000000


static inline uint32_t
top16 (double x)
{
return asuint64 (x) >> 48;
}

double
log2 (double x)
{

double_t z, r, r2, r4, y, invc, logc, kd, hi, lo, t1, t2, t3, p;
uint64_t ix, iz, tmp;
uint32_t top;
int k, i;

ix = asuint64 (x);
top = top16 (x);

#if LOG2_POLY1_ORDER == 11
#define LO asuint64 (1.0 - 0x1.5b51p-5)
#define HI asuint64 (1.0 + 0x1.6ab2p-5)
#endif
if (unlikely (ix - LO < HI - LO))
{


if (WANT_ROUNDING && unlikely (ix == asuint64 (1.0)))
return 0;
r = x - 1.0;
#if HAVE_FAST_FMA
hi = r * InvLn2hi;
lo = r * InvLn2lo + fma (r, InvLn2hi, -hi);
#else
double_t rhi, rlo;
rhi = asdouble (asuint64 (r) & -1ULL << 32);
rlo = r - rhi;
hi = rhi * InvLn2hi;
lo = rlo * InvLn2hi + r * InvLn2lo;
#endif
r2 = r * r;
r4 = r2 * r2;
#if LOG2_POLY1_ORDER == 11

p = r2 * (B[0] + r * B[1]);
y = hi + p;
lo += hi - y + p;
lo += r4 * (B[2] + r * B[3] + r2 * (B[4] + r * B[5])
+ r4 * (B[6] + r * B[7] + r2 * (B[8] + r * B[9])));
y += lo;
#endif
return eval_as_double (y);
}
if (unlikely (top - 0x0010 >= 0x7ff0 - 0x0010))
{

if (ix * 2 == 0)
return __math_divzero (1);
if (ix == asuint64 (INFINITY))
return x;
if ((top & 0x8000) || (top & 0x7ff0) == 0x7ff0)
return __math_invalid (x);

ix = asuint64 (x * 0x1p52);
ix -= 52ULL << 52;
}




tmp = ix - OFF;
i = (tmp >> (52 - LOG2_TABLE_BITS)) % N;
k = (int64_t) tmp >> 52;
iz = ix - (tmp & 0xfffULL << 52);
invc = T[i].invc;
logc = T[i].logc;
z = asdouble (iz);
kd = (double_t) k;



#if HAVE_FAST_FMA

r = fma (z, invc, -1.0);
t1 = r * InvLn2hi;
t2 = r * InvLn2lo + fma (r, InvLn2hi, -t1);
#else
double_t rhi, rlo;

r = (z - T2[i].chi - T2[i].clo) * invc;
rhi = asdouble (asuint64 (r) & -1ULL << 32);
rlo = r - rhi;
t1 = rhi * InvLn2hi;
t2 = rlo * InvLn2hi + r * InvLn2lo;
#endif


t3 = kd + logc;
hi = t3 + t1;
lo = t3 - hi + t1 + t2;



r2 = r * r;
r4 = r2 * r2;
#if LOG2_POLY_ORDER == 7


p = A[0] + r * A[1] + r2 * (A[2] + r * A[3]) + r4 * (A[4] + r * A[5]);
y = lo + r2 * p + hi;
#endif
return eval_as_double (y);
}
#if USE_GLIBC_ABI
strong_alias (log2, __log2_finite)
hidden_alias (log2, __ieee754_log2)
#if LDBL_MANT_DIG == 53
long double log2l (long double x) { return log2 (x); }
#endif
#endif
