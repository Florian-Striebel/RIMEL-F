











#if !defined(LLVM_CLANG_TOOLING_REFACTOR_LOOKUP_H)
#define LLVM_CLANG_TOOLING_REFACTOR_LOOKUP_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include <string>

namespace clang {

class DeclContext;
class NamedDecl;
class NestedNameSpecifier;

namespace tooling {














std::string replaceNestedName(const NestedNameSpecifier *Use,
SourceLocation UseLoc,
const DeclContext *UseContext,
const NamedDecl *FromDecl,
StringRef ReplacementString);

}
}

#endif
