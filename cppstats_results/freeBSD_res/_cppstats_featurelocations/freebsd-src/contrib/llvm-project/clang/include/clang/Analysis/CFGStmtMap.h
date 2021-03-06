












#if !defined(LLVM_CLANG_ANALYSIS_CFGSTMTMAP_H)
#define LLVM_CLANG_ANALYSIS_CFGSTMTMAP_H

#include "clang/Analysis/CFG.h"

namespace clang {

class ParentMap;
class Stmt;

class CFGStmtMap {
ParentMap *PM;
void *M;

CFGStmtMap(ParentMap *pm, void *m) : PM(pm), M(m) {}

public:
~CFGStmtMap();



static CFGStmtMap *Build(CFG* C, ParentMap *PM);





CFGBlock *getBlock(Stmt * S);

const CFGBlock *getBlock(const Stmt * S) const {
return const_cast<CFGStmtMap*>(this)->getBlock(const_cast<Stmt*>(S));
}
};

}
#endif
