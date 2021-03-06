












#if !defined(LLVM_CLANG_LIB_AST_CXXABI_H)
#define LLVM_CLANG_LIB_AST_CXXABI_H

#include "clang/AST/Type.h"

namespace clang {

class ASTContext;
class CXXConstructorDecl;
class DeclaratorDecl;
class Expr;
class MangleContext;
class MangleNumberingContext;
class MemberPointerType;


class CXXABI {
public:
virtual ~CXXABI();

struct MemberPointerInfo {
uint64_t Width;
unsigned Align;
bool HasPadding;
};



virtual MemberPointerInfo
getMemberPointerInfo(const MemberPointerType *MPT) const = 0;


virtual CallingConv getDefaultMethodCallConv(bool isVariadic) const = 0;



virtual bool isNearlyEmpty(const CXXRecordDecl *RD) const = 0;


virtual std::unique_ptr<MangleNumberingContext>
createMangleNumberingContext() const = 0;


virtual void addCopyConstructorForExceptionObject(CXXRecordDecl *,
CXXConstructorDecl *) = 0;


virtual const CXXConstructorDecl *
getCopyConstructorForExceptionObject(CXXRecordDecl *) = 0;

virtual void addTypedefNameForUnnamedTagDecl(TagDecl *TD,
TypedefNameDecl *DD) = 0;

virtual TypedefNameDecl *
getTypedefNameForUnnamedTagDecl(const TagDecl *TD) = 0;

virtual void addDeclaratorForUnnamedTagDecl(TagDecl *TD,
DeclaratorDecl *DD) = 0;

virtual DeclaratorDecl *getDeclaratorForUnnamedTagDecl(const TagDecl *TD) = 0;
};


CXXABI *CreateItaniumCXXABI(ASTContext &Ctx);
CXXABI *CreateMicrosoftCXXABI(ASTContext &Ctx);
std::unique_ptr<MangleNumberingContext>
createItaniumNumberingContext(MangleContext *);
}

#endif
