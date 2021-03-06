







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AMDGPU_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AMDGPU_H

#include "Gnu.h"
#include "ROCm.h"
#include "clang/Basic/TargetID.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/TargetParser.h"

#include <map>

namespace clang {
namespace driver {

namespace tools {
namespace amdgpu {

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("amdgpu::Linker", "ld.lld", TC) {}
bool isLinkJob() const override { return true; }
bool hasIntegratedCPP() const override { return false; }
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

void getAMDGPUTargetFeatures(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args,
std::vector<StringRef> &Features);

}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY AMDGPUToolChain : public Generic_ELF {
protected:
const std::map<options::ID, const StringRef> OptionsDefault;

Tool *buildLinker() const override;
const StringRef getOptionDefault(options::ID OptID) const {
auto opt = OptionsDefault.find(OptID);
assert(opt != OptionsDefault.end() && "No Default for Option");
return opt->second;
}

public:
AMDGPUToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
unsigned GetDefaultDwarfVersion() const override { return 4; }
bool IsIntegratedAssemblerDefault() const override { return true; }
bool IsMathErrnoDefault() const override { return false; }

bool useIntegratedAs() const override { return true; }
bool isCrossCompiling() const override { return true; }
bool isPICDefault() const override { return false; }
bool isPIEDefault() const override { return false; }
bool isPICDefaultForced() const override { return false; }
bool SupportsProfiling() const override { return false; }

llvm::opt::DerivedArgList *
TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind) const override;

void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;



static bool getDefaultDenormsAreZeroForTarget(llvm::AMDGPU::GPUKind GPUKind);

llvm::DenormalMode getDefaultDenormalModeForType(
const llvm::opt::ArgList &DriverArgs, const JobAction &JA,
const llvm::fltSemantics *FPType = nullptr) const override;

static bool isWave64(const llvm::opt::ArgList &DriverArgs,
llvm::AMDGPU::GPUKind Kind);

bool HasNativeLLVMSupport() const override {
return true;
}


const char *getDefaultLinker() const override { return "ld.lld"; }


bool shouldSkipArgument(const llvm::opt::Arg *Arg) const;



llvm::Error getSystemGPUArch(const llvm::opt::ArgList &Args,
std::string &GPUArch) const;

protected:

virtual void checkTargetID(const llvm::opt::ArgList &DriverArgs) const;


struct ParsedTargetIDType {
Optional<std::string> OptionalTargetID;
Optional<std::string> OptionalGPUArch;
Optional<llvm::StringMap<bool>> OptionalFeatures;
};



ParsedTargetIDType
getParsedTargetID(const llvm::opt::ArgList &DriverArgs) const;


StringRef getGPUArch(const llvm::opt::ArgList &DriverArgs) const;

llvm::Error detectSystemGPUs(const llvm::opt::ArgList &Args,
SmallVector<std::string, 1> &GPUArchs) const;
};

class LLVM_LIBRARY_VISIBILITY ROCMToolChain : public AMDGPUToolChain {
public:
ROCMToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
void
addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;


llvm::SmallVector<std::string, 12>
getCommonDeviceLibNames(const llvm::opt::ArgList &DriverArgs,
const std::string &GPUArch) const;
};

}
}
}

#endif
