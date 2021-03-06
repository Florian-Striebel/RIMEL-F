









#if !defined(LLVM_CLANG_TOOLING_SYNTAX_MUTATIONS_H)
#define LLVM_CLANG_TOOLING_SYNTAX_MUTATIONS_H

#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Syntax/Nodes.h"
#include "clang/Tooling/Syntax/Tree.h"

namespace clang {
namespace syntax {



tooling::Replacements computeReplacements(const Arena &A,
const syntax::TranslationUnit &TU);







void removeStatement(syntax::Arena &A, syntax::Statement *S);

}
}

#endif
