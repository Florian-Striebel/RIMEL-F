























#include "inner.h"


void
br_range_dec32le(uint32_t *v, size_t num, const void *src)
{
const unsigned char *buf;

buf = src;
while (num -- > 0) {
*v ++ = br_dec32le(buf);
buf += 4;
}
}
