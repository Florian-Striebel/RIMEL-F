







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_X86_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_X86_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace x86 {

std::string getX86TargetCPU(const llvm::opt::ArgList &Args,
const llvm::Triple &Triple);

void getX86TargetFeatures(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features);

}
}
}
}

#endif
