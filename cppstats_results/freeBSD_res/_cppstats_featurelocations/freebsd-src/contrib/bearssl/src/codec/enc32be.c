























#include "inner.h"


void
br_range_enc32be(void *dst, const uint32_t *v, size_t num)
{
unsigned char *buf;

buf = dst;
while (num -- > 0) {
br_enc32be(buf, *v ++);
buf += 4;
}
}
