













#if !defined(LLVM_CLANG_TOOLING_REFACTOR_RENAME_USR_LOC_FINDER_H)
#define LLVM_CLANG_TOOLING_REFACTOR_RENAME_USR_LOC_FINDER_H

#include "clang/AST/AST.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Refactoring/Rename/SymbolOccurrences.h"
#include "llvm/ADT/StringRef.h"
#include <string>
#include <vector>

namespace clang {
namespace tooling {









std::vector<tooling::AtomicChange>
createRenameAtomicChanges(llvm::ArrayRef<std::string> USRs,
llvm::StringRef NewName, Decl *TranslationUnitDecl);






SymbolOccurrences getOccurrencesOfUSRs(ArrayRef<std::string> USRs,
StringRef PrevName, Decl *Decl);

}
}

#endif
