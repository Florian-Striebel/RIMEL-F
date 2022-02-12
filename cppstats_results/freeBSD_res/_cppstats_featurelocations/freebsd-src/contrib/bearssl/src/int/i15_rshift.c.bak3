























#include "inner.h"


void
br_i15_rshift(uint16_t *x, int count)
{
size_t u, len;
unsigned r;

len = (x[0] + 15) >> 4;
if (len == 0) {
return;
}
r = x[1] >> count;
for (u = 2; u <= len; u ++) {
unsigned w;

w = x[u];
x[u - 1] = ((w << (15 - count)) | r) & 0x7FFF;
r = w >> count;
}
x[len] = r;
}
