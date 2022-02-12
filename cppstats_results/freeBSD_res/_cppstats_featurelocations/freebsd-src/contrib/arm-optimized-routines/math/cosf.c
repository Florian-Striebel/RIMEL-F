






#include <stdint.h>
#include <math.h>
#include "math_config.h"
#include "sincosf.h"





float
cosf (float y)
{
double x = y;
double s;
int n;
const sincos_t *p = &__sincosf_table[0];

if (abstop12 (y) < abstop12 (pio4))
{
double x2 = x * x;

if (unlikely (abstop12 (y) < abstop12 (0x1p-12f)))
return 1.0f;

return sinf_poly (x, x2, p, 1);
}
else if (likely (abstop12 (y) < abstop12 (120.0f)))
{
x = reduce_fast (x, p, &n);


s = p->sign[n & 3];

if (n & 2)
p = &__sincosf_table[1];

return sinf_poly (x * s, x * x, p, n ^ 1);
}
else if (abstop12 (y) < abstop12 (INFINITY))
{
uint32_t xi = asuint (y);
int sign = xi >> 31;

x = reduce_large (xi, &n);


s = p->sign[(n + sign) & 3];

if ((n + sign) & 2)
p = &__sincosf_table[1];

return sinf_poly (x * s, x * x, p, n ^ 1);
}
else
return __math_invalidf (y);
}
