







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_SYSTEMZ_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_SYSTEMZ_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace systemz {

enum class FloatABI {
Soft,
Hard,
};

FloatABI getSystemZFloatABI(const Driver &D, const llvm::opt::ArgList &Args);

std::string getSystemZTargetCPU(const llvm::opt::ArgList &Args);

void getSystemZTargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
std::vector<llvm::StringRef> &Features);

}
}
}
}

#endif
