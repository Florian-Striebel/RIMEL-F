







#if !defined(LLD_COFF_MINGW_H)
#define LLD_COFF_MINGW_H

#include "Config.h"
#include "Symbols.h"
#include "lld/Common/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Option/ArgList.h"
#include <vector>

namespace lld {
namespace coff {



class AutoExporter {
public:
AutoExporter();

void addWholeArchive(StringRef path);

llvm::StringSet<> excludeSymbols;
llvm::StringSet<> excludeSymbolPrefixes;
llvm::StringSet<> excludeSymbolSuffixes;
llvm::StringSet<> excludeLibs;
llvm::StringSet<> excludeObjects;

bool shouldExport(Defined *sym) const;
};

void writeDefFile(StringRef name);









struct WrappedSymbol {
Symbol *sym;
Symbol *real;
Symbol *wrap;
};

std::vector<WrappedSymbol> addWrappedSymbols(llvm::opt::InputArgList &args);

void wrapSymbols(ArrayRef<WrappedSymbol> wrapped);

}
}

#endif
