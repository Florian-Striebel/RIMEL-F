












#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MOVE_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_MOVE_H

#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang {
namespace ento {
namespace move {


bool isMovedFrom(ProgramStateRef State, const MemRegion *Region);

}
}
}

#endif
