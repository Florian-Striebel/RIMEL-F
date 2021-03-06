







#if !defined(LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_WORKER_H)
#define LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_WORKER_H

#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LLVM.h"
#include "clang/Frontend/PCHContainerOperations.h"
#include "clang/Lex/PreprocessorExcludedConditionalDirectiveSkipMapping.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/DependencyScanning/DependencyScanningService.h"
#include "clang/Tooling/DependencyScanning/ModuleDepCollector.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include <string>

namespace clang {

class DependencyOutputOptions;

namespace tooling {
namespace dependencies {

class DependencyScanningWorkerFilesystem;

class DependencyConsumer {
public:
virtual ~DependencyConsumer() {}

virtual void
handleDependencyOutputOpts(const DependencyOutputOptions &Opts) = 0;

virtual void handleFileDependency(StringRef Filename) = 0;

virtual void handlePrebuiltModuleDependency(PrebuiltModuleDep PMD) = 0;

virtual void handleModuleDependency(ModuleDeps MD) = 0;

virtual void handleContextHash(std::string Hash) = 0;
};







class DependencyScanningWorker {
public:
DependencyScanningWorker(DependencyScanningService &Service);







llvm::Error computeDependencies(const std::string &Input,
StringRef WorkingDirectory,
const CompilationDatabase &CDB,
DependencyConsumer &Consumer);

private:
IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;
std::shared_ptr<PCHContainerOperations> PCHContainerOps;
std::unique_ptr<ExcludedPreprocessorDirectiveSkipMapping> PPSkipMappings;

llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> RealFS;



llvm::IntrusiveRefCntPtr<DependencyScanningWorkerFilesystem> DepFS;


llvm::IntrusiveRefCntPtr<FileManager> Files;
ScanningOutputFormat Format;
};

}
}
}

#endif
