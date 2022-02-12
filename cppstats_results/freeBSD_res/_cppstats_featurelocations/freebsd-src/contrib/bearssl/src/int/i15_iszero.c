























#include "inner.h"


uint32_t
br_i15_iszero(const uint16_t *x)
{
uint32_t z;
size_t u;

z = 0;
for (u = (x[0] + 15) >> 4; u > 0; u --) {
z |= x[u];
}
return ~(z | -z) >> 31;
}
