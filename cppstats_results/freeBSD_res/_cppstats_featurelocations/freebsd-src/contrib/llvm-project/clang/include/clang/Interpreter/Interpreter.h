












#if !defined(LLVM_CLANG_INTERPRETER_INTERPRETER_H)
#define LLVM_CLANG_INTERPRETER_INTERPRETER_H

#include "clang/Interpreter/PartialTranslationUnit.h"

#include "llvm/Support/Error.h"

#include <memory>
#include <vector>

namespace llvm {
namespace orc {
class ThreadSafeContext;
}
class Module;
}

namespace clang {

class CompilerInstance;
class DeclGroupRef;
class IncrementalExecutor;
class IncrementalParser;


class IncrementalCompilerBuilder {
public:
static llvm::Expected<std::unique_ptr<CompilerInstance>>
create(std::vector<const char *> &ClangArgv);
};


class Interpreter {
std::unique_ptr<llvm::orc::ThreadSafeContext> TSCtx;
std::unique_ptr<IncrementalParser> IncrParser;
std::unique_ptr<IncrementalExecutor> IncrExecutor;

Interpreter(std::unique_ptr<CompilerInstance> CI, llvm::Error &Err);

public:
~Interpreter();
static llvm::Expected<std::unique_ptr<Interpreter>>
create(std::unique_ptr<CompilerInstance> CI);
const CompilerInstance *getCompilerInstance() const;
llvm::Expected<PartialTranslationUnit &> Parse(llvm::StringRef Code);
llvm::Error Execute(PartialTranslationUnit &T);
llvm::Error ParseAndExecute(llvm::StringRef Code) {
auto PTU = Parse(Code);
if (!PTU)
return PTU.takeError();
if (PTU->TheModule)
return Execute(*PTU);
return llvm::Error::success();
}
};
}

#endif
