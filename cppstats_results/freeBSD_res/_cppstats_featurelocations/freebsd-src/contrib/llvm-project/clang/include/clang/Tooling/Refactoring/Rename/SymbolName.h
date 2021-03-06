







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_RENAME_SYMBOL_NAME_H)
#define LLVM_CLANG_TOOLING_REFACTOR_RENAME_SYMBOL_NAME_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
namespace tooling {










class SymbolName {
public:
explicit SymbolName(StringRef Name) {



assert(!Name.empty() && "Invalid symbol name!");
this->Name.push_back(Name.str());
}

ArrayRef<std::string> getNamePieces() const { return Name; }

private:
llvm::SmallVector<std::string, 1> Name;
};

}
}

#endif
