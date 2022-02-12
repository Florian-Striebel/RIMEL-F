







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_SPARC_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_SPARC_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace sparc {

enum class FloatABI {
Invalid,
Soft,
Hard,
};

FloatABI getSparcFloatABI(const Driver &D, const llvm::opt::ArgList &Args);

void getSparcTargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features);
const char *getSparcAsmModeForCPU(llvm::StringRef Name,
const llvm::Triple &Triple);

}
}
}
}

#endif
