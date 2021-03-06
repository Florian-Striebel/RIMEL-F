












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_COREENGINE_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_COREENGINE_H

#include "clang/AST/Stmt.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/ProgramPoint.h"
#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/BlockCounter.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExplodedGraph.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/WorkList.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace clang {

class AnalyzerOptions;
class CXXBindTemporaryExpr;
class Expr;
class LabelDecl;

namespace ento {

class FunctionSummariesTy;
class ExprEngine;










class CoreEngine {
friend class CommonNodeBuilder;
friend class EndOfFunctionNodeBuilder;
friend class ExprEngine;
friend class IndirectGotoNodeBuilder;
friend class NodeBuilder;
friend struct NodeBuilderContext;
friend class SwitchNodeBuilder;

public:
using BlocksExhausted =
std::vector<std::pair<BlockEdge, const ExplodedNode *>>;

using BlocksAborted =
std::vector<std::pair<const CFGBlock *, const ExplodedNode *>>;

private:
ExprEngine &ExprEng;


mutable ExplodedGraph G;




std::unique_ptr<WorkList> WList;




BlockCounter::Factory BCounterFactory;



BlocksExhausted blocksExhausted;



BlocksAborted blocksAborted;



FunctionSummariesTy *FunctionSummaries;




DataTag::Factory DataTags;

void generateNode(const ProgramPoint &Loc,
ProgramStateRef State,
ExplodedNode *Pred);

void HandleBlockEdge(const BlockEdge &E, ExplodedNode *Pred);
void HandleBlockEntrance(const BlockEntrance &E, ExplodedNode *Pred);
void HandleBlockExit(const CFGBlock *B, ExplodedNode *Pred);

void HandleCallEnter(const CallEnter &CE, ExplodedNode *Pred);

void HandlePostStmt(const CFGBlock *B, unsigned StmtIdx, ExplodedNode *Pred);

void HandleBranch(const Stmt *Cond, const Stmt *Term, const CFGBlock *B,
ExplodedNode *Pred);
void HandleCleanupTemporaryBranch(const CXXBindTemporaryExpr *BTE,
const CFGBlock *B, ExplodedNode *Pred);


void HandleStaticInit(const DeclStmt *DS, const CFGBlock *B,
ExplodedNode *Pred);

void HandleVirtualBaseBranch(const CFGBlock *B, ExplodedNode *Pred);

private:
ExplodedNode *generateCallExitBeginNode(ExplodedNode *N,
const ReturnStmt *RS);

public:

CoreEngine(ExprEngine &exprengine,
FunctionSummariesTy *FS,
AnalyzerOptions &Opts);

CoreEngine(const CoreEngine &) = delete;
CoreEngine &operator=(const CoreEngine &) = delete;


ExplodedGraph &getGraph() { return G; }



bool ExecuteWorkList(const LocationContext *L, unsigned Steps,
ProgramStateRef InitState);


bool ExecuteWorkListWithInitialState(const LocationContext *L,
unsigned Steps,
ProgramStateRef InitState,
ExplodedNodeSet &Dst);



void dispatchWorkItem(ExplodedNode* Pred, ProgramPoint Loc,
const WorkListUnit& WU);


bool wasBlockAborted() const { return !blocksAborted.empty(); }
bool wasBlocksExhausted() const { return !blocksExhausted.empty(); }
bool hasWorkRemaining() const { return wasBlocksExhausted() ||
WList->hasWork() ||
wasBlockAborted(); }



void addAbortedBlock(const ExplodedNode *node, const CFGBlock *block) {
blocksAborted.push_back(std::make_pair(block, node));
}

WorkList *getWorkList() const { return WList.get(); }

BlocksExhausted::const_iterator blocks_exhausted_begin() const {
return blocksExhausted.begin();
}

BlocksExhausted::const_iterator blocks_exhausted_end() const {
return blocksExhausted.end();
}

BlocksAborted::const_iterator blocks_aborted_begin() const {
return blocksAborted.begin();
}

BlocksAborted::const_iterator blocks_aborted_end() const {
return blocksAborted.end();
}


void enqueue(ExplodedNodeSet &Set);



void enqueue(ExplodedNodeSet &Set, const CFGBlock *Block, unsigned Idx);



void enqueueEndOfFunction(ExplodedNodeSet &Set, const ReturnStmt *RS);


void enqueueStmtNode(ExplodedNode *N, const CFGBlock *Block, unsigned Idx);

DataTag::Factory &getDataTags() { return DataTags; }
};


struct NodeBuilderContext {
const CoreEngine &Eng;
const CFGBlock *Block;
const LocationContext *LC;

NodeBuilderContext(const CoreEngine &E, const CFGBlock *B, ExplodedNode *N)
: Eng(E), Block(B), LC(N->getLocationContext()) { assert(B); }


const CFGBlock *getBlock() const { return Block; }



unsigned blockCount() const {
return Eng.WList->getBlockCounter().getNumVisited(
LC->getStackFrame(),
Block->getBlockID());
}
};










class NodeBuilder {
virtual void anchor();

protected:
const NodeBuilderContext &C;



bool Finalized;

bool HasGeneratedNodes = false;



ExplodedNodeSet &Frontier;


virtual bool checkResults() {
return Finalized;
}

bool hasNoSinksInFrontier() {
for (const auto I : Frontier)
if (I->isSink())
return false;
return true;
}


virtual void finalizeResults() {}

ExplodedNode *generateNodeImpl(const ProgramPoint &PP,
ProgramStateRef State,
ExplodedNode *Pred,
bool MarkAsSink = false);

public:
NodeBuilder(ExplodedNode *SrcNode, ExplodedNodeSet &DstSet,
const NodeBuilderContext &Ctx, bool F = true)
: C(Ctx), Finalized(F), Frontier(DstSet) {
Frontier.Add(SrcNode);
}

NodeBuilder(const ExplodedNodeSet &SrcSet, ExplodedNodeSet &DstSet,
const NodeBuilderContext &Ctx, bool F = true)
: C(Ctx), Finalized(F), Frontier(DstSet) {
Frontier.insert(SrcSet);
assert(hasNoSinksInFrontier());
}

virtual ~NodeBuilder() = default;


ExplodedNode *generateNode(const ProgramPoint &PP,
ProgramStateRef State,
ExplodedNode *Pred) {
return generateNodeImpl(PP, State, Pred, false);
}






ExplodedNode *generateSink(const ProgramPoint &PP,
ProgramStateRef State,
ExplodedNode *Pred) {
return generateNodeImpl(PP, State, Pred, true);
}

const ExplodedNodeSet &getResults() {
finalizeResults();
assert(checkResults());
return Frontier;
}

using iterator = ExplodedNodeSet::iterator;


iterator begin() {
finalizeResults();
assert(checkResults());
return Frontier.begin();
}

iterator end() {
finalizeResults();
return Frontier.end();
}

const NodeBuilderContext &getContext() { return C; }
bool hasGeneratedNodes() { return HasGeneratedNodes; }

void takeNodes(const ExplodedNodeSet &S) {
for (const auto I : S)
Frontier.erase(I);
}

void takeNodes(ExplodedNode *N) { Frontier.erase(N); }
void addNodes(const ExplodedNodeSet &S) { Frontier.insert(S); }
void addNodes(ExplodedNode *N) { Frontier.Add(N); }
};



class NodeBuilderWithSinks: public NodeBuilder {
void anchor() override;

protected:
SmallVector<ExplodedNode*, 2> sinksGenerated;
ProgramPoint &Location;

public:
NodeBuilderWithSinks(ExplodedNode *Pred, ExplodedNodeSet &DstSet,
const NodeBuilderContext &Ctx, ProgramPoint &L)
: NodeBuilder(Pred, DstSet, Ctx), Location(L) {}

ExplodedNode *generateNode(ProgramStateRef State,
ExplodedNode *Pred,
const ProgramPointTag *Tag = nullptr) {
const ProgramPoint &LocalLoc = (Tag ? Location.withTag(Tag) : Location);
return NodeBuilder::generateNode(LocalLoc, State, Pred);
}

ExplodedNode *generateSink(ProgramStateRef State, ExplodedNode *Pred,
const ProgramPointTag *Tag = nullptr) {
const ProgramPoint &LocalLoc = (Tag ? Location.withTag(Tag) : Location);
ExplodedNode *N = NodeBuilder::generateSink(LocalLoc, State, Pred);
if (N && N->isSink())
sinksGenerated.push_back(N);
return N;
}

const SmallVectorImpl<ExplodedNode*> &getSinks() const {
return sinksGenerated;
}
};





class StmtNodeBuilder: public NodeBuilder {
NodeBuilder *EnclosingBldr;

public:



StmtNodeBuilder(ExplodedNode *SrcNode, ExplodedNodeSet &DstSet,
const NodeBuilderContext &Ctx,
NodeBuilder *Enclosing = nullptr)
: NodeBuilder(SrcNode, DstSet, Ctx), EnclosingBldr(Enclosing) {
if (EnclosingBldr)
EnclosingBldr->takeNodes(SrcNode);
}

StmtNodeBuilder(ExplodedNodeSet &SrcSet, ExplodedNodeSet &DstSet,
const NodeBuilderContext &Ctx,
NodeBuilder *Enclosing = nullptr)
: NodeBuilder(SrcSet, DstSet, Ctx), EnclosingBldr(Enclosing) {
if (EnclosingBldr)
for (const auto I : SrcSet)
EnclosingBldr->takeNodes(I);
}

~StmtNodeBuilder() override;

using NodeBuilder::generateNode;
using NodeBuilder::generateSink;

ExplodedNode *generateNode(const Stmt *S,
ExplodedNode *Pred,
ProgramStateRef St,
const ProgramPointTag *tag = nullptr,
ProgramPoint::Kind K = ProgramPoint::PostStmtKind){
const ProgramPoint &L = ProgramPoint::getProgramPoint(S, K,
Pred->getLocationContext(), tag);
return NodeBuilder::generateNode(L, St, Pred);
}

ExplodedNode *generateSink(const Stmt *S,
ExplodedNode *Pred,
ProgramStateRef St,
const ProgramPointTag *tag = nullptr,
ProgramPoint::Kind K = ProgramPoint::PostStmtKind){
const ProgramPoint &L = ProgramPoint::getProgramPoint(S, K,
Pred->getLocationContext(), tag);
return NodeBuilder::generateSink(L, St, Pred);
}
};



class BranchNodeBuilder: public NodeBuilder {
const CFGBlock *DstT;
const CFGBlock *DstF;

bool InFeasibleTrue;
bool InFeasibleFalse;

void anchor() override;

public:
BranchNodeBuilder(ExplodedNode *SrcNode, ExplodedNodeSet &DstSet,
const NodeBuilderContext &C,
const CFGBlock *dstT, const CFGBlock *dstF)
: NodeBuilder(SrcNode, DstSet, C), DstT(dstT), DstF(dstF),
InFeasibleTrue(!DstT), InFeasibleFalse(!DstF) {


takeNodes(SrcNode);
}

BranchNodeBuilder(const ExplodedNodeSet &SrcSet, ExplodedNodeSet &DstSet,
const NodeBuilderContext &C,
const CFGBlock *dstT, const CFGBlock *dstF)
: NodeBuilder(SrcSet, DstSet, C), DstT(dstT), DstF(dstF),
InFeasibleTrue(!DstT), InFeasibleFalse(!DstF) {
takeNodes(SrcSet);
}

ExplodedNode *generateNode(ProgramStateRef State, bool branch,
ExplodedNode *Pred);

const CFGBlock *getTargetBlock(bool branch) const {
return branch ? DstT : DstF;
}

void markInfeasible(bool branch) {
if (branch)
InFeasibleTrue = true;
else
InFeasibleFalse = true;
}

bool isFeasible(bool branch) {
return branch ? !InFeasibleTrue : !InFeasibleFalse;
}
};

class IndirectGotoNodeBuilder {
CoreEngine& Eng;
const CFGBlock *Src;
const CFGBlock &DispatchBlock;
const Expr *E;
ExplodedNode *Pred;

public:
IndirectGotoNodeBuilder(ExplodedNode *pred, const CFGBlock *src,
const Expr *e, const CFGBlock *dispatch, CoreEngine* eng)
: Eng(*eng), Src(src), DispatchBlock(*dispatch), E(e), Pred(pred) {}

class iterator {
friend class IndirectGotoNodeBuilder;

CFGBlock::const_succ_iterator I;

iterator(CFGBlock::const_succ_iterator i) : I(i) {}

public:
iterator &operator++() { ++I; return *this; }
bool operator!=(const iterator &X) const { return I != X.I; }

const LabelDecl *getLabel() const {
return cast<LabelStmt>((*I)->getLabel())->getDecl();
}

const CFGBlock *getBlock() const {
return *I;
}
};

iterator begin() { return iterator(DispatchBlock.succ_begin()); }
iterator end() { return iterator(DispatchBlock.succ_end()); }

ExplodedNode *generateNode(const iterator &I,
ProgramStateRef State,
bool isSink = false);

const Expr *getTarget() const { return E; }

ProgramStateRef getState() const { return Pred->State; }

const LocationContext *getLocationContext() const {
return Pred->getLocationContext();
}
};

class SwitchNodeBuilder {
CoreEngine& Eng;
const CFGBlock *Src;
const Expr *Condition;
ExplodedNode *Pred;

public:
SwitchNodeBuilder(ExplodedNode *pred, const CFGBlock *src,
const Expr *condition, CoreEngine* eng)
: Eng(*eng), Src(src), Condition(condition), Pred(pred) {}

class iterator {
friend class SwitchNodeBuilder;

CFGBlock::const_succ_reverse_iterator I;

iterator(CFGBlock::const_succ_reverse_iterator i) : I(i) {}

public:
iterator &operator++() { ++I; return *this; }
bool operator!=(const iterator &X) const { return I != X.I; }
bool operator==(const iterator &X) const { return I == X.I; }

const CaseStmt *getCase() const {
return cast<CaseStmt>((*I)->getLabel());
}

const CFGBlock *getBlock() const {
return *I;
}
};

iterator begin() { return iterator(Src->succ_rbegin()+1); }
iterator end() { return iterator(Src->succ_rend()); }

const SwitchStmt *getSwitch() const {
return cast<SwitchStmt>(Src->getTerminator());
}

ExplodedNode *generateCaseStmtNode(const iterator &I,
ProgramStateRef State);

ExplodedNode *generateDefaultCaseNode(ProgramStateRef State,
bool isSink = false);

const Expr *getCondition() const { return Condition; }

ProgramStateRef getState() const { return Pred->State; }

const LocationContext *getLocationContext() const {
return Pred->getLocationContext();
}
};

}

}

#endif
