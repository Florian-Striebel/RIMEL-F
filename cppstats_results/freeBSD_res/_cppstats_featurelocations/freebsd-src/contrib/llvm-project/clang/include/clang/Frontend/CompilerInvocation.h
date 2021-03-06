







#if !defined(LLVM_CLANG_FRONTEND_COMPILERINVOCATION_H)
#define LLVM_CLANG_FRONTEND_COMPILERINVOCATION_H

#include "clang/Basic/CodeGenOptions.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Frontend/DependencyOutputOptions.h"
#include "clang/Frontend/FrontendOptions.h"
#include "clang/Frontend/MigratorOptions.h"
#include "clang/Frontend/PreprocessorOutputOptions.h"
#include "clang/StaticAnalyzer/Core/AnalyzerOptions.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/ArrayRef.h"
#include <memory>
#include <string>

namespace llvm {

class Triple;

namespace opt {

class ArgList;

}

namespace vfs {

class FileSystem;

}

}

namespace clang {

class DiagnosticsEngine;
class HeaderSearchOptions;
class PreprocessorOptions;
class TargetOptions;








bool ParseDiagnosticArgs(DiagnosticOptions &Opts, llvm::opt::ArgList &Args,
DiagnosticsEngine *Diags = nullptr,
bool DefaultDiagColor = true);









class CompilerInvocationRefBase {
public:

std::shared_ptr<LangOptions> LangOpts;


std::shared_ptr<TargetOptions> TargetOpts;


IntrusiveRefCntPtr<DiagnosticOptions> DiagnosticOpts;


std::shared_ptr<HeaderSearchOptions> HeaderSearchOpts;


std::shared_ptr<PreprocessorOptions> PreprocessorOpts;


AnalyzerOptionsRef AnalyzerOpts;

CompilerInvocationRefBase();
CompilerInvocationRefBase(const CompilerInvocationRefBase &X);
CompilerInvocationRefBase(CompilerInvocationRefBase &&X);
CompilerInvocationRefBase &operator=(CompilerInvocationRefBase X);
CompilerInvocationRefBase &operator=(CompilerInvocationRefBase &&X);
~CompilerInvocationRefBase();

LangOptions *getLangOpts() { return LangOpts.get(); }
const LangOptions *getLangOpts() const { return LangOpts.get(); }

TargetOptions &getTargetOpts() { return *TargetOpts.get(); }
const TargetOptions &getTargetOpts() const { return *TargetOpts.get(); }

DiagnosticOptions &getDiagnosticOpts() const { return *DiagnosticOpts; }

HeaderSearchOptions &getHeaderSearchOpts() { return *HeaderSearchOpts; }

const HeaderSearchOptions &getHeaderSearchOpts() const {
return *HeaderSearchOpts;
}

std::shared_ptr<HeaderSearchOptions> getHeaderSearchOptsPtr() const {
return HeaderSearchOpts;
}

std::shared_ptr<PreprocessorOptions> getPreprocessorOptsPtr() {
return PreprocessorOpts;
}

PreprocessorOptions &getPreprocessorOpts() { return *PreprocessorOpts; }

const PreprocessorOptions &getPreprocessorOpts() const {
return *PreprocessorOpts;
}

AnalyzerOptionsRef getAnalyzerOpts() const { return AnalyzerOpts; }
};


class CompilerInvocationValueBase {
protected:
MigratorOptions MigratorOpts;


CodeGenOptions CodeGenOpts;


DependencyOutputOptions DependencyOutputOpts;


FileSystemOptions FileSystemOpts;


FrontendOptions FrontendOpts;


PreprocessorOutputOptions PreprocessorOutputOpts;

public:
MigratorOptions &getMigratorOpts() { return MigratorOpts; }
const MigratorOptions &getMigratorOpts() const { return MigratorOpts; }

CodeGenOptions &getCodeGenOpts() { return CodeGenOpts; }
const CodeGenOptions &getCodeGenOpts() const { return CodeGenOpts; }

DependencyOutputOptions &getDependencyOutputOpts() {
return DependencyOutputOpts;
}

const DependencyOutputOptions &getDependencyOutputOpts() const {
return DependencyOutputOpts;
}

FileSystemOptions &getFileSystemOpts() { return FileSystemOpts; }

const FileSystemOptions &getFileSystemOpts() const {
return FileSystemOpts;
}

FrontendOptions &getFrontendOpts() { return FrontendOpts; }
const FrontendOptions &getFrontendOpts() const { return FrontendOpts; }

PreprocessorOutputOptions &getPreprocessorOutputOpts() {
return PreprocessorOutputOpts;
}

const PreprocessorOutputOptions &getPreprocessorOutputOpts() const {
return PreprocessorOutputOpts;
}
};






class CompilerInvocation : public CompilerInvocationRefBase,
public CompilerInvocationValueBase {
public:











static bool CreateFromArgs(CompilerInvocation &Res,
ArrayRef<const char *> CommandLineArgs,
DiagnosticsEngine &Diags,
const char *Argv0 = nullptr);









static std::string GetResourcesPath(const char *Argv0, void *MainAddr);









static void
setLangDefaults(LangOptions &Opts, InputKind IK, const llvm::Triple &T,
std::vector<std::string> &Includes,
LangStandard::Kind LangStd = LangStandard::lang_unspecified);



std::string getModuleHash() const;

using StringAllocator = llvm::function_ref<const char *(const llvm::Twine &)>;








void generateCC1CommandLine(llvm::SmallVectorImpl<const char *> &Args,
StringAllocator SA) const;

private:
static bool CreateFromArgsImpl(CompilerInvocation &Res,
ArrayRef<const char *> CommandLineArgs,
DiagnosticsEngine &Diags, const char *Argv0);


static void GenerateDiagnosticArgs(const DiagnosticOptions &Opts,
SmallVectorImpl<const char *> &Args,
StringAllocator SA, bool DefaultDiagColor);


static bool ParseLangArgs(LangOptions &Opts, llvm::opt::ArgList &Args,
InputKind IK, const llvm::Triple &T,
std::vector<std::string> &Includes,
DiagnosticsEngine &Diags);


static void GenerateLangArgs(const LangOptions &Opts,
SmallVectorImpl<const char *> &Args,
StringAllocator SA, const llvm::Triple &T,
InputKind IK);


static bool ParseCodeGenArgs(CodeGenOptions &Opts, llvm::opt::ArgList &Args,
InputKind IK, DiagnosticsEngine &Diags,
const llvm::Triple &T,
const std::string &OutputFile,
const LangOptions &LangOptsRef);


static void GenerateCodeGenArgs(const CodeGenOptions &Opts,
SmallVectorImpl<const char *> &Args,
StringAllocator SA, const llvm::Triple &T,
const std::string &OutputFile,
const LangOptions *LangOpts);
};

IntrusiveRefCntPtr<llvm::vfs::FileSystem>
createVFSFromCompilerInvocation(const CompilerInvocation &CI,
DiagnosticsEngine &Diags);

IntrusiveRefCntPtr<llvm::vfs::FileSystem> createVFSFromCompilerInvocation(
const CompilerInvocation &CI, DiagnosticsEngine &Diags,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> BaseFS);

}

#endif
