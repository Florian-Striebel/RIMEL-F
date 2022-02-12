







#if !defined(LLVM_CLANG_ANALYZER_WEBKIT_ASTUTILS_H)
#define LLVM_CLANG_ANALYZER_WEBKIT_ASTUTILS_H

#include "clang/AST/Decl.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/Casting.h"

#include <string>
#include <utility>

namespace clang {
class CXXRecordDecl;
class CXXBaseSpecifier;
class FunctionDecl;
class CXXMethodDecl;
class Expr;
































std::pair<const clang::Expr *, bool>
tryToFindPtrOrigin(const clang::Expr *E, bool StopAtFirstRefCountedObj);







bool isASafeCallArg(const clang::Expr *E);


template <typename T> std::string safeGetName(const T *ASTNode) {
const auto *const ND = llvm::dyn_cast_or_null<clang::NamedDecl>(ASTNode);
if (!ND)
return "";



if (!ND->getDeclName().isIdentifier())
return "";

return ND->getName().str();
}

}

#endif
