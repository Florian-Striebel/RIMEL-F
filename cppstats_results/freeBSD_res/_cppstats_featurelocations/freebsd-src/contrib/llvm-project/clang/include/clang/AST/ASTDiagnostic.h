







#if !defined(LLVM_CLANG_AST_ASTDIAGNOSTIC_H)
#define LLVM_CLANG_AST_ASTDIAGNOSTIC_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticAST.h"

namespace clang {









void FormatASTNodeDiagnosticArgument(
DiagnosticsEngine::ArgumentKind Kind,
intptr_t Val,
StringRef Modifier,
StringRef Argument,
ArrayRef<DiagnosticsEngine::ArgumentValue> PrevArgs,
SmallVectorImpl<char> &Output,
void *Cookie,
ArrayRef<intptr_t> QualTypeVals);
}

#endif
