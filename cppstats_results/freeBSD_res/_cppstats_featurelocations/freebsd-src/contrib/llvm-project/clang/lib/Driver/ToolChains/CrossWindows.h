







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CROSSWINDOWS_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CROSSWINDOWS_H

#include "Cuda.h"
#include "Gnu.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

namespace CrossWindows {
class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC) : Tool("CrossWindows::Assembler", "as", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("CrossWindows::Linker", "ld", TC) {}

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

class LLVM_LIBRARY_VISIBILITY CrossWindowsToolChain : public Generic_GCC {
public:
CrossWindowsToolChain(const Driver &D, const llvm::Triple &T,
const llvm::opt::ArgList &Args);

bool IsIntegratedAssemblerDefault() const override { return true; }
bool IsUnwindTablesDefault(const llvm::opt::ArgList &Args) const override;
bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;

LangOptions::StackProtectorMode
GetDefaultStackProtectorLevel(bool KernelOrKext) const override {
return LangOptions::SSPOff;
}

void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

SanitizerMask getSupportedSanitizers() const override;

protected:
Tool *buildLinker() const override;
Tool *buildAssembler() const override;
};

}
}
}

#endif
