












#if !defined(LLVM_CLANG_SEMA_SEMACONCEPT_H)
#define LLVM_CLANG_SEMA_SEMACONCEPT_H
#include "clang/AST/ASTConcept.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include <string>
#include <utility>

namespace clang {
class Sema;

struct AtomicConstraint {
const Expr *ConstraintExpr;
Optional<MutableArrayRef<TemplateArgumentLoc>> ParameterMapping;

AtomicConstraint(Sema &S, const Expr *ConstraintExpr) :
ConstraintExpr(ConstraintExpr) { };

bool hasMatchingParameterMapping(ASTContext &C,
const AtomicConstraint &Other) const {
if (!ParameterMapping != !Other.ParameterMapping)
return false;
if (!ParameterMapping)
return true;
if (ParameterMapping->size() != Other.ParameterMapping->size())
return false;

for (unsigned I = 0, S = ParameterMapping->size(); I < S; ++I) {
llvm::FoldingSetNodeID IDA, IDB;
C.getCanonicalTemplateArgument((*ParameterMapping)[I].getArgument())
.Profile(IDA, C);
C.getCanonicalTemplateArgument((*Other.ParameterMapping)[I].getArgument())
.Profile(IDB, C);
if (IDA != IDB)
return false;
}
return true;
}

bool subsumes(ASTContext &C, const AtomicConstraint &Other) const {












if (ConstraintExpr != Other.ConstraintExpr)
return false;


return hasMatchingParameterMapping(C, Other);
}
};




struct NormalizedConstraint {
friend class Sema;

enum CompoundConstraintKind { CCK_Conjunction, CCK_Disjunction };

using CompoundConstraint = llvm::PointerIntPair<
std::pair<NormalizedConstraint, NormalizedConstraint> *, 1,
CompoundConstraintKind>;

llvm::PointerUnion<AtomicConstraint *, CompoundConstraint> Constraint;

NormalizedConstraint(AtomicConstraint *C): Constraint{C} { };
NormalizedConstraint(ASTContext &C, NormalizedConstraint LHS,
NormalizedConstraint RHS, CompoundConstraintKind Kind)
: Constraint{CompoundConstraint{
new (C) std::pair<NormalizedConstraint, NormalizedConstraint>{
std::move(LHS), std::move(RHS)}, Kind}} { };

NormalizedConstraint(ASTContext &C, const NormalizedConstraint &Other) {
if (Other.isAtomic()) {
Constraint = new (C) AtomicConstraint(*Other.getAtomicConstraint());
} else {
Constraint = CompoundConstraint(
new (C) std::pair<NormalizedConstraint, NormalizedConstraint>{
NormalizedConstraint(C, Other.getLHS()),
NormalizedConstraint(C, Other.getRHS())},
Other.getCompoundKind());
}
}
NormalizedConstraint(NormalizedConstraint &&Other):
Constraint(Other.Constraint) {
Other.Constraint = nullptr;
}
NormalizedConstraint &operator=(const NormalizedConstraint &Other) = delete;
NormalizedConstraint &operator=(NormalizedConstraint &&Other) {
if (&Other != this) {
NormalizedConstraint Temp(std::move(Other));
std::swap(Constraint, Temp.Constraint);
}
return *this;
}

CompoundConstraintKind getCompoundKind() const {
assert(!isAtomic() && "getCompoundKind called on atomic constraint.");
return Constraint.get<CompoundConstraint>().getInt();
}

bool isAtomic() const { return Constraint.is<AtomicConstraint *>(); }

NormalizedConstraint &getLHS() const {
assert(!isAtomic() && "getLHS called on atomic constraint.");
return Constraint.get<CompoundConstraint>().getPointer()->first;
}

NormalizedConstraint &getRHS() const {
assert(!isAtomic() && "getRHS called on atomic constraint.");
return Constraint.get<CompoundConstraint>().getPointer()->second;
}

AtomicConstraint *getAtomicConstraint() const {
assert(isAtomic() &&
"getAtomicConstraint called on non-atomic constraint.");
return Constraint.get<AtomicConstraint *>();
}

private:
static Optional<NormalizedConstraint>
fromConstraintExprs(Sema &S, NamedDecl *D, ArrayRef<const Expr *> E);
static Optional<NormalizedConstraint>
fromConstraintExpr(Sema &S, NamedDecl *D, const Expr *E);
};

}

#endif
