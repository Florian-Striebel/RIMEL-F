







#if !defined(LLVM_CLANG_INDEX_INDEXINGOPTIONS_H)
#define LLVM_CLANG_INDEX_INDEXINGOPTIONS_H

#include "clang/Frontend/FrontendOptions.h"
#include <memory>
#include <string>

namespace clang {
class Decl;
namespace index {

struct IndexingOptions {
enum class SystemSymbolFilterKind {
None,
DeclarationsOnly,
All,
};

SystemSymbolFilterKind SystemSymbolFilter =
SystemSymbolFilterKind::DeclarationsOnly;
bool IndexFunctionLocals = false;
bool IndexImplicitInstantiation = false;
bool IndexMacros = true;



bool IndexMacrosInPreprocessor = false;

bool IndexParametersInDeclarations = false;
bool IndexTemplateParameters = false;





std::function<bool(const Decl *)> ShouldTraverseDecl;
};

}
}

#endif
