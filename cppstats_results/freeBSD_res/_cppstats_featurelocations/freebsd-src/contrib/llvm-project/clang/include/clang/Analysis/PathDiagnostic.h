











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_PATHDIAGNOSTIC_H)
#define LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_PATHDIAGNOSTIC_H

#include "clang/AST/Stmt.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include <cassert>
#include <deque>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace clang {

class AnalysisDeclContext;
class BinaryOperator;
class CallEnter;
class CallExitEnd;
class CallExpr;
class ConditionalOperator;
class Decl;
class Expr;
class LocationContext;
class MemberExpr;
class ProgramPoint;
class SourceManager;

namespace ento {





class PathDiagnostic;



struct PathDiagnosticConsumerOptions {


std::string ToolInvocation;




bool ShouldDisplayMacroExpansions = false;



bool ShouldSerializeStats = false;









bool ShouldWriteStableReportFilename = false;



bool ShouldDisplayWarningsAsErrors = false;



bool ShouldApplyFixIts = false;



bool ShouldDisplayDiagnosticName = false;
};

class PathDiagnosticConsumer {
public:
class PDFileEntry : public llvm::FoldingSetNode {
public:
PDFileEntry(llvm::FoldingSetNodeID &NodeID) : NodeID(NodeID) {}

using ConsumerFiles = std::vector<std::pair<StringRef, StringRef>>;


ConsumerFiles files;


const llvm::FoldingSetNodeID NodeID;


void Profile(llvm::FoldingSetNodeID &ID) { ID = NodeID; }
};

class FilesMade {
llvm::BumpPtrAllocator Alloc;
llvm::FoldingSet<PDFileEntry> Set;

public:
~FilesMade();

bool empty() const { return Set.empty(); }

void addDiagnostic(const PathDiagnostic &PD,
StringRef ConsumerName,
StringRef fileName);

PDFileEntry::ConsumerFiles *getFiles(const PathDiagnostic &PD);
};

private:
virtual void anchor();

public:
PathDiagnosticConsumer() = default;
virtual ~PathDiagnosticConsumer();

void FlushDiagnostics(FilesMade *FilesMade);

virtual void FlushDiagnosticsImpl(std::vector<const PathDiagnostic *> &Diags,
FilesMade *filesMade) = 0;

virtual StringRef getName() const = 0;

void HandlePathDiagnostic(std::unique_ptr<PathDiagnostic> D);

enum PathGenerationScheme {

None,


Minimal,


Extensive,
};

virtual PathGenerationScheme getGenerationScheme() const { return Minimal; }

bool shouldGenerateDiagnostics() const {
return getGenerationScheme() != None;
}

bool shouldAddPathEdges() const { return getGenerationScheme() == Extensive; }

virtual bool supportsLogicalOpControlFlow() const { return false; }



virtual bool supportsCrossFileDiagnostics() const { return false; }

protected:
bool flushed = false;
llvm::FoldingSet<PathDiagnostic> Diags;
};





class PathDiagnosticRange : public SourceRange {
public:
bool isPoint = false;

PathDiagnosticRange(SourceRange R, bool isP = false)
: SourceRange(R), isPoint(isP) {}
PathDiagnosticRange() = default;
};

using LocationOrAnalysisDeclContext =
llvm::PointerUnion<const LocationContext *, AnalysisDeclContext *>;

class PathDiagnosticLocation {
private:
enum Kind { RangeK, SingleLocK, StmtK, DeclK } K = SingleLocK;

const Stmt *S = nullptr;
const Decl *D = nullptr;
const SourceManager *SM = nullptr;
FullSourceLoc Loc;
PathDiagnosticRange Range;

PathDiagnosticLocation(SourceLocation L, const SourceManager &sm, Kind kind)
: K(kind), SM(&sm), Loc(genLocation(L)), Range(genRange()) {}

FullSourceLoc genLocation(
SourceLocation L = SourceLocation(),
LocationOrAnalysisDeclContext LAC = (AnalysisDeclContext *)nullptr) const;

PathDiagnosticRange genRange(
LocationOrAnalysisDeclContext LAC = (AnalysisDeclContext *)nullptr) const;

public:

PathDiagnosticLocation() = default;


PathDiagnosticLocation(const Stmt *s, const SourceManager &sm,
LocationOrAnalysisDeclContext lac)
: K(s->getBeginLoc().isValid() ? StmtK : SingleLocK),
S(K == StmtK ? s : nullptr), SM(&sm),
Loc(genLocation(SourceLocation(), lac)), Range(genRange(lac)) {
assert(K == SingleLocK || S);
assert(K == SingleLocK || Loc.isValid());
assert(K == SingleLocK || Range.isValid());
}


PathDiagnosticLocation(const Decl *d, const SourceManager &sm)
: K(DeclK), D(d), SM(&sm), Loc(genLocation()), Range(genRange()) {
assert(D);
assert(Loc.isValid());
assert(Range.isValid());
}




PathDiagnosticLocation(SourceLocation loc, const SourceManager &sm)
: SM(&sm), Loc(loc, sm), Range(genRange()) {
assert(Loc.isValid());
assert(Range.isValid());
}


static PathDiagnosticLocation create(const Decl *D,
const SourceManager &SM) {
return PathDiagnosticLocation(D, SM);
}


static PathDiagnosticLocation createBegin(const Decl *D,
const SourceManager &SM);




static PathDiagnosticLocation
createBegin(const Decl *D, const SourceManager &SM,
const LocationOrAnalysisDeclContext LAC) {
return createBegin(D, SM);
}


static PathDiagnosticLocation createBegin(const Stmt *S,
const SourceManager &SM,
const LocationOrAnalysisDeclContext LAC);





static PathDiagnosticLocation createEnd(const Stmt *S,
const SourceManager &SM,
const LocationOrAnalysisDeclContext LAC);



static PathDiagnosticLocation createOperatorLoc(const BinaryOperator *BO,
const SourceManager &SM);
static PathDiagnosticLocation createConditionalColonLoc(
const ConditionalOperator *CO,
const SourceManager &SM);



static PathDiagnosticLocation createMemberLoc(const MemberExpr *ME,
const SourceManager &SM);



static PathDiagnosticLocation createBeginBrace(const CompoundStmt *CS,
const SourceManager &SM);



static PathDiagnosticLocation createEndBrace(const CompoundStmt *CS,
const SourceManager &SM);



static PathDiagnosticLocation createDeclBegin(const LocationContext *LC,
const SourceManager &SM);



static PathDiagnosticLocation createDeclEnd(const LocationContext *LC,
const SourceManager &SM);


static PathDiagnosticLocation create(const ProgramPoint &P,
const SourceManager &SMng);


static PathDiagnosticLocation createSingleLocation(
const PathDiagnosticLocation &PDL);




static SourceLocation
getValidSourceLocation(const Stmt *S, LocationOrAnalysisDeclContext LAC,
bool UseEndOfStatement = false);

bool operator==(const PathDiagnosticLocation &X) const {
return K == X.K && Loc == X.Loc && Range == X.Range;
}

bool operator!=(const PathDiagnosticLocation &X) const {
return !(*this == X);
}

bool isValid() const {
return SM != nullptr;
}

FullSourceLoc asLocation() const {
return Loc;
}

PathDiagnosticRange asRange() const {
return Range;
}

const Stmt *asStmt() const { assert(isValid()); return S; }
const Stmt *getStmtOrNull() const {
if (!isValid())
return nullptr;
return asStmt();
}

const Decl *asDecl() const { assert(isValid()); return D; }

bool hasRange() const { return K == StmtK || K == RangeK || K == DeclK; }

bool hasValidLocation() const { return asLocation().isValid(); }

void invalidate() {
*this = PathDiagnosticLocation();
}

void flatten();

const SourceManager& getManager() const { assert(isValid()); return *SM; }

void Profile(llvm::FoldingSetNodeID &ID) const;

void dump() const;
};

class PathDiagnosticLocationPair {
private:
PathDiagnosticLocation Start, End;

public:
PathDiagnosticLocationPair(const PathDiagnosticLocation &start,
const PathDiagnosticLocation &end)
: Start(start), End(end) {}

const PathDiagnosticLocation &getStart() const { return Start; }
const PathDiagnosticLocation &getEnd() const { return End; }

void setStart(const PathDiagnosticLocation &L) { Start = L; }
void setEnd(const PathDiagnosticLocation &L) { End = L; }

void flatten() {
Start.flatten();
End.flatten();
}

void Profile(llvm::FoldingSetNodeID &ID) const {
Start.Profile(ID);
End.Profile(ID);
}
};





class PathDiagnosticPiece: public llvm::FoldingSetNode {
public:
enum Kind { ControlFlow, Event, Macro, Call, Note, PopUp };
enum DisplayHint { Above, Below };

private:
const std::string str;
const Kind kind;
const DisplayHint Hint;



bool LastInMainSourceFile = false;





StringRef Tag;

std::vector<SourceRange> ranges;
std::vector<FixItHint> fixits;

protected:
PathDiagnosticPiece(StringRef s, Kind k, DisplayHint hint = Below);
PathDiagnosticPiece(Kind k, DisplayHint hint = Below);

public:
PathDiagnosticPiece() = delete;
PathDiagnosticPiece(const PathDiagnosticPiece &) = delete;
PathDiagnosticPiece &operator=(const PathDiagnosticPiece &) = delete;
virtual ~PathDiagnosticPiece();

StringRef getString() const { return str; }


void setTag(const char *tag) { Tag = tag; }


const void *getTag() const { return Tag.data(); }



StringRef getTagStr() const { return Tag; }



DisplayHint getDisplayHint() const { return Hint; }

virtual PathDiagnosticLocation getLocation() const = 0;
virtual void flattenLocations() = 0;

Kind getKind() const { return kind; }

void addRange(SourceRange R) {
if (!R.isValid())
return;
ranges.push_back(R);
}

void addRange(SourceLocation B, SourceLocation E) {
if (!B.isValid() || !E.isValid())
return;
ranges.push_back(SourceRange(B,E));
}

void addFixit(FixItHint F) {
fixits.push_back(F);
}


ArrayRef<SourceRange> getRanges() const { return ranges; }


ArrayRef<FixItHint> getFixits() const { return fixits; }

virtual void Profile(llvm::FoldingSetNodeID &ID) const;

void setAsLastInMainSourceFile() {
LastInMainSourceFile = true;
}

bool isLastInMainSourceFile() const {
return LastInMainSourceFile;
}

virtual void dump() const = 0;
};

using PathDiagnosticPieceRef = std::shared_ptr<PathDiagnosticPiece>;

class PathPieces : public std::list<PathDiagnosticPieceRef> {
void flattenTo(PathPieces &Primary, PathPieces &Current,
bool ShouldFlattenMacros) const;

public:
PathPieces flatten(bool ShouldFlattenMacros) const {
PathPieces Result;
flattenTo(Result, Result, ShouldFlattenMacros);
return Result;
}

void dump() const;
};

class PathDiagnosticSpotPiece : public PathDiagnosticPiece {
private:
PathDiagnosticLocation Pos;

public:
PathDiagnosticSpotPiece(const PathDiagnosticLocation &pos,
StringRef s,
PathDiagnosticPiece::Kind k,
bool addPosRange = true)
: PathDiagnosticPiece(s, k), Pos(pos) {
assert(Pos.isValid() && Pos.hasValidLocation() &&
"PathDiagnosticSpotPiece's must have a valid location.");
if (addPosRange && Pos.hasRange()) addRange(Pos.asRange());
}

PathDiagnosticLocation getLocation() const override { return Pos; }
void flattenLocations() override { Pos.flatten(); }

void Profile(llvm::FoldingSetNodeID &ID) const override;

static bool classof(const PathDiagnosticPiece *P) {
return P->getKind() == Event || P->getKind() == Macro ||
P->getKind() == Note || P->getKind() == PopUp;
}
};

class PathDiagnosticEventPiece : public PathDiagnosticSpotPiece {
Optional<bool> IsPrunable;

public:
PathDiagnosticEventPiece(const PathDiagnosticLocation &pos,
StringRef s, bool addPosRange = true)
: PathDiagnosticSpotPiece(pos, s, Event, addPosRange) {}
~PathDiagnosticEventPiece() override;




void setPrunable(bool isPrunable, bool override = false) {
if (IsPrunable.hasValue() && !override)
return;
IsPrunable = isPrunable;
}


bool isPrunable() const {
return IsPrunable.hasValue() ? IsPrunable.getValue() : false;
}

void dump() const override;

static bool classof(const PathDiagnosticPiece *P) {
return P->getKind() == Event;
}
};

class PathDiagnosticCallPiece : public PathDiagnosticPiece {
const Decl *Caller;
const Decl *Callee = nullptr;



bool NoExit;



bool IsCalleeAnAutosynthesizedPropertyAccessor = false;



std::string CallStackMessage;

PathDiagnosticCallPiece(const Decl *callerD,
const PathDiagnosticLocation &callReturnPos)
: PathDiagnosticPiece(Call), Caller(callerD), NoExit(false),
callReturn(callReturnPos) {}
PathDiagnosticCallPiece(PathPieces &oldPath, const Decl *caller)
: PathDiagnosticPiece(Call), Caller(caller), NoExit(true),
path(oldPath) {}

public:
PathDiagnosticLocation callEnter;
PathDiagnosticLocation callEnterWithin;
PathDiagnosticLocation callReturn;
PathPieces path;

~PathDiagnosticCallPiece() override;

const Decl *getCaller() const { return Caller; }

const Decl *getCallee() const { return Callee; }
void setCallee(const CallEnter &CE, const SourceManager &SM);

bool hasCallStackMessage() { return !CallStackMessage.empty(); }
void setCallStackMessage(StringRef st) { CallStackMessage = std::string(st); }

PathDiagnosticLocation getLocation() const override { return callEnter; }

std::shared_ptr<PathDiagnosticEventPiece> getCallEnterEvent() const;
std::shared_ptr<PathDiagnosticEventPiece>
getCallEnterWithinCallerEvent() const;
std::shared_ptr<PathDiagnosticEventPiece> getCallExitEvent() const;

void flattenLocations() override {
callEnter.flatten();
callReturn.flatten();
for (const auto &I : path)
I->flattenLocations();
}

static std::shared_ptr<PathDiagnosticCallPiece>
construct(const CallExitEnd &CE,
const SourceManager &SM);

static PathDiagnosticCallPiece *construct(PathPieces &pieces,
const Decl *caller);

void dump() const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;

static bool classof(const PathDiagnosticPiece *P) {
return P->getKind() == Call;
}
};

class PathDiagnosticControlFlowPiece : public PathDiagnosticPiece {
std::vector<PathDiagnosticLocationPair> LPairs;

public:
PathDiagnosticControlFlowPiece(const PathDiagnosticLocation &startPos,
const PathDiagnosticLocation &endPos,
StringRef s)
: PathDiagnosticPiece(s, ControlFlow) {
LPairs.push_back(PathDiagnosticLocationPair(startPos, endPos));
}

PathDiagnosticControlFlowPiece(const PathDiagnosticLocation &startPos,
const PathDiagnosticLocation &endPos)
: PathDiagnosticPiece(ControlFlow) {
LPairs.push_back(PathDiagnosticLocationPair(startPos, endPos));
}

~PathDiagnosticControlFlowPiece() override;

PathDiagnosticLocation getStartLocation() const {
assert(!LPairs.empty() &&
"PathDiagnosticControlFlowPiece needs at least one location.");
return LPairs[0].getStart();
}

PathDiagnosticLocation getEndLocation() const {
assert(!LPairs.empty() &&
"PathDiagnosticControlFlowPiece needs at least one location.");
return LPairs[0].getEnd();
}

void setStartLocation(const PathDiagnosticLocation &L) {
LPairs[0].setStart(L);
}

void setEndLocation(const PathDiagnosticLocation &L) {
LPairs[0].setEnd(L);
}

void push_back(const PathDiagnosticLocationPair &X) { LPairs.push_back(X); }

PathDiagnosticLocation getLocation() const override {
return getStartLocation();
}

using iterator = std::vector<PathDiagnosticLocationPair>::iterator;

iterator begin() { return LPairs.begin(); }
iterator end() { return LPairs.end(); }

void flattenLocations() override {
for (auto &I : *this)
I.flatten();
}

using const_iterator =
std::vector<PathDiagnosticLocationPair>::const_iterator;

const_iterator begin() const { return LPairs.begin(); }
const_iterator end() const { return LPairs.end(); }

static bool classof(const PathDiagnosticPiece *P) {
return P->getKind() == ControlFlow;
}

void dump() const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;
};

class PathDiagnosticMacroPiece : public PathDiagnosticSpotPiece {
public:
PathDiagnosticMacroPiece(const PathDiagnosticLocation &pos)
: PathDiagnosticSpotPiece(pos, "", Macro) {}
~PathDiagnosticMacroPiece() override;

PathPieces subPieces;

void flattenLocations() override {
PathDiagnosticSpotPiece::flattenLocations();
for (const auto &I : subPieces)
I->flattenLocations();
}

static bool classof(const PathDiagnosticPiece *P) {
return P->getKind() == Macro;
}

void dump() const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;
};

class PathDiagnosticNotePiece: public PathDiagnosticSpotPiece {
public:
PathDiagnosticNotePiece(const PathDiagnosticLocation &Pos, StringRef S,
bool AddPosRange = true)
: PathDiagnosticSpotPiece(Pos, S, Note, AddPosRange) {}
~PathDiagnosticNotePiece() override;

static bool classof(const PathDiagnosticPiece *P) {
return P->getKind() == Note;
}

void dump() const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;
};

class PathDiagnosticPopUpPiece: public PathDiagnosticSpotPiece {
public:
PathDiagnosticPopUpPiece(const PathDiagnosticLocation &Pos, StringRef S,
bool AddPosRange = true)
: PathDiagnosticSpotPiece(Pos, S, PopUp, AddPosRange) {}
~PathDiagnosticPopUpPiece() override;

static bool classof(const PathDiagnosticPiece *P) {
return P->getKind() == PopUp;
}

void dump() const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;
};


using FilesToLineNumsMap = std::map<FileID, std::set<unsigned>>;




class PathDiagnostic : public llvm::FoldingSetNode {
std::string CheckerName;
const Decl *DeclWithIssue;
std::string BugType;
std::string VerboseDesc;
std::string ShortDesc;
std::string Category;
std::deque<std::string> OtherDesc;


PathDiagnosticLocation Loc;

PathPieces pathImpl;
SmallVector<PathPieces *, 3> pathStack;



PathDiagnosticLocation UniqueingLoc;
const Decl *UniqueingDecl;


std::unique_ptr<FilesToLineNumsMap> ExecutedLines;

public:
PathDiagnostic() = delete;
PathDiagnostic(StringRef CheckerName, const Decl *DeclWithIssue,
StringRef bugtype, StringRef verboseDesc, StringRef shortDesc,
StringRef category, PathDiagnosticLocation LocationToUnique,
const Decl *DeclToUnique,
std::unique_ptr<FilesToLineNumsMap> ExecutedLines);
~PathDiagnostic();

const PathPieces &path;



PathPieces &getActivePath() {
if (pathStack.empty())
return pathImpl;
return *pathStack.back();
}


PathPieces &getMutablePieces() {
return pathImpl;
}


unsigned full_size();

void pushActivePath(PathPieces *p) { pathStack.push_back(p); }
void popActivePath() { if (!pathStack.empty()) pathStack.pop_back(); }

bool isWithinCall() const { return !pathStack.empty(); }

void setEndOfPath(PathDiagnosticPieceRef EndPiece) {
assert(!Loc.isValid() && "End location already set!");
Loc = EndPiece->getLocation();
assert(Loc.isValid() && "Invalid location for end-of-path piece");
getActivePath().push_back(std::move(EndPiece));
}

void appendToDesc(StringRef S) {
if (!ShortDesc.empty())
ShortDesc += S;
VerboseDesc += S;
}

StringRef getVerboseDescription() const { return VerboseDesc; }

StringRef getShortDescription() const {
return ShortDesc.empty() ? VerboseDesc : ShortDesc;
}

StringRef getCheckerName() const { return CheckerName; }
StringRef getBugType() const { return BugType; }
StringRef getCategory() const { return Category; }

using meta_iterator = std::deque<std::string>::const_iterator;

meta_iterator meta_begin() const { return OtherDesc.begin(); }
meta_iterator meta_end() const { return OtherDesc.end(); }
void addMeta(StringRef s) { OtherDesc.push_back(std::string(s)); }

const FilesToLineNumsMap &getExecutedLines() const {
return *ExecutedLines;
}

FilesToLineNumsMap &getExecutedLines() {
return *ExecutedLines;
}




const Decl *getDeclWithIssue() const { return DeclWithIssue; }

void setDeclWithIssue(const Decl *D) {
DeclWithIssue = D;
}

PathDiagnosticLocation getLocation() const {
return Loc;
}

void setLocation(PathDiagnosticLocation NewLoc) {
Loc = NewLoc;
}


PathDiagnosticLocation getUniqueingLoc() const {
return UniqueingLoc;
}


const Decl *getUniqueingDecl() const {
return UniqueingDecl;
}

void flattenLocations() {
Loc.flatten();
for (const auto &I : pathImpl)
I->flattenLocations();
}





void Profile(llvm::FoldingSetNodeID &ID) const;





void FullProfile(llvm::FoldingSetNodeID &ID) const;
};

}
}

#endif
