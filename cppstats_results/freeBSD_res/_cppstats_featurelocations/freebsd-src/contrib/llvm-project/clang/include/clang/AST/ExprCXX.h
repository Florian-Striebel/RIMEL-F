












#if !defined(LLVM_CLANG_AST_EXPRCXX_H)
#define LLVM_CLANG_AST_EXPRCXX_H

#include "clang/AST/ASTConcept.h"
#include "clang/AST/ComputeDependence.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/DependenceFlags.h"
#include "clang/AST/Expr.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/OperationKinds.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/Type.h"
#include "clang/AST/UnresolvedSet.h"
#include "clang/Basic/ExceptionSpecificationType.h"
#include "clang/Basic/ExpressionTraits.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/Lambda.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Basic/TypeTraits.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace clang {

class ASTContext;
class DeclAccessPair;
class IdentifierInfo;
class LambdaCapture;
class NonTypeTemplateParmDecl;
class TemplateParameterList;


















class CXXOperatorCallExpr final : public CallExpr {
friend class ASTStmtReader;
friend class ASTStmtWriter;

SourceRange Range;




SourceRange getSourceRangeImpl() const LLVM_READONLY;

CXXOperatorCallExpr(OverloadedOperatorKind OpKind, Expr *Fn,
ArrayRef<Expr *> Args, QualType Ty, ExprValueKind VK,
SourceLocation OperatorLoc, FPOptionsOverride FPFeatures,
ADLCallKind UsesADL);

CXXOperatorCallExpr(unsigned NumArgs, bool HasFPFeatures, EmptyShell Empty);

public:
static CXXOperatorCallExpr *
Create(const ASTContext &Ctx, OverloadedOperatorKind OpKind, Expr *Fn,
ArrayRef<Expr *> Args, QualType Ty, ExprValueKind VK,
SourceLocation OperatorLoc, FPOptionsOverride FPFeatures,
ADLCallKind UsesADL = NotADL);

static CXXOperatorCallExpr *CreateEmpty(const ASTContext &Ctx,
unsigned NumArgs, bool HasFPFeatures,
EmptyShell Empty);


OverloadedOperatorKind getOperator() const {
return static_cast<OverloadedOperatorKind>(
CXXOperatorCallExprBits.OperatorKind);
}

static bool isAssignmentOp(OverloadedOperatorKind Opc) {
return Opc == OO_Equal || Opc == OO_StarEqual || Opc == OO_SlashEqual ||
Opc == OO_PercentEqual || Opc == OO_PlusEqual ||
Opc == OO_MinusEqual || Opc == OO_LessLessEqual ||
Opc == OO_GreaterGreaterEqual || Opc == OO_AmpEqual ||
Opc == OO_CaretEqual || Opc == OO_PipeEqual;
}
bool isAssignmentOp() const { return isAssignmentOp(getOperator()); }

static bool isComparisonOp(OverloadedOperatorKind Opc) {
switch (Opc) {
case OO_EqualEqual:
case OO_ExclaimEqual:
case OO_Greater:
case OO_GreaterEqual:
case OO_Less:
case OO_LessEqual:
case OO_Spaceship:
return true;
default:
return false;
}
}
bool isComparisonOp() const { return isComparisonOp(getOperator()); }


bool isInfixBinaryOp() const;






SourceLocation getOperatorLoc() const { return getRParenLoc(); }

SourceLocation getExprLoc() const LLVM_READONLY {
OverloadedOperatorKind Operator = getOperator();
return (Operator < OO_Plus || Operator >= OO_Arrow ||
Operator == OO_PlusPlus || Operator == OO_MinusMinus)
? getBeginLoc()
: getOperatorLoc();
}

SourceLocation getBeginLoc() const { return Range.getBegin(); }
SourceLocation getEndLoc() const { return Range.getEnd(); }
SourceRange getSourceRange() const { return Range; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXOperatorCallExprClass;
}
};









class CXXMemberCallExpr final : public CallExpr {



CXXMemberCallExpr(Expr *Fn, ArrayRef<Expr *> Args, QualType Ty,
ExprValueKind VK, SourceLocation RP,
FPOptionsOverride FPOptions, unsigned MinNumArgs);

CXXMemberCallExpr(unsigned NumArgs, bool HasFPFeatures, EmptyShell Empty);

public:
static CXXMemberCallExpr *Create(const ASTContext &Ctx, Expr *Fn,
ArrayRef<Expr *> Args, QualType Ty,
ExprValueKind VK, SourceLocation RP,
FPOptionsOverride FPFeatures,
unsigned MinNumArgs = 0);

static CXXMemberCallExpr *CreateEmpty(const ASTContext &Ctx, unsigned NumArgs,
bool HasFPFeatures, EmptyShell Empty);




Expr *getImplicitObjectArgument() const;




QualType getObjectType() const;


CXXMethodDecl *getMethodDecl() const;







CXXRecordDecl *getRecordDecl() const;

SourceLocation getExprLoc() const LLVM_READONLY {
SourceLocation CLoc = getCallee()->getExprLoc();
if (CLoc.isValid())
return CLoc;

return getBeginLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXMemberCallExprClass;
}
};


class CUDAKernelCallExpr final : public CallExpr {
friend class ASTStmtReader;

enum { CONFIG, END_PREARG };




CUDAKernelCallExpr(Expr *Fn, CallExpr *Config, ArrayRef<Expr *> Args,
QualType Ty, ExprValueKind VK, SourceLocation RP,
FPOptionsOverride FPFeatures, unsigned MinNumArgs);

CUDAKernelCallExpr(unsigned NumArgs, bool HasFPFeatures, EmptyShell Empty);

public:
static CUDAKernelCallExpr *Create(const ASTContext &Ctx, Expr *Fn,
CallExpr *Config, ArrayRef<Expr *> Args,
QualType Ty, ExprValueKind VK,
SourceLocation RP,
FPOptionsOverride FPFeatures,
unsigned MinNumArgs = 0);

static CUDAKernelCallExpr *CreateEmpty(const ASTContext &Ctx,
unsigned NumArgs, bool HasFPFeatures,
EmptyShell Empty);

const CallExpr *getConfig() const {
return cast_or_null<CallExpr>(getPreArg(CONFIG));
}
CallExpr *getConfig() { return cast_or_null<CallExpr>(getPreArg(CONFIG)); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CUDAKernelCallExprClass;
}
};

















class CXXRewrittenBinaryOperator : public Expr {
friend class ASTStmtReader;


Stmt *SemanticForm;

public:
CXXRewrittenBinaryOperator(Expr *SemanticForm, bool IsReversed)
: Expr(CXXRewrittenBinaryOperatorClass, SemanticForm->getType(),
SemanticForm->getValueKind(), SemanticForm->getObjectKind()),
SemanticForm(SemanticForm) {
CXXRewrittenBinaryOperatorBits.IsReversed = IsReversed;
setDependence(computeDependence(this));
}
CXXRewrittenBinaryOperator(EmptyShell Empty)
: Expr(CXXRewrittenBinaryOperatorClass, Empty), SemanticForm() {}


Expr *getSemanticForm() { return cast<Expr>(SemanticForm); }
const Expr *getSemanticForm() const { return cast<Expr>(SemanticForm); }

struct DecomposedForm {

BinaryOperatorKind Opcode;

const Expr *LHS;

const Expr *RHS;

const Expr *InnerBinOp;
};


DecomposedForm getDecomposedForm() const LLVM_READONLY;


bool isReversed() const { return CXXRewrittenBinaryOperatorBits.IsReversed; }

BinaryOperatorKind getOperator() const { return getDecomposedForm().Opcode; }
BinaryOperatorKind getOpcode() const { return getOperator(); }
static StringRef getOpcodeStr(BinaryOperatorKind Op) {
return BinaryOperator::getOpcodeStr(Op);
}
StringRef getOpcodeStr() const {
return BinaryOperator::getOpcodeStr(getOpcode());
}
bool isComparisonOp() const { return true; }
bool isAssignmentOp() const { return false; }

const Expr *getLHS() const { return getDecomposedForm().LHS; }
const Expr *getRHS() const { return getDecomposedForm().RHS; }

SourceLocation getOperatorLoc() const LLVM_READONLY {
return getDecomposedForm().InnerBinOp->getExprLoc();
}
SourceLocation getExprLoc() const LLVM_READONLY { return getOperatorLoc(); }





SourceLocation getBeginLoc() const LLVM_READONLY {
return getDecomposedForm().LHS->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getDecomposedForm().RHS->getEndLoc();
}
SourceRange getSourceRange() const LLVM_READONLY {
DecomposedForm DF = getDecomposedForm();
return SourceRange(DF.LHS->getBeginLoc(), DF.RHS->getEndLoc());
}


child_range children() {
return child_range(&SemanticForm, &SemanticForm + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXRewrittenBinaryOperatorClass;
}
};








class CXXNamedCastExpr : public ExplicitCastExpr {
private:

SourceLocation Loc;


SourceLocation RParenLoc;


SourceRange AngleBrackets;

protected:
friend class ASTStmtReader;

CXXNamedCastExpr(StmtClass SC, QualType ty, ExprValueKind VK, CastKind kind,
Expr *op, unsigned PathSize, bool HasFPFeatures,
TypeSourceInfo *writtenTy, SourceLocation l,
SourceLocation RParenLoc, SourceRange AngleBrackets)
: ExplicitCastExpr(SC, ty, VK, kind, op, PathSize, HasFPFeatures,
writtenTy),
Loc(l), RParenLoc(RParenLoc), AngleBrackets(AngleBrackets) {}

explicit CXXNamedCastExpr(StmtClass SC, EmptyShell Shell, unsigned PathSize,
bool HasFPFeatures)
: ExplicitCastExpr(SC, Shell, PathSize, HasFPFeatures) {}

public:
const char *getCastName() const;



SourceLocation getOperatorLoc() const { return Loc; }


SourceLocation getRParenLoc() const { return RParenLoc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }
SourceRange getAngleBrackets() const LLVM_READONLY { return AngleBrackets; }

static bool classof(const Stmt *T) {
switch (T->getStmtClass()) {
case CXXStaticCastExprClass:
case CXXDynamicCastExprClass:
case CXXReinterpretCastExprClass:
case CXXConstCastExprClass:
case CXXAddrspaceCastExprClass:
return true;
default:
return false;
}
}
};





class CXXStaticCastExpr final
: public CXXNamedCastExpr,
private llvm::TrailingObjects<CXXStaticCastExpr, CXXBaseSpecifier *,
FPOptionsOverride> {
CXXStaticCastExpr(QualType ty, ExprValueKind vk, CastKind kind, Expr *op,
unsigned pathSize, TypeSourceInfo *writtenTy,
FPOptionsOverride FPO, SourceLocation l,
SourceLocation RParenLoc, SourceRange AngleBrackets)
: CXXNamedCastExpr(CXXStaticCastExprClass, ty, vk, kind, op, pathSize,
FPO.requiresTrailingStorage(), writtenTy, l, RParenLoc,
AngleBrackets) {
if (hasStoredFPFeatures())
*getTrailingFPFeatures() = FPO;
}

explicit CXXStaticCastExpr(EmptyShell Empty, unsigned PathSize,
bool HasFPFeatures)
: CXXNamedCastExpr(CXXStaticCastExprClass, Empty, PathSize,
HasFPFeatures) {}

unsigned numTrailingObjects(OverloadToken<CXXBaseSpecifier *>) const {
return path_size();
}

public:
friend class CastExpr;
friend TrailingObjects;

static CXXStaticCastExpr *
Create(const ASTContext &Context, QualType T, ExprValueKind VK, CastKind K,
Expr *Op, const CXXCastPath *Path, TypeSourceInfo *Written,
FPOptionsOverride FPO, SourceLocation L, SourceLocation RParenLoc,
SourceRange AngleBrackets);
static CXXStaticCastExpr *CreateEmpty(const ASTContext &Context,
unsigned PathSize, bool hasFPFeatures);

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXStaticCastExprClass;
}
};






class CXXDynamicCastExpr final
: public CXXNamedCastExpr,
private llvm::TrailingObjects<CXXDynamicCastExpr, CXXBaseSpecifier *> {
CXXDynamicCastExpr(QualType ty, ExprValueKind VK, CastKind kind, Expr *op,
unsigned pathSize, TypeSourceInfo *writtenTy,
SourceLocation l, SourceLocation RParenLoc,
SourceRange AngleBrackets)
: CXXNamedCastExpr(CXXDynamicCastExprClass, ty, VK, kind, op, pathSize,
false, writtenTy, l, RParenLoc,
AngleBrackets) {}

explicit CXXDynamicCastExpr(EmptyShell Empty, unsigned pathSize)
: CXXNamedCastExpr(CXXDynamicCastExprClass, Empty, pathSize,
false) {}

public:
friend class CastExpr;
friend TrailingObjects;

static CXXDynamicCastExpr *Create(const ASTContext &Context, QualType T,
ExprValueKind VK, CastKind Kind, Expr *Op,
const CXXCastPath *Path,
TypeSourceInfo *Written, SourceLocation L,
SourceLocation RParenLoc,
SourceRange AngleBrackets);

static CXXDynamicCastExpr *CreateEmpty(const ASTContext &Context,
unsigned pathSize);

bool isAlwaysNull() const;

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXDynamicCastExprClass;
}
};









class CXXReinterpretCastExpr final
: public CXXNamedCastExpr,
private llvm::TrailingObjects<CXXReinterpretCastExpr,
CXXBaseSpecifier *> {
CXXReinterpretCastExpr(QualType ty, ExprValueKind vk, CastKind kind, Expr *op,
unsigned pathSize, TypeSourceInfo *writtenTy,
SourceLocation l, SourceLocation RParenLoc,
SourceRange AngleBrackets)
: CXXNamedCastExpr(CXXReinterpretCastExprClass, ty, vk, kind, op,
pathSize, false, writtenTy, l,
RParenLoc, AngleBrackets) {}

CXXReinterpretCastExpr(EmptyShell Empty, unsigned pathSize)
: CXXNamedCastExpr(CXXReinterpretCastExprClass, Empty, pathSize,
false) {}

public:
friend class CastExpr;
friend TrailingObjects;

static CXXReinterpretCastExpr *Create(const ASTContext &Context, QualType T,
ExprValueKind VK, CastKind Kind,
Expr *Op, const CXXCastPath *Path,
TypeSourceInfo *WrittenTy, SourceLocation L,
SourceLocation RParenLoc,
SourceRange AngleBrackets);
static CXXReinterpretCastExpr *CreateEmpty(const ASTContext &Context,
unsigned pathSize);

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXReinterpretCastExprClass;
}
};








class CXXConstCastExpr final
: public CXXNamedCastExpr,
private llvm::TrailingObjects<CXXConstCastExpr, CXXBaseSpecifier *> {
CXXConstCastExpr(QualType ty, ExprValueKind VK, Expr *op,
TypeSourceInfo *writtenTy, SourceLocation l,
SourceLocation RParenLoc, SourceRange AngleBrackets)
: CXXNamedCastExpr(CXXConstCastExprClass, ty, VK, CK_NoOp, op, 0,
false, writtenTy, l, RParenLoc,
AngleBrackets) {}

explicit CXXConstCastExpr(EmptyShell Empty)
: CXXNamedCastExpr(CXXConstCastExprClass, Empty, 0,
false) {}

public:
friend class CastExpr;
friend TrailingObjects;

static CXXConstCastExpr *Create(const ASTContext &Context, QualType T,
ExprValueKind VK, Expr *Op,
TypeSourceInfo *WrittenTy, SourceLocation L,
SourceLocation RParenLoc,
SourceRange AngleBrackets);
static CXXConstCastExpr *CreateEmpty(const ASTContext &Context);

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXConstCastExprClass;
}
};









class CXXAddrspaceCastExpr final
: public CXXNamedCastExpr,
private llvm::TrailingObjects<CXXAddrspaceCastExpr, CXXBaseSpecifier *> {
CXXAddrspaceCastExpr(QualType ty, ExprValueKind VK, CastKind Kind, Expr *op,
TypeSourceInfo *writtenTy, SourceLocation l,
SourceLocation RParenLoc, SourceRange AngleBrackets)
: CXXNamedCastExpr(CXXAddrspaceCastExprClass, ty, VK, Kind, op, 0,
false, writtenTy, l, RParenLoc,
AngleBrackets) {}

explicit CXXAddrspaceCastExpr(EmptyShell Empty)
: CXXNamedCastExpr(CXXAddrspaceCastExprClass, Empty, 0,
false) {}

public:
friend class CastExpr;
friend TrailingObjects;

static CXXAddrspaceCastExpr *
Create(const ASTContext &Context, QualType T, ExprValueKind VK, CastKind Kind,
Expr *Op, TypeSourceInfo *WrittenTy, SourceLocation L,
SourceLocation RParenLoc, SourceRange AngleBrackets);
static CXXAddrspaceCastExpr *CreateEmpty(const ASTContext &Context);

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXAddrspaceCastExprClass;
}
};










class UserDefinedLiteral final : public CallExpr {
friend class ASTStmtReader;
friend class ASTStmtWriter;


SourceLocation UDSuffixLoc;




UserDefinedLiteral(Expr *Fn, ArrayRef<Expr *> Args, QualType Ty,
ExprValueKind VK, SourceLocation LitEndLoc,
SourceLocation SuffixLoc, FPOptionsOverride FPFeatures);

UserDefinedLiteral(unsigned NumArgs, bool HasFPFeatures, EmptyShell Empty);

public:
static UserDefinedLiteral *Create(const ASTContext &Ctx, Expr *Fn,
ArrayRef<Expr *> Args, QualType Ty,
ExprValueKind VK, SourceLocation LitEndLoc,
SourceLocation SuffixLoc,
FPOptionsOverride FPFeatures);

static UserDefinedLiteral *CreateEmpty(const ASTContext &Ctx,
unsigned NumArgs, bool HasFPOptions,
EmptyShell Empty);


enum LiteralOperatorKind {

LOK_Raw,


LOK_Template,


LOK_Integer,


LOK_Floating,


LOK_String,


LOK_Character
};



LiteralOperatorKind getLiteralOperatorKind() const;




Expr *getCookedLiteral();
const Expr *getCookedLiteral() const {
return const_cast<UserDefinedLiteral*>(this)->getCookedLiteral();
}

SourceLocation getBeginLoc() const {
if (getLiteralOperatorKind() == LOK_Template)
return getRParenLoc();
return getArg(0)->getBeginLoc();
}

SourceLocation getEndLoc() const { return getRParenLoc(); }





SourceLocation getUDSuffixLoc() const { return UDSuffixLoc; }


const IdentifierInfo *getUDSuffix() const;

static bool classof(const Stmt *S) {
return S->getStmtClass() == UserDefinedLiteralClass;
}
};


class CXXBoolLiteralExpr : public Expr {
public:
CXXBoolLiteralExpr(bool Val, QualType Ty, SourceLocation Loc)
: Expr(CXXBoolLiteralExprClass, Ty, VK_PRValue, OK_Ordinary) {
CXXBoolLiteralExprBits.Value = Val;
CXXBoolLiteralExprBits.Loc = Loc;
setDependence(ExprDependence::None);
}

explicit CXXBoolLiteralExpr(EmptyShell Empty)
: Expr(CXXBoolLiteralExprClass, Empty) {}

bool getValue() const { return CXXBoolLiteralExprBits.Value; }
void setValue(bool V) { CXXBoolLiteralExprBits.Value = V; }

SourceLocation getBeginLoc() const { return getLocation(); }
SourceLocation getEndLoc() const { return getLocation(); }

SourceLocation getLocation() const { return CXXBoolLiteralExprBits.Loc; }
void setLocation(SourceLocation L) { CXXBoolLiteralExprBits.Loc = L; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXBoolLiteralExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};




class CXXNullPtrLiteralExpr : public Expr {
public:
CXXNullPtrLiteralExpr(QualType Ty, SourceLocation Loc)
: Expr(CXXNullPtrLiteralExprClass, Ty, VK_PRValue, OK_Ordinary) {
CXXNullPtrLiteralExprBits.Loc = Loc;
setDependence(ExprDependence::None);
}

explicit CXXNullPtrLiteralExpr(EmptyShell Empty)
: Expr(CXXNullPtrLiteralExprClass, Empty) {}

SourceLocation getBeginLoc() const { return getLocation(); }
SourceLocation getEndLoc() const { return getLocation(); }

SourceLocation getLocation() const { return CXXNullPtrLiteralExprBits.Loc; }
void setLocation(SourceLocation L) { CXXNullPtrLiteralExprBits.Loc = L; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXNullPtrLiteralExprClass;
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};



class CXXStdInitializerListExpr : public Expr {
Stmt *SubExpr = nullptr;

CXXStdInitializerListExpr(EmptyShell Empty)
: Expr(CXXStdInitializerListExprClass, Empty) {}

public:
friend class ASTReader;
friend class ASTStmtReader;

CXXStdInitializerListExpr(QualType Ty, Expr *SubExpr)
: Expr(CXXStdInitializerListExprClass, Ty, VK_PRValue, OK_Ordinary),
SubExpr(SubExpr) {
setDependence(computeDependence(this));
}

Expr *getSubExpr() { return static_cast<Expr*>(SubExpr); }
const Expr *getSubExpr() const { return static_cast<const Expr*>(SubExpr); }

SourceLocation getBeginLoc() const LLVM_READONLY {
return SubExpr->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
return SubExpr->getEndLoc();
}


SourceRange getSourceRange() const LLVM_READONLY {
return SubExpr->getSourceRange();
}

static bool classof(const Stmt *S) {
return S->getStmtClass() == CXXStdInitializerListExprClass;
}

child_range children() { return child_range(&SubExpr, &SubExpr + 1); }

const_child_range children() const {
return const_child_range(&SubExpr, &SubExpr + 1);
}
};






class CXXTypeidExpr : public Expr {
friend class ASTStmtReader;

private:
llvm::PointerUnion<Stmt *, TypeSourceInfo *> Operand;
SourceRange Range;

public:
CXXTypeidExpr(QualType Ty, TypeSourceInfo *Operand, SourceRange R)
: Expr(CXXTypeidExprClass, Ty, VK_LValue, OK_Ordinary), Operand(Operand),
Range(R) {
setDependence(computeDependence(this));
}

CXXTypeidExpr(QualType Ty, Expr *Operand, SourceRange R)
: Expr(CXXTypeidExprClass, Ty, VK_LValue, OK_Ordinary), Operand(Operand),
Range(R) {
setDependence(computeDependence(this));
}

CXXTypeidExpr(EmptyShell Empty, bool isExpr)
: Expr(CXXTypeidExprClass, Empty) {
if (isExpr)
Operand = (Expr*)nullptr;
else
Operand = (TypeSourceInfo*)nullptr;
}



bool isPotentiallyEvaluated() const;



bool isMostDerived(ASTContext &Context) const;

bool isTypeOperand() const { return Operand.is<TypeSourceInfo *>(); }



QualType getTypeOperand(ASTContext &Context) const;


TypeSourceInfo *getTypeOperandSourceInfo() const {
assert(isTypeOperand() && "Cannot call getTypeOperand for typeid(expr)");
return Operand.get<TypeSourceInfo *>();
}
Expr *getExprOperand() const {
assert(!isTypeOperand() && "Cannot call getExprOperand for typeid(type)");
return static_cast<Expr*>(Operand.get<Stmt *>());
}

SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }
SourceRange getSourceRange() const LLVM_READONLY { return Range; }
void setSourceRange(SourceRange R) { Range = R; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXTypeidExprClass;
}


child_range children() {
if (isTypeOperand())
return child_range(child_iterator(), child_iterator());
auto **begin = reinterpret_cast<Stmt **>(&Operand);
return child_range(begin, begin + 1);
}

const_child_range children() const {
if (isTypeOperand())
return const_child_range(const_child_iterator(), const_child_iterator());

auto **begin =
reinterpret_cast<Stmt **>(&const_cast<CXXTypeidExpr *>(this)->Operand);
return const_child_range(begin, begin + 1);
}
};






class MSPropertyRefExpr : public Expr {
Expr *BaseExpr;
MSPropertyDecl *TheDecl;
SourceLocation MemberLoc;
bool IsArrow;
NestedNameSpecifierLoc QualifierLoc;

public:
friend class ASTStmtReader;

MSPropertyRefExpr(Expr *baseExpr, MSPropertyDecl *decl, bool isArrow,
QualType ty, ExprValueKind VK,
NestedNameSpecifierLoc qualifierLoc, SourceLocation nameLoc)
: Expr(MSPropertyRefExprClass, ty, VK, OK_Ordinary), BaseExpr(baseExpr),
TheDecl(decl), MemberLoc(nameLoc), IsArrow(isArrow),
QualifierLoc(qualifierLoc) {
setDependence(computeDependence(this));
}

MSPropertyRefExpr(EmptyShell Empty) : Expr(MSPropertyRefExprClass, Empty) {}

SourceRange getSourceRange() const LLVM_READONLY {
return SourceRange(getBeginLoc(), getEndLoc());
}

bool isImplicitAccess() const {
return getBaseExpr() && getBaseExpr()->isImplicitCXXThis();
}

SourceLocation getBeginLoc() const {
if (!isImplicitAccess())
return BaseExpr->getBeginLoc();
else if (QualifierLoc)
return QualifierLoc.getBeginLoc();
else
return MemberLoc;
}

SourceLocation getEndLoc() const { return getMemberLoc(); }

child_range children() {
return child_range((Stmt**)&BaseExpr, (Stmt**)&BaseExpr + 1);
}

const_child_range children() const {
auto Children = const_cast<MSPropertyRefExpr *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == MSPropertyRefExprClass;
}

Expr *getBaseExpr() const { return BaseExpr; }
MSPropertyDecl *getPropertyDecl() const { return TheDecl; }
bool isArrow() const { return IsArrow; }
SourceLocation getMemberLoc() const { return MemberLoc; }
NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }
};












class MSPropertySubscriptExpr : public Expr {
friend class ASTStmtReader;

enum { BASE_EXPR, IDX_EXPR, NUM_SUBEXPRS = 2 };

Stmt *SubExprs[NUM_SUBEXPRS];
SourceLocation RBracketLoc;

void setBase(Expr *Base) { SubExprs[BASE_EXPR] = Base; }
void setIdx(Expr *Idx) { SubExprs[IDX_EXPR] = Idx; }

public:
MSPropertySubscriptExpr(Expr *Base, Expr *Idx, QualType Ty, ExprValueKind VK,
ExprObjectKind OK, SourceLocation RBracketLoc)
: Expr(MSPropertySubscriptExprClass, Ty, VK, OK),
RBracketLoc(RBracketLoc) {
SubExprs[BASE_EXPR] = Base;
SubExprs[IDX_EXPR] = Idx;
setDependence(computeDependence(this));
}


explicit MSPropertySubscriptExpr(EmptyShell Shell)
: Expr(MSPropertySubscriptExprClass, Shell) {}

Expr *getBase() { return cast<Expr>(SubExprs[BASE_EXPR]); }
const Expr *getBase() const { return cast<Expr>(SubExprs[BASE_EXPR]); }

Expr *getIdx() { return cast<Expr>(SubExprs[IDX_EXPR]); }
const Expr *getIdx() const { return cast<Expr>(SubExprs[IDX_EXPR]); }

SourceLocation getBeginLoc() const LLVM_READONLY {
return getBase()->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY { return RBracketLoc; }

SourceLocation getRBracketLoc() const { return RBracketLoc; }
void setRBracketLoc(SourceLocation L) { RBracketLoc = L; }

SourceLocation getExprLoc() const LLVM_READONLY {
return getBase()->getExprLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == MSPropertySubscriptExprClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0] + NUM_SUBEXPRS);
}

const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + NUM_SUBEXPRS);
}
};





class CXXUuidofExpr : public Expr {
friend class ASTStmtReader;

private:
llvm::PointerUnion<Stmt *, TypeSourceInfo *> Operand;
MSGuidDecl *Guid;
SourceRange Range;

public:
CXXUuidofExpr(QualType Ty, TypeSourceInfo *Operand, MSGuidDecl *Guid,
SourceRange R)
: Expr(CXXUuidofExprClass, Ty, VK_LValue, OK_Ordinary), Operand(Operand),
Guid(Guid), Range(R) {
setDependence(computeDependence(this));
}

CXXUuidofExpr(QualType Ty, Expr *Operand, MSGuidDecl *Guid, SourceRange R)
: Expr(CXXUuidofExprClass, Ty, VK_LValue, OK_Ordinary), Operand(Operand),
Guid(Guid), Range(R) {
setDependence(computeDependence(this));
}

CXXUuidofExpr(EmptyShell Empty, bool isExpr)
: Expr(CXXUuidofExprClass, Empty) {
if (isExpr)
Operand = (Expr*)nullptr;
else
Operand = (TypeSourceInfo*)nullptr;
}

bool isTypeOperand() const { return Operand.is<TypeSourceInfo *>(); }



QualType getTypeOperand(ASTContext &Context) const;


TypeSourceInfo *getTypeOperandSourceInfo() const {
assert(isTypeOperand() && "Cannot call getTypeOperand for __uuidof(expr)");
return Operand.get<TypeSourceInfo *>();
}
Expr *getExprOperand() const {
assert(!isTypeOperand() && "Cannot call getExprOperand for __uuidof(type)");
return static_cast<Expr*>(Operand.get<Stmt *>());
}

MSGuidDecl *getGuidDecl() const { return Guid; }

SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }
SourceRange getSourceRange() const LLVM_READONLY { return Range; }
void setSourceRange(SourceRange R) { Range = R; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXUuidofExprClass;
}


child_range children() {
if (isTypeOperand())
return child_range(child_iterator(), child_iterator());
auto **begin = reinterpret_cast<Stmt **>(&Operand);
return child_range(begin, begin + 1);
}

const_child_range children() const {
if (isTypeOperand())
return const_child_range(const_child_iterator(), const_child_iterator());
auto **begin =
reinterpret_cast<Stmt **>(&const_cast<CXXUuidofExpr *>(this)->Operand);
return const_child_range(begin, begin + 1);
}
};













class CXXThisExpr : public Expr {
public:
CXXThisExpr(SourceLocation L, QualType Ty, bool IsImplicit)
: Expr(CXXThisExprClass, Ty, VK_PRValue, OK_Ordinary) {
CXXThisExprBits.IsImplicit = IsImplicit;
CXXThisExprBits.Loc = L;
setDependence(computeDependence(this));
}

CXXThisExpr(EmptyShell Empty) : Expr(CXXThisExprClass, Empty) {}

SourceLocation getLocation() const { return CXXThisExprBits.Loc; }
void setLocation(SourceLocation L) { CXXThisExprBits.Loc = L; }

SourceLocation getBeginLoc() const { return getLocation(); }
SourceLocation getEndLoc() const { return getLocation(); }

bool isImplicit() const { return CXXThisExprBits.IsImplicit; }
void setImplicit(bool I) { CXXThisExprBits.IsImplicit = I; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXThisExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};






class CXXThrowExpr : public Expr {
friend class ASTStmtReader;


Stmt *Operand;

public:




CXXThrowExpr(Expr *Operand, QualType Ty, SourceLocation Loc,
bool IsThrownVariableInScope)
: Expr(CXXThrowExprClass, Ty, VK_PRValue, OK_Ordinary), Operand(Operand) {
CXXThrowExprBits.ThrowLoc = Loc;
CXXThrowExprBits.IsThrownVariableInScope = IsThrownVariableInScope;
setDependence(computeDependence(this));
}
CXXThrowExpr(EmptyShell Empty) : Expr(CXXThrowExprClass, Empty) {}

const Expr *getSubExpr() const { return cast_or_null<Expr>(Operand); }
Expr *getSubExpr() { return cast_or_null<Expr>(Operand); }

SourceLocation getThrowLoc() const { return CXXThrowExprBits.ThrowLoc; }






bool isThrownVariableInScope() const {
return CXXThrowExprBits.IsThrownVariableInScope;
}

SourceLocation getBeginLoc() const { return getThrowLoc(); }
SourceLocation getEndLoc() const LLVM_READONLY {
if (!getSubExpr())
return getThrowLoc();
return getSubExpr()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXThrowExprClass;
}


child_range children() {
return child_range(&Operand, Operand ? &Operand + 1 : &Operand);
}

const_child_range children() const {
return const_child_range(&Operand, Operand ? &Operand + 1 : &Operand);
}
};






class CXXDefaultArgExpr final : public Expr {
friend class ASTStmtReader;


ParmVarDecl *Param;


DeclContext *UsedContext;

CXXDefaultArgExpr(StmtClass SC, SourceLocation Loc, ParmVarDecl *Param,
DeclContext *UsedContext)
: Expr(SC,
Param->hasUnparsedDefaultArg()
? Param->getType().getNonReferenceType()
: Param->getDefaultArg()->getType(),
Param->getDefaultArg()->getValueKind(),
Param->getDefaultArg()->getObjectKind()),
Param(Param), UsedContext(UsedContext) {
CXXDefaultArgExprBits.Loc = Loc;
setDependence(computeDependence(this));
}

public:
CXXDefaultArgExpr(EmptyShell Empty) : Expr(CXXDefaultArgExprClass, Empty) {}



static CXXDefaultArgExpr *Create(const ASTContext &C, SourceLocation Loc,
ParmVarDecl *Param,
DeclContext *UsedContext) {
return new (C)
CXXDefaultArgExpr(CXXDefaultArgExprClass, Loc, Param, UsedContext);
}


const ParmVarDecl *getParam() const { return Param; }
ParmVarDecl *getParam() { return Param; }


const Expr *getExpr() const { return getParam()->getDefaultArg(); }
Expr *getExpr() { return getParam()->getDefaultArg(); }

const DeclContext *getUsedContext() const { return UsedContext; }
DeclContext *getUsedContext() { return UsedContext; }


SourceLocation getUsedLocation() const { return CXXDefaultArgExprBits.Loc; }



SourceLocation getBeginLoc() const { return SourceLocation(); }
SourceLocation getEndLoc() const { return SourceLocation(); }

SourceLocation getExprLoc() const { return getUsedLocation(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXDefaultArgExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};









class CXXDefaultInitExpr : public Expr {
friend class ASTReader;
friend class ASTStmtReader;


FieldDecl *Field;


DeclContext *UsedContext;

CXXDefaultInitExpr(const ASTContext &Ctx, SourceLocation Loc,
FieldDecl *Field, QualType Ty, DeclContext *UsedContext);

CXXDefaultInitExpr(EmptyShell Empty) : Expr(CXXDefaultInitExprClass, Empty) {}

public:


static CXXDefaultInitExpr *Create(const ASTContext &Ctx, SourceLocation Loc,
FieldDecl *Field, DeclContext *UsedContext) {
return new (Ctx) CXXDefaultInitExpr(Ctx, Loc, Field, Field->getType(), UsedContext);
}


FieldDecl *getField() { return Field; }
const FieldDecl *getField() const { return Field; }


const Expr *getExpr() const {
assert(Field->getInClassInitializer() && "initializer hasn't been parsed");
return Field->getInClassInitializer();
}
Expr *getExpr() {
assert(Field->getInClassInitializer() && "initializer hasn't been parsed");
return Field->getInClassInitializer();
}

const DeclContext *getUsedContext() const { return UsedContext; }
DeclContext *getUsedContext() { return UsedContext; }



SourceLocation getUsedLocation() const { return getBeginLoc(); }

SourceLocation getBeginLoc() const { return CXXDefaultInitExprBits.Loc; }
SourceLocation getEndLoc() const { return CXXDefaultInitExprBits.Loc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXDefaultInitExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};


class CXXTemporary {

const CXXDestructorDecl *Destructor;

explicit CXXTemporary(const CXXDestructorDecl *destructor)
: Destructor(destructor) {}

public:
static CXXTemporary *Create(const ASTContext &C,
const CXXDestructorDecl *Destructor);

const CXXDestructorDecl *getDestructor() const { return Destructor; }

void setDestructor(const CXXDestructorDecl *Dtor) {
Destructor = Dtor;
}
};















class CXXBindTemporaryExpr : public Expr {
CXXTemporary *Temp = nullptr;
Stmt *SubExpr = nullptr;

CXXBindTemporaryExpr(CXXTemporary *temp, Expr *SubExpr)
: Expr(CXXBindTemporaryExprClass, SubExpr->getType(), VK_PRValue,
OK_Ordinary),
Temp(temp), SubExpr(SubExpr) {
setDependence(computeDependence(this));
}

public:
CXXBindTemporaryExpr(EmptyShell Empty)
: Expr(CXXBindTemporaryExprClass, Empty) {}

static CXXBindTemporaryExpr *Create(const ASTContext &C, CXXTemporary *Temp,
Expr* SubExpr);

CXXTemporary *getTemporary() { return Temp; }
const CXXTemporary *getTemporary() const { return Temp; }
void setTemporary(CXXTemporary *T) { Temp = T; }

const Expr *getSubExpr() const { return cast<Expr>(SubExpr); }
Expr *getSubExpr() { return cast<Expr>(SubExpr); }
void setSubExpr(Expr *E) { SubExpr = E; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return SubExpr->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
return SubExpr->getEndLoc();
}


static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXBindTemporaryExprClass;
}


child_range children() { return child_range(&SubExpr, &SubExpr + 1); }

const_child_range children() const {
return const_child_range(&SubExpr, &SubExpr + 1);
}
};


class CXXConstructExpr : public Expr {
friend class ASTStmtReader;

public:
enum ConstructionKind {
CK_Complete,
CK_NonVirtualBase,
CK_VirtualBase,
CK_Delegating
};

private:

CXXConstructorDecl *Constructor;

SourceRange ParenOrBraceRange;


unsigned NumArgs;

















inline Stmt **getTrailingArgs();
const Stmt *const *getTrailingArgs() const {
return const_cast<CXXConstructExpr *>(this)->getTrailingArgs();
}

protected:

CXXConstructExpr(StmtClass SC, QualType Ty, SourceLocation Loc,
CXXConstructorDecl *Ctor, bool Elidable,
ArrayRef<Expr *> Args, bool HadMultipleCandidates,
bool ListInitialization, bool StdInitListInitialization,
bool ZeroInitialization, ConstructionKind ConstructKind,
SourceRange ParenOrBraceRange);


CXXConstructExpr(StmtClass SC, EmptyShell Empty, unsigned NumArgs);



static unsigned sizeOfTrailingObjects(unsigned NumArgs) {
return NumArgs * sizeof(Stmt *);
}

public:

static CXXConstructExpr *
Create(const ASTContext &Ctx, QualType Ty, SourceLocation Loc,
CXXConstructorDecl *Ctor, bool Elidable, ArrayRef<Expr *> Args,
bool HadMultipleCandidates, bool ListInitialization,
bool StdInitListInitialization, bool ZeroInitialization,
ConstructionKind ConstructKind, SourceRange ParenOrBraceRange);


static CXXConstructExpr *CreateEmpty(const ASTContext &Ctx, unsigned NumArgs);


CXXConstructorDecl *getConstructor() const { return Constructor; }

SourceLocation getLocation() const { return CXXConstructExprBits.Loc; }
void setLocation(SourceLocation Loc) { CXXConstructExprBits.Loc = Loc; }


bool isElidable() const { return CXXConstructExprBits.Elidable; }
void setElidable(bool E) { CXXConstructExprBits.Elidable = E; }



bool hadMultipleCandidates() const {
return CXXConstructExprBits.HadMultipleCandidates;
}
void setHadMultipleCandidates(bool V) {
CXXConstructExprBits.HadMultipleCandidates = V;
}


bool isListInitialization() const {
return CXXConstructExprBits.ListInitialization;
}
void setListInitialization(bool V) {
CXXConstructExprBits.ListInitialization = V;
}





bool isStdInitListInitialization() const {
return CXXConstructExprBits.StdInitListInitialization;
}
void setStdInitListInitialization(bool V) {
CXXConstructExprBits.StdInitListInitialization = V;
}



bool requiresZeroInitialization() const {
return CXXConstructExprBits.ZeroInitialization;
}
void setRequiresZeroInitialization(bool ZeroInit) {
CXXConstructExprBits.ZeroInitialization = ZeroInit;
}



ConstructionKind getConstructionKind() const {
return static_cast<ConstructionKind>(CXXConstructExprBits.ConstructionKind);
}
void setConstructionKind(ConstructionKind CK) {
CXXConstructExprBits.ConstructionKind = CK;
}

using arg_iterator = ExprIterator;
using const_arg_iterator = ConstExprIterator;
using arg_range = llvm::iterator_range<arg_iterator>;
using const_arg_range = llvm::iterator_range<const_arg_iterator>;

arg_range arguments() { return arg_range(arg_begin(), arg_end()); }
const_arg_range arguments() const {
return const_arg_range(arg_begin(), arg_end());
}

arg_iterator arg_begin() { return getTrailingArgs(); }
arg_iterator arg_end() { return arg_begin() + getNumArgs(); }
const_arg_iterator arg_begin() const { return getTrailingArgs(); }
const_arg_iterator arg_end() const { return arg_begin() + getNumArgs(); }

Expr **getArgs() { return reinterpret_cast<Expr **>(getTrailingArgs()); }
const Expr *const *getArgs() const {
return reinterpret_cast<const Expr *const *>(getTrailingArgs());
}


unsigned getNumArgs() const { return NumArgs; }


Expr *getArg(unsigned Arg) {
assert(Arg < getNumArgs() && "Arg access out of range!");
return getArgs()[Arg];
}
const Expr *getArg(unsigned Arg) const {
assert(Arg < getNumArgs() && "Arg access out of range!");
return getArgs()[Arg];
}


void setArg(unsigned Arg, Expr *ArgExpr) {
assert(Arg < getNumArgs() && "Arg access out of range!");
getArgs()[Arg] = ArgExpr;
}

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;
SourceRange getParenOrBraceRange() const { return ParenOrBraceRange; }
void setParenOrBraceRange(SourceRange Range) { ParenOrBraceRange = Range; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXConstructExprClass ||
T->getStmtClass() == CXXTemporaryObjectExprClass;
}


child_range children() {
return child_range(getTrailingArgs(), getTrailingArgs() + getNumArgs());
}

const_child_range children() const {
auto Children = const_cast<CXXConstructExpr *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}
};





class CXXInheritedCtorInitExpr : public Expr {
private:
CXXConstructorDecl *Constructor = nullptr;


SourceLocation Loc;


unsigned ConstructsVirtualBase : 1;



unsigned InheritedFromVirtualBase : 1;

public:
friend class ASTStmtReader;


CXXInheritedCtorInitExpr(SourceLocation Loc, QualType T,
CXXConstructorDecl *Ctor, bool ConstructsVirtualBase,
bool InheritedFromVirtualBase)
: Expr(CXXInheritedCtorInitExprClass, T, VK_PRValue, OK_Ordinary),
Constructor(Ctor), Loc(Loc),
ConstructsVirtualBase(ConstructsVirtualBase),
InheritedFromVirtualBase(InheritedFromVirtualBase) {
assert(!T->isDependentType());
setDependence(ExprDependence::None);
}


explicit CXXInheritedCtorInitExpr(EmptyShell Empty)
: Expr(CXXInheritedCtorInitExprClass, Empty),
ConstructsVirtualBase(false), InheritedFromVirtualBase(false) {}


CXXConstructorDecl *getConstructor() const { return Constructor; }



bool constructsVBase() const { return ConstructsVirtualBase; }
CXXConstructExpr::ConstructionKind getConstructionKind() const {
return ConstructsVirtualBase ? CXXConstructExpr::CK_VirtualBase
: CXXConstructExpr::CK_NonVirtualBase;
}





bool inheritedFromVBase() const { return InheritedFromVirtualBase; }

SourceLocation getLocation() const LLVM_READONLY { return Loc; }
SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return Loc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXInheritedCtorInitExprClass;
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};








class CXXFunctionalCastExpr final
: public ExplicitCastExpr,
private llvm::TrailingObjects<CXXFunctionalCastExpr, CXXBaseSpecifier *,
FPOptionsOverride> {
SourceLocation LParenLoc;
SourceLocation RParenLoc;

CXXFunctionalCastExpr(QualType ty, ExprValueKind VK,
TypeSourceInfo *writtenTy, CastKind kind,
Expr *castExpr, unsigned pathSize,
FPOptionsOverride FPO, SourceLocation lParenLoc,
SourceLocation rParenLoc)
: ExplicitCastExpr(CXXFunctionalCastExprClass, ty, VK, kind, castExpr,
pathSize, FPO.requiresTrailingStorage(), writtenTy),
LParenLoc(lParenLoc), RParenLoc(rParenLoc) {
if (hasStoredFPFeatures())
*getTrailingFPFeatures() = FPO;
}

explicit CXXFunctionalCastExpr(EmptyShell Shell, unsigned PathSize,
bool HasFPFeatures)
: ExplicitCastExpr(CXXFunctionalCastExprClass, Shell, PathSize,
HasFPFeatures) {}

unsigned numTrailingObjects(OverloadToken<CXXBaseSpecifier *>) const {
return path_size();
}

public:
friend class CastExpr;
friend TrailingObjects;

static CXXFunctionalCastExpr *
Create(const ASTContext &Context, QualType T, ExprValueKind VK,
TypeSourceInfo *Written, CastKind Kind, Expr *Op,
const CXXCastPath *Path, FPOptionsOverride FPO, SourceLocation LPLoc,
SourceLocation RPLoc);
static CXXFunctionalCastExpr *
CreateEmpty(const ASTContext &Context, unsigned PathSize, bool HasFPFeatures);

SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation L) { LParenLoc = L; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }


bool isListInitialization() const { return LParenLoc.isInvalid(); }

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXFunctionalCastExprClass;
}
};
















class CXXTemporaryObjectExpr final : public CXXConstructExpr {
friend class ASTStmtReader;





TypeSourceInfo *TSI;

CXXTemporaryObjectExpr(CXXConstructorDecl *Cons, QualType Ty,
TypeSourceInfo *TSI, ArrayRef<Expr *> Args,
SourceRange ParenOrBraceRange,
bool HadMultipleCandidates, bool ListInitialization,
bool StdInitListInitialization,
bool ZeroInitialization);

CXXTemporaryObjectExpr(EmptyShell Empty, unsigned NumArgs);

public:
static CXXTemporaryObjectExpr *
Create(const ASTContext &Ctx, CXXConstructorDecl *Cons, QualType Ty,
TypeSourceInfo *TSI, ArrayRef<Expr *> Args,
SourceRange ParenOrBraceRange, bool HadMultipleCandidates,
bool ListInitialization, bool StdInitListInitialization,
bool ZeroInitialization);

static CXXTemporaryObjectExpr *CreateEmpty(const ASTContext &Ctx,
unsigned NumArgs);

TypeSourceInfo *getTypeSourceInfo() const { return TSI; }

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXTemporaryObjectExprClass;
}
};

Stmt **CXXConstructExpr::getTrailingArgs() {
if (auto *E = dyn_cast<CXXTemporaryObjectExpr>(this))
return reinterpret_cast<Stmt **>(E + 1);
assert((getStmtClass() == CXXConstructExprClass) &&
"Unexpected class deriving from CXXConstructExpr!");
return reinterpret_cast<Stmt **>(this + 1);
}






















class LambdaExpr final : public Expr,
private llvm::TrailingObjects<LambdaExpr, Stmt *> {



SourceRange IntroducerRange;


SourceLocation CaptureDefaultLoc;









SourceLocation ClosingBrace;


LambdaExpr(QualType T, SourceRange IntroducerRange,
LambdaCaptureDefault CaptureDefault,
SourceLocation CaptureDefaultLoc, bool ExplicitParams,
bool ExplicitResultType, ArrayRef<Expr *> CaptureInits,
SourceLocation ClosingBrace, bool ContainsUnexpandedParameterPack);


LambdaExpr(EmptyShell Empty, unsigned NumCaptures);

Stmt **getStoredStmts() { return getTrailingObjects<Stmt *>(); }
Stmt *const *getStoredStmts() const { return getTrailingObjects<Stmt *>(); }

void initBodyIfNeeded() const;

public:
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;


static LambdaExpr *
Create(const ASTContext &C, CXXRecordDecl *Class, SourceRange IntroducerRange,
LambdaCaptureDefault CaptureDefault, SourceLocation CaptureDefaultLoc,
bool ExplicitParams, bool ExplicitResultType,
ArrayRef<Expr *> CaptureInits, SourceLocation ClosingBrace,
bool ContainsUnexpandedParameterPack);



static LambdaExpr *CreateDeserialized(const ASTContext &C,
unsigned NumCaptures);


LambdaCaptureDefault getCaptureDefault() const {
return static_cast<LambdaCaptureDefault>(LambdaExprBits.CaptureDefault);
}


SourceLocation getCaptureDefaultLoc() const { return CaptureDefaultLoc; }


bool isInitCapture(const LambdaCapture *Capture) const;



using capture_iterator = const LambdaCapture *;


using capture_range = llvm::iterator_range<capture_iterator>;


capture_range captures() const;


capture_iterator capture_begin() const;



capture_iterator capture_end() const;


unsigned capture_size() const { return LambdaExprBits.NumCaptures; }


capture_range explicit_captures() const;



capture_iterator explicit_capture_begin() const;



capture_iterator explicit_capture_end() const;


capture_range implicit_captures() const;



capture_iterator implicit_capture_begin() const;



capture_iterator implicit_capture_end() const;



using capture_init_iterator = Expr **;




using const_capture_init_iterator = Expr *const *;


llvm::iterator_range<capture_init_iterator> capture_inits() {
return llvm::make_range(capture_init_begin(), capture_init_end());
}


llvm::iterator_range<const_capture_init_iterator> capture_inits() const {
return llvm::make_range(capture_init_begin(), capture_init_end());
}



capture_init_iterator capture_init_begin() {
return reinterpret_cast<Expr **>(getStoredStmts());
}



const_capture_init_iterator capture_init_begin() const {
return reinterpret_cast<Expr *const *>(getStoredStmts());
}



capture_init_iterator capture_init_end() {
return capture_init_begin() + capture_size();
}



const_capture_init_iterator capture_init_end() const {
return capture_init_begin() + capture_size();
}




SourceRange getIntroducerRange() const { return IntroducerRange; }






CXXRecordDecl *getLambdaClass() const;



CXXMethodDecl *getCallOperator() const;



FunctionTemplateDecl *getDependentCallOperator() const;



TemplateParameterList *getTemplateParameterList() const;



ArrayRef<NamedDecl *> getExplicitTemplateParameters() const;


Expr *getTrailingRequiresClause() const;


bool isGenericLambda() const { return getTemplateParameterList(); }





Stmt *getBody() const;




const CompoundStmt *getCompoundStmtBody() const;
CompoundStmt *getCompoundStmtBody() {
const auto *ConstThis = this;
return const_cast<CompoundStmt *>(ConstThis->getCompoundStmtBody());
}



bool isMutable() const;



bool hasExplicitParameters() const { return LambdaExprBits.ExplicitParams; }


bool hasExplicitResultType() const {
return LambdaExprBits.ExplicitResultType;
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == LambdaExprClass;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return IntroducerRange.getBegin();
}

SourceLocation getEndLoc() const LLVM_READONLY { return ClosingBrace; }


child_range children();
const_child_range children() const;
};



class CXXScalarValueInitExpr : public Expr {
friend class ASTStmtReader;

TypeSourceInfo *TypeInfo;

public:


CXXScalarValueInitExpr(QualType Type, TypeSourceInfo *TypeInfo,
SourceLocation RParenLoc)
: Expr(CXXScalarValueInitExprClass, Type, VK_PRValue, OK_Ordinary),
TypeInfo(TypeInfo) {
CXXScalarValueInitExprBits.RParenLoc = RParenLoc;
setDependence(computeDependence(this));
}

explicit CXXScalarValueInitExpr(EmptyShell Shell)
: Expr(CXXScalarValueInitExprClass, Shell) {}

TypeSourceInfo *getTypeSourceInfo() const {
return TypeInfo;
}

SourceLocation getRParenLoc() const {
return CXXScalarValueInitExprBits.RParenLoc;
}

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const { return getRParenLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXScalarValueInitExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};



class CXXNewExpr final
: public Expr,
private llvm::TrailingObjects<CXXNewExpr, Stmt *, SourceRange> {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;


FunctionDecl *OperatorNew;


FunctionDecl *OperatorDelete;


TypeSourceInfo *AllocatedTypeInfo;


SourceRange Range;


SourceRange DirectInitRange;
















unsigned arraySizeOffset() const { return 0; }
unsigned initExprOffset() const { return arraySizeOffset() + isArray(); }
unsigned placementNewArgsOffset() const {
return initExprOffset() + hasInitializer();
}

unsigned numTrailingObjects(OverloadToken<Stmt *>) const {
return isArray() + hasInitializer() + getNumPlacementArgs();
}

unsigned numTrailingObjects(OverloadToken<SourceRange>) const {
return isParenTypeId();
}

public:
enum InitializationStyle {

NoInit,


CallInit,


ListInit
};

private:

CXXNewExpr(bool IsGlobalNew, FunctionDecl *OperatorNew,
FunctionDecl *OperatorDelete, bool ShouldPassAlignment,
bool UsualArrayDeleteWantsSize, ArrayRef<Expr *> PlacementArgs,
SourceRange TypeIdParens, Optional<Expr *> ArraySize,
InitializationStyle InitializationStyle, Expr *Initializer,
QualType Ty, TypeSourceInfo *AllocatedTypeInfo, SourceRange Range,
SourceRange DirectInitRange);


CXXNewExpr(EmptyShell Empty, bool IsArray, unsigned NumPlacementArgs,
bool IsParenTypeId);

public:

static CXXNewExpr *
Create(const ASTContext &Ctx, bool IsGlobalNew, FunctionDecl *OperatorNew,
FunctionDecl *OperatorDelete, bool ShouldPassAlignment,
bool UsualArrayDeleteWantsSize, ArrayRef<Expr *> PlacementArgs,
SourceRange TypeIdParens, Optional<Expr *> ArraySize,
InitializationStyle InitializationStyle, Expr *Initializer,
QualType Ty, TypeSourceInfo *AllocatedTypeInfo, SourceRange Range,
SourceRange DirectInitRange);


static CXXNewExpr *CreateEmpty(const ASTContext &Ctx, bool IsArray,
bool HasInit, unsigned NumPlacementArgs,
bool IsParenTypeId);

QualType getAllocatedType() const {
return getType()->castAs<PointerType>()->getPointeeType();
}

TypeSourceInfo *getAllocatedTypeSourceInfo() const {
return AllocatedTypeInfo;
}
















bool shouldNullCheckAllocation() const;

FunctionDecl *getOperatorNew() const { return OperatorNew; }
void setOperatorNew(FunctionDecl *D) { OperatorNew = D; }
FunctionDecl *getOperatorDelete() const { return OperatorDelete; }
void setOperatorDelete(FunctionDecl *D) { OperatorDelete = D; }

bool isArray() const { return CXXNewExprBits.IsArray; }

Optional<Expr *> getArraySize() {
if (!isArray())
return None;
return cast_or_null<Expr>(getTrailingObjects<Stmt *>()[arraySizeOffset()]);
}
Optional<const Expr *> getArraySize() const {
if (!isArray())
return None;
return cast_or_null<Expr>(getTrailingObjects<Stmt *>()[arraySizeOffset()]);
}

unsigned getNumPlacementArgs() const {
return CXXNewExprBits.NumPlacementArgs;
}

Expr **getPlacementArgs() {
return reinterpret_cast<Expr **>(getTrailingObjects<Stmt *>() +
placementNewArgsOffset());
}

Expr *getPlacementArg(unsigned I) {
assert((I < getNumPlacementArgs()) && "Index out of range!");
return getPlacementArgs()[I];
}
const Expr *getPlacementArg(unsigned I) const {
return const_cast<CXXNewExpr *>(this)->getPlacementArg(I);
}

bool isParenTypeId() const { return CXXNewExprBits.IsParenTypeId; }
SourceRange getTypeIdParens() const {
return isParenTypeId() ? getTrailingObjects<SourceRange>()[0]
: SourceRange();
}

bool isGlobalNew() const { return CXXNewExprBits.IsGlobalNew; }


bool hasInitializer() const {
return CXXNewExprBits.StoredInitializationStyle > 0;
}


InitializationStyle getInitializationStyle() const {
if (CXXNewExprBits.StoredInitializationStyle == 0)
return NoInit;
return static_cast<InitializationStyle>(
CXXNewExprBits.StoredInitializationStyle - 1);
}


Expr *getInitializer() {
return hasInitializer()
? cast<Expr>(getTrailingObjects<Stmt *>()[initExprOffset()])
: nullptr;
}
const Expr *getInitializer() const {
return hasInitializer()
? cast<Expr>(getTrailingObjects<Stmt *>()[initExprOffset()])
: nullptr;
}


const CXXConstructExpr *getConstructExpr() const {
return dyn_cast_or_null<CXXConstructExpr>(getInitializer());
}



bool passAlignment() const { return CXXNewExprBits.ShouldPassAlignment; }




bool doesUsualArrayDeleteWantSize() const {
return CXXNewExprBits.UsualArrayDeleteWantsSize;
}

using arg_iterator = ExprIterator;
using const_arg_iterator = ConstExprIterator;

llvm::iterator_range<arg_iterator> placement_arguments() {
return llvm::make_range(placement_arg_begin(), placement_arg_end());
}

llvm::iterator_range<const_arg_iterator> placement_arguments() const {
return llvm::make_range(placement_arg_begin(), placement_arg_end());
}

arg_iterator placement_arg_begin() {
return getTrailingObjects<Stmt *>() + placementNewArgsOffset();
}
arg_iterator placement_arg_end() {
return placement_arg_begin() + getNumPlacementArgs();
}
const_arg_iterator placement_arg_begin() const {
return getTrailingObjects<Stmt *>() + placementNewArgsOffset();
}
const_arg_iterator placement_arg_end() const {
return placement_arg_begin() + getNumPlacementArgs();
}

using raw_arg_iterator = Stmt **;

raw_arg_iterator raw_arg_begin() { return getTrailingObjects<Stmt *>(); }
raw_arg_iterator raw_arg_end() {
return raw_arg_begin() + numTrailingObjects(OverloadToken<Stmt *>());
}
const_arg_iterator raw_arg_begin() const {
return getTrailingObjects<Stmt *>();
}
const_arg_iterator raw_arg_end() const {
return raw_arg_begin() + numTrailingObjects(OverloadToken<Stmt *>());
}

SourceLocation getBeginLoc() const { return Range.getBegin(); }
SourceLocation getEndLoc() const { return Range.getEnd(); }

SourceRange getDirectInitRange() const { return DirectInitRange; }
SourceRange getSourceRange() const { return Range; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXNewExprClass;
}


child_range children() { return child_range(raw_arg_begin(), raw_arg_end()); }

const_child_range children() const {
return const_child_range(const_cast<CXXNewExpr *>(this)->children());
}
};



class CXXDeleteExpr : public Expr {
friend class ASTStmtReader;


FunctionDecl *OperatorDelete = nullptr;


Stmt *Argument = nullptr;

public:
CXXDeleteExpr(QualType Ty, bool GlobalDelete, bool ArrayForm,
bool ArrayFormAsWritten, bool UsualArrayDeleteWantsSize,
FunctionDecl *OperatorDelete, Expr *Arg, SourceLocation Loc)
: Expr(CXXDeleteExprClass, Ty, VK_PRValue, OK_Ordinary),
OperatorDelete(OperatorDelete), Argument(Arg) {
CXXDeleteExprBits.GlobalDelete = GlobalDelete;
CXXDeleteExprBits.ArrayForm = ArrayForm;
CXXDeleteExprBits.ArrayFormAsWritten = ArrayFormAsWritten;
CXXDeleteExprBits.UsualArrayDeleteWantsSize = UsualArrayDeleteWantsSize;
CXXDeleteExprBits.Loc = Loc;
setDependence(computeDependence(this));
}

explicit CXXDeleteExpr(EmptyShell Shell) : Expr(CXXDeleteExprClass, Shell) {}

bool isGlobalDelete() const { return CXXDeleteExprBits.GlobalDelete; }
bool isArrayForm() const { return CXXDeleteExprBits.ArrayForm; }
bool isArrayFormAsWritten() const {
return CXXDeleteExprBits.ArrayFormAsWritten;
}





bool doesUsualArrayDeleteWantSize() const {
return CXXDeleteExprBits.UsualArrayDeleteWantsSize;
}

FunctionDecl *getOperatorDelete() const { return OperatorDelete; }

Expr *getArgument() { return cast<Expr>(Argument); }
const Expr *getArgument() const { return cast<Expr>(Argument); }





QualType getDestroyedType() const;

SourceLocation getBeginLoc() const { return CXXDeleteExprBits.Loc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return Argument->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXDeleteExprClass;
}


child_range children() { return child_range(&Argument, &Argument + 1); }

const_child_range children() const {
return const_child_range(&Argument, &Argument + 1);
}
};


class PseudoDestructorTypeStorage {


llvm::PointerUnion<TypeSourceInfo *, IdentifierInfo *> Type;


SourceLocation Location;

public:
PseudoDestructorTypeStorage() = default;

PseudoDestructorTypeStorage(IdentifierInfo *II, SourceLocation Loc)
: Type(II), Location(Loc) {}

PseudoDestructorTypeStorage(TypeSourceInfo *Info);

TypeSourceInfo *getTypeSourceInfo() const {
return Type.dyn_cast<TypeSourceInfo *>();
}

IdentifierInfo *getIdentifier() const {
return Type.dyn_cast<IdentifierInfo *>();
}

SourceLocation getLocation() const { return Location; }
};

























class CXXPseudoDestructorExpr : public Expr {
friend class ASTStmtReader;


Stmt *Base = nullptr;



bool IsArrow : 1;


SourceLocation OperatorLoc;


NestedNameSpecifierLoc QualifierLoc;



TypeSourceInfo *ScopeType = nullptr;



SourceLocation ColonColonLoc;


SourceLocation TildeLoc;



PseudoDestructorTypeStorage DestroyedType;

public:
CXXPseudoDestructorExpr(const ASTContext &Context,
Expr *Base, bool isArrow, SourceLocation OperatorLoc,
NestedNameSpecifierLoc QualifierLoc,
TypeSourceInfo *ScopeType,
SourceLocation ColonColonLoc,
SourceLocation TildeLoc,
PseudoDestructorTypeStorage DestroyedType);

explicit CXXPseudoDestructorExpr(EmptyShell Shell)
: Expr(CXXPseudoDestructorExprClass, Shell), IsArrow(false) {}

Expr *getBase() const { return cast<Expr>(Base); }




bool hasQualifier() const { return QualifierLoc.hasQualifier(); }



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }




NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}



bool isArrow() const { return IsArrow; }


SourceLocation getOperatorLoc() const { return OperatorLoc; }










TypeSourceInfo *getScopeTypeInfo() const { return ScopeType; }



SourceLocation getColonColonLoc() const { return ColonColonLoc; }


SourceLocation getTildeLoc() const { return TildeLoc; }








TypeSourceInfo *getDestroyedTypeInfo() const {
return DestroyedType.getTypeSourceInfo();
}




IdentifierInfo *getDestroyedTypeIdentifier() const {
return DestroyedType.getIdentifier();
}


QualType getDestroyedType() const;


SourceLocation getDestroyedTypeLoc() const {
return DestroyedType.getLocation();
}



void setDestroyedType(IdentifierInfo *II, SourceLocation Loc) {
DestroyedType = PseudoDestructorTypeStorage(II, Loc);
}


void setDestroyedType(TypeSourceInfo *Info) {
DestroyedType = PseudoDestructorTypeStorage(Info);
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return Base->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXPseudoDestructorExprClass;
}


child_range children() { return child_range(&Base, &Base + 1); }

const_child_range children() const {
return const_child_range(&Base, &Base + 1);
}
};









class TypeTraitExpr final
: public Expr,
private llvm::TrailingObjects<TypeTraitExpr, TypeSourceInfo *> {

SourceLocation Loc;


SourceLocation RParenLoc;




TypeTraitExpr(QualType T, SourceLocation Loc, TypeTrait Kind,
ArrayRef<TypeSourceInfo *> Args,
SourceLocation RParenLoc,
bool Value);

TypeTraitExpr(EmptyShell Empty) : Expr(TypeTraitExprClass, Empty) {}

size_t numTrailingObjects(OverloadToken<TypeSourceInfo *>) const {
return getNumArgs();
}

public:
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;


static TypeTraitExpr *Create(const ASTContext &C, QualType T,
SourceLocation Loc, TypeTrait Kind,
ArrayRef<TypeSourceInfo *> Args,
SourceLocation RParenLoc,
bool Value);

static TypeTraitExpr *CreateDeserialized(const ASTContext &C,
unsigned NumArgs);


TypeTrait getTrait() const {
return static_cast<TypeTrait>(TypeTraitExprBits.Kind);
}

bool getValue() const {
assert(!isValueDependent());
return TypeTraitExprBits.Value;
}


unsigned getNumArgs() const { return TypeTraitExprBits.NumArgs; }


TypeSourceInfo *getArg(unsigned I) const {
assert(I < getNumArgs() && "Argument out-of-range");
return getArgs()[I];
}


ArrayRef<TypeSourceInfo *> getArgs() const {
return llvm::makeArrayRef(getTrailingObjects<TypeSourceInfo *>(),
getNumArgs());
}

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == TypeTraitExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};









class ArrayTypeTraitExpr : public Expr {

unsigned ATT : 2;


uint64_t Value = 0;


Expr *Dimension;


SourceLocation Loc;


SourceLocation RParen;


TypeSourceInfo *QueriedType = nullptr;

public:
friend class ASTStmtReader;

ArrayTypeTraitExpr(SourceLocation loc, ArrayTypeTrait att,
TypeSourceInfo *queried, uint64_t value, Expr *dimension,
SourceLocation rparen, QualType ty)
: Expr(ArrayTypeTraitExprClass, ty, VK_PRValue, OK_Ordinary), ATT(att),
Value(value), Dimension(dimension), Loc(loc), RParen(rparen),
QueriedType(queried) {
assert(att <= ATT_Last && "invalid enum value!");
assert(static_cast<unsigned>(att) == ATT && "ATT overflow!");
setDependence(computeDependence(this));
}

explicit ArrayTypeTraitExpr(EmptyShell Empty)
: Expr(ArrayTypeTraitExprClass, Empty), ATT(0) {}

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParen; }

ArrayTypeTrait getTrait() const { return static_cast<ArrayTypeTrait>(ATT); }

QualType getQueriedType() const { return QueriedType->getType(); }

TypeSourceInfo *getQueriedTypeSourceInfo() const { return QueriedType; }

uint64_t getValue() const { assert(!isTypeDependent()); return Value; }

Expr *getDimensionExpression() const { return Dimension; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ArrayTypeTraitExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};








class ExpressionTraitExpr : public Expr {

unsigned ET : 31;


unsigned Value : 1;


SourceLocation Loc;


SourceLocation RParen;


Expr* QueriedExpression = nullptr;

public:
friend class ASTStmtReader;

ExpressionTraitExpr(SourceLocation loc, ExpressionTrait et, Expr *queried,
bool value, SourceLocation rparen, QualType resultType)
: Expr(ExpressionTraitExprClass, resultType, VK_PRValue, OK_Ordinary),
ET(et), Value(value), Loc(loc), RParen(rparen),
QueriedExpression(queried) {
assert(et <= ET_Last && "invalid enum value!");
assert(static_cast<unsigned>(et) == ET && "ET overflow!");
setDependence(computeDependence(this));
}

explicit ExpressionTraitExpr(EmptyShell Empty)
: Expr(ExpressionTraitExprClass, Empty), ET(0), Value(false) {}

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParen; }

ExpressionTrait getTrait() const { return static_cast<ExpressionTrait>(ET); }

Expr *getQueriedExpression() const { return QueriedExpression; }

bool getValue() const { return Value; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ExpressionTraitExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};



class OverloadExpr : public Expr {
friend class ASTStmtReader;
friend class ASTStmtWriter;


DeclarationNameInfo NameInfo;


NestedNameSpecifierLoc QualifierLoc;

protected:
OverloadExpr(StmtClass SC, const ASTContext &Context,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc,
const DeclarationNameInfo &NameInfo,
const TemplateArgumentListInfo *TemplateArgs,
UnresolvedSetIterator Begin, UnresolvedSetIterator End,
bool KnownDependent, bool KnownInstantiationDependent,
bool KnownContainsUnexpandedParameterPack);

OverloadExpr(StmtClass SC, EmptyShell Empty, unsigned NumResults,
bool HasTemplateKWAndArgsInfo);


inline DeclAccessPair *getTrailingResults();
const DeclAccessPair *getTrailingResults() const {
return const_cast<OverloadExpr *>(this)->getTrailingResults();
}



inline ASTTemplateKWAndArgsInfo *getTrailingASTTemplateKWAndArgsInfo();
const ASTTemplateKWAndArgsInfo *getTrailingASTTemplateKWAndArgsInfo() const {
return const_cast<OverloadExpr *>(this)
->getTrailingASTTemplateKWAndArgsInfo();
}



inline TemplateArgumentLoc *getTrailingTemplateArgumentLoc();
const TemplateArgumentLoc *getTrailingTemplateArgumentLoc() const {
return const_cast<OverloadExpr *>(this)->getTrailingTemplateArgumentLoc();
}

bool hasTemplateKWAndArgsInfo() const {
return OverloadExprBits.HasTemplateKWAndArgsInfo;
}

public:
struct FindResult {
OverloadExpr *Expression;
bool IsAddressOfOperand;
bool HasFormOfMemberPointer;
};






static FindResult find(Expr *E) {
assert(E->getType()->isSpecificBuiltinType(BuiltinType::Overload));

FindResult Result;

E = E->IgnoreParens();
if (isa<UnaryOperator>(E)) {
assert(cast<UnaryOperator>(E)->getOpcode() == UO_AddrOf);
E = cast<UnaryOperator>(E)->getSubExpr();
auto *Ovl = cast<OverloadExpr>(E->IgnoreParens());

Result.HasFormOfMemberPointer = (E == Ovl && Ovl->getQualifier());
Result.IsAddressOfOperand = true;
Result.Expression = Ovl;
} else {
Result.HasFormOfMemberPointer = false;
Result.IsAddressOfOperand = false;
Result.Expression = cast<OverloadExpr>(E);
}

return Result;
}



inline CXXRecordDecl *getNamingClass();
const CXXRecordDecl *getNamingClass() const {
return const_cast<OverloadExpr *>(this)->getNamingClass();
}

using decls_iterator = UnresolvedSetImpl::iterator;

decls_iterator decls_begin() const {
return UnresolvedSetIterator(getTrailingResults());
}
decls_iterator decls_end() const {
return UnresolvedSetIterator(getTrailingResults() + getNumDecls());
}
llvm::iterator_range<decls_iterator> decls() const {
return llvm::make_range(decls_begin(), decls_end());
}


unsigned getNumDecls() const { return OverloadExprBits.NumResults; }


const DeclarationNameInfo &getNameInfo() const { return NameInfo; }


DeclarationName getName() const { return NameInfo.getName(); }


SourceLocation getNameLoc() const { return NameInfo.getLoc(); }


NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }



SourceLocation getTemplateKeywordLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingASTTemplateKWAndArgsInfo()->TemplateKWLoc;
}



SourceLocation getLAngleLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingASTTemplateKWAndArgsInfo()->LAngleLoc;
}



SourceLocation getRAngleLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingASTTemplateKWAndArgsInfo()->RAngleLoc;
}


bool hasTemplateKeyword() const { return getTemplateKeywordLoc().isValid(); }


bool hasExplicitTemplateArgs() const { return getLAngleLoc().isValid(); }

TemplateArgumentLoc const *getTemplateArgs() const {
if (!hasExplicitTemplateArgs())
return nullptr;
return const_cast<OverloadExpr *>(this)->getTrailingTemplateArgumentLoc();
}

unsigned getNumTemplateArgs() const {
if (!hasExplicitTemplateArgs())
return 0;

return getTrailingASTTemplateKWAndArgsInfo()->NumTemplateArgs;
}

ArrayRef<TemplateArgumentLoc> template_arguments() const {
return {getTemplateArgs(), getNumTemplateArgs()};
}


void copyTemplateArgumentsInto(TemplateArgumentListInfo &List) const {
if (hasExplicitTemplateArgs())
getTrailingASTTemplateKWAndArgsInfo()->copyInto(getTemplateArgs(), List);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == UnresolvedLookupExprClass ||
T->getStmtClass() == UnresolvedMemberExprClass;
}
};












class UnresolvedLookupExpr final
: public OverloadExpr,
private llvm::TrailingObjects<UnresolvedLookupExpr, DeclAccessPair,
ASTTemplateKWAndArgsInfo,
TemplateArgumentLoc> {
friend class ASTStmtReader;
friend class OverloadExpr;
friend TrailingObjects;




CXXRecordDecl *NamingClass;















UnresolvedLookupExpr(const ASTContext &Context, CXXRecordDecl *NamingClass,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc,
const DeclarationNameInfo &NameInfo, bool RequiresADL,
bool Overloaded,
const TemplateArgumentListInfo *TemplateArgs,
UnresolvedSetIterator Begin, UnresolvedSetIterator End);

UnresolvedLookupExpr(EmptyShell Empty, unsigned NumResults,
bool HasTemplateKWAndArgsInfo);

unsigned numTrailingObjects(OverloadToken<DeclAccessPair>) const {
return getNumDecls();
}

unsigned numTrailingObjects(OverloadToken<ASTTemplateKWAndArgsInfo>) const {
return hasTemplateKWAndArgsInfo();
}

public:
static UnresolvedLookupExpr *
Create(const ASTContext &Context, CXXRecordDecl *NamingClass,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo, bool RequiresADL, bool Overloaded,
UnresolvedSetIterator Begin, UnresolvedSetIterator End);

static UnresolvedLookupExpr *
Create(const ASTContext &Context, CXXRecordDecl *NamingClass,
NestedNameSpecifierLoc QualifierLoc, SourceLocation TemplateKWLoc,
const DeclarationNameInfo &NameInfo, bool RequiresADL,
const TemplateArgumentListInfo *Args, UnresolvedSetIterator Begin,
UnresolvedSetIterator End);

static UnresolvedLookupExpr *CreateEmpty(const ASTContext &Context,
unsigned NumResults,
bool HasTemplateKWAndArgsInfo,
unsigned NumTemplateArgs);



bool requiresADL() const { return UnresolvedLookupExprBits.RequiresADL; }


bool isOverloaded() const { return UnresolvedLookupExprBits.Overloaded; }




CXXRecordDecl *getNamingClass() { return NamingClass; }
const CXXRecordDecl *getNamingClass() const { return NamingClass; }

SourceLocation getBeginLoc() const LLVM_READONLY {
if (NestedNameSpecifierLoc l = getQualifierLoc())
return l.getBeginLoc();
return getNameInfo().getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
if (hasExplicitTemplateArgs())
return getRAngleLoc();
return getNameInfo().getEndLoc();
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == UnresolvedLookupExprClass;
}
};















class DependentScopeDeclRefExpr final
: public Expr,
private llvm::TrailingObjects<DependentScopeDeclRefExpr,
ASTTemplateKWAndArgsInfo,
TemplateArgumentLoc> {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;



NestedNameSpecifierLoc QualifierLoc;


DeclarationNameInfo NameInfo;

DependentScopeDeclRefExpr(QualType Ty, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc,
const DeclarationNameInfo &NameInfo,
const TemplateArgumentListInfo *Args);

size_t numTrailingObjects(OverloadToken<ASTTemplateKWAndArgsInfo>) const {
return hasTemplateKWAndArgsInfo();
}

bool hasTemplateKWAndArgsInfo() const {
return DependentScopeDeclRefExprBits.HasTemplateKWAndArgsInfo;
}

public:
static DependentScopeDeclRefExpr *
Create(const ASTContext &Context, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc, const DeclarationNameInfo &NameInfo,
const TemplateArgumentListInfo *TemplateArgs);

static DependentScopeDeclRefExpr *CreateEmpty(const ASTContext &Context,
bool HasTemplateKWAndArgsInfo,
unsigned NumTemplateArgs);


const DeclarationNameInfo &getNameInfo() const { return NameInfo; }


DeclarationName getDeclName() const { return NameInfo.getName(); }




SourceLocation getLocation() const { return NameInfo.getLoc(); }



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }



NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}



SourceLocation getTemplateKeywordLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->TemplateKWLoc;
}



SourceLocation getLAngleLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->LAngleLoc;
}



SourceLocation getRAngleLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->RAngleLoc;
}


bool hasTemplateKeyword() const { return getTemplateKeywordLoc().isValid(); }


bool hasExplicitTemplateArgs() const { return getLAngleLoc().isValid(); }



void copyTemplateArgumentsInto(TemplateArgumentListInfo &List) const {
if (hasExplicitTemplateArgs())
getTrailingObjects<ASTTemplateKWAndArgsInfo>()->copyInto(
getTrailingObjects<TemplateArgumentLoc>(), List);
}

TemplateArgumentLoc const *getTemplateArgs() const {
if (!hasExplicitTemplateArgs())
return nullptr;

return getTrailingObjects<TemplateArgumentLoc>();
}

unsigned getNumTemplateArgs() const {
if (!hasExplicitTemplateArgs())
return 0;

return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->NumTemplateArgs;
}

ArrayRef<TemplateArgumentLoc> template_arguments() const {
return {getTemplateArgs(), getNumTemplateArgs()};
}



SourceLocation getBeginLoc() const LLVM_READONLY {
return QualifierLoc.getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
if (hasExplicitTemplateArgs())
return getRAngleLoc();
return getLocation();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == DependentScopeDeclRefExprClass;
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};











class ExprWithCleanups final
: public FullExpr,
private llvm::TrailingObjects<
ExprWithCleanups,
llvm::PointerUnion<BlockDecl *, CompoundLiteralExpr *>> {
public:




using CleanupObject = llvm::PointerUnion<BlockDecl *, CompoundLiteralExpr *>;

private:
friend class ASTStmtReader;
friend TrailingObjects;

ExprWithCleanups(EmptyShell, unsigned NumObjects);
ExprWithCleanups(Expr *SubExpr, bool CleanupsHaveSideEffects,
ArrayRef<CleanupObject> Objects);

public:
static ExprWithCleanups *Create(const ASTContext &C, EmptyShell empty,
unsigned numObjects);

static ExprWithCleanups *Create(const ASTContext &C, Expr *subexpr,
bool CleanupsHaveSideEffects,
ArrayRef<CleanupObject> objects);

ArrayRef<CleanupObject> getObjects() const {
return llvm::makeArrayRef(getTrailingObjects<CleanupObject>(),
getNumObjects());
}

unsigned getNumObjects() const { return ExprWithCleanupsBits.NumObjects; }

CleanupObject getObject(unsigned i) const {
assert(i < getNumObjects() && "Index out of range");
return getObjects()[i];
}

bool cleanupsHaveSideEffects() const {
return ExprWithCleanupsBits.CleanupsHaveSideEffects;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return SubExpr->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
return SubExpr->getEndLoc();
}


static bool classof(const Stmt *T) {
return T->getStmtClass() == ExprWithCleanupsClass;
}


child_range children() { return child_range(&SubExpr, &SubExpr + 1); }

const_child_range children() const {
return const_child_range(&SubExpr, &SubExpr + 1);
}
};






















class CXXUnresolvedConstructExpr final
: public Expr,
private llvm::TrailingObjects<CXXUnresolvedConstructExpr, Expr *> {
friend class ASTStmtReader;
friend TrailingObjects;


TypeSourceInfo *TSI;


SourceLocation LParenLoc;


SourceLocation RParenLoc;

CXXUnresolvedConstructExpr(QualType T, TypeSourceInfo *TSI,
SourceLocation LParenLoc, ArrayRef<Expr *> Args,
SourceLocation RParenLoc);

CXXUnresolvedConstructExpr(EmptyShell Empty, unsigned NumArgs)
: Expr(CXXUnresolvedConstructExprClass, Empty), TSI(nullptr) {
CXXUnresolvedConstructExprBits.NumArgs = NumArgs;
}

public:
static CXXUnresolvedConstructExpr *Create(const ASTContext &Context,
QualType T, TypeSourceInfo *TSI,
SourceLocation LParenLoc,
ArrayRef<Expr *> Args,
SourceLocation RParenLoc);

static CXXUnresolvedConstructExpr *CreateEmpty(const ASTContext &Context,
unsigned NumArgs);



QualType getTypeAsWritten() const { return TSI->getType(); }



TypeSourceInfo *getTypeSourceInfo() const { return TSI; }



SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation L) { LParenLoc = L; }



SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }




bool isListInitialization() const { return LParenLoc.isInvalid(); }


unsigned getNumArgs() const { return CXXUnresolvedConstructExprBits.NumArgs; }

using arg_iterator = Expr **;
using arg_range = llvm::iterator_range<arg_iterator>;

arg_iterator arg_begin() { return getTrailingObjects<Expr *>(); }
arg_iterator arg_end() { return arg_begin() + getNumArgs(); }
arg_range arguments() { return arg_range(arg_begin(), arg_end()); }

using const_arg_iterator = const Expr* const *;
using const_arg_range = llvm::iterator_range<const_arg_iterator>;

const_arg_iterator arg_begin() const { return getTrailingObjects<Expr *>(); }
const_arg_iterator arg_end() const { return arg_begin() + getNumArgs(); }
const_arg_range arguments() const {
return const_arg_range(arg_begin(), arg_end());
}

Expr *getArg(unsigned I) {
assert(I < getNumArgs() && "Argument index out-of-range");
return arg_begin()[I];
}

const Expr *getArg(unsigned I) const {
assert(I < getNumArgs() && "Argument index out-of-range");
return arg_begin()[I];
}

void setArg(unsigned I, Expr *E) {
assert(I < getNumArgs() && "Argument index out-of-range");
arg_begin()[I] = E;
}

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY {
if (!RParenLoc.isValid() && getNumArgs() > 0)
return getArg(getNumArgs() - 1)->getEndLoc();
return RParenLoc;
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXUnresolvedConstructExprClass;
}


child_range children() {
auto **begin = reinterpret_cast<Stmt **>(arg_begin());
return child_range(begin, begin + getNumArgs());
}

const_child_range children() const {
auto **begin = reinterpret_cast<Stmt **>(
const_cast<CXXUnresolvedConstructExpr *>(this)->arg_begin());
return const_child_range(begin, begin + getNumArgs());
}
};








class CXXDependentScopeMemberExpr final
: public Expr,
private llvm::TrailingObjects<CXXDependentScopeMemberExpr,
ASTTemplateKWAndArgsInfo,
TemplateArgumentLoc, NamedDecl *> {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;



Stmt *Base;



QualType BaseType;




NestedNameSpecifierLoc QualifierLoc;





DeclarationNameInfo MemberNameInfo;
















bool hasTemplateKWAndArgsInfo() const {
return CXXDependentScopeMemberExprBits.HasTemplateKWAndArgsInfo;
}

bool hasFirstQualifierFoundInScope() const {
return CXXDependentScopeMemberExprBits.HasFirstQualifierFoundInScope;
}

unsigned numTrailingObjects(OverloadToken<ASTTemplateKWAndArgsInfo>) const {
return hasTemplateKWAndArgsInfo();
}

unsigned numTrailingObjects(OverloadToken<TemplateArgumentLoc>) const {
return getNumTemplateArgs();
}

unsigned numTrailingObjects(OverloadToken<NamedDecl *>) const {
return hasFirstQualifierFoundInScope();
}

CXXDependentScopeMemberExpr(const ASTContext &Ctx, Expr *Base,
QualType BaseType, bool IsArrow,
SourceLocation OperatorLoc,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc,
NamedDecl *FirstQualifierFoundInScope,
DeclarationNameInfo MemberNameInfo,
const TemplateArgumentListInfo *TemplateArgs);

CXXDependentScopeMemberExpr(EmptyShell Empty, bool HasTemplateKWAndArgsInfo,
bool HasFirstQualifierFoundInScope);

public:
static CXXDependentScopeMemberExpr *
Create(const ASTContext &Ctx, Expr *Base, QualType BaseType, bool IsArrow,
SourceLocation OperatorLoc, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc, NamedDecl *FirstQualifierFoundInScope,
DeclarationNameInfo MemberNameInfo,
const TemplateArgumentListInfo *TemplateArgs);

static CXXDependentScopeMemberExpr *
CreateEmpty(const ASTContext &Ctx, bool HasTemplateKWAndArgsInfo,
unsigned NumTemplateArgs, bool HasFirstQualifierFoundInScope);




bool isImplicitAccess() const {
if (!Base)
return true;
return cast<Expr>(Base)->isImplicitCXXThis();
}



Expr *getBase() const {
assert(!isImplicitAccess());
return cast<Expr>(Base);
}

QualType getBaseType() const { return BaseType; }



bool isArrow() const { return CXXDependentScopeMemberExprBits.IsArrow; }


SourceLocation getOperatorLoc() const {
return CXXDependentScopeMemberExprBits.OperatorLoc;
}


NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }












NamedDecl *getFirstQualifierFoundInScope() const {
if (!hasFirstQualifierFoundInScope())
return nullptr;
return *getTrailingObjects<NamedDecl *>();
}


const DeclarationNameInfo &getMemberNameInfo() const {
return MemberNameInfo;
}


DeclarationName getMember() const { return MemberNameInfo.getName(); }



SourceLocation getMemberLoc() const { return MemberNameInfo.getLoc(); }



SourceLocation getTemplateKeywordLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->TemplateKWLoc;
}



SourceLocation getLAngleLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->LAngleLoc;
}



SourceLocation getRAngleLoc() const {
if (!hasTemplateKWAndArgsInfo())
return SourceLocation();
return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->RAngleLoc;
}


bool hasTemplateKeyword() const { return getTemplateKeywordLoc().isValid(); }



bool hasExplicitTemplateArgs() const { return getLAngleLoc().isValid(); }



void copyTemplateArgumentsInto(TemplateArgumentListInfo &List) const {
if (hasExplicitTemplateArgs())
getTrailingObjects<ASTTemplateKWAndArgsInfo>()->copyInto(
getTrailingObjects<TemplateArgumentLoc>(), List);
}



const TemplateArgumentLoc *getTemplateArgs() const {
if (!hasExplicitTemplateArgs())
return nullptr;

return getTrailingObjects<TemplateArgumentLoc>();
}



unsigned getNumTemplateArgs() const {
if (!hasExplicitTemplateArgs())
return 0;

return getTrailingObjects<ASTTemplateKWAndArgsInfo>()->NumTemplateArgs;
}

ArrayRef<TemplateArgumentLoc> template_arguments() const {
return {getTemplateArgs(), getNumTemplateArgs()};
}

SourceLocation getBeginLoc() const LLVM_READONLY {
if (!isImplicitAccess())
return Base->getBeginLoc();
if (getQualifier())
return getQualifierLoc().getBeginLoc();
return MemberNameInfo.getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
if (hasExplicitTemplateArgs())
return getRAngleLoc();
return MemberNameInfo.getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXDependentScopeMemberExprClass;
}


child_range children() {
if (isImplicitAccess())
return child_range(child_iterator(), child_iterator());
return child_range(&Base, &Base + 1);
}

const_child_range children() const {
if (isImplicitAccess())
return const_child_range(const_child_iterator(), const_child_iterator());
return const_child_range(&Base, &Base + 1);
}
};
















class UnresolvedMemberExpr final
: public OverloadExpr,
private llvm::TrailingObjects<UnresolvedMemberExpr, DeclAccessPair,
ASTTemplateKWAndArgsInfo,
TemplateArgumentLoc> {
friend class ASTStmtReader;
friend class OverloadExpr;
friend TrailingObjects;





Stmt *Base;


QualType BaseType;


SourceLocation OperatorLoc;















UnresolvedMemberExpr(const ASTContext &Context, bool HasUnresolvedUsing,
Expr *Base, QualType BaseType, bool IsArrow,
SourceLocation OperatorLoc,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc,
const DeclarationNameInfo &MemberNameInfo,
const TemplateArgumentListInfo *TemplateArgs,
UnresolvedSetIterator Begin, UnresolvedSetIterator End);

UnresolvedMemberExpr(EmptyShell Empty, unsigned NumResults,
bool HasTemplateKWAndArgsInfo);

unsigned numTrailingObjects(OverloadToken<DeclAccessPair>) const {
return getNumDecls();
}

unsigned numTrailingObjects(OverloadToken<ASTTemplateKWAndArgsInfo>) const {
return hasTemplateKWAndArgsInfo();
}

public:
static UnresolvedMemberExpr *
Create(const ASTContext &Context, bool HasUnresolvedUsing, Expr *Base,
QualType BaseType, bool IsArrow, SourceLocation OperatorLoc,
NestedNameSpecifierLoc QualifierLoc, SourceLocation TemplateKWLoc,
const DeclarationNameInfo &MemberNameInfo,
const TemplateArgumentListInfo *TemplateArgs,
UnresolvedSetIterator Begin, UnresolvedSetIterator End);

static UnresolvedMemberExpr *CreateEmpty(const ASTContext &Context,
unsigned NumResults,
bool HasTemplateKWAndArgsInfo,
unsigned NumTemplateArgs);





bool isImplicitAccess() const;



Expr *getBase() {
assert(!isImplicitAccess());
return cast<Expr>(Base);
}
const Expr *getBase() const {
assert(!isImplicitAccess());
return cast<Expr>(Base);
}

QualType getBaseType() const { return BaseType; }



bool hasUnresolvedUsing() const {
return UnresolvedMemberExprBits.HasUnresolvedUsing;
}



bool isArrow() const { return UnresolvedMemberExprBits.IsArrow; }


SourceLocation getOperatorLoc() const { return OperatorLoc; }


CXXRecordDecl *getNamingClass();
const CXXRecordDecl *getNamingClass() const {
return const_cast<UnresolvedMemberExpr *>(this)->getNamingClass();
}



const DeclarationNameInfo &getMemberNameInfo() const { return getNameInfo(); }


DeclarationName getMemberName() const { return getName(); }



SourceLocation getMemberLoc() const { return getNameLoc(); }



SourceLocation getExprLoc() const LLVM_READONLY { return getMemberLoc(); }

SourceLocation getBeginLoc() const LLVM_READONLY {
if (!isImplicitAccess())
return Base->getBeginLoc();
if (NestedNameSpecifierLoc l = getQualifierLoc())
return l.getBeginLoc();
return getMemberNameInfo().getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
if (hasExplicitTemplateArgs())
return getRAngleLoc();
return getMemberNameInfo().getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == UnresolvedMemberExprClass;
}


child_range children() {
if (isImplicitAccess())
return child_range(child_iterator(), child_iterator());
return child_range(&Base, &Base + 1);
}

const_child_range children() const {
if (isImplicitAccess())
return const_child_range(const_child_iterator(), const_child_iterator());
return const_child_range(&Base, &Base + 1);
}
};

DeclAccessPair *OverloadExpr::getTrailingResults() {
if (auto *ULE = dyn_cast<UnresolvedLookupExpr>(this))
return ULE->getTrailingObjects<DeclAccessPair>();
return cast<UnresolvedMemberExpr>(this)->getTrailingObjects<DeclAccessPair>();
}

ASTTemplateKWAndArgsInfo *OverloadExpr::getTrailingASTTemplateKWAndArgsInfo() {
if (!hasTemplateKWAndArgsInfo())
return nullptr;

if (auto *ULE = dyn_cast<UnresolvedLookupExpr>(this))
return ULE->getTrailingObjects<ASTTemplateKWAndArgsInfo>();
return cast<UnresolvedMemberExpr>(this)
->getTrailingObjects<ASTTemplateKWAndArgsInfo>();
}

TemplateArgumentLoc *OverloadExpr::getTrailingTemplateArgumentLoc() {
if (auto *ULE = dyn_cast<UnresolvedLookupExpr>(this))
return ULE->getTrailingObjects<TemplateArgumentLoc>();
return cast<UnresolvedMemberExpr>(this)
->getTrailingObjects<TemplateArgumentLoc>();
}

CXXRecordDecl *OverloadExpr::getNamingClass() {
if (auto *ULE = dyn_cast<UnresolvedLookupExpr>(this))
return ULE->getNamingClass();
return cast<UnresolvedMemberExpr>(this)->getNamingClass();
}





class CXXNoexceptExpr : public Expr {
friend class ASTStmtReader;

Stmt *Operand;
SourceRange Range;

public:
CXXNoexceptExpr(QualType Ty, Expr *Operand, CanThrowResult Val,
SourceLocation Keyword, SourceLocation RParen)
: Expr(CXXNoexceptExprClass, Ty, VK_PRValue, OK_Ordinary),
Operand(Operand), Range(Keyword, RParen) {
CXXNoexceptExprBits.Value = Val == CT_Cannot;
setDependence(computeDependence(this, Val));
}

CXXNoexceptExpr(EmptyShell Empty) : Expr(CXXNoexceptExprClass, Empty) {}

Expr *getOperand() const { return static_cast<Expr *>(Operand); }

SourceLocation getBeginLoc() const { return Range.getBegin(); }
SourceLocation getEndLoc() const { return Range.getEnd(); }
SourceRange getSourceRange() const { return Range; }

bool getValue() const { return CXXNoexceptExprBits.Value; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXNoexceptExprClass;
}


child_range children() { return child_range(&Operand, &Operand + 1); }

const_child_range children() const {
return const_child_range(&Operand, &Operand + 1);
}
};


















class PackExpansionExpr : public Expr {
friend class ASTStmtReader;
friend class ASTStmtWriter;

SourceLocation EllipsisLoc;






unsigned NumExpansions;

Stmt *Pattern;

public:
PackExpansionExpr(QualType T, Expr *Pattern, SourceLocation EllipsisLoc,
Optional<unsigned> NumExpansions)
: Expr(PackExpansionExprClass, T, Pattern->getValueKind(),
Pattern->getObjectKind()),
EllipsisLoc(EllipsisLoc),
NumExpansions(NumExpansions ? *NumExpansions + 1 : 0),
Pattern(Pattern) {
setDependence(computeDependence(this));
}

PackExpansionExpr(EmptyShell Empty) : Expr(PackExpansionExprClass, Empty) {}


Expr *getPattern() { return reinterpret_cast<Expr *>(Pattern); }


const Expr *getPattern() const { return reinterpret_cast<Expr *>(Pattern); }



SourceLocation getEllipsisLoc() const { return EllipsisLoc; }



Optional<unsigned> getNumExpansions() const {
if (NumExpansions)
return NumExpansions - 1;

return None;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return Pattern->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY { return EllipsisLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == PackExpansionExprClass;
}


child_range children() {
return child_range(&Pattern, &Pattern + 1);
}

const_child_range children() const {
return const_child_range(&Pattern, &Pattern + 1);
}
};










class SizeOfPackExpr final
: public Expr,
private llvm::TrailingObjects<SizeOfPackExpr, TemplateArgument> {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;


SourceLocation OperatorLoc;


SourceLocation PackLoc;


SourceLocation RParenLoc;











unsigned Length;


NamedDecl *Pack = nullptr;



SizeOfPackExpr(QualType SizeType, SourceLocation OperatorLoc, NamedDecl *Pack,
SourceLocation PackLoc, SourceLocation RParenLoc,
Optional<unsigned> Length,
ArrayRef<TemplateArgument> PartialArgs)
: Expr(SizeOfPackExprClass, SizeType, VK_PRValue, OK_Ordinary),
OperatorLoc(OperatorLoc), PackLoc(PackLoc), RParenLoc(RParenLoc),
Length(Length ? *Length : PartialArgs.size()), Pack(Pack) {
assert((!Length || PartialArgs.empty()) &&
"have partial args for non-dependent sizeof... expression");
auto *Args = getTrailingObjects<TemplateArgument>();
std::uninitialized_copy(PartialArgs.begin(), PartialArgs.end(), Args);
setDependence(Length ? ExprDependence::None
: ExprDependence::ValueInstantiation);
}


SizeOfPackExpr(EmptyShell Empty, unsigned NumPartialArgs)
: Expr(SizeOfPackExprClass, Empty), Length(NumPartialArgs) {}

public:
static SizeOfPackExpr *Create(ASTContext &Context, SourceLocation OperatorLoc,
NamedDecl *Pack, SourceLocation PackLoc,
SourceLocation RParenLoc,
Optional<unsigned> Length = None,
ArrayRef<TemplateArgument> PartialArgs = None);
static SizeOfPackExpr *CreateDeserialized(ASTContext &Context,
unsigned NumPartialArgs);


SourceLocation getOperatorLoc() const { return OperatorLoc; }


SourceLocation getPackLoc() const { return PackLoc; }


SourceLocation getRParenLoc() const { return RParenLoc; }


NamedDecl *getPack() const { return Pack; }





unsigned getPackLength() const {
assert(!isValueDependent() &&
"Cannot get the length of a value-dependent pack size expression");
return Length;
}






bool isPartiallySubstituted() const {
return isValueDependent() && Length;
}


ArrayRef<TemplateArgument> getPartialArguments() const {
assert(isPartiallySubstituted());
const auto *Args = getTrailingObjects<TemplateArgument>();
return llvm::makeArrayRef(Args, Args + Length);
}

SourceLocation getBeginLoc() const LLVM_READONLY { return OperatorLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == SizeOfPackExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};



class SubstNonTypeTemplateParmExpr : public Expr {
friend class ASTReader;
friend class ASTStmtReader;




llvm::PointerIntPair<NonTypeTemplateParmDecl*, 1, bool> ParamAndRef;


Stmt *Replacement;

explicit SubstNonTypeTemplateParmExpr(EmptyShell Empty)
: Expr(SubstNonTypeTemplateParmExprClass, Empty) {}

public:
SubstNonTypeTemplateParmExpr(QualType Ty, ExprValueKind ValueKind,
SourceLocation Loc,
NonTypeTemplateParmDecl *Param, bool RefParam,
Expr *Replacement)
: Expr(SubstNonTypeTemplateParmExprClass, Ty, ValueKind, OK_Ordinary),
ParamAndRef(Param, RefParam), Replacement(Replacement) {
SubstNonTypeTemplateParmExprBits.NameLoc = Loc;
setDependence(computeDependence(this));
}

SourceLocation getNameLoc() const {
return SubstNonTypeTemplateParmExprBits.NameLoc;
}
SourceLocation getBeginLoc() const { return getNameLoc(); }
SourceLocation getEndLoc() const { return getNameLoc(); }

Expr *getReplacement() const { return cast<Expr>(Replacement); }

NonTypeTemplateParmDecl *getParameter() const {
return ParamAndRef.getPointer();
}

bool isReferenceParameter() const { return ParamAndRef.getInt(); }


QualType getParameterType(const ASTContext &Ctx) const;

static bool classof(const Stmt *s) {
return s->getStmtClass() == SubstNonTypeTemplateParmExprClass;
}


child_range children() { return child_range(&Replacement, &Replacement + 1); }

const_child_range children() const {
return const_child_range(&Replacement, &Replacement + 1);
}
};













class SubstNonTypeTemplateParmPackExpr : public Expr {
friend class ASTReader;
friend class ASTStmtReader;


NonTypeTemplateParmDecl *Param;



const TemplateArgument *Arguments;


unsigned NumArguments;


SourceLocation NameLoc;

explicit SubstNonTypeTemplateParmPackExpr(EmptyShell Empty)
: Expr(SubstNonTypeTemplateParmPackExprClass, Empty) {}

public:
SubstNonTypeTemplateParmPackExpr(QualType T,
ExprValueKind ValueKind,
NonTypeTemplateParmDecl *Param,
SourceLocation NameLoc,
const TemplateArgument &ArgPack);


NonTypeTemplateParmDecl *getParameterPack() const { return Param; }


SourceLocation getParameterPackLocation() const { return NameLoc; }



TemplateArgument getArgumentPack() const;

SourceLocation getBeginLoc() const LLVM_READONLY { return NameLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return NameLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == SubstNonTypeTemplateParmPackExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};















class FunctionParmPackExpr final
: public Expr,
private llvm::TrailingObjects<FunctionParmPackExpr, VarDecl *> {
friend class ASTReader;
friend class ASTStmtReader;
friend TrailingObjects;


VarDecl *ParamPack;


SourceLocation NameLoc;


unsigned NumParameters;

FunctionParmPackExpr(QualType T, VarDecl *ParamPack,
SourceLocation NameLoc, unsigned NumParams,
VarDecl *const *Params);

public:
static FunctionParmPackExpr *Create(const ASTContext &Context, QualType T,
VarDecl *ParamPack,
SourceLocation NameLoc,
ArrayRef<VarDecl *> Params);
static FunctionParmPackExpr *CreateEmpty(const ASTContext &Context,
unsigned NumParams);


VarDecl *getParameterPack() const { return ParamPack; }


SourceLocation getParameterPackLocation() const { return NameLoc; }



using iterator = VarDecl * const *;
iterator begin() const { return getTrailingObjects<VarDecl *>(); }
iterator end() const { return begin() + NumParameters; }


unsigned getNumExpansions() const { return NumParameters; }


VarDecl *getExpansion(unsigned I) const { return begin()[I]; }

SourceLocation getBeginLoc() const LLVM_READONLY { return NameLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return NameLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == FunctionParmPackExprClass;
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};





















class MaterializeTemporaryExpr : public Expr {
private:
friend class ASTStmtReader;
friend class ASTStmtWriter;

llvm::PointerUnion<Stmt *, LifetimeExtendedTemporaryDecl *> State;

public:
MaterializeTemporaryExpr(QualType T, Expr *Temporary,
bool BoundToLvalueReference,
LifetimeExtendedTemporaryDecl *MTD = nullptr);

MaterializeTemporaryExpr(EmptyShell Empty)
: Expr(MaterializeTemporaryExprClass, Empty) {}



Expr *getSubExpr() const {
return cast<Expr>(
State.is<Stmt *>()
? State.get<Stmt *>()
: State.get<LifetimeExtendedTemporaryDecl *>()->getTemporaryExpr());
}


StorageDuration getStorageDuration() const {
return State.is<Stmt *>() ? SD_FullExpression
: State.get<LifetimeExtendedTemporaryDecl *>()
->getStorageDuration();
}



APValue *getOrCreateValue(bool MayCreate) const {
assert(State.is<LifetimeExtendedTemporaryDecl *>() &&
"the temporary has not been lifetime extended");
return State.get<LifetimeExtendedTemporaryDecl *>()->getOrCreateValue(
MayCreate);
}

LifetimeExtendedTemporaryDecl *getLifetimeExtendedTemporaryDecl() {
return State.dyn_cast<LifetimeExtendedTemporaryDecl *>();
}
const LifetimeExtendedTemporaryDecl *
getLifetimeExtendedTemporaryDecl() const {
return State.dyn_cast<LifetimeExtendedTemporaryDecl *>();
}



ValueDecl *getExtendingDecl() {
return State.is<Stmt *>() ? nullptr
: State.get<LifetimeExtendedTemporaryDecl *>()
->getExtendingDecl();
}
const ValueDecl *getExtendingDecl() const {
return const_cast<MaterializeTemporaryExpr *>(this)->getExtendingDecl();
}

void setExtendingDecl(ValueDecl *ExtendedBy, unsigned ManglingNumber);

unsigned getManglingNumber() const {
return State.is<Stmt *>() ? 0
: State.get<LifetimeExtendedTemporaryDecl *>()
->getManglingNumber();
}



bool isBoundToLvalueReference() const { return isLValue(); }



bool isUsableInConstantExpressions(const ASTContext &Context) const;

SourceLocation getBeginLoc() const LLVM_READONLY {
return getSubExpr()->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
return getSubExpr()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == MaterializeTemporaryExprClass;
}


child_range children() {
return State.is<Stmt *>()
? child_range(State.getAddrOfPtr1(), State.getAddrOfPtr1() + 1)
: State.get<LifetimeExtendedTemporaryDecl *>()->childrenExpr();
}

const_child_range children() const {
return State.is<Stmt *>()
? const_child_range(State.getAddrOfPtr1(),
State.getAddrOfPtr1() + 1)
: const_cast<const LifetimeExtendedTemporaryDecl *>(
State.get<LifetimeExtendedTemporaryDecl *>())
->childrenExpr();
}
};









class CXXFoldExpr : public Expr {
friend class ASTStmtReader;
friend class ASTStmtWriter;

enum SubExpr { Callee, LHS, RHS, Count };

SourceLocation LParenLoc;
SourceLocation EllipsisLoc;
SourceLocation RParenLoc;


unsigned NumExpansions;
Stmt *SubExprs[SubExpr::Count];
BinaryOperatorKind Opcode;

public:
CXXFoldExpr(QualType T, UnresolvedLookupExpr *Callee,
SourceLocation LParenLoc, Expr *LHS, BinaryOperatorKind Opcode,
SourceLocation EllipsisLoc, Expr *RHS, SourceLocation RParenLoc,
Optional<unsigned> NumExpansions)
: Expr(CXXFoldExprClass, T, VK_PRValue, OK_Ordinary),
LParenLoc(LParenLoc), EllipsisLoc(EllipsisLoc), RParenLoc(RParenLoc),
NumExpansions(NumExpansions ? *NumExpansions + 1 : 0), Opcode(Opcode) {
SubExprs[SubExpr::Callee] = Callee;
SubExprs[SubExpr::LHS] = LHS;
SubExprs[SubExpr::RHS] = RHS;
setDependence(computeDependence(this));
}

CXXFoldExpr(EmptyShell Empty) : Expr(CXXFoldExprClass, Empty) {}

UnresolvedLookupExpr *getCallee() const {
return static_cast<UnresolvedLookupExpr *>(SubExprs[SubExpr::Callee]);
}
Expr *getLHS() const { return static_cast<Expr*>(SubExprs[SubExpr::LHS]); }
Expr *getRHS() const { return static_cast<Expr*>(SubExprs[SubExpr::RHS]); }


bool isRightFold() const {
return getLHS() && getLHS()->containsUnexpandedParameterPack();
}


bool isLeftFold() const { return !isRightFold(); }


Expr *getPattern() const { return isLeftFold() ? getRHS() : getLHS(); }


Expr *getInit() const { return isLeftFold() ? getLHS() : getRHS(); }

SourceLocation getLParenLoc() const { return LParenLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
SourceLocation getEllipsisLoc() const { return EllipsisLoc; }
BinaryOperatorKind getOperator() const { return Opcode; }

Optional<unsigned> getNumExpansions() const {
if (NumExpansions)
return NumExpansions - 1;
return None;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
if (LParenLoc.isValid())
return LParenLoc;
if (isLeftFold())
return getEllipsisLoc();
return getLHS()->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {
if (RParenLoc.isValid())
return RParenLoc;
if (isRightFold())
return getEllipsisLoc();
return getRHS()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXFoldExprClass;
}


child_range children() {
return child_range(SubExprs, SubExprs + SubExpr::Count);
}

const_child_range children() const {
return const_child_range(SubExprs, SubExprs + SubExpr::Count);
}
};














class CoroutineSuspendExpr : public Expr {
friend class ASTStmtReader;

SourceLocation KeywordLoc;

enum SubExpr { Common, Ready, Suspend, Resume, Count };

Stmt *SubExprs[SubExpr::Count];
OpaqueValueExpr *OpaqueValue = nullptr;

public:
CoroutineSuspendExpr(StmtClass SC, SourceLocation KeywordLoc, Expr *Common,
Expr *Ready, Expr *Suspend, Expr *Resume,
OpaqueValueExpr *OpaqueValue)
: Expr(SC, Resume->getType(), Resume->getValueKind(),
Resume->getObjectKind()),
KeywordLoc(KeywordLoc), OpaqueValue(OpaqueValue) {
SubExprs[SubExpr::Common] = Common;
SubExprs[SubExpr::Ready] = Ready;
SubExprs[SubExpr::Suspend] = Suspend;
SubExprs[SubExpr::Resume] = Resume;
setDependence(computeDependence(this));
}

CoroutineSuspendExpr(StmtClass SC, SourceLocation KeywordLoc, QualType Ty,
Expr *Common)
: Expr(SC, Ty, VK_PRValue, OK_Ordinary), KeywordLoc(KeywordLoc) {
assert(Common->isTypeDependent() && Ty->isDependentType() &&
"wrong constructor for non-dependent co_await/co_yield expression");
SubExprs[SubExpr::Common] = Common;
SubExprs[SubExpr::Ready] = nullptr;
SubExprs[SubExpr::Suspend] = nullptr;
SubExprs[SubExpr::Resume] = nullptr;
setDependence(computeDependence(this));
}

CoroutineSuspendExpr(StmtClass SC, EmptyShell Empty) : Expr(SC, Empty) {
SubExprs[SubExpr::Common] = nullptr;
SubExprs[SubExpr::Ready] = nullptr;
SubExprs[SubExpr::Suspend] = nullptr;
SubExprs[SubExpr::Resume] = nullptr;
}

SourceLocation getKeywordLoc() const { return KeywordLoc; }

Expr *getCommonExpr() const {
return static_cast<Expr*>(SubExprs[SubExpr::Common]);
}


OpaqueValueExpr *getOpaqueValue() const { return OpaqueValue; }

Expr *getReadyExpr() const {
return static_cast<Expr*>(SubExprs[SubExpr::Ready]);
}

Expr *getSuspendExpr() const {
return static_cast<Expr*>(SubExprs[SubExpr::Suspend]);
}

Expr *getResumeExpr() const {
return static_cast<Expr*>(SubExprs[SubExpr::Resume]);
}

SourceLocation getBeginLoc() const LLVM_READONLY { return KeywordLoc; }

SourceLocation getEndLoc() const LLVM_READONLY {
return getCommonExpr()->getEndLoc();
}

child_range children() {
return child_range(SubExprs, SubExprs + SubExpr::Count);
}

const_child_range children() const {
return const_child_range(SubExprs, SubExprs + SubExpr::Count);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CoawaitExprClass ||
T->getStmtClass() == CoyieldExprClass;
}
};


class CoawaitExpr : public CoroutineSuspendExpr {
friend class ASTStmtReader;

public:
CoawaitExpr(SourceLocation CoawaitLoc, Expr *Operand, Expr *Ready,
Expr *Suspend, Expr *Resume, OpaqueValueExpr *OpaqueValue,
bool IsImplicit = false)
: CoroutineSuspendExpr(CoawaitExprClass, CoawaitLoc, Operand, Ready,
Suspend, Resume, OpaqueValue) {
CoawaitBits.IsImplicit = IsImplicit;
}

CoawaitExpr(SourceLocation CoawaitLoc, QualType Ty, Expr *Operand,
bool IsImplicit = false)
: CoroutineSuspendExpr(CoawaitExprClass, CoawaitLoc, Ty, Operand) {
CoawaitBits.IsImplicit = IsImplicit;
}

CoawaitExpr(EmptyShell Empty)
: CoroutineSuspendExpr(CoawaitExprClass, Empty) {}

Expr *getOperand() const {

return getCommonExpr();
}

bool isImplicit() const { return CoawaitBits.IsImplicit; }
void setIsImplicit(bool value = true) { CoawaitBits.IsImplicit = value; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CoawaitExprClass;
}
};



class DependentCoawaitExpr : public Expr {
friend class ASTStmtReader;

SourceLocation KeywordLoc;
Stmt *SubExprs[2];

public:
DependentCoawaitExpr(SourceLocation KeywordLoc, QualType Ty, Expr *Op,
UnresolvedLookupExpr *OpCoawait)
: Expr(DependentCoawaitExprClass, Ty, VK_PRValue, OK_Ordinary),
KeywordLoc(KeywordLoc) {


assert(Ty->isDependentType() &&
"wrong constructor for non-dependent co_await/co_yield expression");
SubExprs[0] = Op;
SubExprs[1] = OpCoawait;
setDependence(computeDependence(this));
}

DependentCoawaitExpr(EmptyShell Empty)
: Expr(DependentCoawaitExprClass, Empty) {}

Expr *getOperand() const { return cast<Expr>(SubExprs[0]); }

UnresolvedLookupExpr *getOperatorCoawaitLookup() const {
return cast<UnresolvedLookupExpr>(SubExprs[1]);
}

SourceLocation getKeywordLoc() const { return KeywordLoc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return KeywordLoc; }

SourceLocation getEndLoc() const LLVM_READONLY {
return getOperand()->getEndLoc();
}

child_range children() { return child_range(SubExprs, SubExprs + 2); }

const_child_range children() const {
return const_child_range(SubExprs, SubExprs + 2);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == DependentCoawaitExprClass;
}
};


class CoyieldExpr : public CoroutineSuspendExpr {
friend class ASTStmtReader;

public:
CoyieldExpr(SourceLocation CoyieldLoc, Expr *Operand, Expr *Ready,
Expr *Suspend, Expr *Resume, OpaqueValueExpr *OpaqueValue)
: CoroutineSuspendExpr(CoyieldExprClass, CoyieldLoc, Operand, Ready,
Suspend, Resume, OpaqueValue) {}
CoyieldExpr(SourceLocation CoyieldLoc, QualType Ty, Expr *Operand)
: CoroutineSuspendExpr(CoyieldExprClass, CoyieldLoc, Ty, Operand) {}
CoyieldExpr(EmptyShell Empty)
: CoroutineSuspendExpr(CoyieldExprClass, Empty) {}

Expr *getOperand() const {

return getCommonExpr();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CoyieldExprClass;
}
};




class BuiltinBitCastExpr final
: public ExplicitCastExpr,
private llvm::TrailingObjects<BuiltinBitCastExpr, CXXBaseSpecifier *> {
friend class ASTStmtReader;
friend class CastExpr;
friend TrailingObjects;

SourceLocation KWLoc;
SourceLocation RParenLoc;

public:
BuiltinBitCastExpr(QualType T, ExprValueKind VK, CastKind CK, Expr *SrcExpr,
TypeSourceInfo *DstType, SourceLocation KWLoc,
SourceLocation RParenLoc)
: ExplicitCastExpr(BuiltinBitCastExprClass, T, VK, CK, SrcExpr, 0, false,
DstType),
KWLoc(KWLoc), RParenLoc(RParenLoc) {}
BuiltinBitCastExpr(EmptyShell Empty)
: ExplicitCastExpr(BuiltinBitCastExprClass, Empty, 0, false) {}

SourceLocation getBeginLoc() const LLVM_READONLY { return KWLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == BuiltinBitCastExprClass;
}
};

}

#endif
