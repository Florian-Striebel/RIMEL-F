











#include "int_lib.h"

COMPILER_RT_ABI uint64_t __bswapdi2(uint64_t u) {
return (
(((u)&0xff00000000000000ULL) >> 56) |
(((u)&0x00ff000000000000ULL) >> 40) |
(((u)&0x0000ff0000000000ULL) >> 24) |
(((u)&0x000000ff00000000ULL) >> 8) |
(((u)&0x00000000ff000000ULL) << 8) |
(((u)&0x0000000000ff0000ULL) << 24) |
(((u)&0x000000000000ff00ULL) << 40) |
(((u)&0x00000000000000ffULL) << 56));
}
