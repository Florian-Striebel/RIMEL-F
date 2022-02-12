























#include "inner.h"


void
br_i15_mulacc(uint16_t *d, const uint16_t *a, const uint16_t *b)
{
size_t alen, blen, u;
unsigned dl, dh;

alen = (a[0] + 15) >> 4;
blen = (b[0] + 15) >> 4;





dl = (a[0] & 15) + (b[0] & 15);
dh = (a[0] >> 4) + (b[0] >> 4);
d[0] = (dh << 4) + dl + (~(uint32_t)(dl - 15) >> 31);

for (u = 0; u < blen; u ++) {
uint32_t f;
size_t v;
uint32_t cc;

f = b[1 + u];
cc = 0;
for (v = 0; v < alen; v ++) {
uint32_t z;

z = (uint32_t)d[1 + u + v] + MUL15(f, a[1 + v]) + cc;
cc = z >> 15;
d[1 + u + v] = z & 0x7FFF;
}
d[1 + u + alen] = cc;
}
}
