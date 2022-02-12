







#if !defined(LLD_MACHO_MERGED_OUTPUT_SECTION_H)
#define LLD_MACHO_MERGED_OUTPUT_SECTION_H

#include "InputSection.h"
#include "OutputSection.h"
#include "lld/Common/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/MapVector.h"

namespace lld {
namespace macho {

class Defined;





class ConcatOutputSection final : public OutputSection {
public:
explicit ConcatOutputSection(StringRef name)
: OutputSection(ConcatKind, name) {}

const ConcatInputSection *firstSection() const { return inputs.front(); }
const ConcatInputSection *lastSection() const { return inputs.back(); }
bool isNeeded() const override { return !inputs.empty(); }


uint64_t getSize() const override { return size; }
uint64_t getFileSize() const override { return fileSize; }

void addInput(ConcatInputSection *input);
void finalize() override;
bool needsThunks() const;
uint64_t estimateStubsInRangeVA(size_t callIdx) const;

void writeTo(uint8_t *buf) const override;

std::vector<ConcatInputSection *> inputs;
std::vector<ConcatInputSection *> thunks;

static bool classof(const OutputSection *sec) {
return sec->kind() == ConcatKind;
}

static ConcatOutputSection *getOrCreateForInput(const InputSection *);

private:
void finalizeFlags(InputSection *input);

size_t size = 0;
uint64_t fileSize = 0;
};











struct ThunkInfo {

Defined *sym = nullptr;
ConcatInputSection *isec = nullptr;


uint32_t callSiteCount = 0;
uint32_t callSitesUsed = 0;
uint32_t thunkCallCount = 0;
uint8_t sequence = 0;
};

NamePair maybeRenameSection(NamePair key);



extern llvm::MapVector<NamePair, ConcatOutputSection *> concatOutputSections;

extern llvm::DenseMap<Symbol *, ThunkInfo> thunkMap;

}
}

#endif
