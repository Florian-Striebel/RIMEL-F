












#if !defined(LLVM_CLANG_STATICANALYZER_CORE_ANALYZEROPTIONS_H)
#define LLVM_CLANG_STATICANALYZER_CORE_ANALYZEROPTIONS_H

#include "clang/Analysis/PathDiagnostic.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include <string>
#include <utility>
#include <vector>

namespace clang {

namespace ento {

class CheckerBase;

}


enum Analyses {
#define ANALYSIS(NAME, CMDFLAG, DESC, SCOPE) NAME,
#include "clang/StaticAnalyzer/Core/Analyses.def"
NumAnalyses
};


enum AnalysisStores {
#define ANALYSIS_STORE(NAME, CMDFLAG, DESC, CREATFN) NAME##Model,
#include "clang/StaticAnalyzer/Core/Analyses.def"
NumStores
};


enum AnalysisConstraints {
#define ANALYSIS_CONSTRAINTS(NAME, CMDFLAG, DESC, CREATFN) NAME##Model,
#include "clang/StaticAnalyzer/Core/Analyses.def"
NumConstraints
};



enum AnalysisDiagClients {
#define ANALYSIS_DIAGNOSTICS(NAME, CMDFLAG, DESC, CREATFN) PD_##NAME,
#include "clang/StaticAnalyzer/Core/Analyses.def"
PD_NONE,
NUM_ANALYSIS_DIAG_CLIENTS
};


enum AnalysisPurgeMode {
#define ANALYSIS_PURGE(NAME, CMDFLAG, DESC) NAME,
#include "clang/StaticAnalyzer/Core/Analyses.def"
NumPurgeModes
};


enum AnalysisInliningMode {
#define ANALYSIS_INLINING_MODE(NAME, CMDFLAG, DESC) NAME,
#include "clang/StaticAnalyzer/Core/Analyses.def"
NumInliningModes
};






enum CXXInlineableMemberKind {



CIMK_None,


CIMK_MemberFunctions,





CIMK_Constructors,


CIMK_Destructors
};


enum IPAKind {

IPAK_None = 1,


IPAK_BasicInlining = 2,


IPAK_Inlining = 3,


IPAK_DynamicDispatch = 4,



IPAK_DynamicDispatchBifurcate = 5
};

enum class ExplorationStrategyKind {
DFS,
BFS,
UnexploredFirst,
UnexploredFirstQueue,
UnexploredFirstLocationQueue,
BFSBlockDFSContents,
};


enum UserModeKind {

UMK_Shallow = 1,


UMK_Deep = 2
};























class AnalyzerOptions : public RefCountedBase<AnalyzerOptions> {
public:
using ConfigTable = llvm::StringMap<std::string>;



static std::vector<StringRef>
getRegisteredCheckers(bool IncludeExperimental = false);



static std::vector<StringRef>
getRegisteredPackages(bool IncludeExperimental = false);


















static void printFormattedEntry(llvm::raw_ostream &Out,
std::pair<StringRef, StringRef> EntryDescPair,
size_t InitialPad, size_t EntryWidth,
size_t MinLineWidth = 0);


std::vector<std::pair<std::string, bool>> CheckersAndPackages;


std::vector<std::string> SilencedCheckersAndPackages;



ConfigTable Config;
AnalysisStores AnalysisStoreOpt = RegionStoreModel;
AnalysisConstraints AnalysisConstraintsOpt = RangeConstraintsModel;
AnalysisDiagClients AnalysisDiagOpt = PD_HTML;
AnalysisPurgeMode AnalysisPurgeOpt = PurgeStmt;

std::string AnalyzeSpecificFunction;


std::string DumpExplodedGraphTo;



std::string FullCompilerInvocation;


unsigned maxBlockVisitOnPath;






unsigned DisableAllCheckers : 1;

unsigned ShowCheckerHelp : 1;
unsigned ShowCheckerHelpAlpha : 1;
unsigned ShowCheckerHelpDeveloper : 1;

unsigned ShowCheckerOptionList : 1;
unsigned ShowCheckerOptionAlphaList : 1;
unsigned ShowCheckerOptionDeveloperList : 1;

unsigned ShowEnabledCheckerList : 1;
unsigned ShowConfigOptionsList : 1;
unsigned ShouldEmitErrorsOnInvalidConfigValue : 1;
unsigned AnalyzeAll : 1;
unsigned AnalyzerDisplayProgress : 1;
unsigned AnalyzeNestedBlocks : 1;

unsigned eagerlyAssumeBinOpBifurcation : 1;

unsigned TrimGraph : 1;
unsigned visualizeExplodedGraphWithGraphViz : 1;
unsigned UnoptimizedCFG : 1;
unsigned PrintStats : 1;



unsigned NoRetryExhausted : 1;


bool AnalyzerWerror : 1;


unsigned InlineMaxStackDepth;


AnalysisInliningMode InliningMode = NoRedundancy;


#define ANALYZER_OPTION_DEPENDS_ON_USER_MODE(TYPE, NAME, CMDFLAG, DESC, SHALLOW_VAL, DEEP_VAL) ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, SHALLOW_VAL)



#define ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, DEFAULT_VAL) TYPE NAME;


#include "clang/StaticAnalyzer/Core/AnalyzerOptions.def"
#undef ANALYZER_OPTION
#undef ANALYZER_OPTION_DEPENDS_ON_USER_MODE



std::vector<llvm::StringLiteral> AnalyzerConfigCmdFlags = {
#define ANALYZER_OPTION_DEPENDS_ON_USER_MODE(TYPE, NAME, CMDFLAG, DESC, SHALLOW_VAL, DEEP_VAL) ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, SHALLOW_VAL)



#define ANALYZER_OPTION(TYPE, NAME, CMDFLAG, DESC, DEFAULT_VAL) llvm::StringLiteral(CMDFLAG),


#include "clang/StaticAnalyzer/Core/AnalyzerOptions.def"
#undef ANALYZER_OPTION
#undef ANALYZER_OPTION_DEPENDS_ON_USER_MODE
};

bool isUnknownAnalyzerConfig(StringRef Name) const {
assert(llvm::is_sorted(AnalyzerConfigCmdFlags));

return !std::binary_search(AnalyzerConfigCmdFlags.begin(),
AnalyzerConfigCmdFlags.end(), Name);
}

AnalyzerOptions()
: DisableAllCheckers(false), ShowCheckerHelp(false),
ShowCheckerHelpAlpha(false), ShowCheckerHelpDeveloper(false),
ShowCheckerOptionList(false), ShowCheckerOptionAlphaList(false),
ShowCheckerOptionDeveloperList(false), ShowEnabledCheckerList(false),
ShowConfigOptionsList(false), AnalyzeAll(false),
AnalyzerDisplayProgress(false), AnalyzeNestedBlocks(false),
eagerlyAssumeBinOpBifurcation(false), TrimGraph(false),
visualizeExplodedGraphWithGraphViz(false), UnoptimizedCFG(false),
PrintStats(false), NoRetryExhausted(false), AnalyzerWerror(false) {
llvm::sort(AnalyzerConfigCmdFlags);
}
















bool getCheckerBooleanOption(StringRef CheckerName, StringRef OptionName,
bool SearchInParents = false) const;

bool getCheckerBooleanOption(const ento::CheckerBase *C, StringRef OptionName,
bool SearchInParents = false) const;















int getCheckerIntegerOption(StringRef CheckerName, StringRef OptionName,
bool SearchInParents = false) const;

int getCheckerIntegerOption(const ento::CheckerBase *C, StringRef OptionName,
bool SearchInParents = false) const;















StringRef getCheckerStringOption(StringRef CheckerName, StringRef OptionName,
bool SearchInParents = false) const;

StringRef getCheckerStringOption(const ento::CheckerBase *C,
StringRef OptionName,
bool SearchInParents = false) const;




UserModeKind getUserMode() const;

ExplorationStrategyKind getExplorationStrategy() const;


IPAKind getIPAMode() const;







bool mayInlineCXXMemberFunction(CXXInlineableMemberKind K) const;

ento::PathDiagnosticConsumerOptions getDiagOpts() const {
return {FullCompilerInvocation,
ShouldDisplayMacroExpansions,
ShouldSerializeStats,
ShouldWriteStableReportFilename,
AnalyzerWerror,
ShouldApplyFixIts,
ShouldDisplayCheckerNameForText};
}
};

using AnalyzerOptionsRef = IntrusiveRefCntPtr<AnalyzerOptions>;









inline UserModeKind AnalyzerOptions::getUserMode() const {
auto K = llvm::StringSwitch<llvm::Optional<UserModeKind>>(UserMode)
.Case("shallow", UMK_Shallow)
.Case("deep", UMK_Deep)
.Default(None);
assert(K.hasValue() && "User mode is invalid.");
return K.getValue();
}

inline std::vector<StringRef>
AnalyzerOptions::getRegisteredCheckers(bool IncludeExperimental) {
static constexpr llvm::StringLiteral StaticAnalyzerCheckerNames[] = {
#define GET_CHECKERS
#define CHECKER(FULLNAME, CLASS, HELPTEXT, DOC_URI, IS_HIDDEN) llvm::StringLiteral(FULLNAME),

#include "clang/StaticAnalyzer/Checkers/Checkers.inc"
#undef CHECKER
#undef GET_CHECKERS
};
std::vector<StringRef> Checkers;
for (StringRef CheckerName : StaticAnalyzerCheckerNames) {
if (!CheckerName.startswith("debug.") &&
(IncludeExperimental || !CheckerName.startswith("alpha.")))
Checkers.push_back(CheckerName);
}
return Checkers;
}

inline std::vector<StringRef>
AnalyzerOptions::getRegisteredPackages(bool IncludeExperimental) {
static constexpr llvm::StringLiteral StaticAnalyzerPackageNames[] = {
#define GET_PACKAGES
#define PACKAGE(FULLNAME) llvm::StringLiteral(FULLNAME),
#include "clang/StaticAnalyzer/Checkers/Checkers.inc"
#undef PACKAGE
#undef GET_PACKAGES
};
std::vector<StringRef> Packages;
for (StringRef PackageName : StaticAnalyzerPackageNames) {
if (PackageName != "debug" &&
(IncludeExperimental || PackageName != "alpha"))
Packages.push_back(PackageName);
}
return Packages;
}

}

#endif
