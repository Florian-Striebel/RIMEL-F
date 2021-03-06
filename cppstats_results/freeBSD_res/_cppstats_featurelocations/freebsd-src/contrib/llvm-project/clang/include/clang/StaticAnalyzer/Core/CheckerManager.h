











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_CHECKERMANAGER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_CHECKERMANAGER_H

#include "clang/Analysis/ProgramPoint.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LangOptions.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/Store.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace clang {

class AnalyzerOptions;
class CallExpr;
class CXXNewExpr;
class Decl;
class LocationContext;
class Stmt;
class TranslationUnitDecl;

namespace ento {

class AnalysisManager;
class CXXAllocatorCall;
class BugReporter;
class CallEvent;
class CheckerBase;
class CheckerContext;
class CheckerRegistry;
struct CheckerRegistryData;
class ExplodedGraph;
class ExplodedNode;
class ExplodedNodeSet;
class ExprEngine;
struct EvalCallOptions;
class MemRegion;
struct NodeBuilderContext;
class ObjCMethodCall;
class RegionAndSymbolInvalidationTraits;
class SVal;
class SymbolReaper;

template <typename T> class CheckerFn;

template <typename RET, typename... Ps>
class CheckerFn<RET(Ps...)> {
using Func = RET (*)(void *, Ps...);

Func Fn;

public:
CheckerBase *Checker;

CheckerFn(CheckerBase *checker, Func fn) : Fn(fn), Checker(checker) {}

RET operator()(Ps... ps) const {
return Fn(Checker, ps...);
}
};



enum PointerEscapeKind {


PSK_EscapeOnBind,


PSK_DirectEscapeOnCall,




PSK_IndirectEscapeOnCall,




PSK_EscapeOutParameters,



PSK_EscapeOther
};






class CheckerNameRef {
friend class ::clang::ento::CheckerRegistry;

StringRef Name;

explicit CheckerNameRef(StringRef Name) : Name(Name) {}

public:
CheckerNameRef() = default;

StringRef getName() const { return Name; }
operator StringRef() const { return Name; }
};

enum class ObjCMessageVisitKind {
Pre,
Post,
MessageNil
};

class CheckerManager {
ASTContext *Context = nullptr;
const LangOptions LangOpts;
const AnalyzerOptions &AOptions;
const Preprocessor *PP = nullptr;
CheckerNameRef CurrentCheckerName;
DiagnosticsEngine &Diags;
std::unique_ptr<CheckerRegistryData> RegistryData;

public:






CheckerManager(
ASTContext &Context, AnalyzerOptions &AOptions, const Preprocessor &PP,
ArrayRef<std::string> plugins,
ArrayRef<std::function<void(CheckerRegistry &)>> checkerRegistrationFns);




CheckerManager(ASTContext &Context, AnalyzerOptions &AOptions,
const Preprocessor &PP)
: CheckerManager(Context, AOptions, PP, {}, {}) {}




CheckerManager(AnalyzerOptions &AOptions, const LangOptions &LangOpts,
DiagnosticsEngine &Diags, ArrayRef<std::string> plugins);

~CheckerManager();

void setCurrentCheckerName(CheckerNameRef name) { CurrentCheckerName = name; }
CheckerNameRef getCurrentCheckerName() const { return CurrentCheckerName; }

bool hasPathSensitiveCheckers() const;

void finishedCheckerRegistration();

const LangOptions &getLangOpts() const { return LangOpts; }
const AnalyzerOptions &getAnalyzerOptions() const { return AOptions; }
const Preprocessor &getPreprocessor() const {
assert(PP);
return *PP;
}
const CheckerRegistryData &getCheckerRegistryData() const {
return *RegistryData;
}
DiagnosticsEngine &getDiagnostics() const { return Diags; }
ASTContext &getASTContext() const {
assert(Context);
return *Context;
}



void reportInvalidCheckerOptionValue(const CheckerBase *C,
StringRef OptionName,
StringRef ExpectedValueDesc) const;

using CheckerRef = CheckerBase *;
using CheckerTag = const void *;
using CheckerDtor = CheckerFn<void ()>;










template <typename CHECKER, typename... AT>
CHECKER *registerChecker(AT &&... Args) {
CheckerTag tag = getTag<CHECKER>();
CheckerRef &ref = CheckerTags[tag];
assert(!ref && "Checker already registered, use getChecker!");

CHECKER *checker = new CHECKER(std::forward<AT>(Args)...);
checker->Name = CurrentCheckerName;
CheckerDtors.push_back(CheckerDtor(checker, destruct<CHECKER>));
CHECKER::_register(checker, *this);
ref = checker;
return checker;
}

template <typename CHECKER>
CHECKER *getChecker() {
CheckerTag tag = getTag<CHECKER>();
assert(CheckerTags.count(tag) != 0 &&
"Requested checker is not registered! Maybe you should add it as a "
"dependency in Checkers.td?");
return static_cast<CHECKER *>(CheckerTags[tag]);
}






void runCheckersOnASTDecl(const Decl *D, AnalysisManager& mgr,
BugReporter &BR);


void runCheckersOnASTBody(const Decl *D, AnalysisManager& mgr,
BugReporter &BR);











void runCheckersForPreStmt(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
const Stmt *S,
ExprEngine &Eng) {
runCheckersForStmt(true, Dst, Src, S, Eng);
}







void runCheckersForPostStmt(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
const Stmt *S,
ExprEngine &Eng,
bool wasInlined = false) {
runCheckersForStmt(false, Dst, Src, S, Eng, wasInlined);
}


void runCheckersForStmt(bool isPreVisit,
ExplodedNodeSet &Dst, const ExplodedNodeSet &Src,
const Stmt *S, ExprEngine &Eng,
bool wasInlined = false);


void runCheckersForPreObjCMessage(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
const ObjCMethodCall &msg,
ExprEngine &Eng) {
runCheckersForObjCMessage(ObjCMessageVisitKind::Pre, Dst, Src, msg, Eng);
}


void runCheckersForPostObjCMessage(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
const ObjCMethodCall &msg,
ExprEngine &Eng,
bool wasInlined = false) {
runCheckersForObjCMessage(ObjCMessageVisitKind::Post, Dst, Src, msg, Eng,
wasInlined);
}


void runCheckersForObjCMessageNil(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
const ObjCMethodCall &msg,
ExprEngine &Eng) {
runCheckersForObjCMessage(ObjCMessageVisitKind::MessageNil, Dst, Src, msg,
Eng);
}


void runCheckersForObjCMessage(ObjCMessageVisitKind visitKind,
ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
const ObjCMethodCall &msg, ExprEngine &Eng,
bool wasInlined = false);


void runCheckersForPreCall(ExplodedNodeSet &Dst, const ExplodedNodeSet &Src,
const CallEvent &Call, ExprEngine &Eng) {
runCheckersForCallEvent(true, Dst, Src, Call, Eng);
}


void runCheckersForPostCall(ExplodedNodeSet &Dst, const ExplodedNodeSet &Src,
const CallEvent &Call, ExprEngine &Eng,
bool wasInlined = false) {
runCheckersForCallEvent(false, Dst, Src, Call, Eng,
wasInlined);
}


void runCheckersForCallEvent(bool isPreVisit, ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
const CallEvent &Call, ExprEngine &Eng,
bool wasInlined = false);


void runCheckersForLocation(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
SVal location,
bool isLoad,
const Stmt *NodeEx,
const Stmt *BoundEx,
ExprEngine &Eng);


void runCheckersForBind(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
SVal location, SVal val,
const Stmt *S, ExprEngine &Eng,
const ProgramPoint &PP);


void runCheckersForEndAnalysis(ExplodedGraph &G, BugReporter &BR,
ExprEngine &Eng);


void runCheckersForBeginFunction(ExplodedNodeSet &Dst,
const BlockEdge &L,
ExplodedNode *Pred,
ExprEngine &Eng);


void runCheckersForEndFunction(NodeBuilderContext &BC,
ExplodedNodeSet &Dst,
ExplodedNode *Pred,
ExprEngine &Eng,
const ReturnStmt *RS);


void runCheckersForBranchCondition(const Stmt *condition,
ExplodedNodeSet &Dst, ExplodedNode *Pred,
ExprEngine &Eng);


void runCheckersForNewAllocator(const CXXAllocatorCall &Call,
ExplodedNodeSet &Dst, ExplodedNode *Pred,
ExprEngine &Eng, bool wasInlined = false);






void runCheckersForLiveSymbols(ProgramStateRef state,
SymbolReaper &SymReaper);






void runCheckersForDeadSymbols(ExplodedNodeSet &Dst,
const ExplodedNodeSet &Src,
SymbolReaper &SymReaper, const Stmt *S,
ExprEngine &Eng,
ProgramPoint::Kind K);












ProgramStateRef
runCheckersForRegionChanges(ProgramStateRef state,
const InvalidatedSymbols *invalidated,
ArrayRef<const MemRegion *> ExplicitRegions,
ArrayRef<const MemRegion *> Regions,
const LocationContext *LCtx,
const CallEvent *Call);
















ProgramStateRef
runCheckersForPointerEscape(ProgramStateRef State,
const InvalidatedSymbols &Escaped,
const CallEvent *Call,
PointerEscapeKind Kind,
RegionAndSymbolInvalidationTraits *ITraits);


ProgramStateRef runCheckersForEvalAssume(ProgramStateRef state,
SVal Cond, bool Assumption);




void runCheckersForEvalCall(ExplodedNodeSet &Dst, const ExplodedNodeSet &Src,
const CallEvent &CE, ExprEngine &Eng,
const EvalCallOptions &CallOpts);


void runCheckersOnEndOfTranslationUnit(const TranslationUnitDecl *TU,
AnalysisManager &mgr,
BugReporter &BR);











void runCheckersForPrintStateJson(raw_ostream &Out, ProgramStateRef State,
const char *NL = "\n",
unsigned int Space = 0,
bool IsDot = false) const;








using CheckDeclFunc =
CheckerFn<void (const Decl *, AnalysisManager&, BugReporter &)>;

using HandlesDeclFunc = bool (*)(const Decl *D);

void _registerForDecl(CheckDeclFunc checkfn, HandlesDeclFunc isForDeclFn);

void _registerForBody(CheckDeclFunc checkfn);





using CheckStmtFunc = CheckerFn<void (const Stmt *, CheckerContext &)>;

using CheckObjCMessageFunc =
CheckerFn<void (const ObjCMethodCall &, CheckerContext &)>;

using CheckCallFunc =
CheckerFn<void (const CallEvent &, CheckerContext &)>;

using CheckLocationFunc =
CheckerFn<void (const SVal &location, bool isLoad, const Stmt *S,
CheckerContext &)>;

using CheckBindFunc =
CheckerFn<void (const SVal &location, const SVal &val, const Stmt *S,
CheckerContext &)>;

using CheckEndAnalysisFunc =
CheckerFn<void (ExplodedGraph &, BugReporter &, ExprEngine &)>;

using CheckBeginFunctionFunc = CheckerFn<void (CheckerContext &)>;

using CheckEndFunctionFunc =
CheckerFn<void (const ReturnStmt *, CheckerContext &)>;

using CheckBranchConditionFunc =
CheckerFn<void (const Stmt *, CheckerContext &)>;

using CheckNewAllocatorFunc =
CheckerFn<void(const CXXAllocatorCall &Call, CheckerContext &)>;

using CheckDeadSymbolsFunc =
CheckerFn<void (SymbolReaper &, CheckerContext &)>;

using CheckLiveSymbolsFunc = CheckerFn<void (ProgramStateRef,SymbolReaper &)>;

using CheckRegionChangesFunc =
CheckerFn<ProgramStateRef (ProgramStateRef,
const InvalidatedSymbols *symbols,
ArrayRef<const MemRegion *> ExplicitRegions,
ArrayRef<const MemRegion *> Regions,
const LocationContext *LCtx,
const CallEvent *Call)>;

using CheckPointerEscapeFunc =
CheckerFn<ProgramStateRef (ProgramStateRef,
const InvalidatedSymbols &Escaped,
const CallEvent *Call, PointerEscapeKind Kind,
RegionAndSymbolInvalidationTraits *ITraits)>;

using EvalAssumeFunc =
CheckerFn<ProgramStateRef (ProgramStateRef, const SVal &cond,
bool assumption)>;

using EvalCallFunc = CheckerFn<bool (const CallEvent &, CheckerContext &)>;

using CheckEndOfTranslationUnit =
CheckerFn<void (const TranslationUnitDecl *, AnalysisManager &,
BugReporter &)>;

using HandlesStmtFunc = bool (*)(const Stmt *D);

void _registerForPreStmt(CheckStmtFunc checkfn,
HandlesStmtFunc isForStmtFn);
void _registerForPostStmt(CheckStmtFunc checkfn,
HandlesStmtFunc isForStmtFn);

void _registerForPreObjCMessage(CheckObjCMessageFunc checkfn);
void _registerForPostObjCMessage(CheckObjCMessageFunc checkfn);

void _registerForObjCMessageNil(CheckObjCMessageFunc checkfn);

void _registerForPreCall(CheckCallFunc checkfn);
void _registerForPostCall(CheckCallFunc checkfn);

void _registerForLocation(CheckLocationFunc checkfn);

void _registerForBind(CheckBindFunc checkfn);

void _registerForEndAnalysis(CheckEndAnalysisFunc checkfn);

void _registerForBeginFunction(CheckBeginFunctionFunc checkfn);
void _registerForEndFunction(CheckEndFunctionFunc checkfn);

void _registerForBranchCondition(CheckBranchConditionFunc checkfn);

void _registerForNewAllocator(CheckNewAllocatorFunc checkfn);

void _registerForLiveSymbols(CheckLiveSymbolsFunc checkfn);

void _registerForDeadSymbols(CheckDeadSymbolsFunc checkfn);

void _registerForRegionChanges(CheckRegionChangesFunc checkfn);

void _registerForPointerEscape(CheckPointerEscapeFunc checkfn);

void _registerForConstPointerEscape(CheckPointerEscapeFunc checkfn);

void _registerForEvalAssume(EvalAssumeFunc checkfn);

void _registerForEvalCall(EvalCallFunc checkfn);

void _registerForEndOfTranslationUnit(CheckEndOfTranslationUnit checkfn);





using EventTag = void *;
using CheckEventFunc = CheckerFn<void (const void *event)>;

template <typename EVENT>
void _registerListenerForEvent(CheckEventFunc checkfn) {
EventInfo &info = Events[&EVENT::Tag];
info.Checkers.push_back(checkfn);
}

template <typename EVENT>
void _registerDispatcherForEvent() {
EventInfo &info = Events[&EVENT::Tag];
info.HasDispatcher = true;
}

template <typename EVENT>
void _dispatchEvent(const EVENT &event) const {
EventsTy::const_iterator I = Events.find(&EVENT::Tag);
if (I == Events.end())
return;
const EventInfo &info = I->second;
for (const auto &Checker : info.Checkers)
Checker(&event);
}





private:
template <typename CHECKER>
static void destruct(void *obj) { delete static_cast<CHECKER *>(obj); }

template <typename T>
static void *getTag() { static int tag; return &tag; }

llvm::DenseMap<CheckerTag, CheckerRef> CheckerTags;

std::vector<CheckerDtor> CheckerDtors;

struct DeclCheckerInfo {
CheckDeclFunc CheckFn;
HandlesDeclFunc IsForDeclFn;
};
std::vector<DeclCheckerInfo> DeclCheckers;

std::vector<CheckDeclFunc> BodyCheckers;

using CachedDeclCheckers = SmallVector<CheckDeclFunc, 4>;
using CachedDeclCheckersMapTy = llvm::DenseMap<unsigned, CachedDeclCheckers>;
CachedDeclCheckersMapTy CachedDeclCheckersMap;

struct StmtCheckerInfo {
CheckStmtFunc CheckFn;
HandlesStmtFunc IsForStmtFn;
bool IsPreVisit;
};
std::vector<StmtCheckerInfo> StmtCheckers;

using CachedStmtCheckers = SmallVector<CheckStmtFunc, 4>;
using CachedStmtCheckersMapTy = llvm::DenseMap<unsigned, CachedStmtCheckers>;
CachedStmtCheckersMapTy CachedStmtCheckersMap;

const CachedStmtCheckers &getCachedStmtCheckersFor(const Stmt *S,
bool isPreVisit);



const std::vector<CheckObjCMessageFunc> &
getObjCMessageCheckers(ObjCMessageVisitKind Kind) const;

std::vector<CheckObjCMessageFunc> PreObjCMessageCheckers;
std::vector<CheckObjCMessageFunc> PostObjCMessageCheckers;
std::vector<CheckObjCMessageFunc> ObjCMessageNilCheckers;

std::vector<CheckCallFunc> PreCallCheckers;
std::vector<CheckCallFunc> PostCallCheckers;

std::vector<CheckLocationFunc> LocationCheckers;

std::vector<CheckBindFunc> BindCheckers;

std::vector<CheckEndAnalysisFunc> EndAnalysisCheckers;

std::vector<CheckBeginFunctionFunc> BeginFunctionCheckers;
std::vector<CheckEndFunctionFunc> EndFunctionCheckers;

std::vector<CheckBranchConditionFunc> BranchConditionCheckers;

std::vector<CheckNewAllocatorFunc> NewAllocatorCheckers;

std::vector<CheckLiveSymbolsFunc> LiveSymbolsCheckers;

std::vector<CheckDeadSymbolsFunc> DeadSymbolsCheckers;

std::vector<CheckRegionChangesFunc> RegionChangesCheckers;

std::vector<CheckPointerEscapeFunc> PointerEscapeCheckers;

std::vector<EvalAssumeFunc> EvalAssumeCheckers;

std::vector<EvalCallFunc> EvalCallCheckers;

std::vector<CheckEndOfTranslationUnit> EndOfTranslationUnitCheckers;

struct EventInfo {
SmallVector<CheckEventFunc, 4> Checkers;
bool HasDispatcher = false;

EventInfo() = default;
};

using EventsTy = llvm::DenseMap<EventTag, EventInfo>;
EventsTy Events;
};

}

}

#endif
