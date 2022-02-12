















#include <stdint.h>
#include <stdlib.h>

uint32_t uniform_random(uint32_t);
unsigned long prng_uint32(void);











uint32_t
uniform_random(uint32_t upper_bound)
{
uint32_t r, min;

if (upper_bound < 2)
return 0;


min = -upper_bound % upper_bound;







for (;;) {
r = (uint32_t)prng_uint32();
if (r >= min)
break;
}

return r % upper_bound;
}
