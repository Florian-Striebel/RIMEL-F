







#if !defined(LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_DARWIN_H)
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_DARWIN_H

#include "Cuda.h"
#include "ROCm.h"
#include "clang/Basic/DarwinSDKInfo.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include "clang/Driver/XRayArgs.h"

namespace clang {
namespace driver {

namespace toolchains {
class MachO;
}

namespace tools {

namespace darwin {
llvm::Triple::ArchType getArchTypeForMachOArchName(StringRef Str);
void setTripleTypeForMachOArchName(llvm::Triple &T, StringRef Str);

class LLVM_LIBRARY_VISIBILITY MachOTool : public Tool {
virtual void anchor();

protected:
void AddMachOArch(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;

const toolchains::MachO &getMachOToolChain() const {
return reinterpret_cast<const toolchains::MachO &>(getToolChain());
}

public:
MachOTool(const char *Name, const char *ShortName, const ToolChain &TC)
: Tool(Name, ShortName, TC) {}
};

class LLVM_LIBRARY_VISIBILITY Assembler : public MachOTool {
public:
Assembler(const ToolChain &TC)
: MachOTool("darwin::Assembler", "assembler", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public MachOTool {
bool NeedsTempPath(const InputInfoList &Inputs) const;
void AddLinkArgs(Compilation &C, const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs,
const InputInfoList &Inputs, unsigned Version[5],
bool LinkerIsLLD, bool LinkerIsLLDDarwinNew) const;

public:
Linker(const ToolChain &TC) : MachOTool("darwin::Linker", "linker", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isLinkJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Lipo : public MachOTool {
public:
Lipo(const ToolChain &TC) : MachOTool("darwin::Lipo", "lipo", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Dsymutil : public MachOTool {
public:
Dsymutil(const ToolChain &TC)
: MachOTool("darwin::Dsymutil", "dsymutil", TC) {}

bool hasIntegratedCPP() const override { return false; }
bool isDsymutilJob() const override { return true; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY VerifyDebug : public MachOTool {
public:
VerifyDebug(const ToolChain &TC)
: MachOTool("darwin::VerifyDebug", "dwarfdump", TC) {}

bool hasIntegratedCPP() const override { return false; }

void ConstructJob(Compilation &C, const JobAction &JA,
const InputInfo &Output, const InputInfoList &Inputs,
const llvm::opt::ArgList &TCArgs,
const char *LinkingOutput) const override;
};
}
}

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY MachO : public ToolChain {
protected:
Tool *buildAssembler() const override;
Tool *buildLinker() const override;
Tool *getTool(Action::ActionClass AC) const override;

private:
mutable std::unique_ptr<tools::darwin::Lipo> Lipo;
mutable std::unique_ptr<tools::darwin::Dsymutil> Dsymutil;
mutable std::unique_ptr<tools::darwin::VerifyDebug> VerifyDebug;

public:
MachO(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
~MachO() override;






StringRef getMachOArchName(const llvm::opt::ArgList &Args) const;


virtual void AddLinkARCArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const {}





virtual void AddLinkRuntimeLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs,
bool ForceLinkBuiltinRT = false) const;

virtual void addStartObjectFileArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const {
}

virtual void addMinVersionArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const {}

virtual void addPlatformVersionArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const {
}



virtual bool isKernelStatic() const { return false; }


bool isTargetIOSBased() const { return false; }


enum RuntimeLinkOptions : unsigned {

RLO_AlwaysLink = 1 << 0,


RLO_IsEmbedded = 1 << 1,


RLO_AddRPath = 1 << 2,
};


void AddLinkRuntimeLib(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs, StringRef Component,
RuntimeLinkOptions Opts = RuntimeLinkOptions(),
bool IsShared = false) const;



void addProfileRTLibs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override {

}





types::ID LookupTypeForExtension(StringRef Ext) const override;

bool HasNativeLLVMSupport() const override;

llvm::opt::DerivedArgList *
TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind) const override;

bool IsBlocksDefault() const override {


return true;
}
bool IsIntegratedAssemblerDefault() const override {

return true;
}

bool IsMathErrnoDefault() const override { return false; }

bool IsEncodeExtendedBlockSignatureDefault() const override { return true; }

bool IsObjCNonFragileABIDefault() const override {

return getTriple().getArch() != llvm::Triple::x86;
}

bool UseObjCMixedDispatch() const override { return true; }

bool IsUnwindTablesDefault(const llvm::opt::ArgList &Args) const override;

RuntimeLibType GetDefaultRuntimeLibType() const override {
return ToolChain::RLT_CompilerRT;
}

bool isPICDefault() const override;
bool isPIEDefault() const override;
bool isPICDefaultForced() const override;

bool SupportsProfiling() const override;

bool UseDwarfDebugFlags() const override;

llvm::ExceptionHandling
GetExceptionModel(const llvm::opt::ArgList &Args) const override {
return llvm::ExceptionHandling::None;
}

virtual StringRef getOSLibraryNameSuffix(bool IgnoreSim = false) const {
return "";
}



bool canSplitThinLTOUnit() const override { return false; }

};


class LLVM_LIBRARY_VISIBILITY Darwin : public MachO {
public:





mutable bool TargetInitialized;

enum DarwinPlatformKind {
MacOS,
IPhoneOS,
TvOS,
WatchOS,
LastDarwinPlatform = WatchOS
};
enum DarwinEnvironmentKind {
NativeEnvironment,
Simulator,
MacCatalyst,
};

mutable DarwinPlatformKind TargetPlatform;
mutable DarwinEnvironmentKind TargetEnvironment;


mutable VersionTuple TargetVersion;

mutable VersionTuple OSTargetVersion;


mutable Optional<DarwinSDKInfo> SDKInfo;

CudaInstallationDetector CudaInstallation;
RocmInstallationDetector RocmInstallation;

private:
void AddDeploymentTarget(llvm::opt::DerivedArgList &Args) const;

public:
Darwin(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);
~Darwin() override;

std::string ComputeEffectiveClangTriple(const llvm::opt::ArgList &Args,
types::ID InputType) const override;




void addMinVersionArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

void addPlatformVersionArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

void addStartObjectFileArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

bool isKernelStatic() const override {
return (!(isTargetIPhoneOS() && !isIPhoneOSVersionLT(6, 0)) &&
!isTargetWatchOS());
}

void addProfileRTLibs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

protected:






void setTarget(DarwinPlatformKind Platform, DarwinEnvironmentKind Environment,
unsigned Major, unsigned Minor, unsigned Micro,
VersionTuple NativeTargetVersion) const {


if (TargetInitialized && TargetPlatform == Platform &&
TargetEnvironment == Environment &&
(Environment == MacCatalyst ? OSTargetVersion : TargetVersion) ==
VersionTuple(Major, Minor, Micro))
return;

assert(!TargetInitialized && "Target already initialized!");
TargetInitialized = true;
TargetPlatform = Platform;
TargetEnvironment = Environment;
TargetVersion = VersionTuple(Major, Minor, Micro);
if (Environment == Simulator)
const_cast<Darwin *>(this)->setTripleEnvironment(llvm::Triple::Simulator);
else if (Environment == MacCatalyst) {
const_cast<Darwin *>(this)->setTripleEnvironment(llvm::Triple::MacABI);
TargetVersion = NativeTargetVersion;
OSTargetVersion = VersionTuple(Major, Minor, Micro);
}
}

public:
bool isTargetIPhoneOS() const {
assert(TargetInitialized && "Target not initialized!");
return (TargetPlatform == IPhoneOS || TargetPlatform == TvOS) &&
TargetEnvironment == NativeEnvironment;
}

bool isTargetIOSSimulator() const {
assert(TargetInitialized && "Target not initialized!");
return (TargetPlatform == IPhoneOS || TargetPlatform == TvOS) &&
TargetEnvironment == Simulator;
}

bool isTargetIOSBased() const {
assert(TargetInitialized && "Target not initialized!");
return isTargetIPhoneOS() || isTargetIOSSimulator();
}

bool isTargetTvOS() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == TvOS && TargetEnvironment == NativeEnvironment;
}

bool isTargetTvOSSimulator() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == TvOS && TargetEnvironment == Simulator;
}

bool isTargetTvOSBased() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == TvOS;
}

bool isTargetWatchOS() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == WatchOS && TargetEnvironment == NativeEnvironment;
}

bool isTargetWatchOSSimulator() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == WatchOS && TargetEnvironment == Simulator;
}

bool isTargetWatchOSBased() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == WatchOS;
}

bool isTargetMacCatalyst() const {
return TargetPlatform == IPhoneOS && TargetEnvironment == MacCatalyst;
}

bool isTargetMacOS() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == MacOS;
}

bool isTargetMacOSBased() const {
assert(TargetInitialized && "Target not initialized!");
return TargetPlatform == MacOS || isTargetMacCatalyst();
}

bool isTargetAppleSiliconMac() const {
assert(TargetInitialized && "Target not initialized!");
return isTargetMacOSBased() && getArch() == llvm::Triple::aarch64;
}

bool isTargetInitialized() const { return TargetInitialized; }





VersionTuple getTripleTargetVersion() const {
assert(TargetInitialized && "Target not initialized!");
return isTargetMacCatalyst() ? OSTargetVersion : TargetVersion;
}

bool isIPhoneOSVersionLT(unsigned V0, unsigned V1 = 0,
unsigned V2 = 0) const {
assert(isTargetIOSBased() && "Unexpected call for non iOS target!");
return TargetVersion < VersionTuple(V0, V1, V2);
}





bool isMacosxVersionLT(unsigned V0, unsigned V1 = 0, unsigned V2 = 0) const {
assert(isTargetMacOSBased() &&
(getTriple().isMacOSX() || getTriple().isMacCatalystEnvironment()) &&
"Unexpected call for non OS X target!");


VersionTuple MinVers =
llvm::Triple(getTriple().getArchName(), "apple", "macos")
.getMinimumSupportedOSVersion();
return (!MinVers.empty() && MinVers > TargetVersion
? MinVers
: TargetVersion) < VersionTuple(V0, V1, V2);
}

protected:



bool isAlignedAllocationUnavailable() const;

void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const override;

StringRef getPlatformFamily() const;
StringRef getOSLibraryNameSuffix(bool IgnoreSim = false) const override;

public:
static StringRef getSDKName(StringRef isysroot);








bool isCrossCompiling() const override { return false; }

llvm::opt::DerivedArgList *
TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind) const override;

CXXStdlibType GetDefaultCXXStdlibType() const override;
ObjCRuntime getDefaultObjCRuntime(bool isNonFragile) const override;
bool hasBlocksRuntime() const override;

void AddCudaIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;
void AddHIPIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

bool UseObjCMixedDispatch() const override {



return !(isTargetMacOSBased() && isMacosxVersionLT(10, 6));
}

LangOptions::StackProtectorMode
GetDefaultStackProtectorLevel(bool KernelOrKext) const override {


if (isTargetIOSBased() || isTargetWatchOSBased())
return LangOptions::SSPOn;
else if (isTargetMacOSBased() && !isMacosxVersionLT(10, 6))
return LangOptions::SSPOn;
else if (isTargetMacOSBased() && !isMacosxVersionLT(10, 5) && !KernelOrKext)
return LangOptions::SSPOn;

return LangOptions::SSPOff;
}

void CheckObjCARC() const override;

llvm::ExceptionHandling GetExceptionModel(
const llvm::opt::ArgList &Args) const override;

bool SupportsEmbeddedBitcode() const override;

SanitizerMask getSupportedSanitizers() const override;

void printVerboseInfo(raw_ostream &OS) const override;
};


class LLVM_LIBRARY_VISIBILITY DarwinClang : public Darwin {
public:
DarwinClang(const Driver &D, const llvm::Triple &Triple,
const llvm::opt::ArgList &Args);




RuntimeLibType GetRuntimeLibType(const llvm::opt::ArgList &Args) const override;

void AddLinkRuntimeLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs,
bool ForceLinkBuiltinRT = false) const override;

void AddClangCXXStdlibIncludeArgs(
const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const override;

void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

void AddCCKextLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

void addClangWarningOptions(llvm::opt::ArgStringList &CC1Args) const override;

void AddLinkARCArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const override;

unsigned GetDefaultDwarfVersion() const override;


bool GetDefaultStandaloneDebug() const override { return true; }
llvm::DebuggerKind getDefaultDebuggerTuning() const override {
return llvm::DebuggerKind::LLDB;
}



private:
void AddLinkSanitizerLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs,
StringRef Sanitizer,
bool shared = true) const;

bool AddGnuCPlusPlusIncludePaths(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
llvm::SmallString<128> Base,
llvm::StringRef Version,
llvm::StringRef ArchDir,
llvm::StringRef BitDir) const;

llvm::StringRef GetHeaderSysroot(const llvm::opt::ArgList &DriverArgs) const;
};

}
}
}

#endif
