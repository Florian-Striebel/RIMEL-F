






#include <stdint.h>
#include <math.h>
#include "math_config.h"
#include "sincosf.h"





void
sincosf (float y, float *sinp, float *cosp)
{
double x = y;
double s;
int n;
const sincos_t *p = &__sincosf_table[0];

if (abstop12 (y) < abstop12 (pio4))
{
double x2 = x * x;

if (unlikely (abstop12 (y) < abstop12 (0x1p-12f)))
{
if (unlikely (abstop12 (y) < abstop12 (0x1p-126f)))

force_eval_float (x2);
*sinp = y;
*cosp = 1.0f;
return;
}

sincosf_poly (x, x2, p, 0, sinp, cosp);
}
else if (abstop12 (y) < abstop12 (120.0f))
{
x = reduce_fast (x, p, &n);


s = p->sign[n & 3];

if (n & 2)
p = &__sincosf_table[1];

sincosf_poly (x * s, x * x, p, n, sinp, cosp);
}
else if (likely (abstop12 (y) < abstop12 (INFINITY)))
{
uint32_t xi = asuint (y);
int sign = xi >> 31;

x = reduce_large (xi, &n);


s = p->sign[(n + sign) & 3];

if ((n + sign) & 2)
p = &__sincosf_table[1];

sincosf_poly (x * s, x * x, p, n, sinp, cosp);
}
else
{

*sinp = *cosp = y - y;
#if WANT_ERRNO



__math_invalidf (y + y);
#endif
}
}
