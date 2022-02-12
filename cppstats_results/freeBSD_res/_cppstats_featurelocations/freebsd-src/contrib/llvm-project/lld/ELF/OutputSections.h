







#if !defined(LLD_ELF_OUTPUT_SECTIONS_H)
#define LLD_ELF_OUTPUT_SECTIONS_H

#include "Config.h"
#include "InputSection.h"
#include "LinkerScript.h"
#include "Relocations.h"
#include "lld/Common/LLVM.h"
#include "llvm/MC/StringTableBuilder.h"
#include "llvm/Object/ELF.h"
#include <array>

namespace lld {
namespace elf {

struct PhdrEntry;
class InputSection;
class InputSectionBase;





class OutputSection final : public BaseCommand, public SectionBase {
public:
OutputSection(StringRef name, uint32_t type, uint64_t flags);

static bool classof(const SectionBase *s) {
return s->kind() == SectionBase::Output;
}

static bool classof(const BaseCommand *c);

uint64_t getLMA() const { return ptLoad ? addr + ptLoad->lmaOffset : addr; }
template <typename ELFT> void writeHeaderTo(typename ELFT::Shdr *sHdr);

uint32_t sectionIndex = UINT32_MAX;
unsigned sortRank;

uint32_t getPhdrFlags() const;








PhdrEntry *ptLoad = nullptr;




OutputSection *relocationSection = nullptr;




uint64_t size = 0;


uint64_t offset = 0;
uint64_t addr = 0;
uint32_t shName = 0;

void recordSection(InputSectionBase *isec);
void commitSection(InputSection *isec);
void finalizeInputSections();


MemoryRegion *memRegion = nullptr;
MemoryRegion *lmaRegion = nullptr;
Expr addrExpr;
Expr alignExpr;
Expr lmaExpr;
Expr subalignExpr;
std::vector<BaseCommand *> sectionCommands;
std::vector<StringRef> phdrs;
llvm::Optional<std::array<uint8_t, 4>> filler;
ConstraintKind constraint = ConstraintKind::NoConstraint;
std::string location;
std::string memoryRegionName;
std::string lmaRegionName;
bool nonAlloc = false;
bool noload = false;
bool expressionsUseSymbols = false;
bool usedInExpression = false;
bool inOverlay = false;




bool hasInputSections = false;

void finalize();
template <class ELFT> void writeTo(uint8_t *buf);

void checkDynRelAddends(const uint8_t *bufStart);
template <class ELFT> void maybeCompress();

void sort(llvm::function_ref<int(InputSectionBase *s)> order);
void sortInitFini();
void sortCtorsDtors();

private:

std::vector<uint8_t> zDebugHeader;
llvm::SmallVector<char, 0> compressedData;

std::array<uint8_t, 4> getFiller();
};

int getPriority(StringRef s);

InputSection *getFirstInputSection(const OutputSection *os);
std::vector<InputSection *> getInputSections(const OutputSection *os);




struct Out {
static uint8_t *bufferStart;
static uint8_t first;
static PhdrEntry *tlsPhdr;
static OutputSection *elfHeader;
static OutputSection *programHeaders;
static OutputSection *preinitArray;
static OutputSection *initArray;
static OutputSection *finiArray;
};

uint64_t getHeaderSize();

extern std::vector<OutputSection *> outputSections;
}
}

#endif
