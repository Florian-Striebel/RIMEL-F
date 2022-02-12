























#include "inner.h"


void
br_range_dec64le(uint64_t *v, size_t num, const void *src)
{
const unsigned char *buf;

buf = src;
while (num -- > 0) {
*v ++ = br_dec64le(buf);
buf += 8;
}
}
