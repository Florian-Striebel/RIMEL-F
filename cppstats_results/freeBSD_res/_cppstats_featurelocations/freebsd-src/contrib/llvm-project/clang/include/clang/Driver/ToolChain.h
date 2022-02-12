







#if !defined(LLVM_CLANG_DRIVER_TOOLCHAIN_H)
#define LLVM_CLANG_DRIVER_TOOLCHAIN_H

#include "clang/Basic/DebugInfoOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/Sanitizers.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Multilib.h"
#include "clang/Driver/Types.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FloatingPointMode.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Target/TargetOptions.h"
#include <cassert>
#include <climits>
#include <memory>
#include <string>
#include <utility>

namespace llvm {
namespace opt {

class Arg;
class ArgList;
class DerivedArgList;

}
namespace vfs {

class FileSystem;

}
}

namespace clang {

class ObjCRuntime;

namespace driver {

class Driver;
class InputInfo;
class SanitizerArgs;
class Tool;
class XRayArgs;



struct ParsedClangName {

std::string TargetPrefix;


std::string ModeSuffix;


const char *DriverMode = nullptr;


bool TargetIsValid = false;

ParsedClangName() = default;
ParsedClangName(std::string Suffix, const char *Mode)
: ModeSuffix(Suffix), DriverMode(Mode) {}
ParsedClangName(std::string Target, std::string Suffix, const char *Mode,
bool IsRegistered)
: TargetPrefix(Target), ModeSuffix(Suffix), DriverMode(Mode),
TargetIsValid(IsRegistered) {}

bool isEmpty() const {
return TargetPrefix.empty() && ModeSuffix.empty() && DriverMode == nullptr;
}
};


class ToolChain {
public:
using path_list = SmallVector<std::string, 16>;

enum CXXStdlibType {
CST_Libcxx,
CST_Libstdcxx
};

enum RuntimeLibType {
RLT_CompilerRT,
RLT_Libgcc
};

enum UnwindLibType {
UNW_None,
UNW_CompilerRT,
UNW_Libgcc
};

enum RTTIMode {
RM_Enabled,
RM_Disabled,
};

enum FileType { FT_Object, FT_Static, FT_Shared };

private:
friend class RegisterEffectiveTriple;

const Driver &D;
llvm::Triple Triple;
const llvm::opt::ArgList &Args;


const llvm::opt::Arg *const CachedRTTIArg;

const RTTIMode CachedRTTIMode;


path_list LibraryPaths;


path_list FilePaths;


path_list ProgramPaths;

mutable std::unique_ptr<Tool> Clang;
mutable std::unique_ptr<Tool> Flang;
mutable std::unique_ptr<Tool> Assemble;
mutable std::unique_ptr<Tool> Link;
mutable std::unique_ptr<Tool> StaticLibTool;
mutable std::unique_ptr<Tool> IfsMerge;
mutable std::unique_ptr<Tool> OffloadBundler;
mutable std::unique_ptr<Tool> OffloadWrapper;

Tool *getClang() const;
Tool *getFlang() const;
Tool *getAssemble() const;
Tool *getLink() const;
Tool *getStaticLibTool() const;
Tool *getIfsMerge() const;
Tool *getClangAs() const;
Tool *getOffloadBundler() const;
Tool *getOffloadWrapper() const;

mutable std::unique_ptr<SanitizerArgs> SanitizerArguments;
mutable std::unique_ptr<XRayArgs> XRayArguments;


mutable llvm::Triple EffectiveTriple;


void setEffectiveTriple(llvm::Triple ET) const {
EffectiveTriple = std::move(ET);
}

mutable llvm::Optional<CXXStdlibType> cxxStdlibType;
mutable llvm::Optional<RuntimeLibType> runtimeLibType;
mutable llvm::Optional<UnwindLibType> unwindLibType;

protected:
MultilibSet Multilibs;
Multilib SelectedMultilib;

ToolChain(const Driver &D, const llvm::Triple &T,
const llvm::opt::ArgList &Args);

void setTripleEnvironment(llvm::Triple::EnvironmentType Env);

virtual Tool *buildAssembler() const;
virtual Tool *buildLinker() const;
virtual Tool *buildStaticLibTool() const;
virtual Tool *getTool(Action::ActionClass AC) const;

virtual std::string buildCompilerRTBasename(const llvm::opt::ArgList &Args,
StringRef Component,
FileType Type,
bool AddArch) const;



static void addSystemInclude(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
const Twine &Path);
static void addExternCSystemInclude(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
const Twine &Path);
static void
addExternCSystemIncludeIfExists(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
const Twine &Path);
static void addSystemIncludes(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
ArrayRef<StringRef> Paths);


public:
virtual ~ToolChain();



const Driver &getDriver() const { return D; }
llvm::vfs::FileSystem &getVFS() const;
const llvm::Triple &getTriple() const { return Triple; }






virtual const llvm::Triple *getAuxTriple() const { return nullptr; }




virtual std::string getInputFilename(const InputInfo &Input) const;

llvm::Triple::ArchType getArch() const { return Triple.getArch(); }
StringRef getArchName() const { return Triple.getArchName(); }
StringRef getPlatform() const { return Triple.getVendorName(); }
StringRef getOS() const { return Triple.getOSName(); }



StringRef getDefaultUniversalArchName() const;

std::string getTripleString() const {
return Triple.getTriple();
}


const llvm::Triple &getEffectiveTriple() const {
assert(!EffectiveTriple.getTriple().empty() && "No effective triple");
return EffectiveTriple;
}

path_list &getLibraryPaths() { return LibraryPaths; }
const path_list &getLibraryPaths() const { return LibraryPaths; }

path_list &getFilePaths() { return FilePaths; }
const path_list &getFilePaths() const { return FilePaths; }

path_list &getProgramPaths() { return ProgramPaths; }
const path_list &getProgramPaths() const { return ProgramPaths; }

const MultilibSet &getMultilibs() const { return Multilibs; }

const Multilib &getMultilib() const { return SelectedMultilib; }

const SanitizerArgs& getSanitizerArgs() const;

const XRayArgs& getXRayArgs() const;


const llvm::opt::Arg *getRTTIArg() const { return CachedRTTIArg; }


RTTIMode getRTTIMode() const { return CachedRTTIMode; }















static ParsedClangName getTargetAndModeFromProgramName(StringRef ProgName);











virtual llvm::opt::DerivedArgList *
TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind) const {
return nullptr;
}




virtual llvm::opt::DerivedArgList *TranslateOpenMPTargetArgs(
const llvm::opt::DerivedArgList &Args, bool SameTripleAsHost,
SmallVectorImpl<llvm::opt::Arg *> &AllocatedArgs) const;




virtual void TranslateXarchArgs(
const llvm::opt::DerivedArgList &Args, llvm::opt::Arg *&A,
llvm::opt::DerivedArgList *DAL,
SmallVectorImpl<llvm::opt::Arg *> *AllocatedArgs = nullptr) const;




virtual llvm::opt::DerivedArgList *
TranslateXarchArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
Action::OffloadKind DeviceOffloadKind,
SmallVectorImpl<llvm::opt::Arg *> *AllocatedArgs) const;





virtual Tool *SelectTool(const JobAction &JA) const;



std::string GetFilePath(const char *Name) const;
std::string GetProgramPath(const char *Name) const;









std::string GetLinkerPath(bool *LinkerIsLLD = nullptr,
bool *LinkerIsLLDDarwinNew = nullptr) const;


std::string GetStaticLibToolPath() const;






virtual void printVerboseInfo(raw_ostream &OS) const {}





virtual bool isCrossCompiling() const;



virtual bool HasNativeLLVMSupport() const;



virtual types::ID LookupTypeForExtension(StringRef Ext) const;


virtual bool IsBlocksDefault() const { return false; }



virtual bool IsIntegratedAssemblerDefault() const { return false; }


virtual bool useIntegratedAs() const;



virtual bool parseInlineAsmUsingAsmParser() const { return false; }


virtual bool IsMathErrnoDefault() const { return true; }



virtual bool IsEncodeExtendedBlockSignatureDefault() const { return false; }



virtual bool IsObjCNonFragileABIDefault() const { return false; }



virtual bool UseObjCMixedDispatch() const { return false; }


virtual bool useRelaxRelocations() const;



virtual LangOptions::StackProtectorMode
GetDefaultStackProtectorLevel(bool KernelOrKext) const {
return LangOptions::SSPOff;
}


virtual LangOptions::TrivialAutoVarInitKind
GetDefaultTrivialAutoVarInit() const {
return LangOptions::TrivialAutoVarInitKind::Uninitialized;
}


virtual const char *getDefaultLinker() const { return "ld"; }


virtual RuntimeLibType GetDefaultRuntimeLibType() const {
return ToolChain::RLT_Libgcc;
}

virtual CXXStdlibType GetDefaultCXXStdlibType() const {
return ToolChain::CST_Libstdcxx;
}

virtual UnwindLibType GetDefaultUnwindLibType() const {
return ToolChain::UNW_None;
}

virtual std::string getCompilerRTPath() const;

virtual std::string getCompilerRT(const llvm::opt::ArgList &Args,
StringRef Component,
FileType Type = ToolChain::FT_Static) const;

const char *
getCompilerRTArgString(const llvm::opt::ArgList &Args, StringRef Component,
FileType Type = ToolChain::FT_Static) const;

std::string getCompilerRTBasename(const llvm::opt::ArgList &Args,
StringRef Component,
FileType Type = ToolChain::FT_Static) const;


virtual std::string getRuntimePath() const;


virtual std::string getStdlibPath() const;



std::string getArchSpecificLibPath() const;


virtual StringRef getOSLibName() const;


static bool needsProfileRT(const llvm::opt::ArgList &Args);


static bool needsGCovInstrumentation(const llvm::opt::ArgList &Args);



virtual bool IsUnwindTablesDefault(const llvm::opt::ArgList &Args) const;


virtual bool
IsAArch64OutlineAtomicsDefault(const llvm::opt::ArgList &Args) const {
return false;
}


virtual bool isPICDefault() const = 0;


virtual bool isPIEDefault() const = 0;


virtual bool isNoExecStackDefault() const;





virtual bool isPICDefaultForced() const = 0;


virtual bool SupportsProfiling() const { return true; }


virtual void CheckObjCARC() const {}


virtual codegenoptions::DebugInfoFormat getDefaultDebugFormat() const {
return codegenoptions::DIF_DWARF;
}



virtual bool UseDwarfDebugFlags() const { return false; }



virtual unsigned GetDefaultDwarfVersion() const { return 4; }




virtual unsigned getMaxDwarfVersion() const { return UINT_MAX; }





virtual bool GetDefaultStandaloneDebug() const { return false; }


virtual llvm::DebuggerKind getDefaultDebuggerTuning() const {
return llvm::DebuggerKind::GDB;
}


virtual bool supportsDebugInfoOption(const llvm::opt::Arg *) const {
return true;
}


virtual void adjustDebugInfoKind(codegenoptions::DebugInfoKind &DebugInfoKind,
const llvm::opt::ArgList &Args) const {}


virtual llvm::ExceptionHandling
GetExceptionModel(const llvm::opt::ArgList &Args) const;


virtual bool SupportsEmbeddedBitcode() const { return false; }


virtual std::string getThreadModel() const { return "posix"; }


virtual bool isThreadModelSupported(const StringRef Model) const;

virtual std::string getMultiarchTriple(const Driver &D,
const llvm::Triple &TargetTriple,
StringRef SysRoot) const {
return TargetTriple.str();
}



virtual std::string
ComputeLLVMTriple(const llvm::opt::ArgList &Args,
types::ID InputType = types::TY_INVALID) const;






virtual std::string ComputeEffectiveClangTriple(
const llvm::opt::ArgList &Args,
types::ID InputType = types::TY_INVALID) const;





virtual ObjCRuntime getDefaultObjCRuntime(bool isNonFragile) const;






virtual bool hasBlocksRuntime() const { return true; }



virtual std::string computeSysRoot() const;





virtual void
AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;


virtual void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args,
Action::OffloadKind DeviceOffloadKind) const;


virtual void addClangWarningOptions(llvm::opt::ArgStringList &CC1Args) const;



virtual RuntimeLibType
GetRuntimeLibType(const llvm::opt::ArgList &Args) const;



virtual CXXStdlibType GetCXXStdlibType(const llvm::opt::ArgList &Args) const;



virtual UnwindLibType GetUnwindLibType(const llvm::opt::ArgList &Args) const;


virtual std::string detectLibcxxVersion(StringRef IncludePath) const;



virtual void
AddClangCXXStdlibIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;



void AddClangCXXStdlibIsystemArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;



bool ShouldLinkCXXStdlib(const llvm::opt::ArgList &Args) const;



virtual void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;


void AddFilePathLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;



virtual void AddCCKextLibArgs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;





virtual bool isFastMathRuntimeAvailable(
const llvm::opt::ArgList &Args, std::string &Path) const;





bool addFastMathRuntimeIfAvailable(
const llvm::opt::ArgList &Args, llvm::opt::ArgStringList &CmdArgs) const;



virtual void addProfileRTLibs(const llvm::opt::ArgList &Args,
llvm::opt::ArgStringList &CmdArgs) const;


virtual void AddCudaIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;


virtual void AddHIPIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;


virtual void AddIAMCUIncludeArgs(const llvm::opt::ArgList &DriverArgs,
llvm::opt::ArgStringList &CC1Args) const;


virtual VersionTuple computeMSVCVersion(const Driver *D,
const llvm::opt::ArgList &Args) const;


virtual llvm::SmallVector<std::string, 12>
getHIPDeviceLibs(const llvm::opt::ArgList &Args) const;


virtual SanitizerMask getSupportedSanitizers() const;


virtual SanitizerMask getDefaultSanitizers() const {
return SanitizerMask();
}



virtual bool canSplitThinLTOUnit() const { return true; }




virtual llvm::DenormalMode getDefaultDenormalModeForType(
const llvm::opt::ArgList &DriverArgs, const JobAction &JA,
const llvm::fltSemantics *FPType = nullptr) const {
return llvm::DenormalMode::getIEEE();
}
};



class RegisterEffectiveTriple {
const ToolChain &TC;

public:
RegisterEffectiveTriple(const ToolChain &TC, llvm::Triple T) : TC(TC) {
TC.setEffectiveTriple(std::move(T));
}

~RegisterEffectiveTriple() { TC.setEffectiveTriple(llvm::Triple()); }
};

}

}

#endif
