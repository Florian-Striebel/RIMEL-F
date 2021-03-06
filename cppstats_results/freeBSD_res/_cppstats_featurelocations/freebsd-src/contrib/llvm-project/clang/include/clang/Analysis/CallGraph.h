















#if !defined(LLVM_CLANG_ANALYSIS_CALLGRAPH_H)
#define LLVM_CLANG_ANALYSIS_CALLGRAPH_H

#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include <memory>

namespace clang {

class CallGraphNode;
class Decl;
class DeclContext;
class Stmt;






class CallGraph : public RecursiveASTVisitor<CallGraph> {
friend class CallGraphNode;

using FunctionMapTy =
llvm::DenseMap<const Decl *, std::unique_ptr<CallGraphNode>>;


FunctionMapTy FunctionMap;


CallGraphNode *Root;

public:
CallGraph();
~CallGraph();





void addToCallGraph(Decl *D) {
TraverseDecl(D);
}


static bool includeInGraph(const Decl *D);




static bool includeCalleeInGraph(const Decl *D);


CallGraphNode *getNode(const Decl *) const;



CallGraphNode *getOrInsertNode(Decl *);

using iterator = FunctionMapTy::iterator;
using const_iterator = FunctionMapTy::const_iterator;



iterator begin() { return FunctionMap.begin(); }
iterator end() { return FunctionMap.end(); }
const_iterator begin() const { return FunctionMap.begin(); }
const_iterator end() const { return FunctionMap.end(); }


unsigned size() const { return FunctionMap.size(); }



CallGraphNode *getRoot() const { return Root; }




using nodes_iterator = llvm::SetVector<CallGraphNode *>::iterator;
using const_nodes_iterator = llvm::SetVector<CallGraphNode *>::const_iterator;

void print(raw_ostream &os) const;
void dump() const;
void viewGraph() const;

void addNodesForBlocks(DeclContext *D);



bool VisitFunctionDecl(FunctionDecl *FD) {


if (includeInGraph(FD) && FD->isThisDeclarationADefinition()) {

addNodesForBlocks(FD);



addNodeForDecl(FD, FD->isGlobal());
}
return true;
}


bool VisitObjCMethodDecl(ObjCMethodDecl *MD) {
if (includeInGraph(MD)) {
addNodesForBlocks(MD);
addNodeForDecl(MD, true);
}
return true;
}


bool TraverseStmt(Stmt *S) { return true; }

bool shouldWalkTypesOfTypeLocs() const { return false; }
bool shouldVisitTemplateInstantiations() const { return true; }
bool shouldVisitImplicitCode() const { return true; }

private:

void addNodeForDecl(Decl *D, bool IsGlobal);
};

class CallGraphNode {
public:
struct CallRecord {
CallGraphNode *Callee;
Expr *CallExpr;

CallRecord() = default;

CallRecord(CallGraphNode *Callee_, Expr *CallExpr_)
: Callee(Callee_), CallExpr(CallExpr_) {}



operator CallGraphNode *() const { return Callee; }
};

private:

Decl *FD;


SmallVector<CallRecord, 5> CalledFunctions;

public:
CallGraphNode(Decl *D) : FD(D) {}

using iterator = SmallVectorImpl<CallRecord>::iterator;
using const_iterator = SmallVectorImpl<CallRecord>::const_iterator;


iterator begin() { return CalledFunctions.begin(); }
iterator end() { return CalledFunctions.end(); }
const_iterator begin() const { return CalledFunctions.begin(); }
const_iterator end() const { return CalledFunctions.end(); }


llvm::iterator_range<iterator> callees() {
return llvm::make_range(begin(), end());
}
llvm::iterator_range<const_iterator> callees() const {
return llvm::make_range(begin(), end());
}

bool empty() const { return CalledFunctions.empty(); }
unsigned size() const { return CalledFunctions.size(); }

void addCallee(CallRecord Call) { CalledFunctions.push_back(Call); }

Decl *getDecl() const { return FD; }

FunctionDecl *getDefinition() const {
return getDecl()->getAsFunction()->getDefinition();
}

void print(raw_ostream &os) const;
void dump() const;
};



inline bool operator==(const CallGraphNode::CallRecord &LHS,
const CallGraphNode::CallRecord &RHS) {
return LHS.Callee == RHS.Callee;
}

}

namespace llvm {


template <> struct DenseMapInfo<clang::CallGraphNode::CallRecord> {
static inline clang::CallGraphNode::CallRecord getEmptyKey() {
return clang::CallGraphNode::CallRecord(
DenseMapInfo<clang::CallGraphNode *>::getEmptyKey(),
DenseMapInfo<clang::Expr *>::getEmptyKey());
}

static inline clang::CallGraphNode::CallRecord getTombstoneKey() {
return clang::CallGraphNode::CallRecord(
DenseMapInfo<clang::CallGraphNode *>::getTombstoneKey(),
DenseMapInfo<clang::Expr *>::getTombstoneKey());
}

static unsigned getHashValue(const clang::CallGraphNode::CallRecord &Val) {


return DenseMapInfo<clang::CallGraphNode *>::getHashValue(Val.Callee);
}

static bool isEqual(const clang::CallGraphNode::CallRecord &LHS,
const clang::CallGraphNode::CallRecord &RHS) {
return LHS == RHS;
}
};


template <> struct GraphTraits<clang::CallGraphNode*> {
using NodeType = clang::CallGraphNode;
using NodeRef = clang::CallGraphNode *;
using ChildIteratorType = NodeType::iterator;

static NodeType *getEntryNode(clang::CallGraphNode *CGN) { return CGN; }
static ChildIteratorType child_begin(NodeType *N) { return N->begin(); }
static ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <> struct GraphTraits<const clang::CallGraphNode*> {
using NodeType = const clang::CallGraphNode;
using NodeRef = const clang::CallGraphNode *;
using ChildIteratorType = NodeType::const_iterator;

static NodeType *getEntryNode(const clang::CallGraphNode *CGN) { return CGN; }
static ChildIteratorType child_begin(NodeType *N) { return N->begin();}
static ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <> struct GraphTraits<clang::CallGraph*>
: public GraphTraits<clang::CallGraphNode*> {
static NodeType *getEntryNode(clang::CallGraph *CGN) {
return CGN->getRoot();
}

static clang::CallGraphNode *
CGGetValue(clang::CallGraph::const_iterator::value_type &P) {
return P.second.get();
}


using nodes_iterator =
mapped_iterator<clang::CallGraph::iterator, decltype(&CGGetValue)>;

static nodes_iterator nodes_begin(clang::CallGraph *CG) {
return nodes_iterator(CG->begin(), &CGGetValue);
}

static nodes_iterator nodes_end (clang::CallGraph *CG) {
return nodes_iterator(CG->end(), &CGGetValue);
}

static unsigned size(clang::CallGraph *CG) { return CG->size(); }
};

template <> struct GraphTraits<const clang::CallGraph*> :
public GraphTraits<const clang::CallGraphNode*> {
static NodeType *getEntryNode(const clang::CallGraph *CGN) {
return CGN->getRoot();
}

static clang::CallGraphNode *
CGGetValue(clang::CallGraph::const_iterator::value_type &P) {
return P.second.get();
}


using nodes_iterator =
mapped_iterator<clang::CallGraph::const_iterator, decltype(&CGGetValue)>;

static nodes_iterator nodes_begin(const clang::CallGraph *CG) {
return nodes_iterator(CG->begin(), &CGGetValue);
}

static nodes_iterator nodes_end(const clang::CallGraph *CG) {
return nodes_iterator(CG->end(), &CGGetValue);
}

static unsigned size(const clang::CallGraph *CG) { return CG->size(); }
};

}

#endif
