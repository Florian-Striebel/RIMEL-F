







#if !defined(LLD_COFF_DLL_H)
#define LLD_COFF_DLL_H

#include "Chunks.h"
#include "Symbols.h"

namespace lld {
namespace coff {





class IdataContents {
public:
void add(DefinedImportData *sym) { imports.push_back(sym); }
bool empty() { return imports.empty(); }

void create();

std::vector<DefinedImportData *> imports;
std::vector<Chunk *> dirs;
std::vector<Chunk *> lookups;
std::vector<Chunk *> addresses;
std::vector<Chunk *> hints;
std::vector<Chunk *> dllNames;
};



class DelayLoadContents {
public:
void add(DefinedImportData *sym) { imports.push_back(sym); }
bool empty() { return imports.empty(); }
void create(Defined *helper);
std::vector<Chunk *> getChunks();
std::vector<Chunk *> getDataChunks();
ArrayRef<Chunk *> getCodeChunks() { return thunks; }

uint64_t getDirRVA() { return dirs[0]->getRVA(); }
uint64_t getDirSize();

private:
Chunk *newThunkChunk(DefinedImportData *s, Chunk *tailMerge);
Chunk *newTailMergeChunk(Chunk *dir);

Defined *helper;
std::vector<DefinedImportData *> imports;
std::vector<Chunk *> dirs;
std::vector<Chunk *> moduleHandles;
std::vector<Chunk *> addresses;
std::vector<Chunk *> names;
std::vector<Chunk *> hintNames;
std::vector<Chunk *> thunks;
std::vector<Chunk *> dllNames;
};



class EdataContents {
public:
EdataContents();
std::vector<Chunk *> chunks;

uint64_t getRVA() { return chunks[0]->getRVA(); }
uint64_t getSize() {
return chunks.back()->getRVA() + chunks.back()->getSize() - getRVA();
}
};

}
}

#endif
