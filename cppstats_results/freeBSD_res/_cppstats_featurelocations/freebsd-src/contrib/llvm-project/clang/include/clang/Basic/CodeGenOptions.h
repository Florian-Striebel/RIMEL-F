











#if !defined(LLVM_CLANG_BASIC_CODEGENOPTIONS_H)
#define LLVM_CLANG_BASIC_CODEGENOPTIONS_H

#include "clang/Basic/DebugInfoOptions.h"
#include "clang/Basic/Sanitizers.h"
#include "clang/Basic/XRayInstr.h"
#include "llvm/ADT/FloatingPointMode.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/Regex.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Instrumentation/AddressSanitizerOptions.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace clang {



class CodeGenOptionsBase {
friend class CompilerInvocation;

public:
#define CODEGENOPT(Name, Bits, Default) unsigned Name : Bits;
#define ENUM_CODEGENOPT(Name, Type, Bits, Default)
#include "clang/Basic/CodeGenOptions.def"

protected:
#define CODEGENOPT(Name, Bits, Default)
#define ENUM_CODEGENOPT(Name, Type, Bits, Default) unsigned Name : Bits;
#include "clang/Basic/CodeGenOptions.def"
};



class CodeGenOptions : public CodeGenOptionsBase {
public:
enum InliningMethod {
NormalInlining,
OnlyHintInlining,
OnlyAlwaysInlining
};

enum VectorLibrary {
NoLibrary,
Accelerate,
LIBMVEC,
MASSV,
SVML,
Darwin_libsystem_m
};

enum ObjCDispatchMethodKind {
Legacy = 0,
NonLegacy = 1,
Mixed = 2
};

enum TLSModel {
GeneralDynamicTLSModel,
LocalDynamicTLSModel,
InitialExecTLSModel,
LocalExecTLSModel
};

enum StructReturnConventionKind {
SRCK_Default,
SRCK_OnStack,
SRCK_InRegs
};

enum ProfileInstrKind {
ProfileNone,
ProfileClangInstr,

ProfileIRInstr,
ProfileCSIRInstr,
};

enum EmbedBitcodeKind {
Embed_Off,
Embed_All,
Embed_Bitcode,
Embed_Marker
};















std::string BBSections;





std::string BinutilsVersion;

enum class FramePointerKind {
None,
NonLeaf,
All,
};

enum FiniteLoopsKind {
Language,
Always,
Never,
};


std::string CodeModel;




std::string CoverageDataFile;


std::string CoverageNotesFile;


std::string ProfileFilterFiles;


std::string ProfileExcludeFiles;


char CoverageVersion[4];


std::string DebugPass;


std::string DebugCompilationDir;


std::string CoverageCompilationDir;



std::string DwarfDebugFlags;



std::string RecordCommandLine;

std::map<std::string, std::string> DebugPrefixMap;
std::map<std::string, std::string> CoveragePrefixMap;


std::string FloatABI;



std::string DIBugsReportFilePath;


llvm::DenormalMode FPDenormalMode = llvm::DenormalMode::getIEEE();


llvm::DenormalMode FP32DenormalMode = llvm::DenormalMode::getIEEE();


std::string LimitFloatPrecision;

struct BitcodeFileToLink {

std::string Filename;



bool PropagateAttrs = false;

bool Internalize = false;

unsigned LinkFlags = 0;
};


std::vector<BitcodeFileToLink> LinkBitcodeFiles;




std::string MainFileName;



std::string SplitDwarfFile;


std::string SplitDwarfOutput;


llvm::Reloc::Model RelocationModel;



std::string TrapFuncName;


std::vector<std::string> DependentLibraries;


std::vector<std::string> LinkerOptions;



std::string InstrProfileOutput;


std::string SampleProfileFile;


std::string MemoryProfileOutput;


std::string ProfileInstrumentUsePath;



std::string ProfileRemappingFile;



std::string ThinLTOIndexFile;




std::string ThinLinkBitcodeFile;


std::string SaveTempsFilePrefix;



std::string CudaGpuBinaryFileName;



std::string OptRecordFile;



std::string OptRecordPasses;


std::string OptRecordFormat;



std::string SymbolPartition;

enum RemarkKind {
RK_Missing,
RK_Enabled,
RK_EnabledEverything,
RK_Disabled,
RK_DisabledEverything,
RK_WithPattern,
};


struct OptRemark {
RemarkKind Kind;
std::string Pattern;
std::shared_ptr<llvm::Regex> Regex;


OptRemark() : Kind(RK_Missing), Pattern(""), Regex(nullptr) {}



bool hasValidPattern() const { return Regex != nullptr; }


bool patternMatches(StringRef String) const {
return hasValidPattern() && Regex->match(String);
}
};





OptRemark OptimizationRemark;





OptRemark OptimizationRemarkMissed;






OptRemark OptimizationRemarkAnalysis;


std::vector<std::string> RewriteMapFiles;



SanitizerSet SanitizeRecover;


SanitizerSet SanitizeTrap;


std::vector<uint8_t> CmdArgs;


std::vector<std::string> NoBuiltinFuncs;

std::vector<std::string> Reciprocals;




std::string PreferVectorWidth;


XRayInstrSet XRayInstrumentationBundle;

std::vector<std::string> DefaultFunctionAttrs;


std::vector<std::string> PassPlugins;




std::vector<std::string> SanitizeCoverageAllowlistFiles;




std::string StackProtectorGuard;





std::string StackProtectorGuardReg;




std::vector<std::string> SanitizeCoverageIgnorelistFiles;




std::string StackUsageOutput;



const char *Argv0 = nullptr;
ArrayRef<const char *> CommandLineArgs;














Optional<uint64_t> DiagnosticsHotnessThreshold = 0;

public:

#define CODEGENOPT(Name, Bits, Default)
#define ENUM_CODEGENOPT(Name, Type, Bits, Default) Type get##Name() const { return static_cast<Type>(Name); } void set##Name(Type Value) { Name = static_cast<unsigned>(Value); }


#include "clang/Basic/CodeGenOptions.def"

CodeGenOptions();

const std::vector<std::string> &getNoBuiltinFuncs() const {
return NoBuiltinFuncs;
}


bool hasProfileClangInstr() const {
return getProfileInstr() == ProfileClangInstr;
}


bool hasProfileIRInstr() const {
return getProfileInstr() == ProfileIRInstr;
}


bool hasProfileCSIRInstr() const {
return getProfileInstr() == ProfileCSIRInstr;
}


bool hasProfileClangUse() const {
return getProfileUse() == ProfileClangInstr;
}


bool hasProfileIRUse() const {
return getProfileUse() == ProfileIRInstr ||
getProfileUse() == ProfileCSIRInstr;
}


bool hasProfileCSIRUse() const { return getProfileUse() == ProfileCSIRInstr; }


bool hasReducedDebugInfo() const {
return getDebugInfo() >= codegenoptions::DebugInfoConstructor;
}


bool hasMaybeUnusedDebugInfo() const {
return getDebugInfo() >= codegenoptions::UnusedTypeInfo;
}


bool hasSanitizeCoverage() const {
return SanitizeCoverageType || SanitizeCoverageIndirectCalls ||
SanitizeCoverageTraceCmp;
}
};

}

#endif
