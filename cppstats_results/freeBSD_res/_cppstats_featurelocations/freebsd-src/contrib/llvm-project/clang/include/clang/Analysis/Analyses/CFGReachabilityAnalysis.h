













#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_CFGREACHABILITYANALYSIS_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_CFGREACHABILITYANALYSIS_H

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"

namespace clang {

class CFG;
class CFGBlock;






class CFGReverseBlockReachabilityAnalysis {
using ReachableSet = llvm::BitVector;
using ReachableMap = llvm::DenseMap<unsigned, ReachableSet>;

ReachableSet analyzed;
ReachableMap reachable;

public:
CFGReverseBlockReachabilityAnalysis(const CFG &cfg);


bool isReachable(const CFGBlock *Src, const CFGBlock *Dst);

private:
void mapReachability(const CFGBlock *Dst);
};

}

#endif
