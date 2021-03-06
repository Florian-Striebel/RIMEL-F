



















#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETYCOMMON_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETYCOMMON_H

#include "clang/AST/Decl.h"
#include "clang/Analysis/Analyses/PostOrderCFGView.h"
#include "clang/Analysis/Analyses/ThreadSafetyTIL.h"
#include "clang/Analysis/Analyses/ThreadSafetyTraverse.h"
#include "clang/Analysis/Analyses/ThreadSafetyUtil.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/CFG.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace clang {

class AbstractConditionalOperator;
class ArraySubscriptExpr;
class BinaryOperator;
class CallExpr;
class CastExpr;
class CXXDestructorDecl;
class CXXMemberCallExpr;
class CXXOperatorCallExpr;
class CXXThisExpr;
class DeclRefExpr;
class DeclStmt;
class Expr;
class MemberExpr;
class Stmt;
class UnaryOperator;

namespace threadSafety {


namespace sx {

inline bool equals(const til::SExpr *E1, const til::SExpr *E2) {
return til::EqualsComparator::compareExprs(E1, E2);
}

inline bool matches(const til::SExpr *E1, const til::SExpr *E2) {



if (isa<til::Wildcard>(E1))
return isa<til::Wildcard>(E2);
if (isa<til::Wildcard>(E2))
return isa<til::Wildcard>(E1);

return til::MatchComparator::compareExprs(E1, E2);
}

inline bool partiallyMatches(const til::SExpr *E1, const til::SExpr *E2) {
const auto *PE1 = dyn_cast_or_null<til::Project>(E1);
if (!PE1)
return false;
const auto *PE2 = dyn_cast_or_null<til::Project>(E2);
if (!PE2)
return false;
return PE1->clangDecl() == PE2->clangDecl();
}

inline std::string toString(const til::SExpr *E) {
std::stringstream ss;
til::StdPrinter::print(E, ss);
return ss.str();
}

}




class CFGVisitor {

void enterCFG(CFG *Cfg, const NamedDecl *D, const CFGBlock *First) {}


void enterCFGBlock(const CFGBlock *B) {}


bool visitPredecessors() { return true; }


void handlePredecessor(const CFGBlock *Pred) {}


void handlePredecessorBackEdge(const CFGBlock *Pred) {}


void enterCFGBlockBody(const CFGBlock *B) {}


void handleStatement(const Stmt *S) {}


void handleDestructorCall(const VarDecl *VD, const CXXDestructorDecl *DD) {}


void exitCFGBlockBody(const CFGBlock *B) {}


bool visitSuccessors() { return true; }


void handleSuccessor(const CFGBlock *Succ) {}


void handleSuccessorBackEdge(const CFGBlock *Succ) {}


void exitCFGBlock(const CFGBlock *B) {}


void exitCFG(const CFGBlock *Last) {}
};


class CFGWalker {
public:
CFGWalker() = default;



bool init(AnalysisDeclContext &AC) {
ACtx = &AC;
CFGraph = AC.getCFG();
if (!CFGraph)
return false;


if (!dyn_cast_or_null<NamedDecl>(AC.getDecl()))
return false;

SortedGraph = AC.getAnalysis<PostOrderCFGView>();
if (!SortedGraph)
return false;

return true;
}


template <class Visitor>
void walk(Visitor &V) {
PostOrderCFGView::CFGBlockSet VisitedBlocks(CFGraph);

V.enterCFG(CFGraph, getDecl(), &CFGraph->getEntry());

for (const auto *CurrBlock : *SortedGraph) {
VisitedBlocks.insert(CurrBlock);

V.enterCFGBlock(CurrBlock);


if (V.visitPredecessors()) {
SmallVector<CFGBlock*, 4> BackEdges;

for (CFGBlock::const_pred_iterator SI = CurrBlock->pred_begin(),
SE = CurrBlock->pred_end();
SI != SE; ++SI) {
if (*SI == nullptr)
continue;

if (!VisitedBlocks.alreadySet(*SI)) {
BackEdges.push_back(*SI);
continue;
}
V.handlePredecessor(*SI);
}

for (auto *Blk : BackEdges)
V.handlePredecessorBackEdge(Blk);
}

V.enterCFGBlockBody(CurrBlock);


for (const auto &BI : *CurrBlock) {
switch (BI.getKind()) {
case CFGElement::Statement:
V.handleStatement(BI.castAs<CFGStmt>().getStmt());
break;

case CFGElement::AutomaticObjectDtor: {
CFGAutomaticObjDtor AD = BI.castAs<CFGAutomaticObjDtor>();
auto *DD = const_cast<CXXDestructorDecl *>(
AD.getDestructorDecl(ACtx->getASTContext()));
auto *VD = const_cast<VarDecl *>(AD.getVarDecl());
V.handleDestructorCall(VD, DD);
break;
}
default:
break;
}
}

V.exitCFGBlockBody(CurrBlock);


if (V.visitSuccessors()) {
SmallVector<CFGBlock*, 8> ForwardEdges;


for (CFGBlock::const_succ_iterator SI = CurrBlock->succ_begin(),
SE = CurrBlock->succ_end();
SI != SE; ++SI) {
if (*SI == nullptr)
continue;

if (!VisitedBlocks.alreadySet(*SI)) {
ForwardEdges.push_back(*SI);
continue;
}
V.handleSuccessorBackEdge(*SI);
}

for (auto *Blk : ForwardEdges)
V.handleSuccessor(Blk);
}

V.exitCFGBlock(CurrBlock);
}
V.exitCFG(&CFGraph->getExit());
}

const CFG *getGraph() const { return CFGraph; }
CFG *getGraph() { return CFGraph; }

const NamedDecl *getDecl() const {
return dyn_cast<NamedDecl>(ACtx->getDecl());
}

const PostOrderCFGView *getSortedGraph() const { return SortedGraph; }

private:
CFG *CFGraph = nullptr;
AnalysisDeclContext *ACtx = nullptr;
PostOrderCFGView *SortedGraph = nullptr;
};




class CapabilityExpr {
private:

const til::SExpr* CapExpr;


bool Negated;

public:
CapabilityExpr(const til::SExpr *E, bool Neg) : CapExpr(E), Negated(Neg) {}

const til::SExpr* sexpr() const { return CapExpr; }
bool negative() const { return Negated; }

CapabilityExpr operator!() const {
return CapabilityExpr(CapExpr, !Negated);
}

bool equals(const CapabilityExpr &other) const {
return (Negated == other.Negated) && sx::equals(CapExpr, other.CapExpr);
}

bool matches(const CapabilityExpr &other) const {
return (Negated == other.Negated) && sx::matches(CapExpr, other.CapExpr);
}

bool matchesUniv(const CapabilityExpr &CapE) const {
return isUniversal() || matches(CapE);
}

bool partiallyMatches(const CapabilityExpr &other) const {
return (Negated == other.Negated) &&
sx::partiallyMatches(CapExpr, other.CapExpr);
}

const ValueDecl* valueDecl() const {
if (Negated || CapExpr == nullptr)
return nullptr;
if (const auto *P = dyn_cast<til::Project>(CapExpr))
return P->clangDecl();
if (const auto *P = dyn_cast<til::LiteralPtr>(CapExpr))
return P->clangDecl();
return nullptr;
}

std::string toString() const {
if (Negated)
return "!" + sx::toString(CapExpr);
return sx::toString(CapExpr);
}

bool shouldIgnore() const { return CapExpr == nullptr; }

bool isInvalid() const { return sexpr() && isa<til::Undefined>(sexpr()); }

bool isUniversal() const { return sexpr() && isa<til::Wildcard>(sexpr()); }
};


class SExprBuilder {
public:









struct CallingContext {

CallingContext *Prev;


const NamedDecl *AttrDecl;


const Expr *SelfArg = nullptr;


unsigned NumArgs = 0;


const Expr *const *FunArgs = nullptr;


bool SelfArrow = false;

CallingContext(CallingContext *P, const NamedDecl *D = nullptr)
: Prev(P), AttrDecl(D) {}
};

SExprBuilder(til::MemRegionRef A) : Arena(A) {

SelfVar = new (Arena) til::Variable(nullptr);
SelfVar->setKind(til::Variable::VK_SFun);
}



CapabilityExpr translateAttrExpr(const Expr *AttrExp, const NamedDecl *D,
const Expr *DeclExp, VarDecl *SelfD=nullptr);

CapabilityExpr translateAttrExpr(const Expr *AttrExp, CallingContext *Ctx);




til::SExpr *translate(const Stmt *S, CallingContext *Ctx);
til::SCFG *buildCFG(CFGWalker &Walker);

til::SExpr *lookupStmt(const Stmt *S);

til::BasicBlock *lookupBlock(const CFGBlock *B) {
return BlockMap[B->getBlockID()];
}

const til::SCFG *getCFG() const { return Scfg; }
til::SCFG *getCFG() { return Scfg; }

private:

friend class CFGWalker;

til::SExpr *translateDeclRefExpr(const DeclRefExpr *DRE,
CallingContext *Ctx) ;
til::SExpr *translateCXXThisExpr(const CXXThisExpr *TE, CallingContext *Ctx);
til::SExpr *translateMemberExpr(const MemberExpr *ME, CallingContext *Ctx);
til::SExpr *translateObjCIVarRefExpr(const ObjCIvarRefExpr *IVRE,
CallingContext *Ctx);
til::SExpr *translateCallExpr(const CallExpr *CE, CallingContext *Ctx,
const Expr *SelfE = nullptr);
til::SExpr *translateCXXMemberCallExpr(const CXXMemberCallExpr *ME,
CallingContext *Ctx);
til::SExpr *translateCXXOperatorCallExpr(const CXXOperatorCallExpr *OCE,
CallingContext *Ctx);
til::SExpr *translateUnaryOperator(const UnaryOperator *UO,
CallingContext *Ctx);
til::SExpr *translateBinOp(til::TIL_BinaryOpcode Op,
const BinaryOperator *BO,
CallingContext *Ctx, bool Reverse = false);
til::SExpr *translateBinAssign(til::TIL_BinaryOpcode Op,
const BinaryOperator *BO,
CallingContext *Ctx, bool Assign = false);
til::SExpr *translateBinaryOperator(const BinaryOperator *BO,
CallingContext *Ctx);
til::SExpr *translateCastExpr(const CastExpr *CE, CallingContext *Ctx);
til::SExpr *translateArraySubscriptExpr(const ArraySubscriptExpr *E,
CallingContext *Ctx);
til::SExpr *translateAbstractConditionalOperator(
const AbstractConditionalOperator *C, CallingContext *Ctx);

til::SExpr *translateDeclStmt(const DeclStmt *S, CallingContext *Ctx);


using StatementMap = llvm::DenseMap<const Stmt *, til::SExpr *>;


using LVarIndexMap = llvm::DenseMap<const ValueDecl *, unsigned>;


using NameVarPair = std::pair<const ValueDecl *, til::SExpr *>;
using LVarDefinitionMap = CopyOnWriteVector<NameVarPair>;

struct BlockInfo {
LVarDefinitionMap ExitMap;
bool HasBackEdges = false;


unsigned UnprocessedSuccessors = 0;


unsigned ProcessedPredecessors = 0;

BlockInfo() = default;
BlockInfo(BlockInfo &&) = default;
BlockInfo &operator=(BlockInfo &&) = default;
};

void enterCFG(CFG *Cfg, const NamedDecl *D, const CFGBlock *First);
void enterCFGBlock(const CFGBlock *B);
bool visitPredecessors() { return true; }
void handlePredecessor(const CFGBlock *Pred);
void handlePredecessorBackEdge(const CFGBlock *Pred);
void enterCFGBlockBody(const CFGBlock *B);
void handleStatement(const Stmt *S);
void handleDestructorCall(const VarDecl *VD, const CXXDestructorDecl *DD);
void exitCFGBlockBody(const CFGBlock *B);
bool visitSuccessors() { return true; }
void handleSuccessor(const CFGBlock *Succ);
void handleSuccessorBackEdge(const CFGBlock *Succ);
void exitCFGBlock(const CFGBlock *B);
void exitCFG(const CFGBlock *Last);

void insertStmt(const Stmt *S, til::SExpr *E) {
SMap.insert(std::make_pair(S, E));
}

til::SExpr *getCurrentLVarDefinition(const ValueDecl *VD);

til::SExpr *addStatement(til::SExpr *E, const Stmt *S,
const ValueDecl *VD = nullptr);
til::SExpr *lookupVarDecl(const ValueDecl *VD);
til::SExpr *addVarDecl(const ValueDecl *VD, til::SExpr *E);
til::SExpr *updateVarDecl(const ValueDecl *VD, til::SExpr *E);

void makePhiNodeVar(unsigned i, unsigned NPreds, til::SExpr *E);
void mergeEntryMap(LVarDefinitionMap Map);
void mergeEntryMapBackEdge();
void mergePhiNodesBackEdge(const CFGBlock *Blk);

private:


static const bool CapabilityExprMode = true;

til::MemRegionRef Arena;


til::Variable *SelfVar = nullptr;

til::SCFG *Scfg = nullptr;


StatementMap SMap;


LVarIndexMap LVarIdxMap;


std::vector<til::BasicBlock *> BlockMap;


std::vector<BlockInfo> BBInfo;

LVarDefinitionMap CurrentLVarMap;
std::vector<til::Phi *> CurrentArguments;
std::vector<til::SExpr *> CurrentInstructions;
std::vector<til::Phi *> IncompleteArgs;
til::BasicBlock *CurrentBB = nullptr;
BlockInfo *CurrentBlockInfo = nullptr;
};


void printSCFG(CFGWalker &Walker);

}
}

#endif
