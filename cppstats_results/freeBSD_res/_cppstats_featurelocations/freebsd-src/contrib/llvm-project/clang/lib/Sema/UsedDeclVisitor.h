











#if !defined(LLVM_CLANG_LIB_SEMA_USEDDECLVISITOR_H)
#define LLVM_CLANG_LIB_SEMA_USEDDECLVISITOR_H

#include "clang/AST/EvaluatedExprVisitor.h"
#include "clang/Sema/SemaInternal.h"

namespace clang {
template <class Derived>
class UsedDeclVisitor : public EvaluatedExprVisitor<Derived> {
protected:
Sema &S;

public:
typedef EvaluatedExprVisitor<Derived> Inherited;

UsedDeclVisitor(Sema &S) : Inherited(S.Context), S(S) {}

Derived &asImpl() { return *static_cast<Derived *>(this); }

void VisitDeclRefExpr(DeclRefExpr *E) {
auto *D = E->getDecl();
if (isa<FunctionDecl>(D) || isa<VarDecl>(D)) {
asImpl().visitUsedDecl(E->getLocation(), D);
}
}

void VisitMemberExpr(MemberExpr *E) {
auto *D = E->getMemberDecl();
if (isa<FunctionDecl>(D) || isa<VarDecl>(D)) {
asImpl().visitUsedDecl(E->getMemberLoc(), D);
}
asImpl().Visit(E->getBase());
}

void VisitCapturedStmt(CapturedStmt *Node) {
asImpl().visitUsedDecl(Node->getBeginLoc(), Node->getCapturedDecl());
Inherited::VisitCapturedStmt(Node);
}

void VisitCXXBindTemporaryExpr(CXXBindTemporaryExpr *E) {
asImpl().visitUsedDecl(
E->getBeginLoc(),
const_cast<CXXDestructorDecl *>(E->getTemporary()->getDestructor()));
asImpl().Visit(E->getSubExpr());
}

void VisitCXXNewExpr(CXXNewExpr *E) {
if (E->getOperatorNew())
asImpl().visitUsedDecl(E->getBeginLoc(), E->getOperatorNew());
if (E->getOperatorDelete())
asImpl().visitUsedDecl(E->getBeginLoc(), E->getOperatorDelete());
Inherited::VisitCXXNewExpr(E);
}

void VisitCXXDeleteExpr(CXXDeleteExpr *E) {
if (E->getOperatorDelete())
asImpl().visitUsedDecl(E->getBeginLoc(), E->getOperatorDelete());
QualType DestroyedOrNull = E->getDestroyedType();
if (!DestroyedOrNull.isNull()) {
QualType Destroyed = S.Context.getBaseElementType(DestroyedOrNull);
if (const RecordType *DestroyedRec = Destroyed->getAs<RecordType>()) {
CXXRecordDecl *Record = cast<CXXRecordDecl>(DestroyedRec->getDecl());
asImpl().visitUsedDecl(E->getBeginLoc(), S.LookupDestructor(Record));
}
}

Inherited::VisitCXXDeleteExpr(E);
}

void VisitCXXConstructExpr(CXXConstructExpr *E) {
asImpl().visitUsedDecl(E->getBeginLoc(), E->getConstructor());
Inherited::VisitCXXConstructExpr(E);
}

void VisitCXXDefaultArgExpr(CXXDefaultArgExpr *E) {
asImpl().Visit(E->getExpr());
}

void visitUsedDecl(SourceLocation Loc, Decl *D) {
if (auto *CD = dyn_cast<CapturedDecl>(D)) {
if (auto *S = CD->getBody()) {
asImpl().Visit(S);
}
} else if (auto *CD = dyn_cast<BlockDecl>(D)) {
if (auto *S = CD->getBody()) {
asImpl().Visit(S);
}
}
}
};
}

#endif
