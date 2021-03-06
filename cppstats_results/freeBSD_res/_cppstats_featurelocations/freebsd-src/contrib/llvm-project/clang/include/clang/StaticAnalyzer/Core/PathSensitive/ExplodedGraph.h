
















#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_EXPLODEDGRAPH_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_EXPLODEDGRAPH_H

#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/ProgramPoint.h"
#include "clang/Analysis/Support/BumpVector.h"
#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace clang {

class CFG;
class Decl;
class Expr;
class ParentMap;
class Stmt;

namespace ento {

class ExplodedGraph;










class ExplodedNode : public llvm::FoldingSetNode {
friend class BranchNodeBuilder;
friend class CoreEngine;
friend class EndOfFunctionNodeBuilder;
friend class ExplodedGraph;
friend class IndirectGotoNodeBuilder;
friend class NodeBuilder;
friend class SwitchNodeBuilder;










class NodeGroup {




uintptr_t P;

public:
NodeGroup(bool Flag = false) : P(Flag) {
assert(getFlag() == Flag);
}

ExplodedNode * const *begin() const;

ExplodedNode * const *end() const;

unsigned size() const;

bool empty() const { return P == 0 || getFlag() != 0; }




void addNode(ExplodedNode *N, ExplodedGraph &G);






void replaceNode(ExplodedNode *node);


bool getFlag() const {
return (P & 1);
}
};



const ProgramPoint Location;


ProgramStateRef State;


NodeGroup Preds;


NodeGroup Succs;

int64_t Id;

public:
explicit ExplodedNode(const ProgramPoint &loc, ProgramStateRef state,
int64_t Id, bool IsSink)
: Location(loc), State(std::move(state)), Succs(IsSink), Id(Id) {
assert(isSink() == IsSink);
}


ProgramPoint getLocation() const { return Location; }

const LocationContext *getLocationContext() const {
return getLocation().getLocationContext();
}

const StackFrameContext *getStackFrame() const {
return getLocation().getStackFrame();
}

const Decl &getCodeDecl() const { return *getLocationContext()->getDecl(); }

CFG &getCFG() const { return *getLocationContext()->getCFG(); }

const CFGBlock *getCFGBlock() const;

const ParentMap &getParentMap() const {
return getLocationContext()->getParentMap();
}

template <typename T>
T &getAnalysis() const {
return *getLocationContext()->getAnalysis<T>();
}

const ProgramStateRef &getState() const { return State; }

template <typename T>
Optional<T> getLocationAs() const LLVM_LVALUE_FUNCTION {
return Location.getAs<T>();
}


SVal getSVal(const Stmt *S) const {
return getState()->getSVal(S, getLocationContext());
}

static void Profile(llvm::FoldingSetNodeID &ID,
const ProgramPoint &Loc,
const ProgramStateRef &state,
bool IsSink) {
ID.Add(Loc);
ID.AddPointer(state.get());
ID.AddBoolean(IsSink);
}

void Profile(llvm::FoldingSetNodeID& ID) const {

Profile(ID, Location, State, isSink());
}



void addPredecessor(ExplodedNode *V, ExplodedGraph &G);

unsigned succ_size() const { return Succs.size(); }
unsigned pred_size() const { return Preds.size(); }
bool succ_empty() const { return Succs.empty(); }
bool pred_empty() const { return Preds.empty(); }

bool isSink() const { return Succs.getFlag(); }

bool hasSinglePred() const {
return (pred_size() == 1);
}

ExplodedNode *getFirstPred() {
return pred_empty() ? nullptr : *(pred_begin());
}

const ExplodedNode *getFirstPred() const {
return const_cast<ExplodedNode*>(this)->getFirstPred();
}

ExplodedNode *getFirstSucc() {
return succ_empty() ? nullptr : *(succ_begin());
}

const ExplodedNode *getFirstSucc() const {
return const_cast<ExplodedNode*>(this)->getFirstSucc();
}


using succ_iterator = ExplodedNode * const *;
using succ_range = llvm::iterator_range<succ_iterator>;

using const_succ_iterator = const ExplodedNode * const *;
using const_succ_range = llvm::iterator_range<const_succ_iterator>;

using pred_iterator = ExplodedNode * const *;
using pred_range = llvm::iterator_range<pred_iterator>;

using const_pred_iterator = const ExplodedNode * const *;
using const_pred_range = llvm::iterator_range<const_pred_iterator>;

pred_iterator pred_begin() { return Preds.begin(); }
pred_iterator pred_end() { return Preds.end(); }
pred_range preds() { return {Preds.begin(), Preds.end()}; }

const_pred_iterator pred_begin() const {
return const_cast<ExplodedNode*>(this)->pred_begin();
}
const_pred_iterator pred_end() const {
return const_cast<ExplodedNode*>(this)->pred_end();
}
const_pred_range preds() const { return {Preds.begin(), Preds.end()}; }

succ_iterator succ_begin() { return Succs.begin(); }
succ_iterator succ_end() { return Succs.end(); }
succ_range succs() { return {Succs.begin(), Succs.end()}; }

const_succ_iterator succ_begin() const {
return const_cast<ExplodedNode*>(this)->succ_begin();
}
const_succ_iterator succ_end() const {
return const_cast<ExplodedNode*>(this)->succ_end();
}
const_succ_range succs() const { return {Succs.begin(), Succs.end()}; }

int64_t getID() const { return Id; }






bool isTrivial() const;





const Stmt *getStmtForDiagnostics() const;





const Stmt *getNextStmtForDiagnostics() const;





const Stmt *getPreviousStmtForDiagnostics() const;





const Stmt *getCurrentOrPreviousStmtForDiagnostics() const;

private:
void replaceSuccessor(ExplodedNode *node) { Succs.replaceNode(node); }
void replacePredecessor(ExplodedNode *node) { Preds.replaceNode(node); }
};

using InterExplodedGraphMap =
llvm::DenseMap<const ExplodedNode *, const ExplodedNode *>;

class ExplodedGraph {
protected:
friend class CoreEngine;


using NodeVector = std::vector<ExplodedNode *>;





NodeVector Roots;



NodeVector EndNodes;


llvm::FoldingSet<ExplodedNode> Nodes;



BumpVectorContext BVC;


int64_t NumNodes = 0;


NodeVector ChangedNodes;


NodeVector FreeNodes;




unsigned ReclaimNodeInterval = 0;


unsigned ReclaimCounter;

public:
ExplodedGraph();
~ExplodedGraph();





ExplodedNode *getNode(const ProgramPoint &L, ProgramStateRef State,
bool IsSink = false,
bool* IsNew = nullptr);





ExplodedNode *createUncachedNode(const ProgramPoint &L,
ProgramStateRef State,
int64_t Id,
bool IsSink = false);

std::unique_ptr<ExplodedGraph> MakeEmptyGraph() const {
return std::make_unique<ExplodedGraph>();
}


ExplodedNode *addRoot(ExplodedNode *V) {
Roots.push_back(V);
return V;
}


ExplodedNode *addEndOfPath(ExplodedNode *V) {
EndNodes.push_back(V);
return V;
}

unsigned num_roots() const { return Roots.size(); }
unsigned num_eops() const { return EndNodes.size(); }

bool empty() const { return NumNodes == 0; }
unsigned size() const { return NumNodes; }

void reserve(unsigned NodeCount) { Nodes.reserve(NodeCount); }


using NodeTy = ExplodedNode;
using AllNodesTy = llvm::FoldingSet<ExplodedNode>;
using roots_iterator = NodeVector::iterator;
using const_roots_iterator = NodeVector::const_iterator;
using eop_iterator = NodeVector::iterator;
using const_eop_iterator = NodeVector::const_iterator;
using node_iterator = AllNodesTy::iterator;
using const_node_iterator = AllNodesTy::const_iterator;

node_iterator nodes_begin() { return Nodes.begin(); }

node_iterator nodes_end() { return Nodes.end(); }

const_node_iterator nodes_begin() const { return Nodes.begin(); }

const_node_iterator nodes_end() const { return Nodes.end(); }

roots_iterator roots_begin() { return Roots.begin(); }

roots_iterator roots_end() { return Roots.end(); }

const_roots_iterator roots_begin() const { return Roots.begin(); }

const_roots_iterator roots_end() const { return Roots.end(); }

eop_iterator eop_begin() { return EndNodes.begin(); }

eop_iterator eop_end() { return EndNodes.end(); }

const_eop_iterator eop_begin() const { return EndNodes.begin(); }

const_eop_iterator eop_end() const { return EndNodes.end(); }

llvm::BumpPtrAllocator & getAllocator() { return BVC.getAllocator(); }
BumpVectorContext &getNodeAllocator() { return BVC; }

using NodeMap = llvm::DenseMap<const ExplodedNode *, ExplodedNode *>;











std::unique_ptr<ExplodedGraph>
trim(ArrayRef<const NodeTy *> Nodes,
InterExplodedGraphMap *ForwardMap = nullptr,
InterExplodedGraphMap *InverseMap = nullptr) const;



void enableNodeReclamation(unsigned Interval) {
ReclaimCounter = ReclaimNodeInterval = Interval;
}



void reclaimRecentlyAllocatedNodes();



static bool isInterestingLValueExpr(const Expr *Ex);

private:
bool shouldCollect(const ExplodedNode *node);
void collectNode(ExplodedNode *node);
};

class ExplodedNodeSet {
using ImplTy = llvm::SmallSetVector<ExplodedNode *, 4>;
ImplTy Impl;

public:
ExplodedNodeSet(ExplodedNode *N) {
assert(N && !static_cast<ExplodedNode*>(N)->isSink());
Impl.insert(N);
}

ExplodedNodeSet() = default;

void Add(ExplodedNode *N) {
if (N && !static_cast<ExplodedNode*>(N)->isSink()) Impl.insert(N);
}

using iterator = ImplTy::iterator;
using const_iterator = ImplTy::const_iterator;

unsigned size() const { return Impl.size(); }
bool empty() const { return Impl.empty(); }
bool erase(ExplodedNode *N) { return Impl.remove(N); }

void clear() { Impl.clear(); }

void insert(const ExplodedNodeSet &S) {
assert(&S != this);
if (empty())
Impl = S.Impl;
else
Impl.insert(S.begin(), S.end());
}

iterator begin() { return Impl.begin(); }
iterator end() { return Impl.end(); }

const_iterator begin() const { return Impl.begin(); }
const_iterator end() const { return Impl.end(); }
};

}

}



namespace llvm {
template <> struct GraphTraits<clang::ento::ExplodedGraph *> {
using GraphTy = clang::ento::ExplodedGraph *;
using NodeRef = clang::ento::ExplodedNode *;
using ChildIteratorType = clang::ento::ExplodedNode::succ_iterator;
using nodes_iterator = llvm::df_iterator<GraphTy>;

static NodeRef getEntryNode(const GraphTy G) {
return *G->roots_begin();
}

static bool predecessorOfTrivial(NodeRef N) {
return N->succ_size() == 1 && N->getFirstSucc()->isTrivial();
}

static ChildIteratorType child_begin(NodeRef N) {
if (predecessorOfTrivial(N))
return child_begin(*N->succ_begin());
return N->succ_begin();
}

static ChildIteratorType child_end(NodeRef N) {
if (predecessorOfTrivial(N))
return child_end(N->getFirstSucc());
return N->succ_end();
}

static nodes_iterator nodes_begin(const GraphTy G) {
return df_begin(G);
}

static nodes_iterator nodes_end(const GraphTy G) {
return df_end(G);
}
};
}

#endif
