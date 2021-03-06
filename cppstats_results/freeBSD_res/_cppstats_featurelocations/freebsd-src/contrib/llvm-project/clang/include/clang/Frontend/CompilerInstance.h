







#if !defined(LLVM_CLANG_FRONTEND_COMPILERINSTANCE_H_)
#define LLVM_CLANG_FRONTEND_COMPILERINSTANCE_H_

#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/PCHContainerOperations.h"
#include "clang/Frontend/Utils.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/ModuleLoader.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/FileSystem.h"
#include <cassert>
#include <list>
#include <memory>
#include <string>
#include <utility>

namespace llvm {
class raw_fd_ostream;
class Timer;
class TimerGroup;
}

namespace clang {
class ASTContext;
class ASTReader;
class CodeCompleteConsumer;
class DiagnosticsEngine;
class DiagnosticConsumer;
class ExternalASTSource;
class FileEntry;
class FileManager;
class FrontendAction;
class InMemoryModuleCache;
class Module;
class Preprocessor;
class Sema;
class SourceManager;
class TargetInfo;
enum class DisableValidationForModuleKind;



















class CompilerInstance : public ModuleLoader {

std::shared_ptr<CompilerInvocation> Invocation;


IntrusiveRefCntPtr<DiagnosticsEngine> Diagnostics;


IntrusiveRefCntPtr<TargetInfo> Target;


IntrusiveRefCntPtr<TargetInfo> AuxTarget;


IntrusiveRefCntPtr<FileManager> FileMgr;


IntrusiveRefCntPtr<SourceManager> SourceMgr;


IntrusiveRefCntPtr<InMemoryModuleCache> ModuleCache;


std::shared_ptr<Preprocessor> PP;


IntrusiveRefCntPtr<ASTContext> Context;


IntrusiveRefCntPtr<ExternalSemaSource> ExternalSemaSrc;


std::unique_ptr<ASTConsumer> Consumer;


std::unique_ptr<CodeCompleteConsumer> CompletionConsumer;


std::unique_ptr<Sema> TheSema;


std::unique_ptr<llvm::TimerGroup> FrontendTimerGroup;


std::unique_ptr<llvm::Timer> FrontendTimer;


IntrusiveRefCntPtr<ASTReader> TheASTReader;


std::shared_ptr<ModuleDependencyCollector> ModuleDepCollector;


std::shared_ptr<PCHContainerOperations> ThePCHContainerOperations;

std::vector<std::shared_ptr<DependencyCollector>> DependencyCollectors;



std::map<std::string, std::string, std::less<>> BuiltModules;


bool DeleteBuiltModules = true;



SourceLocation LastModuleImportLoc;



ModuleLoadResult LastModuleImportResult;



bool BuildGlobalModuleIndex = false;


bool HaveFullGlobalModuleIndex = false;


bool DisableGeneratingGlobalModuleIndex = false;


std::unique_ptr<raw_ostream> OwnedVerboseOutputStream;


raw_ostream *VerboseOutputStream = &llvm::errs();






struct OutputFile {
std::string Filename;
Optional<llvm::sys::fs::TempFile> File;

OutputFile(std::string filename, Optional<llvm::sys::fs::TempFile> file)
: Filename(std::move(filename)), File(std::move(file)) {}
};


std::list<OutputFile> OutputFiles;


std::unique_ptr<llvm::raw_pwrite_stream> OutputStream;

CompilerInstance(const CompilerInstance &) = delete;
void operator=(const CompilerInstance &) = delete;
public:
explicit CompilerInstance(
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>(),
InMemoryModuleCache *SharedModuleCache = nullptr);
~CompilerInstance() override;































bool ExecuteAction(FrontendAction &Act);





bool hasInvocation() const { return Invocation != nullptr; }

CompilerInvocation &getInvocation() {
assert(Invocation && "Compiler instance has no invocation!");
return *Invocation;
}


void setInvocation(std::shared_ptr<CompilerInvocation> Value);


bool shouldBuildGlobalModuleIndex() const;



void setBuildGlobalModuleIndex(bool Build) {
BuildGlobalModuleIndex = Build;
}





AnalyzerOptionsRef getAnalyzerOpts() {
return Invocation->getAnalyzerOpts();
}

CodeGenOptions &getCodeGenOpts() {
return Invocation->getCodeGenOpts();
}
const CodeGenOptions &getCodeGenOpts() const {
return Invocation->getCodeGenOpts();
}

DependencyOutputOptions &getDependencyOutputOpts() {
return Invocation->getDependencyOutputOpts();
}
const DependencyOutputOptions &getDependencyOutputOpts() const {
return Invocation->getDependencyOutputOpts();
}

DiagnosticOptions &getDiagnosticOpts() {
return Invocation->getDiagnosticOpts();
}
const DiagnosticOptions &getDiagnosticOpts() const {
return Invocation->getDiagnosticOpts();
}

FileSystemOptions &getFileSystemOpts() {
return Invocation->getFileSystemOpts();
}
const FileSystemOptions &getFileSystemOpts() const {
return Invocation->getFileSystemOpts();
}

FrontendOptions &getFrontendOpts() {
return Invocation->getFrontendOpts();
}
const FrontendOptions &getFrontendOpts() const {
return Invocation->getFrontendOpts();
}

HeaderSearchOptions &getHeaderSearchOpts() {
return Invocation->getHeaderSearchOpts();
}
const HeaderSearchOptions &getHeaderSearchOpts() const {
return Invocation->getHeaderSearchOpts();
}
std::shared_ptr<HeaderSearchOptions> getHeaderSearchOptsPtr() const {
return Invocation->getHeaderSearchOptsPtr();
}

LangOptions &getLangOpts() {
return *Invocation->getLangOpts();
}
const LangOptions &getLangOpts() const {
return *Invocation->getLangOpts();
}

PreprocessorOptions &getPreprocessorOpts() {
return Invocation->getPreprocessorOpts();
}
const PreprocessorOptions &getPreprocessorOpts() const {
return Invocation->getPreprocessorOpts();
}

PreprocessorOutputOptions &getPreprocessorOutputOpts() {
return Invocation->getPreprocessorOutputOpts();
}
const PreprocessorOutputOptions &getPreprocessorOutputOpts() const {
return Invocation->getPreprocessorOutputOpts();
}

TargetOptions &getTargetOpts() {
return Invocation->getTargetOpts();
}
const TargetOptions &getTargetOpts() const {
return Invocation->getTargetOpts();
}





bool hasDiagnostics() const { return Diagnostics != nullptr; }


DiagnosticsEngine &getDiagnostics() const {
assert(Diagnostics && "Compiler instance has no diagnostics!");
return *Diagnostics;
}


void setDiagnostics(DiagnosticsEngine *Value);

DiagnosticConsumer &getDiagnosticClient() const {
assert(Diagnostics && Diagnostics->getClient() &&
"Compiler instance has no diagnostic client!");
return *Diagnostics->getClient();
}






void setVerboseOutputStream(raw_ostream &Value);


void setVerboseOutputStream(std::unique_ptr<raw_ostream> Value);


raw_ostream &getVerboseOutputStream() {
return *VerboseOutputStream;
}





bool hasTarget() const { return Target != nullptr; }

TargetInfo &getTarget() const {
assert(Target && "Compiler instance has no target!");
return *Target;
}


void setTarget(TargetInfo *Value);





TargetInfo *getAuxTarget() const { return AuxTarget.get(); }


void setAuxTarget(TargetInfo *Value);


bool createTarget();





llvm::vfs::FileSystem &getVirtualFileSystem() const;





bool hasFileManager() const { return FileMgr != nullptr; }


FileManager &getFileManager() const {
assert(FileMgr && "Compiler instance has no file manager!");
return *FileMgr;
}

void resetAndLeakFileManager() {
llvm::BuryPointer(FileMgr.get());
FileMgr.resetWithoutRelease();
}


void setFileManager(FileManager *Value);





bool hasSourceManager() const { return SourceMgr != nullptr; }


SourceManager &getSourceManager() const {
assert(SourceMgr && "Compiler instance has no source manager!");
return *SourceMgr;
}

void resetAndLeakSourceManager() {
llvm::BuryPointer(SourceMgr.get());
SourceMgr.resetWithoutRelease();
}


void setSourceManager(SourceManager *Value);





bool hasPreprocessor() const { return PP != nullptr; }


Preprocessor &getPreprocessor() const {
assert(PP && "Compiler instance has no preprocessor!");
return *PP;
}

std::shared_ptr<Preprocessor> getPreprocessorPtr() { return PP; }

void resetAndLeakPreprocessor() {
llvm::BuryPointer(new std::shared_ptr<Preprocessor>(PP));
}


void setPreprocessor(std::shared_ptr<Preprocessor> Value);





bool hasASTContext() const { return Context != nullptr; }

ASTContext &getASTContext() const {
assert(Context && "Compiler instance has no AST context!");
return *Context;
}

void resetAndLeakASTContext() {
llvm::BuryPointer(Context.get());
Context.resetWithoutRelease();
}


void setASTContext(ASTContext *Value);



void setSema(Sema *S);





bool hasASTConsumer() const { return (bool)Consumer; }

ASTConsumer &getASTConsumer() const {
assert(Consumer && "Compiler instance has no AST consumer!");
return *Consumer;
}



std::unique_ptr<ASTConsumer> takeASTConsumer() { return std::move(Consumer); }



void setASTConsumer(std::unique_ptr<ASTConsumer> Value);




bool hasSema() const { return (bool)TheSema; }

Sema &getSema() const {
assert(TheSema && "Compiler instance has no Sema object!");
return *TheSema;
}

std::unique_ptr<Sema> takeSema();
void resetAndLeakSema();





IntrusiveRefCntPtr<ASTReader> getASTReader() const;
void setASTReader(IntrusiveRefCntPtr<ASTReader> Reader);

std::shared_ptr<ModuleDependencyCollector> getModuleDepCollector() const;
void setModuleDepCollector(
std::shared_ptr<ModuleDependencyCollector> Collector);

std::shared_ptr<PCHContainerOperations> getPCHContainerOperations() const {
return ThePCHContainerOperations;
}



const PCHContainerWriter &getPCHContainerWriter() const {
assert(Invocation && "cannot determine module format without invocation");
StringRef Format = getHeaderSearchOpts().ModuleFormat;
auto *Writer = ThePCHContainerOperations->getWriterOrNull(Format);
if (!Writer) {
if (Diagnostics)
Diagnostics->Report(diag::err_module_format_unhandled) << Format;
llvm::report_fatal_error("unknown module format");
}
return *Writer;
}



const PCHContainerReader &getPCHContainerReader() const {
assert(Invocation && "cannot determine module format without invocation");
StringRef Format = getHeaderSearchOpts().ModuleFormat;
auto *Reader = ThePCHContainerOperations->getReaderOrNull(Format);
if (!Reader) {
if (Diagnostics)
Diagnostics->Report(diag::err_module_format_unhandled) << Format;
llvm::report_fatal_error("unknown module format");
}
return *Reader;
}





bool hasCodeCompletionConsumer() const { return (bool)CompletionConsumer; }

CodeCompleteConsumer &getCodeCompletionConsumer() const {
assert(CompletionConsumer &&
"Compiler instance has no code completion consumer!");
return *CompletionConsumer;
}



void setCodeCompletionConsumer(CodeCompleteConsumer *Value);





bool hasFrontendTimer() const { return (bool)FrontendTimer; }

llvm::Timer &getFrontendTimer() const {
assert(FrontendTimer && "Compiler instance has no frontend timer!");
return *FrontendTimer;
}









void clearOutputFiles(bool EraseFiles);

















void createDiagnostics(DiagnosticConsumer *Client = nullptr,
bool ShouldOwnClient = true);



















static IntrusiveRefCntPtr<DiagnosticsEngine>
createDiagnostics(DiagnosticOptions *Opts,
DiagnosticConsumer *Client = nullptr,
bool ShouldOwnClient = true,
const CodeGenOptions *CodeGenOpts = nullptr);




FileManager *
createFileManager(IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS = nullptr);


void createSourceManager(FileManager &FileMgr);



void createPreprocessor(TranslationUnitKind TUKind);

std::string getSpecificModuleCachePath(StringRef ModuleHash);
std::string getSpecificModuleCachePath() {
return getSpecificModuleCachePath(getInvocation().getModuleHash());
}


void createASTContext();



void createPCHExternalASTSource(
StringRef Path, DisableValidationForModuleKind DisableValidation,
bool AllowPCHWithCompilerErrors, void *DeserializationListener,
bool OwnDeserializationListener);




static IntrusiveRefCntPtr<ASTReader> createPCHExternalASTSource(
StringRef Path, StringRef Sysroot,
DisableValidationForModuleKind DisableValidation,
bool AllowPCHWithCompilerErrors, Preprocessor &PP,
InMemoryModuleCache &ModuleCache, ASTContext &Context,
const PCHContainerReader &PCHContainerRdr,
ArrayRef<std::shared_ptr<ModuleFileExtension>> Extensions,
ArrayRef<std::shared_ptr<DependencyCollector>> DependencyCollectors,
void *DeserializationListener, bool OwnDeserializationListener,
bool Preamble, bool UseGlobalModuleIndex);




void createCodeCompletionConsumer();



static CodeCompleteConsumer *createCodeCompletionConsumer(
Preprocessor &PP, StringRef Filename, unsigned Line, unsigned Column,
const CodeCompleteOptions &Opts, raw_ostream &OS);


void createSema(TranslationUnitKind TUKind,
CodeCompleteConsumer *CompletionConsumer);


void createFrontendTimer();










std::unique_ptr<raw_pwrite_stream> createDefaultOutputFile(
bool Binary = true, StringRef BaseInput = "", StringRef Extension = "",
bool RemoveFileOnSignal = true, bool CreateMissingDirectories = false,
bool ForceUseTemporary = false);





std::unique_ptr<raw_pwrite_stream>
createOutputFile(StringRef OutputPath, bool Binary, bool RemoveFileOnSignal,
bool UseTemporary, bool CreateMissingDirectories = false);

private:

















Expected<std::unique_ptr<raw_pwrite_stream>>
createOutputFileImpl(StringRef OutputPath, bool Binary,
bool RemoveFileOnSignal, bool UseTemporary,
bool CreateMissingDirectories);

public:
std::unique_ptr<raw_pwrite_stream> createNullOutputFile();









bool InitializeSourceManager(const FrontendInputFile &Input);





static bool InitializeSourceManager(const FrontendInputFile &Input,
DiagnosticsEngine &Diags,
FileManager &FileMgr,
SourceManager &SourceMgr);



void setOutputStream(std::unique_ptr<llvm::raw_pwrite_stream> OutStream) {
OutputStream = std::move(OutStream);
}

std::unique_ptr<llvm::raw_pwrite_stream> takeOutputStream() {
return std::move(OutputStream);
}

void createASTReader();

bool loadModuleFile(StringRef FileName);

private:











ModuleLoadResult findOrCompileModuleAndReadAST(StringRef ModuleName,
SourceLocation ImportLoc,
SourceLocation ModuleNameLoc,
bool IsInclusionDirective);

public:
ModuleLoadResult loadModule(SourceLocation ImportLoc, ModuleIdPath Path,
Module::NameVisibilityKind Visibility,
bool IsInclusionDirective) override;

void createModuleFromSource(SourceLocation ImportLoc, StringRef ModuleName,
StringRef Source) override;

void makeModuleVisible(Module *Mod, Module::NameVisibilityKind Visibility,
SourceLocation ImportLoc) override;

bool hadModuleLoaderFatalFailure() const {
return ModuleLoader::HadFatalFailure;
}

GlobalModuleIndex *loadGlobalModuleIndex(SourceLocation TriggerLoc) override;

bool lookupMissingImports(StringRef Name, SourceLocation TriggerLoc) override;

void addDependencyCollector(std::shared_ptr<DependencyCollector> Listener) {
DependencyCollectors.push_back(std::move(Listener));
}

void setExternalSemaSource(IntrusiveRefCntPtr<ExternalSemaSource> ESS);

InMemoryModuleCache &getModuleCache() const { return *ModuleCache; }
};

}

#endif
