





















#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_LOOPUNROLLING_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_LOOPUNROLLING_H

#include "clang/Analysis/CFG.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExplodedGraph.h"
namespace clang {
namespace ento {
class AnalysisManager;



bool isUnrolledState(ProgramStateRef State);


ProgramStateRef updateLoopStack(const Stmt *LoopStmt, ASTContext &ASTCtx,
ExplodedNode* Pred, unsigned maxVisitOnPath);



ProgramStateRef processLoopEnd(const Stmt *LoopStmt, ProgramStateRef State);

}
}

#endif
