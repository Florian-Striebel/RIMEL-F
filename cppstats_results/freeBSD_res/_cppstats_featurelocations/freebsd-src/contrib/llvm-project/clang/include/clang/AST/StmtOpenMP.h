












#if !defined(LLVM_CLANG_AST_STMTOPENMP_H)
#define LLVM_CLANG_AST_STMTOPENMP_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/AST/OpenMPClause.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/SourceLocation.h"

namespace clang {




















































































































class OMPCanonicalLoop : public Stmt {
friend class ASTStmtReader;
friend class ASTStmtWriter;


enum {
LOOP_STMT,
DISTANCE_FUNC,
LOOPVAR_FUNC,
LOOPVAR_REF,
LastSubStmt = LOOPVAR_REF
};

private:

Stmt *SubStmts[LastSubStmt + 1] = {};

OMPCanonicalLoop() : Stmt(StmtClass::OMPCanonicalLoopClass) {}

public:

static OMPCanonicalLoop *create(const ASTContext &Ctx, Stmt *LoopStmt,
CapturedStmt *DistanceFunc,
CapturedStmt *LoopVarFunc,
DeclRefExpr *LoopVarRef) {
OMPCanonicalLoop *S = new (Ctx) OMPCanonicalLoop();
S->setLoopStmt(LoopStmt);
S->setDistanceFunc(DistanceFunc);
S->setLoopVarFunc(LoopVarFunc);
S->setLoopVarRef(LoopVarRef);
return S;
}


static OMPCanonicalLoop *createEmpty(const ASTContext &Ctx) {
return new (Ctx) OMPCanonicalLoop();
}

static bool classof(const Stmt *S) {
return S->getStmtClass() == StmtClass::OMPCanonicalLoopClass;
}

SourceLocation getBeginLoc() const { return getLoopStmt()->getBeginLoc(); }
SourceLocation getEndLoc() const { return getLoopStmt()->getEndLoc(); }



child_range children() {
return child_range(&SubStmts[0], &SubStmts[0] + LastSubStmt + 1);
}
const_child_range children() const {
return const_child_range(&SubStmts[0], &SubStmts[0] + LastSubStmt + 1);
}




Stmt *getLoopStmt() { return SubStmts[LOOP_STMT]; }
const Stmt *getLoopStmt() const { return SubStmts[LOOP_STMT]; }
void setLoopStmt(Stmt *S) {
assert((isa<ForStmt>(S) || isa<CXXForRangeStmt>(S)) &&
"Canonical loop must be a for loop (range-based or otherwise)");
SubStmts[LOOP_STMT] = S;
}









CapturedStmt *getDistanceFunc() {
return cast<CapturedStmt>(SubStmts[DISTANCE_FUNC]);
}
const CapturedStmt *getDistanceFunc() const {
return cast<CapturedStmt>(SubStmts[DISTANCE_FUNC]);
}
void setDistanceFunc(CapturedStmt *S) {
assert(S && "Expected non-null captured statement");
SubStmts[DISTANCE_FUNC] = S;
}











CapturedStmt *getLoopVarFunc() {
return cast<CapturedStmt>(SubStmts[LOOPVAR_FUNC]);
}
const CapturedStmt *getLoopVarFunc() const {
return cast<CapturedStmt>(SubStmts[LOOPVAR_FUNC]);
}
void setLoopVarFunc(CapturedStmt *S) {
assert(S && "Expected non-null captured statement");
SubStmts[LOOPVAR_FUNC] = S;
}




DeclRefExpr *getLoopVarRef() {
return cast<DeclRefExpr>(SubStmts[LOOPVAR_REF]);
}
const DeclRefExpr *getLoopVarRef() const {
return cast<DeclRefExpr>(SubStmts[LOOPVAR_REF]);
}
void setLoopVarRef(DeclRefExpr *E) {
assert(E && "Expected non-null loop variable");
SubStmts[LOOPVAR_REF] = E;
}

};




class OMPExecutableDirective : public Stmt {
friend class ASTStmtReader;
friend class ASTStmtWriter;


OpenMPDirectiveKind Kind = llvm::omp::OMPD_unknown;

SourceLocation StartLoc;

SourceLocation EndLoc;


MutableArrayRef<OMPClause *> getClauses() {
if (!Data)
return llvm::None;
return Data->getClauses();
}

protected:

OMPChildren *Data = nullptr;








OMPExecutableDirective(StmtClass SC, OpenMPDirectiveKind K,
SourceLocation StartLoc, SourceLocation EndLoc)
: Stmt(SC), Kind(K), StartLoc(std::move(StartLoc)),
EndLoc(std::move(EndLoc)) {}

template <typename T, typename... Params>
static T *createDirective(const ASTContext &C, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, unsigned NumChildren,
Params &&... P) {
void *Mem =
C.Allocate(sizeof(T) + OMPChildren::size(Clauses.size(), AssociatedStmt,
NumChildren),
alignof(T));

auto *Data = OMPChildren::Create(reinterpret_cast<T *>(Mem) + 1, Clauses,
AssociatedStmt, NumChildren);
auto *Inst = new (Mem) T(std::forward<Params>(P)...);
Inst->Data = Data;
return Inst;
}

template <typename T, typename... Params>
static T *createEmptyDirective(const ASTContext &C, unsigned NumClauses,
bool HasAssociatedStmt, unsigned NumChildren,
Params &&... P) {
void *Mem =
C.Allocate(sizeof(T) + OMPChildren::size(NumClauses, HasAssociatedStmt,
NumChildren),
alignof(T));
auto *Data =
OMPChildren::CreateEmpty(reinterpret_cast<T *>(Mem) + 1, NumClauses,
HasAssociatedStmt, NumChildren);
auto *Inst = new (Mem) T(std::forward<Params>(P)...);
Inst->Data = Data;
return Inst;
}

template <typename T>
static T *createEmptyDirective(const ASTContext &C, unsigned NumClauses,
bool HasAssociatedStmt = false,
unsigned NumChildren = 0) {
void *Mem =
C.Allocate(sizeof(T) + OMPChildren::size(NumClauses, HasAssociatedStmt,
NumChildren),
alignof(T));
auto *Data =
OMPChildren::CreateEmpty(reinterpret_cast<T *>(Mem) + 1, NumClauses,
HasAssociatedStmt, NumChildren);
auto *Inst = new (Mem) T;
Inst->Data = Data;
return Inst;
}

public:

class used_clauses_child_iterator
: public llvm::iterator_adaptor_base<
used_clauses_child_iterator, ArrayRef<OMPClause *>::iterator,
std::forward_iterator_tag, Stmt *, ptrdiff_t, Stmt *, Stmt *> {
ArrayRef<OMPClause *>::iterator End;
OMPClause::child_iterator ChildI, ChildEnd;

void MoveToNext() {
if (ChildI != ChildEnd)
return;
while (this->I != End) {
++this->I;
if (this->I != End) {
ChildI = (*this->I)->used_children().begin();
ChildEnd = (*this->I)->used_children().end();
if (ChildI != ChildEnd)
return;
}
}
}

public:
explicit used_clauses_child_iterator(ArrayRef<OMPClause *> Clauses)
: used_clauses_child_iterator::iterator_adaptor_base(Clauses.begin()),
End(Clauses.end()) {
if (this->I != End) {
ChildI = (*this->I)->used_children().begin();
ChildEnd = (*this->I)->used_children().end();
MoveToNext();
}
}
Stmt *operator*() const { return *ChildI; }
Stmt *operator->() const { return **this; }

used_clauses_child_iterator &operator++() {
++ChildI;
if (ChildI != ChildEnd)
return *this;
if (this->I != End) {
++this->I;
if (this->I != End) {
ChildI = (*this->I)->used_children().begin();
ChildEnd = (*this->I)->used_children().end();
}
}
MoveToNext();
return *this;
}
};

static llvm::iterator_range<used_clauses_child_iterator>
used_clauses_children(ArrayRef<OMPClause *> Clauses) {
return {used_clauses_child_iterator(Clauses),
used_clauses_child_iterator(llvm::makeArrayRef(Clauses.end(), 0))};
}





template <typename SpecificClause>
class specific_clause_iterator
: public llvm::iterator_adaptor_base<
specific_clause_iterator<SpecificClause>,
ArrayRef<OMPClause *>::const_iterator, std::forward_iterator_tag,
const SpecificClause *, ptrdiff_t, const SpecificClause *,
const SpecificClause *> {
ArrayRef<OMPClause *>::const_iterator End;

void SkipToNextClause() {
while (this->I != End && !isa<SpecificClause>(*this->I))
++this->I;
}

public:
explicit specific_clause_iterator(ArrayRef<OMPClause *> Clauses)
: specific_clause_iterator::iterator_adaptor_base(Clauses.begin()),
End(Clauses.end()) {
SkipToNextClause();
}

const SpecificClause *operator*() const {
return cast<SpecificClause>(*this->I);
}
const SpecificClause *operator->() const { return **this; }

specific_clause_iterator &operator++() {
++this->I;
SkipToNextClause();
return *this;
}
};

template <typename SpecificClause>
static llvm::iterator_range<specific_clause_iterator<SpecificClause>>
getClausesOfKind(ArrayRef<OMPClause *> Clauses) {
return {specific_clause_iterator<SpecificClause>(Clauses),
specific_clause_iterator<SpecificClause>(
llvm::makeArrayRef(Clauses.end(), 0))};
}

template <typename SpecificClause>
llvm::iterator_range<specific_clause_iterator<SpecificClause>>
getClausesOfKind() const {
return getClausesOfKind<SpecificClause>(clauses());
}






template <typename SpecificClause>
static const SpecificClause *getSingleClause(ArrayRef<OMPClause *> Clauses) {
auto ClausesOfKind = getClausesOfKind<SpecificClause>(Clauses);

if (ClausesOfKind.begin() != ClausesOfKind.end()) {
assert(std::next(ClausesOfKind.begin()) == ClausesOfKind.end() &&
"There are at least 2 clauses of the specified kind");
return *ClausesOfKind.begin();
}
return nullptr;
}

template <typename SpecificClause>
const SpecificClause *getSingleClause() const {
return getSingleClause<SpecificClause>(clauses());
}



template <typename SpecificClause>
bool hasClausesOfKind() const {
auto Clauses = getClausesOfKind<SpecificClause>();
return Clauses.begin() != Clauses.end();
}


SourceLocation getBeginLoc() const { return StartLoc; }

SourceLocation getEndLoc() const { return EndLoc; }





void setLocStart(SourceLocation Loc) { StartLoc = Loc; }




void setLocEnd(SourceLocation Loc) { EndLoc = Loc; }


unsigned getNumClauses() const {
if (!Data)
return 0;
return Data->getNumClauses();
}





OMPClause *getClause(unsigned I) const { return clauses()[I]; }


bool hasAssociatedStmt() const { return Data && Data->hasAssociatedStmt(); }


const Stmt *getAssociatedStmt() const {
return const_cast<OMPExecutableDirective *>(this)->getAssociatedStmt();
}
Stmt *getAssociatedStmt() {
assert(hasAssociatedStmt() &&
"Expected directive with the associated statement.");
return Data->getAssociatedStmt();
}





const CapturedStmt *getCapturedStmt(OpenMPDirectiveKind RegionKind) const {
assert(hasAssociatedStmt() &&
"Expected directive with the associated statement.");
SmallVector<OpenMPDirectiveKind, 4> CaptureRegions;
getOpenMPCaptureRegions(CaptureRegions, getDirectiveKind());
return Data->getCapturedStmt(RegionKind, CaptureRegions);
}


CapturedStmt *getInnermostCapturedStmt() {
assert(hasAssociatedStmt() &&
"Expected directive with the associated statement.");
SmallVector<OpenMPDirectiveKind, 4> CaptureRegions;
getOpenMPCaptureRegions(CaptureRegions, getDirectiveKind());
return Data->getInnermostCapturedStmt(CaptureRegions);
}

const CapturedStmt *getInnermostCapturedStmt() const {
return const_cast<OMPExecutableDirective *>(this)
->getInnermostCapturedStmt();
}

OpenMPDirectiveKind getDirectiveKind() const { return Kind; }

static bool classof(const Stmt *S) {
return S->getStmtClass() >= firstOMPExecutableDirectiveConstant &&
S->getStmtClass() <= lastOMPExecutableDirectiveConstant;
}

child_range children() {
if (!Data)
return child_range(child_iterator(), child_iterator());
return Data->getAssociatedStmtAsRange();
}

const_child_range children() const {
return const_cast<OMPExecutableDirective *>(this)->children();
}

ArrayRef<OMPClause *> clauses() const {
if (!Data)
return llvm::None;
return Data->getClauses();
}





bool isStandaloneDirective() const;




const Stmt *getStructuredBlock() const {
return const_cast<OMPExecutableDirective *>(this)->getStructuredBlock();
}
Stmt *getStructuredBlock();

const Stmt *getRawStmt() const {
return const_cast<OMPExecutableDirective *>(this)->getRawStmt();
}
Stmt *getRawStmt() {
assert(hasAssociatedStmt() &&
"Expected directive with the associated statement.");
return Data->getRawStmt();
}
};










class OMPParallelDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;






OMPParallelDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPParallelDirectiveClass,
llvm::omp::OMPD_parallel, StartLoc, EndLoc) {}



explicit OMPParallelDirective()
: OMPExecutableDirective(OMPParallelDirectiveClass,
llvm::omp::OMPD_parallel, SourceLocation(),
SourceLocation()) {}


void setTaskReductionRefExpr(Expr *E) { Data->getChildren()[0] = E; }


void setHasCancel(bool Has) { HasCancel = Has; }

public:











static OMPParallelDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt, Expr *TaskRedRef,
bool HasCancel);






static OMPParallelDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[0]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPParallelDirective *>(this)->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPParallelDirectiveClass;
}
};



class OMPLoopBasedDirective : public OMPExecutableDirective {
friend class ASTStmtReader;

protected:

unsigned NumAssociatedLoops = 0;









OMPLoopBasedDirective(StmtClass SC, OpenMPDirectiveKind Kind,
SourceLocation StartLoc, SourceLocation EndLoc,
unsigned NumAssociatedLoops)
: OMPExecutableDirective(SC, Kind, StartLoc, EndLoc),
NumAssociatedLoops(NumAssociatedLoops) {}

public:


struct DistCombinedHelperExprs {


Expr *LB;


Expr *UB;


Expr *EUB;



Expr *Init;


Expr *Cond;


Expr *NLB;


Expr *NUB;


Expr *DistCond;



Expr *ParForInDistCond;
};



struct HelperExprs {

Expr *IterationVarRef;

Expr *LastIteration;

Expr *NumIterations;

Expr *CalcLastIteration;

Expr *PreCond;

Expr *Cond;

Expr *Init;

Expr *Inc;

Expr *IL;

Expr *LB;

Expr *UB;

Expr *ST;

Expr *EUB;

Expr *NLB;

Expr *NUB;


Expr *PrevLB;


Expr *PrevUB;



Expr *DistInc;





Expr *PrevEUB;

SmallVector<Expr *, 4> Counters;

SmallVector<Expr *, 4> PrivateCounters;

SmallVector<Expr *, 4> Inits;

SmallVector<Expr *, 4> Updates;

SmallVector<Expr *, 4> Finals;


SmallVector<Expr *, 4> DependentCounters;


SmallVector<Expr *, 4> DependentInits;


SmallVector<Expr *, 4> FinalsConditions;

Stmt *PreInits;


DistCombinedHelperExprs DistCombinedFields;



bool builtAll() {
return IterationVarRef != nullptr && LastIteration != nullptr &&
NumIterations != nullptr && PreCond != nullptr &&
Cond != nullptr && Init != nullptr && Inc != nullptr;
}





void clear(unsigned Size) {
IterationVarRef = nullptr;
LastIteration = nullptr;
CalcLastIteration = nullptr;
PreCond = nullptr;
Cond = nullptr;
Init = nullptr;
Inc = nullptr;
IL = nullptr;
LB = nullptr;
UB = nullptr;
ST = nullptr;
EUB = nullptr;
NLB = nullptr;
NUB = nullptr;
NumIterations = nullptr;
PrevLB = nullptr;
PrevUB = nullptr;
DistInc = nullptr;
PrevEUB = nullptr;
Counters.resize(Size);
PrivateCounters.resize(Size);
Inits.resize(Size);
Updates.resize(Size);
Finals.resize(Size);
DependentCounters.resize(Size);
DependentInits.resize(Size);
FinalsConditions.resize(Size);
for (unsigned I = 0; I < Size; ++I) {
Counters[I] = nullptr;
PrivateCounters[I] = nullptr;
Inits[I] = nullptr;
Updates[I] = nullptr;
Finals[I] = nullptr;
DependentCounters[I] = nullptr;
DependentInits[I] = nullptr;
FinalsConditions[I] = nullptr;
}
PreInits = nullptr;
DistCombinedFields.LB = nullptr;
DistCombinedFields.UB = nullptr;
DistCombinedFields.EUB = nullptr;
DistCombinedFields.Init = nullptr;
DistCombinedFields.Cond = nullptr;
DistCombinedFields.NLB = nullptr;
DistCombinedFields.NUB = nullptr;
DistCombinedFields.DistCond = nullptr;
DistCombinedFields.ParForInDistCond = nullptr;
}
};


unsigned getLoopsNumber() const { return NumAssociatedLoops; }





static Stmt *tryToFindNextInnerLoop(Stmt *CurStmt,
bool TryImperfectlyNestedLoops);
static const Stmt *tryToFindNextInnerLoop(const Stmt *CurStmt,
bool TryImperfectlyNestedLoops) {
return tryToFindNextInnerLoop(const_cast<Stmt *>(CurStmt),
TryImperfectlyNestedLoops);
}



static bool doForAllLoops(Stmt *CurStmt, bool TryImperfectlyNestedLoops,
unsigned NumLoops,
llvm::function_ref<bool(unsigned, Stmt *)> Callback,
llvm::function_ref<void(OMPLoopBasedDirective *)>
OnTransformationCallback);
static bool
doForAllLoops(const Stmt *CurStmt, bool TryImperfectlyNestedLoops,
unsigned NumLoops,
llvm::function_ref<bool(unsigned, const Stmt *)> Callback,
llvm::function_ref<void(const OMPLoopBasedDirective *)>
OnTransformationCallback) {
auto &&NewCallback = [Callback](unsigned Cnt, Stmt *CurStmt) {
return Callback(Cnt, CurStmt);
};
auto &&NewTransformCb =
[OnTransformationCallback](OMPLoopBasedDirective *A) {
OnTransformationCallback(A);
};
return doForAllLoops(const_cast<Stmt *>(CurStmt), TryImperfectlyNestedLoops,
NumLoops, NewCallback, NewTransformCb);
}



static bool
doForAllLoops(Stmt *CurStmt, bool TryImperfectlyNestedLoops,
unsigned NumLoops,
llvm::function_ref<bool(unsigned, Stmt *)> Callback) {
auto &&TransformCb = [](OMPLoopBasedDirective *) {};
return doForAllLoops(CurStmt, TryImperfectlyNestedLoops, NumLoops, Callback,
TransformCb);
}
static bool
doForAllLoops(const Stmt *CurStmt, bool TryImperfectlyNestedLoops,
unsigned NumLoops,
llvm::function_ref<bool(unsigned, const Stmt *)> Callback) {
auto &&NewCallback = [Callback](unsigned Cnt, const Stmt *CurStmt) {
return Callback(Cnt, CurStmt);
};
return doForAllLoops(const_cast<Stmt *>(CurStmt), TryImperfectlyNestedLoops,
NumLoops, NewCallback);
}



static void doForAllLoopsBodies(
Stmt *CurStmt, bool TryImperfectlyNestedLoops, unsigned NumLoops,
llvm::function_ref<void(unsigned, Stmt *, Stmt *)> Callback);
static void doForAllLoopsBodies(
const Stmt *CurStmt, bool TryImperfectlyNestedLoops, unsigned NumLoops,
llvm::function_ref<void(unsigned, const Stmt *, const Stmt *)> Callback) {
auto &&NewCallback = [Callback](unsigned Cnt, Stmt *Loop, Stmt *Body) {
Callback(Cnt, Loop, Body);
};
doForAllLoopsBodies(const_cast<Stmt *>(CurStmt), TryImperfectlyNestedLoops,
NumLoops, NewCallback);
}

static bool classof(const Stmt *T) {
if (auto *D = dyn_cast<OMPExecutableDirective>(T))
return isOpenMPLoopDirective(D->getDirectiveKind());
return false;
}
};




class OMPLoopDirective : public OMPLoopBasedDirective {
friend class ASTStmtReader;

















enum {
IterationVariableOffset = 0,
LastIterationOffset = 1,
CalcLastIterationOffset = 2,
PreConditionOffset = 3,
CondOffset = 4,
InitOffset = 5,
IncOffset = 6,
PreInitsOffset = 7,




DefaultEnd = 8,

IsLastIterVariableOffset = 8,
LowerBoundVariableOffset = 9,
UpperBoundVariableOffset = 10,
StrideVariableOffset = 11,
EnsureUpperBoundOffset = 12,
NextLowerBoundOffset = 13,
NextUpperBoundOffset = 14,
NumIterationsOffset = 15,

WorksharingEnd = 16,
PrevLowerBoundVariableOffset = 16,
PrevUpperBoundVariableOffset = 17,
DistIncOffset = 18,
PrevEnsureUpperBoundOffset = 19,
CombinedLowerBoundVariableOffset = 20,
CombinedUpperBoundVariableOffset = 21,
CombinedEnsureUpperBoundOffset = 22,
CombinedInitOffset = 23,
CombinedConditionOffset = 24,
CombinedNextLowerBoundOffset = 25,
CombinedNextUpperBoundOffset = 26,
CombinedDistConditionOffset = 27,
CombinedParForInDistConditionOffset = 28,



CombinedDistributeEnd = 29,
};


MutableArrayRef<Expr *> getCounters() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind())]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}


MutableArrayRef<Expr *> getPrivateCounters() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind()) +
getLoopsNumber()]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}


MutableArrayRef<Expr *> getInits() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind()) +
2 * getLoopsNumber()]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}


MutableArrayRef<Expr *> getUpdates() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind()) +
3 * getLoopsNumber()]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}


MutableArrayRef<Expr *> getFinals() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind()) +
4 * getLoopsNumber()]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}


MutableArrayRef<Expr *> getDependentCounters() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind()) +
5 * getLoopsNumber()]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}


MutableArrayRef<Expr *> getDependentInits() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind()) +
6 * getLoopsNumber()]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}


MutableArrayRef<Expr *> getFinalsConditions() {
auto **Storage = reinterpret_cast<Expr **>(
&Data->getChildren()[getArraysOffset(getDirectiveKind()) +
7 * getLoopsNumber()]);
return llvm::makeMutableArrayRef(Storage, getLoopsNumber());
}

protected:








OMPLoopDirective(StmtClass SC, OpenMPDirectiveKind Kind,
SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopBasedDirective(SC, Kind, StartLoc, EndLoc, CollapsedNum) {}


static unsigned getArraysOffset(OpenMPDirectiveKind Kind) {
if (isOpenMPLoopBoundSharingDirective(Kind))
return CombinedDistributeEnd;
if (isOpenMPWorksharingDirective(Kind) || isOpenMPTaskLoopDirective(Kind) ||
isOpenMPDistributeDirective(Kind))
return WorksharingEnd;
return DefaultEnd;
}


static unsigned numLoopChildren(unsigned CollapsedNum,
OpenMPDirectiveKind Kind) {
return getArraysOffset(Kind) +
8 * CollapsedNum;


}

void setIterationVariable(Expr *IV) {
Data->getChildren()[IterationVariableOffset] = IV;
}
void setLastIteration(Expr *LI) {
Data->getChildren()[LastIterationOffset] = LI;
}
void setCalcLastIteration(Expr *CLI) {
Data->getChildren()[CalcLastIterationOffset] = CLI;
}
void setPreCond(Expr *PC) { Data->getChildren()[PreConditionOffset] = PC; }
void setCond(Expr *Cond) { Data->getChildren()[CondOffset] = Cond; }
void setInit(Expr *Init) { Data->getChildren()[InitOffset] = Init; }
void setInc(Expr *Inc) { Data->getChildren()[IncOffset] = Inc; }
void setPreInits(Stmt *PreInits) {
Data->getChildren()[PreInitsOffset] = PreInits;
}
void setIsLastIterVariable(Expr *IL) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[IsLastIterVariableOffset] = IL;
}
void setLowerBoundVariable(Expr *LB) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[LowerBoundVariableOffset] = LB;
}
void setUpperBoundVariable(Expr *UB) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[UpperBoundVariableOffset] = UB;
}
void setStrideVariable(Expr *ST) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[StrideVariableOffset] = ST;
}
void setEnsureUpperBound(Expr *EUB) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[EnsureUpperBoundOffset] = EUB;
}
void setNextLowerBound(Expr *NLB) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[NextLowerBoundOffset] = NLB;
}
void setNextUpperBound(Expr *NUB) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[NextUpperBoundOffset] = NUB;
}
void setNumIterations(Expr *NI) {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
Data->getChildren()[NumIterationsOffset] = NI;
}
void setPrevLowerBoundVariable(Expr *PrevLB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[PrevLowerBoundVariableOffset] = PrevLB;
}
void setPrevUpperBoundVariable(Expr *PrevUB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[PrevUpperBoundVariableOffset] = PrevUB;
}
void setDistInc(Expr *DistInc) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[DistIncOffset] = DistInc;
}
void setPrevEnsureUpperBound(Expr *PrevEUB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[PrevEnsureUpperBoundOffset] = PrevEUB;
}
void setCombinedLowerBoundVariable(Expr *CombLB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[CombinedLowerBoundVariableOffset] = CombLB;
}
void setCombinedUpperBoundVariable(Expr *CombUB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[CombinedUpperBoundVariableOffset] = CombUB;
}
void setCombinedEnsureUpperBound(Expr *CombEUB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[CombinedEnsureUpperBoundOffset] = CombEUB;
}
void setCombinedInit(Expr *CombInit) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[CombinedInitOffset] = CombInit;
}
void setCombinedCond(Expr *CombCond) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[CombinedConditionOffset] = CombCond;
}
void setCombinedNextLowerBound(Expr *CombNLB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[CombinedNextLowerBoundOffset] = CombNLB;
}
void setCombinedNextUpperBound(Expr *CombNUB) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
Data->getChildren()[CombinedNextUpperBoundOffset] = CombNUB;
}
void setCombinedDistCond(Expr *CombDistCond) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound distribute sharing directive");
Data->getChildren()[CombinedDistConditionOffset] = CombDistCond;
}
void setCombinedParForInDistCond(Expr *CombParForInDistCond) {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound distribute sharing directive");
Data->getChildren()[CombinedParForInDistConditionOffset] =
CombParForInDistCond;
}
void setCounters(ArrayRef<Expr *> A);
void setPrivateCounters(ArrayRef<Expr *> A);
void setInits(ArrayRef<Expr *> A);
void setUpdates(ArrayRef<Expr *> A);
void setFinals(ArrayRef<Expr *> A);
void setDependentCounters(ArrayRef<Expr *> A);
void setDependentInits(ArrayRef<Expr *> A);
void setFinalsConditions(ArrayRef<Expr *> A);

public:
Expr *getIterationVariable() const {
return cast<Expr>(Data->getChildren()[IterationVariableOffset]);
}
Expr *getLastIteration() const {
return cast<Expr>(Data->getChildren()[LastIterationOffset]);
}
Expr *getCalcLastIteration() const {
return cast<Expr>(Data->getChildren()[CalcLastIterationOffset]);
}
Expr *getPreCond() const {
return cast<Expr>(Data->getChildren()[PreConditionOffset]);
}
Expr *getCond() const { return cast<Expr>(Data->getChildren()[CondOffset]); }
Expr *getInit() const { return cast<Expr>(Data->getChildren()[InitOffset]); }
Expr *getInc() const { return cast<Expr>(Data->getChildren()[IncOffset]); }
const Stmt *getPreInits() const {
return Data->getChildren()[PreInitsOffset];
}
Stmt *getPreInits() { return Data->getChildren()[PreInitsOffset]; }
Expr *getIsLastIterVariable() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[IsLastIterVariableOffset]);
}
Expr *getLowerBoundVariable() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[LowerBoundVariableOffset]);
}
Expr *getUpperBoundVariable() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[UpperBoundVariableOffset]);
}
Expr *getStrideVariable() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[StrideVariableOffset]);
}
Expr *getEnsureUpperBound() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[EnsureUpperBoundOffset]);
}
Expr *getNextLowerBound() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[NextLowerBoundOffset]);
}
Expr *getNextUpperBound() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[NextUpperBoundOffset]);
}
Expr *getNumIterations() const {
assert((isOpenMPWorksharingDirective(getDirectiveKind()) ||
isOpenMPTaskLoopDirective(getDirectiveKind()) ||
isOpenMPDistributeDirective(getDirectiveKind())) &&
"expected worksharing loop directive");
return cast<Expr>(Data->getChildren()[NumIterationsOffset]);
}
Expr *getPrevLowerBoundVariable() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[PrevLowerBoundVariableOffset]);
}
Expr *getPrevUpperBoundVariable() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[PrevUpperBoundVariableOffset]);
}
Expr *getDistInc() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[DistIncOffset]);
}
Expr *getPrevEnsureUpperBound() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[PrevEnsureUpperBoundOffset]);
}
Expr *getCombinedLowerBoundVariable() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[CombinedLowerBoundVariableOffset]);
}
Expr *getCombinedUpperBoundVariable() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[CombinedUpperBoundVariableOffset]);
}
Expr *getCombinedEnsureUpperBound() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[CombinedEnsureUpperBoundOffset]);
}
Expr *getCombinedInit() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[CombinedInitOffset]);
}
Expr *getCombinedCond() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[CombinedConditionOffset]);
}
Expr *getCombinedNextLowerBound() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[CombinedNextLowerBoundOffset]);
}
Expr *getCombinedNextUpperBound() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound sharing directive");
return cast<Expr>(Data->getChildren()[CombinedNextUpperBoundOffset]);
}
Expr *getCombinedDistCond() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound distribute sharing directive");
return cast<Expr>(Data->getChildren()[CombinedDistConditionOffset]);
}
Expr *getCombinedParForInDistCond() const {
assert(isOpenMPLoopBoundSharingDirective(getDirectiveKind()) &&
"expected loop bound distribute sharing directive");
return cast<Expr>(Data->getChildren()[CombinedParForInDistConditionOffset]);
}
Stmt *getBody();
const Stmt *getBody() const {
return const_cast<OMPLoopDirective *>(this)->getBody();
}

ArrayRef<Expr *> counters() { return getCounters(); }

ArrayRef<Expr *> counters() const {
return const_cast<OMPLoopDirective *>(this)->getCounters();
}

ArrayRef<Expr *> private_counters() { return getPrivateCounters(); }

ArrayRef<Expr *> private_counters() const {
return const_cast<OMPLoopDirective *>(this)->getPrivateCounters();
}

ArrayRef<Expr *> inits() { return getInits(); }

ArrayRef<Expr *> inits() const {
return const_cast<OMPLoopDirective *>(this)->getInits();
}

ArrayRef<Expr *> updates() { return getUpdates(); }

ArrayRef<Expr *> updates() const {
return const_cast<OMPLoopDirective *>(this)->getUpdates();
}

ArrayRef<Expr *> finals() { return getFinals(); }

ArrayRef<Expr *> finals() const {
return const_cast<OMPLoopDirective *>(this)->getFinals();
}

ArrayRef<Expr *> dependent_counters() { return getDependentCounters(); }

ArrayRef<Expr *> dependent_counters() const {
return const_cast<OMPLoopDirective *>(this)->getDependentCounters();
}

ArrayRef<Expr *> dependent_inits() { return getDependentInits(); }

ArrayRef<Expr *> dependent_inits() const {
return const_cast<OMPLoopDirective *>(this)->getDependentInits();
}

ArrayRef<Expr *> finals_conditions() { return getFinalsConditions(); }

ArrayRef<Expr *> finals_conditions() const {
return const_cast<OMPLoopDirective *>(this)->getFinalsConditions();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPSimdDirectiveClass ||
T->getStmtClass() == OMPForDirectiveClass ||
T->getStmtClass() == OMPForSimdDirectiveClass ||
T->getStmtClass() == OMPParallelForDirectiveClass ||
T->getStmtClass() == OMPParallelForSimdDirectiveClass ||
T->getStmtClass() == OMPTaskLoopDirectiveClass ||
T->getStmtClass() == OMPTaskLoopSimdDirectiveClass ||
T->getStmtClass() == OMPMasterTaskLoopDirectiveClass ||
T->getStmtClass() == OMPMasterTaskLoopSimdDirectiveClass ||
T->getStmtClass() == OMPParallelMasterTaskLoopDirectiveClass ||
T->getStmtClass() == OMPParallelMasterTaskLoopSimdDirectiveClass ||
T->getStmtClass() == OMPDistributeDirectiveClass ||
T->getStmtClass() == OMPTargetParallelForDirectiveClass ||
T->getStmtClass() == OMPDistributeParallelForDirectiveClass ||
T->getStmtClass() == OMPDistributeParallelForSimdDirectiveClass ||
T->getStmtClass() == OMPDistributeSimdDirectiveClass ||
T->getStmtClass() == OMPTargetParallelForSimdDirectiveClass ||
T->getStmtClass() == OMPTargetSimdDirectiveClass ||
T->getStmtClass() == OMPTeamsDistributeDirectiveClass ||
T->getStmtClass() == OMPTeamsDistributeSimdDirectiveClass ||
T->getStmtClass() ==
OMPTeamsDistributeParallelForSimdDirectiveClass ||
T->getStmtClass() == OMPTeamsDistributeParallelForDirectiveClass ||
T->getStmtClass() ==
OMPTargetTeamsDistributeParallelForDirectiveClass ||
T->getStmtClass() ==
OMPTargetTeamsDistributeParallelForSimdDirectiveClass ||
T->getStmtClass() == OMPTargetTeamsDistributeDirectiveClass ||
T->getStmtClass() == OMPTargetTeamsDistributeSimdDirectiveClass;
}
};










class OMPSimdDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPSimdDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPSimdDirectiveClass, llvm::omp::OMPD_simd, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPSimdDirectiveClass, llvm::omp::OMPD_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPSimdDirective *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc, unsigned CollapsedNum,
ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt,
const HelperExprs &Exprs);








static OMPSimdDirective *CreateEmpty(const ASTContext &C, unsigned NumClauses,
unsigned CollapsedNum, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPSimdDirectiveClass;
}
};










class OMPForDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;







OMPForDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPForDirectiveClass, llvm::omp::OMPD_for, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPForDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPForDirectiveClass, llvm::omp::OMPD_for,
SourceLocation(), SourceLocation(), CollapsedNum) {}


void setTaskReductionRefExpr(Expr *E) {
Data->getChildren()[numLoopChildren(getLoopsNumber(),
llvm::omp::OMPD_for)] = E;
}


void setHasCancel(bool Has) { HasCancel = Has; }

public:













static OMPForDirective *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc, unsigned CollapsedNum,
ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs,
Expr *TaskRedRef, bool HasCancel);








static OMPForDirective *CreateEmpty(const ASTContext &C, unsigned NumClauses,
unsigned CollapsedNum, EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_for)]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPForDirective *>(this)->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPForDirectiveClass;
}
};










class OMPForSimdDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPForSimdDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPForSimdDirectiveClass, llvm::omp::OMPD_for_simd,
StartLoc, EndLoc, CollapsedNum) {}





explicit OMPForSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPForSimdDirectiveClass, llvm::omp::OMPD_for_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPForSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);








static OMPForSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPForSimdDirectiveClass;
}
};










class OMPSectionsDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


bool HasCancel = false;






OMPSectionsDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPSectionsDirectiveClass,
llvm::omp::OMPD_sections, StartLoc, EndLoc) {}



explicit OMPSectionsDirective()
: OMPExecutableDirective(OMPSectionsDirectiveClass,
llvm::omp::OMPD_sections, SourceLocation(),
SourceLocation()) {}


void setTaskReductionRefExpr(Expr *E) { Data->getChildren()[0] = E; }


void setHasCancel(bool Has) { HasCancel = Has; }

public:











static OMPSectionsDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt, Expr *TaskRedRef,
bool HasCancel);







static OMPSectionsDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[0]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPSectionsDirective *>(this)->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPSectionsDirectiveClass;
}
};







class OMPSectionDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


bool HasCancel = false;






OMPSectionDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPSectionDirectiveClass,
llvm::omp::OMPD_section, StartLoc, EndLoc) {}



explicit OMPSectionDirective()
: OMPExecutableDirective(OMPSectionDirectiveClass,
llvm::omp::OMPD_section, SourceLocation(),
SourceLocation()) {}

public:








static OMPSectionDirective *Create(const ASTContext &C,
SourceLocation StartLoc,
SourceLocation EndLoc,
Stmt *AssociatedStmt, bool HasCancel);





static OMPSectionDirective *CreateEmpty(const ASTContext &C, EmptyShell);


void setHasCancel(bool Has) { HasCancel = Has; }


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPSectionDirectiveClass;
}
};









class OMPSingleDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPSingleDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPSingleDirectiveClass, llvm::omp::OMPD_single,
StartLoc, EndLoc) {}



explicit OMPSingleDirective()
: OMPExecutableDirective(OMPSingleDirectiveClass, llvm::omp::OMPD_single,
SourceLocation(), SourceLocation()) {}

public:








static OMPSingleDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);







static OMPSingleDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPSingleDirectiveClass;
}
};







class OMPMasterDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPMasterDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPMasterDirectiveClass, llvm::omp::OMPD_master,
StartLoc, EndLoc) {}



explicit OMPMasterDirective()
: OMPExecutableDirective(OMPMasterDirectiveClass, llvm::omp::OMPD_master,
SourceLocation(), SourceLocation()) {}

public:







static OMPMasterDirective *Create(const ASTContext &C,
SourceLocation StartLoc,
SourceLocation EndLoc,
Stmt *AssociatedStmt);





static OMPMasterDirective *CreateEmpty(const ASTContext &C, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPMasterDirectiveClass;
}
};







class OMPCriticalDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

DeclarationNameInfo DirName;






OMPCriticalDirective(const DeclarationNameInfo &Name, SourceLocation StartLoc,
SourceLocation EndLoc)
: OMPExecutableDirective(OMPCriticalDirectiveClass,
llvm::omp::OMPD_critical, StartLoc, EndLoc),
DirName(Name) {}



explicit OMPCriticalDirective()
: OMPExecutableDirective(OMPCriticalDirectiveClass,
llvm::omp::OMPD_critical, SourceLocation(),
SourceLocation()) {}





void setDirectiveName(const DeclarationNameInfo &Name) { DirName = Name; }

public:









static OMPCriticalDirective *
Create(const ASTContext &C, const DeclarationNameInfo &Name,
SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);






static OMPCriticalDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);



DeclarationNameInfo getDirectiveName() const { return DirName; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPCriticalDirectiveClass;
}
};










class OMPParallelForDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


bool HasCancel = false;







OMPParallelForDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelForDirectiveClass,
llvm::omp::OMPD_parallel_for, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPParallelForDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelForDirectiveClass,
llvm::omp::OMPD_parallel_for, SourceLocation(),
SourceLocation(), CollapsedNum) {}


void setTaskReductionRefExpr(Expr *E) {
Data->getChildren()[numLoopChildren(getLoopsNumber(),
llvm::omp::OMPD_parallel_for)] = E;
}


void setHasCancel(bool Has) { HasCancel = Has; }

public:













static OMPParallelForDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, Expr *TaskRedRef,
bool HasCancel);








static OMPParallelForDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_parallel_for)]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPParallelForDirective *>(this)
->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPParallelForDirectiveClass;
}
};











class OMPParallelForSimdDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPParallelForSimdDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelForSimdDirectiveClass,
llvm::omp::OMPD_parallel_for_simd, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPParallelForSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelForSimdDirectiveClass,
llvm::omp::OMPD_parallel_for_simd, SourceLocation(),
SourceLocation(), CollapsedNum) {}

public:










static OMPParallelForSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);








static OMPParallelForSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPParallelForSimdDirectiveClass;
}
};









class OMPParallelMasterDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

OMPParallelMasterDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPParallelMasterDirectiveClass,
llvm::omp::OMPD_parallel_master, StartLoc,
EndLoc) {}

explicit OMPParallelMasterDirective()
: OMPExecutableDirective(OMPParallelMasterDirectiveClass,
llvm::omp::OMPD_parallel_master,
SourceLocation(), SourceLocation()) {}


void setTaskReductionRefExpr(Expr *E) { Data->getChildren()[0] = E; }

public:










static OMPParallelMasterDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt, Expr *TaskRedRef);







static OMPParallelMasterDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[0]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPParallelMasterDirective *>(this)
->getTaskReductionRefExpr();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPParallelMasterDirectiveClass;
}
};










class OMPParallelSectionsDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


bool HasCancel = false;






OMPParallelSectionsDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPParallelSectionsDirectiveClass,
llvm::omp::OMPD_parallel_sections, StartLoc,
EndLoc) {}



explicit OMPParallelSectionsDirective()
: OMPExecutableDirective(OMPParallelSectionsDirectiveClass,
llvm::omp::OMPD_parallel_sections,
SourceLocation(), SourceLocation()) {}


void setTaskReductionRefExpr(Expr *E) { Data->getChildren()[0] = E; }


void setHasCancel(bool Has) { HasCancel = Has; }

public:











static OMPParallelSectionsDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt, Expr *TaskRedRef,
bool HasCancel);







static OMPParallelSectionsDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[0]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPParallelSectionsDirective *>(this)
->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPParallelSectionsDirectiveClass;
}
};









class OMPTaskDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;






OMPTaskDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTaskDirectiveClass, llvm::omp::OMPD_task,
StartLoc, EndLoc) {}



explicit OMPTaskDirective()
: OMPExecutableDirective(OMPTaskDirectiveClass, llvm::omp::OMPD_task,
SourceLocation(), SourceLocation()) {}


void setHasCancel(bool Has) { HasCancel = Has; }

public:









static OMPTaskDirective *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, bool HasCancel);







static OMPTaskDirective *CreateEmpty(const ASTContext &C, unsigned NumClauses,
EmptyShell);


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTaskDirectiveClass;
}
};







class OMPTaskyieldDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTaskyieldDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTaskyieldDirectiveClass,
llvm::omp::OMPD_taskyield, StartLoc, EndLoc) {}



explicit OMPTaskyieldDirective()
: OMPExecutableDirective(OMPTaskyieldDirectiveClass,
llvm::omp::OMPD_taskyield, SourceLocation(),
SourceLocation()) {}

public:






static OMPTaskyieldDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc);





static OMPTaskyieldDirective *CreateEmpty(const ASTContext &C, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTaskyieldDirectiveClass;
}
};







class OMPBarrierDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPBarrierDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPBarrierDirectiveClass,
llvm::omp::OMPD_barrier, StartLoc, EndLoc) {}



explicit OMPBarrierDirective()
: OMPExecutableDirective(OMPBarrierDirectiveClass,
llvm::omp::OMPD_barrier, SourceLocation(),
SourceLocation()) {}

public:






static OMPBarrierDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc);





static OMPBarrierDirective *CreateEmpty(const ASTContext &C, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPBarrierDirectiveClass;
}
};







class OMPTaskwaitDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTaskwaitDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTaskwaitDirectiveClass,
llvm::omp::OMPD_taskwait, StartLoc, EndLoc) {}



explicit OMPTaskwaitDirective()
: OMPExecutableDirective(OMPTaskwaitDirectiveClass,
llvm::omp::OMPD_taskwait, SourceLocation(),
SourceLocation()) {}

public:






static OMPTaskwaitDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc);





static OMPTaskwaitDirective *CreateEmpty(const ASTContext &C, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTaskwaitDirectiveClass;
}
};







class OMPTaskgroupDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTaskgroupDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTaskgroupDirectiveClass,
llvm::omp::OMPD_taskgroup, StartLoc, EndLoc) {}



explicit OMPTaskgroupDirective()
: OMPExecutableDirective(OMPTaskgroupDirectiveClass,
llvm::omp::OMPD_taskgroup, SourceLocation(),
SourceLocation()) {}


void setReductionRef(Expr *RR) { Data->getChildren()[0] = RR; }

public:









static OMPTaskgroupDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt,
Expr *ReductionRef);






static OMPTaskgroupDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);



const Expr *getReductionRef() const {
return const_cast<OMPTaskgroupDirective *>(this)->getReductionRef();
}
Expr *getReductionRef() { return cast_or_null<Expr>(Data->getChildren()[0]); }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTaskgroupDirectiveClass;
}
};











class OMPFlushDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPFlushDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPFlushDirectiveClass, llvm::omp::OMPD_flush,
StartLoc, EndLoc) {}



explicit OMPFlushDirective()
: OMPExecutableDirective(OMPFlushDirectiveClass, llvm::omp::OMPD_flush,
SourceLocation(), SourceLocation()) {}

public:








static OMPFlushDirective *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses);







static OMPFlushDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPFlushDirectiveClass;
}
};








class OMPDepobjDirective final : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPDepobjDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPDepobjDirectiveClass, llvm::omp::OMPD_depobj,
StartLoc, EndLoc) {}



explicit OMPDepobjDirective()
: OMPExecutableDirective(OMPDepobjDirectiveClass, llvm::omp::OMPD_depobj,
SourceLocation(), SourceLocation()) {}

public:







static OMPDepobjDirective *Create(const ASTContext &C,
SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses);







static OMPDepobjDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPDepobjDirectiveClass;
}
};







class OMPOrderedDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPOrderedDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPOrderedDirectiveClass,
llvm::omp::OMPD_ordered, StartLoc, EndLoc) {}



explicit OMPOrderedDirective()
: OMPExecutableDirective(OMPOrderedDirectiveClass,
llvm::omp::OMPD_ordered, SourceLocation(),
SourceLocation()) {}

public:








static OMPOrderedDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);







static OMPOrderedDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
bool IsStandalone, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPOrderedDirectiveClass;
}
};








class OMPAtomicDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;









bool IsXLHSInRHSPart = false;








bool IsPostfixUpdate = false;






OMPAtomicDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPAtomicDirectiveClass, llvm::omp::OMPD_atomic,
StartLoc, EndLoc) {}



explicit OMPAtomicDirective()
: OMPExecutableDirective(OMPAtomicDirectiveClass, llvm::omp::OMPD_atomic,
SourceLocation(), SourceLocation()) {}


void setX(Expr *X) { Data->getChildren()[0] = X; }



void setUpdateExpr(Expr *UE) { Data->getChildren()[1] = UE; }

void setV(Expr *V) { Data->getChildren()[2] = V; }

void setExpr(Expr *E) { Data->getChildren()[3] = E; }

public:



















static OMPAtomicDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt, Expr *X, Expr *V,
Expr *E, Expr *UE, bool IsXLHSInRHSPart, bool IsPostfixUpdate);







static OMPAtomicDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);


Expr *getX() { return cast_or_null<Expr>(Data->getChildren()[0]); }
const Expr *getX() const {
return cast_or_null<Expr>(Data->getChildren()[0]);
}



Expr *getUpdateExpr() { return cast_or_null<Expr>(Data->getChildren()[1]); }
const Expr *getUpdateExpr() const {
return cast_or_null<Expr>(Data->getChildren()[1]);
}



bool isXLHSInRHSPart() const { return IsXLHSInRHSPart; }


bool isPostfixUpdate() const { return IsPostfixUpdate; }

Expr *getV() { return cast_or_null<Expr>(Data->getChildren()[2]); }
const Expr *getV() const {
return cast_or_null<Expr>(Data->getChildren()[2]);
}

Expr *getExpr() { return cast_or_null<Expr>(Data->getChildren()[3]); }
const Expr *getExpr() const {
return cast_or_null<Expr>(Data->getChildren()[3]);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPAtomicDirectiveClass;
}
};









class OMPTargetDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTargetDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTargetDirectiveClass, llvm::omp::OMPD_target,
StartLoc, EndLoc) {}



explicit OMPTargetDirective()
: OMPExecutableDirective(OMPTargetDirectiveClass, llvm::omp::OMPD_target,
SourceLocation(), SourceLocation()) {}

public:








static OMPTargetDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);







static OMPTargetDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetDirectiveClass;
}
};










class OMPTargetDataDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTargetDataDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTargetDataDirectiveClass,
llvm::omp::OMPD_target_data, StartLoc, EndLoc) {}



explicit OMPTargetDataDirective()
: OMPExecutableDirective(OMPTargetDataDirectiveClass,
llvm::omp::OMPD_target_data, SourceLocation(),
SourceLocation()) {}

public:








static OMPTargetDataDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);






static OMPTargetDataDirective *CreateEmpty(const ASTContext &C, unsigned N,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetDataDirectiveClass;
}
};










class OMPTargetEnterDataDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTargetEnterDataDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTargetEnterDataDirectiveClass,
llvm::omp::OMPD_target_enter_data, StartLoc,
EndLoc) {}



explicit OMPTargetEnterDataDirective()
: OMPExecutableDirective(OMPTargetEnterDataDirectiveClass,
llvm::omp::OMPD_target_enter_data,
SourceLocation(), SourceLocation()) {}

public:








static OMPTargetEnterDataDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);






static OMPTargetEnterDataDirective *CreateEmpty(const ASTContext &C,
unsigned N, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetEnterDataDirectiveClass;
}
};










class OMPTargetExitDataDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTargetExitDataDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTargetExitDataDirectiveClass,
llvm::omp::OMPD_target_exit_data, StartLoc,
EndLoc) {}



explicit OMPTargetExitDataDirective()
: OMPExecutableDirective(OMPTargetExitDataDirectiveClass,
llvm::omp::OMPD_target_exit_data,
SourceLocation(), SourceLocation()) {}

public:








static OMPTargetExitDataDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);






static OMPTargetExitDataDirective *CreateEmpty(const ASTContext &C,
unsigned N, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetExitDataDirectiveClass;
}
};









class OMPTargetParallelDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;






OMPTargetParallelDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTargetParallelDirectiveClass,
llvm::omp::OMPD_target_parallel, StartLoc,
EndLoc) {}



explicit OMPTargetParallelDirective()
: OMPExecutableDirective(OMPTargetParallelDirectiveClass,
llvm::omp::OMPD_target_parallel,
SourceLocation(), SourceLocation()) {}


void setTaskReductionRefExpr(Expr *E) { Data->getChildren()[0] = E; }

void setHasCancel(bool Has) { HasCancel = Has; }

public:











static OMPTargetParallelDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt, Expr *TaskRedRef,
bool HasCancel);







static OMPTargetParallelDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[0]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPTargetParallelDirective *>(this)
->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetParallelDirectiveClass;
}
};










class OMPTargetParallelForDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


bool HasCancel = false;







OMPTargetParallelForDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetParallelForDirectiveClass,
llvm::omp::OMPD_target_parallel_for, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPTargetParallelForDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetParallelForDirectiveClass,
llvm::omp::OMPD_target_parallel_for, SourceLocation(),
SourceLocation(), CollapsedNum) {}


void setTaskReductionRefExpr(Expr *E) {
Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_target_parallel_for)] = E;
}


void setHasCancel(bool Has) { HasCancel = Has; }

public:













static OMPTargetParallelForDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, Expr *TaskRedRef,
bool HasCancel);








static OMPTargetParallelForDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_target_parallel_for)]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPTargetParallelForDirective *>(this)
->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetParallelForDirectiveClass;
}
};









class OMPTeamsDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTeamsDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTeamsDirectiveClass, llvm::omp::OMPD_teams,
StartLoc, EndLoc) {}



explicit OMPTeamsDirective()
: OMPExecutableDirective(OMPTeamsDirectiveClass, llvm::omp::OMPD_teams,
SourceLocation(), SourceLocation()) {}

public:








static OMPTeamsDirective *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt);







static OMPTeamsDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTeamsDirectiveClass;
}
};








class OMPCancellationPointDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;
OpenMPDirectiveKind CancelRegion = llvm::omp::OMPD_unknown;






OMPCancellationPointDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPCancellationPointDirectiveClass,
llvm::omp::OMPD_cancellation_point, StartLoc,
EndLoc) {}


explicit OMPCancellationPointDirective()
: OMPExecutableDirective(OMPCancellationPointDirectiveClass,
llvm::omp::OMPD_cancellation_point,
SourceLocation(), SourceLocation()) {}



void setCancelRegion(OpenMPDirectiveKind CR) { CancelRegion = CR; }

public:






static OMPCancellationPointDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
OpenMPDirectiveKind CancelRegion);





static OMPCancellationPointDirective *CreateEmpty(const ASTContext &C,
EmptyShell);


OpenMPDirectiveKind getCancelRegion() const { return CancelRegion; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPCancellationPointDirectiveClass;
}
};








class OMPCancelDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;
OpenMPDirectiveKind CancelRegion = llvm::omp::OMPD_unknown;





OMPCancelDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPCancelDirectiveClass, llvm::omp::OMPD_cancel,
StartLoc, EndLoc) {}



explicit OMPCancelDirective()
: OMPExecutableDirective(OMPCancelDirectiveClass, llvm::omp::OMPD_cancel,
SourceLocation(), SourceLocation()) {}



void setCancelRegion(OpenMPDirectiveKind CR) { CancelRegion = CR; }

public:







static OMPCancelDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, OpenMPDirectiveKind CancelRegion);






static OMPCancelDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);


OpenMPDirectiveKind getCancelRegion() const { return CancelRegion; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPCancelDirectiveClass;
}
};










class OMPTaskLoopDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;







OMPTaskLoopDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTaskLoopDirectiveClass, llvm::omp::OMPD_taskloop,
StartLoc, EndLoc, CollapsedNum) {}





explicit OMPTaskLoopDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTaskLoopDirectiveClass, llvm::omp::OMPD_taskloop,
SourceLocation(), SourceLocation(), CollapsedNum) {}


void setHasCancel(bool Has) { HasCancel = Has; }

public:











static OMPTaskLoopDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, bool HasCancel);








static OMPTaskLoopDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum, EmptyShell);


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTaskLoopDirectiveClass;
}
};










class OMPTaskLoopSimdDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPTaskLoopSimdDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTaskLoopSimdDirectiveClass,
llvm::omp::OMPD_taskloop_simd, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPTaskLoopSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTaskLoopSimdDirectiveClass,
llvm::omp::OMPD_taskloop_simd, SourceLocation(),
SourceLocation(), CollapsedNum) {}

public:










static OMPTaskLoopSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);








static OMPTaskLoopSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTaskLoopSimdDirectiveClass;
}
};










class OMPMasterTaskLoopDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;







OMPMasterTaskLoopDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPMasterTaskLoopDirectiveClass,
llvm::omp::OMPD_master_taskloop, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPMasterTaskLoopDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPMasterTaskLoopDirectiveClass,
llvm::omp::OMPD_master_taskloop, SourceLocation(),
SourceLocation(), CollapsedNum) {}


void setHasCancel(bool Has) { HasCancel = Has; }

public:











static OMPMasterTaskLoopDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, bool HasCancel);








static OMPMasterTaskLoopDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPMasterTaskLoopDirectiveClass;
}
};










class OMPMasterTaskLoopSimdDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPMasterTaskLoopSimdDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPMasterTaskLoopSimdDirectiveClass,
llvm::omp::OMPD_master_taskloop_simd, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPMasterTaskLoopSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPMasterTaskLoopSimdDirectiveClass,
llvm::omp::OMPD_master_taskloop_simd, SourceLocation(),
SourceLocation(), CollapsedNum) {}

public:










static OMPMasterTaskLoopSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPMasterTaskLoopSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPMasterTaskLoopSimdDirectiveClass;
}
};











class OMPParallelMasterTaskLoopDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;







OMPParallelMasterTaskLoopDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelMasterTaskLoopDirectiveClass,
llvm::omp::OMPD_parallel_master_taskloop, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPParallelMasterTaskLoopDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelMasterTaskLoopDirectiveClass,
llvm::omp::OMPD_parallel_master_taskloop,
SourceLocation(), SourceLocation(), CollapsedNum) {}


void setHasCancel(bool Has) { HasCancel = Has; }

public:











static OMPParallelMasterTaskLoopDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, bool HasCancel);








static OMPParallelMasterTaskLoopDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPParallelMasterTaskLoopDirectiveClass;
}
};











class OMPParallelMasterTaskLoopSimdDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPParallelMasterTaskLoopSimdDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelMasterTaskLoopSimdDirectiveClass,
llvm::omp::OMPD_parallel_master_taskloop_simd,
StartLoc, EndLoc, CollapsedNum) {}





explicit OMPParallelMasterTaskLoopSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPParallelMasterTaskLoopSimdDirectiveClass,
llvm::omp::OMPD_parallel_master_taskloop_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPParallelMasterTaskLoopSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);








static OMPParallelMasterTaskLoopSimdDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPParallelMasterTaskLoopSimdDirectiveClass;
}
};









class OMPDistributeDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPDistributeDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeDirectiveClass,
llvm::omp::OMPD_distribute, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPDistributeDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeDirectiveClass,
llvm::omp::OMPD_distribute, SourceLocation(),
SourceLocation(), CollapsedNum) {}

public:










static OMPDistributeDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);








static OMPDistributeDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPDistributeDirectiveClass;
}
};










class OMPTargetUpdateDirective : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTargetUpdateDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTargetUpdateDirectiveClass,
llvm::omp::OMPD_target_update, StartLoc,
EndLoc) {}



explicit OMPTargetUpdateDirective()
: OMPExecutableDirective(OMPTargetUpdateDirectiveClass,
llvm::omp::OMPD_target_update, SourceLocation(),
SourceLocation()) {}

public:








static OMPTargetUpdateDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);







static OMPTargetUpdateDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetUpdateDirectiveClass;
}
};










class OMPDistributeParallelForDirective : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;







OMPDistributeParallelForDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeParallelForDirectiveClass,
llvm::omp::OMPD_distribute_parallel_for, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPDistributeParallelForDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeParallelForDirectiveClass,
llvm::omp::OMPD_distribute_parallel_for,
SourceLocation(), SourceLocation(), CollapsedNum) {}


void setTaskReductionRefExpr(Expr *E) {
Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_distribute_parallel_for)] = E;
}


void setHasCancel(bool Has) { HasCancel = Has; }

public:













static OMPDistributeParallelForDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, Expr *TaskRedRef,
bool HasCancel);








static OMPDistributeParallelForDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_distribute_parallel_for)]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPDistributeParallelForDirective *>(this)
->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPDistributeParallelForDirectiveClass;
}
};










class OMPDistributeParallelForSimdDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPDistributeParallelForSimdDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeParallelForSimdDirectiveClass,
llvm::omp::OMPD_distribute_parallel_for_simd, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPDistributeParallelForSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeParallelForSimdDirectiveClass,
llvm::omp::OMPD_distribute_parallel_for_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPDistributeParallelForSimdDirective *Create(
const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPDistributeParallelForSimdDirective *CreateEmpty(
const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPDistributeParallelForSimdDirectiveClass;
}
};









class OMPDistributeSimdDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPDistributeSimdDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeSimdDirectiveClass,
llvm::omp::OMPD_distribute_simd, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPDistributeSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPDistributeSimdDirectiveClass,
llvm::omp::OMPD_distribute_simd, SourceLocation(),
SourceLocation(), CollapsedNum) {}

public:










static OMPDistributeSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPDistributeSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPDistributeSimdDirectiveClass;
}
};










class OMPTargetParallelForSimdDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTargetParallelForSimdDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetParallelForSimdDirectiveClass,
llvm::omp::OMPD_target_parallel_for_simd, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPTargetParallelForSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetParallelForSimdDirectiveClass,
llvm::omp::OMPD_target_parallel_for_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPTargetParallelForSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPTargetParallelForSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetParallelForSimdDirectiveClass;
}
};










class OMPTargetSimdDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTargetSimdDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetSimdDirectiveClass,
llvm::omp::OMPD_target_simd, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPTargetSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetSimdDirectiveClass,
llvm::omp::OMPD_target_simd, SourceLocation(),
SourceLocation(), CollapsedNum) {}

public:










static OMPTargetSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPTargetSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetSimdDirectiveClass;
}
};









class OMPTeamsDistributeDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTeamsDistributeDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeDirectiveClass,
llvm::omp::OMPD_teams_distribute, StartLoc, EndLoc,
CollapsedNum) {}





explicit OMPTeamsDistributeDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeDirectiveClass,
llvm::omp::OMPD_teams_distribute, SourceLocation(),
SourceLocation(), CollapsedNum) {}

public:










static OMPTeamsDistributeDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPTeamsDistributeDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTeamsDistributeDirectiveClass;
}
};










class OMPTeamsDistributeSimdDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTeamsDistributeSimdDirective(SourceLocation StartLoc,
SourceLocation EndLoc, unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeSimdDirectiveClass,
llvm::omp::OMPD_teams_distribute_simd, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPTeamsDistributeSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeSimdDirectiveClass,
llvm::omp::OMPD_teams_distribute_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPTeamsDistributeSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);








static OMPTeamsDistributeSimdDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses,
unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTeamsDistributeSimdDirectiveClass;
}
};










class OMPTeamsDistributeParallelForSimdDirective final
: public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTeamsDistributeParallelForSimdDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeParallelForSimdDirectiveClass,
llvm::omp::OMPD_teams_distribute_parallel_for_simd,
StartLoc, EndLoc, CollapsedNum) {}





explicit OMPTeamsDistributeParallelForSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeParallelForSimdDirectiveClass,
llvm::omp::OMPD_teams_distribute_parallel_for_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPTeamsDistributeParallelForSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPTeamsDistributeParallelForSimdDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTeamsDistributeParallelForSimdDirectiveClass;
}
};










class OMPTeamsDistributeParallelForDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;







OMPTeamsDistributeParallelForDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeParallelForDirectiveClass,
llvm::omp::OMPD_teams_distribute_parallel_for,
StartLoc, EndLoc, CollapsedNum) {}





explicit OMPTeamsDistributeParallelForDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTeamsDistributeParallelForDirectiveClass,
llvm::omp::OMPD_teams_distribute_parallel_for,
SourceLocation(), SourceLocation(), CollapsedNum) {}


void setTaskReductionRefExpr(Expr *E) {
Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_teams_distribute_parallel_for)] = E;
}


void setHasCancel(bool Has) { HasCancel = Has; }

public:













static OMPTeamsDistributeParallelForDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, Expr *TaskRedRef,
bool HasCancel);







static OMPTeamsDistributeParallelForDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[numLoopChildren(
getLoopsNumber(), llvm::omp::OMPD_teams_distribute_parallel_for)]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPTeamsDistributeParallelForDirective *>(this)
->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTeamsDistributeParallelForDirectiveClass;
}
};









class OMPTargetTeamsDirective final : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPTargetTeamsDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPTargetTeamsDirectiveClass,
llvm::omp::OMPD_target_teams, StartLoc, EndLoc) {
}



explicit OMPTargetTeamsDirective()
: OMPExecutableDirective(OMPTargetTeamsDirectiveClass,
llvm::omp::OMPD_target_teams, SourceLocation(),
SourceLocation()) {}

public:








static OMPTargetTeamsDirective *Create(const ASTContext &C,
SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt);






static OMPTargetTeamsDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetTeamsDirectiveClass;
}
};









class OMPTargetTeamsDistributeDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTargetTeamsDistributeDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetTeamsDistributeDirectiveClass,
llvm::omp::OMPD_target_teams_distribute, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPTargetTeamsDistributeDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetTeamsDistributeDirectiveClass,
llvm::omp::OMPD_target_teams_distribute,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPTargetTeamsDistributeDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPTargetTeamsDistributeDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetTeamsDistributeDirectiveClass;
}
};










class OMPTargetTeamsDistributeParallelForDirective final
: public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;

bool HasCancel = false;







OMPTargetTeamsDistributeParallelForDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetTeamsDistributeParallelForDirectiveClass,
llvm::omp::OMPD_target_teams_distribute_parallel_for,
StartLoc, EndLoc, CollapsedNum) {}





explicit OMPTargetTeamsDistributeParallelForDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetTeamsDistributeParallelForDirectiveClass,
llvm::omp::OMPD_target_teams_distribute_parallel_for,
SourceLocation(), SourceLocation(), CollapsedNum) {}


void setTaskReductionRefExpr(Expr *E) {
Data->getChildren()[numLoopChildren(
getLoopsNumber(),
llvm::omp::OMPD_target_teams_distribute_parallel_for)] = E;
}


void setHasCancel(bool Has) { HasCancel = Has; }

public:













static OMPTargetTeamsDistributeParallelForDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs, Expr *TaskRedRef,
bool HasCancel);







static OMPTargetTeamsDistributeParallelForDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);


Expr *getTaskReductionRefExpr() {
return cast_or_null<Expr>(Data->getChildren()[numLoopChildren(
getLoopsNumber(),
llvm::omp::OMPD_target_teams_distribute_parallel_for)]);
}
const Expr *getTaskReductionRefExpr() const {
return const_cast<OMPTargetTeamsDistributeParallelForDirective *>(this)
->getTaskReductionRefExpr();
}


bool hasCancel() const { return HasCancel; }

static bool classof(const Stmt *T) {
return T->getStmtClass() ==
OMPTargetTeamsDistributeParallelForDirectiveClass;
}
};










class OMPTargetTeamsDistributeParallelForSimdDirective final
: public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTargetTeamsDistributeParallelForSimdDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(
OMPTargetTeamsDistributeParallelForSimdDirectiveClass,
llvm::omp::OMPD_target_teams_distribute_parallel_for_simd, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPTargetTeamsDistributeParallelForSimdDirective(
unsigned CollapsedNum)
: OMPLoopDirective(
OMPTargetTeamsDistributeParallelForSimdDirectiveClass,
llvm::omp::OMPD_target_teams_distribute_parallel_for_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPTargetTeamsDistributeParallelForSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPTargetTeamsDistributeParallelForSimdDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() ==
OMPTargetTeamsDistributeParallelForSimdDirectiveClass;
}
};










class OMPTargetTeamsDistributeSimdDirective final : public OMPLoopDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;







OMPTargetTeamsDistributeSimdDirective(SourceLocation StartLoc,
SourceLocation EndLoc,
unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetTeamsDistributeSimdDirectiveClass,
llvm::omp::OMPD_target_teams_distribute_simd, StartLoc,
EndLoc, CollapsedNum) {}





explicit OMPTargetTeamsDistributeSimdDirective(unsigned CollapsedNum)
: OMPLoopDirective(OMPTargetTeamsDistributeSimdDirectiveClass,
llvm::omp::OMPD_target_teams_distribute_simd,
SourceLocation(), SourceLocation(), CollapsedNum) {}

public:










static OMPTargetTeamsDistributeSimdDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
unsigned CollapsedNum, ArrayRef<OMPClause *> Clauses,
Stmt *AssociatedStmt, const HelperExprs &Exprs);







static OMPTargetTeamsDistributeSimdDirective *
CreateEmpty(const ASTContext &C, unsigned NumClauses, unsigned CollapsedNum,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTargetTeamsDistributeSimdDirectiveClass;
}
};


class OMPTileDirective final : public OMPLoopBasedDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


enum {
PreInitsOffset = 0,
TransformedStmtOffset,
};

explicit OMPTileDirective(SourceLocation StartLoc, SourceLocation EndLoc,
unsigned NumLoops)
: OMPLoopBasedDirective(OMPTileDirectiveClass, llvm::omp::OMPD_tile,
StartLoc, EndLoc, NumLoops) {}

void setPreInits(Stmt *PreInits) {
Data->getChildren()[PreInitsOffset] = PreInits;
}

void setTransformedStmt(Stmt *S) {
Data->getChildren()[TransformedStmtOffset] = S;
}

public:












static OMPTileDirective *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses,
unsigned NumLoops, Stmt *AssociatedStmt,
Stmt *TransformedStmt, Stmt *PreInits);






static OMPTileDirective *CreateEmpty(const ASTContext &C, unsigned NumClauses,
unsigned NumLoops);

unsigned getNumAssociatedLoops() const { return getLoopsNumber(); }












Stmt *getTransformedStmt() const {
return Data->getChildren()[TransformedStmtOffset];
}


Stmt *getPreInits() const { return Data->getChildren()[PreInitsOffset]; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPTileDirectiveClass;
}
};







class OMPUnrollDirective final : public OMPLoopBasedDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


enum {
PreInitsOffset = 0,
TransformedStmtOffset,
};

explicit OMPUnrollDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPLoopBasedDirective(OMPUnrollDirectiveClass, llvm::omp::OMPD_unroll,
StartLoc, EndLoc, 1) {}


void setPreInits(Stmt *PreInits) {
Data->getChildren()[PreInitsOffset] = PreInits;
}


void setTransformedStmt(Stmt *S) {
Data->getChildren()[TransformedStmtOffset] = S;
}

public:










static OMPUnrollDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt,
Stmt *TransformedStmt, Stmt *PreInits);





static OMPUnrollDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses);








Stmt *getTransformedStmt() const {
return Data->getChildren()[TransformedStmtOffset];
}


Stmt *getPreInits() const { return Data->getChildren()[PreInitsOffset]; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPUnrollDirectiveClass;
}
};








class OMPScanDirective final : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;





OMPScanDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPScanDirectiveClass, llvm::omp::OMPD_scan,
StartLoc, EndLoc) {}



explicit OMPScanDirective()
: OMPExecutableDirective(OMPScanDirectiveClass, llvm::omp::OMPD_scan,
SourceLocation(), SourceLocation()) {}

public:








static OMPScanDirective *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses);







static OMPScanDirective *CreateEmpty(const ASTContext &C, unsigned NumClauses,
EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPScanDirectiveClass;
}
};









class OMPInteropDirective final : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPInteropDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPInteropDirectiveClass,
llvm::omp::OMPD_interop, StartLoc, EndLoc) {}



explicit OMPInteropDirective()
: OMPExecutableDirective(OMPInteropDirectiveClass,
llvm::omp::OMPD_interop, SourceLocation(),
SourceLocation()) {}

public:







static OMPInteropDirective *Create(const ASTContext &C,
SourceLocation StartLoc,
SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses);





static OMPInteropDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPInteropDirectiveClass;
}
};









class OMPDispatchDirective final : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;


SourceLocation TargetCallLoc;


void setTargetCallLoc(SourceLocation Loc) { TargetCallLoc = Loc; }






OMPDispatchDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPDispatchDirectiveClass,
llvm::omp::OMPD_dispatch, StartLoc, EndLoc) {}



explicit OMPDispatchDirective()
: OMPExecutableDirective(OMPDispatchDirectiveClass,
llvm::omp::OMPD_dispatch, SourceLocation(),
SourceLocation()) {}

public:









static OMPDispatchDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt,
SourceLocation TargetCallLoc);







static OMPDispatchDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);


SourceLocation getTargetCallLoc() const { return TargetCallLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPDispatchDirectiveClass;
}
};








class OMPMaskedDirective final : public OMPExecutableDirective {
friend class ASTStmtReader;
friend class OMPExecutableDirective;






OMPMaskedDirective(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPExecutableDirective(OMPMaskedDirectiveClass, llvm::omp::OMPD_masked,
StartLoc, EndLoc) {}



explicit OMPMaskedDirective()
: OMPExecutableDirective(OMPMaskedDirectiveClass, llvm::omp::OMPD_masked,
SourceLocation(), SourceLocation()) {}

public:







static OMPMaskedDirective *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation EndLoc,
ArrayRef<OMPClause *> Clauses, Stmt *AssociatedStmt);





static OMPMaskedDirective *CreateEmpty(const ASTContext &C,
unsigned NumClauses, EmptyShell);

static bool classof(const Stmt *T) {
return T->getStmtClass() == OMPMaskedDirectiveClass;
}
};

}

#endif
