












#if !defined(LLVM_CLANG_AST_EVALUATEDEXPRVISITOR_H)
#define LLVM_CLANG_AST_EVALUATEDEXPRVISITOR_H

#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/StmtVisitor.h"
#include "llvm/ADT/STLExtras.h"

namespace clang {

class ASTContext;



template<template <typename> class Ptr, typename ImplClass>
class EvaluatedExprVisitorBase : public StmtVisitorBase<Ptr, ImplClass, void> {
protected:
const ASTContext &Context;

public:


bool shouldVisitDiscardedStmt() const { return true; }
#define PTR(CLASS) typename Ptr<CLASS>::type

explicit EvaluatedExprVisitorBase(const ASTContext &Context) : Context(Context) { }



void VisitDeclRefExpr(PTR(DeclRefExpr) E) { }
void VisitOffsetOfExpr(PTR(OffsetOfExpr) E) { }
void VisitUnaryExprOrTypeTraitExpr(PTR(UnaryExprOrTypeTraitExpr) E) { }
void VisitExpressionTraitExpr(PTR(ExpressionTraitExpr) E) { }
void VisitBlockExpr(PTR(BlockExpr) E) { }
void VisitCXXUuidofExpr(PTR(CXXUuidofExpr) E) { }
void VisitCXXNoexceptExpr(PTR(CXXNoexceptExpr) E) { }

void VisitMemberExpr(PTR(MemberExpr) E) {

return this->Visit(E->getBase());
}

void VisitChooseExpr(PTR(ChooseExpr) E) {

if (E->getCond()->isValueDependent())
return;

return this->Visit(E->getChosenSubExpr());
}

void VisitGenericSelectionExpr(PTR(GenericSelectionExpr) E) {



if (E->isResultDependent())
return;


return this->Visit(E->getResultExpr());
}

void VisitDesignatedInitExpr(PTR(DesignatedInitExpr) E) {


return this->Visit(E->getInit());
}

void VisitCXXTypeidExpr(PTR(CXXTypeidExpr) E) {
if (E->isPotentiallyEvaluated())
return this->Visit(E->getExprOperand());
}

void VisitCallExpr(PTR(CallExpr) CE) {
if (!CE->isUnevaluatedBuiltinCall(Context))
return getDerived().VisitExpr(CE);
}

void VisitLambdaExpr(PTR(LambdaExpr) LE) {

for (LambdaExpr::const_capture_init_iterator I = LE->capture_init_begin(),
E = LE->capture_init_end();
I != E; ++I)
if (*I)
this->Visit(*I);
}



void VisitStmt(PTR(Stmt) S) {
for (auto *SubStmt : S->children())
if (SubStmt)
this->Visit(SubStmt);
}

void VisitIfStmt(PTR(IfStmt) If) {
if (!getDerived().shouldVisitDiscardedStmt()) {
if (auto SubStmt = If->getNondiscardedCase(Context)) {
if (*SubStmt)
this->Visit(*SubStmt);
return;
}
}

getDerived().VisitStmt(If);
}

ImplClass &getDerived() { return *static_cast<ImplClass *>(this); }

#undef PTR
};


template <typename ImplClass>
class EvaluatedExprVisitor
: public EvaluatedExprVisitorBase<std::add_pointer, ImplClass> {
public:
explicit EvaluatedExprVisitor(const ASTContext &Context)
: EvaluatedExprVisitorBase<std::add_pointer, ImplClass>(Context) {}
};


template <typename ImplClass>
class ConstEvaluatedExprVisitor
: public EvaluatedExprVisitorBase<llvm::make_const_ptr, ImplClass> {
public:
explicit ConstEvaluatedExprVisitor(const ASTContext &Context)
: EvaluatedExprVisitorBase<llvm::make_const_ptr, ImplClass>(Context) {}
};
}

#endif
