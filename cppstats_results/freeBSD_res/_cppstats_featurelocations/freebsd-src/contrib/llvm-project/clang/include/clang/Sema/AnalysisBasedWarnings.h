











#if !defined(LLVM_CLANG_SEMA_ANALYSISBASEDWARNINGS_H)
#define LLVM_CLANG_SEMA_ANALYSISBASEDWARNINGS_H

#include "llvm/ADT/DenseMap.h"
#include <memory>

namespace clang {

class BlockExpr;
class Decl;
class FunctionDecl;
class ObjCMethodDecl;
class QualType;
class Sema;
namespace sema {
class FunctionScopeInfo;
}

namespace sema {

class AnalysisBasedWarnings {
public:
class Policy {
friend class AnalysisBasedWarnings;

unsigned enableCheckFallThrough : 1;
unsigned enableCheckUnreachable : 1;
unsigned enableThreadSafetyAnalysis : 1;
unsigned enableConsumedAnalysis : 1;
public:
Policy();
void disableCheckFallThrough() { enableCheckFallThrough = 0; }
};

private:
Sema &S;
Policy DefaultPolicy;

class InterProceduralData;
std::unique_ptr<InterProceduralData> IPData;

enum VisitFlag { NotVisited = 0, Visited = 1, Pending = 2 };
llvm::DenseMap<const FunctionDecl*, VisitFlag> VisitedFD;





unsigned NumFunctionsAnalyzed;



unsigned NumFunctionsWithBadCFGs;


unsigned NumCFGBlocks;


unsigned MaxCFGBlocksPerFunction;



unsigned NumUninitAnalysisFunctions;


unsigned NumUninitAnalysisVariables;



unsigned MaxUninitAnalysisVariablesPerFunction;


unsigned NumUninitAnalysisBlockVisits;



unsigned MaxUninitAnalysisBlockVisitsPerFunction;



public:
AnalysisBasedWarnings(Sema &s);
~AnalysisBasedWarnings();

void IssueWarnings(Policy P, FunctionScopeInfo *fscope,
const Decl *D, QualType BlockType);

Policy getDefaultPolicy() { return DefaultPolicy; }

void PrintStats() const;
};

}
}

#endif
