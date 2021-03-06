







#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_ALLOCATIONSTATE_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_ALLOCATIONSTATE_H

#include "clang/StaticAnalyzer/Core/BugReporter/BugReporterVisitors.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang {
namespace ento {

namespace allocation_state {

ProgramStateRef markReleased(ProgramStateRef State, SymbolRef Sym,
const Expr *Origin);




std::unique_ptr<BugReporterVisitor> getInnerPointerBRVisitor(SymbolRef Sym);




const MemRegion *getContainerObjRegion(ProgramStateRef State, SymbolRef Sym);

}

}
}

#endif
