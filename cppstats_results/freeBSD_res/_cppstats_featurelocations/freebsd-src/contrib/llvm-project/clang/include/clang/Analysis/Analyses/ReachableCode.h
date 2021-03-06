











#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_REACHABLECODE_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_REACHABLECODE_H

#include "clang/Basic/SourceLocation.h"





namespace llvm {
class BitVector;
}

namespace clang {
class AnalysisDeclContext;
class CFGBlock;
class Preprocessor;
}





namespace clang {
namespace reachable_code {


enum UnreachableKind {
UK_Return,
UK_Break,
UK_Loop_Increment,
UK_Other
};

class Callback {
virtual void anchor();
public:
virtual ~Callback() {}
virtual void HandleUnreachable(UnreachableKind UK,
SourceLocation L,
SourceRange ConditionVal,
SourceRange R1,
SourceRange R2) = 0;
};



unsigned ScanReachableFromBlock(const CFGBlock *Start,
llvm::BitVector &Reachable);

void FindUnreachableCode(AnalysisDeclContext &AC, Preprocessor &PP,
Callback &CB);

}}

#endif
