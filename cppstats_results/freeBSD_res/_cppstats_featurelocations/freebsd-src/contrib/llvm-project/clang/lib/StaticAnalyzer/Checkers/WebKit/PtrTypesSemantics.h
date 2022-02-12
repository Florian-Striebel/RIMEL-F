







#if !defined(LLVM_CLANG_ANALYZER_WEBKIT_PTRTYPESEMANTICS_H)
#define LLVM_CLANG_ANALYZER_WEBKIT_PTRTYPESEMANTICS_H

#include "llvm/ADT/APInt.h"

namespace clang {
class CXXBaseSpecifier;
class CXXMethodDecl;
class CXXRecordDecl;
class Expr;
class FunctionDecl;
class Type;










llvm::Optional<const clang::CXXRecordDecl *>
isRefCountable(const clang::CXXBaseSpecifier *Base);



llvm::Optional<bool> isRefCountable(const clang::CXXRecordDecl *Class);


bool isRefCounted(const clang::CXXRecordDecl *Class);



llvm::Optional<bool> isUncounted(const clang::CXXRecordDecl *Class);



llvm::Optional<bool> isUncountedPtr(const clang::Type *T);



bool isCtorOfRefCounted(const clang::FunctionDecl *F);


llvm::Optional<bool> isGetterOfRefCounted(const clang::CXXMethodDecl *Method);



bool isPtrConversion(const FunctionDecl *F);

}

#endif
