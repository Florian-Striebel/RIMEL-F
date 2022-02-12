







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_LINUX_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_LINUX_H

#include "Gnu.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Linux : public Generic_ELF {
public:
Linux(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

bool HasNativeLLVMSupport() const override;

std::string getMultiarchTriple(const Driver &D,
const llvm::Triple &TargetTriple,
StringRef SysRoot) const override;

void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void addLibStdCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddCudaIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddHIPIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddIAMCUIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
RuntimeLibType GetDefaultRuntimeLibType() const override;
CXXStdlibType GetDefaultCXXStdlibType() const override;
bool
IsAArch64OutlineAtomicsDefault(const llvm::opt::ArgList &Args) const override;
bool isPIEDefault() const override;
bool isNoExecStackDefault() const override;
bool IsMathErrnoDefault() const override;
SanitizerMask getSupportedSanitizers() const override;
void addProfileRTLibs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;
std::string computeSysRoot() const override;

std::string getDynamicLinker(const llvm::opt::ArgList &Args) const override;

void addExtraOpts(llvm::opt::ArgStringList &CmdArgs) const override;

std::vector<std::string> ExtraOpts;

llvm::DenormalMode getDefaultDenormalModeForType(
const llvm::opt::ArgList &DriverArgs, const JobAction &JA,
const llvm::fltSemantics *FPType = nullptr) const override;

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;
Tool *buildStaticLibTool() const override;
};

}
}
}

#endif
