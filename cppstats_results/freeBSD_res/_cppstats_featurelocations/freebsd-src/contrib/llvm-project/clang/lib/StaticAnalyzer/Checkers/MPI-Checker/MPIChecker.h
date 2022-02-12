















#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MPICHECKER_MPICHECKER_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MPICHECKER_MPICHECKER_H

#include "MPIBugReporter.h"
#include "MPITypes.h"
#include "clang/StaticAnalyzer/Checkers/MPIFunctionClassifier.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"

namespace clang {
namespace ento {
namespace mpi {

class MPIChecker : public Checker<check::PreCall, check::DeadSymbols> {
public:
MPIChecker() : BReporter(*this) {}


void checkPreCall(const CallEvent &CE, CheckerContext &Ctx) const {
dynamicInit(Ctx);
checkUnmatchedWaits(CE, Ctx);
checkDoubleNonblocking(CE, Ctx);
}

void checkDeadSymbols(SymbolReaper &SymReaper, CheckerContext &Ctx) const {
dynamicInit(Ctx);
checkMissingWaits(SymReaper, Ctx);
}

void dynamicInit(CheckerContext &Ctx) const {
if (FuncClassifier)
return;
const_cast<std::unique_ptr<MPIFunctionClassifier> &>(FuncClassifier)
.reset(new MPIFunctionClassifier{Ctx.getASTContext()});
}






void checkDoubleNonblocking(const clang::ento::CallEvent &PreCallEvent,
clang::ento::CheckerContext &Ctx) const;






void checkUnmatchedWaits(const clang::ento::CallEvent &PreCallEvent,
clang::ento::CheckerContext &Ctx) const;




void checkMissingWaits(clang::ento::SymbolReaper &SymReaper,
clang::ento::CheckerContext &Ctx) const;

private:








void allRegionsUsedByWait(
llvm::SmallVector<const clang::ento::MemRegion *, 2> &ReqRegions,
const clang::ento::MemRegion *const MR, const clang::ento::CallEvent &CE,
clang::ento::CheckerContext &Ctx) const;





const clang::ento::MemRegion *
topRegionUsedByWait(const clang::ento::CallEvent &CE) const;

const std::unique_ptr<MPIFunctionClassifier> FuncClassifier;
MPIBugReporter BReporter;
};

}
}
}

#endif
