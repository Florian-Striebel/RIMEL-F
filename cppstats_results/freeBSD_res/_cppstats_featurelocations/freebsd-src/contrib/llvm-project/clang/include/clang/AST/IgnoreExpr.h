











#if !defined(LLVM_CLANG_AST_IGNOREEXPR_H)
#define LLVM_CLANG_AST_IGNOREEXPR_H

#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"

namespace clang {
namespace detail {


inline Expr *IgnoreExprNodesImpl(Expr *E) { return E; }
template <typename FnTy, typename... FnTys>
Expr *IgnoreExprNodesImpl(Expr *E, FnTy &&Fn, FnTys &&... Fns) {
return IgnoreExprNodesImpl(Fn(E), std::forward<FnTys>(Fns)...);
}
}




template <typename... FnTys> Expr *IgnoreExprNodes(Expr *E, FnTys &&... Fns) {
Expr *LastE = nullptr;
while (E != LastE) {
LastE = E;
E = detail::IgnoreExprNodesImpl(E, std::forward<FnTys>(Fns)...);
}
return E;
}

template <typename... FnTys>
const Expr *IgnoreExprNodes(const Expr *E, FnTys &&...Fns) {
return IgnoreExprNodes(const_cast<Expr *>(E), std::forward<FnTys>(Fns)...);
}

inline Expr *IgnoreImplicitCastsSingleStep(Expr *E) {
if (auto *ICE = dyn_cast<ImplicitCastExpr>(E))
return ICE->getSubExpr();

if (auto *FE = dyn_cast<FullExpr>(E))
return FE->getSubExpr();

return E;
}

inline Expr *IgnoreImplicitCastsExtraSingleStep(Expr *E) {



Expr *SubE = IgnoreImplicitCastsSingleStep(E);
if (SubE != E)
return SubE;

if (auto *MTE = dyn_cast<MaterializeTemporaryExpr>(E))
return MTE->getSubExpr();

if (auto *NTTP = dyn_cast<SubstNonTypeTemplateParmExpr>(E))
return NTTP->getReplacement();

return E;
}

inline Expr *IgnoreCastsSingleStep(Expr *E) {
if (auto *CE = dyn_cast<CastExpr>(E))
return CE->getSubExpr();

if (auto *FE = dyn_cast<FullExpr>(E))
return FE->getSubExpr();

if (auto *MTE = dyn_cast<MaterializeTemporaryExpr>(E))
return MTE->getSubExpr();

if (auto *NTTP = dyn_cast<SubstNonTypeTemplateParmExpr>(E))
return NTTP->getReplacement();

return E;
}

inline Expr *IgnoreLValueCastsSingleStep(Expr *E) {


if (auto *CE = dyn_cast<CastExpr>(E))
if (CE->getCastKind() != CK_LValueToRValue)
return E;

return IgnoreCastsSingleStep(E);
}

inline Expr *IgnoreBaseCastsSingleStep(Expr *E) {
if (auto *CE = dyn_cast<CastExpr>(E))
if (CE->getCastKind() == CK_DerivedToBase ||
CE->getCastKind() == CK_UncheckedDerivedToBase ||
CE->getCastKind() == CK_NoOp)
return CE->getSubExpr();

return E;
}

inline Expr *IgnoreImplicitSingleStep(Expr *E) {
Expr *SubE = IgnoreImplicitCastsSingleStep(E);
if (SubE != E)
return SubE;

if (auto *MTE = dyn_cast<MaterializeTemporaryExpr>(E))
return MTE->getSubExpr();

if (auto *BTE = dyn_cast<CXXBindTemporaryExpr>(E))
return BTE->getSubExpr();

return E;
}

inline Expr *IgnoreElidableImplicitConstructorSingleStep(Expr *E) {
auto *CCE = dyn_cast<CXXConstructExpr>(E);
if (CCE && CCE->isElidable() && !isa<CXXTemporaryObjectExpr>(CCE)) {
unsigned NumArgs = CCE->getNumArgs();
if ((NumArgs == 1 ||
(NumArgs > 1 && CCE->getArg(1)->isDefaultArgument())) &&
!CCE->getArg(0)->isDefaultArgument() && !CCE->isListInitialization())
return CCE->getArg(0);
}
return E;
}

inline Expr *IgnoreImplicitAsWrittenSingleStep(Expr *E) {
if (auto *ICE = dyn_cast<ImplicitCastExpr>(E))
return ICE->getSubExprAsWritten();

return IgnoreImplicitSingleStep(E);
}

inline Expr *IgnoreParensOnlySingleStep(Expr *E) {
if (auto *PE = dyn_cast<ParenExpr>(E))
return PE->getSubExpr();
return E;
}

inline Expr *IgnoreParensSingleStep(Expr *E) {
if (auto *PE = dyn_cast<ParenExpr>(E))
return PE->getSubExpr();

if (auto *UO = dyn_cast<UnaryOperator>(E)) {
if (UO->getOpcode() == UO_Extension)
return UO->getSubExpr();
}

else if (auto *GSE = dyn_cast<GenericSelectionExpr>(E)) {
if (!GSE->isResultDependent())
return GSE->getResultExpr();
}

else if (auto *CE = dyn_cast<ChooseExpr>(E)) {
if (!CE->isConditionDependent())
return CE->getChosenSubExpr();
}

return E;
}

}

#endif
