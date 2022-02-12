























#include "inner.h"


void
br_i31_from_monty(uint32_t *x, const uint32_t *m, uint32_t m0i)
{
size_t len, u, v;

len = (m[0] + 31) >> 5;
for (u = 0; u < len; u ++) {
uint32_t f;
uint64_t cc;

f = MUL31_lo(x[1], m0i);
cc = 0;
for (v = 0; v < len; v ++) {
uint64_t z;

z = (uint64_t)x[v + 1] + MUL31(f, m[v + 1]) + cc;
cc = z >> 31;
if (v != 0) {
x[v] = (uint32_t)z & 0x7FFFFFFF;
}
}
x[len] = (uint32_t)cc;
}








br_i31_sub(x, m, NOT(br_i31_sub(x, m, 0)));
}
