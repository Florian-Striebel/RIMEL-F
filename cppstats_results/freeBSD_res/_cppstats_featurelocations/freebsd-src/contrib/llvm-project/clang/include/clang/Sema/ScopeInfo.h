












#if !defined(LLVM_CLANG_SEMA_SCOPEINFO_H)
#define LLVM_CLANG_SEMA_SCOPEINFO_H

#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/Type.h"
#include "clang/Basic/CapturedStmt.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/CleanupInfo.h"
#include "clang/Sema/DeclSpec.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include <algorithm>
#include <cassert>
#include <utility>

namespace clang {

class BlockDecl;
class CapturedDecl;
class CXXMethodDecl;
class CXXRecordDecl;
class ImplicitParamDecl;
class NamedDecl;
class ObjCIvarRefExpr;
class ObjCMessageExpr;
class ObjCPropertyDecl;
class ObjCPropertyRefExpr;
class ParmVarDecl;
class RecordDecl;
class ReturnStmt;
class Scope;
class Stmt;
class SwitchStmt;
class TemplateParameterList;
class TemplateTypeParmDecl;
class VarDecl;

namespace sema {



class CompoundScopeInfo {
public:


bool HasEmptyLoopBodies = false;



bool IsStmtExpr;

CompoundScopeInfo(bool IsStmtExpr) : IsStmtExpr(IsStmtExpr) {}

void setHasEmptyLoopBodies() {
HasEmptyLoopBodies = true;
}
};

class PossiblyUnreachableDiag {
public:
PartialDiagnostic PD;
SourceLocation Loc;
llvm::TinyPtrVector<const Stmt*> Stmts;

PossiblyUnreachableDiag(const PartialDiagnostic &PD, SourceLocation Loc,
ArrayRef<const Stmt *> Stmts)
: PD(PD), Loc(Loc), Stmts(Stmts) {}
};



class FunctionScopeInfo {
protected:
enum ScopeKind {
SK_Function,
SK_Block,
SK_Lambda,
SK_CapturedRegion
};

public:

ScopeKind Kind : 3;



bool HasBranchProtectedScope : 1;


bool HasBranchIntoScope : 1;


bool HasIndirectGoto : 1;



bool HasMustTail : 1;


bool HasDroppedStmt : 1;


bool HasOMPDeclareReductionCombiner : 1;


bool HasFallthroughStmt : 1;


bool UsesFPIntrin : 1;



bool HasPotentialAvailabilityViolations : 1;




bool ObjCShouldCallSuper : 1;


bool ObjCIsDesignatedInit : 1;




bool ObjCWarnForNoDesignatedInitChain : 1;




bool ObjCIsSecondaryInit : 1;



bool ObjCWarnForNoInitDelegation : 1;



bool NeedsCoroutineSuspends : 1;



unsigned char FirstCoroutineStmtKind : 2;



SourceLocation FirstCoroutineStmtLoc;


SourceLocation FirstReturnLoc;


SourceLocation FirstCXXTryLoc;


SourceLocation FirstSEHTryLoc;

private:

DiagnosticErrorTrap ErrorTrap;

public:


using SwitchInfo = llvm::PointerIntPair<SwitchStmt*, 1, bool>;



SmallVector<SwitchInfo, 8> SwitchStack;




SmallVector<ReturnStmt*, 4> Returns;


VarDecl *CoroutinePromise = nullptr;



llvm::SmallMapVector<ParmVarDecl *, Stmt *, 4> CoroutineParameterMoves;


std::pair<Stmt *, Stmt *> CoroutineSuspends;



SmallVector<CompoundScopeInfo, 4> CompoundScopes;


llvm::SmallPtrSet<const BlockDecl *, 1> Blocks;


llvm::TinyPtrVector<VarDecl *> ByrefBlockVars;




SmallVector<PossiblyUnreachableDiag, 4> PossiblyUnreachableDiags;



llvm::SmallPtrSet<const ParmVarDecl *, 8> ModifiedNonNullParams;

public:






















class WeakObjectProfileTy {






using BaseInfoTy = llvm::PointerIntPair<const NamedDecl *, 1, bool>;
BaseInfoTy Base;





const NamedDecl *Property = nullptr;


static BaseInfoTy getBaseInfo(const Expr *BaseE);

inline WeakObjectProfileTy();
static inline WeakObjectProfileTy getSentinel();

public:
WeakObjectProfileTy(const ObjCPropertyRefExpr *RE);
WeakObjectProfileTy(const Expr *Base, const ObjCPropertyDecl *Property);
WeakObjectProfileTy(const DeclRefExpr *RE);
WeakObjectProfileTy(const ObjCIvarRefExpr *RE);

const NamedDecl *getBase() const { return Base.getPointer(); }
const NamedDecl *getProperty() const { return Property; }












bool isExactProfile() const {
return Base.getInt();
}

bool operator==(const WeakObjectProfileTy &Other) const {
return Base == Other.Base && Property == Other.Property;
}





class DenseMapInfo {
public:
static inline WeakObjectProfileTy getEmptyKey() {
return WeakObjectProfileTy();
}

static inline WeakObjectProfileTy getTombstoneKey() {
return WeakObjectProfileTy::getSentinel();
}

static unsigned getHashValue(const WeakObjectProfileTy &Val) {
using Pair = std::pair<BaseInfoTy, const NamedDecl *>;

return llvm::DenseMapInfo<Pair>::getHashValue(Pair(Val.Base,
Val.Property));
}

static bool isEqual(const WeakObjectProfileTy &LHS,
const WeakObjectProfileTy &RHS) {
return LHS == RHS;
}
};
};







class WeakUseTy {
llvm::PointerIntPair<const Expr *, 1, bool> Rep;

public:
WeakUseTy(const Expr *Use, bool IsRead) : Rep(Use, IsRead) {}

const Expr *getUseExpr() const { return Rep.getPointer(); }
bool isUnsafe() const { return Rep.getInt(); }
void markSafe() { Rep.setInt(false); }

bool operator==(const WeakUseTy &Other) const {
return Rep == Other.Rep;
}
};




using WeakUseVector = SmallVector<WeakUseTy, 4>;




using WeakObjectUseMap =
llvm::SmallDenseMap<WeakObjectProfileTy, WeakUseVector, 8,
WeakObjectProfileTy::DenseMapInfo>;

private:



WeakObjectUseMap WeakObjectUses;

protected:
FunctionScopeInfo(const FunctionScopeInfo&) = default;

public:
FunctionScopeInfo(DiagnosticsEngine &Diag)
: Kind(SK_Function), HasBranchProtectedScope(false),
HasBranchIntoScope(false), HasIndirectGoto(false), HasMustTail(false),
HasDroppedStmt(false), HasOMPDeclareReductionCombiner(false),
HasFallthroughStmt(false), UsesFPIntrin(false),
HasPotentialAvailabilityViolations(false), ObjCShouldCallSuper(false),
ObjCIsDesignatedInit(false), ObjCWarnForNoDesignatedInitChain(false),
ObjCIsSecondaryInit(false), ObjCWarnForNoInitDelegation(false),
NeedsCoroutineSuspends(true), ErrorTrap(Diag) {}

virtual ~FunctionScopeInfo();








bool hasUnrecoverableErrorOccurred() const {
return ErrorTrap.hasUnrecoverableErrorOccurred();
}




template <typename ExprT>
inline void recordUseOfWeak(const ExprT *E, bool IsRead = true);

void recordUseOfWeak(const ObjCMessageExpr *Msg,
const ObjCPropertyDecl *Prop);





void markSafeWeakUse(const Expr *E);

const WeakObjectUseMap &getWeakObjectUses() const {
return WeakObjectUses;
}

void setHasBranchIntoScope() {
HasBranchIntoScope = true;
}

void setHasBranchProtectedScope() {
HasBranchProtectedScope = true;
}

void setHasIndirectGoto() {
HasIndirectGoto = true;
}

void setHasMustTail() { HasMustTail = true; }

void setHasDroppedStmt() {
HasDroppedStmt = true;
}

void setHasOMPDeclareReductionCombiner() {
HasOMPDeclareReductionCombiner = true;
}

void setHasFallthroughStmt() {
HasFallthroughStmt = true;
}

void setUsesFPIntrin() {
UsesFPIntrin = true;
}

void setHasCXXTry(SourceLocation TryLoc) {
setHasBranchProtectedScope();
FirstCXXTryLoc = TryLoc;
}

void setHasSEHTry(SourceLocation TryLoc) {
setHasBranchProtectedScope();
FirstSEHTryLoc = TryLoc;
}

bool NeedsScopeChecking() const {
return !HasDroppedStmt && (HasIndirectGoto || HasMustTail ||
(HasBranchProtectedScope && HasBranchIntoScope));
}


void addBlock(const BlockDecl *BD) {
Blocks.insert(BD);
}


void addByrefBlockVar(VarDecl *VD) {
ByrefBlockVars.push_back(VD);
}

bool isCoroutine() const { return !FirstCoroutineStmtLoc.isInvalid(); }

void setFirstCoroutineStmt(SourceLocation Loc, StringRef Keyword) {
assert(FirstCoroutineStmtLoc.isInvalid() &&
"first coroutine statement location already set");
FirstCoroutineStmtLoc = Loc;
FirstCoroutineStmtKind = llvm::StringSwitch<unsigned char>(Keyword)
.Case("co_return", 0)
.Case("co_await", 1)
.Case("co_yield", 2);
}

StringRef getFirstCoroutineStmtKeyword() const {
assert(FirstCoroutineStmtLoc.isValid()
&& "no coroutine statement available");
switch (FirstCoroutineStmtKind) {
case 0: return "co_return";
case 1: return "co_await";
case 2: return "co_yield";
default:
llvm_unreachable("FirstCoroutineStmtKind has an invalid value");
};
}

void setNeedsCoroutineSuspends(bool value = true) {
assert((!value || CoroutineSuspends.first == nullptr) &&
"we already have valid suspend points");
NeedsCoroutineSuspends = value;
}

bool hasInvalidCoroutineSuspends() const {
return !NeedsCoroutineSuspends && CoroutineSuspends.first == nullptr;
}

void setCoroutineSuspends(Stmt *Initial, Stmt *Final) {
assert(Initial && Final && "suspend points cannot be null");
assert(CoroutineSuspends.first == nullptr && "suspend points already set");
NeedsCoroutineSuspends = false;
CoroutineSuspends.first = Initial;
CoroutineSuspends.second = Final;
}



void Clear();

bool isPlainFunction() const { return Kind == SK_Function; }
};

class Capture {













enum CaptureKind {
Cap_ByCopy, Cap_ByRef, Cap_Block, Cap_VLA
};

union {

const VariableArrayType *CapturedVLA;


VarDecl *CapturedVar;
};


SourceLocation Loc;


SourceLocation EllipsisLoc;



QualType CaptureType;


unsigned Kind : 2;



unsigned Nested : 1;


unsigned CapturesThis : 1;



unsigned ODRUsed : 1;



unsigned NonODRUsed : 1;



unsigned Invalid : 1;

public:
Capture(VarDecl *Var, bool Block, bool ByRef, bool IsNested,
SourceLocation Loc, SourceLocation EllipsisLoc, QualType CaptureType,
bool Invalid)
: CapturedVar(Var), Loc(Loc), EllipsisLoc(EllipsisLoc),
CaptureType(CaptureType),
Kind(Block ? Cap_Block : ByRef ? Cap_ByRef : Cap_ByCopy),
Nested(IsNested), CapturesThis(false), ODRUsed(false),
NonODRUsed(false), Invalid(Invalid) {}

enum IsThisCapture { ThisCapture };
Capture(IsThisCapture, bool IsNested, SourceLocation Loc,
QualType CaptureType, const bool ByCopy, bool Invalid)
: Loc(Loc), CaptureType(CaptureType),
Kind(ByCopy ? Cap_ByCopy : Cap_ByRef), Nested(IsNested),
CapturesThis(true), ODRUsed(false), NonODRUsed(false),
Invalid(Invalid) {}

enum IsVLACapture { VLACapture };
Capture(IsVLACapture, const VariableArrayType *VLA, bool IsNested,
SourceLocation Loc, QualType CaptureType)
: CapturedVLA(VLA), Loc(Loc), CaptureType(CaptureType), Kind(Cap_VLA),
Nested(IsNested), CapturesThis(false), ODRUsed(false),
NonODRUsed(false), Invalid(false) {}

bool isThisCapture() const { return CapturesThis; }
bool isVariableCapture() const {
return !isThisCapture() && !isVLATypeCapture();
}

bool isCopyCapture() const { return Kind == Cap_ByCopy; }
bool isReferenceCapture() const { return Kind == Cap_ByRef; }
bool isBlockCapture() const { return Kind == Cap_Block; }
bool isVLATypeCapture() const { return Kind == Cap_VLA; }

bool isNested() const { return Nested; }

bool isInvalid() const { return Invalid; }


bool isInitCapture() const;

bool isODRUsed() const { return ODRUsed; }
bool isNonODRUsed() const { return NonODRUsed; }
void markUsed(bool IsODRUse) {
if (IsODRUse)
ODRUsed = true;
else
NonODRUsed = true;
}

VarDecl *getVariable() const {
assert(isVariableCapture());
return CapturedVar;
}

const VariableArrayType *getCapturedVLAType() const {
assert(isVLATypeCapture());
return CapturedVLA;
}


SourceLocation getLocation() const { return Loc; }



SourceLocation getEllipsisLoc() const { return EllipsisLoc; }




QualType getCaptureType() const { return CaptureType; }
};

class CapturingScopeInfo : public FunctionScopeInfo {
protected:
CapturingScopeInfo(const CapturingScopeInfo&) = default;

public:
enum ImplicitCaptureStyle {
ImpCap_None, ImpCap_LambdaByval, ImpCap_LambdaByref, ImpCap_Block,
ImpCap_CapturedRegion
};

ImplicitCaptureStyle ImpCaptureStyle;

CapturingScopeInfo(DiagnosticsEngine &Diag, ImplicitCaptureStyle Style)
: FunctionScopeInfo(Diag), ImpCaptureStyle(Style) {}


llvm::DenseMap<VarDecl*, unsigned> CaptureMap;



unsigned CXXThisCaptureIndex = 0;


SmallVector<Capture, 4> Captures;



bool HasImplicitReturnType = false;



QualType ReturnType;

void addCapture(VarDecl *Var, bool isBlock, bool isByref, bool isNested,
SourceLocation Loc, SourceLocation EllipsisLoc,
QualType CaptureType, bool Invalid) {
Captures.push_back(Capture(Var, isBlock, isByref, isNested, Loc,
EllipsisLoc, CaptureType, Invalid));
CaptureMap[Var] = Captures.size();
}

void addVLATypeCapture(SourceLocation Loc, const VariableArrayType *VLAType,
QualType CaptureType) {
Captures.push_back(Capture(Capture::VLACapture, VLAType,
false, Loc, CaptureType));
}

void addThisCapture(bool isNested, SourceLocation Loc, QualType CaptureType,
bool ByCopy);


bool isCXXThisCaptured() const { return CXXThisCaptureIndex != 0; }


Capture &getCXXThisCapture() {
assert(isCXXThisCaptured() && "this has not been captured");
return Captures[CXXThisCaptureIndex - 1];
}


bool isCaptured(VarDecl *Var) const {
return CaptureMap.count(Var);
}


bool isVLATypeCaptured(const VariableArrayType *VAT) const;



Capture &getCapture(VarDecl *Var) {
assert(isCaptured(Var) && "Variable has not been captured");
return Captures[CaptureMap[Var] - 1];
}

const Capture &getCapture(VarDecl *Var) const {
llvm::DenseMap<VarDecl*, unsigned>::const_iterator Known
= CaptureMap.find(Var);
assert(Known != CaptureMap.end() && "Variable has not been captured");
return Captures[Known->second - 1];
}

static bool classof(const FunctionScopeInfo *FSI) {
return FSI->Kind == SK_Block || FSI->Kind == SK_Lambda
|| FSI->Kind == SK_CapturedRegion;
}
};


class BlockScopeInfo final : public CapturingScopeInfo {
public:
BlockDecl *TheDecl;



Scope *TheScope;



QualType FunctionType;

BlockScopeInfo(DiagnosticsEngine &Diag, Scope *BlockScope, BlockDecl *Block)
: CapturingScopeInfo(Diag, ImpCap_Block), TheDecl(Block),
TheScope(BlockScope) {
Kind = SK_Block;
}

~BlockScopeInfo() override;

static bool classof(const FunctionScopeInfo *FSI) {
return FSI->Kind == SK_Block;
}
};


class CapturedRegionScopeInfo final : public CapturingScopeInfo {
public:

CapturedDecl *TheCapturedDecl;


RecordDecl *TheRecordDecl;


Scope *TheScope;


ImplicitParamDecl *ContextParam;


unsigned short CapRegionKind;

unsigned short OpenMPLevel;
unsigned short OpenMPCaptureLevel;

CapturedRegionScopeInfo(DiagnosticsEngine &Diag, Scope *S, CapturedDecl *CD,
RecordDecl *RD, ImplicitParamDecl *Context,
CapturedRegionKind K, unsigned OpenMPLevel,
unsigned OpenMPCaptureLevel)
: CapturingScopeInfo(Diag, ImpCap_CapturedRegion),
TheCapturedDecl(CD), TheRecordDecl(RD), TheScope(S),
ContextParam(Context), CapRegionKind(K), OpenMPLevel(OpenMPLevel),
OpenMPCaptureLevel(OpenMPCaptureLevel) {
Kind = SK_CapturedRegion;
}

~CapturedRegionScopeInfo() override;


StringRef getRegionName() const {
switch (CapRegionKind) {
case CR_Default:
return "default captured statement";
case CR_ObjCAtFinally:
return "Objective-C @finally statement";
case CR_OpenMP:
return "OpenMP region";
}
llvm_unreachable("Invalid captured region kind!");
}

static bool classof(const FunctionScopeInfo *FSI) {
return FSI->Kind == SK_CapturedRegion;
}
};

class LambdaScopeInfo final :
public CapturingScopeInfo, public InventedTemplateParameterInfo {
public:

CXXRecordDecl *Lambda = nullptr;


CXXMethodDecl *CallOperator = nullptr;


SourceRange IntroducerRange;



SourceLocation CaptureDefaultLoc;



unsigned NumExplicitCaptures = 0;


bool Mutable = false;


bool ExplicitParams = false;


CleanupInfo Cleanup;


bool ContainsUnexpandedParameterPack = false;


SmallVector<NamedDecl*, 4> LocalPacks;


SourceRange ExplicitTemplateParamsRange;




ExprResult RequiresClause;




TemplateParameterList *GLTemplateParameterList = nullptr;














llvm::SmallVector<Expr*, 4> PotentiallyCapturingExprs;





llvm::SmallSet<Expr *, 8> NonODRUsedCapturingExprs;


llvm::DenseMap<unsigned, SourceRange> ExplicitCaptureRanges;




struct ShadowedOuterDecl {
const VarDecl *VD;
const VarDecl *ShadowedDecl;
};
llvm::SmallVector<ShadowedOuterDecl, 4> ShadowingDecls;

SourceLocation PotentialThisCaptureLocation;

LambdaScopeInfo(DiagnosticsEngine &Diag)
: CapturingScopeInfo(Diag, ImpCap_None) {
Kind = SK_Lambda;
}


void finishedExplicitCaptures() {
NumExplicitCaptures = Captures.size();
}

static bool classof(const FunctionScopeInfo *FSI) {
return FSI->Kind == SK_Lambda;
}



bool isGenericLambda() const {
return !TemplateParams.empty() || GLTemplateParameterList;
}


















void addPotentialCapture(Expr *VarExpr) {
assert(isa<DeclRefExpr>(VarExpr) || isa<MemberExpr>(VarExpr) ||
isa<FunctionParmPackExpr>(VarExpr));
PotentiallyCapturingExprs.push_back(VarExpr);
}

void addPotentialThisCapture(SourceLocation Loc) {
PotentialThisCaptureLocation = Loc;
}

bool hasPotentialThisCapture() const {
return PotentialThisCaptureLocation.isValid();
}








































void markVariableExprAsNonODRUsed(Expr *CapturingVarExpr) {
assert(isa<DeclRefExpr>(CapturingVarExpr) ||
isa<MemberExpr>(CapturingVarExpr) ||
isa<FunctionParmPackExpr>(CapturingVarExpr));
NonODRUsedCapturingExprs.insert(CapturingVarExpr);
}
bool isVariableExprMarkedAsNonODRUsed(Expr *CapturingVarExpr) const {
assert(isa<DeclRefExpr>(CapturingVarExpr) ||
isa<MemberExpr>(CapturingVarExpr) ||
isa<FunctionParmPackExpr>(CapturingVarExpr));
return NonODRUsedCapturingExprs.count(CapturingVarExpr);
}
void removePotentialCapture(Expr *E) {
PotentiallyCapturingExprs.erase(
std::remove(PotentiallyCapturingExprs.begin(),
PotentiallyCapturingExprs.end(), E),
PotentiallyCapturingExprs.end());
}
void clearPotentialCaptures() {
PotentiallyCapturingExprs.clear();
PotentialThisCaptureLocation = SourceLocation();
}
unsigned getNumPotentialVariableCaptures() const {
return PotentiallyCapturingExprs.size();
}

bool hasPotentialCaptures() const {
return getNumPotentialVariableCaptures() ||
PotentialThisCaptureLocation.isValid();
}

void visitPotentialCaptures(
llvm::function_ref<void(VarDecl *, Expr *)> Callback) const;
};

FunctionScopeInfo::WeakObjectProfileTy::WeakObjectProfileTy()
: Base(nullptr, false) {}

FunctionScopeInfo::WeakObjectProfileTy
FunctionScopeInfo::WeakObjectProfileTy::getSentinel() {
FunctionScopeInfo::WeakObjectProfileTy Result;
Result.Base.setInt(true);
return Result;
}

template <typename ExprT>
void FunctionScopeInfo::recordUseOfWeak(const ExprT *E, bool IsRead) {
assert(E);
WeakUseVector &Uses = WeakObjectUses[WeakObjectProfileTy(E)];
Uses.push_back(WeakUseTy(E, IsRead));
}

inline void CapturingScopeInfo::addThisCapture(bool isNested,
SourceLocation Loc,
QualType CaptureType,
bool ByCopy) {
Captures.push_back(Capture(Capture::ThisCapture, isNested, Loc, CaptureType,
ByCopy, false));
CXXThisCaptureIndex = Captures.size();
}

}

}

#endif
