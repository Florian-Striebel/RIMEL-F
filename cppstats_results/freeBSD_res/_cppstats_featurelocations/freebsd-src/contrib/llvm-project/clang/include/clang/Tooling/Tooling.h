



























#if !defined(LLVM_CLANG_TOOLING_TOOLING_H)
#define LLVM_CLANG_TOOLING_TOOLING_H

#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LLVM.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/PCHContainerOperations.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace clang {

class CompilerInstance;
class CompilerInvocation;
class DiagnosticConsumer;
class DiagnosticsEngine;
class SourceManager;

namespace driver {

class Compilation;

}

namespace tooling {

class CompilationDatabase;





const llvm::opt::ArgStringList *
getCC1Arguments(DiagnosticsEngine *Diagnostics,
driver::Compilation *Compilation);





class ToolAction {
public:
virtual ~ToolAction();


virtual bool
runInvocation(std::shared_ptr<CompilerInvocation> Invocation,
FileManager *Files,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
DiagnosticConsumer *DiagConsumer) = 0;
};







class FrontendActionFactory : public ToolAction {
public:
~FrontendActionFactory() override;


bool runInvocation(std::shared_ptr<CompilerInvocation> Invocation,
FileManager *Files,
std::shared_ptr<PCHContainerOperations> PCHContainerOps,
DiagnosticConsumer *DiagConsumer) override;


virtual std::unique_ptr<FrontendAction> create() = 0;
};








template <typename T>
std::unique_ptr<FrontendActionFactory> newFrontendActionFactory();




class SourceFileCallbacks {
public:
virtual ~SourceFileCallbacks() = default;



virtual bool handleBeginSource(CompilerInstance &CI) {
return true;
}



virtual void handleEndSource() {}
};












template <typename FactoryT>
inline std::unique_ptr<FrontendActionFactory> newFrontendActionFactory(
FactoryT *ConsumerFactory, SourceFileCallbacks *Callbacks = nullptr);










bool runToolOnCode(std::unique_ptr<FrontendAction> ToolAction, const Twine &Code,
const Twine &FileName = "input.cc",
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());



using FileContentMappings = std::vector<std::pair<std::string, std::string>>;














bool runToolOnCodeWithArgs(
std::unique_ptr<FrontendAction> ToolAction, const Twine &Code,
const std::vector<std::string> &Args, const Twine &FileName = "input.cc",
const Twine &ToolName = "clang-tool",
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>(),
const FileContentMappings &VirtualMappedFiles = FileContentMappings());


bool runToolOnCodeWithArgs(
std::unique_ptr<FrontendAction> ToolAction, const Twine &Code,
llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS,
const std::vector<std::string> &Args, const Twine &FileName = "input.cc",
const Twine &ToolName = "clang-tool",
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());









std::unique_ptr<ASTUnit>
buildASTFromCode(StringRef Code, StringRef FileName = "input.cc",
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());














std::unique_ptr<ASTUnit> buildASTFromCodeWithArgs(
StringRef Code, const std::vector<std::string> &Args,
StringRef FileName = "input.cc", StringRef ToolName = "clang-tool",
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>(),
ArgumentsAdjuster Adjuster = getClangStripDependencyFileAdjuster(),
const FileContentMappings &VirtualMappedFiles = FileContentMappings(),
DiagnosticConsumer *DiagConsumer = nullptr);


class ToolInvocation {
public:











ToolInvocation(std::vector<std::string> CommandLine,
std::unique_ptr<FrontendAction> FAction, FileManager *Files,
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());








ToolInvocation(std::vector<std::string> CommandLine, ToolAction *Action,
FileManager *Files,
std::shared_ptr<PCHContainerOperations> PCHContainerOps);

~ToolInvocation();


void setDiagnosticConsumer(DiagnosticConsumer *DiagConsumer) {
this->DiagConsumer = DiagConsumer;
}




bool run();

private:
bool runInvocation(const char *BinaryName,
driver::Compilation *Compilation,
std::shared_ptr<CompilerInvocation> Invocation,
std::shared_ptr<PCHContainerOperations> PCHContainerOps);

std::vector<std::string> CommandLine;
ToolAction *Action;
bool OwnsAction;
FileManager *Files;
std::shared_ptr<PCHContainerOperations> PCHContainerOps;
DiagnosticConsumer *DiagConsumer = nullptr;
};








class ClangTool {
public:












ClangTool(const CompilationDatabase &Compilations,
ArrayRef<std::string> SourcePaths,
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>(),
IntrusiveRefCntPtr<llvm::vfs::FileSystem> BaseFS =
llvm::vfs::getRealFileSystem(),
IntrusiveRefCntPtr<FileManager> Files = nullptr);

~ClangTool();


void setDiagnosticConsumer(DiagnosticConsumer *DiagConsumer) {
this->DiagConsumer = DiagConsumer;
}





void mapVirtualFile(StringRef FilePath, StringRef Content);





void appendArgumentsAdjuster(ArgumentsAdjuster Adjuster);


void clearArgumentsAdjusters();







int run(ToolAction *Action);



int buildASTs(std::vector<std::unique_ptr<ASTUnit>> &ASTs);




void setRestoreWorkingDir(bool RestoreCWD);



void setPrintErrorMessage(bool PrintErrorMessage);




FileManager &getFiles() { return *Files; }

llvm::ArrayRef<std::string> getSourcePaths() const { return SourcePaths; }

private:
const CompilationDatabase &Compilations;
std::vector<std::string> SourcePaths;
std::shared_ptr<PCHContainerOperations> PCHContainerOps;

llvm::IntrusiveRefCntPtr<llvm::vfs::OverlayFileSystem> OverlayFileSystem;
llvm::IntrusiveRefCntPtr<llvm::vfs::InMemoryFileSystem> InMemoryFileSystem;
llvm::IntrusiveRefCntPtr<FileManager> Files;


std::vector<std::pair<StringRef, StringRef>> MappedFileContents;

llvm::StringSet<> SeenWorkingDirectories;

ArgumentsAdjuster ArgsAdjuster;

DiagnosticConsumer *DiagConsumer = nullptr;

bool RestoreCWD = true;
bool PrintErrorMessage = true;
};

template <typename T>
std::unique_ptr<FrontendActionFactory> newFrontendActionFactory() {
class SimpleFrontendActionFactory : public FrontendActionFactory {
public:
std::unique_ptr<FrontendAction> create() override {
return std::make_unique<T>();
}
};

return std::unique_ptr<FrontendActionFactory>(
new SimpleFrontendActionFactory);
}

template <typename FactoryT>
inline std::unique_ptr<FrontendActionFactory> newFrontendActionFactory(
FactoryT *ConsumerFactory, SourceFileCallbacks *Callbacks) {
class FrontendActionFactoryAdapter : public FrontendActionFactory {
public:
explicit FrontendActionFactoryAdapter(FactoryT *ConsumerFactory,
SourceFileCallbacks *Callbacks)
: ConsumerFactory(ConsumerFactory), Callbacks(Callbacks) {}

std::unique_ptr<FrontendAction> create() override {
return std::make_unique<ConsumerFactoryAdaptor>(ConsumerFactory,
Callbacks);
}

private:
class ConsumerFactoryAdaptor : public ASTFrontendAction {
public:
ConsumerFactoryAdaptor(FactoryT *ConsumerFactory,
SourceFileCallbacks *Callbacks)
: ConsumerFactory(ConsumerFactory), Callbacks(Callbacks) {}

std::unique_ptr<ASTConsumer>
CreateASTConsumer(CompilerInstance &, StringRef) override {
return ConsumerFactory->newASTConsumer();
}

protected:
bool BeginSourceFileAction(CompilerInstance &CI) override {
if (!ASTFrontendAction::BeginSourceFileAction(CI))
return false;
if (Callbacks)
return Callbacks->handleBeginSource(CI);
return true;
}

void EndSourceFileAction() override {
if (Callbacks)
Callbacks->handleEndSource();
ASTFrontendAction::EndSourceFileAction();
}

private:
FactoryT *ConsumerFactory;
SourceFileCallbacks *Callbacks;
};
FactoryT *ConsumerFactory;
SourceFileCallbacks *Callbacks;
};

return std::unique_ptr<FrontendActionFactory>(
new FrontendActionFactoryAdapter(ConsumerFactory, Callbacks));
}













std::string getAbsolutePath(StringRef File);


llvm::Expected<std::string> getAbsolutePath(llvm::vfs::FileSystem &FS,
StringRef File);





















void addTargetAndModeForProgramName(std::vector<std::string> &CommandLine,
StringRef InvokedAs);


CompilerInvocation *newInvocation(DiagnosticsEngine *Diagnostics,
const llvm::opt::ArgStringList &CC1Args,
const char *const BinaryName);

}

}

#endif
