






#include "types.h"
#include "random.h"

static uint32 seedbuf[55];
static int seedptr;

void seed_random(uint32 seed) {
int i;

seedptr = 0;
for (i = 0; i < 55; i++) {
seed = seed % 44488 * 48271 - seed / 44488 * 3399;
seedbuf[i] = seed - 1;
}
}

uint32 base_random(void) {
seedptr %= 55;
seedbuf[seedptr] += seedbuf[(seedptr+31)%55];
return seedbuf[seedptr++];
}

uint32 random32(void) {
uint32 a, b, b1, b2;
a = base_random();
b = base_random();
for (b1 = 0x80000000, b2 = 1; b1 > b2; b1 >>= 1, b2 <<= 1) {
uint32 b3 = b1 | b2;
if ((b & b3) != 0 && (b & b3) != b3)
b ^= b3;
}
return a ^ b;
}



















static uint32 random_upto_makemask(uint32 limit) {
uint32 mask = 0xFFFFFFFF;
int i;
for (i = 16; i > 0; i >>= 1)
if ((limit & (mask >> i)) == limit)
mask >>= i;
return mask;
}

static uint32 random_upto_internal(uint32 limit, uint32 mask) {
uint32 ret;
do {
ret = random32() & mask;
} while (ret > limit);
return ret;
}

uint32 random_upto(uint32 limit) {
uint32 mask = random_upto_makemask(limit);
return random_upto_internal(limit, mask);
}

uint32 random_upto_biased(uint32 limit, int bias) {
uint32 mask = random_upto_makemask(limit);

uint32 ret = random_upto_internal(limit, mask);
while (bias--) {
uint32 tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
tmp = random_upto_internal(limit, mask); if (tmp < ret) ret = tmp;
}

return ret;
}
