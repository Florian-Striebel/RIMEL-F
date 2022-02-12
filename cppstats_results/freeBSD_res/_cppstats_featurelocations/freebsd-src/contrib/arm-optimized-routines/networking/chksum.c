







#include "networking.h"
#include "chksum_common.h"

always_inline
static inline uint32_t
slurp_head32(const void **pptr, uint32_t *nbytes)
{
uint32_t sum = 0;
Assert(*nbytes >= 4);
uint32_t off = (uintptr_t) *pptr % 4;
if (likely(off != 0))
{

const unsigned char *ptr32 = align_ptr(*pptr, 4);
uint32_t mask = ~0U << (CHAR_BIT * off);
sum = load32(ptr32) & mask;
*pptr = ptr32 + 4;
*nbytes -= 4 - off;
}
return sum;
}


unsigned short
__chksum(const void *ptr, unsigned int nbytes)
{
bool swap = false;
uint64_t sum = 0;

if (nbytes > 300)
{

swap = (uintptr_t) ptr & 1;
sum = slurp_head32(&ptr, &nbytes);
}



const char *cptr = ptr;
for (uint32_t nquads = nbytes / 16; nquads != 0; nquads--)
{
uint64_t h0 = load32(cptr + 0);
uint64_t h1 = load32(cptr + 4);
uint64_t h2 = load32(cptr + 8);
uint64_t h3 = load32(cptr + 12);
sum += h0 + h1 + h2 + h3;
cptr += 16;
}
nbytes %= 16;
Assert(nbytes < 16);


while (nbytes >= 4)
{
sum += load32(cptr);
cptr += 4;
nbytes -= 4;
}
Assert(nbytes < 4);

if (nbytes & 2)
{
sum += load16(cptr);
cptr += 2;
}

if (nbytes & 1)
{
sum += *(uint8_t *)cptr;
}

return fold_and_swap(sum, swap);
}
