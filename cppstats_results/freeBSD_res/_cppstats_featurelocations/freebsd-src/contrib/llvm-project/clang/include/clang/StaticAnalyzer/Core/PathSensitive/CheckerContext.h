












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CHECKERCONTEXT_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CHECKERCONTEXT_H

#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"

namespace clang {
namespace ento {

class CheckerContext {
ExprEngine &Eng;

ExplodedNode *Pred;



bool Changed;

const ProgramPoint Location;
NodeBuilder &NB;

public:


const bool wasInlined;

CheckerContext(NodeBuilder &builder,
ExprEngine &eng,
ExplodedNode *pred,
const ProgramPoint &loc,
bool wasInlined = false)
: Eng(eng),
Pred(pred),
Changed(false),
Location(loc),
NB(builder),
wasInlined(wasInlined) {
assert(Pred->getState() &&
"We should not call the checkers on an empty state.");
}

AnalysisManager &getAnalysisManager() {
return Eng.getAnalysisManager();
}

ConstraintManager &getConstraintManager() {
return Eng.getConstraintManager();
}

StoreManager &getStoreManager() {
return Eng.getStoreManager();
}




ExplodedNode *getPredecessor() { return Pred; }
const ProgramStateRef &getState() const { return Pred->getState(); }



bool isDifferent() { return Changed; }



unsigned blockCount() const {
return NB.getContext().blockCount();
}

ASTContext &getASTContext() {
return Eng.getContext();
}

const LangOptions &getLangOpts() const {
return Eng.getContext().getLangOpts();
}

const LocationContext *getLocationContext() const {
return Pred->getLocationContext();
}

const StackFrameContext *getStackFrame() const {
return Pred->getStackFrame();
}


bool inTopFrame() const { return getLocationContext()->inTopFrame(); }

BugReporter &getBugReporter() {
return Eng.getBugReporter();
}

const SourceManager &getSourceManager() {
return getBugReporter().getSourceManager();
}

Preprocessor &getPreprocessor() { return getBugReporter().getPreprocessor(); }

SValBuilder &getSValBuilder() {
return Eng.getSValBuilder();
}

SymbolManager &getSymbolManager() {
return getSValBuilder().getSymbolManager();
}

ProgramStateManager &getStateManager() {
return Eng.getStateManager();
}

AnalysisDeclContext *getCurrentAnalysisDeclContext() const {
return Pred->getLocationContext()->getAnalysisDeclContext();
}


unsigned getBlockID() const {
return NB.getContext().getBlock()->getBlockID();
}






static const MemRegion *getLocationRegionIfPostStore(const ExplodedNode *N) {
ProgramPoint L = N->getLocation();
if (Optional<PostStore> PSL = L.getAs<PostStore>())
return reinterpret_cast<const MemRegion*>(PSL->getLocationValue());
return nullptr;
}


SVal getSVal(const Stmt *S) const {
return Pred->getSVal(S);
}



bool isGreaterOrEqual(const Expr *E, unsigned long long Val);


bool isNegative(const Expr *E);










ExplodedNode *addTransition(ProgramStateRef State = nullptr,
const ProgramPointTag *Tag = nullptr) {
return addTransitionImpl(State ? State : getState(), false, nullptr, Tag);
}








ExplodedNode *addTransition(ProgramStateRef State, ExplodedNode *Pred,
const ProgramPointTag *Tag = nullptr) {
return addTransitionImpl(State, false, Pred, Tag);
}




ExplodedNode *generateSink(ProgramStateRef State, ExplodedNode *Pred,
const ProgramPointTag *Tag = nullptr) {
return addTransitionImpl(State ? State : getState(), true, Pred, Tag);
}


void addSink(ProgramStateRef State = nullptr,
const ProgramPointTag *Tag = nullptr) {
if (!State)
State = getState();
addTransition(State, generateSink(State, getPredecessor()));
}








ExplodedNode *generateErrorNode(ProgramStateRef State = nullptr,
const ProgramPointTag *Tag = nullptr) {
return generateSink(State, Pred,
(Tag ? Tag : Location.getTag()));
}








ExplodedNode *
generateNonFatalErrorNode(ProgramStateRef State = nullptr,
const ProgramPointTag *Tag = nullptr) {
return addTransition(State, (Tag ? Tag : Location.getTag()));
}










ExplodedNode *
generateNonFatalErrorNode(ProgramStateRef State,
ExplodedNode *Pred,
const ProgramPointTag *Tag = nullptr) {
return addTransition(State, Pred, (Tag ? Tag : Location.getTag()));
}


void emitReport(std::unique_ptr<BugReport> R) {
Changed = true;
Eng.getBugReporter().emitReport(std::move(R));
}











const NoteTag *getNoteTag(NoteTag::Callback &&Cb, bool IsPrunable = false) {
return Eng.getDataTags().make<NoteTag>(std::move(Cb), IsPrunable);
}








const NoteTag
*getNoteTag(std::function<std::string(PathSensitiveBugReport &)> &&Cb,
bool IsPrunable = false) {
return getNoteTag(
[Cb](BugReporterContext &,
PathSensitiveBugReport &BR) { return Cb(BR); },
IsPrunable);
}








const NoteTag *getNoteTag(std::function<std::string()> &&Cb,
bool IsPrunable = false) {
return getNoteTag([Cb](BugReporterContext &,
PathSensitiveBugReport &) { return Cb(); },
IsPrunable);
}







const NoteTag *getNoteTag(StringRef Note, bool IsPrunable = false) {
return getNoteTag(
[Note](BugReporterContext &,
PathSensitiveBugReport &) { return std::string(Note); },
IsPrunable);
}








const NoteTag *getNoteTag(
std::function<void(PathSensitiveBugReport &BR, llvm::raw_ostream &OS)> &&Cb,
bool IsPrunable = false) {
return getNoteTag(
[Cb](PathSensitiveBugReport &BR) -> std::string {
llvm::SmallString<128> Str;
llvm::raw_svector_ostream OS(Str);
Cb(BR, OS);
return std::string(OS.str());
},
IsPrunable);
}



StringRef getDeclDescription(const Decl *D);


const FunctionDecl *getCalleeDecl(const CallExpr *CE) const;


StringRef getCalleeName(const FunctionDecl *FunDecl) const;


const IdentifierInfo *getCalleeIdentifier(const CallExpr *CE) const {
const FunctionDecl *FunDecl = getCalleeDecl(CE);
if (FunDecl)
return FunDecl->getIdentifier();
else
return nullptr;
}


StringRef getCalleeName(const CallExpr *CE) const {
const FunctionDecl *FunDecl = getCalleeDecl(CE);
return getCalleeName(FunDecl);
}











static bool isCLibraryFunction(const FunctionDecl *FD,
StringRef Name = StringRef());










StringRef getMacroNameOrSpelling(SourceLocation &Loc);

private:
ExplodedNode *addTransitionImpl(ProgramStateRef State,
bool MarkAsSink,
ExplodedNode *P = nullptr,
const ProgramPointTag *Tag = nullptr) {












if (!State || (State == Pred->getState() && !Tag && !MarkAsSink))
return Pred;

Changed = true;
const ProgramPoint &LocalLoc = (Tag ? Location.withTag(Tag) : Location);
if (!P)
P = Pred;

ExplodedNode *node;
if (MarkAsSink)
node = NB.generateSink(LocalLoc, State, P);
else
node = NB.generateNode(LocalLoc, State, P);
return node;
}
};

}

}

#endif
