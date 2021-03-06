












#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_CONSUMED_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_CONSUMED_H

#include "clang/Analysis/Analyses/PostOrderCFGView.h"
#include "clang/Analysis/CFG.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include <list>
#include <memory>
#include <utility>
#include <vector>

namespace clang {

class AnalysisDeclContext;
class CXXBindTemporaryExpr;
class FunctionDecl;
class PostOrderCFGView;
class Stmt;
class VarDecl;

namespace consumed {

class ConsumedStmtVisitor;

enum ConsumedState {

CS_None,

CS_Unknown,
CS_Unconsumed,
CS_Consumed
};

using OptionalNotes = SmallVector<PartialDiagnosticAt, 1>;
using DelayedDiag = std::pair<PartialDiagnosticAt, OptionalNotes>;
using DiagList = std::list<DelayedDiag>;

class ConsumedWarningsHandlerBase {
public:
virtual ~ConsumedWarningsHandlerBase();


virtual void emitDiagnostics() {}








virtual void warnLoopStateMismatch(SourceLocation Loc,
StringRef VariableName) {}










virtual void warnParamReturnTypestateMismatch(SourceLocation Loc,
StringRef VariableName,
StringRef ExpectedState,
StringRef ObservedState) {}


virtual void warnParamTypestateMismatch(SourceLocation LOC,
StringRef ExpectedState,
StringRef ObservedState) {}








virtual void warnReturnTypestateForUnconsumableType(SourceLocation Loc,
StringRef TypeName) {}










virtual void warnReturnTypestateMismatch(SourceLocation Loc,
StringRef ExpectedState,
StringRef ObservedState) {}








virtual void warnUseOfTempInInvalidState(StringRef MethodName,
StringRef State,
SourceLocation Loc) {}











virtual void warnUseInInvalidState(StringRef MethodName,
StringRef VariableName,
StringRef State,
SourceLocation Loc) {}
};

class ConsumedStateMap {
using VarMapType = llvm::DenseMap<const VarDecl *, ConsumedState>;
using TmpMapType =
llvm::DenseMap<const CXXBindTemporaryExpr *, ConsumedState>;

protected:
bool Reachable = true;
const Stmt *From = nullptr;
VarMapType VarMap;
TmpMapType TmpMap;

public:
ConsumedStateMap() = default;
ConsumedStateMap(const ConsumedStateMap &Other)
: Reachable(Other.Reachable), From(Other.From), VarMap(Other.VarMap),
TmpMap() {}



void checkParamsForReturnTypestate(SourceLocation BlameLoc,
ConsumedWarningsHandlerBase &WarningsHandler) const;


void clearTemporaries();


ConsumedState getState(const VarDecl *Var) const;


ConsumedState getState(const CXXBindTemporaryExpr *Tmp) const;


void intersect(const ConsumedStateMap &Other);

void intersectAtLoopHead(const CFGBlock *LoopHead, const CFGBlock *LoopBack,
const ConsumedStateMap *LoopBackStates,
ConsumedWarningsHandlerBase &WarningsHandler);


bool isReachable() const { return Reachable; }


void markUnreachable();




void setSource(const Stmt *Source) { this->From = Source; }


void setState(const VarDecl *Var, ConsumedState State);


void setState(const CXXBindTemporaryExpr *Tmp, ConsumedState State);


void remove(const CXXBindTemporaryExpr *Tmp);





bool operator!=(const ConsumedStateMap *Other) const;
};

class ConsumedBlockInfo {
std::vector<std::unique_ptr<ConsumedStateMap>> StateMapsArray;
std::vector<unsigned int> VisitOrder;

public:
ConsumedBlockInfo() = default;

ConsumedBlockInfo(unsigned int NumBlocks, PostOrderCFGView *SortedGraph)
: StateMapsArray(NumBlocks), VisitOrder(NumBlocks, 0) {
unsigned int VisitOrderCounter = 0;
for (const auto BI : *SortedGraph)
VisitOrder[BI->getBlockID()] = VisitOrderCounter++;
}

bool allBackEdgesVisited(const CFGBlock *CurrBlock,
const CFGBlock *TargetBlock);

void addInfo(const CFGBlock *Block, ConsumedStateMap *StateMap,
std::unique_ptr<ConsumedStateMap> &OwnedStateMap);
void addInfo(const CFGBlock *Block,
std::unique_ptr<ConsumedStateMap> StateMap);

ConsumedStateMap* borrowInfo(const CFGBlock *Block);

void discardInfo(const CFGBlock *Block);

std::unique_ptr<ConsumedStateMap> getInfo(const CFGBlock *Block);

bool isBackEdge(const CFGBlock *From, const CFGBlock *To);
bool isBackEdgeTarget(const CFGBlock *Block);
};


class ConsumedAnalyzer {
ConsumedBlockInfo BlockInfo;
std::unique_ptr<ConsumedStateMap> CurrStates;

ConsumedState ExpectedReturnState;

void determineExpectedReturnState(AnalysisDeclContext &AC,
const FunctionDecl *D);
bool splitState(const CFGBlock *CurrBlock,
const ConsumedStmtVisitor &Visitor);

public:
ConsumedWarningsHandlerBase &WarningsHandler;

ConsumedAnalyzer(ConsumedWarningsHandlerBase &WarningsHandler)
: WarningsHandler(WarningsHandler) {}

ConsumedState getExpectedReturnState() const { return ExpectedReturnState; }







void run(AnalysisDeclContext &AC);
};

}

}

#endif
