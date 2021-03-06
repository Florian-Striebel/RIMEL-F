







#if !defined(LLVM_CLANG_FRONTEND_FRONTENDOPTIONS_H)
#define LLVM_CLANG_FRONTEND_FRONTENDOPTIONS_H

#include "clang/AST/ASTDumperUtils.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Frontend/CommandLineSourceLoc.h"
#include "clang/Sema/CodeCompleteOptions.h"
#include "clang/Serialization/ModuleFileExtension.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace llvm {

class MemoryBuffer;

}

namespace clang {

namespace frontend {

enum ActionKind {

ASTDeclList,


ASTDump,


ASTPrint,


ASTView,


DumpCompilerOptions,


DumpRawTokens,


DumpTokens,


EmitAssembly,


EmitBC,


EmitHTML,


EmitLLVM,


EmitLLVMOnly,


EmitCodeGenOnly,


EmitObj,


FixIt,


GenerateModule,


GenerateModuleInterface,


GenerateHeaderModule,


GeneratePCH,


GenerateInterfaceStubs,


InitOnly,


ModuleFileInfo,


VerifyPCH,


ParseSyntaxOnly,


PluginAction,


PrintPreamble,


PrintPreprocessedInput,


RewriteMacros,


RewriteObjC,


RewriteTest,


RunAnalysis,


TemplightDump,


MigrateSource,


RunPreprocessorOnly,


PrintDependencyDirectivesSourceMinimizerOutput
};

}


class InputKind {
private:
Language Lang;
unsigned Fmt : 3;
unsigned Preprocessed : 1;

public:

enum Format {
Source,
ModuleMap,
Precompiled
};

constexpr InputKind(Language L = Language::Unknown, Format F = Source,
bool PP = false)
: Lang(L), Fmt(F), Preprocessed(PP) {}

Language getLanguage() const { return static_cast<Language>(Lang); }
Format getFormat() const { return static_cast<Format>(Fmt); }
bool isPreprocessed() const { return Preprocessed; }


bool isUnknown() const { return Lang == Language::Unknown && Fmt == Source; }


bool isObjectiveC() const {
return Lang == Language::ObjC || Lang == Language::ObjCXX;
}

InputKind getPreprocessed() const {
return InputKind(getLanguage(), getFormat(), true);
}

InputKind withFormat(Format F) const {
return InputKind(getLanguage(), F, isPreprocessed());
}
};


class FrontendInputFile {

std::string File;




llvm::Optional<llvm::MemoryBufferRef> Buffer;


InputKind Kind;


bool IsSystem = false;

public:
FrontendInputFile() = default;
FrontendInputFile(StringRef File, InputKind Kind, bool IsSystem = false)
: File(File.str()), Kind(Kind), IsSystem(IsSystem) {}
FrontendInputFile(llvm::MemoryBufferRef Buffer, InputKind Kind,
bool IsSystem = false)
: Buffer(Buffer), Kind(Kind), IsSystem(IsSystem) {}

InputKind getKind() const { return Kind; }
bool isSystem() const { return IsSystem; }

bool isEmpty() const { return File.empty() && Buffer == None; }
bool isFile() const { return !isBuffer(); }
bool isBuffer() const { return Buffer != None; }
bool isPreprocessed() const { return Kind.isPreprocessed(); }

StringRef getFile() const {
assert(isFile());
return File;
}

llvm::MemoryBufferRef getBuffer() const {
assert(isBuffer());
return *Buffer;
}
};


class FrontendOptions {
public:

unsigned DisableFree : 1;



unsigned RelocatablePCH : 1;


unsigned ShowHelp : 1;


unsigned ShowStats : 1;


unsigned PrintSupportedCPUs : 1;


unsigned TimeTrace : 1;


unsigned ShowVersion : 1;


unsigned FixWhatYouCan : 1;


unsigned FixOnlyWarnings : 1;


unsigned FixAndRecompile : 1;


unsigned FixToTemporaries : 1;


unsigned ARCMTMigrateEmitARCErrors : 1;



unsigned SkipFunctionBodies : 1;


unsigned UseGlobalModuleIndex : 1;


unsigned GenerateGlobalModuleIndex : 1;


unsigned ASTDumpDecls : 1;


unsigned ASTDumpAll : 1;


unsigned ASTDumpLookups : 1;


unsigned ASTDumpDeclTypes : 1;


unsigned BuildingImplicitModule : 1;


unsigned ModulesEmbedAllFiles : 1;


unsigned IncludeTimestamps : 1;


unsigned UseTemporary : 1;


unsigned IsSystemModule : 1;


unsigned AllowPCMWithCompilerErrors : 1;

CodeCompleteOptions CodeCompleteOpts;


ASTDumpOutputFormat ASTDumpFormat = ADOF_Default;

enum {
ARCMT_None,
ARCMT_Check,
ARCMT_Modify,
ARCMT_Migrate
} ARCMTAction = ARCMT_None;

enum {
ObjCMT_None = 0,


ObjCMT_Literals = 0x1,


ObjCMT_Subscripting = 0x2,


ObjCMT_ReadonlyProperty = 0x4,


ObjCMT_ReadwriteProperty = 0x8,


ObjCMT_Property = (ObjCMT_ReadonlyProperty | ObjCMT_ReadwriteProperty),


ObjCMT_Annotation = 0x10,


ObjCMT_Instancetype = 0x20,


ObjCMT_NsMacros = 0x40,


ObjCMT_ProtocolConformance = 0x80,


ObjCMT_AtomicProperty = 0x100,


ObjCMT_ReturnsInnerPointerProperty = 0x200,


ObjCMT_NsAtomicIOSOnlyProperty = 0x400,


ObjCMT_DesignatedInitializer = 0x800,


ObjCMT_PropertyDotSyntax = 0x1000,

ObjCMT_MigrateDecls = (ObjCMT_ReadonlyProperty | ObjCMT_ReadwriteProperty |
ObjCMT_Annotation | ObjCMT_Instancetype |
ObjCMT_NsMacros | ObjCMT_ProtocolConformance |
ObjCMT_NsAtomicIOSOnlyProperty |
ObjCMT_DesignatedInitializer),
ObjCMT_MigrateAll = (ObjCMT_Literals | ObjCMT_Subscripting |
ObjCMT_MigrateDecls | ObjCMT_PropertyDotSyntax)
};
unsigned ObjCMTAction = ObjCMT_None;
std::string ObjCMTWhiteListPath;

std::string MTMigrateDir;
std::string ARCMTMigrateReportOut;



InputKind DashX;


SmallVector<FrontendInputFile, 0> Inputs;



std::string OriginalModuleMap;


std::string OutputFile;


std::string FixItSuffix;


std::string ASTDumpFilter;


ParsedSourceLocation CodeCompletionAt;


frontend::ActionKind ProgramAction = frontend::ParseSyntaxOnly;


std::string ActionName;


std::map<std::string, std::vector<std::string>> PluginArgs;


std::vector<std::string> AddPluginActions;


std::vector<std::string> Plugins;


std::vector<std::shared_ptr<ModuleFileExtension>> ModuleFileExtensions;


std::vector<std::string> ModuleMapFiles;



std::vector<std::string> ModuleFiles;


std::vector<std::string> ModulesEmbedFiles;


std::vector<std::string> ASTMergeFiles;



std::vector<std::string> LLVMArgs;



std::string OverrideRecordLayoutsFile;


std::string AuxTriple;


Optional<std::string> AuxTargetCPU;


Optional<std::vector<std::string>> AuxTargetFeatures;


std::string StatsFile;


unsigned TimeTraceGranularity;

public:
FrontendOptions()
: DisableFree(false), RelocatablePCH(false), ShowHelp(false),
ShowStats(false), TimeTrace(false), ShowVersion(false),
FixWhatYouCan(false), FixOnlyWarnings(false), FixAndRecompile(false),
FixToTemporaries(false), ARCMTMigrateEmitARCErrors(false),
SkipFunctionBodies(false), UseGlobalModuleIndex(true),
GenerateGlobalModuleIndex(true), ASTDumpDecls(false),
ASTDumpLookups(false), BuildingImplicitModule(false),
ModulesEmbedAllFiles(false), IncludeTimestamps(true),
UseTemporary(true), AllowPCMWithCompilerErrors(false),
TimeTraceGranularity(500) {}






static InputKind getInputKindForExtension(StringRef Extension);
};

}

#endif
