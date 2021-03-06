











#if !defined(LLVM_CLANG_FRONTEND_FRONTENDPLUGINREGISTRY_H)
#define LLVM_CLANG_FRONTEND_FRONTENDPLUGINREGISTRY_H

#include "clang/Frontend/FrontendAction.h"
#include "llvm/Support/Registry.h"

namespace clang {


using FrontendPluginRegistry = llvm::Registry<PluginASTAction>;

}

#endif
