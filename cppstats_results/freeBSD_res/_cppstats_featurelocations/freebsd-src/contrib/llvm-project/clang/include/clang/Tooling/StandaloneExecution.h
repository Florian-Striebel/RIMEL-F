











#if !defined(LLVM_CLANG_TOOLING_STANDALONEEXECUTION_H)
#define LLVM_CLANG_TOOLING_STANDALONEEXECUTION_H

#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/Execution.h"

namespace clang {
namespace tooling {









class StandaloneToolExecutor : public ToolExecutor {
public:
static const char *ExecutorName;



StandaloneToolExecutor(
const CompilationDatabase &Compilations,
llvm::ArrayRef<std::string> SourcePaths,
IntrusiveRefCntPtr<llvm::vfs::FileSystem> BaseFS =
llvm::vfs::getRealFileSystem(),
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());





StandaloneToolExecutor(
CommonOptionsParser Options,
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());

StringRef getExecutorName() const override { return ExecutorName; }

using ToolExecutor::execute;

llvm::Error
execute(llvm::ArrayRef<
std::pair<std::unique_ptr<FrontendActionFactory>, ArgumentsAdjuster>>
Actions) override;


void setDiagnosticConsumer(DiagnosticConsumer *DiagConsumer) {
Tool.setDiagnosticConsumer(DiagConsumer);
}

ExecutionContext *getExecutionContext() override { return &Context; };

ToolResults *getToolResults() override { return &Results; }

llvm::ArrayRef<std::string> getSourcePaths() const {
return Tool.getSourcePaths();
}

void mapVirtualFile(StringRef FilePath, StringRef Content) override {
Tool.mapVirtualFile(FilePath, Content);
}




FileManager &getFiles() { return Tool.getFiles(); }

private:

llvm::Optional<CommonOptionsParser> OptionsParser;


ClangTool Tool;
ExecutionContext Context;
InMemoryToolResults Results;
ArgumentsAdjuster ArgsAdjuster;
};

}
}

#endif
