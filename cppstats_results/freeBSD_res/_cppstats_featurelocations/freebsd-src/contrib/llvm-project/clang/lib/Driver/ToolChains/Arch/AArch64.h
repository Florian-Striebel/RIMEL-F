







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_AARCH64_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_AARCH64_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace aarch64 {

void getAArch64TargetFeatures(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features,
bool ForAS);

std::string getAArch64TargetCPU(const llvm::opt::ArgList &Args,
const llvm::Triple &Triple, llvm::opt::Arg *&A);

}
}
}
}

#endif
