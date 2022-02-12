







#if !defined(LLVM_CLANG_DRIVER_DRIVER_H)
#define LLVM_CLANG_DRIVER_DRIVER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Driver/Action.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/Phases.h"
#include "clang/Driver/ToolChain.h"
#include "clang/Driver/Types.h"
#include "clang/Driver/Util.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"

#include <list>
#include <map>
#include <string>

namespace llvm {
class Triple;
namespace vfs {
class FileSystem;
}
}

namespace clang {

namespace driver {

class Command;
class Compilation;
class InputInfo;
class JobList;
class JobAction;
class SanitizerArgs;
class ToolChain;


enum LTOKind {
LTOK_None,
LTOK_Full,
LTOK_Thin,
LTOK_Unknown
};



class Driver {
DiagnosticsEngine &Diags;

IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS;

enum DriverMode {
GCCMode,
GXXMode,
CPPMode,
CLMode,
FlangMode
} Mode;

enum SaveTempsMode {
SaveTempsNone,
SaveTempsCwd,
SaveTempsObj
} SaveTemps;

enum BitcodeEmbedMode {
EmbedNone,
EmbedMarker,
EmbedBitcode
} BitcodeEmbed;


LTOKind LTOMode;


LTOKind OffloadLTOMode;

public:
enum OpenMPRuntimeKind {


OMPRT_Unknown,



OMPRT_OMP,




OMPRT_GOMP,




OMPRT_IOMP5
};


DiagnosticBuilder Diag(unsigned DiagID) const {
return Diags.Report(DiagID);
}


public:

std::string Name;



std::string Dir;


std::string ClangExecutable;


ParsedClangName ClangNameParts;


std::string InstalledDir;


std::string ResourceDir;


std::string SystemConfigDir;


std::string UserConfigDir;





typedef SmallVector<std::string, 4> prefix_list;
prefix_list PrefixDirs;


std::string SysRoot;


std::string DyldPrefix;


std::string DriverTitle;


std::string HostBits, HostMachine, HostSystem, HostRelease;


std::string CCPrintStatReportFilename;


std::string CCPrintOptionsFilename;


std::string CCPrintHeadersFilename;


std::string CCLogDiagnosticsFilename;


typedef SmallVector<std::pair<types::ID, const llvm::opt::Arg *>, 16>
InputList;


bool CCCIsCXX() const { return Mode == GXXMode; }


bool CCCIsCPP() const { return Mode == CPPMode; }


bool CCCIsCC() const { return Mode == GCCMode; }


bool IsCLMode() const { return Mode == CLMode; }



bool IsFlangMode() const { return Mode == FlangMode; }


unsigned CCCPrintBindings : 1;



unsigned CCPrintOptions : 1;



unsigned CCPrintHeaders : 1;




unsigned CCLogDiagnostics : 1;


unsigned CCGenDiagnostics : 1;



unsigned CCPrintProcessStats : 1;





typedef int (*CC1ToolFunc)(SmallVectorImpl<const char *> &ArgV);
CC1ToolFunc CC1Main = nullptr;

private:

std::string TargetTriple;


std::string CCCGenericGCCName;


std::string ConfigFile;


llvm::BumpPtrAllocator Alloc;


llvm::StringSaver Saver;


std::unique_ptr<llvm::opt::InputArgList> CfgOptions;


std::unique_ptr<llvm::opt::InputArgList> CLOptions;



unsigned CheckInputsExist : 1;

public:



unsigned GenReproducer : 1;

private:

unsigned SuppressMissingInputWarning : 1;






mutable llvm::StringMap<std::unique_ptr<ToolChain>> ToolChains;

private:


llvm::opt::DerivedArgList *
TranslateInputArgs(const llvm::opt::InputArgList &Args) const;






phases::ID getFinalPhase(const llvm::opt::DerivedArgList &DAL,
llvm::opt::Arg **FinalPhaseArg = nullptr) const;



void handleArguments(Compilation &C, llvm::opt::DerivedArgList &Args,
const InputList &Inputs, ActionList &Actions) const;


void setUpResponseFiles(Compilation &C, Command &Cmd);

void generatePrefixedToolNames(StringRef Tool, const ToolChain &TC,
SmallVectorImpl<std::string> &Names) const;












bool getCrashDiagnosticFile(StringRef ReproCrashFilename,
SmallString<128> &CrashDiagDir);

public:



static std::string GetResourcesPath(StringRef BinaryPath,
StringRef CustomResourceDir = "");

Driver(StringRef ClangExecutable, StringRef TargetTriple,
DiagnosticsEngine &Diags, std::string Title = "clang LLVM compiler",
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS = nullptr);





const std::string &getCCCGenericGCCName() const { return CCCGenericGCCName; }

const std::string &getConfigFile() const { return ConfigFile; }

const llvm::opt::OptTable &getOpts() const { return getDriverOptTable(); }

DiagnosticsEngine &getDiags() const { return Diags; }

llvm::vfs::FileSystem &getVFS() const { return *VFS; }

bool getCheckInputsExist() const { return CheckInputsExist; }

void setCheckInputsExist(bool Value) { CheckInputsExist = Value; }

void setTargetAndMode(const ParsedClangName &TM) { ClangNameParts = TM; }

const std::string &getTitle() { return DriverTitle; }
void setTitle(std::string Value) { DriverTitle = std::move(Value); }

std::string getTargetTriple() const { return TargetTriple; }


const char *getClangProgramPath() const {
return ClangExecutable.c_str();
}


const char *getInstalledDir() const {
if (!InstalledDir.empty())
return InstalledDir.c_str();
return Dir.c_str();
}
void setInstalledDir(StringRef Value) { InstalledDir = std::string(Value); }

bool isSaveTempsEnabled() const { return SaveTemps != SaveTempsNone; }
bool isSaveTempsObj() const { return SaveTemps == SaveTempsObj; }

bool embedBitcodeEnabled() const { return BitcodeEmbed != EmbedNone; }
bool embedBitcodeInObject() const { return (BitcodeEmbed == EmbedBitcode); }
bool embedBitcodeMarkerOnly() const { return (BitcodeEmbed == EmbedMarker); }


OpenMPRuntimeKind getOpenMPRuntime(const llvm::opt::ArgList &Args) const;








void CreateOffloadingDeviceToolChains(Compilation &C, InputList &Inputs);








Compilation *BuildCompilation(ArrayRef<const char *> Args);



llvm::opt::InputArgList ParseArgStrings(ArrayRef<const char *> Args,
bool IsClCompatMode,
bool &ContainsError);








void BuildInputs(const ToolChain &TC, llvm::opt::DerivedArgList &Args,
InputList &Inputs) const;







void BuildActions(Compilation &C, llvm::opt::DerivedArgList &Args,
const InputList &Inputs, ActionList &Actions) const;






void BuildUniversalActions(Compilation &C, const ToolChain &TC,
const InputList &BAInputs) const;





bool DiagnoseInputExistence(const llvm::opt::DerivedArgList &Args,
StringRef Value, types::ID Ty,
bool TypoCorrect) const;





void BuildJobs(Compilation &C) const;







int ExecuteCompilation(Compilation &C,
SmallVectorImpl< std::pair<int, const Command *> > &FailingCommands);



struct CompilationDiagnosticReport {
llvm::SmallVector<std::string, 4> TemporaryFiles;
};




void generateCompilationDiagnostics(
Compilation &C, const Command &FailingCommand,
StringRef AdditionalInformation = "",
CompilationDiagnosticReport *GeneratedReport = nullptr);






void PrintActions(const Compilation &C) const;




void PrintHelp(bool ShowHidden) const;


void PrintVersion(const Compilation &C, raw_ostream &OS) const;







std::string GetFilePath(StringRef Name, const ToolChain &TC) const;







std::string GetProgramPath(StringRef Name, const ToolChain &TC) const;



void HandleAutocompletions(StringRef PassedFlags) const;






bool HandleImmediateArgs(const Compilation &C);




Action *ConstructPhaseAction(
Compilation &C, const llvm::opt::ArgList &Args, phases::ID Phase,
Action *Input,
Action::OffloadKind TargetDeviceOffloadKind = Action::OFK_None) const;




InputInfo
BuildJobsForAction(Compilation &C, const Action *A, const ToolChain *TC,
StringRef BoundArch, bool AtTopLevel, bool MultipleArchs,
const char *LinkingOutput,
std::map<std::pair<const Action *, std::string>, InputInfo>
&CachedResults,
Action::OffloadKind TargetDeviceOffloadKind) const;


const char *getDefaultImageName() const;













const char *GetNamedOutputPath(Compilation &C, const JobAction &JA,
const char *BaseInput, StringRef BoundArch,
bool AtTopLevel, bool MultipleArchs,
StringRef NormalizedTriple) const;





std::string GetTemporaryPath(StringRef Prefix, StringRef Suffix) const;



std::string GetTemporaryDirectory(StringRef Prefix) const;


std::string GetClPchPath(Compilation &C, StringRef BaseName) const;



bool ShouldUseClangCompiler(const JobAction &JA) const;



bool ShouldUseFlangCompiler(const JobAction &JA) const;


bool ShouldEmitStaticLibrary(const llvm::opt::ArgList &Args) const;


bool isUsingLTO(bool IsOffload = false) const {
return getLTOMode(IsOffload) != LTOK_None;
}


LTOKind getLTOMode(bool IsOffload = false) const {
return IsOffload ? OffloadLTOMode : LTOMode;
}

private:




bool loadConfigFile();





bool readConfigFile(StringRef FileName);



void setDriverMode(StringRef DriverModeValue);



void setLTOMode(const llvm::opt::ArgList &Args);





const ToolChain &getToolChain(const llvm::opt::ArgList &Args,
const llvm::Triple &Target) const;





std::pair<unsigned, unsigned> getIncludeExcludeOptionFlagMasks(bool IsClCompatMode) const;




InputInfo BuildJobsForActionNoCache(
Compilation &C, const Action *A, const ToolChain *TC, StringRef BoundArch,
bool AtTopLevel, bool MultipleArchs, const char *LinkingOutput,
std::map<std::pair<const Action *, std::string>, InputInfo>
&CachedResults,
Action::OffloadKind TargetDeviceOffloadKind) const;

public:







static bool GetReleaseVersion(StringRef Str, unsigned &Major, unsigned &Minor,
unsigned &Micro, bool &HadExtra);







static bool GetReleaseVersion(StringRef Str,
MutableArrayRef<unsigned> Digits);


static bool getDefaultModuleCachePath(SmallVectorImpl<char> &Result);
};



bool isOptimizationLevelFast(const llvm::opt::ArgList &Args);


bool willEmitRemarks(const llvm::opt::ArgList &Args);






llvm::StringRef getDriverMode(StringRef ProgName, ArrayRef<const char *> Args);


bool IsClangCL(StringRef DriverMode);

}
}

#endif
