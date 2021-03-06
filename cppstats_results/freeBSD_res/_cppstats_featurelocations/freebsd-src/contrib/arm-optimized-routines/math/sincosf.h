






#include <stdint.h>
#include <math.h>
#include "math_config.h"


static const double pi63 = 0x1.921FB54442D18p-62;

static const double pio4 = 0x1.921FB54442D18p-1;


typedef struct
{
double sign[4];
double hpi_inv;
double hpi;
double c0, c1, c2, c3, c4;
double s1, s2, s3;
} sincos_t;


extern const sincos_t __sincosf_table[2] HIDDEN;


extern const uint32_t __inv_pio4[] HIDDEN;


static inline uint32_t
abstop12 (float x)
{
return (asuint (x) >> 20) & 0x7ff;
}




static inline void
sincosf_poly (double x, double x2, const sincos_t *p, int n, float *sinp,
float *cosp)
{
double x3, x4, x5, x6, s, c, c1, c2, s1;

x4 = x2 * x2;
x3 = x2 * x;
c2 = p->c3 + x2 * p->c4;
s1 = p->s2 + x2 * p->s3;


float *tmp = (n & 1 ? cosp : sinp);
cosp = (n & 1 ? sinp : cosp);
sinp = tmp;

c1 = p->c0 + x2 * p->c1;
x5 = x3 * x2;
x6 = x4 * x2;

s = x + x3 * p->s1;
c = c1 + x4 * p->c2;

*sinp = s + x5 * s1;
*cosp = c + x6 * c2;
}



static inline float
sinf_poly (double x, double x2, const sincos_t *p, int n)
{
double x3, x4, x6, x7, s, c, c1, c2, s1;

if ((n & 1) == 0)
{
x3 = x * x2;
s1 = p->s2 + x2 * p->s3;

x7 = x3 * x2;
s = x + x3 * p->s1;

return s + x7 * s1;
}
else
{
x4 = x2 * x2;
c2 = p->c3 + x2 * p->c4;
c1 = p->c0 + x2 * p->c1;

x6 = x4 * x2;
c = c1 + x4 * p->c2;

return c + x6 * c2;
}
}






static inline double
reduce_fast (double x, const sincos_t *p, int *np)
{
double r;
#if TOINT_INTRINSICS

r = x * p->hpi_inv;
*np = converttoint (r);
return x - roundtoint (r) * p->hpi;
#else



r = x * p->hpi_inv;
int n = ((int32_t)r + 0x800000) >> 24;
*np = n;
return x - n * p->hpi;
#endif
}








static inline double
reduce_large (uint32_t xi, int *np)
{
const uint32_t *arr = &__inv_pio4[(xi >> 26) & 15];
int shift = (xi >> 23) & 7;
uint64_t n, res0, res1, res2;

xi = (xi & 0xffffff) | 0x800000;
xi <<= shift;

res0 = xi * arr[0];
res1 = (uint64_t)xi * arr[4];
res2 = (uint64_t)xi * arr[8];
res0 = (res2 >> 32) | (res0 << 32);
res0 += res1;

n = (res0 + (1ULL << 61)) >> 62;
res0 -= n << 62;
double x = (int64_t)res0;
*np = n;
return x * pi63;
}
