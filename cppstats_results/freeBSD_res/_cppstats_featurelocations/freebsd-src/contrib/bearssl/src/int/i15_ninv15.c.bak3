























#include "inner.h"


uint16_t
br_i15_ninv15(uint16_t x)
{
uint32_t y;

y = 2 - x;
y = MUL15(y, 2 - MUL15(x, y));
y = MUL15(y, 2 - MUL15(x, y));
y = MUL15(y, 2 - MUL15(x, y));
return MUX(x & 1, -y, 0) & 0x7FFF;
}
