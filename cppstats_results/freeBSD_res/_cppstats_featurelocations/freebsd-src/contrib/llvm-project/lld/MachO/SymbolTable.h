







#if !defined(LLD_MACHO_SYMBOL_TABLE_H)
#define LLD_MACHO_SYMBOL_TABLE_H

#include "Symbols.h"

#include "lld/Common/LLVM.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Object/Archive.h"

namespace lld {
namespace macho {

class ArchiveFile;
class DylibFile;
class InputFile;
class ObjFile;
class InputSection;
class MachHeaderSection;
class Symbol;
class Defined;
class Undefined;







class SymbolTable {
public:
Defined *addDefined(StringRef name, InputFile *, InputSection *,
uint64_t value, uint64_t size, bool isWeakDef,
bool isPrivateExtern, bool isThumb,
bool isReferencedDynamically, bool noDeadStrip);

Symbol *addUndefined(StringRef name, InputFile *, bool isWeakRef);

Symbol *addCommon(StringRef name, InputFile *, uint64_t size, uint32_t align,
bool isPrivateExtern);

Symbol *addDylib(StringRef name, DylibFile *file, bool isWeakDef, bool isTlv);
Symbol *addDynamicLookup(StringRef name);

Symbol *addLazy(StringRef name, ArchiveFile *file,
const llvm::object::Archive::Symbol &sym);

Defined *addSynthetic(StringRef name, InputSection *, uint64_t value,
bool isPrivateExtern, bool includeInSymtab,
bool referencedDynamically);

ArrayRef<Symbol *> getSymbols() const { return symVector; }
Symbol *find(llvm::CachedHashStringRef name);
Symbol *find(StringRef name) { return find(llvm::CachedHashStringRef(name)); }

private:
std::pair<Symbol *, bool> insert(StringRef name, const InputFile *);
llvm::DenseMap<llvm::CachedHashStringRef, int> symMap;
std::vector<Symbol *> symVector;
};

void treatUndefinedSymbol(const Undefined &, StringRef source = "");

extern SymbolTable *symtab;

}
}

#endif
