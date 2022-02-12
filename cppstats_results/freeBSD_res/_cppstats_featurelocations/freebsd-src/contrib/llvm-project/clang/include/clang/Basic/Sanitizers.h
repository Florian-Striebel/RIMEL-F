












#if !defined(LLVM_CLANG_BASIC_SANITIZERS_H)
#define LLVM_CLANG_BASIC_SANITIZERS_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Transforms/Instrumentation/AddressSanitizerOptions.h"
#include <cassert>
#include <cstdint>

namespace llvm {
class hash_code;
}

namespace clang {

class SanitizerMask {





static constexpr unsigned kNumElem = 2;

uint64_t maskLoToHigh[kNumElem]{};

static constexpr unsigned kNumBits = sizeof(decltype(maskLoToHigh)) * 8;

static constexpr unsigned kNumBitElem = sizeof(decltype(maskLoToHigh[0])) * 8;

constexpr SanitizerMask(uint64_t mask1, uint64_t mask2)
: maskLoToHigh{mask1, mask2} {}

public:
SanitizerMask() = default;

static constexpr bool checkBitPos(const unsigned Pos) {
return Pos < kNumBits;
}


static constexpr SanitizerMask bitPosToMask(const unsigned Pos) {
uint64_t mask1 = (Pos < kNumBitElem) ? 1ULL << (Pos % kNumBitElem) : 0;
uint64_t mask2 = (Pos >= kNumBitElem && Pos < (kNumBitElem * 2))
? 1ULL << (Pos % kNumBitElem)
: 0;
return SanitizerMask(mask1, mask2);
}

unsigned countPopulation() const;

void flipAllBits() {
for (auto &Val : maskLoToHigh)
Val = ~Val;
}

bool isPowerOf2() const {
return countPopulation() == 1;
}

llvm::hash_code hash_value() const;

constexpr explicit operator bool() const {
return maskLoToHigh[0] || maskLoToHigh[1];
}

constexpr bool operator==(const SanitizerMask &V) const {
return maskLoToHigh[0] == V.maskLoToHigh[0] &&
maskLoToHigh[1] == V.maskLoToHigh[1];
}

SanitizerMask &operator&=(const SanitizerMask &RHS) {
for (unsigned k = 0; k < kNumElem; k++)
maskLoToHigh[k] &= RHS.maskLoToHigh[k];
return *this;
}

SanitizerMask &operator|=(const SanitizerMask &RHS) {
for (unsigned k = 0; k < kNumElem; k++)
maskLoToHigh[k] |= RHS.maskLoToHigh[k];
return *this;
}

constexpr bool operator!() const { return !bool(*this); }

constexpr bool operator!=(const SanitizerMask &RHS) const {
return !((*this) == RHS);
}

friend constexpr inline SanitizerMask operator~(SanitizerMask v) {
return SanitizerMask(~v.maskLoToHigh[0], ~v.maskLoToHigh[1]);
}

friend constexpr inline SanitizerMask operator&(SanitizerMask a,
const SanitizerMask &b) {
return SanitizerMask(a.maskLoToHigh[0] & b.maskLoToHigh[0],
a.maskLoToHigh[1] & b.maskLoToHigh[1]);
}

friend constexpr inline SanitizerMask operator|(SanitizerMask a,
const SanitizerMask &b) {
return SanitizerMask(a.maskLoToHigh[0] | b.maskLoToHigh[0],
a.maskLoToHigh[1] | b.maskLoToHigh[1]);
}
};


llvm::hash_code hash_value(const clang::SanitizerMask &Arg);



struct SanitizerKind {


enum SanitizerOrdinal : uint64_t {
#define SANITIZER(NAME, ID) SO_##ID,
#define SANITIZER_GROUP(NAME, ID, ALIAS) SO_##ID##Group,
#include "clang/Basic/Sanitizers.def"
SO_Count
};

#define SANITIZER(NAME, ID) static constexpr SanitizerMask ID = SanitizerMask::bitPosToMask(SO_##ID); static_assert(SanitizerMask::checkBitPos(SO_##ID), "Bit position too big.");


#define SANITIZER_GROUP(NAME, ID, ALIAS) static constexpr SanitizerMask ID = SanitizerMask(ALIAS); static constexpr SanitizerMask ID##Group = SanitizerMask::bitPosToMask(SO_##ID##Group); static_assert(SanitizerMask::checkBitPos(SO_##ID##Group), "Bit position too big.");





#include "clang/Basic/Sanitizers.def"
};

struct SanitizerSet {

bool has(SanitizerMask K) const {
assert(K.isPowerOf2() && "Has to be a single sanitizer.");
return static_cast<bool>(Mask & K);
}


bool hasOneOf(SanitizerMask K) const { return static_cast<bool>(Mask & K); }


void set(SanitizerMask K, bool Value) {
assert(K.isPowerOf2() && "Has to be a single sanitizer.");
Mask = Value ? (Mask | K) : (Mask & ~K);
}


void clear(SanitizerMask K = SanitizerKind::All) { Mask &= ~K; }


bool empty() const { return !Mask; }


SanitizerMask Mask;
};



SanitizerMask parseSanitizerValue(StringRef Value, bool AllowGroups);


void serializeSanitizerSet(SanitizerSet Set,
SmallVectorImpl<StringRef> &Values);



SanitizerMask expandSanitizerGroups(SanitizerMask Kinds);


inline SanitizerMask getPPTransparentSanitizers() {
return SanitizerKind::CFI | SanitizerKind::Integer |
SanitizerKind::ImplicitConversion | SanitizerKind::Nullability |
SanitizerKind::Undefined | SanitizerKind::FloatDivideByZero;
}

StringRef AsanDtorKindToString(llvm::AsanDtorKind kind);

llvm::AsanDtorKind AsanDtorKindFromString(StringRef kind);

StringRef AsanDetectStackUseAfterReturnModeToString(
llvm::AsanDetectStackUseAfterReturnMode mode);

llvm::AsanDetectStackUseAfterReturnMode
AsanDetectStackUseAfterReturnModeFromString(StringRef modeStr);

}

#endif
