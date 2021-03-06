












#if !defined(LLVM_CLANG_TOOLING_ALLTUSEXECUTION_H)
#define LLVM_CLANG_TOOLING_ALLTUSEXECUTION_H

#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/Execution.h"

namespace clang {
namespace tooling {



class AllTUsToolExecutor : public ToolExecutor {
public:
static const char *ExecutorName;




AllTUsToolExecutor(const CompilationDatabase &Compilations,
unsigned ThreadCount,
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());





AllTUsToolExecutor(CommonOptionsParser Options, unsigned ThreadCount,
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());

StringRef getExecutorName() const override { return ExecutorName; }

using ToolExecutor::execute;

llvm::Error
execute(llvm::ArrayRef<
std::pair<std::unique_ptr<FrontendActionFactory>, ArgumentsAdjuster>>
Actions) override;

ExecutionContext *getExecutionContext() override { return &Context; };

ToolResults *getToolResults() override { return Results.get(); }

void mapVirtualFile(StringRef FilePath, StringRef Content) override {
OverlayFiles[FilePath] = std::string(Content);
}

private:

llvm::Optional<CommonOptionsParser> OptionsParser;
const CompilationDatabase &Compilations;
std::unique_ptr<ToolResults> Results;
ExecutionContext Context;
llvm::StringMap<std::string> OverlayFiles;
unsigned ThreadCount;
};

extern llvm::cl::opt<unsigned> ExecutorConcurrency;
extern llvm::cl::opt<std::string> Filter;

}
}

#endif
