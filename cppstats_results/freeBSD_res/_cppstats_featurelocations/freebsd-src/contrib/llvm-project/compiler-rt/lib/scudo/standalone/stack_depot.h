







#if !defined(SCUDO_STACK_DEPOT_H_)
#define SCUDO_STACK_DEPOT_H_

#include "atomic_helpers.h"
#include "mutex.h"

namespace scudo {

class MurMur2HashBuilder {
static const u32 M = 0x5bd1e995;
static const u32 Seed = 0x9747b28c;
static const u32 R = 24;
u32 H;

public:
explicit MurMur2HashBuilder(u32 Init = 0) { H = Seed ^ Init; }
void add(u32 K) {
K *= M;
K ^= K >> R;
K *= M;
H *= M;
H ^= K;
}
u32 get() {
u32 X = H;
X ^= X >> 13;
X *= M;
X ^= X >> 15;
return X;
}
};

class StackDepot {
HybridMutex RingEndMu;
u32 RingEnd = 0;





















#if defined(SCUDO_FUZZ)

static const uptr TabBits = 4;
#else
static const uptr TabBits = 16;
#endif
static const uptr TabSize = 1 << TabBits;
static const uptr TabMask = TabSize - 1;
atomic_u32 Tab[TabSize] = {};

#if defined(SCUDO_FUZZ)
static const uptr RingBits = 4;
#else
static const uptr RingBits = 19;
#endif
static const uptr RingSize = 1 << RingBits;
static const uptr RingMask = RingSize - 1;
atomic_u64 Ring[RingSize] = {};

public:


u32 insert(uptr *Begin, uptr *End) {
MurMur2HashBuilder B;
for (uptr *I = Begin; I != End; ++I)
B.add(u32(*I) >> 2);
u32 Hash = B.get();

u32 Pos = Hash & TabMask;
u32 RingPos = atomic_load_relaxed(&Tab[Pos]);
u64 Entry = atomic_load_relaxed(&Ring[RingPos]);
u64 Id = (u64(End - Begin) << 33) | (u64(Hash) << 1) | 1;
if (Entry == Id)
return Hash;

ScopedLock Lock(RingEndMu);
RingPos = RingEnd;
atomic_store_relaxed(&Tab[Pos], RingPos);
atomic_store_relaxed(&Ring[RingPos], Id);
for (uptr *I = Begin; I != End; ++I) {
RingPos = (RingPos + 1) & RingMask;
atomic_store_relaxed(&Ring[RingPos], *I);
}
RingEnd = (RingPos + 1) & RingMask;
return Hash;
}




bool find(u32 Hash, uptr *RingPosPtr, uptr *SizePtr) const {
u32 Pos = Hash & TabMask;
u32 RingPos = atomic_load_relaxed(&Tab[Pos]);
if (RingPos >= RingSize)
return false;
u64 Entry = atomic_load_relaxed(&Ring[RingPos]);
u64 HashWithTagBit = (u64(Hash) << 1) | 1;
if ((Entry & 0x1ffffffff) != HashWithTagBit)
return false;
u32 Size = u32(Entry >> 33);
if (Size >= RingSize)
return false;
*RingPosPtr = (RingPos + 1) & RingMask;
*SizePtr = Size;
MurMur2HashBuilder B;
for (uptr I = 0; I != Size; ++I) {
RingPos = (RingPos + 1) & RingMask;
B.add(u32(atomic_load_relaxed(&Ring[RingPos])) >> 2);
}
return B.get() == Hash;
}

u64 operator[](uptr RingPos) const {
return atomic_load_relaxed(&Ring[RingPos & RingMask]);
}
};

}

#endif
