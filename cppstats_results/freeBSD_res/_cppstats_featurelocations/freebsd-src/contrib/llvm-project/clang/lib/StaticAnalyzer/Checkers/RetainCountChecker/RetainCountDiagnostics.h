












#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_RETAINCOUNTCHECKER_DIAGNOSTICS_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_RETAINCOUNTCHECKER_DIAGNOSTICS_H

#include "clang/Analysis/PathDiagnostic.h"
#include "clang/Analysis/RetainSummaryManager.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporterVisitors.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"

namespace clang {
namespace ento {
namespace retaincountchecker {

class RefCountBug : public BugType {
public:
enum RefCountBugKind {
UseAfterRelease,
ReleaseNotOwned,
DeallocNotOwned,
FreeNotOwned,
OverAutorelease,
ReturnNotOwnedForOwned,
LeakWithinFunction,
LeakAtReturn,
};
RefCountBug(CheckerNameRef Checker, RefCountBugKind BT);
StringRef getDescription() const;

RefCountBugKind getBugType() const { return BT; }

private:
RefCountBugKind BT;
static StringRef bugTypeToName(RefCountBugKind BT);
};

class RefCountReport : public PathSensitiveBugReport {
protected:
SymbolRef Sym;
bool isLeak = false;

public:
RefCountReport(const RefCountBug &D, const LangOptions &LOpts,
ExplodedNode *n, SymbolRef sym,
bool isLeak=false);

RefCountReport(const RefCountBug &D, const LangOptions &LOpts,
ExplodedNode *n, SymbolRef sym,
StringRef endText);

ArrayRef<SourceRange> getRanges() const override {
if (!isLeak)
return PathSensitiveBugReport::getRanges();
return {};
}
};

class RefLeakReport : public RefCountReport {
const MemRegion *AllocFirstBinding = nullptr;
const MemRegion *AllocBindingToReport = nullptr;
const Stmt *AllocStmt = nullptr;
PathDiagnosticLocation Location;



void deriveParamLocation(CheckerContext &Ctx);

void deriveAllocLocation(CheckerContext &Ctx);

void createDescription(CheckerContext &Ctx);

void findBindingToReport(CheckerContext &Ctx, ExplodedNode *Node);

public:
RefLeakReport(const RefCountBug &D, const LangOptions &LOpts, ExplodedNode *n,
SymbolRef sym, CheckerContext &Ctx);
PathDiagnosticLocation getLocation() const override {
assert(Location.isValid());
return Location;
}

PathDiagnosticLocation getEndOfPath() const {
return PathSensitiveBugReport::getLocation();
}
};

}
}
}

#endif
