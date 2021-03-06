







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_MSVC_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_MSVC_H

#include "AMDGPU.h"
#include "Cuda.h"
#include "clang/Basic/DebugInfoOptions.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {


namespace visualstudio {
class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("visualstudio::Linker", "linker", TC) {}

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

class LLVM_LIBRARY_VISIBILITY MSVCToolChain : public ToolChain {
public:
MSVCToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);

llvm::opt::DerivedArgList *
TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind) const override;

bool IsIntegratedAssemblerDefault() const override;
bool IsUnwindTablesDefault(const llvm::opt::ArgList &Args) const override;
bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;




codegenoptions::DebugInfoFormat getDefaultDebugFormat() const override {
return getTriple().isOSBinFormatMachO() ? codegenoptions::DIF_DWARF
: codegenoptions::DIF_CodeView;
}



llvm::DebuggerKind getDefaultDebuggerTuning() const override {
return llvm::DebuggerKind::Default;
}

enum class SubDirectoryType {
Bin,
Include,
Lib,
};
std::string getSubDirectoryPath(SubDirectoryType Type,
llvm::StringRef SubdirParent,
llvm::Triple::ArchType TargetArch) const;



std::string getSubDirectoryPath(SubDirectoryType Type,
llvm::StringRef SubdirParent = "") const {
return getSubDirectoryPath(Type, SubdirParent, getArch());
}

enum class ToolsetLayout {
OlderVS,
VS2017OrNewer,
DevDivInternal,
};
bool getIsVS2017OrNewer() const { return VSLayout == ToolsetLayout::VS2017OrNewer; }

void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void AddCudaIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void AddHIPIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

bool getWindowsSDKLibraryPath(
const llvm::opt::ArgList &Args, std::string &path) const;
bool getUniversalCRTLibraryPath(const llvm::opt::ArgList &Args,
std::string &path) const;
bool useUniversalCRT() const;
VersionTuple
computeMSVCVersion(const Driver *D,
const llvm::opt::ArgList &Args) const override;

std::string ComputeEffectiveClangTriple(const llvm::opt::ArgList &Args,
types::ID InputType) const override;
SanitizerMask getSupportedSanitizers() const override;

void printVerboseInfo(raw_ostream &OS) const override;

bool FoundMSVCInstall() const { return !VCToolChainPath.empty(); }

void
addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;

protected:
void AddSystemIncludeWithSubfolder(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
const std::string &folder,
const Twine &subfolder1,
const Twine &subfolder2 = "",
const Twine &subfolder3 = "") const;

Tool *buildLinker() const override;
Tool *buildAssembler() const override;
private:
std::string VCToolChainPath;
ToolsetLayout VSLayout = ToolsetLayout::OlderVS;
CudaInstallationDetector CudaInstallation;
RocmInstallationDetector RocmInstallation;
};

}
}
}

#endif
