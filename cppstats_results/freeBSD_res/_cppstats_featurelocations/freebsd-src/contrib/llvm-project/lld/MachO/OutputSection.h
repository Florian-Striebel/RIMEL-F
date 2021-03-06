







#if !defined(LLD_MACHO_OUTPUT_SECTION_H)
#define LLD_MACHO_OUTPUT_SECTION_H

#include "Symbols.h"
#include "lld/Common/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/TinyPtrVector.h"

#include <limits>

namespace lld {
namespace macho {

class Defined;
class InputSection;
class OutputSegment;





constexpr int UnspecifiedInputOrder = std::numeric_limits<int>::max() - 1024;





class OutputSection {
public:
enum Kind {
ConcatKind,
SyntheticKind,
};

OutputSection(Kind kind, StringRef name) : name(name), sectionKind(kind) {}
virtual ~OutputSection() = default;
Kind kind() const { return sectionKind; }


uint64_t getSegmentOffset() const;


virtual uint64_t getSize() const = 0;


virtual uint64_t getFileSize() const { return getSize(); }


virtual bool isHidden() const { return false; }

virtual bool isNeeded() const { return true; }

virtual void finalize() {


}

virtual void writeTo(uint8_t *buf) const = 0;

void assignAddressesToStartEndSymbols();

StringRef name;
llvm::TinyPtrVector<Defined *> sectionStartSymbols;
llvm::TinyPtrVector<Defined *> sectionEndSymbols;
OutputSegment *parent = nullptr;



int inputOrder = UnspecifiedInputOrder;

uint32_t index = 0;
uint64_t addr = 0;
uint64_t fileOff = 0;
uint32_t align = 1;
uint32_t flags = 0;
uint32_t reserved1 = 0;
uint32_t reserved2 = 0;

private:
Kind sectionKind;
};

}
}

#endif
