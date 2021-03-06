







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_H

#include "clang/Basic/LLVM.h"
#include "clang/Tooling/Refactoring/RefactoringActionRules.h"
#include <vector>

namespace clang {
namespace tooling {





















class RefactoringAction {
public:
virtual ~RefactoringAction() {}



virtual StringRef getCommand() const = 0;

virtual StringRef getDescription() const = 0;

RefactoringActionRules createActiveActionRules();

protected:


virtual RefactoringActionRules createActionRules() const = 0;
};


std::vector<std::unique_ptr<RefactoringAction>> createRefactoringActions();

}
}

#endif
