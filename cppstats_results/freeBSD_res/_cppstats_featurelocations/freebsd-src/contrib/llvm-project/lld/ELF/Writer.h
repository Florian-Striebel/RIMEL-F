







#if !defined(LLD_ELF_WRITER_H)
#define LLD_ELF_WRITER_H

#include "Config.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include <cstdint>
#include <memory>

namespace lld {
namespace elf {
class InputFile;
class OutputSection;
class InputSectionBase;
void copySectionsIntoPartitions();
template <class ELFT> void createSyntheticSections();
void combineEhSections();
template <class ELFT> void writeResult();




struct PhdrEntry {
PhdrEntry(unsigned type, unsigned flags)
: p_align(type == llvm::ELF::PT_LOAD ? config->maxPageSize : 0),
p_type(type), p_flags(flags) {}
void add(OutputSection *sec);

uint64_t p_paddr = 0;
uint64_t p_vaddr = 0;
uint64_t p_memsz = 0;
uint64_t p_filesz = 0;
uint64_t p_offset = 0;
uint32_t p_align = 0;
uint32_t p_type = 0;
uint32_t p_flags = 0;

OutputSection *firstSec = nullptr;
OutputSection *lastSec = nullptr;
bool hasLMA = false;

uint64_t lmaOffset = 0;
};

void addReservedSymbols();
llvm::StringRef getOutputSectionName(const InputSectionBase *s);

template <class ELFT> uint32_t calcMipsEFlags();

uint8_t getMipsFpAbiFlag(uint8_t oldFlag, uint8_t newFlag,
llvm::StringRef fileName);

bool isMipsN32Abi(const InputFile *f);
bool isMicroMips();
bool isMipsR6();
}
}

#endif
