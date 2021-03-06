







#if !defined(LLVM_CLANG_ANALYZER_WEBKIT_DIAGPRINTUTILS_H)
#define LLVM_CLANG_ANALYZER_WEBKIT_DIAGPRINTUTILS_H

#include "clang/AST/Decl.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {

template <typename NamedDeclDerivedT>
void printQuotedQualifiedName(llvm::raw_ostream &Os,
const NamedDeclDerivedT &D) {
Os << "'";
D->getNameForDiagnostic(Os, D->getASTContext().getPrintingPolicy(),
true);
Os << "'";
}

template <typename NamedDeclDerivedT>
void printQuotedName(llvm::raw_ostream &Os, const NamedDeclDerivedT &D) {
Os << "'";
D->getNameForDiagnostic(Os, D->getASTContext().getPrintingPolicy(),
false);
Os << "'";
}

}

#endif
