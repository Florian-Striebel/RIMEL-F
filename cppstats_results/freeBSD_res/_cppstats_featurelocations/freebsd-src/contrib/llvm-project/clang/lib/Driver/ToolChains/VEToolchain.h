







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_VE_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_VE_H

#include "Linux.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY VEToolChain : public Linux {
public:
VEToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;

public:
bool IsIntegratedAssemblerDefault() const override { return true; }
bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;
bool SupportsProfiling() const override;
bool hasBlocksRuntime() const override;
void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void
addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;
void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

llvm::ExceptionHandling
GetExceptionModel(const llvm::opt::ArgList &Args) const override;

CXXStdlibType
GetCXXStdlibType(const llvm::opt::ArgList &Args) const override {
return ToolChain::CST_Libcxx;
}

RuntimeLibType GetDefaultRuntimeLibType() const override {
return ToolChain::RLT_CompilerRT;
}

const char *getDefaultLinker() const override { return "nld"; }
};

}
}
}

#endif
