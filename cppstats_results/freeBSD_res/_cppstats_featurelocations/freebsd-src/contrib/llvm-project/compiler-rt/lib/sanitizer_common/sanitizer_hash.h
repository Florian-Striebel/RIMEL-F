










#if !defined(SANITIZER_HASH_H)
#define SANITIZER_HASH_H

#include "sanitizer_internal_defs.h"

namespace __sanitizer {
class MurMur2HashBuilder {
static const u32 m = 0x5bd1e995;
static const u32 seed = 0x9747b28c;
static const u32 r = 24;
u32 h;

public:
explicit MurMur2HashBuilder(u32 init = 0) { h = seed ^ init; }
void add(u32 k) {
k *= m;
k ^= k >> r;
k *= m;
h *= m;
h ^= k;
}
u32 get() {
u32 x = h;
x ^= x >> 13;
x *= m;
x ^= x >> 15;
return x;
}
};
}

#endif
