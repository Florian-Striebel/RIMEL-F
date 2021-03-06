







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_RESULT_CONSUMER_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_RESULT_CONSUMER_H

#include "clang/Basic/LLVM.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Refactoring/Rename/SymbolOccurrences.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace tooling {





class RefactoringResultConsumer {
public:
virtual ~RefactoringResultConsumer() {}



virtual void handleError(llvm::Error Err) = 0;


virtual void handle(AtomicChanges SourceReplacements) {
defaultResultHandler();
}



virtual void handle(SymbolOccurrences Occurrences) { defaultResultHandler(); }

private:
void defaultResultHandler() {
handleError(llvm::make_error<llvm::StringError>(
"unsupported refactoring result", llvm::inconvertibleErrorCode()));
}
};

}
}

#endif
