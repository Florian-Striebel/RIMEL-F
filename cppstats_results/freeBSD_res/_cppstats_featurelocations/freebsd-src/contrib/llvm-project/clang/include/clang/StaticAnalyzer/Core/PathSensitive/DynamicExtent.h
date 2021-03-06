











#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_DYNAMICEXTENT_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_DYNAMICEXTENT_H

#include "clang/StaticAnalyzer/Core/PathSensitive/MemRegion.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState_Fwd.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SValBuilder.h"

namespace clang {
namespace ento {


DefinedOrUnknownSVal getDynamicExtent(ProgramStateRef State,
const MemRegion *MR, SValBuilder &SVB);


DefinedOrUnknownSVal getElementExtent(QualType Ty, SValBuilder &SVB);


DefinedOrUnknownSVal getDynamicElementCount(ProgramStateRef State,
const MemRegion *MR,
SValBuilder &SVB, QualType Ty);


ProgramStateRef setDynamicExtent(ProgramStateRef State, const MemRegion *MR,
DefinedOrUnknownSVal Extent, SValBuilder &SVB);














SVal getDynamicExtentWithOffset(ProgramStateRef State, SVal BufV);

}
}

#endif
