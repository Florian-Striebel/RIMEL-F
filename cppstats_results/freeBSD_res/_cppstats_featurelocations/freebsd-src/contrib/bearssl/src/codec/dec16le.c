























#include "inner.h"


void
br_range_dec16le(uint16_t *v, size_t num, const void *src)
{
const unsigned char *buf;

buf = src;
while (num -- > 0) {
*v ++ = br_dec16le(buf);
buf += 2;
}
}
