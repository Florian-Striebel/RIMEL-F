













#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_LOOPWIDENING_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_LOOPWIDENING_H

#include "clang/Analysis/CFG.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang {
namespace ento {





ProgramStateRef getWidenedLoopState(ProgramStateRef PrevState,
const LocationContext *LCtx,
unsigned BlockCount, const Stmt *LoopStmt);

}
}

#endif
