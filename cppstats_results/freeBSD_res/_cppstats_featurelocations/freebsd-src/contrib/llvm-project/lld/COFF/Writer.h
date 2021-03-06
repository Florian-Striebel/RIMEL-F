







#if !defined(LLD_COFF_WRITER_H)
#define LLD_COFF_WRITER_H

#include "Chunks.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Object/COFF.h"
#include <chrono>
#include <cstdint>
#include <vector>

namespace lld {
namespace coff {
static const int pageSize = 4096;

void writeResult();

class PartialSection {
public:
PartialSection(StringRef n, uint32_t chars)
: name(n), characteristics(chars) {}
StringRef name;
unsigned characteristics;
std::vector<Chunk *> chunks;
};






class OutputSection {
public:
OutputSection(llvm::StringRef n, uint32_t chars) : name(n) {
header.Characteristics = chars;
}
void addChunk(Chunk *c);
void insertChunkAtStart(Chunk *c);
void merge(OutputSection *other);
void setPermissions(uint32_t c);
uint64_t getRVA() { return header.VirtualAddress; }
uint64_t getFileOff() { return header.PointerToRawData; }
void writeHeaderTo(uint8_t *buf);
void addContributingPartialSection(PartialSection *sec);


static void clear();






uint64_t getVirtualSize() { return header.VirtualSize; }


uint64_t getRawSize() { return header.SizeOfRawData; }



void setStringTableOff(uint32_t v) { stringTableOff = v; }


uint32_t sectionIndex = 0;

llvm::StringRef name;
llvm::object::coff_section header = {};

std::vector<Chunk *> chunks;
std::vector<Chunk *> origChunks;

std::vector<PartialSection *> contribSections;

private:
uint32_t stringTableOff = 0;
};

}
}

#endif
