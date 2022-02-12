











#if !defined(LLVM_CLANG_FRONTEND_UTILS_H)
#define LLVM_CLANG_FRONTEND_UTILS_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Driver/OptionUtils.h"
#include "clang/Frontend/DependencyOutputOptions.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Option/OptSpecifier.h"
#include "llvm/Support/FileCollector.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace llvm {

class Triple;

}

namespace clang {

class ASTReader;
class CompilerInstance;
class CompilerInvocation;
class DiagnosticsEngine;
class ExternalSemaSource;
class FrontendOptions;
class HeaderSearch;
class HeaderSearchOptions;
class LangOptions;
class PCHContainerReader;
class Preprocessor;
class PreprocessorOptions;
class PreprocessorOutputOptions;


void ApplyHeaderSearchOptions(HeaderSearch &HS,
const HeaderSearchOptions &HSOpts,
const LangOptions &Lang,
const llvm::Triple &triple);



void InitializePreprocessor(Preprocessor &PP, const PreprocessorOptions &PPOpts,
const PCHContainerReader &PCHContainerRdr,
const FrontendOptions &FEOpts);


void DoPrintPreprocessedInput(Preprocessor &PP, raw_ostream *OS,
const PreprocessorOutputOptions &Opts);





class DependencyCollector {
public:
virtual ~DependencyCollector();

virtual void attachToPreprocessor(Preprocessor &PP);
virtual void attachToASTReader(ASTReader &R);
ArrayRef<std::string> getDependencies() const { return Dependencies; }





virtual bool sawDependency(StringRef Filename, bool FromModule,
bool IsSystem, bool IsModuleFile, bool IsMissing);


virtual void finishedMainFile(DiagnosticsEngine &Diags) {}


virtual bool needSystemDependencies() { return false; }



virtual void maybeAddDependency(StringRef Filename, bool FromModule,
bool IsSystem, bool IsModuleFile,
bool IsMissing);

protected:


bool addDependency(StringRef Filename);

private:
llvm::StringSet<> Seen;
std::vector<std::string> Dependencies;
};





class DependencyFileGenerator : public DependencyCollector {
public:
DependencyFileGenerator(const DependencyOutputOptions &Opts);

void attachToPreprocessor(Preprocessor &PP) override;

void finishedMainFile(DiagnosticsEngine &Diags) override;

bool needSystemDependencies() final override { return IncludeSystemHeaders; }

bool sawDependency(StringRef Filename, bool FromModule, bool IsSystem,
bool IsModuleFile, bool IsMissing) final override;

protected:
void outputDependencyFile(llvm::raw_ostream &OS);

private:
void outputDependencyFile(DiagnosticsEngine &Diags);

std::string OutputFile;
std::vector<std::string> Targets;
bool IncludeSystemHeaders;
bool PhonyTarget;
bool AddMissingHeaderDeps;
bool SeenMissingHeader;
bool IncludeModuleFiles;
DependencyOutputFormat OutputFormat;
unsigned InputFileIndex;
};



class ModuleDependencyCollector : public DependencyCollector {
std::string DestDir;
bool HasErrors = false;
llvm::StringSet<> Seen;
llvm::vfs::YAMLVFSWriter VFSWriter;
llvm::FileCollector::PathCanonicalizer Canonicalizer;

std::error_code copyToRoot(StringRef Src, StringRef Dst = {});

public:
ModuleDependencyCollector(std::string DestDir)
: DestDir(std::move(DestDir)) {}
~ModuleDependencyCollector() override { writeFileMap(); }

StringRef getDest() { return DestDir; }
virtual bool insertSeen(StringRef Filename) { return Seen.insert(Filename).second; }
virtual void addFile(StringRef Filename, StringRef FileDst = {});

virtual void addFileMapping(StringRef VPath, StringRef RPath) {
VFSWriter.addFileMapping(VPath, RPath);
}

void attachToPreprocessor(Preprocessor &PP) override;
void attachToASTReader(ASTReader &R) override;

virtual void writeFileMap();
virtual bool hasErrors() { return HasErrors; }
};



void AttachDependencyGraphGen(Preprocessor &PP, StringRef OutputFile,
StringRef SysRoot);













void AttachHeaderIncludeGen(Preprocessor &PP,
const DependencyOutputOptions &DepOpts,
bool ShowAllHeaders = false,
StringRef OutputPath = {},
bool ShowDepth = true, bool MSStyle = false);



IntrusiveRefCntPtr<ExternalSemaSource>
createChainedIncludesSource(CompilerInstance &CI,
IntrusiveRefCntPtr<ExternalSemaSource> &Reader);













std::unique_ptr<CompilerInvocation> createInvocationFromCommandLine(
ArrayRef<const char *> Args,
IntrusiveRefCntPtr<DiagnosticsEngine> Diags =
IntrusiveRefCntPtr<DiagnosticsEngine>(),
IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS = nullptr,
bool ShouldRecoverOnErrors = false,
std::vector<std::string> *CC1Args = nullptr);



}

#endif
