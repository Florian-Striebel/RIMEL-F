







#if !defined(LLVM_CLANG_DRIVER_UTIL_H)
#define LLVM_CLANG_DRIVER_UTIL_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"

namespace clang {
class DiagnosticsEngine;

namespace driver {
class Action;
class JobAction;


typedef llvm::DenseMap<const JobAction*, const char*> ArgStringMap;


typedef SmallVector<Action*, 3> ActionList;

}
}

#endif
