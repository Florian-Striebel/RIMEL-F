







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_RENAME_SYMBOL_OCCURRENCES_H)
#define LLVM_CLANG_TOOLING_REFACTOR_RENAME_SYMBOL_OCCURRENCES_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace clang {
namespace tooling {

class SymbolName;

















class SymbolOccurrence {
public:
enum OccurrenceKind {


















MatchingSymbol
};

SymbolOccurrence(const SymbolName &Name, OccurrenceKind Kind,
ArrayRef<SourceLocation> Locations);

SymbolOccurrence(SymbolOccurrence &&) = default;
SymbolOccurrence &operator=(SymbolOccurrence &&) = default;

OccurrenceKind getKind() const { return Kind; }

ArrayRef<SourceRange> getNameRanges() const {
if (MultipleRanges)
return llvm::makeArrayRef(MultipleRanges.get(), NumRanges);
return SingleRange;
}

private:
OccurrenceKind Kind;
std::unique_ptr<SourceRange[]> MultipleRanges;
union {
SourceRange SingleRange;
unsigned NumRanges;
};
};

using SymbolOccurrences = std::vector<SymbolOccurrence>;

}
}

#endif
