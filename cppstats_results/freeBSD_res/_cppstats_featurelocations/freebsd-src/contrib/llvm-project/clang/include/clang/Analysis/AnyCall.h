











#if !defined(LLVM_CLANG_ANALYSIS_ANY_CALL_H)
#define LLVM_CLANG_ANALYSIS_ANY_CALL_H

#include "clang/AST/Decl.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"

namespace clang {




class AnyCall {
public:
enum Kind {

Function,


ObjCMethod,


Block,



Destructor,


Constructor,


InheritedConstructor,


Allocator,



Deallocator
};

private:





const Expr *E = nullptr;



const Decl *D = nullptr;
Kind K;

public:
AnyCall(const CallExpr *CE) : E(CE) {
D = CE->getCalleeDecl();
K = (CE->getCallee()->getType()->getAs<BlockPointerType>()) ? Block
: Function;
if (D && ((K == Function && !isa<FunctionDecl>(D)) ||
(K == Block && !isa<BlockDecl>(D))))
D = nullptr;
}

AnyCall(const ObjCMessageExpr *ME)
: E(ME), D(ME->getMethodDecl()), K(ObjCMethod) {}

AnyCall(const CXXNewExpr *NE)
: E(NE), D(NE->getOperatorNew()), K(Allocator) {}

AnyCall(const CXXDeleteExpr *NE)
: E(NE), D(NE->getOperatorDelete()), K(Deallocator) {}

AnyCall(const CXXConstructExpr *NE)
: E(NE), D(NE->getConstructor()), K(Constructor) {}

AnyCall(const CXXInheritedCtorInitExpr *CIE)
: E(CIE), D(CIE->getConstructor()), K(InheritedConstructor) {}

AnyCall(const CXXDestructorDecl *D) : E(nullptr), D(D), K(Destructor) {}

AnyCall(const CXXConstructorDecl *D) : E(nullptr), D(D), K(Constructor) {}

AnyCall(const ObjCMethodDecl *D) : E(nullptr), D(D), K(ObjCMethod) {}

AnyCall(const FunctionDecl *D) : E(nullptr), D(D) {
if (isa<CXXConstructorDecl>(D)) {
K = Constructor;
} else if (isa <CXXDestructorDecl>(D)) {
K = Destructor;
} else {
K = Function;
}

}



static Optional<AnyCall> forExpr(const Expr *E) {
if (const auto *ME = dyn_cast<ObjCMessageExpr>(E)) {
return AnyCall(ME);
} else if (const auto *CE = dyn_cast<CallExpr>(E)) {
return AnyCall(CE);
} else if (const auto *CXNE = dyn_cast<CXXNewExpr>(E)) {
return AnyCall(CXNE);
} else if (const auto *CXDE = dyn_cast<CXXDeleteExpr>(E)) {
return AnyCall(CXDE);
} else if (const auto *CXCE = dyn_cast<CXXConstructExpr>(E)) {
return AnyCall(CXCE);
} else if (const auto *CXCIE = dyn_cast<CXXInheritedCtorInitExpr>(E)) {
return AnyCall(CXCIE);
} else {
return None;
}
}




static Optional<AnyCall> forDecl(const Decl *D) {
if (const auto *FD = dyn_cast<FunctionDecl>(D)) {
return AnyCall(FD);
} else if (const auto *MD = dyn_cast<ObjCMethodDecl>(D)) {
return AnyCall(MD);
}
return None;
}


ArrayRef<ParmVarDecl *> parameters() const {
if (!D)
return None;

if (const auto *FD = dyn_cast<FunctionDecl>(D)) {
return FD->parameters();
} else if (const auto *MD = dyn_cast<ObjCMethodDecl>(D)) {
return MD->parameters();
} else if (const auto *BD = dyn_cast<BlockDecl>(D)) {
return BD->parameters();
} else {
return None;
}
}

using param_const_iterator = ArrayRef<ParmVarDecl *>::const_iterator;
param_const_iterator param_begin() const { return parameters().begin(); }
param_const_iterator param_end() const { return parameters().end(); }
size_t param_size() const { return parameters().size(); }
bool param_empty() const { return parameters().empty(); }

QualType getReturnType(ASTContext &Ctx) const {
switch (K) {
case Function:
if (E)
return cast<CallExpr>(E)->getCallReturnType(Ctx);
return cast<FunctionDecl>(D)->getReturnType();
case ObjCMethod:
if (E)
return cast<ObjCMessageExpr>(E)->getCallReturnType(Ctx);
return cast<ObjCMethodDecl>(D)->getReturnType();
case Block:


return cast<CallExpr>(E)->getCallReturnType(Ctx);
case Destructor:
case Constructor:
case InheritedConstructor:
case Allocator:
case Deallocator:
return cast<FunctionDecl>(D)->getReturnType();
}
llvm_unreachable("Unknown AnyCall::Kind");
}



const IdentifierInfo *getIdentifier() const {
if (const auto *ND = dyn_cast_or_null<NamedDecl>(D))
return ND->getIdentifier();
return nullptr;
}

const Decl *getDecl() const {
return D;
}

const Expr *getExpr() const {
return E;
}

Kind getKind() const {
return K;
}

void dump() const {
if (E)
E->dump();
if (D)
D->dump();
}
};

}

#endif
