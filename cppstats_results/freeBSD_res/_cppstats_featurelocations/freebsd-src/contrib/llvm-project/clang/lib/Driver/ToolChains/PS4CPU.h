







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_PS4CPU_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_PS4CPU_H

#include "Gnu.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

namespace PS4cpu {

void addProfileRTArgs(const ToolChain &TC, const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs);

void addSanitizerArgs(const ToolChain &TC, llvm::opt::ArgStringList &CmdArgs);

class LLVM_LIBRARY_VISIBILITY Assemble : public Tool {
public:
Assemble(const ToolChain &TC) : Tool("PS4cpu::Assemble", "assembler", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output,
const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Link : public Tool {
public:
Link(const ToolChain &TC) : Tool("PS4cpu::Link", "linker", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output,
const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};
}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY PS4CPU : public Generic_ELF {
public:
PS4CPU(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);


void addLibCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override {}
void addLibStdCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override {}

bool IsMathErrnoDefault() const override { return false; }
bool IsObjCNonFragileABIDefault() const override { return true; }
bool HasNativeLLVMSupport() const override;
bool isPICDefault() const override;

LangOptions::StackProtectorMode
GetDefaultStackProtectorLevel(bool KernelOrKext) const override {
return LangOptions::SSPStrong;
}

llvm::DebuggerKind getDefaultDebuggerTuning() const override {
return llvm::DebuggerKind::SCE;
}

SanitizerMask getSupportedSanitizers() const override;



bool canSplitThinLTOUnit() const override { return false; }

void addClangTargetOptions(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadingKind) const override;

llvm::DenormalMode getDefaultDenormalModeForType(
const llvm::opt::ArgList &DriverArgs, const JobAction &JA,
const llvm::fltSemantics *FPType) const override {

return llvm::DenormalMode::getPreserveSign();
}

bool useRelaxRelocations() const override { return true; }

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;
};

}
}
}

#endif
