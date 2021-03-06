











#if !defined(LLVM_CLANG_LIB_INTERPRETER_INCREMENTALEXECUTOR_H)
#define LLVM_CLANG_LIB_INTERPRETER_INCREMENTALEXECUTOR_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"

#include <memory>

namespace llvm {
class Error;
class Module;
namespace orc {
class LLJIT;
class ThreadSafeContext;
}
}

namespace clang {
class IncrementalExecutor {
using CtorDtorIterator = llvm::orc::CtorDtorIterator;
std::unique_ptr<llvm::orc::LLJIT> Jit;
llvm::orc::ThreadSafeContext &TSCtx;

public:
IncrementalExecutor(llvm::orc::ThreadSafeContext &TSC, llvm::Error &Err,
const llvm::Triple &Triple);
~IncrementalExecutor();

llvm::Error addModule(std::unique_ptr<llvm::Module> M);
llvm::Error runCtors() const;
};

}

#endif
