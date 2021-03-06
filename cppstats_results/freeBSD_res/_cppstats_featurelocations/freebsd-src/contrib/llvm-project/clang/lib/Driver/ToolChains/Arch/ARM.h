







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_ARM_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_ARM_H

#include "clang/Driver/ToolChain.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/TargetParser.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace arm {

std::string getARMTargetCPU(StringRef CPU, llvm::StringRef Arch,
const llvm::Triple &Triple);
const std::string getARMArch(llvm::StringRef Arch, const llvm::Triple &Triple);
StringRef getARMCPUForMArch(llvm::StringRef Arch, const llvm::Triple &Triple);
llvm::ARM::ArchKind getLLVMArchKindForARM(StringRef CPU, StringRef Arch,
const llvm::Triple &Triple);
StringRef getLLVMArchSuffixForARM(llvm::StringRef CPU, llvm::StringRef Arch,
const llvm::Triple &Triple);

void appendBE8LinkFlag(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs,
const llvm::Triple &Triple);
enum class ReadTPMode {
Invalid,
Soft,
Cp15,
};

enum class FloatABI {
Invalid,
Soft,
SoftFP,
Hard,
};

FloatABI getDefaultFloatABI(const llvm::Triple &Triple);
FloatABI getARMFloatABI(const ToolChain &TC, const llvm::opt::ArgList &Args);
FloatABI getARMFloatABI(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
void setFloatABIInTriple(const Driver &D, const llvm::opt::ArgList &Args,
llvm::Triple &triple);
ReadTPMode getReadTPMode(const Driver &D, const llvm::opt::ArgList &Args);
void setArchNameInTriple(const Driver &D, const llvm::opt::ArgList &Args,
types::ID InputType, llvm::Triple &Triple);

bool useAAPCSForMachO(const llvm::Triple &T);
void getARMArchCPUFromArgs(const llvm::opt::ArgList &Args,
llvm::StringRef &Arch, llvm::StringRef &CPU,
bool FromAs = false);
void getARMTargetFeatures(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs,
std::vector<llvm::StringRef> &Features, bool ForAS);
int getARMSubArchVersionNumber(const llvm::Triple &Triple);
bool isARMMProfile(const llvm::Triple &Triple);
bool isARMAProfile(const llvm::Triple &Triple);

}
}
}
}

#endif
