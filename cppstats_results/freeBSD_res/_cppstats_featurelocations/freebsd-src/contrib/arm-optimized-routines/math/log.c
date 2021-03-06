






#include <float.h>
#include <math.h>
#include <stdint.h>
#include "math_config.h"

#define T __log_data.tab
#define T2 __log_data.tab2
#define B __log_data.poly1
#define A __log_data.poly
#define Ln2hi __log_data.ln2hi
#define Ln2lo __log_data.ln2lo
#define N (1 << LOG_TABLE_BITS)
#define OFF 0x3fe6000000000000


static inline uint32_t
top16 (double x)
{
return asuint64 (x) >> 48;
}

double
log (double x)
{

double_t w, z, r, r2, r3, y, invc, logc, kd, hi, lo;
uint64_t ix, iz, tmp;
uint32_t top;
int k, i;

ix = asuint64 (x);
top = top16 (x);

#if LOG_POLY1_ORDER == 10 || LOG_POLY1_ORDER == 11
#define LO asuint64 (1.0 - 0x1p-5)
#define HI asuint64 (1.0 + 0x1.1p-5)
#elif LOG_POLY1_ORDER == 12
#define LO asuint64 (1.0 - 0x1p-4)
#define HI asuint64 (1.0 + 0x1.09p-4)
#endif
if (unlikely (ix - LO < HI - LO))
{


if (WANT_ROUNDING && unlikely (ix == asuint64 (1.0)))
return 0;
r = x - 1.0;
r2 = r * r;
r3 = r * r2;
#if LOG_POLY1_ORDER == 10

y = r3 * (B[1] + r * B[2] + r2 * B[3]
+ r3 * (B[4] + r * B[5] + r2 * B[6] + r3 * (B[7] + r * B[8])));
w = B[0] * r2;
hi = r + w;
y += r - hi + w;
y += hi;
#elif LOG_POLY1_ORDER == 11

y = r3 * (B[1] + r * B[2]
+ r2 * (B[3] + r * B[4] + r2 * B[5]
+ r3 * (B[6] + r * B[7] + r2 * B[8] + r3 * B[9])));
w = B[0] * r2;
hi = r + w;
y += r - hi + w;
y += hi;
#elif LOG_POLY1_ORDER == 12
y = r3 * (B[1] + r * B[2] + r2 * B[3]
+ r3 * (B[4] + r * B[5] + r2 * B[6]
+ r3 * (B[7] + r * B[8] + r2 * B[9] + r3 * B[10])));
#if N <= 64

w = B[0] * r2;
hi = r + w;
y += r - hi + w;
y += hi;
#else

w = r * 0x1p27;
double_t rhi = r + w - w;
double_t rlo = r - rhi;
w = rhi * rhi * B[0];
hi = r + w;
lo = r - hi + w;
lo += B[0] * rlo * (rhi + r);
y += lo;
y += hi;
#endif
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
i = (tmp >> (52 - LOG_TABLE_BITS)) % N;
k = (int64_t) tmp >> 52;
iz = ix - (tmp & 0xfffULL << 52);
invc = T[i].invc;
logc = T[i].logc;
z = asdouble (iz);



#if HAVE_FAST_FMA

r = fma (z, invc, -1.0);
#else

r = (z - T2[i].chi - T2[i].clo) * invc;
#endif
kd = (double_t) k;


w = kd * Ln2hi + logc;
hi = w + r;
lo = w - hi + r + kd * Ln2lo;


r2 = r * r;




#if LOG_POLY_ORDER == 6
y = lo + r2 * A[0] + r * r2 * (A[1] + r * A[2] + r2 * (A[3] + r * A[4])) + hi;
#elif LOG_POLY_ORDER == 7
y = lo
+ r2 * (A[0] + r * A[1] + r2 * (A[2] + r * A[3])
+ r2 * r2 * (A[4] + r * A[5]))
+ hi;
#endif
return eval_as_double (y);
}
#if USE_GLIBC_ABI
strong_alias (log, __log_finite)
hidden_alias (log, __ieee754_log)
#if LDBL_MANT_DIG == 53
long double logl (long double x) { return log (x); }
#endif
#endif
