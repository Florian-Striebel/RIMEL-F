







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CLOUDABI_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CLOUDABI_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {


namespace cloudabi {
class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("cloudabi::Linker", "linker", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};
}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY CloudABI : public Generic_ELF {
public:
CloudABI(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
bool HasNativeLLVMSupport() const override { return true; }

bool IsMathErrnoDefault() const override { return false; }
bool IsObjCNonFragileABIDefault() const override { return true; }

CXXStdlibType
GetCXXStdlibType(const llvm::opt::ArgList &Args) const override {
return ToolChain::CST_Libcxx;
}
void addLibCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

bool isPIEDefault() const override;
SanitizerMask getSupportedSanitizers() const override;
SanitizerMask getDefaultSanitizers() const override;

protected:
Tool *buildLinker() const override;
};

}
}
}

#endif
