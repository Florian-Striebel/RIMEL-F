











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SIMPLECONSTRAINTMANAGER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SIMPLECONSTRAINTMANAGER_H

#include "clang/StaticAnalyzer/Core/PathSensitive/ConstraintManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang {

namespace ento {

class SimpleConstraintManager : public ConstraintManager {
ExprEngine *EE;
SValBuilder &SVB;

public:
SimpleConstraintManager(ExprEngine *exprengine, SValBuilder &SB)
: EE(exprengine), SVB(SB) {}

~SimpleConstraintManager() override;







ProgramStateRef assume(ProgramStateRef State, DefinedSVal Cond,
bool Assumption) override;

ProgramStateRef assumeInclusiveRange(ProgramStateRef State, NonLoc Value,
const llvm::APSInt &From,
const llvm::APSInt &To,
bool InRange) override;

protected:






virtual ProgramStateRef assumeSym(ProgramStateRef State, SymbolRef Sym,
bool Assumption) = 0;





virtual ProgramStateRef assumeSymInclusiveRange(ProgramStateRef State,
SymbolRef Sym,
const llvm::APSInt &From,
const llvm::APSInt &To,
bool InRange) = 0;



virtual ProgramStateRef assumeSymUnsupported(ProgramStateRef State,
SymbolRef Sym,
bool Assumption) = 0;





SValBuilder &getSValBuilder() const { return SVB; }
BasicValueFactory &getBasicVals() const { return SVB.getBasicValueFactory(); }
SymbolManager &getSymbolManager() const { return SVB.getSymbolManager(); }

private:
ProgramStateRef assume(ProgramStateRef State, NonLoc Cond, bool Assumption);

ProgramStateRef assumeAux(ProgramStateRef State, NonLoc Cond,
bool Assumption);
};

}

}

#endif
