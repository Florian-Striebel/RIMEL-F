







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_GNU_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_GNU_H

#include "Cuda.h"
#include "ROCm.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include <set>

namespace clang {
namespace driver {

struct DetectedMultilibs {

MultilibSet Multilibs;


Multilib SelectedMultilib;



llvm::Optional<Multilib> BiarchSibling;
};

bool findMIPSMultilibs(const Driver &D, const llvm::Triple &TargetTriple,
StringRef Path, const llvm::opt::ArgList &Args,
DetectedMultilibs &Result);

namespace tools {


namespace gnutools {
class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC) : Tool("GNU::Assembler", "assembler", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("GNU::Linker", "linker", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY StaticLibTool : public Tool {
public:
StaticLibTool(const ToolChain &TC)
: Tool("GNU::StaticLibTool", "static-lib-linker", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};
}


namespace gcc {
class LLVM_LIBRARY_VISIBILITY Common : public Tool {
public:
Common(const char *Name, const char *ShortName, const ToolChain &TC)
: Tool(Name, ShortName, TC) {}




bool hasIntegratedAssembler() const override { return true; }
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;



virtual void RenderExtraToolArgs(const JobAction &JA,
llvm::opt::ArgStringList &CmdArgs) const = 0;
};

class LLVM_LIBRARY_VISIBILITY Preprocessor : public Common {
public:
Preprocessor(const ToolChain &TC)
: Common("gcc::Preprocessor", "gcc preprocessor", TC) {}

bool hasGoodDiagnostics() const override { return true; }
bool hasIntegratedCPP() const override { return false; }

void RenderExtraToolArgs(const JobAction &JA,
llvm::opt::ArgStringList &CmdArgs) const override;
};

class LLVM_LIBRARY_VISIBILITY Compiler : public Common {
public:
Compiler(const ToolChain &TC) : Common("gcc::Compiler", "gcc frontend", TC) {}

bool hasGoodDiagnostics() const override { return true; }
bool hasIntegratedCPP() const override { return true; }

void RenderExtraToolArgs(const JobAction &JA,
llvm::opt::ArgStringList &CmdArgs) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Common {
public:
Linker(const ToolChain &TC) : Common("gcc::Linker", "linker (via gcc)", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void RenderExtraToolArgs(const JobAction &JA,
llvm::opt::ArgStringList &CmdArgs) const override;
};
}
}

namespace toolchains {




class LLVM_LIBRARY_VISIBILITY Generic_GCC : public ToolChain {
public:















struct GCCVersion {

std::string Text;


int Major, Minor, Patch;


std::string MajorStr, MinorStr;


std::string PatchSuffix;

static GCCVersion Parse(StringRef VersionText);
bool isOlderThan(int RHSMajor, int RHSMinor, int RHSPatch,
StringRef RHSPatchSuffix = StringRef()) const;
bool operator<(const GCCVersion &RHS) const {
return isOlderThan(RHS.Major, RHS.Minor, RHS.Patch, RHS.PatchSuffix);
}
bool operator>(const GCCVersion &RHS) const { return RHS < *this; }
bool operator<=(const GCCVersion &RHS) const { return !(*this > RHS); }
bool operator>=(const GCCVersion &RHS) const { return !(*this < RHS); }
};







class GCCInstallationDetector {
bool IsValid;
llvm::Triple GCCTriple;
const Driver &D;


std::string GCCInstallPath;
std::string GCCParentLibPath;


Multilib SelectedMultilib;


llvm::Optional<Multilib> BiarchSibling;

GCCVersion Version;



std::set<std::string> CandidateGCCInstallPaths;


MultilibSet Multilibs;


const std::string GentooConfigDir = "/etc/env.d/gcc";

public:
explicit GCCInstallationDetector(const Driver &D) : IsValid(false), D(D) {}
void init(const llvm::Triple &TargetTriple, const llvm::opt::ArgList &Args,
ArrayRef<std::string> ExtraTripleAliases = None);


bool isValid() const { return IsValid; }


const llvm::Triple &getTriple() const { return GCCTriple; }


StringRef getInstallPath() const { return GCCInstallPath; }


StringRef getParentLibPath() const { return GCCParentLibPath; }


const Multilib &getMultilib() const { return SelectedMultilib; }


const MultilibSet &getMultilibs() const { return Multilibs; }



bool getBiarchSibling(Multilib &M) const;


const GCCVersion &getVersion() const { return Version; }


void print(raw_ostream &OS) const;

private:
static void
CollectLibDirsAndTriples(const llvm::Triple &TargetTriple,
const llvm::Triple &BiarchTriple,
SmallVectorImpl<StringRef> &LibDirs,
SmallVectorImpl<StringRef> &TripleAliases,
SmallVectorImpl<StringRef> &BiarchLibDirs,
SmallVectorImpl<StringRef> &BiarchTripleAliases);

void AddDefaultGCCPrefixes(const llvm::Triple &TargetTriple,
SmallVectorImpl<std::string> &Prefixes,
StringRef SysRoot);

bool ScanGCCForMultilibs(const llvm::Triple &TargetTriple,
const llvm::opt::ArgList &Args,
StringRef Path,
bool NeedsBiarchSuffix = false);

void ScanLibDirForGCCTriple(const llvm::Triple &TargetArch,
const llvm::opt::ArgList &Args,
const std::string &LibDir,
StringRef CandidateTriple,
bool NeedsBiarchSuffix, bool GCCDirExists,
bool GCCCrossDirExists);

bool ScanGentooConfigs(const llvm::Triple &TargetTriple,
const llvm::opt::ArgList &Args,
const SmallVectorImpl<StringRef> &CandidateTriples,
const SmallVectorImpl<StringRef> &BiarchTriples);

bool ScanGentooGccConfig(const llvm::Triple &TargetTriple,
const llvm::opt::ArgList &Args,
StringRef CandidateTriple,
bool NeedsBiarchSuffix = false);
};

protected:
GCCInstallationDetector GCCInstallation;
CudaInstallationDetector CudaInstallation;
RocmInstallationDetector RocmInstallation;

public:
Generic_GCC(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
~Generic_GCC() override;

void printVerboseInfo(raw_ostream &OS) const override;

bool IsUnwindTablesDefault(const llvm::opt::ArgList &Args) const override;
bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;
bool IsIntegratedAssemblerDefault() const override;
llvm::opt::DerivedArgList *
TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind) const override;

protected:
Tool *getTool(Action::ActionClass AC) const override;
Tool *buildAssembler() const override;
Tool *buildLinker() const override;





bool isTarget64Bit() const { return getTriple().isArch64Bit(); }


bool isTarget32Bit() const { return getTriple().isArch32Bit(); }

void PushPPaths(ToolChain::path_list &PPaths);
void AddMultilibPaths(const Driver &D, const std::string &SysRoot,
const std::string &OSLibDir,
const std::string &MultiarchTriple,
path_list &Paths);
void AddMultiarchPaths(const Driver &D, const std::string &SysRoot,
const std::string &OSLibDir, path_list &Paths);
void AddMultilibIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;



void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

virtual void
addLibCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;
virtual void
addLibStdCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;

bool addGCCLibStdCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
StringRef DebianMultiarch) const;

bool addLibStdCXXIncludePaths(Twine IncludeDir, StringRef Triple,
Twine IncludeSuffix,
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
bool DetectDebian = false) const;



private:
mutable std::unique_ptr<tools::gcc::Preprocessor> Preprocess;
mutable std::unique_ptr<tools::gcc::Compiler> Compile;
};

class LLVM_LIBRARY_VISIBILITY Generic_ELF : public Generic_GCC {
virtual void anchor();

public:
Generic_ELF(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args)
: Generic_GCC(D, Triple, Args) {}

void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;

virtual std::string getDynamicLinker(const llvm::opt::ArgList &Args) const {
return {};
}

virtual void addExtraOpts(llvm::opt::ArgStringList &CmdArgs) const {}
};

}
}
}

#endif
