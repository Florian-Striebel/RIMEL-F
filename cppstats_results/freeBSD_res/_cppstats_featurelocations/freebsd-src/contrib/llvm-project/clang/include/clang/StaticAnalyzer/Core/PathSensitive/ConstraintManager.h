











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CONSTRAINTMANAGER_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_CONSTRAINTMANAGER_H

#include "clang/Basic/LLVM.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymExpr.h"
#include "llvm/ADT/Optional.h"
#include "llvm/Support/SaveAndRestore.h"
#include <memory>
#include <utility>

namespace llvm {

class APSInt;

}

namespace clang {
namespace ento {

class ProgramStateManager;
class ExprEngine;
class SymbolReaper;

class ConditionTruthVal {
Optional<bool> Val;

public:


ConditionTruthVal(bool constraint) : Val(constraint) {}


ConditionTruthVal() = default;



bool getValue() const {
return *Val;
}


bool isConstrainedTrue() const {
return Val.hasValue() && Val.getValue();
}


bool isConstrainedFalse() const {
return Val.hasValue() && !Val.getValue();
}


bool isConstrained() const {
return Val.hasValue();
}



bool isUnderconstrained() const {
return !Val.hasValue();
}
};

class ConstraintManager {
public:
ConstraintManager() = default;
virtual ~ConstraintManager();

virtual bool haveEqualConstraints(ProgramStateRef S1,
ProgramStateRef S2) const = 0;

virtual ProgramStateRef assume(ProgramStateRef state,
DefinedSVal Cond,
bool Assumption) = 0;

using ProgramStatePair = std::pair<ProgramStateRef, ProgramStateRef>;



ProgramStatePair assumeDual(ProgramStateRef State, DefinedSVal Cond) {
ProgramStateRef StTrue = assume(State, Cond, true);



if (!StTrue) {
#if defined(EXPENSIVE_CHECKS)
assert(assume(State, Cond, false) && "System is over constrained.");
#endif
return ProgramStatePair((ProgramStateRef)nullptr, State);
}

ProgramStateRef StFalse = assume(State, Cond, false);
if (!StFalse) {



return ProgramStatePair(State, (ProgramStateRef)nullptr);
}

return ProgramStatePair(StTrue, StFalse);
}

virtual ProgramStateRef assumeInclusiveRange(ProgramStateRef State,
NonLoc Value,
const llvm::APSInt &From,
const llvm::APSInt &To,
bool InBound) = 0;

virtual ProgramStatePair assumeInclusiveRangeDual(ProgramStateRef State,
NonLoc Value,
const llvm::APSInt &From,
const llvm::APSInt &To) {
ProgramStateRef StInRange =
assumeInclusiveRange(State, Value, From, To, true);



if (!StInRange)
return ProgramStatePair((ProgramStateRef)nullptr, State);

ProgramStateRef StOutOfRange =
assumeInclusiveRange(State, Value, From, To, false);
if (!StOutOfRange) {



return ProgramStatePair(State, (ProgramStateRef)nullptr);
}

return ProgramStatePair(StInRange, StOutOfRange);
}






virtual const llvm::APSInt* getSymVal(ProgramStateRef state,
SymbolRef sym) const {
return nullptr;
}



virtual ProgramStateRef removeDeadBindings(ProgramStateRef state,
SymbolReaper& SymReaper) = 0;

virtual void printJson(raw_ostream &Out, ProgramStateRef State,
const char *NL, unsigned int Space,
bool IsDot) const = 0;



ConditionTruthVal isNull(ProgramStateRef State, SymbolRef Sym) {
SaveAndRestore<bool> DisableNotify(NotifyAssumeClients, false);

return checkNull(State, Sym);
}

protected:






bool NotifyAssumeClients = true;






virtual bool canReasonAbout(SVal X) const = 0;



virtual ConditionTruthVal checkNull(ProgramStateRef State, SymbolRef Sym);
};

std::unique_ptr<ConstraintManager>
CreateRangeConstraintManager(ProgramStateManager &statemgr,
ExprEngine *exprengine);

std::unique_ptr<ConstraintManager>
CreateZ3ConstraintManager(ProgramStateManager &statemgr,
ExprEngine *exprengine);

}
}

#endif
