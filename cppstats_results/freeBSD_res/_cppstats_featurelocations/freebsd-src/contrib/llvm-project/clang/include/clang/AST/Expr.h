











#if !defined(LLVM_CLANG_AST_EXPR_H)
#define LLVM_CLANG_AST_EXPR_H

#include "clang/AST/APValue.h"
#include "clang/AST/ASTVector.h"
#include "clang/AST/ComputeDependence.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclAccessPair.h"
#include "clang/AST/DependenceFlags.h"
#include "clang/AST/OperationKinds.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/Type.h"
#include "clang/Basic/CharInfo.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SyncScope.h"
#include "clang/Basic/TypeTraits.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/AtomicOrdering.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"

namespace clang {
class APValue;
class ASTContext;
class BlockDecl;
class CXXBaseSpecifier;
class CXXMemberCallExpr;
class CXXOperatorCallExpr;
class CastExpr;
class Decl;
class IdentifierInfo;
class MaterializeTemporaryExpr;
class NamedDecl;
class ObjCPropertyRefExpr;
class OpaqueValueExpr;
class ParmVarDecl;
class StringLiteral;
class TargetInfo;
class ValueDecl;


typedef SmallVector<CXXBaseSpecifier*, 4> CXXCastPath;



struct SubobjectAdjustment {
enum {
DerivedToBaseAdjustment,
FieldAdjustment,
MemberPointerAdjustment
} Kind;

struct DTB {
const CastExpr *BasePath;
const CXXRecordDecl *DerivedClass;
};

struct P {
const MemberPointerType *MPT;
Expr *RHS;
};

union {
struct DTB DerivedToBase;
FieldDecl *Field;
struct P Ptr;
};

SubobjectAdjustment(const CastExpr *BasePath,
const CXXRecordDecl *DerivedClass)
: Kind(DerivedToBaseAdjustment) {
DerivedToBase.BasePath = BasePath;
DerivedToBase.DerivedClass = DerivedClass;
}

SubobjectAdjustment(FieldDecl *Field)
: Kind(FieldAdjustment) {
this->Field = Field;
}

SubobjectAdjustment(const MemberPointerType *MPT, Expr *RHS)
: Kind(MemberPointerAdjustment) {
this->Ptr.MPT = MPT;
this->Ptr.RHS = RHS;
}
};




class Expr : public ValueStmt {
QualType TR;

public:
Expr() = delete;
Expr(const Expr&) = delete;
Expr(Expr &&) = delete;
Expr &operator=(const Expr&) = delete;
Expr &operator=(Expr&&) = delete;

protected:
Expr(StmtClass SC, QualType T, ExprValueKind VK, ExprObjectKind OK)
: ValueStmt(SC) {
ExprBits.Dependent = 0;
ExprBits.ValueKind = VK;
ExprBits.ObjectKind = OK;
assert(ExprBits.ObjectKind == OK && "truncated kind");
setType(T);
}


explicit Expr(StmtClass SC, EmptyShell) : ValueStmt(SC) { }



void setDependence(ExprDependence Deps) {
ExprBits.Dependent = static_cast<unsigned>(Deps);
}
friend class ASTImporter;
friend class ASTStmtReader;

public:
QualType getType() const { return TR; }
void setType(QualType t) {






assert((t.isNull() || !t->isReferenceType()) &&
"Expressions can't have reference type");

TR = t;
}

ExprDependence getDependence() const {
return static_cast<ExprDependence>(ExprBits.Dependent);
}










bool isValueDependent() const {
return static_cast<bool>(getDependence() & ExprDependence::Value);
}














bool isTypeDependent() const {
return static_cast<bool>(getDependence() & ExprDependence::Type);
}


























bool isInstantiationDependent() const {
return static_cast<bool>(getDependence() & ExprDependence::Instantiation);
}















bool containsUnexpandedParameterPack() const {
return static_cast<bool>(getDependence() & ExprDependence::UnexpandedPack);
}



bool containsErrors() const {
return static_cast<bool>(getDependence() & ExprDependence::Error);
}



SourceLocation getExprLoc() const LLVM_READONLY;




bool isReadIfDiscardedInCPlusPlus11() const;





bool isUnusedResultAWarning(const Expr *&WarnExpr, SourceLocation &Loc,
SourceRange &R1, SourceRange &R2,
ASTContext &Ctx) const;












bool isLValue() const { return getValueKind() == VK_LValue; }
bool isPRValue() const { return getValueKind() == VK_PRValue; }
bool isXValue() const { return getValueKind() == VK_XValue; }
bool isGLValue() const { return getValueKind() != VK_PRValue; }

enum LValueClassification {
LV_Valid,
LV_NotObjectType,
LV_IncompleteVoidType,
LV_DuplicateVectorComponents,
LV_InvalidExpression,
LV_InvalidMessageExpression,
LV_MemberFunction,
LV_SubObjCPropertySetting,
LV_ClassTemporary,
LV_ArrayTemporary
};

LValueClassification ClassifyLValue(ASTContext &Ctx) const;

enum isModifiableLvalueResult {
MLV_Valid,
MLV_NotObjectType,
MLV_IncompleteVoidType,
MLV_DuplicateVectorComponents,
MLV_InvalidExpression,
MLV_LValueCast,
MLV_IncompleteType,
MLV_ConstQualified,
MLV_ConstQualifiedField,
MLV_ConstAddrSpace,
MLV_ArrayType,
MLV_NoSetterProperty,
MLV_MemberFunction,
MLV_SubObjCPropertySetting,
MLV_InvalidMessageExpression,
MLV_ClassTemporary,
MLV_ArrayTemporary
};









isModifiableLvalueResult
isModifiableLvalue(ASTContext &Ctx, SourceLocation *Loc = nullptr) const;



class Classification {
public:

enum Kinds {
CL_LValue,
CL_XValue,
CL_Function,
CL_Void,
CL_AddressableVoid,
CL_DuplicateVectorComponents,
CL_MemberFunction,
CL_SubObjCPropertySetting,
CL_ClassTemporary,
CL_ArrayTemporary,
CL_ObjCMessageRValue,
CL_PRValue
};

enum ModifiableType {
CM_Untested,
CM_Modifiable,
CM_RValue,
CM_Function,
CM_LValueCast,
CM_NoSetterProperty,
CM_ConstQualified,
CM_ConstQualifiedField,
CM_ConstAddrSpace,
CM_ArrayType,
CM_IncompleteType
};

private:
friend class Expr;

unsigned short Kind;
unsigned short Modifiable;

explicit Classification(Kinds k, ModifiableType m)
: Kind(k), Modifiable(m)
{}

public:
Classification() {}

Kinds getKind() const { return static_cast<Kinds>(Kind); }
ModifiableType getModifiable() const {
assert(Modifiable != CM_Untested && "Did not test for modifiability.");
return static_cast<ModifiableType>(Modifiable);
}
bool isLValue() const { return Kind == CL_LValue; }
bool isXValue() const { return Kind == CL_XValue; }
bool isGLValue() const { return Kind <= CL_XValue; }
bool isPRValue() const { return Kind >= CL_Function; }
bool isRValue() const { return Kind >= CL_XValue; }
bool isModifiable() const { return getModifiable() == CM_Modifiable; }


static Classification makeSimpleLValue() {
return Classification(CL_LValue, CM_Modifiable);
}

};












Classification Classify(ASTContext &Ctx) const {
return ClassifyImpl(Ctx, nullptr);
}









Classification ClassifyModifiable(ASTContext &Ctx, SourceLocation &Loc) const{
return ClassifyImpl(Ctx, &Loc);
}



FPOptions getFPFeaturesInEffect(const LangOptions &LO) const;



static ExprValueKind getValueKindForType(QualType T) {
if (const ReferenceType *RT = T->getAs<ReferenceType>())
return (isa<LValueReferenceType>(RT)
? VK_LValue
: (RT->getPointeeType()->isFunctionType()
? VK_LValue : VK_XValue));
return VK_PRValue;
}


ExprValueKind getValueKind() const {
return static_cast<ExprValueKind>(ExprBits.ValueKind);
}




ExprObjectKind getObjectKind() const {
return static_cast<ExprObjectKind>(ExprBits.ObjectKind);
}

bool isOrdinaryOrBitFieldObject() const {
ExprObjectKind OK = getObjectKind();
return (OK == OK_Ordinary || OK == OK_BitField);
}


void setValueKind(ExprValueKind Cat) { ExprBits.ValueKind = Cat; }


void setObjectKind(ExprObjectKind Cat) { ExprBits.ObjectKind = Cat; }

private:
Classification ClassifyImpl(ASTContext &Ctx, SourceLocation *Loc) const;

public:






bool refersToBitField() const { return getObjectKind() == OK_BitField; }








FieldDecl *getSourceBitField();

const FieldDecl *getSourceBitField() const {
return const_cast<Expr*>(this)->getSourceBitField();
}

Decl *getReferencedDeclOfCallee();
const Decl *getReferencedDeclOfCallee() const {
return const_cast<Expr*>(this)->getReferencedDeclOfCallee();
}



const ObjCPropertyRefExpr *getObjCProperty() const;


bool isObjCSelfExpr() const;


bool refersToVectorElement() const;


bool refersToMatrixElement() const {
return getObjectKind() == OK_MatrixComponent;
}



bool refersToGlobalRegisterVar() const;


bool hasPlaceholderType() const {
return getType()->isPlaceholderType();
}


bool hasPlaceholderType(BuiltinType::Kind K) const {
assert(BuiltinType::isPlaceholderTypeKind(K));
if (const BuiltinType *BT = dyn_cast<BuiltinType>(getType()))
return BT->getKind() == K;
return false;
}











bool isKnownToHaveBooleanValue(bool Semantic = true) const;







Optional<llvm::APSInt> getIntegerConstantExpr(const ASTContext &Ctx,
SourceLocation *Loc = nullptr,
bool isEvaluated = true) const;
bool isIntegerConstantExpr(const ASTContext &Ctx,
SourceLocation *Loc = nullptr) const;



bool isCXX98IntegralConstantExpr(const ASTContext &Ctx) const;






bool isCXX11ConstantExpr(const ASTContext &Ctx, APValue *Result = nullptr,
SourceLocation *Loc = nullptr) const;





static bool isPotentialConstantExpr(const FunctionDecl *FD,
SmallVectorImpl<
PartialDiagnosticAt> &Diags);






static bool isPotentialConstantExprUnevaluated(Expr *E,
const FunctionDecl *FD,
SmallVectorImpl<
PartialDiagnosticAt> &Diags);





bool isConstantInitializer(ASTContext &Ctx, bool ForRef,
const Expr **Culprit = nullptr) const;


struct EvalStatus {


bool HasSideEffects;




bool HasUndefinedBehavior;








SmallVectorImpl<PartialDiagnosticAt> *Diag;

EvalStatus()
: HasSideEffects(false), HasUndefinedBehavior(false), Diag(nullptr) {}



bool hasSideEffects() const {
return HasSideEffects;
}
};


struct EvalResult : EvalStatus {

APValue Val;



bool isGlobalLValue() const;
};







bool EvaluateAsRValue(EvalResult &Result, const ASTContext &Ctx,
bool InConstantContext = false) const;





bool EvaluateAsBooleanCondition(bool &Result, const ASTContext &Ctx,
bool InConstantContext = false) const;

enum SideEffectsKind {
SE_NoSideEffects,
SE_AllowUndefinedBehavior,

SE_AllowSideEffects
};



bool EvaluateAsInt(EvalResult &Result, const ASTContext &Ctx,
SideEffectsKind AllowSideEffects = SE_NoSideEffects,
bool InConstantContext = false) const;




bool EvaluateAsFloat(llvm::APFloat &Result, const ASTContext &Ctx,
SideEffectsKind AllowSideEffects = SE_NoSideEffects,
bool InConstantContext = false) const;



bool EvaluateAsFixedPoint(EvalResult &Result, const ASTContext &Ctx,
SideEffectsKind AllowSideEffects = SE_NoSideEffects,
bool InConstantContext = false) const;



bool isEvaluatable(const ASTContext &Ctx,
SideEffectsKind AllowSideEffects = SE_NoSideEffects) const;








bool HasSideEffects(const ASTContext &Ctx,
bool IncludePossibleEffects = true) const;



bool hasNonTrivialCall(const ASTContext &Ctx) const;




llvm::APSInt EvaluateKnownConstInt(
const ASTContext &Ctx,
SmallVectorImpl<PartialDiagnosticAt> *Diag = nullptr) const;

llvm::APSInt EvaluateKnownConstIntCheckOverflow(
const ASTContext &Ctx,
SmallVectorImpl<PartialDiagnosticAt> *Diag = nullptr) const;

void EvaluateForOverflow(const ASTContext &Ctx) const;



bool EvaluateAsLValue(EvalResult &Result, const ASTContext &Ctx,
bool InConstantContext = false) const;





bool EvaluateAsInitializer(APValue &Result, const ASTContext &Ctx,
const VarDecl *VD,
SmallVectorImpl<PartialDiagnosticAt> &Notes) const;





bool EvaluateWithSubstitution(APValue &Value, ASTContext &Ctx,
const FunctionDecl *Callee,
ArrayRef<const Expr*> Args,
const Expr *This = nullptr) const;

enum class ConstantExprKind {


Normal,


NonClassTemplateArgument,

ClassTemplateArgument,



ImmediateInvocation,
};



bool EvaluateAsConstantExpr(
EvalResult &Result, const ASTContext &Ctx,
ConstantExprKind Kind = ConstantExprKind::Normal) const;








bool tryEvaluateObjectSize(uint64_t &Result, ASTContext &Ctx,
unsigned Type) const;



enum NullPointerConstantKind {

NPCK_NotNull = 0,






NPCK_ZeroExpression,


NPCK_ZeroLiteral,


NPCK_CXX11_nullptr,


NPCK_GNUNull
};



enum NullPointerConstantValueDependence {

NPC_NeverValueDependent = 0,



NPC_ValueDependentIsNull,



NPC_ValueDependentIsNotNull
};




NullPointerConstantKind isNullPointerConstant(
ASTContext &Ctx,
NullPointerConstantValueDependence NPC) const;



bool isOBJCGCCandidate(ASTContext &Ctx) const;


bool isBoundMemberFunction(ASTContext &Ctx) const;




static QualType findBoundMemberType(const Expr *expr);





Expr *IgnoreUnlessSpelledInSource();
const Expr *IgnoreUnlessSpelledInSource() const {
return const_cast<Expr *>(this)->IgnoreUnlessSpelledInSource();
}





Expr *IgnoreImpCasts() LLVM_READONLY;
const Expr *IgnoreImpCasts() const {
return const_cast<Expr *>(this)->IgnoreImpCasts();
}







Expr *IgnoreCasts() LLVM_READONLY;
const Expr *IgnoreCasts() const {
return const_cast<Expr *>(this)->IgnoreCasts();
}






Expr *IgnoreImplicit() LLVM_READONLY;
const Expr *IgnoreImplicit() const {
return const_cast<Expr *>(this)->IgnoreImplicit();
}






Expr *IgnoreImplicitAsWritten() LLVM_READONLY;
const Expr *IgnoreImplicitAsWritten() const {
return const_cast<Expr *>(this)->IgnoreImplicitAsWritten();
}








Expr *IgnoreParens() LLVM_READONLY;
const Expr *IgnoreParens() const {
return const_cast<Expr *>(this)->IgnoreParens();
}










Expr *IgnoreParenImpCasts() LLVM_READONLY;
const Expr *IgnoreParenImpCasts() const {
return const_cast<Expr *>(this)->IgnoreParenImpCasts();
}





Expr *IgnoreParenCasts() LLVM_READONLY;
const Expr *IgnoreParenCasts() const {
return const_cast<Expr *>(this)->IgnoreParenCasts();
}



Expr *IgnoreConversionOperatorSingleStep() LLVM_READONLY;
const Expr *IgnoreConversionOperatorSingleStep() const {
return const_cast<Expr *>(this)->IgnoreConversionOperatorSingleStep();
}









Expr *IgnoreParenLValueCasts() LLVM_READONLY;
const Expr *IgnoreParenLValueCasts() const {
return const_cast<Expr *>(this)->IgnoreParenLValueCasts();
}







Expr *IgnoreParenNoopCasts(const ASTContext &Ctx) LLVM_READONLY;
const Expr *IgnoreParenNoopCasts(const ASTContext &Ctx) const {
return const_cast<Expr *>(this)->IgnoreParenNoopCasts(Ctx);
}






Expr *IgnoreParenBaseCasts() LLVM_READONLY;
const Expr *IgnoreParenBaseCasts() const {
return const_cast<Expr *>(this)->IgnoreParenBaseCasts();
}








bool isDefaultArgument() const;



bool isTemporaryObject(ASTContext &Ctx, const CXXRecordDecl *TempTy) const;


bool isImplicitCXXThis() const;

static bool hasAnyTypeDependentArguments(ArrayRef<Expr *> Exprs);








const CXXRecordDecl *getBestDynamicClassType() const;




const Expr *getBestDynamicClassTypeExpr() const;




const Expr *skipRValueSubobjectAdjustments(
SmallVectorImpl<const Expr *> &CommaLHS,
SmallVectorImpl<SubobjectAdjustment> &Adjustments) const;
const Expr *skipRValueSubobjectAdjustments() const {
SmallVector<const Expr *, 8> CommaLHSs;
SmallVector<SubobjectAdjustment, 8> Adjustments;
return skipRValueSubobjectAdjustments(CommaLHSs, Adjustments);
}




static bool isSameComparisonOperand(const Expr* E1, const Expr* E2);

static bool classof(const Stmt *T) {
return T->getStmtClass() >= firstExprConstant &&
T->getStmtClass() <= lastExprConstant;
}
};


static_assert(llvm::PointerLikeTypeTraits<Expr *>::NumLowBitsAvailable <=
llvm::detail::ConstantLog2<alignof(Expr)>::value,
"PointerLikeTypeTraits<Expr*> assumes too much alignment.");

using ConstantExprKind = Expr::ConstantExprKind;






class FullExpr : public Expr {
protected:
Stmt *SubExpr;

FullExpr(StmtClass SC, Expr *subexpr)
: Expr(SC, subexpr->getType(), subexpr->getValueKind(),
subexpr->getObjectKind()),
SubExpr(subexpr) {
setDependence(computeDependence(this));
}
FullExpr(StmtClass SC, EmptyShell Empty)
: Expr(SC, Empty) {}
public:
const Expr *getSubExpr() const { return cast<Expr>(SubExpr); }
Expr *getSubExpr() { return cast<Expr>(SubExpr); }



void setSubExpr(Expr *E) { SubExpr = E; }

static bool classof(const Stmt *T) {
return T->getStmtClass() >= firstFullExprConstant &&
T->getStmtClass() <= lastFullExprConstant;
}
};



class ConstantExpr final
: public FullExpr,
private llvm::TrailingObjects<ConstantExpr, APValue, uint64_t> {
static_assert(std::is_same<uint64_t, llvm::APInt::WordType>::value,
"ConstantExpr assumes that llvm::APInt::WordType is uint64_t "
"for tail-allocated storage");
friend TrailingObjects;
friend class ASTStmtReader;
friend class ASTStmtWriter;

public:

enum ResultStorageKind { RSK_None, RSK_Int64, RSK_APValue };

private:
size_t numTrailingObjects(OverloadToken<APValue>) const {
return ConstantExprBits.ResultKind == ConstantExpr::RSK_APValue;
}
size_t numTrailingObjects(OverloadToken<uint64_t>) const {
return ConstantExprBits.ResultKind == ConstantExpr::RSK_Int64;
}

uint64_t &Int64Result() {
assert(ConstantExprBits.ResultKind == ConstantExpr::RSK_Int64 &&
"invalid accessor");
return *getTrailingObjects<uint64_t>();
}
const uint64_t &Int64Result() const {
return const_cast<ConstantExpr *>(this)->Int64Result();
}
APValue &APValueResult() {
assert(ConstantExprBits.ResultKind == ConstantExpr::RSK_APValue &&
"invalid accessor");
return *getTrailingObjects<APValue>();
}
APValue &APValueResult() const {
return const_cast<ConstantExpr *>(this)->APValueResult();
}

ConstantExpr(Expr *SubExpr, ResultStorageKind StorageKind,
bool IsImmediateInvocation);
ConstantExpr(EmptyShell Empty, ResultStorageKind StorageKind);

public:
static ConstantExpr *Create(const ASTContext &Context, Expr *E,
const APValue &Result);
static ConstantExpr *Create(const ASTContext &Context, Expr *E,
ResultStorageKind Storage = RSK_None,
bool IsImmediateInvocation = false);
static ConstantExpr *CreateEmpty(const ASTContext &Context,
ResultStorageKind StorageKind);

static ResultStorageKind getStorageKind(const APValue &Value);
static ResultStorageKind getStorageKind(const Type *T,
const ASTContext &Context);

SourceLocation getBeginLoc() const LLVM_READONLY {
return SubExpr->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return SubExpr->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ConstantExprClass;
}

void SetResult(APValue Value, const ASTContext &Context) {
MoveIntoResult(Value, Context);
}
void MoveIntoResult(APValue &Value, const ASTContext &Context);

APValue::ValueKind getResultAPValueKind() const {
return static_cast<APValue::ValueKind>(ConstantExprBits.APValueKind);
}
ResultStorageKind getResultStorageKind() const {
return static_cast<ResultStorageKind>(ConstantExprBits.ResultKind);
}
bool isImmediateInvocation() const {
return ConstantExprBits.IsImmediateInvocation;
}
bool hasAPValueResult() const {
return ConstantExprBits.APValueKind != APValue::None;
}
APValue getAPValueResult() const;
APValue &getResultAsAPValue() const { return APValueResult(); }
llvm::APSInt getResultAsAPSInt() const;

child_range children() { return child_range(&SubExpr, &SubExpr+1); }
const_child_range children() const {
return const_child_range(&SubExpr, &SubExpr + 1);
}
};










class OpaqueValueExpr : public Expr {
friend class ASTStmtReader;
Expr *SourceExpr;

public:
OpaqueValueExpr(SourceLocation Loc, QualType T, ExprValueKind VK,
ExprObjectKind OK = OK_Ordinary, Expr *SourceExpr = nullptr)
: Expr(OpaqueValueExprClass, T, VK, OK), SourceExpr(SourceExpr) {
setIsUnique(false);
OpaqueValueExprBits.Loc = Loc;
setDependence(computeDependence(this));
}




static const OpaqueValueExpr *findInCopyConstruct(const Expr *expr);

explicit OpaqueValueExpr(EmptyShell Empty)
: Expr(OpaqueValueExprClass, Empty) {}


SourceLocation getLocation() const { return OpaqueValueExprBits.Loc; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return SourceExpr ? SourceExpr->getBeginLoc() : getLocation();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return SourceExpr ? SourceExpr->getEndLoc() : getLocation();
}
SourceLocation getExprLoc() const LLVM_READONLY {
return SourceExpr ? SourceExpr->getExprLoc() : getLocation();
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}









Expr *getSourceExpr() const { return SourceExpr; }

void setIsUnique(bool V) {
assert((!V || SourceExpr) &&
"unique OVEs are expected to have source expressions");
OpaqueValueExprBits.IsUnique = V;
}

bool isUnique() const { return OpaqueValueExprBits.IsUnique; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OpaqueValueExprClass;
}
};
























class DeclRefExpr final
: public Expr,
private llvm::TrailingObjects<DeclRefExpr, NestedNameSpecifierLoc,
NamedDecl *, ASTTemplateKWAndArgsInfo,
TemplateArgumentLoc> {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;


ValueDecl *D;



DeclarationNameLoc DNLoc;

size_t numTrailingObjects(OverloadToken<NestedNameSpecifierLoc>) const {
return hasQualifier();
}

size_t numTrailingObjects(OverloadToken<NamedDecl *>) const {
return hasFoundDecl();
}

size_t numTrailingObjects(OverloadToken<ASTTemplateKWAndArgsInfo>) const {
return hasTemplateKWAndArgsInfo();
}



bool hasFoundDecl() const { return DeclRefExprBits.HasFoundDecl; }

DeclRefExpr(const ASTContext &Ctx, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc, ValueDecl *D,
bool RefersToEnlosingVariableOrCapture,
const DeclarationNameInfo &NameInfo, NamedDecl *FoundD,
const TemplateArgumentListInfo *TemplateArgs, QualType T,
ExprValueKind VK, NonOdrUseReason NOUR);


explicit DeclRefExpr(EmptyShell Empty) : Expr(DeclRefExprClass, Empty) {}

public:
DeclRefExpr(const ASTContext &Ctx, ValueDecl *D,
bool RefersToEnclosingVariableOrCapture, QualType T,
ExprValueKind VK, SourceLocation L,
const DeclarationNameLoc &LocInfo = DeclarationNameLoc(),
NonOdrUseReason NOUR = NOUR_None);

static DeclRefExpr *
Create(const ASTContext &Context, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc, ValueDecl *D,
bool RefersToEnclosingVariableOrCapture, SourceLocation NameLoc,
QualType T, ExprValueKind VK, NamedDecl *FoundD = nullptr,
const TemplateArgumentListInfo *TemplateArgs = nullptr,
NonOdrUseReason NOUR = NOUR_None);

static DeclRefExpr *
Create(const ASTContext &Context, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc, ValueDecl *D,
bool RefersToEnclosingVariableOrCapture,
const DeclarationNameInfo &NameInfo, QualType T, ExprValueKind VK,
NamedDecl *FoundD = nullptr,
const TemplateArgumentListInfo *TemplateArgs = nullptr,
NonOdrUseReason NOUR = NOUR_None);


static DeclRefExpr *CreateEmpty(const ASTContext &Context, bool HasQualifier,
bool HasFoundDecl,
bool HasTemplateKWAndArgsInfo,
unsigned NumTemplateArgs);

ValueDecl *getDecl() { return D; }
const ValueDecl *getDecl() const { return D; }
void setDecl(ValueDecl *NewD);

DeclarationNameInfo getNameInfo() const {
return DeclarationNameInfo(getDecl()->getDeclName(), getLocation(), DNLoc);
}

SourceLocation getLocation() const { return DeclRefExprBits.Loc; }
void setLocation(SourceLocation L) { DeclRefExprBits.Loc = L; }
SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;



bool hasQualifier() const { return DeclRefExprBits.HasQualifier; }



NestedNameSpecifierLoc getQualifierLoc() const {
if (!hasQualifier())
return NestedNameSpecifierLoc();
return *getTrailingObjects<NestedNameSpecifierLoc>();
}



NestedNameSpecifier *getQualifier() const {
return getQualifierLoc().getNestedNameSpecifier();
}







NamedDecl *getFoundDecl() {
return hasFoundDecl() ? *getTrailingObjects<NamedDecl *>() : D;
}



const NamedDecl *getFoundDecl() const {
return hasFoundDecl() ? *getTrailingObjects<NamedDecl *>() : D;
}

bool hasTemplateKWAndArgsInfo() const {
return DeclRefExprBits.HasTemplateKWAndArgsInfo;
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



bool hadMultipleCandidates() const {
return DeclRefExprBits.HadMultipleCandidates;
}



void setHadMultipleCandidates(bool V = true) {
DeclRefExprBits.HadMultipleCandidates = V;
}


NonOdrUseReason isNonOdrUse() const {
return static_cast<NonOdrUseReason>(DeclRefExprBits.NonOdrUseReason);
}



bool refersToEnclosingVariableOrCapture() const {
return DeclRefExprBits.RefersToEnclosingVariableOrCapture;
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == DeclRefExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};









class APNumericStorage {
union {
uint64_t VAL;
uint64_t *pVal;
};
unsigned BitWidth;

bool hasAllocation() const { return llvm::APInt::getNumWords(BitWidth) > 1; }

APNumericStorage(const APNumericStorage &) = delete;
void operator=(const APNumericStorage &) = delete;

protected:
APNumericStorage() : VAL(0), BitWidth(0) { }

llvm::APInt getIntValue() const {
unsigned NumWords = llvm::APInt::getNumWords(BitWidth);
if (NumWords > 1)
return llvm::APInt(BitWidth, NumWords, pVal);
else
return llvm::APInt(BitWidth, VAL);
}
void setIntValue(const ASTContext &C, const llvm::APInt &Val);
};

class APIntStorage : private APNumericStorage {
public:
llvm::APInt getValue() const { return getIntValue(); }
void setValue(const ASTContext &C, const llvm::APInt &Val) {
setIntValue(C, Val);
}
};

class APFloatStorage : private APNumericStorage {
public:
llvm::APFloat getValue(const llvm::fltSemantics &Semantics) const {
return llvm::APFloat(Semantics, getIntValue());
}
void setValue(const ASTContext &C, const llvm::APFloat &Val) {
setIntValue(C, Val.bitcastToAPInt());
}
};

class IntegerLiteral : public Expr, public APIntStorage {
SourceLocation Loc;


explicit IntegerLiteral(EmptyShell Empty)
: Expr(IntegerLiteralClass, Empty) { }

public:


IntegerLiteral(const ASTContext &C, const llvm::APInt &V, QualType type,
SourceLocation l);





static IntegerLiteral *Create(const ASTContext &C, const llvm::APInt &V,
QualType type, SourceLocation l);

static IntegerLiteral *Create(const ASTContext &C, EmptyShell Empty);

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return Loc; }


SourceLocation getLocation() const { return Loc; }

void setLocation(SourceLocation Location) { Loc = Location; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == IntegerLiteralClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};

class FixedPointLiteral : public Expr, public APIntStorage {
SourceLocation Loc;
unsigned Scale;


explicit FixedPointLiteral(EmptyShell Empty)
: Expr(FixedPointLiteralClass, Empty) {}

public:
FixedPointLiteral(const ASTContext &C, const llvm::APInt &V, QualType type,
SourceLocation l, unsigned Scale);


static FixedPointLiteral *CreateFromRawInt(const ASTContext &C,
const llvm::APInt &V,
QualType type, SourceLocation l,
unsigned Scale);


static FixedPointLiteral *Create(const ASTContext &C, EmptyShell Empty);

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return Loc; }


SourceLocation getLocation() const { return Loc; }

void setLocation(SourceLocation Location) { Loc = Location; }

unsigned getScale() const { return Scale; }
void setScale(unsigned S) { Scale = S; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == FixedPointLiteralClass;
}

std::string getValueAsString(unsigned Radix) const;


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};

class CharacterLiteral : public Expr {
public:
enum CharacterKind {
Ascii,
Wide,
UTF8,
UTF16,
UTF32
};

private:
unsigned Value;
SourceLocation Loc;
public:

CharacterLiteral(unsigned value, CharacterKind kind, QualType type,
SourceLocation l)
: Expr(CharacterLiteralClass, type, VK_PRValue, OK_Ordinary),
Value(value), Loc(l) {
CharacterLiteralBits.Kind = kind;
setDependence(ExprDependence::None);
}


CharacterLiteral(EmptyShell Empty) : Expr(CharacterLiteralClass, Empty) { }

SourceLocation getLocation() const { return Loc; }
CharacterKind getKind() const {
return static_cast<CharacterKind>(CharacterLiteralBits.Kind);
}

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return Loc; }

unsigned getValue() const { return Value; }

void setLocation(SourceLocation Location) { Loc = Location; }
void setKind(CharacterKind kind) { CharacterLiteralBits.Kind = kind; }
void setValue(unsigned Val) { Value = Val; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CharacterLiteralClass;
}

static void print(unsigned val, CharacterKind Kind, raw_ostream &OS);


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};

class FloatingLiteral : public Expr, private APFloatStorage {
SourceLocation Loc;

FloatingLiteral(const ASTContext &C, const llvm::APFloat &V, bool isexact,
QualType Type, SourceLocation L);


explicit FloatingLiteral(const ASTContext &C, EmptyShell Empty);

public:
static FloatingLiteral *Create(const ASTContext &C, const llvm::APFloat &V,
bool isexact, QualType Type, SourceLocation L);
static FloatingLiteral *Create(const ASTContext &C, EmptyShell Empty);

llvm::APFloat getValue() const {
return APFloatStorage::getValue(getSemantics());
}
void setValue(const ASTContext &C, const llvm::APFloat &Val) {
assert(&getSemantics() == &Val.getSemantics() && "Inconsistent semantics");
APFloatStorage::setValue(C, Val);
}



llvm::APFloatBase::Semantics getRawSemantics() const {
return static_cast<llvm::APFloatBase::Semantics>(
FloatingLiteralBits.Semantics);
}



void setRawSemantics(llvm::APFloatBase::Semantics Sem) {
FloatingLiteralBits.Semantics = Sem;
}


const llvm::fltSemantics &getSemantics() const {
return llvm::APFloatBase::EnumToSemantics(
static_cast<llvm::APFloatBase::Semantics>(
FloatingLiteralBits.Semantics));
}


void setSemantics(const llvm::fltSemantics &Sem) {
FloatingLiteralBits.Semantics = llvm::APFloatBase::SemanticsToEnum(Sem);
}

bool isExact() const { return FloatingLiteralBits.IsExact; }
void setExact(bool E) { FloatingLiteralBits.IsExact = E; }




double getValueAsApproximateDouble() const;

SourceLocation getLocation() const { return Loc; }
void setLocation(SourceLocation L) { Loc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return Loc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == FloatingLiteralClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};






class ImaginaryLiteral : public Expr {
Stmt *Val;
public:
ImaginaryLiteral(Expr *val, QualType Ty)
: Expr(ImaginaryLiteralClass, Ty, VK_PRValue, OK_Ordinary), Val(val) {
setDependence(ExprDependence::None);
}


explicit ImaginaryLiteral(EmptyShell Empty)
: Expr(ImaginaryLiteralClass, Empty) { }

const Expr *getSubExpr() const { return cast<Expr>(Val); }
Expr *getSubExpr() { return cast<Expr>(Val); }
void setSubExpr(Expr *E) { Val = E; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return Val->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY { return Val->getEndLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ImaginaryLiteralClass;
}


child_range children() { return child_range(&Val, &Val+1); }
const_child_range children() const {
return const_child_range(&Val, &Val + 1);
}
};


















class StringLiteral final
: public Expr,
private llvm::TrailingObjects<StringLiteral, unsigned, SourceLocation,
char> {
friend class ASTStmtReader;
friend TrailingObjects;















public:
enum StringKind { Ascii, Wide, UTF8, UTF16, UTF32 };

private:
unsigned numTrailingObjects(OverloadToken<unsigned>) const { return 1; }
unsigned numTrailingObjects(OverloadToken<SourceLocation>) const {
return getNumConcatenated();
}

unsigned numTrailingObjects(OverloadToken<char>) const {
return getByteLength();
}

char *getStrDataAsChar() { return getTrailingObjects<char>(); }
const char *getStrDataAsChar() const { return getTrailingObjects<char>(); }

const uint16_t *getStrDataAsUInt16() const {
return reinterpret_cast<const uint16_t *>(getTrailingObjects<char>());
}

const uint32_t *getStrDataAsUInt32() const {
return reinterpret_cast<const uint32_t *>(getTrailingObjects<char>());
}


StringLiteral(const ASTContext &Ctx, StringRef Str, StringKind Kind,
bool Pascal, QualType Ty, const SourceLocation *Loc,
unsigned NumConcatenated);


StringLiteral(EmptyShell Empty, unsigned NumConcatenated, unsigned Length,
unsigned CharByteWidth);


static unsigned mapCharByteWidth(TargetInfo const &Target, StringKind SK);


void setStrTokenLoc(unsigned TokNum, SourceLocation L) {
assert(TokNum < getNumConcatenated() && "Invalid tok number");
getTrailingObjects<SourceLocation>()[TokNum] = L;
}

public:


static StringLiteral *Create(const ASTContext &Ctx, StringRef Str,
StringKind Kind, bool Pascal, QualType Ty,
const SourceLocation *Loc,
unsigned NumConcatenated);


static StringLiteral *Create(const ASTContext &Ctx, StringRef Str,
StringKind Kind, bool Pascal, QualType Ty,
SourceLocation Loc) {
return Create(Ctx, Str, Kind, Pascal, Ty, &Loc, 1);
}


static StringLiteral *CreateEmpty(const ASTContext &Ctx,
unsigned NumConcatenated, unsigned Length,
unsigned CharByteWidth);

StringRef getString() const {
assert(getCharByteWidth() == 1 &&
"This function is used in places that assume strings use char");
return StringRef(getStrDataAsChar(), getByteLength());
}



StringRef getBytes() const {

return StringRef(getStrDataAsChar(), getByteLength());
}

void outputString(raw_ostream &OS) const;

uint32_t getCodeUnit(size_t i) const {
assert(i < getLength() && "out of bounds access");
switch (getCharByteWidth()) {
case 1:
return static_cast<unsigned char>(getStrDataAsChar()[i]);
case 2:
return getStrDataAsUInt16()[i];
case 4:
return getStrDataAsUInt32()[i];
}
llvm_unreachable("Unsupported character width!");
}

unsigned getByteLength() const { return getCharByteWidth() * getLength(); }
unsigned getLength() const { return *getTrailingObjects<unsigned>(); }
unsigned getCharByteWidth() const { return StringLiteralBits.CharByteWidth; }

StringKind getKind() const {
return static_cast<StringKind>(StringLiteralBits.Kind);
}

bool isAscii() const { return getKind() == Ascii; }
bool isWide() const { return getKind() == Wide; }
bool isUTF8() const { return getKind() == UTF8; }
bool isUTF16() const { return getKind() == UTF16; }
bool isUTF32() const { return getKind() == UTF32; }
bool isPascal() const { return StringLiteralBits.IsPascal; }

bool containsNonAscii() const {
for (auto c : getString())
if (!isASCII(c))
return true;
return false;
}

bool containsNonAsciiOrNull() const {
for (auto c : getString())
if (!isASCII(c) || !c)
return true;
return false;
}



unsigned getNumConcatenated() const {
return StringLiteralBits.NumConcatenated;
}


SourceLocation getStrTokenLoc(unsigned TokNum) const {
assert(TokNum < getNumConcatenated() && "Invalid tok number");
return getTrailingObjects<SourceLocation>()[TokNum];
}








SourceLocation
getLocationOfByte(unsigned ByteNo, const SourceManager &SM,
const LangOptions &Features, const TargetInfo &Target,
unsigned *StartToken = nullptr,
unsigned *StartTokenByteOffset = nullptr) const;

typedef const SourceLocation *tokloc_iterator;

tokloc_iterator tokloc_begin() const {
return getTrailingObjects<SourceLocation>();
}

tokloc_iterator tokloc_end() const {
return getTrailingObjects<SourceLocation>() + getNumConcatenated();
}

SourceLocation getBeginLoc() const LLVM_READONLY { return *tokloc_begin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return *(tokloc_end() - 1); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == StringLiteralClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};


class PredefinedExpr final
: public Expr,
private llvm::TrailingObjects<PredefinedExpr, Stmt *> {
friend class ASTStmtReader;
friend TrailingObjects;





public:
enum IdentKind {
Func,
Function,
LFunction,
FuncDName,
FuncSig,
LFuncSig,
PrettyFunction,


PrettyFunctionNoVirtual
};

private:
PredefinedExpr(SourceLocation L, QualType FNTy, IdentKind IK,
StringLiteral *SL);

explicit PredefinedExpr(EmptyShell Empty, bool HasFunctionName);


bool hasFunctionName() const { return PredefinedExprBits.HasFunctionName; }

void setFunctionName(StringLiteral *SL) {
assert(hasFunctionName() &&
"This PredefinedExpr has no storage for a function name!");
*getTrailingObjects<Stmt *>() = SL;
}

public:

static PredefinedExpr *Create(const ASTContext &Ctx, SourceLocation L,
QualType FNTy, IdentKind IK, StringLiteral *SL);


static PredefinedExpr *CreateEmpty(const ASTContext &Ctx,
bool HasFunctionName);

IdentKind getIdentKind() const {
return static_cast<IdentKind>(PredefinedExprBits.Kind);
}

SourceLocation getLocation() const { return PredefinedExprBits.Loc; }
void setLocation(SourceLocation L) { PredefinedExprBits.Loc = L; }

StringLiteral *getFunctionName() {
return hasFunctionName()
? static_cast<StringLiteral *>(*getTrailingObjects<Stmt *>())
: nullptr;
}

const StringLiteral *getFunctionName() const {
return hasFunctionName()
? static_cast<StringLiteral *>(*getTrailingObjects<Stmt *>())
: nullptr;
}

static StringRef getIdentKindName(IdentKind IK);
StringRef getIdentKindName() const {
return getIdentKindName(getIdentKind());
}

static std::string ComputeName(IdentKind IK, const Decl *CurrentDecl);

SourceLocation getBeginLoc() const { return getLocation(); }
SourceLocation getEndLoc() const { return getLocation(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == PredefinedExprClass;
}


child_range children() {
return child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() + hasFunctionName());
}

const_child_range children() const {
return const_child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() + hasFunctionName());
}
};





class SYCLUniqueStableNameExpr final : public Expr {
friend class ASTStmtReader;
SourceLocation OpLoc, LParen, RParen;
TypeSourceInfo *TypeInfo;

SYCLUniqueStableNameExpr(EmptyShell Empty, QualType ResultTy);
SYCLUniqueStableNameExpr(SourceLocation OpLoc, SourceLocation LParen,
SourceLocation RParen, QualType ResultTy,
TypeSourceInfo *TSI);

void setTypeSourceInfo(TypeSourceInfo *Ty) { TypeInfo = Ty; }

void setLocation(SourceLocation L) { OpLoc = L; }
void setLParenLocation(SourceLocation L) { LParen = L; }
void setRParenLocation(SourceLocation L) { RParen = L; }

public:
TypeSourceInfo *getTypeSourceInfo() { return TypeInfo; }

const TypeSourceInfo *getTypeSourceInfo() const { return TypeInfo; }

static SYCLUniqueStableNameExpr *
Create(const ASTContext &Ctx, SourceLocation OpLoc, SourceLocation LParen,
SourceLocation RParen, TypeSourceInfo *TSI);

static SYCLUniqueStableNameExpr *CreateEmpty(const ASTContext &Ctx);

SourceLocation getBeginLoc() const { return getLocation(); }
SourceLocation getEndLoc() const { return RParen; }
SourceLocation getLocation() const { return OpLoc; }
SourceLocation getLParenLocation() const { return LParen; }
SourceLocation getRParenLocation() const { return RParen; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == SYCLUniqueStableNameExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}


std::string ComputeName(ASTContext &Context) const;



static std::string ComputeName(ASTContext &Context, QualType Ty);
};



class ParenExpr : public Expr {
SourceLocation L, R;
Stmt *Val;
public:
ParenExpr(SourceLocation l, SourceLocation r, Expr *val)
: Expr(ParenExprClass, val->getType(), val->getValueKind(),
val->getObjectKind()),
L(l), R(r), Val(val) {
setDependence(computeDependence(this));
}


explicit ParenExpr(EmptyShell Empty)
: Expr(ParenExprClass, Empty) { }

const Expr *getSubExpr() const { return cast<Expr>(Val); }
Expr *getSubExpr() { return cast<Expr>(Val); }
void setSubExpr(Expr *E) { Val = E; }

SourceLocation getBeginLoc() const LLVM_READONLY { return L; }
SourceLocation getEndLoc() const LLVM_READONLY { return R; }


SourceLocation getLParen() const { return L; }
void setLParen(SourceLocation Loc) { L = Loc; }


SourceLocation getRParen() const { return R; }
void setRParen(SourceLocation Loc) { R = Loc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ParenExprClass;
}


child_range children() { return child_range(&Val, &Val+1); }
const_child_range children() const {
return const_child_range(&Val, &Val + 1);
}
};











class UnaryOperator final
: public Expr,
private llvm::TrailingObjects<UnaryOperator, FPOptionsOverride> {
Stmt *Val;

size_t numTrailingObjects(OverloadToken<FPOptionsOverride>) const {
return UnaryOperatorBits.HasFPFeatures ? 1 : 0;
}

FPOptionsOverride &getTrailingFPFeatures() {
assert(UnaryOperatorBits.HasFPFeatures);
return *getTrailingObjects<FPOptionsOverride>();
}

const FPOptionsOverride &getTrailingFPFeatures() const {
assert(UnaryOperatorBits.HasFPFeatures);
return *getTrailingObjects<FPOptionsOverride>();
}

public:
typedef UnaryOperatorKind Opcode;

protected:
UnaryOperator(const ASTContext &Ctx, Expr *input, Opcode opc, QualType type,
ExprValueKind VK, ExprObjectKind OK, SourceLocation l,
bool CanOverflow, FPOptionsOverride FPFeatures);


explicit UnaryOperator(bool HasFPFeatures, EmptyShell Empty)
: Expr(UnaryOperatorClass, Empty) {
UnaryOperatorBits.Opc = UO_AddrOf;
UnaryOperatorBits.HasFPFeatures = HasFPFeatures;
}

public:
static UnaryOperator *CreateEmpty(const ASTContext &C, bool hasFPFeatures);

static UnaryOperator *Create(const ASTContext &C, Expr *input, Opcode opc,
QualType type, ExprValueKind VK,
ExprObjectKind OK, SourceLocation l,
bool CanOverflow, FPOptionsOverride FPFeatures);

Opcode getOpcode() const {
return static_cast<Opcode>(UnaryOperatorBits.Opc);
}
void setOpcode(Opcode Opc) { UnaryOperatorBits.Opc = Opc; }

Expr *getSubExpr() const { return cast<Expr>(Val); }
void setSubExpr(Expr *E) { Val = E; }


SourceLocation getOperatorLoc() const { return UnaryOperatorBits.Loc; }
void setOperatorLoc(SourceLocation L) { UnaryOperatorBits.Loc = L; }







bool canOverflow() const { return UnaryOperatorBits.CanOverflow; }
void setCanOverflow(bool C) { UnaryOperatorBits.CanOverflow = C; }



bool isFPContractableWithinStatement(const LangOptions &LO) const {
return getFPFeaturesInEffect(LO).allowFPContractWithinStatement();
}



bool isFEnvAccessOn(const LangOptions &LO) const {
return getFPFeaturesInEffect(LO).getAllowFEnvAccess();
}


static bool isPostfix(Opcode Op) {
return Op == UO_PostInc || Op == UO_PostDec;
}


static bool isPrefix(Opcode Op) {
return Op == UO_PreInc || Op == UO_PreDec;
}

bool isPrefix() const { return isPrefix(getOpcode()); }
bool isPostfix() const { return isPostfix(getOpcode()); }

static bool isIncrementOp(Opcode Op) {
return Op == UO_PreInc || Op == UO_PostInc;
}
bool isIncrementOp() const {
return isIncrementOp(getOpcode());
}

static bool isDecrementOp(Opcode Op) {
return Op == UO_PreDec || Op == UO_PostDec;
}
bool isDecrementOp() const {
return isDecrementOp(getOpcode());
}

static bool isIncrementDecrementOp(Opcode Op) { return Op <= UO_PreDec; }
bool isIncrementDecrementOp() const {
return isIncrementDecrementOp(getOpcode());
}

static bool isArithmeticOp(Opcode Op) {
return Op >= UO_Plus && Op <= UO_LNot;
}
bool isArithmeticOp() const { return isArithmeticOp(getOpcode()); }



static StringRef getOpcodeStr(Opcode Op);



static Opcode getOverloadedOpcode(OverloadedOperatorKind OO, bool Postfix);



static OverloadedOperatorKind getOverloadedOperator(Opcode Opc);

SourceLocation getBeginLoc() const LLVM_READONLY {
return isPostfix() ? Val->getBeginLoc() : getOperatorLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return isPostfix() ? getOperatorLoc() : Val->getEndLoc();
}
SourceLocation getExprLoc() const { return getOperatorLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == UnaryOperatorClass;
}


child_range children() { return child_range(&Val, &Val+1); }
const_child_range children() const {
return const_child_range(&Val, &Val + 1);
}


bool hasStoredFPFeatures() const { return UnaryOperatorBits.HasFPFeatures; }


FPOptionsOverride getStoredFPFeatures() const {
return getTrailingFPFeatures();
}

protected:

void setStoredFPFeatures(FPOptionsOverride F) { getTrailingFPFeatures() = F; }

public:


FPOptions getFPFeaturesInEffect(const LangOptions &LO) const {
if (UnaryOperatorBits.HasFPFeatures)
return getStoredFPFeatures().applyOverrides(LO);
return FPOptions::defaultWithoutTrailingStorage(LO);
}
FPOptionsOverride getFPOptionsOverride() const {
if (UnaryOperatorBits.HasFPFeatures)
return getStoredFPFeatures();
return FPOptionsOverride();
}

friend TrailingObjects;
friend class ASTReader;
friend class ASTStmtReader;
friend class ASTStmtWriter;
};




class OffsetOfNode {
public:

enum Kind {

Array = 0x00,

Field = 0x01,

Identifier = 0x02,


Base = 0x03
};

private:
enum { MaskBits = 2, Mask = 0x03 };


SourceRange Range;










uintptr_t Data;

public:

OffsetOfNode(SourceLocation LBracketLoc, unsigned Index,
SourceLocation RBracketLoc)
: Range(LBracketLoc, RBracketLoc), Data((Index << 2) | Array) {}


OffsetOfNode(SourceLocation DotLoc, FieldDecl *Field, SourceLocation NameLoc)
: Range(DotLoc.isValid() ? DotLoc : NameLoc, NameLoc),
Data(reinterpret_cast<uintptr_t>(Field) | OffsetOfNode::Field) {}


OffsetOfNode(SourceLocation DotLoc, IdentifierInfo *Name,
SourceLocation NameLoc)
: Range(DotLoc.isValid() ? DotLoc : NameLoc, NameLoc),
Data(reinterpret_cast<uintptr_t>(Name) | Identifier) {}


explicit OffsetOfNode(const CXXBaseSpecifier *Base)
: Range(), Data(reinterpret_cast<uintptr_t>(Base) | OffsetOfNode::Base) {}


Kind getKind() const { return static_cast<Kind>(Data & Mask); }



unsigned getArrayExprIndex() const {
assert(getKind() == Array);
return Data >> 2;
}


FieldDecl *getField() const {
assert(getKind() == Field);
return reinterpret_cast<FieldDecl *>(Data & ~(uintptr_t)Mask);
}



IdentifierInfo *getFieldName() const;


CXXBaseSpecifier *getBase() const {
assert(getKind() == Base);
return reinterpret_cast<CXXBaseSpecifier *>(Data & ~(uintptr_t)Mask);
}







SourceRange getSourceRange() const LLVM_READONLY { return Range; }
SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }
};















class OffsetOfExpr final
: public Expr,
private llvm::TrailingObjects<OffsetOfExpr, OffsetOfNode, Expr *> {
SourceLocation OperatorLoc, RParenLoc;

TypeSourceInfo *TSInfo;

unsigned NumComps;

unsigned NumExprs;

size_t numTrailingObjects(OverloadToken<OffsetOfNode>) const {
return NumComps;
}

OffsetOfExpr(const ASTContext &C, QualType type,
SourceLocation OperatorLoc, TypeSourceInfo *tsi,
ArrayRef<OffsetOfNode> comps, ArrayRef<Expr*> exprs,
SourceLocation RParenLoc);

explicit OffsetOfExpr(unsigned numComps, unsigned numExprs)
: Expr(OffsetOfExprClass, EmptyShell()),
TSInfo(nullptr), NumComps(numComps), NumExprs(numExprs) {}

public:

static OffsetOfExpr *Create(const ASTContext &C, QualType type,
SourceLocation OperatorLoc, TypeSourceInfo *tsi,
ArrayRef<OffsetOfNode> comps,
ArrayRef<Expr*> exprs, SourceLocation RParenLoc);

static OffsetOfExpr *CreateEmpty(const ASTContext &C,
unsigned NumComps, unsigned NumExprs);


SourceLocation getOperatorLoc() const { return OperatorLoc; }
void setOperatorLoc(SourceLocation L) { OperatorLoc = L; }


SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation R) { RParenLoc = R; }

TypeSourceInfo *getTypeSourceInfo() const {
return TSInfo;
}
void setTypeSourceInfo(TypeSourceInfo *tsi) {
TSInfo = tsi;
}

const OffsetOfNode &getComponent(unsigned Idx) const {
assert(Idx < NumComps && "Subscript out of range");
return getTrailingObjects<OffsetOfNode>()[Idx];
}

void setComponent(unsigned Idx, OffsetOfNode ON) {
assert(Idx < NumComps && "Subscript out of range");
getTrailingObjects<OffsetOfNode>()[Idx] = ON;
}

unsigned getNumComponents() const {
return NumComps;
}

Expr* getIndexExpr(unsigned Idx) {
assert(Idx < NumExprs && "Subscript out of range");
return getTrailingObjects<Expr *>()[Idx];
}

const Expr *getIndexExpr(unsigned Idx) const {
assert(Idx < NumExprs && "Subscript out of range");
return getTrailingObjects<Expr *>()[Idx];
}

void setIndexExpr(unsigned Idx, Expr* E) {
assert(Idx < NumComps && "Subscript out of range");
getTrailingObjects<Expr *>()[Idx] = E;
}

unsigned getNumExpressions() const {
return NumExprs;
}

SourceLocation getBeginLoc() const LLVM_READONLY { return OperatorLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OffsetOfExprClass;
}


child_range children() {
Stmt **begin = reinterpret_cast<Stmt **>(getTrailingObjects<Expr *>());
return child_range(begin, begin + NumExprs);
}
const_child_range children() const {
Stmt *const *begin =
reinterpret_cast<Stmt *const *>(getTrailingObjects<Expr *>());
return const_child_range(begin, begin + NumExprs);
}
friend TrailingObjects;
};




class UnaryExprOrTypeTraitExpr : public Expr {
union {
TypeSourceInfo *Ty;
Stmt *Ex;
} Argument;
SourceLocation OpLoc, RParenLoc;

public:
UnaryExprOrTypeTraitExpr(UnaryExprOrTypeTrait ExprKind, TypeSourceInfo *TInfo,
QualType resultType, SourceLocation op,
SourceLocation rp)
: Expr(UnaryExprOrTypeTraitExprClass, resultType, VK_PRValue,
OK_Ordinary),
OpLoc(op), RParenLoc(rp) {
assert(ExprKind <= UETT_Last && "invalid enum value!");
UnaryExprOrTypeTraitExprBits.Kind = ExprKind;
assert(static_cast<unsigned>(ExprKind) ==
UnaryExprOrTypeTraitExprBits.Kind &&
"UnaryExprOrTypeTraitExprBits.Kind overflow!");
UnaryExprOrTypeTraitExprBits.IsType = true;
Argument.Ty = TInfo;
setDependence(computeDependence(this));
}

UnaryExprOrTypeTraitExpr(UnaryExprOrTypeTrait ExprKind, Expr *E,
QualType resultType, SourceLocation op,
SourceLocation rp);


explicit UnaryExprOrTypeTraitExpr(EmptyShell Empty)
: Expr(UnaryExprOrTypeTraitExprClass, Empty) { }

UnaryExprOrTypeTrait getKind() const {
return static_cast<UnaryExprOrTypeTrait>(UnaryExprOrTypeTraitExprBits.Kind);
}
void setKind(UnaryExprOrTypeTrait K) {
assert(K <= UETT_Last && "invalid enum value!");
UnaryExprOrTypeTraitExprBits.Kind = K;
assert(static_cast<unsigned>(K) == UnaryExprOrTypeTraitExprBits.Kind &&
"UnaryExprOrTypeTraitExprBits.Kind overflow!");
}

bool isArgumentType() const { return UnaryExprOrTypeTraitExprBits.IsType; }
QualType getArgumentType() const {
return getArgumentTypeInfo()->getType();
}
TypeSourceInfo *getArgumentTypeInfo() const {
assert(isArgumentType() && "calling getArgumentType() when arg is expr");
return Argument.Ty;
}
Expr *getArgumentExpr() {
assert(!isArgumentType() && "calling getArgumentExpr() when arg is type");
return static_cast<Expr*>(Argument.Ex);
}
const Expr *getArgumentExpr() const {
return const_cast<UnaryExprOrTypeTraitExpr*>(this)->getArgumentExpr();
}

void setArgument(Expr *E) {
Argument.Ex = E;
UnaryExprOrTypeTraitExprBits.IsType = false;
}
void setArgument(TypeSourceInfo *TInfo) {
Argument.Ty = TInfo;
UnaryExprOrTypeTraitExprBits.IsType = true;
}



QualType getTypeOfArgument() const {
return isArgumentType() ? getArgumentType() : getArgumentExpr()->getType();
}

SourceLocation getOperatorLoc() const { return OpLoc; }
void setOperatorLoc(SourceLocation L) { OpLoc = L; }

SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return OpLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == UnaryExprOrTypeTraitExprClass;
}


child_range children();
const_child_range children() const;
};






class ArraySubscriptExpr : public Expr {
enum { LHS, RHS, END_EXPR };
Stmt *SubExprs[END_EXPR];

bool lhsIsBase() const { return getRHS()->getType()->isIntegerType(); }

public:
ArraySubscriptExpr(Expr *lhs, Expr *rhs, QualType t, ExprValueKind VK,
ExprObjectKind OK, SourceLocation rbracketloc)
: Expr(ArraySubscriptExprClass, t, VK, OK) {
SubExprs[LHS] = lhs;
SubExprs[RHS] = rhs;
ArrayOrMatrixSubscriptExprBits.RBracketLoc = rbracketloc;
setDependence(computeDependence(this));
}


explicit ArraySubscriptExpr(EmptyShell Shell)
: Expr(ArraySubscriptExprClass, Shell) { }










Expr *getLHS() { return cast<Expr>(SubExprs[LHS]); }
const Expr *getLHS() const { return cast<Expr>(SubExprs[LHS]); }
void setLHS(Expr *E) { SubExprs[LHS] = E; }

Expr *getRHS() { return cast<Expr>(SubExprs[RHS]); }
const Expr *getRHS() const { return cast<Expr>(SubExprs[RHS]); }
void setRHS(Expr *E) { SubExprs[RHS] = E; }

Expr *getBase() { return lhsIsBase() ? getLHS() : getRHS(); }
const Expr *getBase() const { return lhsIsBase() ? getLHS() : getRHS(); }

Expr *getIdx() { return lhsIsBase() ? getRHS() : getLHS(); }
const Expr *getIdx() const { return lhsIsBase() ? getRHS() : getLHS(); }

SourceLocation getBeginLoc() const LLVM_READONLY {
return getLHS()->getBeginLoc();
}
SourceLocation getEndLoc() const { return getRBracketLoc(); }

SourceLocation getRBracketLoc() const {
return ArrayOrMatrixSubscriptExprBits.RBracketLoc;
}
void setRBracketLoc(SourceLocation L) {
ArrayOrMatrixSubscriptExprBits.RBracketLoc = L;
}

SourceLocation getExprLoc() const LLVM_READONLY {
return getBase()->getExprLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ArraySubscriptExprClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0]+END_EXPR);
}
const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}
};







class MatrixSubscriptExpr : public Expr {
enum { BASE, ROW_IDX, COLUMN_IDX, END_EXPR };
Stmt *SubExprs[END_EXPR];

public:
MatrixSubscriptExpr(Expr *Base, Expr *RowIdx, Expr *ColumnIdx, QualType T,
SourceLocation RBracketLoc)
: Expr(MatrixSubscriptExprClass, T, Base->getValueKind(),
OK_MatrixComponent) {
SubExprs[BASE] = Base;
SubExprs[ROW_IDX] = RowIdx;
SubExprs[COLUMN_IDX] = ColumnIdx;
ArrayOrMatrixSubscriptExprBits.RBracketLoc = RBracketLoc;
setDependence(computeDependence(this));
}


explicit MatrixSubscriptExpr(EmptyShell Shell)
: Expr(MatrixSubscriptExprClass, Shell) {}

bool isIncomplete() const {
bool IsIncomplete = hasPlaceholderType(BuiltinType::IncompleteMatrixIdx);
assert((SubExprs[COLUMN_IDX] || IsIncomplete) &&
"expressions without column index must be marked as incomplete");
return IsIncomplete;
}
Expr *getBase() { return cast<Expr>(SubExprs[BASE]); }
const Expr *getBase() const { return cast<Expr>(SubExprs[BASE]); }
void setBase(Expr *E) { SubExprs[BASE] = E; }

Expr *getRowIdx() { return cast<Expr>(SubExprs[ROW_IDX]); }
const Expr *getRowIdx() const { return cast<Expr>(SubExprs[ROW_IDX]); }
void setRowIdx(Expr *E) { SubExprs[ROW_IDX] = E; }

Expr *getColumnIdx() { return cast_or_null<Expr>(SubExprs[COLUMN_IDX]); }
const Expr *getColumnIdx() const {
assert(!isIncomplete() &&
"cannot get the column index of an incomplete expression");
return cast<Expr>(SubExprs[COLUMN_IDX]);
}
void setColumnIdx(Expr *E) { SubExprs[COLUMN_IDX] = E; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return getBase()->getBeginLoc();
}

SourceLocation getEndLoc() const { return getRBracketLoc(); }

SourceLocation getExprLoc() const LLVM_READONLY {
return getBase()->getExprLoc();
}

SourceLocation getRBracketLoc() const {
return ArrayOrMatrixSubscriptExprBits.RBracketLoc;
}
void setRBracketLoc(SourceLocation L) {
ArrayOrMatrixSubscriptExprBits.RBracketLoc = L;
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == MatrixSubscriptExprClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}
const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}
};







class CallExpr : public Expr {
enum { FN = 0, PREARGS_START = 1 };


unsigned NumArgs;



SourceLocation RParenLoc;

























Stmt **getTrailingStmts() {
return reinterpret_cast<Stmt **>(reinterpret_cast<char *>(this) +
CallExprBits.OffsetToTrailingObjects);
}
Stmt *const *getTrailingStmts() const {
return const_cast<CallExpr *>(this)->getTrailingStmts();
}



static unsigned offsetToTrailingObjects(StmtClass SC);

unsigned getSizeOfTrailingStmts() const {
return (1 + getNumPreArgs() + getNumArgs()) * sizeof(Stmt *);
}

size_t getOffsetOfTrailingFPFeatures() const {
assert(hasStoredFPFeatures());
return CallExprBits.OffsetToTrailingObjects + getSizeOfTrailingStmts();
}

public:
enum class ADLCallKind : bool { NotADL, UsesADL };
static constexpr ADLCallKind NotADL = ADLCallKind::NotADL;
static constexpr ADLCallKind UsesADL = ADLCallKind::UsesADL;

protected:


CallExpr(StmtClass SC, Expr *Fn, ArrayRef<Expr *> PreArgs,
ArrayRef<Expr *> Args, QualType Ty, ExprValueKind VK,
SourceLocation RParenLoc, FPOptionsOverride FPFeatures,
unsigned MinNumArgs, ADLCallKind UsesADL);


CallExpr(StmtClass SC, unsigned NumPreArgs, unsigned NumArgs,
bool hasFPFeatures, EmptyShell Empty);



static unsigned sizeOfTrailingObjects(unsigned NumPreArgs, unsigned NumArgs,
bool HasFPFeatures) {
return (1 + NumPreArgs + NumArgs) * sizeof(Stmt *) +
HasFPFeatures * sizeof(FPOptionsOverride);
}

Stmt *getPreArg(unsigned I) {
assert(I < getNumPreArgs() && "Prearg access out of range!");
return getTrailingStmts()[PREARGS_START + I];
}
const Stmt *getPreArg(unsigned I) const {
assert(I < getNumPreArgs() && "Prearg access out of range!");
return getTrailingStmts()[PREARGS_START + I];
}
void setPreArg(unsigned I, Stmt *PreArg) {
assert(I < getNumPreArgs() && "Prearg access out of range!");
getTrailingStmts()[PREARGS_START + I] = PreArg;
}

unsigned getNumPreArgs() const { return CallExprBits.NumPreArgs; }


FPOptionsOverride *getTrailingFPFeatures() {
assert(hasStoredFPFeatures());
return reinterpret_cast<FPOptionsOverride *>(
reinterpret_cast<char *>(this) + CallExprBits.OffsetToTrailingObjects +
getSizeOfTrailingStmts());
}
const FPOptionsOverride *getTrailingFPFeatures() const {
assert(hasStoredFPFeatures());
return reinterpret_cast<const FPOptionsOverride *>(
reinterpret_cast<const char *>(this) +
CallExprBits.OffsetToTrailingObjects + getSizeOfTrailingStmts());
}

public:


















static CallExpr *Create(const ASTContext &Ctx, Expr *Fn,
ArrayRef<Expr *> Args, QualType Ty, ExprValueKind VK,
SourceLocation RParenLoc,
FPOptionsOverride FPFeatures, unsigned MinNumArgs = 0,
ADLCallKind UsesADL = NotADL);









static CallExpr *CreateTemporary(void *Mem, Expr *Fn, QualType Ty,
ExprValueKind VK, SourceLocation RParenLoc,
ADLCallKind UsesADL = NotADL);


static CallExpr *CreateEmpty(const ASTContext &Ctx, unsigned NumArgs,
bool HasFPFeatures, EmptyShell Empty);

Expr *getCallee() { return cast<Expr>(getTrailingStmts()[FN]); }
const Expr *getCallee() const { return cast<Expr>(getTrailingStmts()[FN]); }
void setCallee(Expr *F) { getTrailingStmts()[FN] = F; }

ADLCallKind getADLCallKind() const {
return static_cast<ADLCallKind>(CallExprBits.UsesADL);
}
void setADLCallKind(ADLCallKind V = UsesADL) {
CallExprBits.UsesADL = static_cast<bool>(V);
}
bool usesADL() const { return getADLCallKind() == UsesADL; }

bool hasStoredFPFeatures() const { return CallExprBits.HasFPFeatures; }

Decl *getCalleeDecl() { return getCallee()->getReferencedDeclOfCallee(); }
const Decl *getCalleeDecl() const {
return getCallee()->getReferencedDeclOfCallee();
}


FunctionDecl *getDirectCallee() {
return dyn_cast_or_null<FunctionDecl>(getCalleeDecl());
}
const FunctionDecl *getDirectCallee() const {
return dyn_cast_or_null<FunctionDecl>(getCalleeDecl());
}


unsigned getNumArgs() const { return NumArgs; }


Expr **getArgs() {
return reinterpret_cast<Expr **>(getTrailingStmts() + PREARGS_START +
getNumPreArgs());
}
const Expr *const *getArgs() const {
return reinterpret_cast<const Expr *const *>(
getTrailingStmts() + PREARGS_START + getNumPreArgs());
}


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


void computeDependence() {
setDependence(clang::computeDependence(
this, llvm::makeArrayRef(
reinterpret_cast<Expr **>(getTrailingStmts() + PREARGS_START),
getNumPreArgs())));
}







void shrinkNumArgs(unsigned NewNumArgs) {
assert((NewNumArgs <= getNumArgs()) &&
"shrinkNumArgs cannot increase the number of arguments!");
NumArgs = NewNumArgs;
}




void setNumArgsUnsafe(unsigned NewNumArgs) { NumArgs = NewNumArgs; }

typedef ExprIterator arg_iterator;
typedef ConstExprIterator const_arg_iterator;
typedef llvm::iterator_range<arg_iterator> arg_range;
typedef llvm::iterator_range<const_arg_iterator> const_arg_range;

arg_range arguments() { return arg_range(arg_begin(), arg_end()); }
const_arg_range arguments() const {
return const_arg_range(arg_begin(), arg_end());
}

arg_iterator arg_begin() {
return getTrailingStmts() + PREARGS_START + getNumPreArgs();
}
arg_iterator arg_end() { return arg_begin() + getNumArgs(); }

const_arg_iterator arg_begin() const {
return getTrailingStmts() + PREARGS_START + getNumPreArgs();
}
const_arg_iterator arg_end() const { return arg_begin() + getNumArgs(); }





ArrayRef<Stmt *> getRawSubExprs() {
return llvm::makeArrayRef(getTrailingStmts(),
PREARGS_START + getNumPreArgs() + getNumArgs());
}



unsigned getNumCommas() const { return getNumArgs() ? getNumArgs() - 1 : 0; }


FPOptionsOverride getStoredFPFeatures() const {
assert(hasStoredFPFeatures());
return *getTrailingFPFeatures();
}

void setStoredFPFeatures(FPOptionsOverride F) {
assert(hasStoredFPFeatures());
*getTrailingFPFeatures() = F;
}



FPOptions getFPFeaturesInEffect(const LangOptions &LO) const {
if (hasStoredFPFeatures())
return getStoredFPFeatures().applyOverrides(LO);
return FPOptions::defaultWithoutTrailingStorage(LO);
}

FPOptionsOverride getFPFeatures() const {
if (hasStoredFPFeatures())
return getStoredFPFeatures();
return FPOptionsOverride();
}



unsigned getBuiltinCallee() const;



bool isUnevaluatedBuiltinCall(const ASTContext &Ctx) const;




QualType getCallReturnType(const ASTContext &Ctx) const;



const Attr *getUnusedResultAttr(const ASTContext &Ctx) const;


bool hasUnusedResultAttr(const ASTContext &Ctx) const {
return getUnusedResultAttr(Ctx) != nullptr;
}

SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;



bool isBuiltinAssumeFalse(const ASTContext &Ctx) const;



void markDependentForPostponedNameLookup() {
setDependence(getDependence() | ExprDependence::TypeValueInstantiation);
}

bool isCallToStdMove() const {
const FunctionDecl *FD = getDirectCallee();
return getNumArgs() == 1 && FD && FD->isInStdNamespace() &&
FD->getIdentifier() && FD->getIdentifier()->isStr("move");
}

static bool classof(const Stmt *T) {
return T->getStmtClass() >= firstCallExprConstant &&
T->getStmtClass() <= lastCallExprConstant;
}


child_range children() {
return child_range(getTrailingStmts(), getTrailingStmts() + PREARGS_START +
getNumPreArgs() + getNumArgs());
}

const_child_range children() const {
return const_child_range(getTrailingStmts(),
getTrailingStmts() + PREARGS_START +
getNumPreArgs() + getNumArgs());
}
};


struct MemberExprNameQualifier {


NestedNameSpecifierLoc QualifierLoc;



DeclAccessPair FoundDecl;
};



class MemberExpr final
: public Expr,
private llvm::TrailingObjects<MemberExpr, MemberExprNameQualifier,
ASTTemplateKWAndArgsInfo,
TemplateArgumentLoc> {
friend class ASTReader;
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;



Stmt *Base;



ValueDecl *MemberDecl;



DeclarationNameLoc MemberDNLoc;


SourceLocation MemberLoc;

size_t numTrailingObjects(OverloadToken<MemberExprNameQualifier>) const {
return hasQualifierOrFoundDecl();
}

size_t numTrailingObjects(OverloadToken<ASTTemplateKWAndArgsInfo>) const {
return hasTemplateKWAndArgsInfo();
}

bool hasQualifierOrFoundDecl() const {
return MemberExprBits.HasQualifierOrFoundDecl;
}

bool hasTemplateKWAndArgsInfo() const {
return MemberExprBits.HasTemplateKWAndArgsInfo;
}

MemberExpr(Expr *Base, bool IsArrow, SourceLocation OperatorLoc,
ValueDecl *MemberDecl, const DeclarationNameInfo &NameInfo,
QualType T, ExprValueKind VK, ExprObjectKind OK,
NonOdrUseReason NOUR);
MemberExpr(EmptyShell Empty)
: Expr(MemberExprClass, Empty), Base(), MemberDecl() {}

public:
static MemberExpr *Create(const ASTContext &C, Expr *Base, bool IsArrow,
SourceLocation OperatorLoc,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateKWLoc, ValueDecl *MemberDecl,
DeclAccessPair FoundDecl,
DeclarationNameInfo MemberNameInfo,
const TemplateArgumentListInfo *TemplateArgs,
QualType T, ExprValueKind VK, ExprObjectKind OK,
NonOdrUseReason NOUR);



static MemberExpr *CreateImplicit(const ASTContext &C, Expr *Base,
bool IsArrow, ValueDecl *MemberDecl,
QualType T, ExprValueKind VK,
ExprObjectKind OK) {
return Create(C, Base, IsArrow, SourceLocation(), NestedNameSpecifierLoc(),
SourceLocation(), MemberDecl,
DeclAccessPair::make(MemberDecl, MemberDecl->getAccess()),
DeclarationNameInfo(), nullptr, T, VK, OK, NOUR_None);
}

static MemberExpr *CreateEmpty(const ASTContext &Context, bool HasQualifier,
bool HasFoundDecl,
bool HasTemplateKWAndArgsInfo,
unsigned NumTemplateArgs);

void setBase(Expr *E) { Base = E; }
Expr *getBase() const { return cast<Expr>(Base); }





ValueDecl *getMemberDecl() const { return MemberDecl; }
void setMemberDecl(ValueDecl *D);


DeclAccessPair getFoundDecl() const {
if (!hasQualifierOrFoundDecl())
return DeclAccessPair::make(getMemberDecl(),
getMemberDecl()->getAccess());
return getTrailingObjects<MemberExprNameQualifier>()->FoundDecl;
}




bool hasQualifier() const { return getQualifier() != nullptr; }




NestedNameSpecifierLoc getQualifierLoc() const {
if (!hasQualifierOrFoundDecl())
return NestedNameSpecifierLoc();
return getTrailingObjects<MemberExprNameQualifier>()->QualifierLoc;
}




NestedNameSpecifier *getQualifier() const {
return getQualifierLoc().getNestedNameSpecifier();
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


DeclarationNameInfo getMemberNameInfo() const {
return DeclarationNameInfo(MemberDecl->getDeclName(),
MemberLoc, MemberDNLoc);
}

SourceLocation getOperatorLoc() const { return MemberExprBits.OperatorLoc; }

bool isArrow() const { return MemberExprBits.IsArrow; }
void setArrow(bool A) { MemberExprBits.IsArrow = A; }



SourceLocation getMemberLoc() const { return MemberLoc; }
void setMemberLoc(SourceLocation L) { MemberLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;

SourceLocation getExprLoc() const LLVM_READONLY { return MemberLoc; }


bool isImplicitAccess() const {
return getBase() && getBase()->isImplicitCXXThis();
}



bool hadMultipleCandidates() const {
return MemberExprBits.HadMultipleCandidates;
}



void setHadMultipleCandidates(bool V = true) {
MemberExprBits.HadMultipleCandidates = V;
}





bool performsVirtualDispatch(const LangOptions &LO) const {
return LO.AppleKext || !hasQualifier();
}



NonOdrUseReason isNonOdrUse() const {
return static_cast<NonOdrUseReason>(MemberExprBits.NonOdrUseReason);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == MemberExprClass;
}


child_range children() { return child_range(&Base, &Base+1); }
const_child_range children() const {
return const_child_range(&Base, &Base + 1);
}
};



class CompoundLiteralExpr : public Expr {



SourceLocation LParenLoc;




llvm::PointerIntPair<TypeSourceInfo *, 1, bool> TInfoAndScope;
Stmt *Init;
public:
CompoundLiteralExpr(SourceLocation lparenloc, TypeSourceInfo *tinfo,
QualType T, ExprValueKind VK, Expr *init, bool fileScope)
: Expr(CompoundLiteralExprClass, T, VK, OK_Ordinary),
LParenLoc(lparenloc), TInfoAndScope(tinfo, fileScope), Init(init) {
setDependence(computeDependence(this));
}


explicit CompoundLiteralExpr(EmptyShell Empty)
: Expr(CompoundLiteralExprClass, Empty) { }

const Expr *getInitializer() const { return cast<Expr>(Init); }
Expr *getInitializer() { return cast<Expr>(Init); }
void setInitializer(Expr *E) { Init = E; }

bool isFileScope() const { return TInfoAndScope.getInt(); }
void setFileScope(bool FS) { TInfoAndScope.setInt(FS); }

SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation L) { LParenLoc = L; }

TypeSourceInfo *getTypeSourceInfo() const {
return TInfoAndScope.getPointer();
}
void setTypeSourceInfo(TypeSourceInfo *tinfo) {
TInfoAndScope.setPointer(tinfo);
}

SourceLocation getBeginLoc() const LLVM_READONLY {

if (!Init)
return SourceLocation();
if (LParenLoc.isInvalid())
return Init->getBeginLoc();
return LParenLoc;
}
SourceLocation getEndLoc() const LLVM_READONLY {

if (!Init)
return SourceLocation();
return Init->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CompoundLiteralExprClass;
}


child_range children() { return child_range(&Init, &Init+1); }
const_child_range children() const {
return const_child_range(&Init, &Init + 1);
}
};





class CastExpr : public Expr {
Stmt *Op;

bool CastConsistency() const;

const CXXBaseSpecifier * const *path_buffer() const {
return const_cast<CastExpr*>(this)->path_buffer();
}
CXXBaseSpecifier **path_buffer();

friend class ASTStmtReader;

protected:
CastExpr(StmtClass SC, QualType ty, ExprValueKind VK, const CastKind kind,
Expr *op, unsigned BasePathSize, bool HasFPFeatures)
: Expr(SC, ty, VK, OK_Ordinary), Op(op) {
CastExprBits.Kind = kind;
CastExprBits.PartOfExplicitCast = false;
CastExprBits.BasePathSize = BasePathSize;
assert((CastExprBits.BasePathSize == BasePathSize) &&
"BasePathSize overflow!");
setDependence(computeDependence(this));
assert(CastConsistency());
CastExprBits.HasFPFeatures = HasFPFeatures;
}


CastExpr(StmtClass SC, EmptyShell Empty, unsigned BasePathSize,
bool HasFPFeatures)
: Expr(SC, Empty) {
CastExprBits.PartOfExplicitCast = false;
CastExprBits.BasePathSize = BasePathSize;
CastExprBits.HasFPFeatures = HasFPFeatures;
assert((CastExprBits.BasePathSize == BasePathSize) &&
"BasePathSize overflow!");
}



FPOptionsOverride *getTrailingFPFeatures();
const FPOptionsOverride *getTrailingFPFeatures() const {
return const_cast<CastExpr *>(this)->getTrailingFPFeatures();
}

public:
CastKind getCastKind() const { return (CastKind) CastExprBits.Kind; }
void setCastKind(CastKind K) { CastExprBits.Kind = K; }

static const char *getCastKindName(CastKind CK);
const char *getCastKindName() const { return getCastKindName(getCastKind()); }

Expr *getSubExpr() { return cast<Expr>(Op); }
const Expr *getSubExpr() const { return cast<Expr>(Op); }
void setSubExpr(Expr *E) { Op = E; }




Expr *getSubExprAsWritten();
const Expr *getSubExprAsWritten() const {
return const_cast<CastExpr *>(this)->getSubExprAsWritten();
}



NamedDecl *getConversionFunction() const;

typedef CXXBaseSpecifier **path_iterator;
typedef const CXXBaseSpecifier *const *path_const_iterator;
bool path_empty() const { return path_size() == 0; }
unsigned path_size() const { return CastExprBits.BasePathSize; }
path_iterator path_begin() { return path_buffer(); }
path_iterator path_end() { return path_buffer() + path_size(); }
path_const_iterator path_begin() const { return path_buffer(); }
path_const_iterator path_end() const { return path_buffer() + path_size(); }

llvm::iterator_range<path_iterator> path() {
return llvm::make_range(path_begin(), path_end());
}
llvm::iterator_range<path_const_iterator> path() const {
return llvm::make_range(path_begin(), path_end());
}

const FieldDecl *getTargetUnionField() const {
assert(getCastKind() == CK_ToUnion);
return getTargetFieldForToUnionCast(getType(), getSubExpr()->getType());
}

bool hasStoredFPFeatures() const { return CastExprBits.HasFPFeatures; }


FPOptionsOverride getStoredFPFeatures() const {
assert(hasStoredFPFeatures());
return *getTrailingFPFeatures();
}



FPOptions getFPFeaturesInEffect(const LangOptions &LO) const {
if (hasStoredFPFeatures())
return getStoredFPFeatures().applyOverrides(LO);
return FPOptions::defaultWithoutTrailingStorage(LO);
}

FPOptionsOverride getFPFeatures() const {
if (hasStoredFPFeatures())
return getStoredFPFeatures();
return FPOptionsOverride();
}

static const FieldDecl *getTargetFieldForToUnionCast(QualType unionType,
QualType opType);
static const FieldDecl *getTargetFieldForToUnionCast(const RecordDecl *RD,
QualType opType);

static bool classof(const Stmt *T) {
return T->getStmtClass() >= firstCastExprConstant &&
T->getStmtClass() <= lastCastExprConstant;
}


child_range children() { return child_range(&Op, &Op+1); }
const_child_range children() const { return const_child_range(&Op, &Op + 1); }
};





















class ImplicitCastExpr final
: public CastExpr,
private llvm::TrailingObjects<ImplicitCastExpr, CXXBaseSpecifier *,
FPOptionsOverride> {

ImplicitCastExpr(QualType ty, CastKind kind, Expr *op,
unsigned BasePathLength, FPOptionsOverride FPO,
ExprValueKind VK)
: CastExpr(ImplicitCastExprClass, ty, VK, kind, op, BasePathLength,
FPO.requiresTrailingStorage()) {
if (hasStoredFPFeatures())
*getTrailingFPFeatures() = FPO;
}


explicit ImplicitCastExpr(EmptyShell Shell, unsigned PathSize,
bool HasFPFeatures)
: CastExpr(ImplicitCastExprClass, Shell, PathSize, HasFPFeatures) {}

unsigned numTrailingObjects(OverloadToken<CXXBaseSpecifier *>) const {
return path_size();
}

public:
enum OnStack_t { OnStack };
ImplicitCastExpr(OnStack_t _, QualType ty, CastKind kind, Expr *op,
ExprValueKind VK, FPOptionsOverride FPO)
: CastExpr(ImplicitCastExprClass, ty, VK, kind, op, 0,
FPO.requiresTrailingStorage()) {
if (hasStoredFPFeatures())
*getTrailingFPFeatures() = FPO;
}

bool isPartOfExplicitCast() const { return CastExprBits.PartOfExplicitCast; }
void setIsPartOfExplicitCast(bool PartOfExplicitCast) {
CastExprBits.PartOfExplicitCast = PartOfExplicitCast;
}

static ImplicitCastExpr *Create(const ASTContext &Context, QualType T,
CastKind Kind, Expr *Operand,
const CXXCastPath *BasePath,
ExprValueKind Cat, FPOptionsOverride FPO);

static ImplicitCastExpr *CreateEmpty(const ASTContext &Context,
unsigned PathSize, bool HasFPFeatures);

SourceLocation getBeginLoc() const LLVM_READONLY {
return getSubExpr()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getSubExpr()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ImplicitCastExprClass;
}

friend TrailingObjects;
friend class CastExpr;
};

















class ExplicitCastExpr : public CastExpr {


TypeSourceInfo *TInfo;

protected:
ExplicitCastExpr(StmtClass SC, QualType exprTy, ExprValueKind VK,
CastKind kind, Expr *op, unsigned PathSize,
bool HasFPFeatures, TypeSourceInfo *writtenTy)
: CastExpr(SC, exprTy, VK, kind, op, PathSize, HasFPFeatures),
TInfo(writtenTy) {}


ExplicitCastExpr(StmtClass SC, EmptyShell Shell, unsigned PathSize,
bool HasFPFeatures)
: CastExpr(SC, Shell, PathSize, HasFPFeatures) {}

public:


TypeSourceInfo *getTypeInfoAsWritten() const { return TInfo; }
void setTypeInfoAsWritten(TypeSourceInfo *writtenTy) { TInfo = writtenTy; }



QualType getTypeAsWritten() const { return TInfo->getType(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() >= firstExplicitCastExprConstant &&
T->getStmtClass() <= lastExplicitCastExprConstant;
}
};




class CStyleCastExpr final
: public ExplicitCastExpr,
private llvm::TrailingObjects<CStyleCastExpr, CXXBaseSpecifier *,
FPOptionsOverride> {
SourceLocation LPLoc;
SourceLocation RPLoc;

CStyleCastExpr(QualType exprTy, ExprValueKind vk, CastKind kind, Expr *op,
unsigned PathSize, FPOptionsOverride FPO,
TypeSourceInfo *writtenTy, SourceLocation l, SourceLocation r)
: ExplicitCastExpr(CStyleCastExprClass, exprTy, vk, kind, op, PathSize,
FPO.requiresTrailingStorage(), writtenTy),
LPLoc(l), RPLoc(r) {
if (hasStoredFPFeatures())
*getTrailingFPFeatures() = FPO;
}


explicit CStyleCastExpr(EmptyShell Shell, unsigned PathSize,
bool HasFPFeatures)
: ExplicitCastExpr(CStyleCastExprClass, Shell, PathSize, HasFPFeatures) {}

unsigned numTrailingObjects(OverloadToken<CXXBaseSpecifier *>) const {
return path_size();
}

public:
static CStyleCastExpr *
Create(const ASTContext &Context, QualType T, ExprValueKind VK, CastKind K,
Expr *Op, const CXXCastPath *BasePath, FPOptionsOverride FPO,
TypeSourceInfo *WrittenTy, SourceLocation L, SourceLocation R);

static CStyleCastExpr *CreateEmpty(const ASTContext &Context,
unsigned PathSize, bool HasFPFeatures);

SourceLocation getLParenLoc() const { return LPLoc; }
void setLParenLoc(SourceLocation L) { LPLoc = L; }

SourceLocation getRParenLoc() const { return RPLoc; }
void setRParenLoc(SourceLocation L) { RPLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return LPLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return getSubExpr()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CStyleCastExprClass;
}

friend TrailingObjects;
friend class CastExpr;
};



















class BinaryOperator : public Expr {
enum { LHS, RHS, END_EXPR };
Stmt *SubExprs[END_EXPR];

public:
typedef BinaryOperatorKind Opcode;

protected:
size_t offsetOfTrailingStorage() const;


FPOptionsOverride *getTrailingFPFeatures() {
assert(BinaryOperatorBits.HasFPFeatures);
return reinterpret_cast<FPOptionsOverride *>(
reinterpret_cast<char *>(this) + offsetOfTrailingStorage());
}
const FPOptionsOverride *getTrailingFPFeatures() const {
assert(BinaryOperatorBits.HasFPFeatures);
return reinterpret_cast<const FPOptionsOverride *>(
reinterpret_cast<const char *>(this) + offsetOfTrailingStorage());
}



BinaryOperator(const ASTContext &Ctx, Expr *lhs, Expr *rhs, Opcode opc,
QualType ResTy, ExprValueKind VK, ExprObjectKind OK,
SourceLocation opLoc, FPOptionsOverride FPFeatures);


explicit BinaryOperator(EmptyShell Empty) : Expr(BinaryOperatorClass, Empty) {
BinaryOperatorBits.Opc = BO_Comma;
}

public:
static BinaryOperator *CreateEmpty(const ASTContext &C, bool hasFPFeatures);

static BinaryOperator *Create(const ASTContext &C, Expr *lhs, Expr *rhs,
Opcode opc, QualType ResTy, ExprValueKind VK,
ExprObjectKind OK, SourceLocation opLoc,
FPOptionsOverride FPFeatures);
SourceLocation getExprLoc() const { return getOperatorLoc(); }
SourceLocation getOperatorLoc() const { return BinaryOperatorBits.OpLoc; }
void setOperatorLoc(SourceLocation L) { BinaryOperatorBits.OpLoc = L; }

Opcode getOpcode() const {
return static_cast<Opcode>(BinaryOperatorBits.Opc);
}
void setOpcode(Opcode Opc) { BinaryOperatorBits.Opc = Opc; }

Expr *getLHS() const { return cast<Expr>(SubExprs[LHS]); }
void setLHS(Expr *E) { SubExprs[LHS] = E; }
Expr *getRHS() const { return cast<Expr>(SubExprs[RHS]); }
void setRHS(Expr *E) { SubExprs[RHS] = E; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return getLHS()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getRHS()->getEndLoc();
}



static StringRef getOpcodeStr(Opcode Op);

StringRef getOpcodeStr() const { return getOpcodeStr(getOpcode()); }



static Opcode getOverloadedOpcode(OverloadedOperatorKind OO);



static OverloadedOperatorKind getOverloadedOperator(Opcode Opc);


static bool isPtrMemOp(Opcode Opc) {
return Opc == BO_PtrMemD || Opc == BO_PtrMemI;
}
bool isPtrMemOp() const { return isPtrMemOp(getOpcode()); }

static bool isMultiplicativeOp(Opcode Opc) {
return Opc >= BO_Mul && Opc <= BO_Rem;
}
bool isMultiplicativeOp() const { return isMultiplicativeOp(getOpcode()); }
static bool isAdditiveOp(Opcode Opc) { return Opc == BO_Add || Opc==BO_Sub; }
bool isAdditiveOp() const { return isAdditiveOp(getOpcode()); }
static bool isShiftOp(Opcode Opc) { return Opc == BO_Shl || Opc == BO_Shr; }
bool isShiftOp() const { return isShiftOp(getOpcode()); }

static bool isBitwiseOp(Opcode Opc) { return Opc >= BO_And && Opc <= BO_Or; }
bool isBitwiseOp() const { return isBitwiseOp(getOpcode()); }

static bool isRelationalOp(Opcode Opc) { return Opc >= BO_LT && Opc<=BO_GE; }
bool isRelationalOp() const { return isRelationalOp(getOpcode()); }

static bool isEqualityOp(Opcode Opc) { return Opc == BO_EQ || Opc == BO_NE; }
bool isEqualityOp() const { return isEqualityOp(getOpcode()); }

static bool isComparisonOp(Opcode Opc) { return Opc >= BO_Cmp && Opc<=BO_NE; }
bool isComparisonOp() const { return isComparisonOp(getOpcode()); }

static bool isCommaOp(Opcode Opc) { return Opc == BO_Comma; }
bool isCommaOp() const { return isCommaOp(getOpcode()); }

static Opcode negateComparisonOp(Opcode Opc) {
switch (Opc) {
default:
llvm_unreachable("Not a comparison operator.");
case BO_LT: return BO_GE;
case BO_GT: return BO_LE;
case BO_LE: return BO_GT;
case BO_GE: return BO_LT;
case BO_EQ: return BO_NE;
case BO_NE: return BO_EQ;
}
}

static Opcode reverseComparisonOp(Opcode Opc) {
switch (Opc) {
default:
llvm_unreachable("Not a comparison operator.");
case BO_LT: return BO_GT;
case BO_GT: return BO_LT;
case BO_LE: return BO_GE;
case BO_GE: return BO_LE;
case BO_EQ:
case BO_NE:
return Opc;
}
}

static bool isLogicalOp(Opcode Opc) { return Opc == BO_LAnd || Opc==BO_LOr; }
bool isLogicalOp() const { return isLogicalOp(getOpcode()); }

static bool isAssignmentOp(Opcode Opc) {
return Opc >= BO_Assign && Opc <= BO_OrAssign;
}
bool isAssignmentOp() const { return isAssignmentOp(getOpcode()); }

static bool isCompoundAssignmentOp(Opcode Opc) {
return Opc > BO_Assign && Opc <= BO_OrAssign;
}
bool isCompoundAssignmentOp() const {
return isCompoundAssignmentOp(getOpcode());
}
static Opcode getOpForCompoundAssignment(Opcode Opc) {
assert(isCompoundAssignmentOp(Opc));
if (Opc >= BO_AndAssign)
return Opcode(unsigned(Opc) - BO_AndAssign + BO_And);
else
return Opcode(unsigned(Opc) - BO_MulAssign + BO_Mul);
}

static bool isShiftAssignOp(Opcode Opc) {
return Opc == BO_ShlAssign || Opc == BO_ShrAssign;
}
bool isShiftAssignOp() const {
return isShiftAssignOp(getOpcode());
}




static bool isNullPointerArithmeticExtension(ASTContext &Ctx, Opcode Opc,
Expr *LHS, Expr *RHS);

static bool classof(const Stmt *S) {
return S->getStmtClass() >= firstBinaryOperatorConstant &&
S->getStmtClass() <= lastBinaryOperatorConstant;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0]+END_EXPR);
}
const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}



void setHasStoredFPFeatures(bool B) { BinaryOperatorBits.HasFPFeatures = B; }
bool hasStoredFPFeatures() const { return BinaryOperatorBits.HasFPFeatures; }


FPOptionsOverride getStoredFPFeatures() const {
assert(hasStoredFPFeatures());
return *getTrailingFPFeatures();
}

void setStoredFPFeatures(FPOptionsOverride F) {
assert(BinaryOperatorBits.HasFPFeatures);
*getTrailingFPFeatures() = F;
}



FPOptions getFPFeaturesInEffect(const LangOptions &LO) const {
if (BinaryOperatorBits.HasFPFeatures)
return getStoredFPFeatures().applyOverrides(LO);
return FPOptions::defaultWithoutTrailingStorage(LO);
}


FPOptionsOverride getFPFeatures(const LangOptions &LO) const {
if (BinaryOperatorBits.HasFPFeatures)
return getStoredFPFeatures();
return FPOptionsOverride();
}



bool isFPContractableWithinStatement(const LangOptions &LO) const {
return getFPFeaturesInEffect(LO).allowFPContractWithinStatement();
}



bool isFEnvAccessOn(const LangOptions &LO) const {
return getFPFeaturesInEffect(LO).getAllowFEnvAccess();
}

protected:
BinaryOperator(const ASTContext &Ctx, Expr *lhs, Expr *rhs, Opcode opc,
QualType ResTy, ExprValueKind VK, ExprObjectKind OK,
SourceLocation opLoc, FPOptionsOverride FPFeatures,
bool dead2);


BinaryOperator(StmtClass SC, EmptyShell Empty) : Expr(SC, Empty) {
BinaryOperatorBits.Opc = BO_MulAssign;
}



static unsigned sizeOfTrailingObjects(bool HasFPFeatures) {
return HasFPFeatures * sizeof(FPOptionsOverride);
}
};







class CompoundAssignOperator : public BinaryOperator {
QualType ComputationLHSType;
QualType ComputationResultType;


explicit CompoundAssignOperator(const ASTContext &C, EmptyShell Empty,
bool hasFPFeatures)
: BinaryOperator(CompoundAssignOperatorClass, Empty) {}

protected:
CompoundAssignOperator(const ASTContext &C, Expr *lhs, Expr *rhs, Opcode opc,
QualType ResType, ExprValueKind VK, ExprObjectKind OK,
SourceLocation OpLoc, FPOptionsOverride FPFeatures,
QualType CompLHSType, QualType CompResultType)
: BinaryOperator(C, lhs, rhs, opc, ResType, VK, OK, OpLoc, FPFeatures,
true),
ComputationLHSType(CompLHSType), ComputationResultType(CompResultType) {
assert(isCompoundAssignmentOp() &&
"Only should be used for compound assignments");
}

public:
static CompoundAssignOperator *CreateEmpty(const ASTContext &C,
bool hasFPFeatures);

static CompoundAssignOperator *
Create(const ASTContext &C, Expr *lhs, Expr *rhs, Opcode opc, QualType ResTy,
ExprValueKind VK, ExprObjectKind OK, SourceLocation opLoc,
FPOptionsOverride FPFeatures, QualType CompLHSType = QualType(),
QualType CompResultType = QualType());




QualType getComputationLHSType() const { return ComputationLHSType; }
void setComputationLHSType(QualType T) { ComputationLHSType = T; }

QualType getComputationResultType() const { return ComputationResultType; }
void setComputationResultType(QualType T) { ComputationResultType = T; }

static bool classof(const Stmt *S) {
return S->getStmtClass() == CompoundAssignOperatorClass;
}
};

inline size_t BinaryOperator::offsetOfTrailingStorage() const {
assert(BinaryOperatorBits.HasFPFeatures);
return isa<CompoundAssignOperator>(this) ? sizeof(CompoundAssignOperator)
: sizeof(BinaryOperator);
}



class AbstractConditionalOperator : public Expr {
SourceLocation QuestionLoc, ColonLoc;
friend class ASTStmtReader;

protected:
AbstractConditionalOperator(StmtClass SC, QualType T, ExprValueKind VK,
ExprObjectKind OK, SourceLocation qloc,
SourceLocation cloc)
: Expr(SC, T, VK, OK), QuestionLoc(qloc), ColonLoc(cloc) {}

AbstractConditionalOperator(StmtClass SC, EmptyShell Empty)
: Expr(SC, Empty) { }

public:


Expr *getCond() const;



Expr *getTrueExpr() const;




Expr *getFalseExpr() const;

SourceLocation getQuestionLoc() const { return QuestionLoc; }
SourceLocation getColonLoc() const { return ColonLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ConditionalOperatorClass ||
T->getStmtClass() == BinaryConditionalOperatorClass;
}
};



class ConditionalOperator : public AbstractConditionalOperator {
enum { COND, LHS, RHS, END_EXPR };
Stmt* SubExprs[END_EXPR];

friend class ASTStmtReader;
public:
ConditionalOperator(Expr *cond, SourceLocation QLoc, Expr *lhs,
SourceLocation CLoc, Expr *rhs, QualType t,
ExprValueKind VK, ExprObjectKind OK)
: AbstractConditionalOperator(ConditionalOperatorClass, t, VK, OK, QLoc,
CLoc) {
SubExprs[COND] = cond;
SubExprs[LHS] = lhs;
SubExprs[RHS] = rhs;
setDependence(computeDependence(this));
}


explicit ConditionalOperator(EmptyShell Empty)
: AbstractConditionalOperator(ConditionalOperatorClass, Empty) { }



Expr *getCond() const { return cast<Expr>(SubExprs[COND]); }



Expr *getTrueExpr() const { return cast<Expr>(SubExprs[LHS]); }




Expr *getFalseExpr() const { return cast<Expr>(SubExprs[RHS]); }

Expr *getLHS() const { return cast<Expr>(SubExprs[LHS]); }
Expr *getRHS() const { return cast<Expr>(SubExprs[RHS]); }

SourceLocation getBeginLoc() const LLVM_READONLY {
return getCond()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getRHS()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ConditionalOperatorClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0]+END_EXPR);
}
const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}
};






class BinaryConditionalOperator : public AbstractConditionalOperator {
enum { COMMON, COND, LHS, RHS, NUM_SUBEXPRS };






Stmt *SubExprs[NUM_SUBEXPRS];
OpaqueValueExpr *OpaqueValue;

friend class ASTStmtReader;
public:
BinaryConditionalOperator(Expr *common, OpaqueValueExpr *opaqueValue,
Expr *cond, Expr *lhs, Expr *rhs,
SourceLocation qloc, SourceLocation cloc,
QualType t, ExprValueKind VK, ExprObjectKind OK)
: AbstractConditionalOperator(BinaryConditionalOperatorClass, t, VK, OK,
qloc, cloc),
OpaqueValue(opaqueValue) {
SubExprs[COMMON] = common;
SubExprs[COND] = cond;
SubExprs[LHS] = lhs;
SubExprs[RHS] = rhs;
assert(OpaqueValue->getSourceExpr() == common && "Wrong opaque value");
setDependence(computeDependence(this));
}


explicit BinaryConditionalOperator(EmptyShell Empty)
: AbstractConditionalOperator(BinaryConditionalOperatorClass, Empty) { }




Expr *getCommon() const { return cast<Expr>(SubExprs[COMMON]); }


OpaqueValueExpr *getOpaqueValue() const { return OpaqueValue; }



Expr *getCond() const { return cast<Expr>(SubExprs[COND]); }




Expr *getTrueExpr() const {
return cast<Expr>(SubExprs[LHS]);
}




Expr *getFalseExpr() const {
return cast<Expr>(SubExprs[RHS]);
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return getCommon()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getFalseExpr()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == BinaryConditionalOperatorClass;
}


child_range children() {
return child_range(SubExprs, SubExprs + NUM_SUBEXPRS);
}
const_child_range children() const {
return const_child_range(SubExprs, SubExprs + NUM_SUBEXPRS);
}
};

inline Expr *AbstractConditionalOperator::getCond() const {
if (const ConditionalOperator *co = dyn_cast<ConditionalOperator>(this))
return co->getCond();
return cast<BinaryConditionalOperator>(this)->getCond();
}

inline Expr *AbstractConditionalOperator::getTrueExpr() const {
if (const ConditionalOperator *co = dyn_cast<ConditionalOperator>(this))
return co->getTrueExpr();
return cast<BinaryConditionalOperator>(this)->getTrueExpr();
}

inline Expr *AbstractConditionalOperator::getFalseExpr() const {
if (const ConditionalOperator *co = dyn_cast<ConditionalOperator>(this))
return co->getFalseExpr();
return cast<BinaryConditionalOperator>(this)->getFalseExpr();
}


class AddrLabelExpr : public Expr {
SourceLocation AmpAmpLoc, LabelLoc;
LabelDecl *Label;
public:
AddrLabelExpr(SourceLocation AALoc, SourceLocation LLoc, LabelDecl *L,
QualType t)
: Expr(AddrLabelExprClass, t, VK_PRValue, OK_Ordinary), AmpAmpLoc(AALoc),
LabelLoc(LLoc), Label(L) {
setDependence(ExprDependence::None);
}


explicit AddrLabelExpr(EmptyShell Empty)
: Expr(AddrLabelExprClass, Empty) { }

SourceLocation getAmpAmpLoc() const { return AmpAmpLoc; }
void setAmpAmpLoc(SourceLocation L) { AmpAmpLoc = L; }
SourceLocation getLabelLoc() const { return LabelLoc; }
void setLabelLoc(SourceLocation L) { LabelLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AmpAmpLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return LabelLoc; }

LabelDecl *getLabel() const { return Label; }
void setLabel(LabelDecl *L) { Label = L; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == AddrLabelExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};







class StmtExpr : public Expr {
Stmt *SubStmt;
SourceLocation LParenLoc, RParenLoc;
public:
StmtExpr(CompoundStmt *SubStmt, QualType T, SourceLocation LParenLoc,
SourceLocation RParenLoc, unsigned TemplateDepth)
: Expr(StmtExprClass, T, VK_PRValue, OK_Ordinary), SubStmt(SubStmt),
LParenLoc(LParenLoc), RParenLoc(RParenLoc) {
setDependence(computeDependence(this, TemplateDepth));


StmtExprBits.TemplateDepth = TemplateDepth;
}


explicit StmtExpr(EmptyShell Empty) : Expr(StmtExprClass, Empty) { }

CompoundStmt *getSubStmt() { return cast<CompoundStmt>(SubStmt); }
const CompoundStmt *getSubStmt() const { return cast<CompoundStmt>(SubStmt); }
void setSubStmt(CompoundStmt *S) { SubStmt = S; }

SourceLocation getBeginLoc() const LLVM_READONLY { return LParenLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation L) { LParenLoc = L; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

unsigned getTemplateDepth() const { return StmtExprBits.TemplateDepth; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == StmtExprClass;
}


child_range children() { return child_range(&SubStmt, &SubStmt+1); }
const_child_range children() const {
return const_child_range(&SubStmt, &SubStmt + 1);
}
};







class ShuffleVectorExpr : public Expr {
SourceLocation BuiltinLoc, RParenLoc;





Stmt **SubExprs;
unsigned NumExprs;

public:
ShuffleVectorExpr(const ASTContext &C, ArrayRef<Expr*> args, QualType Type,
SourceLocation BLoc, SourceLocation RP);


explicit ShuffleVectorExpr(EmptyShell Empty)
: Expr(ShuffleVectorExprClass, Empty), SubExprs(nullptr) { }

SourceLocation getBuiltinLoc() const { return BuiltinLoc; }
void setBuiltinLoc(SourceLocation L) { BuiltinLoc = L; }

SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return BuiltinLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ShuffleVectorExprClass;
}




unsigned getNumSubExprs() const { return NumExprs; }


Expr **getSubExprs() { return reinterpret_cast<Expr **>(SubExprs); }


Expr *getExpr(unsigned Index) {
assert((Index < NumExprs) && "Arg access out of range!");
return cast<Expr>(SubExprs[Index]);
}
const Expr *getExpr(unsigned Index) const {
assert((Index < NumExprs) && "Arg access out of range!");
return cast<Expr>(SubExprs[Index]);
}

void setExprs(const ASTContext &C, ArrayRef<Expr *> Exprs);

llvm::APSInt getShuffleMaskIdx(const ASTContext &Ctx, unsigned N) const {
assert((N < NumExprs - 2) && "Shuffle idx out of range!");
return getExpr(N+2)->EvaluateKnownConstInt(Ctx);
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0]+NumExprs);
}
const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + NumExprs);
}
};




class ConvertVectorExpr : public Expr {
private:
Stmt *SrcExpr;
TypeSourceInfo *TInfo;
SourceLocation BuiltinLoc, RParenLoc;

friend class ASTReader;
friend class ASTStmtReader;
explicit ConvertVectorExpr(EmptyShell Empty) : Expr(ConvertVectorExprClass, Empty) {}

public:
ConvertVectorExpr(Expr *SrcExpr, TypeSourceInfo *TI, QualType DstType,
ExprValueKind VK, ExprObjectKind OK,
SourceLocation BuiltinLoc, SourceLocation RParenLoc)
: Expr(ConvertVectorExprClass, DstType, VK, OK), SrcExpr(SrcExpr),
TInfo(TI), BuiltinLoc(BuiltinLoc), RParenLoc(RParenLoc) {
setDependence(computeDependence(this));
}


Expr *getSrcExpr() const { return cast<Expr>(SrcExpr); }


TypeSourceInfo *getTypeSourceInfo() const {
return TInfo;
}
void setTypeSourceInfo(TypeSourceInfo *ti) {
TInfo = ti;
}


SourceLocation getBuiltinLoc() const { return BuiltinLoc; }


SourceLocation getRParenLoc() const { return RParenLoc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return BuiltinLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ConvertVectorExprClass;
}


child_range children() { return child_range(&SrcExpr, &SrcExpr+1); }
const_child_range children() const {
return const_child_range(&SrcExpr, &SrcExpr + 1);
}
};










class ChooseExpr : public Expr {
enum { COND, LHS, RHS, END_EXPR };
Stmt* SubExprs[END_EXPR];
SourceLocation BuiltinLoc, RParenLoc;
bool CondIsTrue;
public:
ChooseExpr(SourceLocation BLoc, Expr *cond, Expr *lhs, Expr *rhs, QualType t,
ExprValueKind VK, ExprObjectKind OK, SourceLocation RP,
bool condIsTrue)
: Expr(ChooseExprClass, t, VK, OK), BuiltinLoc(BLoc), RParenLoc(RP),
CondIsTrue(condIsTrue) {
SubExprs[COND] = cond;
SubExprs[LHS] = lhs;
SubExprs[RHS] = rhs;

setDependence(computeDependence(this));
}


explicit ChooseExpr(EmptyShell Empty) : Expr(ChooseExprClass, Empty) { }



bool isConditionTrue() const {
assert(!isConditionDependent() &&
"Dependent condition isn't true or false");
return CondIsTrue;
}
void setIsConditionTrue(bool isTrue) { CondIsTrue = isTrue; }

bool isConditionDependent() const {
return getCond()->isTypeDependent() || getCond()->isValueDependent();
}



Expr *getChosenSubExpr() const {
return isConditionTrue() ? getLHS() : getRHS();
}

Expr *getCond() const { return cast<Expr>(SubExprs[COND]); }
void setCond(Expr *E) { SubExprs[COND] = E; }
Expr *getLHS() const { return cast<Expr>(SubExprs[LHS]); }
void setLHS(Expr *E) { SubExprs[LHS] = E; }
Expr *getRHS() const { return cast<Expr>(SubExprs[RHS]); }
void setRHS(Expr *E) { SubExprs[RHS] = E; }

SourceLocation getBuiltinLoc() const { return BuiltinLoc; }
void setBuiltinLoc(SourceLocation L) { BuiltinLoc = L; }

SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return BuiltinLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ChooseExprClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[0]+END_EXPR);
}
const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[0] + END_EXPR);
}
};







class GNUNullExpr : public Expr {

SourceLocation TokenLoc;

public:
GNUNullExpr(QualType Ty, SourceLocation Loc)
: Expr(GNUNullExprClass, Ty, VK_PRValue, OK_Ordinary), TokenLoc(Loc) {
setDependence(ExprDependence::None);
}


explicit GNUNullExpr(EmptyShell Empty) : Expr(GNUNullExprClass, Empty) { }


SourceLocation getTokenLocation() const { return TokenLoc; }
void setTokenLocation(SourceLocation L) { TokenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return TokenLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return TokenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == GNUNullExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};


class VAArgExpr : public Expr {
Stmt *Val;
llvm::PointerIntPair<TypeSourceInfo *, 1, bool> TInfo;
SourceLocation BuiltinLoc, RParenLoc;
public:
VAArgExpr(SourceLocation BLoc, Expr *e, TypeSourceInfo *TInfo,
SourceLocation RPLoc, QualType t, bool IsMS)
: Expr(VAArgExprClass, t, VK_PRValue, OK_Ordinary), Val(e),
TInfo(TInfo, IsMS), BuiltinLoc(BLoc), RParenLoc(RPLoc) {
setDependence(computeDependence(this));
}


explicit VAArgExpr(EmptyShell Empty)
: Expr(VAArgExprClass, Empty), Val(nullptr), TInfo(nullptr, false) {}

const Expr *getSubExpr() const { return cast<Expr>(Val); }
Expr *getSubExpr() { return cast<Expr>(Val); }
void setSubExpr(Expr *E) { Val = E; }


bool isMicrosoftABI() const { return TInfo.getInt(); }
void setIsMicrosoftABI(bool IsMS) { TInfo.setInt(IsMS); }

TypeSourceInfo *getWrittenTypeInfo() const { return TInfo.getPointer(); }
void setWrittenTypeInfo(TypeSourceInfo *TI) { TInfo.setPointer(TI); }

SourceLocation getBuiltinLoc() const { return BuiltinLoc; }
void setBuiltinLoc(SourceLocation L) { BuiltinLoc = L; }

SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return BuiltinLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == VAArgExprClass;
}


child_range children() { return child_range(&Val, &Val+1); }
const_child_range children() const {
return const_child_range(&Val, &Val + 1);
}
};



class SourceLocExpr final : public Expr {
SourceLocation BuiltinLoc, RParenLoc;
DeclContext *ParentContext;

public:
enum IdentKind { Function, File, Line, Column };

SourceLocExpr(const ASTContext &Ctx, IdentKind Type, SourceLocation BLoc,
SourceLocation RParenLoc, DeclContext *Context);


explicit SourceLocExpr(EmptyShell Empty) : Expr(SourceLocExprClass, Empty) {}



APValue EvaluateInContext(const ASTContext &Ctx,
const Expr *DefaultExpr) const;


StringRef getBuiltinStr() const;

IdentKind getIdentKind() const {
return static_cast<IdentKind>(SourceLocExprBits.Kind);
}

bool isStringType() const {
switch (getIdentKind()) {
case File:
case Function:
return true;
case Line:
case Column:
return false;
}
llvm_unreachable("unknown source location expression kind");
}
bool isIntType() const LLVM_READONLY { return !isStringType(); }



const DeclContext *getParentContext() const { return ParentContext; }
DeclContext *getParentContext() { return ParentContext; }

SourceLocation getLocation() const { return BuiltinLoc; }
SourceLocation getBeginLoc() const { return BuiltinLoc; }
SourceLocation getEndLoc() const { return RParenLoc; }

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(child_iterator(), child_iterator());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == SourceLocExprClass;
}

private:
friend class ASTStmtReader;
};














































class InitListExpr : public Expr {

typedef ASTVector<Stmt *> InitExprsTy;
InitExprsTy InitExprs;
SourceLocation LBraceLoc, RBraceLoc;






llvm::PointerIntPair<InitListExpr *, 1, bool> AltForm;








llvm::PointerUnion<Expr *, FieldDecl *> ArrayFillerOrUnionFieldInit;

public:
InitListExpr(const ASTContext &C, SourceLocation lbraceloc,
ArrayRef<Expr*> initExprs, SourceLocation rbraceloc);


explicit InitListExpr(EmptyShell Empty)
: Expr(InitListExprClass, Empty), AltForm(nullptr, true) { }

unsigned getNumInits() const { return InitExprs.size(); }


Expr **getInits() { return reinterpret_cast<Expr **>(InitExprs.data()); }


Expr * const *getInits() const {
return reinterpret_cast<Expr * const *>(InitExprs.data());
}

ArrayRef<Expr *> inits() {
return llvm::makeArrayRef(getInits(), getNumInits());
}

ArrayRef<Expr *> inits() const {
return llvm::makeArrayRef(getInits(), getNumInits());
}

const Expr *getInit(unsigned Init) const {
assert(Init < getNumInits() && "Initializer access out of range!");
return cast_or_null<Expr>(InitExprs[Init]);
}

Expr *getInit(unsigned Init) {
assert(Init < getNumInits() && "Initializer access out of range!");
return cast_or_null<Expr>(InitExprs[Init]);
}

void setInit(unsigned Init, Expr *expr) {
assert(Init < getNumInits() && "Initializer access out of range!");
InitExprs[Init] = expr;

if (expr)
setDependence(getDependence() | expr->getDependence());
}



void markError() {
assert(isSemanticForm());
setDependence(getDependence() | ExprDependence::ErrorDependent);
}


void reserveInits(const ASTContext &C, unsigned NumInits);







void resizeInits(const ASTContext &Context, unsigned NumInits);








Expr *updateInit(const ASTContext &C, unsigned Init, Expr *expr);




Expr *getArrayFiller() {
return ArrayFillerOrUnionFieldInit.dyn_cast<Expr *>();
}
const Expr *getArrayFiller() const {
return const_cast<InitListExpr *>(this)->getArrayFiller();
}
void setArrayFiller(Expr *filler);



bool hasArrayFiller() const { return getArrayFiller(); }







FieldDecl *getInitializedFieldInUnion() {
return ArrayFillerOrUnionFieldInit.dyn_cast<FieldDecl *>();
}
const FieldDecl *getInitializedFieldInUnion() const {
return const_cast<InitListExpr *>(this)->getInitializedFieldInUnion();
}
void setInitializedFieldInUnion(FieldDecl *FD) {
assert((FD == nullptr
|| getInitializedFieldInUnion() == nullptr
|| getInitializedFieldInUnion() == FD)
&& "Only one field of a union may be initialized at a time!");
ArrayFillerOrUnionFieldInit = FD;
}





bool isExplicit() const {
return LBraceLoc.isValid() && RBraceLoc.isValid();
}



bool isStringLiteralInit() const;




bool isTransparent() const;



bool isIdiomaticZeroInitializer(const LangOptions &LangOpts) const;

SourceLocation getLBraceLoc() const { return LBraceLoc; }
void setLBraceLoc(SourceLocation Loc) { LBraceLoc = Loc; }
SourceLocation getRBraceLoc() const { return RBraceLoc; }
void setRBraceLoc(SourceLocation Loc) { RBraceLoc = Loc; }

bool isSemanticForm() const { return AltForm.getInt(); }
InitListExpr *getSemanticForm() const {
return isSemanticForm() ? nullptr : AltForm.getPointer();
}
bool isSyntacticForm() const {
return !AltForm.getInt() || !AltForm.getPointer();
}
InitListExpr *getSyntacticForm() const {
return isSemanticForm() ? AltForm.getPointer() : nullptr;
}

void setSyntacticForm(InitListExpr *Init) {
AltForm.setPointer(Init);
AltForm.setInt(true);
Init->AltForm.setPointer(this);
Init->AltForm.setInt(false);
}

bool hadArrayRangeDesignator() const {
return InitListExprBits.HadArrayRangeDesignator != 0;
}
void sawArrayRangeDesignator(bool ARD = true) {
InitListExprBits.HadArrayRangeDesignator = ARD;
}

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == InitListExprClass;
}


child_range children() {
const_child_range CCR = const_cast<const InitListExpr *>(this)->children();
return child_range(cast_away_const(CCR.begin()),
cast_away_const(CCR.end()));
}

const_child_range children() const {

if (InitExprs.empty())
return const_child_range(const_child_iterator(), const_child_iterator());
return const_child_range(&InitExprs[0], &InitExprs[0] + InitExprs.size());
}

typedef InitExprsTy::iterator iterator;
typedef InitExprsTy::const_iterator const_iterator;
typedef InitExprsTy::reverse_iterator reverse_iterator;
typedef InitExprsTy::const_reverse_iterator const_reverse_iterator;

iterator begin() { return InitExprs.begin(); }
const_iterator begin() const { return InitExprs.begin(); }
iterator end() { return InitExprs.end(); }
const_iterator end() const { return InitExprs.end(); }
reverse_iterator rbegin() { return InitExprs.rbegin(); }
const_reverse_iterator rbegin() const { return InitExprs.rbegin(); }
reverse_iterator rend() { return InitExprs.rend(); }
const_reverse_iterator rend() const { return InitExprs.rend(); }

friend class ASTStmtReader;
friend class ASTStmtWriter;
};





















class DesignatedInitExpr final
: public Expr,
private llvm::TrailingObjects<DesignatedInitExpr, Stmt *> {
public:

class Designator;

private:


SourceLocation EqualOrColonLoc;



unsigned GNUSyntax : 1;


unsigned NumDesignators : 15;




unsigned NumSubExprs : 16;



Designator *Designators;

DesignatedInitExpr(const ASTContext &C, QualType Ty,
llvm::ArrayRef<Designator> Designators,
SourceLocation EqualOrColonLoc, bool GNUSyntax,
ArrayRef<Expr *> IndexExprs, Expr *Init);

explicit DesignatedInitExpr(unsigned NumSubExprs)
: Expr(DesignatedInitExprClass, EmptyShell()),
NumDesignators(0), NumSubExprs(NumSubExprs), Designators(nullptr) { }

public:

struct FieldDesignator {






uintptr_t NameOrField;


SourceLocation DotLoc;


SourceLocation FieldLoc;
};


struct ArrayOrRangeDesignator {


unsigned Index;

SourceLocation LBracketLoc;


SourceLocation EllipsisLoc;

SourceLocation RBracketLoc;
};







class Designator {

enum {
FieldDesignator,
ArrayDesignator,
ArrayRangeDesignator
} Kind;

union {

struct FieldDesignator Field;

struct ArrayOrRangeDesignator ArrayOrRange;
};
friend class DesignatedInitExpr;

public:
Designator() {}


Designator(const IdentifierInfo *FieldName, SourceLocation DotLoc,
SourceLocation FieldLoc)
: Kind(FieldDesignator) {
new (&Field) DesignatedInitExpr::FieldDesignator;
Field.NameOrField = reinterpret_cast<uintptr_t>(FieldName) | 0x01;
Field.DotLoc = DotLoc;
Field.FieldLoc = FieldLoc;
}


Designator(unsigned Index, SourceLocation LBracketLoc,
SourceLocation RBracketLoc)
: Kind(ArrayDesignator) {
new (&ArrayOrRange) DesignatedInitExpr::ArrayOrRangeDesignator;
ArrayOrRange.Index = Index;
ArrayOrRange.LBracketLoc = LBracketLoc;
ArrayOrRange.EllipsisLoc = SourceLocation();
ArrayOrRange.RBracketLoc = RBracketLoc;
}


Designator(unsigned Index, SourceLocation LBracketLoc,
SourceLocation EllipsisLoc, SourceLocation RBracketLoc)
: Kind(ArrayRangeDesignator) {
new (&ArrayOrRange) DesignatedInitExpr::ArrayOrRangeDesignator;
ArrayOrRange.Index = Index;
ArrayOrRange.LBracketLoc = LBracketLoc;
ArrayOrRange.EllipsisLoc = EllipsisLoc;
ArrayOrRange.RBracketLoc = RBracketLoc;
}

bool isFieldDesignator() const { return Kind == FieldDesignator; }
bool isArrayDesignator() const { return Kind == ArrayDesignator; }
bool isArrayRangeDesignator() const { return Kind == ArrayRangeDesignator; }

IdentifierInfo *getFieldName() const;

FieldDecl *getField() const {
assert(Kind == FieldDesignator && "Only valid on a field designator");
if (Field.NameOrField & 0x01)
return nullptr;
else
return reinterpret_cast<FieldDecl *>(Field.NameOrField);
}

void setField(FieldDecl *FD) {
assert(Kind == FieldDesignator && "Only valid on a field designator");
Field.NameOrField = reinterpret_cast<uintptr_t>(FD);
}

SourceLocation getDotLoc() const {
assert(Kind == FieldDesignator && "Only valid on a field designator");
return Field.DotLoc;
}

SourceLocation getFieldLoc() const {
assert(Kind == FieldDesignator && "Only valid on a field designator");
return Field.FieldLoc;
}

SourceLocation getLBracketLoc() const {
assert((Kind == ArrayDesignator || Kind == ArrayRangeDesignator) &&
"Only valid on an array or array-range designator");
return ArrayOrRange.LBracketLoc;
}

SourceLocation getRBracketLoc() const {
assert((Kind == ArrayDesignator || Kind == ArrayRangeDesignator) &&
"Only valid on an array or array-range designator");
return ArrayOrRange.RBracketLoc;
}

SourceLocation getEllipsisLoc() const {
assert(Kind == ArrayRangeDesignator &&
"Only valid on an array-range designator");
return ArrayOrRange.EllipsisLoc;
}

unsigned getFirstExprIndex() const {
assert((Kind == ArrayDesignator || Kind == ArrayRangeDesignator) &&
"Only valid on an array or array-range designator");
return ArrayOrRange.Index;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
if (Kind == FieldDesignator)
return getDotLoc().isInvalid()? getFieldLoc() : getDotLoc();
else
return getLBracketLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return Kind == FieldDesignator ? getFieldLoc() : getRBracketLoc();
}
SourceRange getSourceRange() const LLVM_READONLY {
return SourceRange(getBeginLoc(), getEndLoc());
}
};

static DesignatedInitExpr *Create(const ASTContext &C,
llvm::ArrayRef<Designator> Designators,
ArrayRef<Expr*> IndexExprs,
SourceLocation EqualOrColonLoc,
bool GNUSyntax, Expr *Init);

static DesignatedInitExpr *CreateEmpty(const ASTContext &C,
unsigned NumIndexExprs);


unsigned size() const { return NumDesignators; }


llvm::MutableArrayRef<Designator> designators() {
return {Designators, NumDesignators};
}

llvm::ArrayRef<Designator> designators() const {
return {Designators, NumDesignators};
}

Designator *getDesignator(unsigned Idx) { return &designators()[Idx]; }
const Designator *getDesignator(unsigned Idx) const {
return &designators()[Idx];
}

void setDesignators(const ASTContext &C, const Designator *Desigs,
unsigned NumDesigs);

Expr *getArrayIndex(const Designator &D) const;
Expr *getArrayRangeStart(const Designator &D) const;
Expr *getArrayRangeEnd(const Designator &D) const;



SourceLocation getEqualOrColonLoc() const { return EqualOrColonLoc; }
void setEqualOrColonLoc(SourceLocation L) { EqualOrColonLoc = L; }



bool isDirectInit() const { return EqualOrColonLoc.isInvalid(); }



bool usesGNUSyntax() const { return GNUSyntax; }
void setGNUSyntax(bool GNU) { GNUSyntax = GNU; }


Expr *getInit() const {
return cast<Expr>(*const_cast<DesignatedInitExpr*>(this)->child_begin());
}

void setInit(Expr *init) {
*child_begin() = init;
}





unsigned getNumSubExprs() const { return NumSubExprs; }

Expr *getSubExpr(unsigned Idx) const {
assert(Idx < NumSubExprs && "Subscript out of range");
return cast<Expr>(getTrailingObjects<Stmt *>()[Idx]);
}

void setSubExpr(unsigned Idx, Expr *E) {
assert(Idx < NumSubExprs && "Subscript out of range");
getTrailingObjects<Stmt *>()[Idx] = E;
}



void ExpandDesignator(const ASTContext &C, unsigned Idx,
const Designator *First, const Designator *Last);

SourceRange getDesignatorsSourceRange() const;

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == DesignatedInitExprClass;
}


child_range children() {
Stmt **begin = getTrailingObjects<Stmt *>();
return child_range(begin, begin + NumSubExprs);
}
const_child_range children() const {
Stmt * const *begin = getTrailingObjects<Stmt *>();
return const_child_range(begin, begin + NumSubExprs);
}

friend TrailingObjects;
};










class NoInitExpr : public Expr {
public:
explicit NoInitExpr(QualType ty)
: Expr(NoInitExprClass, ty, VK_PRValue, OK_Ordinary) {
setDependence(computeDependence(this));
}

explicit NoInitExpr(EmptyShell Empty)
: Expr(NoInitExprClass, Empty) { }

static bool classof(const Stmt *T) {
return T->getStmtClass() == NoInitExprClass;
}

SourceLocation getBeginLoc() const LLVM_READONLY { return SourceLocation(); }
SourceLocation getEndLoc() const LLVM_READONLY { return SourceLocation(); }


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};












class DesignatedInitUpdateExpr : public Expr {


Stmt *BaseAndUpdaterExprs[2];

public:
DesignatedInitUpdateExpr(const ASTContext &C, SourceLocation lBraceLoc,
Expr *baseExprs, SourceLocation rBraceLoc);

explicit DesignatedInitUpdateExpr(EmptyShell Empty)
: Expr(DesignatedInitUpdateExprClass, Empty) { }

SourceLocation getBeginLoc() const LLVM_READONLY;
SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == DesignatedInitUpdateExprClass;
}

Expr *getBase() const { return cast<Expr>(BaseAndUpdaterExprs[0]); }
void setBase(Expr *Base) { BaseAndUpdaterExprs[0] = Base; }

InitListExpr *getUpdater() const {
return cast<InitListExpr>(BaseAndUpdaterExprs[1]);
}
void setUpdater(Expr *Updater) { BaseAndUpdaterExprs[1] = Updater; }



child_range children() {
return child_range(&BaseAndUpdaterExprs[0], &BaseAndUpdaterExprs[0] + 2);
}
const_child_range children() const {
return const_child_range(&BaseAndUpdaterExprs[0],
&BaseAndUpdaterExprs[0] + 2);
}
};

















class ArrayInitLoopExpr : public Expr {
Stmt *SubExprs[2];

explicit ArrayInitLoopExpr(EmptyShell Empty)
: Expr(ArrayInitLoopExprClass, Empty), SubExprs{} {}

public:
explicit ArrayInitLoopExpr(QualType T, Expr *CommonInit, Expr *ElementInit)
: Expr(ArrayInitLoopExprClass, T, VK_PRValue, OK_Ordinary),
SubExprs{CommonInit, ElementInit} {
setDependence(computeDependence(this));
}



OpaqueValueExpr *getCommonExpr() const {
return cast<OpaqueValueExpr>(SubExprs[0]);
}


Expr *getSubExpr() const { return cast<Expr>(SubExprs[1]); }

llvm::APInt getArraySize() const {
return cast<ConstantArrayType>(getType()->castAsArrayTypeUnsafe())
->getSize();
}

static bool classof(const Stmt *S) {
return S->getStmtClass() == ArrayInitLoopExprClass;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return getCommonExpr()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getCommonExpr()->getEndLoc();
}

child_range children() {
return child_range(SubExprs, SubExprs + 2);
}
const_child_range children() const {
return const_child_range(SubExprs, SubExprs + 2);
}

friend class ASTReader;
friend class ASTStmtReader;
friend class ASTStmtWriter;
};




class ArrayInitIndexExpr : public Expr {
explicit ArrayInitIndexExpr(EmptyShell Empty)
: Expr(ArrayInitIndexExprClass, Empty) {}

public:
explicit ArrayInitIndexExpr(QualType T)
: Expr(ArrayInitIndexExprClass, T, VK_PRValue, OK_Ordinary) {
setDependence(ExprDependence::None);
}

static bool classof(const Stmt *S) {
return S->getStmtClass() == ArrayInitIndexExprClass;
}

SourceLocation getBeginLoc() const LLVM_READONLY { return SourceLocation(); }
SourceLocation getEndLoc() const LLVM_READONLY { return SourceLocation(); }

child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

friend class ASTReader;
friend class ASTStmtReader;
};









class ImplicitValueInitExpr : public Expr {
public:
explicit ImplicitValueInitExpr(QualType ty)
: Expr(ImplicitValueInitExprClass, ty, VK_PRValue, OK_Ordinary) {
setDependence(computeDependence(this));
}


explicit ImplicitValueInitExpr(EmptyShell Empty)
: Expr(ImplicitValueInitExprClass, Empty) { }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ImplicitValueInitExprClass;
}

SourceLocation getBeginLoc() const LLVM_READONLY { return SourceLocation(); }
SourceLocation getEndLoc() const LLVM_READONLY { return SourceLocation(); }


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};

class ParenListExpr final
: public Expr,
private llvm::TrailingObjects<ParenListExpr, Stmt *> {
friend class ASTStmtReader;
friend TrailingObjects;


SourceLocation LParenLoc, RParenLoc;


ParenListExpr(SourceLocation LParenLoc, ArrayRef<Expr *> Exprs,
SourceLocation RParenLoc);


ParenListExpr(EmptyShell Empty, unsigned NumExprs);

public:

static ParenListExpr *Create(const ASTContext &Ctx, SourceLocation LParenLoc,
ArrayRef<Expr *> Exprs,
SourceLocation RParenLoc);


static ParenListExpr *CreateEmpty(const ASTContext &Ctx, unsigned NumExprs);


unsigned getNumExprs() const { return ParenListExprBits.NumExprs; }

Expr *getExpr(unsigned Init) {
assert(Init < getNumExprs() && "Initializer access out of range!");
return getExprs()[Init];
}

const Expr *getExpr(unsigned Init) const {
return const_cast<ParenListExpr *>(this)->getExpr(Init);
}

Expr **getExprs() {
return reinterpret_cast<Expr **>(getTrailingObjects<Stmt *>());
}

ArrayRef<Expr *> exprs() {
return llvm::makeArrayRef(getExprs(), getNumExprs());
}

SourceLocation getLParenLoc() const { return LParenLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
SourceLocation getBeginLoc() const { return getLParenLoc(); }
SourceLocation getEndLoc() const { return getRParenLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ParenListExprClass;
}


child_range children() {
return child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() + getNumExprs());
}
const_child_range children() const {
return const_child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() + getNumExprs());
}
};



























class GenericSelectionExpr final
: public Expr,
private llvm::TrailingObjects<GenericSelectionExpr, Stmt *,
TypeSourceInfo *> {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;





unsigned NumAssocs, ResultIndex;
enum : unsigned {
ResultDependentIndex = std::numeric_limits<unsigned>::max(),
ControllingIndex = 0,
AssocExprStartIndex = 1
};


SourceLocation DefaultLoc, RParenLoc;








unsigned numTrailingObjects(OverloadToken<Stmt *>) const {


return 1 + getNumAssocs();
}

unsigned numTrailingObjects(OverloadToken<TypeSourceInfo *>) const {
return getNumAssocs();
}

template <bool Const> class AssociationIteratorTy;



template <bool Const> class AssociationTy {
friend class GenericSelectionExpr;
template <bool OtherConst> friend class AssociationIteratorTy;
using ExprPtrTy = std::conditional_t<Const, const Expr *, Expr *>;
using TSIPtrTy =
std::conditional_t<Const, const TypeSourceInfo *, TypeSourceInfo *>;
ExprPtrTy E;
TSIPtrTy TSI;
bool Selected;
AssociationTy(ExprPtrTy E, TSIPtrTy TSI, bool Selected)
: E(E), TSI(TSI), Selected(Selected) {}

public:
ExprPtrTy getAssociationExpr() const { return E; }
TSIPtrTy getTypeSourceInfo() const { return TSI; }
QualType getType() const { return TSI ? TSI->getType() : QualType(); }
bool isSelected() const { return Selected; }
AssociationTy *operator->() { return this; }
const AssociationTy *operator->() const { return this; }
};





template <bool Const>
class AssociationIteratorTy
: public llvm::iterator_facade_base<
AssociationIteratorTy<Const>, std::input_iterator_tag,
AssociationTy<Const>, std::ptrdiff_t, AssociationTy<Const>,
AssociationTy<Const>> {
friend class GenericSelectionExpr;













using BaseTy = typename AssociationIteratorTy::iterator_facade_base;
using StmtPtrPtrTy =
std::conditional_t<Const, const Stmt *const *, Stmt **>;
using TSIPtrPtrTy = std::conditional_t<Const, const TypeSourceInfo *const *,
TypeSourceInfo **>;
StmtPtrPtrTy E;
TSIPtrPtrTy TSI;
unsigned Offset = 0, SelectedOffset = 0;
AssociationIteratorTy(StmtPtrPtrTy E, TSIPtrPtrTy TSI, unsigned Offset,
unsigned SelectedOffset)
: E(E), TSI(TSI), Offset(Offset), SelectedOffset(SelectedOffset) {}

public:
AssociationIteratorTy() : E(nullptr), TSI(nullptr) {}
typename BaseTy::reference operator*() const {
return AssociationTy<Const>(cast<Expr>(*E), *TSI,
Offset == SelectedOffset);
}
typename BaseTy::pointer operator->() const { return **this; }
using BaseTy::operator++;
AssociationIteratorTy &operator++() {
++E;
++TSI;
++Offset;
return *this;
}
bool operator==(AssociationIteratorTy Other) const { return E == Other.E; }
};


GenericSelectionExpr(const ASTContext &Context, SourceLocation GenericLoc,
Expr *ControllingExpr,
ArrayRef<TypeSourceInfo *> AssocTypes,
ArrayRef<Expr *> AssocExprs, SourceLocation DefaultLoc,
SourceLocation RParenLoc,
bool ContainsUnexpandedParameterPack,
unsigned ResultIndex);


GenericSelectionExpr(const ASTContext &Context, SourceLocation GenericLoc,
Expr *ControllingExpr,
ArrayRef<TypeSourceInfo *> AssocTypes,
ArrayRef<Expr *> AssocExprs, SourceLocation DefaultLoc,
SourceLocation RParenLoc,
bool ContainsUnexpandedParameterPack);


explicit GenericSelectionExpr(EmptyShell Empty, unsigned NumAssocs);

public:

static GenericSelectionExpr *
Create(const ASTContext &Context, SourceLocation GenericLoc,
Expr *ControllingExpr, ArrayRef<TypeSourceInfo *> AssocTypes,
ArrayRef<Expr *> AssocExprs, SourceLocation DefaultLoc,
SourceLocation RParenLoc, bool ContainsUnexpandedParameterPack,
unsigned ResultIndex);


static GenericSelectionExpr *
Create(const ASTContext &Context, SourceLocation GenericLoc,
Expr *ControllingExpr, ArrayRef<TypeSourceInfo *> AssocTypes,
ArrayRef<Expr *> AssocExprs, SourceLocation DefaultLoc,
SourceLocation RParenLoc, bool ContainsUnexpandedParameterPack);


static GenericSelectionExpr *CreateEmpty(const ASTContext &Context,
unsigned NumAssocs);

using Association = AssociationTy<false>;
using ConstAssociation = AssociationTy<true>;
using AssociationIterator = AssociationIteratorTy<false>;
using ConstAssociationIterator = AssociationIteratorTy<true>;
using association_range = llvm::iterator_range<AssociationIterator>;
using const_association_range =
llvm::iterator_range<ConstAssociationIterator>;


unsigned getNumAssocs() const { return NumAssocs; }




unsigned getResultIndex() const {
assert(!isResultDependent() &&
"Generic selection is result-dependent but getResultIndex called!");
return ResultIndex;
}


bool isResultDependent() const { return ResultIndex == ResultDependentIndex; }


Expr *getControllingExpr() {
return cast<Expr>(getTrailingObjects<Stmt *>()[ControllingIndex]);
}
const Expr *getControllingExpr() const {
return cast<Expr>(getTrailingObjects<Stmt *>()[ControllingIndex]);
}



Expr *getResultExpr() {
return cast<Expr>(
getTrailingObjects<Stmt *>()[AssocExprStartIndex + getResultIndex()]);
}
const Expr *getResultExpr() const {
return cast<Expr>(
getTrailingObjects<Stmt *>()[AssocExprStartIndex + getResultIndex()]);
}

ArrayRef<Expr *> getAssocExprs() const {
return {reinterpret_cast<Expr *const *>(getTrailingObjects<Stmt *>() +
AssocExprStartIndex),
NumAssocs};
}
ArrayRef<TypeSourceInfo *> getAssocTypeSourceInfos() const {
return {getTrailingObjects<TypeSourceInfo *>(), NumAssocs};
}



Association getAssociation(unsigned I) {
assert(I < getNumAssocs() &&
"Out-of-range index in GenericSelectionExpr::getAssociation!");
return Association(
cast<Expr>(getTrailingObjects<Stmt *>()[AssocExprStartIndex + I]),
getTrailingObjects<TypeSourceInfo *>()[I],
!isResultDependent() && (getResultIndex() == I));
}
ConstAssociation getAssociation(unsigned I) const {
assert(I < getNumAssocs() &&
"Out-of-range index in GenericSelectionExpr::getAssociation!");
return ConstAssociation(
cast<Expr>(getTrailingObjects<Stmt *>()[AssocExprStartIndex + I]),
getTrailingObjects<TypeSourceInfo *>()[I],
!isResultDependent() && (getResultIndex() == I));
}

association_range associations() {
AssociationIterator Begin(getTrailingObjects<Stmt *>() +
AssocExprStartIndex,
getTrailingObjects<TypeSourceInfo *>(),
0, ResultIndex);
AssociationIterator End(Begin.E + NumAssocs, Begin.TSI + NumAssocs,
NumAssocs, ResultIndex);
return llvm::make_range(Begin, End);
}

const_association_range associations() const {
ConstAssociationIterator Begin(getTrailingObjects<Stmt *>() +
AssocExprStartIndex,
getTrailingObjects<TypeSourceInfo *>(),
0, ResultIndex);
ConstAssociationIterator End(Begin.E + NumAssocs, Begin.TSI + NumAssocs,
NumAssocs, ResultIndex);
return llvm::make_range(Begin, End);
}

SourceLocation getGenericLoc() const {
return GenericSelectionExprBits.GenericLoc;
}
SourceLocation getDefaultLoc() const { return DefaultLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
SourceLocation getBeginLoc() const { return getGenericLoc(); }
SourceLocation getEndLoc() const { return getRParenLoc(); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == GenericSelectionExprClass;
}

child_range children() {
return child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}
const_child_range children() const {
return const_child_range(getTrailingObjects<Stmt *>(),
getTrailingObjects<Stmt *>() +
numTrailingObjects(OverloadToken<Stmt *>()));
}
};












class ExtVectorElementExpr : public Expr {
Stmt *Base;
IdentifierInfo *Accessor;
SourceLocation AccessorLoc;
public:
ExtVectorElementExpr(QualType ty, ExprValueKind VK, Expr *base,
IdentifierInfo &accessor, SourceLocation loc)
: Expr(ExtVectorElementExprClass, ty, VK,
(VK == VK_PRValue ? OK_Ordinary : OK_VectorComponent)),
Base(base), Accessor(&accessor), AccessorLoc(loc) {
setDependence(computeDependence(this));
}


explicit ExtVectorElementExpr(EmptyShell Empty)
: Expr(ExtVectorElementExprClass, Empty) { }

const Expr *getBase() const { return cast<Expr>(Base); }
Expr *getBase() { return cast<Expr>(Base); }
void setBase(Expr *E) { Base = E; }

IdentifierInfo &getAccessor() const { return *Accessor; }
void setAccessor(IdentifierInfo *II) { Accessor = II; }

SourceLocation getAccessorLoc() const { return AccessorLoc; }
void setAccessorLoc(SourceLocation L) { AccessorLoc = L; }


unsigned getNumElements() const;



bool containsDuplicateElements() const;



void getEncodedElementAccess(SmallVectorImpl<uint32_t> &Elts) const;

SourceLocation getBeginLoc() const LLVM_READONLY {
return getBase()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY { return AccessorLoc; }



bool isArrow() const;

static bool classof(const Stmt *T) {
return T->getStmtClass() == ExtVectorElementExprClass;
}


child_range children() { return child_range(&Base, &Base+1); }
const_child_range children() const {
return const_child_range(&Base, &Base + 1);
}
};



class BlockExpr : public Expr {
protected:
BlockDecl *TheBlock;
public:
BlockExpr(BlockDecl *BD, QualType ty)
: Expr(BlockExprClass, ty, VK_PRValue, OK_Ordinary), TheBlock(BD) {
setDependence(computeDependence(this));
}


explicit BlockExpr(EmptyShell Empty) : Expr(BlockExprClass, Empty) { }

const BlockDecl *getBlockDecl() const { return TheBlock; }
BlockDecl *getBlockDecl() { return TheBlock; }
void setBlockDecl(BlockDecl *BD) { TheBlock = BD; }


SourceLocation getCaretLocation() const;
const Stmt *getBody() const;
Stmt *getBody();

SourceLocation getBeginLoc() const LLVM_READONLY {
return getCaretLocation();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getBody()->getEndLoc();
}


const FunctionProtoType *getFunctionType() const;

static bool classof(const Stmt *T) {
return T->getStmtClass() == BlockExprClass;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};



struct BlockVarCopyInit {
BlockVarCopyInit() = default;
BlockVarCopyInit(Expr *CopyExpr, bool CanThrow)
: ExprAndFlag(CopyExpr, CanThrow) {}
void setExprAndFlag(Expr *CopyExpr, bool CanThrow) {
ExprAndFlag.setPointerAndInt(CopyExpr, CanThrow);
}
Expr *getCopyExpr() const { return ExprAndFlag.getPointer(); }
bool canThrow() const { return ExprAndFlag.getInt(); }
llvm::PointerIntPair<Expr *, 1, bool> ExprAndFlag;
};




class AsTypeExpr : public Expr {
private:
Stmt *SrcExpr;
SourceLocation BuiltinLoc, RParenLoc;

friend class ASTReader;
friend class ASTStmtReader;
explicit AsTypeExpr(EmptyShell Empty) : Expr(AsTypeExprClass, Empty) {}

public:
AsTypeExpr(Expr *SrcExpr, QualType DstType, ExprValueKind VK,
ExprObjectKind OK, SourceLocation BuiltinLoc,
SourceLocation RParenLoc)
: Expr(AsTypeExprClass, DstType, VK, OK), SrcExpr(SrcExpr),
BuiltinLoc(BuiltinLoc), RParenLoc(RParenLoc) {
setDependence(computeDependence(this));
}


Expr *getSrcExpr() const { return cast<Expr>(SrcExpr); }


SourceLocation getBuiltinLoc() const { return BuiltinLoc; }


SourceLocation getRParenLoc() const { return RParenLoc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return BuiltinLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == AsTypeExprClass;
}


child_range children() { return child_range(&SrcExpr, &SrcExpr+1); }
const_child_range children() const {
return const_child_range(&SrcExpr, &SrcExpr + 1);
}
};





























class PseudoObjectExpr final
: public Expr,
private llvm::TrailingObjects<PseudoObjectExpr, Expr *> {











Expr **getSubExprsBuffer() { return getTrailingObjects<Expr *>(); }
const Expr * const *getSubExprsBuffer() const {
return getTrailingObjects<Expr *>();
}

PseudoObjectExpr(QualType type, ExprValueKind VK,
Expr *syntactic, ArrayRef<Expr*> semantic,
unsigned resultIndex);

PseudoObjectExpr(EmptyShell shell, unsigned numSemanticExprs);

unsigned getNumSubExprs() const {
return PseudoObjectExprBits.NumSubExprs;
}

public:


enum : unsigned { NoResult = ~0U };

static PseudoObjectExpr *Create(const ASTContext &Context, Expr *syntactic,
ArrayRef<Expr*> semantic,
unsigned resultIndex);

static PseudoObjectExpr *Create(const ASTContext &Context, EmptyShell shell,
unsigned numSemanticExprs);




Expr *getSyntacticForm() { return getSubExprsBuffer()[0]; }
const Expr *getSyntacticForm() const { return getSubExprsBuffer()[0]; }



unsigned getResultExprIndex() const {
if (PseudoObjectExprBits.ResultIndex == 0) return NoResult;
return PseudoObjectExprBits.ResultIndex - 1;
}


Expr *getResultExpr() {
if (PseudoObjectExprBits.ResultIndex == 0)
return nullptr;
return getSubExprsBuffer()[PseudoObjectExprBits.ResultIndex];
}
const Expr *getResultExpr() const {
return const_cast<PseudoObjectExpr*>(this)->getResultExpr();
}

unsigned getNumSemanticExprs() const { return getNumSubExprs() - 1; }

typedef Expr * const *semantics_iterator;
typedef const Expr * const *const_semantics_iterator;
semantics_iterator semantics_begin() {
return getSubExprsBuffer() + 1;
}
const_semantics_iterator semantics_begin() const {
return getSubExprsBuffer() + 1;
}
semantics_iterator semantics_end() {
return getSubExprsBuffer() + getNumSubExprs();
}
const_semantics_iterator semantics_end() const {
return getSubExprsBuffer() + getNumSubExprs();
}

llvm::iterator_range<semantics_iterator> semantics() {
return llvm::make_range(semantics_begin(), semantics_end());
}
llvm::iterator_range<const_semantics_iterator> semantics() const {
return llvm::make_range(semantics_begin(), semantics_end());
}

Expr *getSemanticExpr(unsigned index) {
assert(index + 1 < getNumSubExprs());
return getSubExprsBuffer()[index + 1];
}
const Expr *getSemanticExpr(unsigned index) const {
return const_cast<PseudoObjectExpr*>(this)->getSemanticExpr(index);
}

SourceLocation getExprLoc() const LLVM_READONLY {
return getSyntacticForm()->getExprLoc();
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return getSyntacticForm()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getSyntacticForm()->getEndLoc();
}

child_range children() {
const_child_range CCR =
const_cast<const PseudoObjectExpr *>(this)->children();
return child_range(cast_away_const(CCR.begin()),
cast_away_const(CCR.end()));
}
const_child_range children() const {
Stmt *const *cs = const_cast<Stmt *const *>(
reinterpret_cast<const Stmt *const *>(getSubExprsBuffer()));
return const_child_range(cs, cs + getNumSubExprs());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == PseudoObjectExprClass;
}

friend TrailingObjects;
friend class ASTStmtReader;
};








class AtomicExpr : public Expr {
public:
enum AtomicOp {
#define BUILTIN(ID, TYPE, ATTRS)
#define ATOMIC_BUILTIN(ID, TYPE, ATTRS) AO ##ID,
#include "clang/Basic/Builtins.def"

BI_First = 0
};

private:



enum { PTR, ORDER, VAL1, ORDER_FAIL, VAL2, WEAK, END_EXPR };
Stmt *SubExprs[END_EXPR + 1];
unsigned NumSubExprs;
SourceLocation BuiltinLoc, RParenLoc;
AtomicOp Op;

friend class ASTStmtReader;
public:
AtomicExpr(SourceLocation BLoc, ArrayRef<Expr*> args, QualType t,
AtomicOp op, SourceLocation RP);



static unsigned getNumSubExprs(AtomicOp Op);


explicit AtomicExpr(EmptyShell Empty) : Expr(AtomicExprClass, Empty) { }

Expr *getPtr() const {
return cast<Expr>(SubExprs[PTR]);
}
Expr *getOrder() const {
return cast<Expr>(SubExprs[ORDER]);
}
Expr *getScope() const {
assert(getScopeModel() && "No scope");
return cast<Expr>(SubExprs[NumSubExprs - 1]);
}
Expr *getVal1() const {
if (Op == AO__c11_atomic_init || Op == AO__opencl_atomic_init)
return cast<Expr>(SubExprs[ORDER]);
assert(NumSubExprs > VAL1);
return cast<Expr>(SubExprs[VAL1]);
}
Expr *getOrderFail() const {
assert(NumSubExprs > ORDER_FAIL);
return cast<Expr>(SubExprs[ORDER_FAIL]);
}
Expr *getVal2() const {
if (Op == AO__atomic_exchange)
return cast<Expr>(SubExprs[ORDER_FAIL]);
assert(NumSubExprs > VAL2);
return cast<Expr>(SubExprs[VAL2]);
}
Expr *getWeak() const {
assert(NumSubExprs > WEAK);
return cast<Expr>(SubExprs[WEAK]);
}
QualType getValueType() const;

AtomicOp getOp() const { return Op; }
unsigned getNumSubExprs() const { return NumSubExprs; }

Expr **getSubExprs() { return reinterpret_cast<Expr **>(SubExprs); }
const Expr * const *getSubExprs() const {
return reinterpret_cast<Expr * const *>(SubExprs);
}

bool isVolatile() const {
return getPtr()->getType()->getPointeeType().isVolatileQualified();
}

bool isCmpXChg() const {
return getOp() == AO__c11_atomic_compare_exchange_strong ||
getOp() == AO__c11_atomic_compare_exchange_weak ||
getOp() == AO__opencl_atomic_compare_exchange_strong ||
getOp() == AO__opencl_atomic_compare_exchange_weak ||
getOp() == AO__atomic_compare_exchange ||
getOp() == AO__atomic_compare_exchange_n;
}

bool isOpenCL() const {
return getOp() >= AO__opencl_atomic_init &&
getOp() <= AO__opencl_atomic_fetch_max;
}

SourceLocation getBuiltinLoc() const { return BuiltinLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return BuiltinLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == AtomicExprClass;
}


child_range children() {
return child_range(SubExprs, SubExprs+NumSubExprs);
}
const_child_range children() const {
return const_child_range(SubExprs, SubExprs + NumSubExprs);
}




static std::unique_ptr<AtomicScopeModel> getScopeModel(AtomicOp Op) {
auto Kind =
(Op >= AO__opencl_atomic_load && Op <= AO__opencl_atomic_fetch_max)
? AtomicScopeModelKind::OpenCL
: AtomicScopeModelKind::None;
return AtomicScopeModel::create(Kind);
}




std::unique_ptr<AtomicScopeModel> getScopeModel() const {
return getScopeModel(getOp());
}
};



class TypoExpr : public Expr {

SourceLocation TypoLoc;

public:
TypoExpr(QualType T, SourceLocation TypoLoc)
: Expr(TypoExprClass, T, VK_LValue, OK_Ordinary), TypoLoc(TypoLoc) {
assert(T->isDependentType() && "TypoExpr given a non-dependent type");
setDependence(ExprDependence::TypeValueInstantiation |
ExprDependence::Error);
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

SourceLocation getBeginLoc() const LLVM_READONLY { return TypoLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return TypoLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == TypoExprClass;
}

};



























class RecoveryExpr final : public Expr,
private llvm::TrailingObjects<RecoveryExpr, Expr *> {
public:
static RecoveryExpr *Create(ASTContext &Ctx, QualType T,
SourceLocation BeginLoc, SourceLocation EndLoc,
ArrayRef<Expr *> SubExprs);
static RecoveryExpr *CreateEmpty(ASTContext &Ctx, unsigned NumSubExprs);

ArrayRef<Expr *> subExpressions() {
auto *B = getTrailingObjects<Expr *>();
return llvm::makeArrayRef(B, B + NumExprs);
}

ArrayRef<const Expr *> subExpressions() const {
return const_cast<RecoveryExpr *>(this)->subExpressions();
}

child_range children() {
Stmt **B = reinterpret_cast<Stmt **>(getTrailingObjects<Expr *>());
return child_range(B, B + NumExprs);
}

SourceLocation getBeginLoc() const { return BeginLoc; }
SourceLocation getEndLoc() const { return EndLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == RecoveryExprClass;
}

private:
RecoveryExpr(ASTContext &Ctx, QualType T, SourceLocation BeginLoc,
SourceLocation EndLoc, ArrayRef<Expr *> SubExprs);
RecoveryExpr(EmptyShell Empty, unsigned NumSubExprs)
: Expr(RecoveryExprClass, Empty), NumExprs(NumSubExprs) {}

size_t numTrailingObjects(OverloadToken<Stmt *>) const { return NumExprs; }

SourceLocation BeginLoc, EndLoc;
unsigned NumExprs;
friend TrailingObjects;
friend class ASTStmtReader;
friend class ASTStmtWriter;
};

}

#endif
