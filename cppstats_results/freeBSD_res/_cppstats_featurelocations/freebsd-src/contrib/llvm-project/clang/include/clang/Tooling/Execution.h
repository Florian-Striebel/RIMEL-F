

























#if !defined(LLVM_CLANG_TOOLING_EXECUTION_H)
#define LLVM_CLANG_TOOLING_EXECUTION_H

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Registry.h"
#include "llvm/Support/StringSaver.h"

namespace clang {
namespace tooling {

extern llvm::cl::opt<std::string> ExecutorName;






class ToolResults {
public:
virtual ~ToolResults() = default;
virtual void addResult(StringRef Key, StringRef Value) = 0;
virtual std::vector<std::pair<llvm::StringRef, llvm::StringRef>>
AllKVResults() = 0;
virtual void forEachResult(
llvm::function_ref<void(StringRef Key, StringRef Value)> Callback) = 0;
};




class InMemoryToolResults : public ToolResults {
public:
InMemoryToolResults() : Strings(Arena) {}
void addResult(StringRef Key, StringRef Value) override;
std::vector<std::pair<llvm::StringRef, llvm::StringRef>>
AllKVResults() override;
void forEachResult(llvm::function_ref<void(StringRef Key, StringRef Value)>
Callback) override;

private:
llvm::BumpPtrAllocator Arena;
llvm::UniqueStringSaver Strings;

std::vector<std::pair<llvm::StringRef, llvm::StringRef>> KVResults;
};



class ExecutionContext {
public:
virtual ~ExecutionContext() {}


explicit ExecutionContext(ToolResults *Results) : Results(Results) {}


void reportResult(StringRef Key, StringRef Value);



virtual std::string getRevision() { return ""; }



virtual std::string getCorpus() { return ""; }


virtual std::string getCurrentCompilationUnit() { return ""; }

private:
ToolResults *Results;
};










class ToolExecutor {
public:
virtual ~ToolExecutor() {}


virtual StringRef getExecutorName() const = 0;


virtual llvm::Error
execute(llvm::ArrayRef<
std::pair<std::unique_ptr<FrontendActionFactory>, ArgumentsAdjuster>>
Actions) = 0;


llvm::Error execute(std::unique_ptr<FrontendActionFactory> Action);

llvm::Error execute(std::unique_ptr<FrontendActionFactory> Action,
ArgumentsAdjuster Adjuster);





virtual ExecutionContext *getExecutionContext() = 0;





virtual ToolResults *getToolResults() = 0;





virtual void mapVirtualFile(StringRef FilePath, StringRef Content) = 0;
};



class ToolExecutorPlugin {
public:
virtual ~ToolExecutorPlugin() {}




virtual llvm::Expected<std::unique_ptr<ToolExecutor>>
create(CommonOptionsParser &OptionsParser) = 0;
};










llvm::Expected<std::unique_ptr<ToolExecutor>>
createExecutorFromCommandLineArgs(int &argc, const char **argv,
llvm::cl::OptionCategory &Category,
const char *Overview = nullptr);

namespace internal {
llvm::Expected<std::unique_ptr<ToolExecutor>>
createExecutorFromCommandLineArgsImpl(int &argc, const char **argv,
llvm::cl::OptionCategory &Category,
const char *Overview = nullptr);
}

}
}

#endif
