























#include "inner.h"


void
br_i15_to_monty(uint16_t *x, const uint16_t *m)
{
unsigned k;

for (k = (m[0] + 15) >> 4; k > 0; k --) {
br_i15_muladd_small(x, 0, m);
}
}
