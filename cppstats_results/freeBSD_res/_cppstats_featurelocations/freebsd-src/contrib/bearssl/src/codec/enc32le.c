























#include "inner.h"


void
br_range_enc32le(void *dst, const uint32_t *v, size_t num)
{
unsigned char *buf;

buf = dst;
while (num -- > 0) {
br_enc32le(buf, *v ++);
buf += 4;
}
}
