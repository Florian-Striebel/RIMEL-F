












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_BUGREPORTERVISITORS_H)
#define LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_BUGREPORTERVISITORS_H

#include "clang/Analysis/ProgramPoint.h"
#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/RangedConstraintManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include <list>
#include <memory>
#include <utility>

namespace clang {

class BinaryOperator;
class CFGBlock;
class DeclRefExpr;
class Expr;
class Stmt;

namespace ento {

class PathSensitiveBugReport;
class BugReporterContext;
class ExplodedNode;
class MemRegion;
class PathDiagnosticPiece;
using PathDiagnosticPieceRef = std::shared_ptr<PathDiagnosticPiece>;


class BugReporterVisitor : public llvm::FoldingSetNode {
public:
BugReporterVisitor() = default;
BugReporterVisitor(const BugReporterVisitor &) = default;
BugReporterVisitor(BugReporterVisitor &&) {}
virtual ~BugReporterVisitor();











virtual PathDiagnosticPieceRef VisitNode(const ExplodedNode *Succ,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) = 0;



virtual void finalizeVisitor(BugReporterContext &BRC,
const ExplodedNode *EndPathNode,
PathSensitiveBugReport &BR);






virtual PathDiagnosticPieceRef getEndPath(BugReporterContext &BRC,
const ExplodedNode *N,
PathSensitiveBugReport &BR);

virtual void Profile(llvm::FoldingSetNodeID &ID) const = 0;


static PathDiagnosticPieceRef
getDefaultEndPath(const BugReporterContext &BRC, const ExplodedNode *N,
const PathSensitiveBugReport &BR);
};

namespace bugreporter {


enum class TrackingKind {


Thorough,




Condition
};


struct TrackingOptions {

TrackingKind Kind = TrackingKind::Thorough;


bool EnableNullFPSuppression = true;
};






struct StoreInfo {
enum Kind {


Initialization,



Assignment,


CallArgument,




BlockCapture
};


Kind StoreKind;

const ExplodedNode *StoreSite;


const Expr *SourceOfTheValue;

SVal Value;




const MemRegion *Dest, *Origin;
};

class Tracker;
using TrackerRef = llvm::IntrusiveRefCntPtr<Tracker>;

class ExpressionHandler;
class StoreHandler;






class Tracker : public llvm::RefCountedBase<Tracker> {
private:
using ExpressionHandlerPtr = std::unique_ptr<ExpressionHandler>;
using StoreHandlerPtr = std::unique_ptr<StoreHandler>;

PathSensitiveBugReport &Report;
std::list<ExpressionHandlerPtr> ExpressionHandlers;
std::list<StoreHandlerPtr> StoreHandlers;

protected:

Tracker(PathSensitiveBugReport &Report);

public:
virtual ~Tracker() = default;

static TrackerRef create(PathSensitiveBugReport &Report) {
return new Tracker(Report);
}

PathSensitiveBugReport &getReport() { return Report; }



struct Result {

bool FoundSomethingToTrack = false;


bool WasInterrupted = false;


void combineWith(const Result &Other) {


FoundSomethingToTrack |= Other.FoundSomethingToTrack;

WasInterrupted |= Other.WasInterrupted;
}
};






virtual Result track(const Expr *E, const ExplodedNode *N,
TrackingOptions Opts = {});














virtual Result track(SVal V, const MemRegion *R, TrackingOptions Opts = {},
const StackFrameContext *Origin = nullptr);







virtual PathDiagnosticPieceRef handle(StoreInfo SI, BugReporterContext &BRC,
TrackingOptions Opts);





void addHighPriorityHandler(ExpressionHandlerPtr SH) {
ExpressionHandlers.push_front(std::move(SH));
}





void addLowPriorityHandler(ExpressionHandlerPtr SH) {
ExpressionHandlers.push_back(std::move(SH));
}





void addHighPriorityHandler(StoreHandlerPtr SH) {
StoreHandlers.push_front(std::move(SH));
}





void addLowPriorityHandler(StoreHandlerPtr SH) {
StoreHandlers.push_back(std::move(SH));
}




template <class HandlerType, class... Args>
void addHighPriorityHandler(Args &&... ConstructorArgs) {
addHighPriorityHandler(std::make_unique<HandlerType>(
*this, std::forward<Args>(ConstructorArgs)...));
}




template <class HandlerType, class... Args>
void addLowPriorityHandler(Args &&... ConstructorArgs) {
addLowPriorityHandler(std::make_unique<HandlerType>(
*this, std::forward<Args>(ConstructorArgs)...));
}
};


class ExpressionHandler {
private:
Tracker &ParentTracker;

public:
ExpressionHandler(Tracker &ParentTracker) : ParentTracker(ParentTracker) {}
virtual ~ExpressionHandler() {}







virtual Tracker::Result handle(const Expr *E, const ExplodedNode *Original,
const ExplodedNode *ExprNode,
TrackingOptions Opts) = 0;


Tracker &getParentTracker() { return ParentTracker; }
};


class StoreHandler {
private:
Tracker &ParentTracker;

public:
StoreHandler(Tracker &ParentTracker) : ParentTracker(ParentTracker) {}
virtual ~StoreHandler() {}








virtual PathDiagnosticPieceRef handle(StoreInfo SI, BugReporterContext &BRC,
TrackingOptions Opts) = 0;

Tracker &getParentTracker() { return ParentTracker; }

protected:
PathDiagnosticPieceRef constructNote(StoreInfo SI, BugReporterContext &BRC,
StringRef NodeText);
};


class TrackingBugReporterVisitor : public BugReporterVisitor {
private:
TrackerRef ParentTracker;

public:
TrackingBugReporterVisitor(TrackerRef ParentTracker)
: ParentTracker(ParentTracker) {}

Tracker &getParentTracker() { return *ParentTracker; }
};












bool trackExpressionValue(const ExplodedNode *N, const Expr *E,
PathSensitiveBugReport &R, TrackingOptions Opts = {});














void trackStoredValue(KnownSVal V, const MemRegion *R,
PathSensitiveBugReport &Report, TrackingOptions Opts = {},
const StackFrameContext *Origin = nullptr);

const Expr *getDerefExpr(const Stmt *S);

}

class TrackConstraintBRVisitor final : public BugReporterVisitor {
DefinedSVal Constraint;
bool Assumption;
bool IsSatisfied = false;
bool IsZeroCheck;



bool IsTrackingTurnedOn = false;

public:
TrackConstraintBRVisitor(DefinedSVal constraint, bool assumption)
: Constraint(constraint), Assumption(assumption),
IsZeroCheck(!Assumption && Constraint.getAs<Loc>()) {}

void Profile(llvm::FoldingSetNodeID &ID) const override;



static const char *getTag();

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;

private:

bool isUnderconstrained(const ExplodedNode *N) const;
};



class NilReceiverBRVisitor final : public BugReporterVisitor {
public:
void Profile(llvm::FoldingSetNodeID &ID) const override {
static int x = 0;
ID.AddPointer(&x);
}

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;



static const Expr *getNilReceiver(const Stmt *S, const ExplodedNode *N);
};


class ConditionBRVisitor final : public BugReporterVisitor {

constexpr static llvm::StringLiteral GenericTrueMessage =
"Assuming the condition is true";
constexpr static llvm::StringLiteral GenericFalseMessage =
"Assuming the condition is false";

public:
void Profile(llvm::FoldingSetNodeID &ID) const override {
static int x = 0;
ID.AddPointer(&x);
}



static const char *getTag();

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;

PathDiagnosticPieceRef VisitNodeImpl(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR);

PathDiagnosticPieceRef
VisitTerminator(const Stmt *Term, const ExplodedNode *N,
const CFGBlock *SrcBlk, const CFGBlock *DstBlk,
PathSensitiveBugReport &R, BugReporterContext &BRC);

PathDiagnosticPieceRef VisitTrueTest(const Expr *Cond,
BugReporterContext &BRC,
PathSensitiveBugReport &R,
const ExplodedNode *N, bool TookTrue);

PathDiagnosticPieceRef VisitTrueTest(const Expr *Cond, const DeclRefExpr *DR,
BugReporterContext &BRC,
PathSensitiveBugReport &R,
const ExplodedNode *N, bool TookTrue,
bool IsAssuming);

PathDiagnosticPieceRef
VisitTrueTest(const Expr *Cond, const BinaryOperator *BExpr,
BugReporterContext &BRC, PathSensitiveBugReport &R,
const ExplodedNode *N, bool TookTrue, bool IsAssuming);

PathDiagnosticPieceRef VisitTrueTest(const Expr *Cond, const MemberExpr *ME,
BugReporterContext &BRC,
PathSensitiveBugReport &R,
const ExplodedNode *N, bool TookTrue,
bool IsAssuming);

PathDiagnosticPieceRef
VisitConditionVariable(StringRef LhsString, const Expr *CondVarExpr,
BugReporterContext &BRC, PathSensitiveBugReport &R,
const ExplodedNode *N, bool TookTrue);










bool printValue(const Expr *CondVarExpr, raw_ostream &Out,
const ExplodedNode *N, bool TookTrue, bool IsAssuming);

bool patternMatch(const Expr *Ex,
const Expr *ParentEx,
raw_ostream &Out,
BugReporterContext &BRC,
PathSensitiveBugReport &R,
const ExplodedNode *N,
Optional<bool> &prunable,
bool IsSameFieldName);

static bool isPieceMessageGeneric(const PathDiagnosticPiece *Piece);
};




class LikelyFalsePositiveSuppressionBRVisitor final
: public BugReporterVisitor {
public:
static void *getTag() {
static int Tag = 0;
return static_cast<void *>(&Tag);
}

void Profile(llvm::FoldingSetNodeID &ID) const override {
ID.AddPointer(getTag());
}

PathDiagnosticPieceRef VisitNode(const ExplodedNode *, BugReporterContext &,
PathSensitiveBugReport &) override {
return nullptr;
}

void finalizeVisitor(BugReporterContext &BRC, const ExplodedNode *N,
PathSensitiveBugReport &BR) override;
};






class UndefOrNullArgVisitor final : public BugReporterVisitor {

const MemRegion *R;

public:
UndefOrNullArgVisitor(const MemRegion *InR) : R(InR) {}

void Profile(llvm::FoldingSetNodeID &ID) const override {
static int Tag = 0;
ID.AddPointer(&Tag);
ID.AddPointer(R);
}

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;
};

class SuppressInlineDefensiveChecksVisitor final : public BugReporterVisitor {


DefinedSVal V;


bool IsSatisfied = false;






bool IsTrackingTurnedOn = false;

public:
SuppressInlineDefensiveChecksVisitor(DefinedSVal Val, const ExplodedNode *N);

void Profile(llvm::FoldingSetNodeID &ID) const override;



static const char *getTag();

PathDiagnosticPieceRef VisitNode(const ExplodedNode *Succ,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;
};




class FalsePositiveRefutationBRVisitor final : public BugReporterVisitor {
private:

ConstraintMap Constraints;

public:
FalsePositiveRefutationBRVisitor();

void Profile(llvm::FoldingSetNodeID &ID) const override;

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &BR) override;

void finalizeVisitor(BugReporterContext &BRC, const ExplodedNode *EndPathNode,
PathSensitiveBugReport &BR) override;
void addConstraints(const ExplodedNode *N,
bool OverwriteConstraintsOnExistingSyms);
};


class TagVisitor : public BugReporterVisitor {
public:
void Profile(llvm::FoldingSetNodeID &ID) const override;

PathDiagnosticPieceRef VisitNode(const ExplodedNode *N,
BugReporterContext &BRC,
PathSensitiveBugReport &R) override;
};

}

}

#endif
