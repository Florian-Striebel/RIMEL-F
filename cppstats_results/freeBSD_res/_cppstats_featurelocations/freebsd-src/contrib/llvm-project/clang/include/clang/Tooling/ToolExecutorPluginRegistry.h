







#if !defined(LLVM_CLANG_TOOLING_TOOLEXECUTORPLUGINREGISTRY_H)
#define LLVM_CLANG_TOOLING_TOOLEXECUTORPLUGINREGISTRY_H

#include "clang/Tooling/Execution.h"
#include "llvm/Support/Registry.h"

namespace clang {
namespace tooling {

using ToolExecutorPluginRegistry = llvm::Registry<ToolExecutorPlugin>;

}
}

#endif
