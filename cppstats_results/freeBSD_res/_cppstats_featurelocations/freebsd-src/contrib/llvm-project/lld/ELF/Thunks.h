







#if !defined(LLD_ELF_THUNKS_H)
#define LLD_ELF_THUNKS_H

#include "llvm/ADT/SmallVector.h"
#include "Relocations.h"

namespace lld {
namespace elf {
class Defined;
class InputFile;
class Symbol;
class ThunkSection;









class Thunk {
public:
Thunk(Symbol &destination, int64_t addend);
virtual ~Thunk();

virtual uint32_t size() = 0;
virtual void writeTo(uint8_t *buf) = 0;




virtual void addSymbols(ThunkSection &isec) = 0;

void setOffset(uint64_t offset);
Defined *addSymbol(StringRef name, uint8_t type, uint64_t value,
InputSectionBase &section);



virtual InputSection *getTargetInputSection() const { return nullptr; }



virtual bool isCompatibleWith(const InputSection &,
const Relocation &) const {
return true;
}

Defined *getThunkTargetSym() const { return syms[0]; }

Symbol &destination;
int64_t addend;
llvm::SmallVector<Defined *, 3> syms;
uint64_t offset = 0;


uint32_t alignment = 4;
};



Thunk *addThunk(const InputSection &isec, Relocation &rel);

void writePPC32PltCallStub(uint8_t *buf, uint64_t gotPltVA,
const InputFile *file, int64_t addend);
void writePPC64LoadAndBranch(uint8_t *buf, int64_t offset);

static inline uint16_t computeHiBits(uint32_t toCompute) {
return (toCompute + 0x8000) >> 16;
}

}
}

#endif
