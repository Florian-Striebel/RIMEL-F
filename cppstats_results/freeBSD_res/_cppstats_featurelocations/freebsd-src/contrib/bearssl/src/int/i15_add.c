























#include "inner.h"


uint32_t
br_i15_add(uint16_t *a, const uint16_t *b, uint32_t ctl)
{
uint32_t cc;
size_t u, m;

cc = 0;
m = (a[0] + 31) >> 4;
for (u = 1; u < m; u ++) {
uint32_t aw, bw, naw;

aw = a[u];
bw = b[u];
naw = aw + bw + cc;
cc = naw >> 15;
a[u] = MUX(ctl, naw & 0x7FFF, aw);
}
return cc;
}
