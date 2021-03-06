


























































#if !defined(LLVM_CLANG_AST_QUALTYPENAMES_H)
#define LLVM_CLANG_AST_QUALTYPENAMES_H

#include "clang/AST/ASTContext.h"

namespace clang {
namespace TypeName {








std::string getFullyQualifiedName(QualType QT, const ASTContext &Ctx,
const PrintingPolicy &Policy,
bool WithGlobalNsPrefix = false);










QualType getFullyQualifiedType(QualType QT, const ASTContext &Ctx,
bool WithGlobalNsPrefix = false);
}
}
#endif
