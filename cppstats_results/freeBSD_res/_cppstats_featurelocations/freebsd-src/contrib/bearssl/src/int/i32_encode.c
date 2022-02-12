























#include "inner.h"


void
br_i32_encode(void *dst, size_t len, const uint32_t *x)
{
unsigned char *buf;
size_t k;

buf = dst;





k = (x[0] + 7) >> 3;
while (len > k) {
*buf ++ = 0;
len --;
}






k = (len + 3) >> 2;
switch (len & 3) {
case 3:
*buf ++ = x[k] >> 16;

case 2:
*buf ++ = x[k] >> 8;

case 1:
*buf ++ = x[k];
k --;
}




while (k > 0) {
br_enc32be(buf, x[k]);
k --;
buf += 4;
}
}
