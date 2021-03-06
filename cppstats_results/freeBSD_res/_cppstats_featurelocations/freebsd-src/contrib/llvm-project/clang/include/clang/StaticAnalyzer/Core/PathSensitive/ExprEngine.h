













#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_EXPRENGINE_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_EXPRENGINE_H

#include "clang/AST/Expr.h"
#include "clang/AST/Type.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/DomainSpecific/ObjCNoReturn.h"
#include "clang/Analysis/ProgramPoint.h"
#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporterVisitors.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/AnalysisManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CoreEngine.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/FunctionSummary.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/Store.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SValBuilder.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/WorkList.h"
#include "llvm/ADT/ArrayRef.h"
#include <cassert>
#include <utility>

namespace clang {

class AnalysisDeclContextManager;
class AnalyzerOptions;
class ASTContext;
class CFGBlock;
class CFGElement;
class ConstructionContext;
class CXXBindTemporaryExpr;
class CXXCatchStmt;
class CXXConstructExpr;
class CXXDeleteExpr;
class CXXNewExpr;
class CXXThisExpr;
class Decl;
class DeclStmt;
class GCCAsmStmt;
class LambdaExpr;
class LocationContext;
class MaterializeTemporaryExpr;
class MSAsmStmt;
class NamedDecl;
class ObjCAtSynchronizedStmt;
class ObjCForCollectionStmt;
class ObjCIvarRefExpr;
class ObjCMessageExpr;
class ReturnStmt;
class Stmt;

namespace cross_tu {

class CrossTranslationUnitContext;

}

namespace ento {

class AnalysisManager;
class BasicValueFactory;
class BlockCounter;
class BranchNodeBuilder;
class CallEvent;
class CheckerManager;
class ConstraintManager;
class CXXTempObjectRegion;
class EndOfFunctionNodeBuilder;
class ExplodedNodeSet;
class ExplodedNode;
class IndirectGotoNodeBuilder;
class MemRegion;
struct NodeBuilderContext;
class NodeBuilderWithSinks;
class ProgramState;
class ProgramStateManager;
class RegionAndSymbolInvalidationTraits;
class SymbolManager;
class SwitchNodeBuilder;


struct EvalCallOptions {


bool IsCtorOrDtorWithImproperlyModeledTargetRegion = false;



bool IsArrayCtorOrDtor = false;


bool IsTemporaryCtorOrDtor = false;




bool IsTemporaryLifetimeExtendedViaAggregate = false;






bool IsElidableCtorThatHasNotBeenElided = false;

EvalCallOptions() {}
};

class ExprEngine {
void anchor();

public:

enum InliningModes {

Inline_Regular = 0,


Inline_Minimal = 0x1
};

private:
cross_tu::CrossTranslationUnitContext &CTU;

AnalysisManager &AMgr;

AnalysisDeclContextManager &AnalysisDeclContexts;

CoreEngine Engine;


ExplodedGraph &G;


ProgramStateManager StateMgr;


SymbolManager &SymMgr;


MemRegionManager &MRMgr;


SValBuilder &svalBuilder;

unsigned int currStmtIdx = 0;
const NodeBuilderContext *currBldrCtx = nullptr;



ObjCNoReturn ObjCNoRet;




PathSensitiveBugReporter BR;



SetOfConstDecls *VisitedCallees;


InliningModes HowToInline;

public:
ExprEngine(cross_tu::CrossTranslationUnitContext &CTU, AnalysisManager &mgr,
SetOfConstDecls *VisitedCalleesIn,
FunctionSummariesTy *FS, InliningModes HowToInlineIn);

virtual ~ExprEngine() = default;


bool ExecuteWorkList(const LocationContext *L, unsigned Steps = 150000) {
return Engine.ExecuteWorkList(L, Steps, nullptr);
}





bool ExecuteWorkListWithInitialState(const LocationContext *L, unsigned Steps,
ProgramStateRef InitState,
ExplodedNodeSet &Dst) {
return Engine.ExecuteWorkListWithInitialState(L, Steps, InitState, Dst);
}


ASTContext &getContext() const { return AMgr.getASTContext(); }

AnalysisManager &getAnalysisManager() { return AMgr; }

AnalysisDeclContextManager &getAnalysisDeclContextManager() {
return AMgr.getAnalysisDeclContextManager();
}

CheckerManager &getCheckerManager() const {
return *AMgr.getCheckerManager();
}

SValBuilder &getSValBuilder() { return svalBuilder; }

BugReporter &getBugReporter() { return BR; }

cross_tu::CrossTranslationUnitContext *
getCrossTranslationUnitContext() {
return &CTU;
}

const NodeBuilderContext &getBuilderContext() {
assert(currBldrCtx);
return *currBldrCtx;
}

const Stmt *getStmt() const;

void GenerateAutoTransition(ExplodedNode *N);
void enqueueEndOfPath(ExplodedNodeSet &S);
void GenerateCallExitNode(ExplodedNode *N);





std::string DumpGraph(bool trim = false, StringRef Filename="");




std::string DumpGraph(ArrayRef<const ExplodedNode *> Nodes,
StringRef Filename = "");


void ViewGraph(bool trim = false);



void ViewGraph(ArrayRef<const ExplodedNode *> Nodes);



ProgramStateRef getInitialState(const LocationContext *InitLoc);

ExplodedGraph &getGraph() { return G; }
const ExplodedGraph &getGraph() const { return G; }


























void removeDead(ExplodedNode *Node, ExplodedNodeSet &Out,
const Stmt *ReferenceStmt, const LocationContext *LC,
const Stmt *DiagnosticStmt = nullptr,
ProgramPoint::Kind K = ProgramPoint::PreStmtPurgeDeadSymbolsKind);



void processCFGElement(const CFGElement E, ExplodedNode *Pred,
unsigned StmtIdx, NodeBuilderContext *Ctx);

void ProcessStmt(const Stmt *S, ExplodedNode *Pred);

void ProcessLoopExit(const Stmt* S, ExplodedNode *Pred);

void ProcessInitializer(const CFGInitializer I, ExplodedNode *Pred);

void ProcessImplicitDtor(const CFGImplicitDtor D, ExplodedNode *Pred);

void ProcessNewAllocator(const CXXNewExpr *NE, ExplodedNode *Pred);

void ProcessAutomaticObjDtor(const CFGAutomaticObjDtor D,
ExplodedNode *Pred, ExplodedNodeSet &Dst);
void ProcessDeleteDtor(const CFGDeleteDtor D,
ExplodedNode *Pred, ExplodedNodeSet &Dst);
void ProcessBaseDtor(const CFGBaseDtor D,
ExplodedNode *Pred, ExplodedNodeSet &Dst);
void ProcessMemberDtor(const CFGMemberDtor D,
ExplodedNode *Pred, ExplodedNodeSet &Dst);
void ProcessTemporaryDtor(const CFGTemporaryDtor D,
ExplodedNode *Pred, ExplodedNodeSet &Dst);


void processCFGBlockEntrance(const BlockEdge &L,
NodeBuilderWithSinks &nodeBuilder,
ExplodedNode *Pred);



void processBranch(const Stmt *Condition,
NodeBuilderContext& BuilderCtx,
ExplodedNode *Pred,
ExplodedNodeSet &Dst,
const CFGBlock *DstT,
const CFGBlock *DstF);




void processCleanupTemporaryBranch(const CXXBindTemporaryExpr *BTE,
NodeBuilderContext &BldCtx,
ExplodedNode *Pred, ExplodedNodeSet &Dst,
const CFGBlock *DstT,
const CFGBlock *DstF);



void processStaticInitializer(const DeclStmt *DS,
NodeBuilderContext& BuilderCtx,
ExplodedNode *Pred,
ExplodedNodeSet &Dst,
const CFGBlock *DstT,
const CFGBlock *DstF);



void processIndirectGoto(IndirectGotoNodeBuilder& builder);



void processSwitch(SwitchNodeBuilder& builder);



void processBeginOfFunction(NodeBuilderContext &BC,
ExplodedNode *Pred, ExplodedNodeSet &Dst,
const BlockEdge &L);



void processEndOfFunction(NodeBuilderContext& BC,
ExplodedNode *Pred,
const ReturnStmt *RS = nullptr);


void removeDeadOnEndOfFunction(NodeBuilderContext& BC,
ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void processCallEnter(NodeBuilderContext& BC, CallEnter CE,
ExplodedNode *Pred);



void processCallExit(ExplodedNode *Pred);


void processEndWorklist();



ProgramStateRef processAssume(ProgramStateRef state, SVal cond,
bool assumption);



ProgramStateRef
processRegionChanges(ProgramStateRef state,
const InvalidatedSymbols *invalidated,
ArrayRef<const MemRegion *> ExplicitRegions,
ArrayRef<const MemRegion *> Regions,
const LocationContext *LCtx,
const CallEvent *Call);

inline ProgramStateRef
processRegionChange(ProgramStateRef state,
const MemRegion* MR,
const LocationContext *LCtx) {
return processRegionChanges(state, nullptr, MR, MR, LCtx, nullptr);
}


void printJson(raw_ostream &Out, ProgramStateRef State,
const LocationContext *LCtx, const char *NL,
unsigned int Space, bool IsDot) const;

ProgramStateManager &getStateManager() { return StateMgr; }

StoreManager &getStoreManager() { return StateMgr.getStoreManager(); }

ConstraintManager &getConstraintManager() {
return StateMgr.getConstraintManager();
}


BasicValueFactory &getBasicVals() {
return StateMgr.getBasicVals();
}

SymbolManager &getSymbolManager() { return SymMgr; }
MemRegionManager &getRegionManager() { return MRMgr; }

DataTag::Factory &getDataTags() { return Engine.getDataTags(); }


bool wasBlocksExhausted() const { return Engine.wasBlocksExhausted(); }
bool hasEmptyWorkList() const { return !Engine.getWorkList()->hasWork(); }
bool hasWorkRemaining() const { return Engine.hasWorkRemaining(); }

const CoreEngine &getCoreEngine() const { return Engine; }

public:


void Visit(const Stmt *S, ExplodedNode *Pred, ExplodedNodeSet &Dst);


void VisitArraySubscriptExpr(const ArraySubscriptExpr *Ex,
ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitGCCAsmStmt(const GCCAsmStmt *A, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitMSAsmStmt(const MSAsmStmt *A, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitBlockExpr(const BlockExpr *BE, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitLambdaExpr(const LambdaExpr *LE, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitBinaryOperator(const BinaryOperator* B, ExplodedNode *Pred,
ExplodedNodeSet &Dst);



void VisitCallExpr(const CallExpr *CE, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitCast(const CastExpr *CastE, const Expr *Ex, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitCompoundLiteralExpr(const CompoundLiteralExpr *CL,
ExplodedNode *Pred, ExplodedNodeSet &Dst);


void VisitCommonDeclRefExpr(const Expr *DR, const NamedDecl *D,
ExplodedNode *Pred, ExplodedNodeSet &Dst);


void VisitDeclStmt(const DeclStmt *DS, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitGuardedExpr(const Expr *Ex, const Expr *L, const Expr *R,
ExplodedNode *Pred, ExplodedNodeSet &Dst);

void VisitInitListExpr(const InitListExpr *E, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitLogicalExpr(const BinaryOperator* B, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitMemberExpr(const MemberExpr *M, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitAtomicExpr(const AtomicExpr *E, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitObjCAtSynchronizedStmt(const ObjCAtSynchronizedStmt *S,
ExplodedNode *Pred, ExplodedNodeSet &Dst);


void VisitLvalObjCIvarRefExpr(const ObjCIvarRefExpr *DR, ExplodedNode *Pred,
ExplodedNodeSet &Dst);



void VisitObjCForCollectionStmt(const ObjCForCollectionStmt *S,
ExplodedNode *Pred, ExplodedNodeSet &Dst);

void VisitObjCMessage(const ObjCMessageExpr *ME, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitReturnStmt(const ReturnStmt *R, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitOffsetOfExpr(const OffsetOfExpr *Ex, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitUnaryExprOrTypeTraitExpr(const UnaryExprOrTypeTraitExpr *Ex,
ExplodedNode *Pred, ExplodedNodeSet &Dst);


void VisitUnaryOperator(const UnaryOperator* B, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void VisitIncrementDecrementOperator(const UnaryOperator* U,
ExplodedNode *Pred,
ExplodedNodeSet &Dst);

void VisitCXXBindTemporaryExpr(const CXXBindTemporaryExpr *BTE,
ExplodedNodeSet &PreVisit,
ExplodedNodeSet &Dst);

void VisitCXXCatchStmt(const CXXCatchStmt *CS, ExplodedNode *Pred,
ExplodedNodeSet &Dst);

void VisitCXXThisExpr(const CXXThisExpr *TE, ExplodedNode *Pred,
ExplodedNodeSet & Dst);

void VisitCXXConstructExpr(const CXXConstructExpr *E, ExplodedNode *Pred,
ExplodedNodeSet &Dst);

void VisitCXXInheritedCtorInitExpr(const CXXInheritedCtorInitExpr *E,
ExplodedNode *Pred, ExplodedNodeSet &Dst);

void VisitCXXDestructor(QualType ObjectType, const MemRegion *Dest,
const Stmt *S, bool IsBaseDtor,
ExplodedNode *Pred, ExplodedNodeSet &Dst,
EvalCallOptions &Options);

void VisitCXXNewAllocatorCall(const CXXNewExpr *CNE,
ExplodedNode *Pred,
ExplodedNodeSet &Dst);

void VisitCXXNewExpr(const CXXNewExpr *CNE, ExplodedNode *Pred,
ExplodedNodeSet &Dst);

void VisitCXXDeleteExpr(const CXXDeleteExpr *CDE, ExplodedNode *Pred,
ExplodedNodeSet &Dst);


void CreateCXXTemporaryObject(const MaterializeTemporaryExpr *ME,
ExplodedNode *Pred,
ExplodedNodeSet &Dst);




void evalEagerlyAssumeBinOpBifurcation(ExplodedNodeSet &Dst, ExplodedNodeSet &Src,
const Expr *Ex);

static std::pair<const ProgramPointTag *, const ProgramPointTag *>
geteagerlyAssumeBinOpBifurcationTags();

SVal evalMinus(SVal X) {
return X.isValid() ? svalBuilder.evalMinus(X.castAs<NonLoc>()) : X;
}

SVal evalComplement(SVal X) {
return X.isValid() ? svalBuilder.evalComplement(X.castAs<NonLoc>()) : X;
}

ProgramStateRef handleLValueBitCast(ProgramStateRef state, const Expr *Ex,
const LocationContext *LCtx, QualType T,
QualType ExTy, const CastExpr *CastE,
StmtNodeBuilder &Bldr,
ExplodedNode *Pred);

ProgramStateRef handleLVectorSplat(ProgramStateRef state,
const LocationContext *LCtx,
const CastExpr *CastE,
StmtNodeBuilder &Bldr,
ExplodedNode *Pred);

void handleUOExtension(ExplodedNodeSet::iterator I,
const UnaryOperator* U,
StmtNodeBuilder &Bldr);

public:
SVal evalBinOp(ProgramStateRef state, BinaryOperator::Opcode op,
NonLoc L, NonLoc R, QualType T) {
return svalBuilder.evalBinOpNN(state, op, L, R, T);
}

SVal evalBinOp(ProgramStateRef state, BinaryOperator::Opcode op,
NonLoc L, SVal R, QualType T) {
return R.isValid() ? svalBuilder.evalBinOpNN(state, op, L,
R.castAs<NonLoc>(), T) : R;
}

SVal evalBinOp(ProgramStateRef ST, BinaryOperator::Opcode Op,
SVal LHS, SVal RHS, QualType T) {
return svalBuilder.evalBinOp(ST, Op, LHS, RHS, T);
}




static Optional<SVal>
getObjectUnderConstruction(ProgramStateRef State,
const ConstructionContextItem &Item,
const LocationContext *LC);


ProgramStateRef processPointerEscapedOnBind(
ProgramStateRef State, ArrayRef<std::pair<SVal, SVal>> LocAndVals,
const LocationContext *LCtx, PointerEscapeKind Kind,
const CallEvent *Call);




ProgramStateRef notifyCheckersOfPointerEscape(
ProgramStateRef State,
const InvalidatedSymbols *Invalidated,
ArrayRef<const MemRegion *> ExplicitRegions,
const CallEvent *Call,
RegionAndSymbolInvalidationTraits &ITraits);

private:


void evalBind(ExplodedNodeSet &Dst, const Stmt *StoreE, ExplodedNode *Pred,
SVal location, SVal Val, bool atDeclInit = false,
const ProgramPoint *PP = nullptr);

ProgramStateRef
processPointerEscapedOnBind(ProgramStateRef State,
SVal Loc, SVal Val,
const LocationContext *LCtx);



ProgramStateRef escapeValues(ProgramStateRef State, ArrayRef<SVal> Vs,
PointerEscapeKind K,
const CallEvent *Call = nullptr) const;

public:






void evalLoad(ExplodedNodeSet &Dst,
const Expr *NodeEx,
const Expr *BoundExpr,
ExplodedNode *Pred,
ProgramStateRef St,
SVal location,
const ProgramPointTag *tag = nullptr,
QualType LoadTy = QualType());



void evalStore(ExplodedNodeSet &Dst, const Expr *AssignE, const Expr *StoreE,
ExplodedNode *Pred, ProgramStateRef St, SVal TargetLV, SVal Val,
const ProgramPointTag *tag = nullptr);



CFGElement getCurrentCFGElement() {
return (*currBldrCtx->getBlock())[currStmtIdx];
}



ProgramStateRef bindReturnValue(const CallEvent &Call,
const LocationContext *LCtx,
ProgramStateRef State);



void evalCall(ExplodedNodeSet &Dst, ExplodedNode *Pred,
const CallEvent &Call);


void defaultEvalCall(NodeBuilder &B, ExplodedNode *Pred,
const CallEvent &Call,
const EvalCallOptions &CallOpts = {});







SVal computeObjectUnderConstruction(
const Expr *E, ProgramStateRef State, const LocationContext *LCtx,
const ConstructionContext *CC, EvalCallOptions &CallOpts);






ProgramStateRef updateObjectsUnderConstruction(
SVal V, const Expr *E, ProgramStateRef State, const LocationContext *LCtx,
const ConstructionContext *CC, const EvalCallOptions &CallOpts);



std::pair<ProgramStateRef, SVal> handleConstructionContext(
const Expr *E, ProgramStateRef State, const LocationContext *LCtx,
const ConstructionContext *CC, EvalCallOptions &CallOpts) {
SVal V = computeObjectUnderConstruction(E, State, LCtx, CC, CallOpts);
return std::make_pair(
updateObjectsUnderConstruction(V, E, State, LCtx, CC, CallOpts), V);
}

private:
ProgramStateRef finishArgumentConstruction(ProgramStateRef State,
const CallEvent &Call);
void finishArgumentConstruction(ExplodedNodeSet &Dst, ExplodedNode *Pred,
const CallEvent &Call);

void evalLoadCommon(ExplodedNodeSet &Dst,
const Expr *NodeEx,
const Expr *BoundEx,
ExplodedNode *Pred,
ProgramStateRef St,
SVal location,
const ProgramPointTag *tag,
QualType LoadTy);

void evalLocation(ExplodedNodeSet &Dst,
const Stmt *NodeEx,
const Stmt *BoundEx,
ExplodedNode *Pred,
ProgramStateRef St,
SVal location,
bool isLoad);


void examineStackFrames(const Decl *D, const LocationContext *LCtx,
bool &IsRecursive, unsigned &StackDepth);

enum CallInlinePolicy {
CIP_Allowed,
CIP_DisallowedOnce,
CIP_DisallowedAlways
};



CallInlinePolicy mayInlineCallKind(const CallEvent &Call,
const ExplodedNode *Pred,
AnalyzerOptions &Opts,
const EvalCallOptions &CallOpts);






bool isSmall(AnalysisDeclContext *ADC) const;



bool isLarge(AnalysisDeclContext *ADC) const;



bool isHuge(AnalysisDeclContext *ADC) const;



bool mayInlineDecl(AnalysisDeclContext *ADC) const;


bool shouldInlineCall(const CallEvent &Call, const Decl *D,
const ExplodedNode *Pred,
const EvalCallOptions &CallOpts = {});

bool inlineCall(const CallEvent &Call, const Decl *D, NodeBuilder &Bldr,
ExplodedNode *Pred, ProgramStateRef State);



void conservativeEvalCall(const CallEvent &Call, NodeBuilder &Bldr,
ExplodedNode *Pred, ProgramStateRef State);



void BifurcateCall(const MemRegion *BifurReg,
const CallEvent &Call, const Decl *D, NodeBuilder &Bldr,
ExplodedNode *Pred);

bool replayWithoutInlining(ExplodedNode *P, const LocationContext *CalleeLC);



void performTrivialCopy(NodeBuilder &Bldr, ExplodedNode *Pred,
const CallEvent &Call);











ProgramStateRef createTemporaryRegionIfNeeded(
ProgramStateRef State, const LocationContext *LC,
const Expr *InitWithAdjustments, const Expr *Result = nullptr,
const SubRegion **OutRegionWithAdjustments = nullptr);









static SVal makeZeroElementRegion(ProgramStateRef State, SVal LValue,
QualType &Ty, bool &IsArray);






const CXXConstructExpr *findDirectConstructorForCurrentCFGElement();



void handleConstructor(const Expr *E, ExplodedNode *Pred,
ExplodedNodeSet &Dst);

public:



LLVM_NODISCARD static ProgramStateRef
setWhetherHasMoreIteration(ProgramStateRef State,
const ObjCForCollectionStmt *O,
const LocationContext *LC, bool HasMoreIteraton);

LLVM_NODISCARD static ProgramStateRef
removeIterationState(ProgramStateRef State, const ObjCForCollectionStmt *O,
const LocationContext *LC);

LLVM_NODISCARD static bool hasMoreIteration(ProgramStateRef State,
const ObjCForCollectionStmt *O,
const LocationContext *LC);
private:







static ProgramStateRef
addObjectUnderConstruction(ProgramStateRef State,
const ConstructionContextItem &Item,
const LocationContext *LC, SVal V);



static ProgramStateRef
finishObjectConstruction(ProgramStateRef State,
const ConstructionContextItem &Item,
const LocationContext *LC);




static ProgramStateRef elideDestructor(ProgramStateRef State,
const CXXBindTemporaryExpr *BTE,
const LocationContext *LC);


static ProgramStateRef
cleanupElidedDestructor(ProgramStateRef State,
const CXXBindTemporaryExpr *BTE,
const LocationContext *LC);




static bool isDestructorElided(ProgramStateRef State,
const CXXBindTemporaryExpr *BTE,
const LocationContext *LC);





static bool areAllObjectsFullyConstructed(ProgramStateRef State,
const LocationContext *FromLC,
const LocationContext *ToLC);
};





struct ReplayWithoutInlining{};
template <>
struct ProgramStateTrait<ReplayWithoutInlining> :
public ProgramStatePartialTrait<const void*> {
static void *GDMIndex();
};

}

}

#endif
