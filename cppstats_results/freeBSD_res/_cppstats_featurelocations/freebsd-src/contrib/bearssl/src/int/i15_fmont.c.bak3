























#include "inner.h"


void
br_i15_from_monty(uint16_t *x, const uint16_t *m, uint16_t m0i)
{
size_t len, u, v;

len = (m[0] + 15) >> 4;
for (u = 0; u < len; u ++) {
uint32_t f, cc;

f = MUL15(x[1], m0i) & 0x7FFF;
cc = 0;
for (v = 0; v < len; v ++) {
uint32_t z;

z = (uint32_t)x[v + 1] + MUL15(f, m[v + 1]) + cc;
cc = z >> 15;
if (v != 0) {
x[v] = z & 0x7FFF;
}
}
x[len] = cc;
}








br_i15_sub(x, m, NOT(br_i15_sub(x, m, 0)));
}
