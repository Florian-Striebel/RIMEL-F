












#if !defined(LLVM_CLANG_TOOLING_TRANSFORMER_SOURCE_CODE_BUILDERS_H_)
#define LLVM_CLANG_TOOLING_TRANSFORMER_SOURCE_CODE_BUILDERS_H_

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include <string>

namespace clang {
namespace tooling {





const Expr *reallyIgnoreImplicit(const Expr &E);




bool mayEverNeedParens(const Expr &E);





inline bool needParensBeforeDotOrArrow(const Expr &E) {
return mayEverNeedParens(E);
}



bool needParensAfterUnaryOperator(const Expr &E);







llvm::Optional<std::string> buildParens(const Expr &E,
const ASTContext &Context);



llvm::Optional<std::string> buildDereference(const Expr &E,
const ASTContext &Context);



llvm::Optional<std::string> buildAddressOf(const Expr &E,
const ASTContext &Context);







llvm::Optional<std::string> buildDot(const Expr &E, const ASTContext &Context);







llvm::Optional<std::string> buildArrow(const Expr &E,
const ASTContext &Context);


}
}
#endif
