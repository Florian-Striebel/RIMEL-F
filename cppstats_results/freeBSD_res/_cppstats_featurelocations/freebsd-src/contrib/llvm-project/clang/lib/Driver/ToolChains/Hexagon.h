







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_HEXAGON_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_HEXAGON_H

#include "Linux.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {
namespace hexagon {



class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
Assembler(const ToolChain &TC)
: Tool("hexagon::Assembler", "hexagon-as", TC) {}

bool hasIntegratedCPP() const override { return false; }

void RenderExtraToolArgs(const JobAction &JA,
llvm::opt::ArgStringList &CmdArgs) const;
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
Linker(const ToolChain &TC) : Tool("hexagon::Linker", "hexagon-ld", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

virtual void RenderExtraToolArgs(const JobAction &JA,
llvm::opt::ArgStringList &CmdArgs) const;
void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

void getHexagonTargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
std::vector<StringRef> &Features);

}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY HexagonToolChain : public Linux {
protected:
GCCVersion GCCLibAndIncVersion;
Tool *buildAssembler() const override;
Tool *buildLinker() const override;

unsigned getOptimizationLevel(const llvm::opt::ArgList &DriverArgs) const;

public:
HexagonToolChain(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
~HexagonToolChain() override;

void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;
void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void addLibStdCxxIncludePaths(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void addLibCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

const char *getDefaultLinker() const override {
return getTriple().isMusl() ? "ld.lld" : "hexagon-link";
}

CXXStdlibType GetCXXStdlibType(const llvm::opt::ArgList &Args) const override;

void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

StringRef GetGCCLibAndIncVersion() const { return GCCLibAndIncVersion.Text; }
bool IsIntegratedAssemblerDefault() const override {
return true;
}

std::string getHexagonTargetDir(
const std::string &InstalledDir,
const SmallVectorImpl<std::string> &PrefixDirs) const;
void getHexagonLibraryPaths(const llvm::opt::ArgList &Args,
ToolChain::path_list &LibPaths) const;

static bool isAutoHVXEnabled(const llvm::opt::ArgList &Args);
static const StringRef GetDefaultCPU();
static const StringRef GetTargetCPUVersion(const llvm::opt::ArgList &Args);

static Optional<unsigned> getSmallDataThreshold(
const llvm::opt::ArgList &Args);
};

}
}
}

#endif
