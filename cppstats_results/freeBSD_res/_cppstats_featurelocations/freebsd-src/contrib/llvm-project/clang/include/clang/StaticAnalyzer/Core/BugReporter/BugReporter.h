












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_BUGREPORTER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_BUGREPORTER_BUGREPORTER_H

#include "clang/Analysis/PathDiagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporterVisitors.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExplodedGraph.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymExpr.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/ImmutableSet.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"
#include "llvm/ADT/iterator_range.h"
#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace clang {

class AnalyzerOptions;
class ASTContext;
class Decl;
class DiagnosticsEngine;
class LocationContext;
class SourceManager;
class Stmt;

namespace ento {

class BugType;
class CheckerBase;
class ExplodedGraph;
class ExplodedNode;
class ExprEngine;
class MemRegion;
class SValBuilder;







using DiagnosticForConsumerMapTy =
llvm::DenseMap<PathDiagnosticConsumer *, std::unique_ptr<PathDiagnostic>>;





class StackHintGenerator {
public:
virtual ~StackHintGenerator() = 0;


virtual std::string getMessage(const ExplodedNode *N) = 0;
};







class StackHintGeneratorForSymbol : public StackHintGenerator {
private:
SymbolRef Sym;
std::string Msg;

public:
StackHintGeneratorForSymbol(SymbolRef S, StringRef M) : Sym(S), Msg(M) {}
~StackHintGeneratorForSymbol() override = default;



std::string getMessage(const ExplodedNode *N) override;



virtual std::string getMessageForArg(const Expr *ArgE, unsigned ArgIndex);

virtual std::string getMessageForReturn(const CallExpr *CallExpr) {
return Msg;
}

virtual std::string getMessageForSymbolNotFound() {
return Msg;
}
};



class BugReport {
public:
enum class Kind { Basic, PathSensitive };

protected:
friend class BugReportEquivClass;
friend class BugReporter;

Kind K;
const BugType& BT;
std::string ShortDescription;
std::string Description;

SmallVector<SourceRange, 4> Ranges;
SmallVector<std::shared_ptr<PathDiagnosticNotePiece>, 4> Notes;
SmallVector<FixItHint, 4> Fixits;

BugReport(Kind kind, const BugType &bt, StringRef desc)
: BugReport(kind, bt, "", desc) {}

BugReport(Kind K, const BugType &BT, StringRef ShortDescription,
StringRef Description)
: K(K), BT(BT), ShortDescription(ShortDescription),
Description(Description) {}

public:
virtual ~BugReport() = default;

Kind getKind() const { return K; }

const BugType& getBugType() const { return BT; }







StringRef getDescription() const { return Description; }





StringRef getShortDescription(bool UseFallback = true) const {
if (ShortDescription.empty() && UseFallback)
return Description;
return ShortDescription;
}





virtual PathDiagnosticLocation getLocation() const = 0;




virtual const Decl *getDeclWithIssue() const = 0;










virtual PathDiagnosticLocation getUniqueingLocation() const = 0;





virtual const Decl *getUniqueingDecl() const = 0;





void addNote(StringRef Msg, const PathDiagnosticLocation &Pos,
ArrayRef<SourceRange> Ranges = {}) {
auto P = std::make_shared<PathDiagnosticNotePiece>(Pos, Msg);

for (const auto &R : Ranges)
P->addRange(R);

Notes.push_back(std::move(P));
}

ArrayRef<std::shared_ptr<PathDiagnosticNotePiece>> getNotes() {
return Notes;
}








void addRange(SourceRange R) {
assert((R.isValid() || Ranges.empty()) && "Invalid range can only be used "
"to specify that the report does not have a range.");
Ranges.push_back(R);
}


virtual ArrayRef<SourceRange> getRanges() const {
return Ranges;
}








void addFixItHint(const FixItHint &F) {
Fixits.push_back(F);
}

llvm::ArrayRef<FixItHint> getFixits() const { return Fixits; }



virtual void Profile(llvm::FoldingSetNodeID& hash) const = 0;
};

class BasicBugReport : public BugReport {
PathDiagnosticLocation Location;
const Decl *DeclWithIssue = nullptr;

public:
BasicBugReport(const BugType &bt, StringRef desc, PathDiagnosticLocation l)
: BugReport(Kind::Basic, bt, desc), Location(l) {}

static bool classof(const BugReport *R) {
return R->getKind() == Kind::Basic;
}

PathDiagnosticLocation getLocation() const override {
assert(Location.isValid());
return Location;
}

const Decl *getDeclWithIssue() const override {
return DeclWithIssue;
}

PathDiagnosticLocation getUniqueingLocation() const override {
return getLocation();
}

const Decl *getUniqueingDecl() const override {
return getDeclWithIssue();
}



void setDeclWithIssue(const Decl *declWithIssue) {
DeclWithIssue = declWithIssue;
}

void Profile(llvm::FoldingSetNodeID& hash) const override;
};

class PathSensitiveBugReport : public BugReport {
public:
using VisitorList = SmallVector<std::unique_ptr<BugReporterVisitor>, 8>;
using visitor_iterator = VisitorList::iterator;
using visitor_range = llvm::iterator_range<visitor_iterator>;

protected:


const ExplodedNode *ErrorNode = nullptr;



const SourceRange ErrorNodeRange;








llvm::DenseMap<SymbolRef, bugreporter::TrackingKind> InterestingSymbols;






llvm::DenseMap<const MemRegion *, bugreporter::TrackingKind>
InterestingRegions;



llvm::SmallSet<const LocationContext *, 2> InterestingLocationContexts;



VisitorList Callbacks;


llvm::FoldingSet<BugReporterVisitor> CallbacksSet;




bool DoNotPrunePath = false;





using InvalidationRecord = std::pair<const void *, const void *>;






llvm::SmallSet<InvalidationRecord, 4> Invalidations;


llvm::SmallSet<const ExplodedNode *, 4> TrackedConditions;



PathDiagnosticLocation UniqueingLocation;
const Decl *UniqueingDecl;

const Stmt *getStmt() const;






std::map<PathDiagnosticPieceRef, std::unique_ptr<StackHintGenerator>>
StackHints;

public:
PathSensitiveBugReport(const BugType &bt, StringRef desc,
const ExplodedNode *errorNode)
: PathSensitiveBugReport(bt, desc, desc, errorNode) {}

PathSensitiveBugReport(const BugType &bt, StringRef shortDesc, StringRef desc,
const ExplodedNode *errorNode)
: PathSensitiveBugReport(bt, shortDesc, desc, errorNode,
{},
nullptr) {}








PathSensitiveBugReport(const BugType &bt, StringRef desc,
const ExplodedNode *errorNode,
PathDiagnosticLocation LocationToUnique,
const Decl *DeclToUnique)
: PathSensitiveBugReport(bt, desc, desc, errorNode, LocationToUnique,
DeclToUnique) {}

PathSensitiveBugReport(const BugType &bt, StringRef shortDesc, StringRef desc,
const ExplodedNode *errorNode,
PathDiagnosticLocation LocationToUnique,
const Decl *DeclToUnique);

static bool classof(const BugReport *R) {
return R->getKind() == Kind::PathSensitive;
}

const ExplodedNode *getErrorNode() const { return ErrorNode; }



bool shouldPrunePath() const { return !DoNotPrunePath; }


void disablePathPruning() { DoNotPrunePath = true; }


PathDiagnosticLocation getUniqueingLocation() const override {
return UniqueingLocation;
}


const Decl *getUniqueingDecl() const override {
return UniqueingDecl;
}

const Decl *getDeclWithIssue() const override;

ArrayRef<SourceRange> getRanges() const override;

PathDiagnosticLocation getLocation() const override;




void markInteresting(SymbolRef sym, bugreporter::TrackingKind TKind =
bugreporter::TrackingKind::Thorough);

void markNotInteresting(SymbolRef sym);




void markInteresting(
const MemRegion *R,
bugreporter::TrackingKind TKind = bugreporter::TrackingKind::Thorough);

void markNotInteresting(const MemRegion *R);




void markInteresting(SVal V, bugreporter::TrackingKind TKind =
bugreporter::TrackingKind::Thorough);
void markInteresting(const LocationContext *LC);

bool isInteresting(SymbolRef sym) const;
bool isInteresting(const MemRegion *R) const;
bool isInteresting(SVal V) const;
bool isInteresting(const LocationContext *LC) const;

Optional<bugreporter::TrackingKind>
getInterestingnessKind(SymbolRef sym) const;

Optional<bugreporter::TrackingKind>
getInterestingnessKind(const MemRegion *R) const;

Optional<bugreporter::TrackingKind> getInterestingnessKind(SVal V) const;





bool isValid() const {
return Invalidations.empty();
}










void markInvalid(const void *Tag, const void *Data) {
Invalidations.insert(std::make_pair(Tag, Data));
}




void Profile(llvm::FoldingSetNodeID &hash) const override;






void addVisitor(std::unique_ptr<BugReporterVisitor> visitor);

template <class VisitorType, class... Args>
void addVisitor(Args &&... ConstructorArgs) {
addVisitor(
std::make_unique<VisitorType>(std::forward<Args>(ConstructorArgs)...));
}



void clearVisitors();


visitor_iterator visitor_begin() { return Callbacks.begin(); }
visitor_iterator visitor_end() { return Callbacks.end(); }
visitor_range visitors() { return {visitor_begin(), visitor_end()}; }




bool addTrackedCondition(const ExplodedNode *Cond) {
return TrackedConditions.insert(Cond).second;
}

void addCallStackHint(PathDiagnosticPieceRef Piece,
std::unique_ptr<StackHintGenerator> StackHint) {
StackHints[Piece] = std::move(StackHint);
}

bool hasCallStackHint(PathDiagnosticPieceRef Piece) const {
return StackHints.count(Piece) > 0;
}



std::string
getCallStackMessage(PathDiagnosticPieceRef Piece,
const ExplodedNode *N) const {
auto I = StackHints.find(Piece);
if (I != StackHints.end())
return I->second->getMessage(N);
return "";
}
};





class BugReportEquivClass : public llvm::FoldingSetNode {
friend class BugReporter;


llvm::SmallVector<std::unique_ptr<BugReport>, 4> Reports;

void AddReport(std::unique_ptr<BugReport> &&R) {
Reports.push_back(std::move(R));
}

public:
BugReportEquivClass(std::unique_ptr<BugReport> R) { AddReport(std::move(R)); }

ArrayRef<std::unique_ptr<BugReport>> getReports() const { return Reports; }

void Profile(llvm::FoldingSetNodeID& ID) const {
assert(!Reports.empty());
Reports.front()->Profile(ID);
}
};





class BugReporterData {
public:
virtual ~BugReporterData() = default;

virtual ArrayRef<PathDiagnosticConsumer*> getPathDiagnosticConsumers() = 0;
virtual ASTContext &getASTContext() = 0;
virtual SourceManager &getSourceManager() = 0;
virtual AnalyzerOptions &getAnalyzerOptions() = 0;
virtual Preprocessor &getPreprocessor() = 0;
};






class BugReporter {
private:
BugReporterData& D;


void FlushReport(BugReportEquivClass& EQ);


llvm::FoldingSet<BugReportEquivClass> EQClasses;


std::vector<BugReportEquivClass *> EQClassesVector;

public:
BugReporter(BugReporterData &d);
virtual ~BugReporter();


void FlushReports();

ArrayRef<PathDiagnosticConsumer*> getPathDiagnosticConsumers() {
return D.getPathDiagnosticConsumers();
}


using EQClasses_iterator = llvm::FoldingSet<BugReportEquivClass>::iterator;
EQClasses_iterator EQClasses_begin() { return EQClasses.begin(); }
EQClasses_iterator EQClasses_end() { return EQClasses.end(); }

ASTContext &getContext() { return D.getASTContext(); }

const SourceManager &getSourceManager() { return D.getSourceManager(); }

const AnalyzerOptions &getAnalyzerOptions() { return D.getAnalyzerOptions(); }

Preprocessor &getPreprocessor() { return D.getPreprocessor(); }






virtual void emitReport(std::unique_ptr<BugReport> R);

void EmitBasicReport(const Decl *DeclWithIssue, const CheckerBase *Checker,
StringRef BugName, StringRef BugCategory,
StringRef BugStr, PathDiagnosticLocation Loc,
ArrayRef<SourceRange> Ranges = None,
ArrayRef<FixItHint> Fixits = None);

void EmitBasicReport(const Decl *DeclWithIssue, CheckerNameRef CheckerName,
StringRef BugName, StringRef BugCategory,
StringRef BugStr, PathDiagnosticLocation Loc,
ArrayRef<SourceRange> Ranges = None,
ArrayRef<FixItHint> Fixits = None);

private:
llvm::StringMap<std::unique_ptr<BugType>> StrBugTypes;



BugType *getBugTypeForName(CheckerNameRef CheckerName, StringRef name,
StringRef category);

virtual BugReport *
findReportInEquivalenceClass(BugReportEquivClass &eqClass,
SmallVectorImpl<BugReport *> &bugReports) {
return eqClass.getReports()[0].get();
}

protected:

virtual std::unique_ptr<DiagnosticForConsumerMapTy>
generateDiagnosticForConsumerMap(BugReport *exampleReport,
ArrayRef<PathDiagnosticConsumer *> consumers,
ArrayRef<BugReport *> bugReports);
};


class PathSensitiveBugReporter final : public BugReporter {
ExprEngine& Eng;

BugReport *findReportInEquivalenceClass(
BugReportEquivClass &eqClass,
SmallVectorImpl<BugReport *> &bugReports) override;


std::unique_ptr<DiagnosticForConsumerMapTy>
generateDiagnosticForConsumerMap(BugReport *exampleReport,
ArrayRef<PathDiagnosticConsumer *> consumers,
ArrayRef<BugReport *> bugReports) override;
public:
PathSensitiveBugReporter(BugReporterData& d, ExprEngine& eng)
: BugReporter(d), Eng(eng) {}



const ExplodedGraph &getGraph() const;



ProgramStateManager &getStateManager() const;






std::unique_ptr<DiagnosticForConsumerMapTy> generatePathDiagnostics(
ArrayRef<PathDiagnosticConsumer *> consumers,
ArrayRef<PathSensitiveBugReport *> &bugReports);

void emitReport(std::unique_ptr<BugReport> R) override;
};


class BugReporterContext {
PathSensitiveBugReporter &BR;

virtual void anchor();

public:
BugReporterContext(PathSensitiveBugReporter &br) : BR(br) {}

virtual ~BugReporterContext() = default;

PathSensitiveBugReporter& getBugReporter() { return BR; }

ProgramStateManager& getStateManager() const {
return BR.getStateManager();
}

ASTContext &getASTContext() const {
return BR.getContext();
}

const SourceManager& getSourceManager() const {
return BR.getSourceManager();
}

const AnalyzerOptions &getAnalyzerOptions() const {
return BR.getAnalyzerOptions();
}
};









class DataTag : public ProgramPointTag {
public:
StringRef getTagDescription() const override { return "Data Tag"; }


class Factory {
std::vector<std::unique_ptr<DataTag>> Tags;

public:
template <class DataTagType, class... Args>
const DataTagType *make(Args &&... ConstructorArgs) {


Tags.emplace_back(
new DataTagType(std::forward<Args>(ConstructorArgs)...));
return static_cast<DataTagType *>(Tags.back().get());
}
};

protected:
DataTag(void *TagKind) : ProgramPointTag(TagKind) {}
};



class NoteTag : public DataTag {
public:
using Callback = std::function<std::string(BugReporterContext &,
PathSensitiveBugReport &)>;

private:
static int Kind;

const Callback Cb;
const bool IsPrunable;

NoteTag(Callback &&Cb, bool IsPrunable)
: DataTag(&Kind), Cb(std::move(Cb)), IsPrunable(IsPrunable) {}

public:
static bool classof(const ProgramPointTag *T) {
return T->getTagKind() == &Kind;
}

Optional<std::string> generateMessage(BugReporterContext &BRC,
PathSensitiveBugReport &R) const {
std::string Msg = Cb(BRC, R);
if (Msg.empty())
return None;

return std::move(Msg);
}

StringRef getTagDescription() const override {



return "Note Tag";
}

bool isPrunable() const { return IsPrunable; }

friend class Factory;
friend class TagVisitor;
};

}

}

#endif
