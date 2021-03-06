












#if !defined(LLVM_CLANG_BASIC_XRAYINSTR_H)
#define LLVM_CLANG_BASIC_XRAYINSTR_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MathExtras.h"
#include <cassert>
#include <cstdint>

namespace clang {

using XRayInstrMask = uint32_t;

namespace XRayInstrKind {


enum XRayInstrOrdinal : XRayInstrMask {
XRIO_FunctionEntry,
XRIO_FunctionExit,
XRIO_Custom,
XRIO_Typed,
XRIO_Count
};

constexpr XRayInstrMask None = 0;
constexpr XRayInstrMask FunctionEntry = 1U << XRIO_FunctionEntry;
constexpr XRayInstrMask FunctionExit = 1U << XRIO_FunctionExit;
constexpr XRayInstrMask Custom = 1U << XRIO_Custom;
constexpr XRayInstrMask Typed = 1U << XRIO_Typed;
constexpr XRayInstrMask All = FunctionEntry | FunctionExit | Custom | Typed;

}

struct XRayInstrSet {
bool has(XRayInstrMask K) const {
assert(llvm::isPowerOf2_32(K));
return Mask & K;
}

bool hasOneOf(XRayInstrMask K) const { return Mask & K; }

void set(XRayInstrMask K, bool Value) {
Mask = Value ? (Mask | K) : (Mask & ~K);
}

void clear(XRayInstrMask K = XRayInstrKind::All) { Mask &= ~K; }

bool empty() const { return Mask == 0; }

bool full() const { return Mask == XRayInstrKind::All; }

XRayInstrMask Mask = 0;
};


XRayInstrMask parseXRayInstrValue(StringRef Value);


void serializeXRayInstrValue(XRayInstrSet Set,
SmallVectorImpl<StringRef> &Values);

}

#endif
