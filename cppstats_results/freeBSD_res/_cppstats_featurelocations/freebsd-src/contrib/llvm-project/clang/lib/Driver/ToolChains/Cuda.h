







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CUDA_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_CUDA_H

#include "clang/Basic/Cuda.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Multilib.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include "llvm/ADT/Optional.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/VersionTuple.h"
#include <bitset>
#include <set>
#include <vector>

namespace clang {
namespace driver {


class CudaInstallationDetector {
private:
const Driver &D;
bool IsValid = false;
CudaVersion Version = CudaVersion::UNKNOWN;
std::string DetectedVersion;
bool DetectedVersionIsNotSupported = false;
std::string InstallPath;
std::string BinPath;
std::string LibPath;
std::string LibDevicePath;
std::string IncludePath;
llvm::StringMap<std::string> LibDeviceMap;



mutable std::bitset<(int)CudaArch::LAST> ArchsWithBadVersion;

public:
CudaInstallationDetector(const Driver &D, const llvm::Triple &HostTriple,
const llvm::opt::ArgList &Args);

void AddCudaIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;





void CheckCudaVersionSupportsArch(CudaArch Arch) const;


bool isValid() const { return IsValid; }

void print(raw_ostream &OS) const;


CudaVersion version() const { return Version; }

StringRef getInstallPath() const { return InstallPath; }

StringRef getBinPath() const { return BinPath; }

StringRef getIncludePath() const { return IncludePath; }

StringRef getLibPath() const { return LibPath; }

StringRef getLibDevicePath() const { return LibDevicePath; }

std::string getLibDeviceFile(StringRef Gpu) const {
return LibDeviceMap.lookup(Gpu);
}
void WarnIfUnsupportedVersion();
};

namespace tools {
namespace NVPTX {


class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC) : Tool("NVPTX::Assembler", "ptxas", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};



class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("NVPTX::Linker", "fatbinary", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY OpenMPLinker : public Tool {
public:
OpenMPLinker(const ToolChain &TC)
: Tool("NVPTX::OpenMPLinker", "nvlink", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY CudaToolChain : public ToolChain {
public:
CudaToolChain(const Driver &D, const llvm::Triple &Triple,
const ToolChain &HostTC, const llvm::opt::ArgList &Args,
const Action::OffloadKind OK);

const llvm::Triple *getAuxTriple() const override {
return &HostTC.getTriple();
}

std::string getInputFilename(const InputInfo &Input) const override;

llvm::opt::DerivedArgList *
TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind) const override;
void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;

llvm::DenormalMode getDefaultDenormalModeForType(
const llvm::opt::ArgList &DriverArgs, const JobAction &JA,
const llvm::fltSemantics *FPType = nullptr) const override;



bool useIntegratedAs() const override { return false; }
bool isCrossCompiling() const override { return true; }
bool isPICDefault() const override { return false; }
bool isPIEDefault() const override { return false; }
bool isPICDefaultForced() const override { return false; }
bool SupportsProfiling() const override { return false; }
bool supportsDebugInfoOption(const llvm::opt::Arg *A) const override;
void adjustDebugInfoKind(codegenoptions::DebugInfoKind &DebugInfoKind,
const llvm::opt::ArgList &Args) const override;
bool IsMathErrnoDefault() const override { return false; }

void AddCudaIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void addClangWarningOptions(llvm::opt::ArgStringList &CC1Args) const override;
CXXStdlibType GetCXXStdlibType(const llvm::opt::ArgList &Args) const override;
void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CC1Args) const override;
void AddIAMCUIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

SanitizerMask getSupportedSanitizers() const override;

VersionTuple
computeMSVCVersion(const Driver *D,
const llvm::opt::ArgList &Args) const override;

unsigned GetDefaultDwarfVersion() const override { return 2; }

unsigned getMaxDwarfVersion() const override { return 2; }

const ToolChain &HostTC;
CudaInstallationDetector CudaInstallation;

protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;

private:
const Action::OffloadKind OK;
};

}
}
}

#endif
