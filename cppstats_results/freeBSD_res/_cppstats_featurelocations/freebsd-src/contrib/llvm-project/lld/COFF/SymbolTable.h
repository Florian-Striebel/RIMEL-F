







#if !defined(LLD_COFF_SYMBOL_TABLE_H)
#define LLD_COFF_SYMBOL_TABLE_H

#include "InputFiles.h"
#include "LTO.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
struct LTOCodeGenerator;
}

namespace lld {
namespace coff {

class Chunk;
class CommonChunk;
class Defined;
class DefinedAbsolute;
class DefinedRegular;
class DefinedRelative;
class LazyArchive;
class SectionChunk;
class Symbol;













class SymbolTable {
public:
void addFile(InputFile *file);


void reportUnresolvable();




void resolveRemainingUndefines();



void loadMinGWSymbols();
bool handleMinGWAutomaticImport(Symbol *sym, StringRef name);


std::vector<Chunk *> getChunks();


Symbol *find(StringRef name);
Symbol *findUnderscore(StringRef name);





Symbol *findMangle(StringRef name);




void addCombinedLTOObjects();


Symbol *addUndefined(StringRef name);

Symbol *addSynthetic(StringRef n, Chunk *c);
Symbol *addAbsolute(StringRef n, uint64_t va);

Symbol *addUndefined(StringRef name, InputFile *f, bool isWeakAlias);
void addLazyArchive(ArchiveFile *f, const Archive::Symbol &sym);
void addLazyObject(LazyObjFile *f, StringRef n);
void addLazyDLLSymbol(DLLFile *f, DLLFile::Symbol *sym, StringRef n);
Symbol *addAbsolute(StringRef n, COFFSymbolRef s);
Symbol *addRegular(InputFile *f, StringRef n,
const llvm::object::coff_symbol_generic *s = nullptr,
SectionChunk *c = nullptr, uint32_t sectionOffset = 0);
std::pair<DefinedRegular *, bool>
addComdat(InputFile *f, StringRef n,
const llvm::object::coff_symbol_generic *s = nullptr);
Symbol *addCommon(InputFile *f, StringRef n, uint64_t size,
const llvm::object::coff_symbol_generic *s = nullptr,
CommonChunk *c = nullptr);
Symbol *addImportData(StringRef n, ImportFile *f);
Symbol *addImportThunk(StringRef name, DefinedImportData *s,
uint16_t machine);
void addLibcall(StringRef name);

void reportDuplicate(Symbol *existing, InputFile *newFile,
SectionChunk *newSc = nullptr,
uint32_t newSectionOffset = 0);


std::vector<Chunk *> localImportChunks;


template <typename T> void forEachSymbol(T callback) {
for (auto &pair : symMap)
callback(pair.second);
}

private:


Defined *impSymbol(StringRef name);

std::pair<Symbol *, bool> insert(StringRef name);

std::pair<Symbol *, bool> insert(StringRef name, InputFile *f);

std::vector<Symbol *> getSymsWithPrefix(StringRef prefix);

llvm::DenseMap<llvm::CachedHashStringRef, Symbol *> symMap;
std::unique_ptr<BitcodeCompiler> lto;
};

extern SymbolTable *symtab;

std::vector<std::string> getSymbolLocations(ObjFile *file, uint32_t symIndex);

StringRef ltrim1(StringRef s, const char *chars);

}
}

#endif
