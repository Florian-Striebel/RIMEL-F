







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULE_REQUIREMENTS_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULE_REQUIREMENTS_H

#include "clang/Basic/LLVM.h"
#include "clang/Tooling/Refactoring/ASTSelection.h"
#include "clang/Tooling/Refactoring/RefactoringDiagnostic.h"
#include "clang/Tooling/Refactoring/RefactoringOption.h"
#include "clang/Tooling/Refactoring/RefactoringRuleContext.h"
#include "llvm/Support/Error.h"
#include <type_traits>

namespace clang {
namespace tooling {














class RefactoringActionRuleRequirement {

};



class SourceSelectionRequirement : public RefactoringActionRuleRequirement {};



class SourceRangeSelectionRequirement : public SourceSelectionRequirement {
public:
Expected<SourceRange> evaluate(RefactoringRuleContext &Context) const {
if (Context.getSelectionRange().isValid())
return Context.getSelectionRange();
return Context.createDiagnosticError(diag::err_refactor_no_selection);
}
};






class ASTSelectionRequirement : public SourceRangeSelectionRequirement {
public:
Expected<SelectedASTNode> evaluate(RefactoringRuleContext &Context) const;
};










class CodeRangeASTSelectionRequirement : public ASTSelectionRequirement {
public:
Expected<CodeRangeASTSelection>
evaluate(RefactoringRuleContext &Context) const;
};


class RefactoringOptionsRequirement : public RefactoringActionRuleRequirement {
public:
virtual ~RefactoringOptionsRequirement() {}



virtual ArrayRef<std::shared_ptr<RefactoringOption>>
getRefactoringOptions() const = 0;
};





template <typename OptionType>
class OptionRequirement : public RefactoringOptionsRequirement {
public:
OptionRequirement() : Opt(createRefactoringOption<OptionType>()) {}

ArrayRef<std::shared_ptr<RefactoringOption>>
getRefactoringOptions() const final override {
return Opt;
}

Expected<typename OptionType::ValueType>
evaluate(RefactoringRuleContext &) const {
return static_cast<OptionType *>(Opt.get())->getValue();
}

private:





std::shared_ptr<RefactoringOption> Opt;
};

}
}

#endif
