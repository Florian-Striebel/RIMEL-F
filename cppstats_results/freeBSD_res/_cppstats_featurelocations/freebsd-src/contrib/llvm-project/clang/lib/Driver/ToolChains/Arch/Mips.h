







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_MIPS_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_MIPS_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {

namespace mips {
typedef enum { Legacy = 1, Std2008 = 2 } IEEE754Standard;

enum class FloatABI {
Invalid,
Soft,
Hard,
};

IEEE754Standard getIEEE754Standard(StringRef &CPU);
bool hasCompactBranches(StringRef &CPU);
void getMipsCPUAndABI(const llvm::opt::ArgList &Args,
const llvm::Triple &Triple, StringRef &CPUName,
StringRef &ABIName);
void getMIPSTargetFeatures(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args,
std::vector<StringRef> &Features);
StringRef getGnuCompatibleMipsABIName(StringRef ABI);
mips::FloatABI getMipsFloatABI(const Driver &D, const llvm::opt::ArgList &Args,
const llvm::Triple &Triple);
std::string getMipsABILibSuffix(const llvm::opt::ArgList &Args,
const llvm::Triple &Triple);
bool hasMipsAbiArg(const llvm::opt::ArgList &Args, const char *Value);
bool isUCLibc(const llvm::opt::ArgList &Args);
bool isNaN2008(const llvm::opt::ArgList &Args, const llvm::Triple &Triple);
bool isFP64ADefault(const llvm::Triple &Triple, StringRef CPUName);
bool isFPXXDefault(const llvm::Triple &Triple, StringRef CPUName,
StringRef ABIName, mips::FloatABI FloatABI);
bool shouldUseFPXX(const llvm::opt::ArgList &Args, const llvm::Triple &Triple,
StringRef CPUName, StringRef ABIName,
mips::FloatABI FloatABI);
bool supportsIndirectJumpHazardBarrier(StringRef &CPU);

}
}
}
}

#endif
