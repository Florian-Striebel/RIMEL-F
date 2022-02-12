







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_VE_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_VE_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace ve {

void getVETargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features);

}
}
}
}

#endif
