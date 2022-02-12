







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_RISCV_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_RISCV_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace riscv {
void getRISCVTargetFeatures(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features);
StringRef getRISCVABI(const llvm::opt::ArgList &Args,
const llvm::Triple &Triple);
StringRef getRISCVArch(const llvm::opt::ArgList &Args,
const llvm::Triple &Triple);
}
}
}
}

#endif
