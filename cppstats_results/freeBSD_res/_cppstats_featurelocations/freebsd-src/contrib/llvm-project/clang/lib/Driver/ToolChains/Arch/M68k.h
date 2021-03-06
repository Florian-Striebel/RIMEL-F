







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_M680X0_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_M680X0_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace m68k {

enum class FloatABI {
Invalid,
Soft,
Hard,
};

FloatABI getM68kFloatABI(const Driver &D, const llvm::opt::ArgList &Args);

std::string getM68kTargetCPU(const llvm::opt::ArgList &Args);

void getM68kTargetFeatures(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features);

}
}
}
}

#endif
