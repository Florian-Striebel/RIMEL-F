







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_WEBASSEMBLY_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_WEBASSEMBLY_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {
namespace wasm {

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
explicit Linker(const ToolChain &TC) : Tool("wasm::Linker", "linker", TC) {}
bool isLinkJob() const override { return true; }
bool hasIntegratedCPP() const override { return false; }
std::string getLinkerPath(const llvm::opt::ArgList &Args) const;
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY WebAssembly final : public ToolChain {
public:
WebAssembly(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

private:
bool IsMathErrnoDefault() const override;
bool IsObjCNonFragileABIDefault() const override;
bool UseObjCMixedDispatch() const override;
bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;
bool IsIntegratedAssemblerDefault() const override;
bool hasBlocksRuntime() const override;
bool SupportsProfiling() const override;
bool HasNativeLLVMSupport() const override;
void
addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;
RuntimeLibType GetDefaultRuntimeLibType() const override;
CXXStdlibType GetCXXStdlibType(const llvm::opt::ArgList &Args) const override;
void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;
SanitizerMask getSupportedSanitizers() const override;

const char *getDefaultLinker() const override { return "wasm-ld"; }

Tool *buildLinker() const override;

std::string getMultiarchTriple(const Driver &D,
const llvm::Triple &TargetTriple,
StringRef SysRoot) const override;
};

}
}
}

#endif
