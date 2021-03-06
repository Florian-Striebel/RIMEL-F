







#if !defined(LLD_MACHO_RELOCATIONS_H)
#define LLD_MACHO_RELOCATIONS_H

#include "llvm/ADT/BitmaskEnum.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/BinaryFormat/MachO.h"
#include "llvm/Support/Endian.h"

#include <cstddef>
#include <cstdint>

namespace lld {
namespace macho {
LLVM_ENABLE_BITMASK_ENUMS_IN_NAMESPACE();

class Symbol;
class InputSection;

enum class RelocAttrBits {
_0 = 0,
PCREL = 1 << 0,
ABSOLUTE = 1 << 1,
BYTE4 = 1 << 2,
BYTE8 = 1 << 3,
EXTERN = 1 << 4,
LOCAL = 1 << 5,
ADDEND = 1 << 6,
SUBTRAHEND = 1 << 7,
BRANCH = 1 << 8,
GOT = 1 << 9,
TLV = 1 << 10,
LOAD = 1 << 11,
POINTER = 1 << 12,
UNSIGNED = 1 << 13,
LLVM_MARK_AS_BITMASK_ENUM( (1 << 14) - 1),
};


struct RelocAttrs {
llvm::StringRef name;
RelocAttrBits bits;
bool hasAttr(RelocAttrBits b) const { return (bits & b) == b; }
};

struct Reloc {
uint8_t type = llvm::MachO::GENERIC_RELOC_INVALID;
bool pcrel = false;
uint8_t length = 0;


uint64_t offset = 0;


int64_t addend = 0;
llvm::PointerUnion<Symbol *, InputSection *> referent = nullptr;
};

bool validateSymbolRelocation(const Symbol *, const InputSection *,
const Reloc &);





void reportRangeError(const Reloc &, const llvm::Twine &v, uint8_t bits,
int64_t min, uint64_t max);

struct SymbolDiagnostic {
const Symbol *symbol;
llvm::StringRef reason;
};

void reportRangeError(SymbolDiagnostic, const llvm::Twine &v, uint8_t bits,
int64_t min, uint64_t max);

template <typename Diagnostic>
inline void checkInt(Diagnostic d, int64_t v, int bits) {
if (v != llvm::SignExtend64(v, bits))
reportRangeError(d, llvm::Twine(v), bits, llvm::minIntN(bits),
llvm::maxIntN(bits));
}

template <typename Diagnostic>
inline void checkUInt(Diagnostic d, uint64_t v, int bits) {
if ((v >> bits) != 0)
reportRangeError(d, llvm::Twine(v), bits, 0, llvm::maxUIntN(bits));
}

inline void writeAddress(uint8_t *loc, uint64_t addr, uint8_t length) {
switch (length) {
case 2:
llvm::support::endian::write32le(loc, addr);
break;
case 3:
llvm::support::endian::write64le(loc, addr);
break;
default:
llvm_unreachable("invalid r_length");
}
}

extern const RelocAttrs invalidRelocAttrs;

}
}

#endif
