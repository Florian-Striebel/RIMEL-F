























#include "inner.h"


uint32_t
br_i32_ninv32(uint32_t x)
{
uint32_t y;

y = 2 - x;
y *= 2 - y * x;
y *= 2 - y * x;
y *= 2 - y * x;
y *= 2 - y * x;
return MUX(x & 1, -y, 0);
}
