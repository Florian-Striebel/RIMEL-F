























#include "inner.h"


void
br_range_enc64le(void *dst, const uint64_t *v, size_t num)
{
unsigned char *buf;

buf = dst;
while (num -- > 0) {
br_enc64le(buf, *v ++);
buf += 8;
}
}
