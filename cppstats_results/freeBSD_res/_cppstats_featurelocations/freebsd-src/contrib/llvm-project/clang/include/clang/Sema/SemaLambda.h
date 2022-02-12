













#if !defined(LLVM_CLANG_SEMA_SEMALAMBDA_H)
#define LLVM_CLANG_SEMA_SEMALAMBDA_H

#include "clang/AST/ASTLambda.h"

namespace clang {
namespace sema {
class FunctionScopeInfo;
}
class Sema;








Optional<unsigned> getStackIndexOfNearestEnclosingCaptureCapableLambda(
ArrayRef<const sema::FunctionScopeInfo *> FunctionScopes,
VarDecl *VarToCapture, Sema &S);

}

#endif
