






#include <stdint.h>
#include <math.h>
#include "math_config.h"

#define TwoOverSqrtPiMinusOne 0x1.06eba8p-3f
#define A __erff_data.erff_poly_A
#define B __erff_data.erff_poly_B


static inline uint32_t
top12 (float x)
{
return asuint (x) >> 20;
}





float
erff (float x)
{
float r, x2, u;


uint32_t ix = asuint (x);
uint32_t sign = ix >> 31;
uint32_t ia12 = top12 (x) & 0x7ff;




if (ia12 < 0x3f6)
{


if (unlikely (ia12 < 0x318))
{
if (unlikely (ia12 < 0x040))
{
float y = fmaf (TwoOverSqrtPiMinusOne, x, x);
return check_uflowf (y);
}
return x + TwoOverSqrtPiMinusOne * x;
}

x2 = x * x;


r = A[5];
r = fmaf (r, x2, A[4]);
r = fmaf (r, x2, A[3]);
r = fmaf (r, x2, A[2]);
r = fmaf (r, x2, A[1]);
r = fmaf (r, x2, A[0]);
r = fmaf (r, x, x);
}
else if (ia12 < 0x408)
{

float a = fabsf (x);

r = fmaf (B[6], a, B[5]);
u = fmaf (B[4], a, B[3]);
x2 = x * x;
r = fmaf (r, x2, u);

r = fmaf (r, a, B[2]);
r = fmaf (r, a, B[1]);
r = fmaf (r, a, B[0]);
r = fmaf (r, a, a);




r = expf (-r);

if (sign)
r = -1.0f + r;
else
r = 1.0f - r;
}
else
{


if (unlikely (ia12 >= 0x7f8))
return (1.f - (float) ((ix >> 31) << 1)) + 1.f / x;


if (sign)
r = -1.0f;
else
r = 1.0f;
}
return r;
}
