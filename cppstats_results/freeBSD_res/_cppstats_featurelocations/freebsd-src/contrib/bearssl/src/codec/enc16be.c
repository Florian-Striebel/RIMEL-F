























#include "inner.h"


void
br_range_enc16be(void *dst, const uint16_t *v, size_t num)
{
unsigned char *buf;

buf = dst;
while (num -- > 0) {
br_enc16be(buf, *v ++);
buf += 2;
}
}
