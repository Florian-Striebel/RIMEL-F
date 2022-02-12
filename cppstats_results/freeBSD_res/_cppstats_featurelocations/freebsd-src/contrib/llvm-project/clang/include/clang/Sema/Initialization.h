











#if !defined(LLVM_CLANG_SEMA_INITIALIZATION_H)
#define LLVM_CLANG_SEMA_INITIALIZATION_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclAccessPair.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Type.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Sema/Overload.h"
#include "clang/Sema/Ownership.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include <cassert>
#include <cstdint>
#include <string>

namespace clang {

class APValue;
class CXXBaseSpecifier;
class CXXConstructorDecl;
class ObjCMethodDecl;
class Sema;


class alignas(8) InitializedEntity {
public:

enum EntityKind {

EK_Variable,


EK_Parameter,


EK_TemplateParameter,


EK_Result,


EK_StmtExprResult,



EK_Exception,



EK_Member,


EK_ArrayElement,



EK_New,


EK_Temporary,


EK_Base,


EK_Delegating,



EK_VectorElement,



EK_BlockElement,



EK_LambdaToBlockConversionBlockElement,



EK_ComplexElement,



EK_LambdaCapture,



EK_CompoundLiteralInit,



EK_RelatedResult,



EK_Parameter_CF_Audited,



EK_Binding,




};

private:

EntityKind Kind;



const InitializedEntity *Parent = nullptr;


QualType Type;


mutable unsigned ManglingNumber = 0;

struct LN {




SourceLocation Location;



bool NRVO;
};

struct VD {

ValueDecl *VariableOrMember;




bool IsImplicitFieldInit;



bool IsDefaultMemberInit;
};

struct C {

IdentifierInfo *VarID;


SourceLocation Location;
};

union {


VD Variable;



ObjCMethodDecl *MethodDecl;



llvm::PointerIntPair<ParmVarDecl *, 1> Parameter;



TypeSourceInfo *TypeInfo;

struct LN LocAndNRVO;




llvm::PointerIntPair<const CXXBaseSpecifier *, 1> Base;




unsigned Index;

struct C Capture;
};

InitializedEntity() {};


InitializedEntity(VarDecl *Var, EntityKind EK = EK_Variable)
: Kind(EK), Type(Var->getType()), Variable{Var, false, false} {}




InitializedEntity(EntityKind Kind, SourceLocation Loc, QualType Type,
bool NRVO = false)
: Kind(Kind), Type(Type) {
new (&LocAndNRVO) LN;
LocAndNRVO.Location = Loc;
LocAndNRVO.NRVO = NRVO;
}


InitializedEntity(FieldDecl *Member, const InitializedEntity *Parent,
bool Implicit, bool DefaultMemberInit)
: Kind(EK_Member), Parent(Parent), Type(Member->getType()),
Variable{Member, Implicit, DefaultMemberInit} {}


InitializedEntity(ASTContext &Context, unsigned Index,
const InitializedEntity &Parent);


InitializedEntity(IdentifierInfo *VarID, QualType FieldType, SourceLocation Loc)
: Kind(EK_LambdaCapture), Type(FieldType) {
new (&Capture) C;
Capture.VarID = VarID;
Capture.Location = Loc;
}

public:

static InitializedEntity InitializeVariable(VarDecl *Var) {
return InitializedEntity(Var);
}


static InitializedEntity InitializeParameter(ASTContext &Context,
ParmVarDecl *Parm) {
return InitializeParameter(Context, Parm, Parm->getType());
}



static InitializedEntity
InitializeParameter(ASTContext &Context, ParmVarDecl *Parm, QualType Type) {
bool Consumed = (Context.getLangOpts().ObjCAutoRefCount &&
Parm->hasAttr<NSConsumedAttr>());

InitializedEntity Entity;
Entity.Kind = EK_Parameter;
Entity.Type =
Context.getVariableArrayDecayedType(Type.getUnqualifiedType());
Entity.Parent = nullptr;
Entity.Parameter = {Parm, Consumed};
return Entity;
}



static InitializedEntity InitializeParameter(ASTContext &Context,
QualType Type,
bool Consumed) {
InitializedEntity Entity;
Entity.Kind = EK_Parameter;
Entity.Type = Context.getVariableArrayDecayedType(Type);
Entity.Parent = nullptr;
Entity.Parameter = {nullptr, Consumed};
return Entity;
}


static InitializedEntity
InitializeTemplateParameter(QualType T, NonTypeTemplateParmDecl *Param) {
InitializedEntity Entity;
Entity.Kind = EK_TemplateParameter;
Entity.Type = T;
Entity.Parent = nullptr;
Entity.Variable = {Param, false, false};
return Entity;
}


static InitializedEntity InitializeResult(SourceLocation ReturnLoc,
QualType Type) {
return InitializedEntity(EK_Result, ReturnLoc, Type);
}

static InitializedEntity InitializeStmtExprResult(SourceLocation ReturnLoc,
QualType Type) {
return InitializedEntity(EK_StmtExprResult, ReturnLoc, Type);
}

static InitializedEntity InitializeBlock(SourceLocation BlockVarLoc,
QualType Type) {
return InitializedEntity(EK_BlockElement, BlockVarLoc, Type);
}

static InitializedEntity InitializeLambdaToBlock(SourceLocation BlockVarLoc,
QualType Type) {
return InitializedEntity(EK_LambdaToBlockConversionBlockElement,
BlockVarLoc, Type);
}


static InitializedEntity InitializeException(SourceLocation ThrowLoc,
QualType Type) {
return InitializedEntity(EK_Exception, ThrowLoc, Type);
}


static InitializedEntity InitializeNew(SourceLocation NewLoc, QualType Type) {
return InitializedEntity(EK_New, NewLoc, Type);
}


static InitializedEntity InitializeTemporary(QualType Type) {
return InitializeTemporary(nullptr, Type);
}


static InitializedEntity InitializeTemporary(TypeSourceInfo *TypeInfo) {
return InitializeTemporary(TypeInfo, TypeInfo->getType());
}


static InitializedEntity InitializeTemporary(TypeSourceInfo *TypeInfo,
QualType Type) {
InitializedEntity Result(EK_Temporary, SourceLocation(), Type);
Result.TypeInfo = TypeInfo;
return Result;
}


static InitializedEntity InitializeRelatedResult(ObjCMethodDecl *MD,
QualType Type) {
InitializedEntity Result(EK_RelatedResult, SourceLocation(), Type);
Result.MethodDecl = MD;
return Result;
}


static InitializedEntity
InitializeBase(ASTContext &Context, const CXXBaseSpecifier *Base,
bool IsInheritedVirtualBase,
const InitializedEntity *Parent = nullptr);


static InitializedEntity InitializeDelegation(QualType Type) {
return InitializedEntity(EK_Delegating, SourceLocation(), Type);
}


static InitializedEntity
InitializeMember(FieldDecl *Member,
const InitializedEntity *Parent = nullptr,
bool Implicit = false) {
return InitializedEntity(Member, Parent, Implicit, false);
}


static InitializedEntity
InitializeMember(IndirectFieldDecl *Member,
const InitializedEntity *Parent = nullptr,
bool Implicit = false) {
return InitializedEntity(Member->getAnonField(), Parent, Implicit, false);
}


static InitializedEntity
InitializeMemberFromDefaultMemberInitializer(FieldDecl *Member) {
return InitializedEntity(Member, nullptr, false, true);
}


static InitializedEntity InitializeElement(ASTContext &Context,
unsigned Index,
const InitializedEntity &Parent) {
return InitializedEntity(Context, Index, Parent);
}


static InitializedEntity InitializeBinding(VarDecl *Binding) {
return InitializedEntity(Binding, EK_Binding);
}




static InitializedEntity InitializeLambdaCapture(IdentifierInfo *VarID,
QualType FieldType,
SourceLocation Loc) {
return InitializedEntity(VarID, FieldType, Loc);
}


static InitializedEntity InitializeCompoundLiteralInit(TypeSourceInfo *TSI) {
InitializedEntity Result(EK_CompoundLiteralInit, SourceLocation(),
TSI->getType());
Result.TypeInfo = TSI;
return Result;
}


EntityKind getKind() const { return Kind; }




const InitializedEntity *getParent() const { return Parent; }


QualType getType() const { return Type; }



TypeSourceInfo *getTypeSourceInfo() const {
if (Kind == EK_Temporary || Kind == EK_CompoundLiteralInit)
return TypeInfo;

return nullptr;
}


DeclarationName getName() const;



ValueDecl *getDecl() const;


ObjCMethodDecl *getMethodDecl() const { return MethodDecl; }



bool allowsNRVO() const;

bool isParameterKind() const {
return (getKind() == EK_Parameter ||
getKind() == EK_Parameter_CF_Audited);
}

bool isParamOrTemplateParamKind() const {
return isParameterKind() || getKind() == EK_TemplateParameter;
}



bool isParameterConsumed() const {
assert(isParameterKind() && "Not a parameter");
return Parameter.getInt();
}


const CXXBaseSpecifier *getBaseSpecifier() const {
assert(getKind() == EK_Base && "Not a base specifier");
return Base.getPointer();
}


bool isInheritedVirtualBase() const {
assert(getKind() == EK_Base && "Not a base specifier");
return Base.getInt();
}


bool isVariableLengthArrayNew() const {
return getKind() == EK_New && dyn_cast_or_null<IncompleteArrayType>(
getType()->getAsArrayTypeUnsafe());
}



bool isImplicitMemberInitializer() const {
return getKind() == EK_Member && Variable.IsImplicitFieldInit;
}



bool isDefaultMemberInitializer() const {
return getKind() == EK_Member && Variable.IsDefaultMemberInit;
}



SourceLocation getReturnLoc() const {
assert(getKind() == EK_Result && "No 'return' location!");
return LocAndNRVO.Location;
}



SourceLocation getThrowLoc() const {
assert(getKind() == EK_Exception && "No 'throw' location!");
return LocAndNRVO.Location;
}



unsigned getElementIndex() const {
assert(getKind() == EK_ArrayElement || getKind() == EK_VectorElement ||
getKind() == EK_ComplexElement);
return Index;
}



void setElementIndex(unsigned Index) {
assert(getKind() == EK_ArrayElement || getKind() == EK_VectorElement ||
getKind() == EK_ComplexElement);
this->Index = Index;
}


StringRef getCapturedVarName() const {
assert(getKind() == EK_LambdaCapture && "Not a lambda capture!");
return Capture.VarID ? Capture.VarID->getName() : "this";
}



SourceLocation getCaptureLoc() const {
assert(getKind() == EK_LambdaCapture && "Not a lambda capture!");
return Capture.Location;
}

void setParameterCFAudited() {
Kind = EK_Parameter_CF_Audited;
}

unsigned allocateManglingNumber() const { return ++ManglingNumber; }



void dump() const;

private:
unsigned dumpImpl(raw_ostream &OS) const;
};




class InitializationKind {
public:

enum InitKind {

IK_Direct,


IK_DirectList,


IK_Copy,


IK_Default,


IK_Value
};

private:

enum InitContext {

IC_Normal,


IC_ExplicitConvs,


IC_Implicit,


IC_StaticCast,


IC_CStyleCast,


IC_FunctionalCast
};


InitKind Kind : 8;


InitContext Context : 8;


SourceLocation Locations[3];

InitializationKind(InitKind Kind, InitContext Context, SourceLocation Loc1,
SourceLocation Loc2, SourceLocation Loc3)
: Kind(Kind), Context(Context) {
Locations[0] = Loc1;
Locations[1] = Loc2;
Locations[2] = Loc3;
}

public:

static InitializationKind CreateDirect(SourceLocation InitLoc,
SourceLocation LParenLoc,
SourceLocation RParenLoc) {
return InitializationKind(IK_Direct, IC_Normal,
InitLoc, LParenLoc, RParenLoc);
}

static InitializationKind CreateDirectList(SourceLocation InitLoc) {
return InitializationKind(IK_DirectList, IC_Normal, InitLoc, InitLoc,
InitLoc);
}

static InitializationKind CreateDirectList(SourceLocation InitLoc,
SourceLocation LBraceLoc,
SourceLocation RBraceLoc) {
return InitializationKind(IK_DirectList, IC_Normal, InitLoc, LBraceLoc,
RBraceLoc);
}



static InitializationKind CreateCast(SourceRange TypeRange) {
return InitializationKind(IK_Direct, IC_StaticCast, TypeRange.getBegin(),
TypeRange.getBegin(), TypeRange.getEnd());
}


static InitializationKind CreateCStyleCast(SourceLocation StartLoc,
SourceRange TypeRange,
bool InitList) {


return InitializationKind(InitList ? IK_DirectList : IK_Direct,
IC_CStyleCast, StartLoc, TypeRange.getBegin(),
TypeRange.getEnd());
}


static InitializationKind CreateFunctionalCast(SourceRange TypeRange,
bool InitList) {
return InitializationKind(InitList ? IK_DirectList : IK_Direct,
IC_FunctionalCast, TypeRange.getBegin(),
TypeRange.getBegin(), TypeRange.getEnd());
}


static InitializationKind CreateCopy(SourceLocation InitLoc,
SourceLocation EqualLoc,
bool AllowExplicitConvs = false) {
return InitializationKind(IK_Copy,
AllowExplicitConvs? IC_ExplicitConvs : IC_Normal,
InitLoc, EqualLoc, EqualLoc);
}


static InitializationKind CreateDefault(SourceLocation InitLoc) {
return InitializationKind(IK_Default, IC_Normal, InitLoc, InitLoc, InitLoc);
}


static InitializationKind CreateValue(SourceLocation InitLoc,
SourceLocation LParenLoc,
SourceLocation RParenLoc,
bool isImplicit = false) {
return InitializationKind(IK_Value, isImplicit ? IC_Implicit : IC_Normal,
InitLoc, LParenLoc, RParenLoc);
}



static InitializationKind CreateForInit(SourceLocation Loc, bool DirectInit,
Expr *Init) {
if (!Init) return CreateDefault(Loc);
if (!DirectInit)
return CreateCopy(Loc, Init->getBeginLoc());
if (isa<InitListExpr>(Init))
return CreateDirectList(Loc, Init->getBeginLoc(), Init->getEndLoc());
return CreateDirect(Loc, Init->getBeginLoc(), Init->getEndLoc());
}


InitKind getKind() const {
return Kind;
}


bool isExplicitCast() const {
return Context >= IC_StaticCast;
}


bool isStaticCast() const { return Context == IC_StaticCast; }


bool isCStyleOrFunctionalCast() const {
return Context >= IC_CStyleCast;
}


bool isCStyleCast() const {
return Context == IC_CStyleCast;
}


bool isFunctionalCast() const {
return Context == IC_FunctionalCast;
}




bool isImplicitValueInit() const { return Context == IC_Implicit; }


SourceLocation getLocation() const { return Locations[0]; }


SourceRange getRange() const {
return SourceRange(Locations[0], Locations[2]);
}



SourceLocation getEqualLoc() const {
assert(Kind == IK_Copy && "Only copy initialization has an '='");
return Locations[1];
}

bool isCopyInit() const { return Kind == IK_Copy; }



bool AllowExplicit() const { return !isCopyInit(); }





bool allowExplicitConversionFunctionsInRefBinding() const {
return !isCopyInit() || Context == IC_ExplicitConvs;
}



bool hasParenOrBraceRange() const {
return Kind == IK_Direct || Kind == IK_Value || Kind == IK_DirectList;
}




SourceRange getParenOrBraceRange() const {
assert(hasParenOrBraceRange() && "Only direct, value, and direct-list "
"initialization have parentheses or "
"braces");
return SourceRange(Locations[1], Locations[2]);
}
};



class InitializationSequence {
public:

enum SequenceKind {


FailedSequence = 0,




DependentSequence,


NormalSequence
};



enum StepKind {


SK_ResolveAddressOfOverloadedFunction,


SK_CastDerivedToBasePRValue,


SK_CastDerivedToBaseXValue,


SK_CastDerivedToBaseLValue,


SK_BindReference,


SK_BindReferenceToTemporary,




SK_ExtraneousCopyToTemporary,



SK_FinalCopy,



SK_UserConversion,


SK_QualificationConversionPRValue,


SK_QualificationConversionXValue,


SK_QualificationConversionLValue,


SK_FunctionReferenceConversion,


SK_AtomicConversion,


SK_ConversionSequence,


SK_ConversionSequenceNoNarrowing,


SK_ListInitialization,


SK_UnwrapInitList,


SK_RewrapInitList,


SK_ConstructorInitialization,



SK_ConstructorInitializationFromList,


SK_ZeroInitialization,


SK_CAssignment,


SK_StringInit,



SK_ObjCObjectConversion,


SK_ArrayLoopIndex,


SK_ArrayLoopInit,


SK_ArrayInit,


SK_GNUArrayInit,



SK_ParenthesizedArrayInit,


SK_PassByIndirectCopyRestore,


SK_PassByIndirectRestore,


SK_ProduceObjCObject,


SK_StdInitializerList,



SK_StdInitializerListConstructorCall,


SK_OCLSamplerInit,


SK_OCLZeroOpaqueType
};


class Step {
public:

StepKind Kind;


QualType Type;

struct F {
bool HadMultipleCandidates;
FunctionDecl *Function;
DeclAccessPair FoundDecl;
};

union {










struct F Function;



ImplicitConversionSequence *ICS;



InitListExpr *WrappingSyntacticList;
};

void Destroy();
};

private:

enum SequenceKind SequenceKind;


SmallVector<Step, 4> Steps;

public:

enum FailureKind {

FK_TooManyInitsForReference,


FK_ParenthesizedListInitForReference,


FK_ArrayNeedsInitList,



FK_ArrayNeedsInitListOrStringLiteral,



FK_ArrayNeedsInitListOrWideStringLiteral,


FK_NarrowStringIntoWideCharArray,


FK_WideStringIntoCharArray,



FK_IncompatWideStringIntoWideChar,


FK_PlainStringIntoUTF8Char,


FK_UTF8StringIntoPlainChar,


FK_ArrayTypeMismatch,


FK_NonConstantArrayInit,


FK_AddressOfOverloadFailed,


FK_ReferenceInitOverloadFailed,


FK_NonConstLValueReferenceBindingToTemporary,


FK_NonConstLValueReferenceBindingToBitfield,


FK_NonConstLValueReferenceBindingToVectorElement,


FK_NonConstLValueReferenceBindingToMatrixElement,



FK_NonConstLValueReferenceBindingToUnrelated,


FK_RValueReferenceBindingToLValue,


FK_ReferenceInitDropsQualifiers,


FK_ReferenceAddrspaceMismatchTemporary,


FK_ReferenceInitFailed,


FK_ConversionFailed,


FK_ConversionFromPropertyFailed,


FK_TooManyInitsForScalar,


FK_ParenthesizedListInitForScalar,


FK_ReferenceBindingToInitList,



FK_InitListBadDestinationType,


FK_UserConversionOverloadFailed,


FK_ConstructorOverloadFailed,


FK_ListConstructorOverloadFailed,


FK_DefaultInitOfConst,


FK_Incomplete,


FK_VariableLengthArrayHasInitializer,


FK_ListInitializationFailed,



FK_PlaceholderType,



FK_AddressOfUnaddressableFunction,


FK_ExplicitConstructor,
};

private:

FailureKind Failure;


OverloadingResult FailedOverloadResult;


OverloadCandidateSet FailedCandidateSet;


QualType FailedIncompleteType;



std::string ZeroInitializationFixit;
SourceLocation ZeroInitializationFixitLoc;

public:


void SetZeroInitializationFixit(const std::string& Fixit, SourceLocation L) {
ZeroInitializationFixit = Fixit;
ZeroInitializationFixitLoc = L;
}

private:


void PrintInitLocationNote(Sema &S, const InitializedEntity &Entity);

public:



















InitializationSequence(Sema &S,
const InitializedEntity &Entity,
const InitializationKind &Kind,
MultiExprArg Args,
bool TopLevelOfInitList = false,
bool TreatUnavailableAsInvalid = true);
void InitializeFrom(Sema &S, const InitializedEntity &Entity,
const InitializationKind &Kind, MultiExprArg Args,
bool TopLevelOfInitList, bool TreatUnavailableAsInvalid);

~InitializationSequence();






















ExprResult Perform(Sema &S,
const InitializedEntity &Entity,
const InitializationKind &Kind,
MultiExprArg Args,
QualType *ResultType = nullptr);





bool Diagnose(Sema &S,
const InitializedEntity &Entity,
const InitializationKind &Kind,
ArrayRef<Expr *> Args);


enum SequenceKind getKind() const { return SequenceKind; }


void setSequenceKind(enum SequenceKind SK) { SequenceKind = SK; }


explicit operator bool() const { return !Failed(); }


bool Failed() const { return SequenceKind == FailedSequence; }

using step_iterator = SmallVectorImpl<Step>::const_iterator;

step_iterator step_begin() const { return Steps.begin(); }
step_iterator step_end() const { return Steps.end(); }

using step_range = llvm::iterator_range<step_iterator>;

step_range steps() const { return {step_begin(), step_end()}; }



bool isDirectReferenceBinding() const;


bool isAmbiguous() const;



bool isConstructorInitialization() const;






void AddAddressOverloadResolutionStep(FunctionDecl *Function,
DeclAccessPair Found,
bool HadMultipleCandidates);








void AddDerivedToBaseCastStep(QualType BaseType,
ExprValueKind Category);






void AddReferenceBindingStep(QualType T, bool BindingTemporary);











void AddExtraneousCopyToTemporary(QualType T);



void AddFinalCopy(QualType T);



void AddUserConversionStep(FunctionDecl *Function,
DeclAccessPair FoundDecl,
QualType T,
bool HadMultipleCandidates);



void AddQualificationConversionStep(QualType Ty,
ExprValueKind Category);



void AddFunctionReferenceConversionStep(QualType Ty);



void AddAtomicConversionStep(QualType Ty);


void AddConversionSequenceStep(const ImplicitConversionSequence &ICS,
QualType T, bool TopLevelOfInitList = false);


void AddListInitializationStep(QualType T);






void AddConstructorInitializationStep(DeclAccessPair FoundDecl,
CXXConstructorDecl *Constructor,
QualType T,
bool HadMultipleCandidates,
bool FromInitList, bool AsInitList);


void AddZeroInitializationStep(QualType T);






void AddCAssignmentStep(QualType T);


void AddStringInitStep(QualType T);



void AddObjCObjectConversionStep(QualType T);


void AddArrayInitLoopStep(QualType T, QualType EltTy);


void AddArrayInitStep(QualType T, bool IsGNUExtension);


void AddParenthesizedArrayInitStep(QualType T);


void AddPassByIndirectCopyRestoreStep(QualType T, bool shouldCopy);



void AddProduceObjCObjectStep(QualType T);



void AddStdInitializerListConstructionStep(QualType T);



void AddOCLSamplerInitStep(QualType T);



void AddOCLZeroOpaqueTypeStep(QualType T);



void RewrapReferenceInitList(QualType T, InitListExpr *Syntactic);


void SetFailed(FailureKind Failure) {
SequenceKind = FailedSequence;
this->Failure = Failure;
assert((Failure != FK_Incomplete || !FailedIncompleteType.isNull()) &&
"Incomplete type failure requires a type!");
}



void SetOverloadFailure(FailureKind Failure, OverloadingResult Result);



OverloadCandidateSet &getFailedCandidateSet() {
return FailedCandidateSet;
}



OverloadingResult getFailedOverloadResult() const {
return FailedOverloadResult;
}



void setIncompleteTypeFailure(QualType IncompleteType) {
FailedIncompleteType = IncompleteType;
SetFailed(FK_Incomplete);
}


FailureKind getFailureKind() const {
assert(Failed() && "Not an initialization failure!");
return Failure;
}



void dump(raw_ostream &OS) const;



void dump() const;
};

}

#endif
