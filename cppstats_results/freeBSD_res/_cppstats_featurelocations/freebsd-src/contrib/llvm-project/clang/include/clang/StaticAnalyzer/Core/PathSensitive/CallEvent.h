













#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CALLEVENT_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CALLEVENT_H

#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <limits>
#include <utility>

namespace clang {

class LocationContext;
class ProgramPoint;
class ProgramPointTag;
class StackFrameContext;

namespace ento {

enum CallEventKind {
CE_Function,
CE_CXXMember,
CE_CXXMemberOperator,
CE_CXXDestructor,
CE_BEG_CXX_INSTANCE_CALLS = CE_CXXMember,
CE_END_CXX_INSTANCE_CALLS = CE_CXXDestructor,
CE_CXXConstructor,
CE_CXXInheritedConstructor,
CE_BEG_CXX_CONSTRUCTOR_CALLS = CE_CXXConstructor,
CE_END_CXX_CONSTRUCTOR_CALLS = CE_CXXInheritedConstructor,
CE_CXXAllocator,
CE_CXXDeallocator,
CE_BEG_FUNCTION_CALLS = CE_Function,
CE_END_FUNCTION_CALLS = CE_CXXDeallocator,
CE_Block,
CE_ObjCMessage
};

class CallEvent;
class CallDescription;

template<typename T = CallEvent>
class CallEventRef : public IntrusiveRefCntPtr<const T> {
public:
CallEventRef(const T *Call) : IntrusiveRefCntPtr<const T>(Call) {}
CallEventRef(const CallEventRef &Orig) : IntrusiveRefCntPtr<const T>(Orig) {}

CallEventRef<T> cloneWithState(ProgramStateRef State) const {
return this->get()->template cloneWithState<T>(State);
}



template <typename SuperT>
operator CallEventRef<SuperT> () const {
return this->get();
}
};








class RuntimeDefinition {


const Decl *D = nullptr;





const MemRegion *R = nullptr;

public:
RuntimeDefinition() = default;
RuntimeDefinition(const Decl *InD): D(InD) {}
RuntimeDefinition(const Decl *InD, const MemRegion *InR): D(InD), R(InR) {}

const Decl *getDecl() { return D; }




bool mayHaveOtherDefinitions() { return R != nullptr; }



const MemRegion *getDispatchRegion() { return R; }
};










class CallEvent {
public:
using Kind = CallEventKind;

private:
ProgramStateRef State;
const LocationContext *LCtx;
llvm::PointerUnion<const Expr *, const Decl *> Origin;

protected:

const void *Data;




SourceLocation Location;

private:
template <typename T> friend struct llvm::IntrusiveRefCntPtrInfo;

mutable unsigned RefCount = 0;

void Retain() const { ++RefCount; }
void Release() const;

protected:
friend class CallEventManager;

CallEvent(const Expr *E, ProgramStateRef state, const LocationContext *lctx)
: State(std::move(state)), LCtx(lctx), Origin(E) {}

CallEvent(const Decl *D, ProgramStateRef state, const LocationContext *lctx)
: State(std::move(state)), LCtx(lctx), Origin(D) {}


CallEvent(const CallEvent &Original)
: State(Original.State), LCtx(Original.LCtx), Origin(Original.Origin),
Data(Original.Data), Location(Original.Location) {}


virtual void cloneTo(void *Dest) const = 0;


SVal getSVal(const Stmt *S) const {
return getState()->getSVal(S, getLocationContext());
}

using ValueList = SmallVectorImpl<SVal>;



virtual void getExtraInvalidatedValues(ValueList &Values,
RegionAndSymbolInvalidationTraits *ETraits) const {}

public:
CallEvent &operator=(const CallEvent &) = delete;
virtual ~CallEvent() = default;


virtual Kind getKind() const = 0;
virtual StringRef getKindAsString() const = 0;



virtual const Decl *getDecl() const {
return Origin.dyn_cast<const Decl *>();
}


const ProgramStateRef &getState() const {
return State;
}


const LocationContext *getLocationContext() const {
return LCtx;
}



virtual RuntimeDefinition getRuntimeDefinition() const = 0;



virtual const Expr *getOriginExpr() const {
return Origin.dyn_cast<const Expr *>();
}






virtual unsigned getNumArgs() const = 0;


bool isInSystemHeader() const {
const Decl *D = getDecl();
if (!D)
return false;

SourceLocation Loc = D->getLocation();
if (Loc.isValid()) {
const SourceManager &SM =
getState()->getStateManager().getContext().getSourceManager();
return SM.isInSystemHeader(D->getLocation());
}



if (const auto *FD = dyn_cast<FunctionDecl>(D))
return FD->isOverloadedOperator() && FD->isImplicit() && FD->isGlobal();

return false;
}






bool isCalled(const CallDescription &CD) const;



template <typename FirstCallDesc, typename... CallDescs>
bool isCalled(const FirstCallDesc &First, const CallDescs &... Rest) const {
return isCalled(First) || isCalled(Rest...);
}



virtual SourceRange getSourceRange() const {
return getOriginExpr()->getSourceRange();
}


virtual SVal getArgSVal(unsigned Index) const;



virtual const Expr *getArgExpr(unsigned Index) const { return nullptr; }




virtual SourceRange getArgSourceRange(unsigned Index) const;


QualType getResultType() const;





SVal getReturnValue() const;



bool hasNonNullArgumentsWithType(bool (*Condition)(QualType)) const;


bool hasNonZeroCallbackArg() const;


bool hasVoidPointerToNonConstArg() const;






virtual bool argumentsMayEscape() const {
return hasNonZeroCallbackArg();
}

















bool isGlobalCFunction(StringRef SpecificName = StringRef()) const;








const IdentifierInfo *getCalleeIdentifier() const {
const auto *ND = dyn_cast_or_null<NamedDecl>(getDecl());
if (!ND)
return nullptr;
return ND->getIdentifier();
}


ProgramPoint getProgramPoint(bool IsPreVisit = false,
const ProgramPointTag *Tag = nullptr) const;





ProgramStateRef invalidateRegions(unsigned BlockCount,
ProgramStateRef Orig = nullptr) const;

using FrameBindingTy = std::pair<SVal, SVal>;
using BindingsTy = SmallVectorImpl<FrameBindingTy>;



virtual void getInitialStackFrameContents(const StackFrameContext *CalleeCtx,
BindingsTy &Bindings) const = 0;


template <typename T>
CallEventRef<T> cloneWithState(ProgramStateRef NewState) const;


CallEventRef<> cloneWithState(ProgramStateRef NewState) const {
return cloneWithState<CallEvent>(NewState);
}



static bool isCallStmt(const Stmt *S);




static QualType getDeclaredResultType(const Decl *D);




static bool isVariadic(const Decl *D);



AnalysisDeclContext *getCalleeAnalysisDeclContext() const;





const StackFrameContext *getCalleeStackFrame(unsigned BlockCount) const;




const ParamVarRegion *getParameterLocation(unsigned Index,
unsigned BlockCount) const;







bool isArgumentConstructedDirectly(unsigned Index) const {

return ExprEngine::getObjectUnderConstruction(
getState(), {getOriginExpr(), Index}, getLocationContext()).hasValue();
}





virtual Optional<unsigned>
getAdjustedParameterIndex(unsigned ASTArgumentIndex) const {
return ASTArgumentIndex;
}




virtual unsigned getASTArgumentIndex(unsigned CallArgumentIndex) const {
return CallArgumentIndex;
}




const ConstructionContext *getConstructionContext() const;



Optional<SVal> getReturnValueUnderConstruction() const;


private:
struct GetTypeFn {
QualType operator()(ParmVarDecl *PD) const { return PD->getType(); }
};

public:





virtual ArrayRef<ParmVarDecl *> parameters() const = 0;

using param_type_iterator =
llvm::mapped_iterator<ArrayRef<ParmVarDecl *>::iterator, GetTypeFn>;






param_type_iterator param_type_begin() const {
return llvm::map_iterator(parameters().begin(), GetTypeFn());
}

param_type_iterator param_type_end() const {
return llvm::map_iterator(parameters().end(), GetTypeFn());
}


void dump(raw_ostream &Out) const;
void dump() const;
};



class AnyFunctionCall : public CallEvent {
protected:
AnyFunctionCall(const Expr *E, ProgramStateRef St,
const LocationContext *LCtx)
: CallEvent(E, St, LCtx) {}
AnyFunctionCall(const Decl *D, ProgramStateRef St,
const LocationContext *LCtx)
: CallEvent(D, St, LCtx) {}
AnyFunctionCall(const AnyFunctionCall &Other) = default;

public:


const FunctionDecl *getDecl() const override {
return cast<FunctionDecl>(CallEvent::getDecl());
}

RuntimeDefinition getRuntimeDefinition() const override;

bool argumentsMayEscape() const override;

void getInitialStackFrameContents(const StackFrameContext *CalleeCtx,
BindingsTy &Bindings) const override;

ArrayRef<ParmVarDecl *> parameters() const override;

static bool classof(const CallEvent *CA) {
return CA->getKind() >= CE_BEG_FUNCTION_CALLS &&
CA->getKind() <= CE_END_FUNCTION_CALLS;
}
};




class SimpleFunctionCall : public AnyFunctionCall {
friend class CallEventManager;

protected:
SimpleFunctionCall(const CallExpr *CE, ProgramStateRef St,
const LocationContext *LCtx)
: AnyFunctionCall(CE, St, LCtx) {}
SimpleFunctionCall(const SimpleFunctionCall &Other) = default;

void cloneTo(void *Dest) const override {
new (Dest) SimpleFunctionCall(*this);
}

public:
const CallExpr *getOriginExpr() const override {
return cast<CallExpr>(AnyFunctionCall::getOriginExpr());
}

const FunctionDecl *getDecl() const override;

unsigned getNumArgs() const override { return getOriginExpr()->getNumArgs(); }

const Expr *getArgExpr(unsigned Index) const override {
return getOriginExpr()->getArg(Index);
}

Kind getKind() const override { return CE_Function; }
StringRef getKindAsString() const override { return "SimpleFunctionCall"; }

static bool classof(const CallEvent *CA) {
return CA->getKind() == CE_Function;
}
};




class BlockCall : public CallEvent {
friend class CallEventManager;

protected:
BlockCall(const CallExpr *CE, ProgramStateRef St,
const LocationContext *LCtx)
: CallEvent(CE, St, LCtx) {}
BlockCall(const BlockCall &Other) = default;

void cloneTo(void *Dest) const override { new (Dest) BlockCall(*this); }

void getExtraInvalidatedValues(ValueList &Values,
RegionAndSymbolInvalidationTraits *ETraits) const override;

public:
const CallExpr *getOriginExpr() const override {
return cast<CallExpr>(CallEvent::getOriginExpr());
}

unsigned getNumArgs() const override { return getOriginExpr()->getNumArgs(); }

const Expr *getArgExpr(unsigned Index) const override {
return getOriginExpr()->getArg(Index);
}




const BlockDataRegion *getBlockRegion() const;

const BlockDecl *getDecl() const override {
const BlockDataRegion *BR = getBlockRegion();
if (!BR)
return nullptr;
return BR->getDecl();
}

bool isConversionFromLambda() const {
const BlockDecl *BD = getDecl();
if (!BD)
return false;

return BD->isConversionFromLambda();
}



const VarRegion *getRegionStoringCapturedLambda() const {
assert(isConversionFromLambda());
const BlockDataRegion *BR = getBlockRegion();
assert(BR && "Block converted from lambda must have a block region");

auto I = BR->referenced_vars_begin();
assert(I != BR->referenced_vars_end());

return I.getCapturedRegion();
}

RuntimeDefinition getRuntimeDefinition() const override {
if (!isConversionFromLambda())
return RuntimeDefinition(getDecl());




















const VarDecl *LambdaVD = getRegionStoringCapturedLambda()->getDecl();
const CXXRecordDecl *LambdaDecl = LambdaVD->getType()->getAsCXXRecordDecl();
CXXMethodDecl* LambdaCallOperator = LambdaDecl->getLambdaCallOperator();

return RuntimeDefinition(LambdaCallOperator);
}

bool argumentsMayEscape() const override {
return true;
}

void getInitialStackFrameContents(const StackFrameContext *CalleeCtx,
BindingsTy &Bindings) const override;

ArrayRef<ParmVarDecl *> parameters() const override;

Kind getKind() const override { return CE_Block; }
StringRef getKindAsString() const override { return "BlockCall"; }

static bool classof(const CallEvent *CA) { return CA->getKind() == CE_Block; }
};



class CXXInstanceCall : public AnyFunctionCall {
protected:
CXXInstanceCall(const CallExpr *CE, ProgramStateRef St,
const LocationContext *LCtx)
: AnyFunctionCall(CE, St, LCtx) {}
CXXInstanceCall(const FunctionDecl *D, ProgramStateRef St,
const LocationContext *LCtx)
: AnyFunctionCall(D, St, LCtx) {}
CXXInstanceCall(const CXXInstanceCall &Other) = default;

void getExtraInvalidatedValues(ValueList &Values,
RegionAndSymbolInvalidationTraits *ETraits) const override;

public:

virtual const Expr *getCXXThisExpr() const { return nullptr; }


virtual SVal getCXXThisVal() const;

const FunctionDecl *getDecl() const override;

RuntimeDefinition getRuntimeDefinition() const override;

void getInitialStackFrameContents(const StackFrameContext *CalleeCtx,
BindingsTy &Bindings) const override;

static bool classof(const CallEvent *CA) {
return CA->getKind() >= CE_BEG_CXX_INSTANCE_CALLS &&
CA->getKind() <= CE_END_CXX_INSTANCE_CALLS;
}
};




class CXXMemberCall : public CXXInstanceCall {
friend class CallEventManager;

protected:
CXXMemberCall(const CXXMemberCallExpr *CE, ProgramStateRef St,
const LocationContext *LCtx)
: CXXInstanceCall(CE, St, LCtx) {}
CXXMemberCall(const CXXMemberCall &Other) = default;

void cloneTo(void *Dest) const override { new (Dest) CXXMemberCall(*this); }

public:
const CXXMemberCallExpr *getOriginExpr() const override {
return cast<CXXMemberCallExpr>(CXXInstanceCall::getOriginExpr());
}

unsigned getNumArgs() const override {
if (const CallExpr *CE = getOriginExpr())
return CE->getNumArgs();
return 0;
}

const Expr *getArgExpr(unsigned Index) const override {
return getOriginExpr()->getArg(Index);
}

const Expr *getCXXThisExpr() const override;

RuntimeDefinition getRuntimeDefinition() const override;

Kind getKind() const override { return CE_CXXMember; }
StringRef getKindAsString() const override { return "CXXMemberCall"; }

static bool classof(const CallEvent *CA) {
return CA->getKind() == CE_CXXMember;
}
};





class CXXMemberOperatorCall : public CXXInstanceCall {
friend class CallEventManager;

protected:
CXXMemberOperatorCall(const CXXOperatorCallExpr *CE, ProgramStateRef St,
const LocationContext *LCtx)
: CXXInstanceCall(CE, St, LCtx) {}
CXXMemberOperatorCall(const CXXMemberOperatorCall &Other) = default;

void cloneTo(void *Dest) const override {
new (Dest) CXXMemberOperatorCall(*this);
}

public:
const CXXOperatorCallExpr *getOriginExpr() const override {
return cast<CXXOperatorCallExpr>(CXXInstanceCall::getOriginExpr());
}

unsigned getNumArgs() const override {
return getOriginExpr()->getNumArgs() - 1;
}

const Expr *getArgExpr(unsigned Index) const override {
return getOriginExpr()->getArg(Index + 1);
}

const Expr *getCXXThisExpr() const override;

Kind getKind() const override { return CE_CXXMemberOperator; }
StringRef getKindAsString() const override { return "CXXMemberOperatorCall"; }

static bool classof(const CallEvent *CA) {
return CA->getKind() == CE_CXXMemberOperator;
}

Optional<unsigned>
getAdjustedParameterIndex(unsigned ASTArgumentIndex) const override {


return (ASTArgumentIndex > 0) ? Optional<unsigned>(ASTArgumentIndex - 1)
: None;
}

unsigned getASTArgumentIndex(unsigned CallArgumentIndex) const override {


return CallArgumentIndex + 1;
}

OverloadedOperatorKind getOverloadedOperator() const {
return getOriginExpr()->getOperator();
}
};





class CXXDestructorCall : public CXXInstanceCall {
friend class CallEventManager;

protected:
using DtorDataTy = llvm::PointerIntPair<const MemRegion *, 1, bool>;








CXXDestructorCall(const CXXDestructorDecl *DD, const Stmt *Trigger,
const MemRegion *Target, bool IsBaseDestructor,
ProgramStateRef St, const LocationContext *LCtx)
: CXXInstanceCall(DD, St, LCtx) {
Data = DtorDataTy(Target, IsBaseDestructor).getOpaqueValue();
Location = Trigger->getEndLoc();
}

CXXDestructorCall(const CXXDestructorCall &Other) = default;

void cloneTo(void *Dest) const override {new (Dest) CXXDestructorCall(*this);}

public:
SourceRange getSourceRange() const override { return Location; }
unsigned getNumArgs() const override { return 0; }

RuntimeDefinition getRuntimeDefinition() const override;


SVal getCXXThisVal() const override;


bool isBaseDestructor() const {
return DtorDataTy::getFromOpaqueValue(Data).getInt();
}

Kind getKind() const override { return CE_CXXDestructor; }
StringRef getKindAsString() const override { return "CXXDestructorCall"; }

static bool classof(const CallEvent *CA) {
return CA->getKind() == CE_CXXDestructor;
}
};



class AnyCXXConstructorCall : public AnyFunctionCall {
protected:
AnyCXXConstructorCall(const Expr *E, const MemRegion *Target,
ProgramStateRef St, const LocationContext *LCtx)
: AnyFunctionCall(E, St, LCtx) {
assert(E && (isa<CXXConstructExpr>(E) || isa<CXXInheritedCtorInitExpr>(E)));

Data = Target;
}

void getExtraInvalidatedValues(ValueList &Values,
RegionAndSymbolInvalidationTraits *ETraits) const override;

void getInitialStackFrameContents(const StackFrameContext *CalleeCtx,
BindingsTy &Bindings) const override;

public:

SVal getCXXThisVal() const;

static bool classof(const CallEvent *Call) {
return Call->getKind() >= CE_BEG_CXX_CONSTRUCTOR_CALLS &&
Call->getKind() <= CE_END_CXX_CONSTRUCTOR_CALLS;
}
};




class CXXConstructorCall : public AnyCXXConstructorCall {
friend class CallEventManager;

protected:







CXXConstructorCall(const CXXConstructExpr *CE, const MemRegion *Target,
ProgramStateRef St, const LocationContext *LCtx)
: AnyCXXConstructorCall(CE, Target, St, LCtx) {}

CXXConstructorCall(const CXXConstructorCall &Other) = default;

void cloneTo(void *Dest) const override { new (Dest) CXXConstructorCall(*this); }

public:
const CXXConstructExpr *getOriginExpr() const override {
return cast<CXXConstructExpr>(AnyFunctionCall::getOriginExpr());
}

const CXXConstructorDecl *getDecl() const override {
return getOriginExpr()->getConstructor();
}

unsigned getNumArgs() const override { return getOriginExpr()->getNumArgs(); }

const Expr *getArgExpr(unsigned Index) const override {
return getOriginExpr()->getArg(Index);
}

Kind getKind() const override { return CE_CXXConstructor; }
StringRef getKindAsString() const override { return "CXXConstructorCall"; }

static bool classof(const CallEvent *CA) {
return CA->getKind() == CE_CXXConstructor;
}
};





















class CXXInheritedConstructorCall : public AnyCXXConstructorCall {
friend class CallEventManager;

protected:
CXXInheritedConstructorCall(const CXXInheritedCtorInitExpr *CE,
const MemRegion *Target, ProgramStateRef St,
const LocationContext *LCtx)
: AnyCXXConstructorCall(CE, Target, St, LCtx) {}

CXXInheritedConstructorCall(const CXXInheritedConstructorCall &Other) =
default;

void cloneTo(void *Dest) const override {
new (Dest) CXXInheritedConstructorCall(*this);
}

public:
const CXXInheritedCtorInitExpr *getOriginExpr() const override {
return cast<CXXInheritedCtorInitExpr>(AnyFunctionCall::getOriginExpr());
}

const CXXConstructorDecl *getDecl() const override {
return getOriginExpr()->getConstructor();
}



const StackFrameContext *getInheritingStackFrame() const;




const CXXConstructExpr *getInheritingConstructor() const {
return cast<CXXConstructExpr>(getInheritingStackFrame()->getCallSite());
}

unsigned getNumArgs() const override {
return getInheritingConstructor()->getNumArgs();
}

const Expr *getArgExpr(unsigned Index) const override {
return getInheritingConstructor()->getArg(Index);
}

SVal getArgSVal(unsigned Index) const override {
return getState()->getSVal(
getArgExpr(Index),
getInheritingStackFrame()->getParent()->getStackFrame());
}

Kind getKind() const override { return CE_CXXInheritedConstructor; }
StringRef getKindAsString() const override {
return "CXXInheritedConstructorCall";
}

static bool classof(const CallEvent *CA) {
return CA->getKind() == CE_CXXInheritedConstructor;
}
};




class CXXAllocatorCall : public AnyFunctionCall {
friend class CallEventManager;

protected:
CXXAllocatorCall(const CXXNewExpr *E, ProgramStateRef St,
const LocationContext *LCtx)
: AnyFunctionCall(E, St, LCtx) {}
CXXAllocatorCall(const CXXAllocatorCall &Other) = default;

void cloneTo(void *Dest) const override { new (Dest) CXXAllocatorCall(*this); }

public:
const CXXNewExpr *getOriginExpr() const override {
return cast<CXXNewExpr>(AnyFunctionCall::getOriginExpr());
}

const FunctionDecl *getDecl() const override {
return getOriginExpr()->getOperatorNew();
}

SVal getObjectUnderConstruction() const {
return ExprEngine::getObjectUnderConstruction(getState(), getOriginExpr(),
getLocationContext())
.getValue();
}




unsigned getNumImplicitArgs() const {
return getOriginExpr()->passAlignment() ? 2 : 1;
}

unsigned getNumArgs() const override {
return getOriginExpr()->getNumPlacementArgs() + getNumImplicitArgs();
}

const Expr *getArgExpr(unsigned Index) const override {

if (Index < getNumImplicitArgs())
return nullptr;
return getOriginExpr()->getPlacementArg(Index - getNumImplicitArgs());
}





const Expr *getPlacementArgExpr(unsigned Index) const {
return getOriginExpr()->getPlacementArg(Index);
}

Kind getKind() const override { return CE_CXXAllocator; }
StringRef getKindAsString() const override { return "CXXAllocatorCall"; }

static bool classof(const CallEvent *CE) {
return CE->getKind() == CE_CXXAllocator;
}
};











class CXXDeallocatorCall : public AnyFunctionCall {
friend class CallEventManager;

protected:
CXXDeallocatorCall(const CXXDeleteExpr *E, ProgramStateRef St,
const LocationContext *LCtx)
: AnyFunctionCall(E, St, LCtx) {}
CXXDeallocatorCall(const CXXDeallocatorCall &Other) = default;

void cloneTo(void *Dest) const override {
new (Dest) CXXDeallocatorCall(*this);
}

public:
const CXXDeleteExpr *getOriginExpr() const override {
return cast<CXXDeleteExpr>(AnyFunctionCall::getOriginExpr());
}

const FunctionDecl *getDecl() const override {
return getOriginExpr()->getOperatorDelete();
}

unsigned getNumArgs() const override { return getDecl()->getNumParams(); }

const Expr *getArgExpr(unsigned Index) const override {

return getOriginExpr()->getArgument();
}

Kind getKind() const override { return CE_CXXDeallocator; }
StringRef getKindAsString() const override { return "CXXDeallocatorCall"; }

static bool classof(const CallEvent *CE) {
return CE->getKind() == CE_CXXDeallocator;
}
};





enum ObjCMessageKind {
OCM_PropertyAccess,
OCM_Subscript,
OCM_Message
};




class ObjCMethodCall : public CallEvent {
friend class CallEventManager;

const PseudoObjectExpr *getContainingPseudoObjectExpr() const;

protected:
ObjCMethodCall(const ObjCMessageExpr *Msg, ProgramStateRef St,
const LocationContext *LCtx)
: CallEvent(Msg, St, LCtx) {
Data = nullptr;
}

ObjCMethodCall(const ObjCMethodCall &Other) = default;

void cloneTo(void *Dest) const override { new (Dest) ObjCMethodCall(*this); }

void getExtraInvalidatedValues(ValueList &Values,
RegionAndSymbolInvalidationTraits *ETraits) const override;


virtual bool canBeOverridenInSubclass(ObjCInterfaceDecl *IDecl,
Selector Sel) const;

public:
const ObjCMessageExpr *getOriginExpr() const override {
return cast<ObjCMessageExpr>(CallEvent::getOriginExpr());
}

const ObjCMethodDecl *getDecl() const override {
return getOriginExpr()->getMethodDecl();
}

unsigned getNumArgs() const override {
return getOriginExpr()->getNumArgs();
}

const Expr *getArgExpr(unsigned Index) const override {
return getOriginExpr()->getArg(Index);
}

bool isInstanceMessage() const {
return getOriginExpr()->isInstanceMessage();
}

ObjCMethodFamily getMethodFamily() const {
return getOriginExpr()->getMethodFamily();
}

Selector getSelector() const {
return getOriginExpr()->getSelector();
}

SourceRange getSourceRange() const override;


SVal getReceiverSVal() const;





const ObjCInterfaceDecl *getReceiverInterface() const {
return getOriginExpr()->getReceiverInterface();
}


bool isReceiverSelfOrSuper() const;



ObjCMessageKind getMessageKind() const;



bool isSetter() const {
switch (getMessageKind()) {
case OCM_Message:
llvm_unreachable("This is not a pseudo-object access!");
case OCM_PropertyAccess:
return getNumArgs() > 0;
case OCM_Subscript:
return getNumArgs() > 1;
}
llvm_unreachable("Unknown message kind");
}




const ObjCPropertyDecl *getAccessedProperty() const;

RuntimeDefinition getRuntimeDefinition() const override;

bool argumentsMayEscape() const override;

void getInitialStackFrameContents(const StackFrameContext *CalleeCtx,
BindingsTy &Bindings) const override;

ArrayRef<ParmVarDecl*> parameters() const override;

Kind getKind() const override { return CE_ObjCMessage; }
StringRef getKindAsString() const override { return "ObjCMethodCall"; }

static bool classof(const CallEvent *CA) {
return CA->getKind() == CE_ObjCMessage;
}
};

enum CallDescriptionFlags : int {




CDF_MaybeBuiltin = 1 << 0,
};



class CallDescription {
friend CallEvent;

mutable IdentifierInfo *II = nullptr;
mutable bool IsLookupDone = false;


std::vector<const char *> QualifiedName;
Optional<unsigned> RequiredArgs;
Optional<size_t> RequiredParams;
int Flags;


static Optional<size_t> readRequiredParams(Optional<unsigned> RequiredArgs,
Optional<size_t> RequiredParams) {
if (RequiredParams)
return RequiredParams;
if (RequiredArgs)
return static_cast<size_t>(*RequiredArgs);
return None;
}

public:










CallDescription(int Flags, ArrayRef<const char *> QualifiedName,
Optional<unsigned> RequiredArgs = None,
Optional<size_t> RequiredParams = None)
: QualifiedName(QualifiedName), RequiredArgs(RequiredArgs),
RequiredParams(readRequiredParams(RequiredArgs, RequiredParams)),
Flags(Flags) {}


CallDescription(ArrayRef<const char *> QualifiedName,
Optional<unsigned> RequiredArgs = None,
Optional<size_t> RequiredParams = None)
: CallDescription(0, QualifiedName, RequiredArgs, RequiredParams) {}


StringRef getFunctionName() const { return QualifiedName.back(); }
};



template <typename T> class CallDescriptionMap {





std::vector<std::pair<CallDescription, T>> LinearMap;

public:
CallDescriptionMap(
std::initializer_list<std::pair<CallDescription, T>> &&List)
: LinearMap(List) {}

~CallDescriptionMap() = default;



CallDescriptionMap(const CallDescriptionMap &) = delete;
CallDescriptionMap &operator=(const CallDescription &) = delete;

const T *lookup(const CallEvent &Call) const {


for (const std::pair<CallDescription, T> &I : LinearMap)
if (Call.isCalled(I.first))
return &I.second;

return nullptr;
}
};








class CallEventManager {
friend class CallEvent;

llvm::BumpPtrAllocator &Alloc;
SmallVector<void *, 8> Cache;

using CallEventTemplateTy = SimpleFunctionCall;

void reclaim(const void *Memory) {
Cache.push_back(const_cast<void *>(Memory));
}


void *allocate() {
if (Cache.empty())
return Alloc.Allocate<CallEventTemplateTy>();
else
return Cache.pop_back_val();
}

template <typename T, typename Arg>
T *create(Arg A, ProgramStateRef St, const LocationContext *LCtx) {
static_assert(sizeof(T) == sizeof(CallEventTemplateTy),
"CallEvent subclasses are not all the same size");
return new (allocate()) T(A, St, LCtx);
}

template <typename T, typename Arg1, typename Arg2>
T *create(Arg1 A1, Arg2 A2, ProgramStateRef St, const LocationContext *LCtx) {
static_assert(sizeof(T) == sizeof(CallEventTemplateTy),
"CallEvent subclasses are not all the same size");
return new (allocate()) T(A1, A2, St, LCtx);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3>
T *create(Arg1 A1, Arg2 A2, Arg3 A3, ProgramStateRef St,
const LocationContext *LCtx) {
static_assert(sizeof(T) == sizeof(CallEventTemplateTy),
"CallEvent subclasses are not all the same size");
return new (allocate()) T(A1, A2, A3, St, LCtx);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3,
typename Arg4>
T *create(Arg1 A1, Arg2 A2, Arg3 A3, Arg4 A4, ProgramStateRef St,
const LocationContext *LCtx) {
static_assert(sizeof(T) == sizeof(CallEventTemplateTy),
"CallEvent subclasses are not all the same size");
return new (allocate()) T(A1, A2, A3, A4, St, LCtx);
}

public:
CallEventManager(llvm::BumpPtrAllocator &alloc) : Alloc(alloc) {}


CallEventRef<>
getCaller(const StackFrameContext *CalleeCtx, ProgramStateRef State);



CallEventRef<>
getCall(const Stmt *S, ProgramStateRef State,
const LocationContext *LC);

CallEventRef<>
getSimpleCall(const CallExpr *E, ProgramStateRef State,
const LocationContext *LCtx);

CallEventRef<ObjCMethodCall>
getObjCMethodCall(const ObjCMessageExpr *E, ProgramStateRef State,
const LocationContext *LCtx) {
return create<ObjCMethodCall>(E, State, LCtx);
}

CallEventRef<CXXConstructorCall>
getCXXConstructorCall(const CXXConstructExpr *E, const MemRegion *Target,
ProgramStateRef State, const LocationContext *LCtx) {
return create<CXXConstructorCall>(E, Target, State, LCtx);
}

CallEventRef<CXXInheritedConstructorCall>
getCXXInheritedConstructorCall(const CXXInheritedCtorInitExpr *E,
const MemRegion *Target, ProgramStateRef State,
const LocationContext *LCtx) {
return create<CXXInheritedConstructorCall>(E, Target, State, LCtx);
}

CallEventRef<CXXDestructorCall>
getCXXDestructorCall(const CXXDestructorDecl *DD, const Stmt *Trigger,
const MemRegion *Target, bool IsBase,
ProgramStateRef State, const LocationContext *LCtx) {
return create<CXXDestructorCall>(DD, Trigger, Target, IsBase, State, LCtx);
}

CallEventRef<CXXAllocatorCall>
getCXXAllocatorCall(const CXXNewExpr *E, ProgramStateRef State,
const LocationContext *LCtx) {
return create<CXXAllocatorCall>(E, State, LCtx);
}

CallEventRef<CXXDeallocatorCall>
getCXXDeallocatorCall(const CXXDeleteExpr *E, ProgramStateRef State,
const LocationContext *LCtx) {
return create<CXXDeallocatorCall>(E, State, LCtx);
}
};

template <typename T>
CallEventRef<T> CallEvent::cloneWithState(ProgramStateRef NewState) const {
assert(isa<T>(*this) && "Cloning to unrelated type");
static_assert(sizeof(T) == sizeof(CallEvent),
"Subclasses may not add fields");

if (NewState == State)
return cast<T>(this);

CallEventManager &Mgr = State->getStateManager().getCallEventManager();
T *Copy = static_cast<T *>(Mgr.allocate());
cloneTo(Copy);
assert(Copy->getKind() == this->getKind() && "Bad copy");

Copy->State = NewState;
return Copy;
}

inline void CallEvent::Release() const {
assert(RefCount > 0 && "Reference count is already zero.");
--RefCount;

if (RefCount > 0)
return;

CallEventManager &Mgr = State->getStateManager().getCallEventManager();
Mgr.reclaim(this);

this->~CallEvent();
}

}

}

namespace llvm {


template<class T> struct simplify_type< clang::ento::CallEventRef<T>> {
using SimpleType = const T *;

static SimpleType
getSimplifiedValue(clang::ento::CallEventRef<T> Val) {
return Val.get();
}
};

}

#endif
