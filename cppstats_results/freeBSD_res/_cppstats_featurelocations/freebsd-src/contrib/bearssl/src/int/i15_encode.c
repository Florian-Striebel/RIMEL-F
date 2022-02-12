























#include "inner.h"


void
br_i15_encode(void *dst, size_t len, const uint16_t *x)
{
unsigned char *buf;
size_t u, xlen;
uint32_t acc;
int acc_len;

xlen = (x[0] + 15) >> 4;
if (xlen == 0) {
memset(dst, 0, len);
return;
}
u = 1;
acc = 0;
acc_len = 0;
buf = dst;
while (len -- > 0) {
if (acc_len < 8) {
if (u <= xlen) {
acc += (uint32_t)x[u ++] << acc_len;
}
acc_len += 15;
}
buf[len] = (unsigned char)acc;
acc >>= 8;
acc_len -= 8;
}
}
