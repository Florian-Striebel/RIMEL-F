























#include "inner.h"


void
br_range_dec32be(uint32_t *v, size_t num, const void *src)
{
const unsigned char *buf;

buf = src;
while (num -- > 0) {
*v ++ = br_dec32be(buf);
buf += 4;
}
}
