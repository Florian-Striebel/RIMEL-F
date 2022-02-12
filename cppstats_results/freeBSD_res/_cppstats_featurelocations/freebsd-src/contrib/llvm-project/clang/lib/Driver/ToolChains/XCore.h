







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_XCORE_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_XCORE_H

#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

namespace XCore {



class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC) : Tool("XCore::Assembler", "XCore-as", TC) {}

bool hasIntegratedCPP() const override { return false; }
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("XCore::Linker", "XCore-ld", TC) {}

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

class LLVM_LIBRARY_VISIBILITY XCoreToolChain : public ToolChain {
public:
XCoreToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;

public:
bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;
bool SupportsProfiling() const override;
bool hasBlocksRuntime() const override;
void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;
void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;
};

}
}
}

#endif
