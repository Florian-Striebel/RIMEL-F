












#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_RETAINCOUNTCHECKER_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_RETAINCOUNTCHECKER_H

#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "RetainCountDiagnostics.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/ParentMap.h"
#include "clang/Analysis/DomainSpecific/CocoaConventions.h"
#include "clang/Analysis/PathDiagnostic.h"
#include "clang/Analysis/RetainSummaryManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Analysis/SelectorExtras.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymbolManager.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/ImmutableList.h"
#include "llvm/ADT/ImmutableMap.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include <cstdarg>
#include <utility>

namespace clang {
namespace ento {
namespace retaincountchecker {


class RefVal {
public:
enum Kind {
Owned = 0,
NotOwned,
Released,
ReturnedOwned,
ReturnedNotOwned,
ERROR_START,
ErrorDeallocNotOwned,
ErrorUseAfterRelease,
ErrorReleaseNotOwned,
ERROR_LEAK_START,
ErrorLeak,
ErrorLeakReturned,

ErrorOverAutorelease,
ErrorReturnedNotOwned
};





enum class IvarAccessHistory {
None,
AccessedDirectly,
ReleasedAfterDirectAccess
};

private:

unsigned Cnt;

unsigned ACnt;

QualType T;




unsigned RawKind : 5;




unsigned RawObjectKind : 3;










unsigned RawIvarAccessHistory : 2;

RefVal(Kind k, ObjKind o, unsigned cnt, unsigned acnt, QualType t,
IvarAccessHistory IvarAccess)
: Cnt(cnt), ACnt(acnt), T(t), RawKind(static_cast<unsigned>(k)),
RawObjectKind(static_cast<unsigned>(o)),
RawIvarAccessHistory(static_cast<unsigned>(IvarAccess)) {
assert(getKind() == k && "not enough bits for the kind");
assert(getObjKind() == o && "not enough bits for the object kind");
assert(getIvarAccessHistory() == IvarAccess && "not enough bits");
}

public:
Kind getKind() const { return static_cast<Kind>(RawKind); }

ObjKind getObjKind() const {
return static_cast<ObjKind>(RawObjectKind);
}

unsigned getCount() const { return Cnt; }
unsigned getAutoreleaseCount() const { return ACnt; }
unsigned getCombinedCounts() const { return Cnt + ACnt; }
void clearCounts() {
Cnt = 0;
ACnt = 0;
}
void setCount(unsigned i) {
Cnt = i;
}
void setAutoreleaseCount(unsigned i) {
ACnt = i;
}

QualType getType() const { return T; }






IvarAccessHistory getIvarAccessHistory() const {
return static_cast<IvarAccessHistory>(RawIvarAccessHistory);
}

bool isOwned() const {
return getKind() == Owned;
}

bool isNotOwned() const {
return getKind() == NotOwned;
}

bool isReturnedOwned() const {
return getKind() == ReturnedOwned;
}

bool isReturnedNotOwned() const {
return getKind() == ReturnedNotOwned;
}





static RefVal makeOwned(ObjKind o, QualType t) {
return RefVal(Owned, o, 1, 0, t, IvarAccessHistory::None);
}





static RefVal makeNotOwned(ObjKind o, QualType t) {
return RefVal(NotOwned, o, 0, 0, t, IvarAccessHistory::None);
}

RefVal operator-(size_t i) const {
return RefVal(getKind(), getObjKind(), getCount() - i,
getAutoreleaseCount(), getType(), getIvarAccessHistory());
}

RefVal operator+(size_t i) const {
return RefVal(getKind(), getObjKind(), getCount() + i,
getAutoreleaseCount(), getType(), getIvarAccessHistory());
}

RefVal operator^(Kind k) const {
return RefVal(k, getObjKind(), getCount(), getAutoreleaseCount(),
getType(), getIvarAccessHistory());
}

RefVal autorelease() const {
return RefVal(getKind(), getObjKind(), getCount(), getAutoreleaseCount()+1,
getType(), getIvarAccessHistory());
}

RefVal withIvarAccess() const {
assert(getIvarAccessHistory() == IvarAccessHistory::None);
return RefVal(getKind(), getObjKind(), getCount(), getAutoreleaseCount(),
getType(), IvarAccessHistory::AccessedDirectly);
}

RefVal releaseViaIvar() const {
assert(getIvarAccessHistory() == IvarAccessHistory::AccessedDirectly);
return RefVal(getKind(), getObjKind(), getCount(), getAutoreleaseCount(),
getType(), IvarAccessHistory::ReleasedAfterDirectAccess);
}


bool hasSameState(const RefVal &X) const {
return getKind() == X.getKind() && Cnt == X.Cnt && ACnt == X.ACnt &&
getIvarAccessHistory() == X.getIvarAccessHistory();
}

bool operator==(const RefVal& X) const {
return T == X.T && hasSameState(X) && getObjKind() == X.getObjKind();
}

void Profile(llvm::FoldingSetNodeID& ID) const {
ID.Add(T);
ID.AddInteger(RawKind);
ID.AddInteger(Cnt);
ID.AddInteger(ACnt);
ID.AddInteger(RawObjectKind);
ID.AddInteger(RawIvarAccessHistory);
}

void print(raw_ostream &Out) const;
};

class RetainCountChecker
: public Checker< check::Bind,
check::DeadSymbols,
check::BeginFunction,
check::EndFunction,
check::PostStmt<BlockExpr>,
check::PostStmt<CastExpr>,
check::PostStmt<ObjCArrayLiteral>,
check::PostStmt<ObjCDictionaryLiteral>,
check::PostStmt<ObjCBoxedExpr>,
check::PostStmt<ObjCIvarRefExpr>,
check::PostCall,
check::RegionChanges,
eval::Assume,
eval::Call > {

public:
std::unique_ptr<RefCountBug> UseAfterRelease;
std::unique_ptr<RefCountBug> ReleaseNotOwned;
std::unique_ptr<RefCountBug> DeallocNotOwned;
std::unique_ptr<RefCountBug> FreeNotOwned;
std::unique_ptr<RefCountBug> OverAutorelease;
std::unique_ptr<RefCountBug> ReturnNotOwnedForOwned;
std::unique_ptr<RefCountBug> LeakWithinFunction;
std::unique_ptr<RefCountBug> LeakAtReturn;

mutable std::unique_ptr<RetainSummaryManager> Summaries;

static std::unique_ptr<CheckerProgramPointTag> DeallocSentTag;
static std::unique_ptr<CheckerProgramPointTag> CastFailTag;


bool TrackObjCAndCFObjects = false;


bool TrackOSObjects = false;


bool TrackNSCFStartParam = false;

RetainCountChecker() {};

RetainSummaryManager &getSummaryManager(ASTContext &Ctx) const {
if (!Summaries)
Summaries.reset(
new RetainSummaryManager(Ctx, TrackObjCAndCFObjects, TrackOSObjects));
return *Summaries;
}

RetainSummaryManager &getSummaryManager(CheckerContext &C) const {
return getSummaryManager(C.getASTContext());
}

void printState(raw_ostream &Out, ProgramStateRef State,
const char *NL, const char *Sep) const override;

void checkBind(SVal loc, SVal val, const Stmt *S, CheckerContext &C) const;
void checkPostStmt(const BlockExpr *BE, CheckerContext &C) const;
void checkPostStmt(const CastExpr *CE, CheckerContext &C) const;

void checkPostStmt(const ObjCArrayLiteral *AL, CheckerContext &C) const;
void checkPostStmt(const ObjCDictionaryLiteral *DL, CheckerContext &C) const;
void checkPostStmt(const ObjCBoxedExpr *BE, CheckerContext &C) const;

void checkPostStmt(const ObjCIvarRefExpr *IRE, CheckerContext &C) const;

void checkPostCall(const CallEvent &Call, CheckerContext &C) const;

void checkSummary(const RetainSummary &Summ, const CallEvent &Call,
CheckerContext &C) const;

void processSummaryOfInlined(const RetainSummary &Summ,
const CallEvent &Call,
CheckerContext &C) const;

bool evalCall(const CallEvent &Call, CheckerContext &C) const;

ProgramStateRef evalAssume(ProgramStateRef state, SVal Cond,
bool Assumption) const;

ProgramStateRef
checkRegionChanges(ProgramStateRef state,
const InvalidatedSymbols *invalidated,
ArrayRef<const MemRegion *> ExplicitRegions,
ArrayRef<const MemRegion *> Regions,
const LocationContext* LCtx,
const CallEvent *Call) const;

ExplodedNode* checkReturnWithRetEffect(const ReturnStmt *S, CheckerContext &C,
ExplodedNode *Pred, RetEffect RE, RefVal X,
SymbolRef Sym, ProgramStateRef state) const;

void checkDeadSymbols(SymbolReaper &SymReaper, CheckerContext &C) const;
void checkBeginFunction(CheckerContext &C) const;
void checkEndFunction(const ReturnStmt *RS, CheckerContext &C) const;

ProgramStateRef updateSymbol(ProgramStateRef state, SymbolRef sym,
RefVal V, ArgEffect E, RefVal::Kind &hasErr,
CheckerContext &C) const;

const RefCountBug &errorKindToBugKind(RefVal::Kind ErrorKind,
SymbolRef Sym) const;

void processNonLeakError(ProgramStateRef St, SourceRange ErrorRange,
RefVal::Kind ErrorKind, SymbolRef Sym,
CheckerContext &C) const;

void processObjCLiterals(CheckerContext &C, const Expr *Ex) const;

ProgramStateRef handleSymbolDeath(ProgramStateRef state,
SymbolRef sid, RefVal V,
SmallVectorImpl<SymbolRef> &Leaked) const;

ProgramStateRef
handleAutoreleaseCounts(ProgramStateRef state, ExplodedNode *Pred,
const ProgramPointTag *Tag, CheckerContext &Ctx,
SymbolRef Sym,
RefVal V,
const ReturnStmt *S=nullptr) const;

ExplodedNode *processLeaks(ProgramStateRef state,
SmallVectorImpl<SymbolRef> &Leaked,
CheckerContext &Ctx,
ExplodedNode *Pred = nullptr) const;

static const CheckerProgramPointTag &getDeallocSentTag() {
return *DeallocSentTag;
}

static const CheckerProgramPointTag &getCastFailTag() { return *CastFailTag; }

private:



ExplodedNode * processReturn(const ReturnStmt *S, CheckerContext &C) const;
};





const RefVal *getRefBinding(ProgramStateRef State, SymbolRef Sym);



inline bool isSynthesizedAccessor(const StackFrameContext *SFC) {
auto Method = dyn_cast_or_null<ObjCMethodDecl>(SFC->getDecl());
if (!Method || !Method->isPropertyAccessor())
return false;

return SFC->getAnalysisDeclContext()->isBodyAutosynthesized();
}

}
}
}

#endif
