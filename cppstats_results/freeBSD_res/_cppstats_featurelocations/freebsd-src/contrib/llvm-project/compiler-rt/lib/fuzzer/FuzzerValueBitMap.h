









#if !defined(LLVM_FUZZER_VALUE_BIT_MAP_H)
#define LLVM_FUZZER_VALUE_BIT_MAP_H

#include "FuzzerPlatform.h"
#include <cstdint>

namespace fuzzer {


struct ValueBitMap {
static const size_t kMapSizeInBits = 1 << 16;
static const size_t kMapPrimeMod = 65371;
static const size_t kBitsInWord = (sizeof(uintptr_t) * 8);
static const size_t kMapSizeInWords = kMapSizeInBits / kBitsInWord;
public:


void Reset() { memset(Map, 0, sizeof(Map)); }



ATTRIBUTE_NO_SANITIZE_ALL
inline bool AddValue(uintptr_t Value) {
uintptr_t Idx = Value % kMapSizeInBits;
uintptr_t WordIdx = Idx / kBitsInWord;
uintptr_t BitIdx = Idx % kBitsInWord;
uintptr_t Old = Map[WordIdx];
uintptr_t New = Old | (1ULL << BitIdx);
Map[WordIdx] = New;
return New != Old;
}

ATTRIBUTE_NO_SANITIZE_ALL
inline bool AddValueModPrime(uintptr_t Value) {
return AddValue(Value % kMapPrimeMod);
}

inline bool Get(uintptr_t Idx) {
assert(Idx < kMapSizeInBits);
uintptr_t WordIdx = Idx / kBitsInWord;
uintptr_t BitIdx = Idx % kBitsInWord;
return Map[WordIdx] & (1ULL << BitIdx);
}

size_t SizeInBits() const { return kMapSizeInBits; }

template <class Callback>
ATTRIBUTE_NO_SANITIZE_ALL
void ForEach(Callback CB) const {
for (size_t i = 0; i < kMapSizeInWords; i++)
if (uintptr_t M = Map[i])
for (size_t j = 0; j < sizeof(M) * 8; j++)
if (M & ((uintptr_t)1 << j))
CB(i * sizeof(M) * 8 + j);
}

private:
ATTRIBUTE_ALIGNED(512) uintptr_t Map[kMapSizeInWords];
};

}

#endif
