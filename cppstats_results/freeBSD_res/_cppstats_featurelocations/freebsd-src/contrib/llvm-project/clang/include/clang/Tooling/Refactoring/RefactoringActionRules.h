







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULES_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULES_H

#include "clang/Tooling/Refactoring/RefactoringActionRule.h"
#include "clang/Tooling/Refactoring/RefactoringActionRulesInternal.h"

namespace clang {
namespace tooling {





















template <typename RuleType, typename... RequirementTypes>
std::unique_ptr<RefactoringActionRule>
createRefactoringActionRule(const RequirementTypes &... Requirements);



using RefactoringActionRules =
std::vector<std::unique_ptr<RefactoringActionRule>>;






class SourceChangeRefactoringRule : public RefactoringActionRuleBase {
public:
void invoke(RefactoringResultConsumer &Consumer,
RefactoringRuleContext &Context) final override {
Expected<AtomicChanges> Changes = createSourceReplacements(Context);
if (!Changes)
Consumer.handleError(Changes.takeError());
else
Consumer.handle(std::move(*Changes));
}

private:
virtual Expected<AtomicChanges>
createSourceReplacements(RefactoringRuleContext &Context) = 0;
};







class FindSymbolOccurrencesRefactoringRule : public RefactoringActionRuleBase {
public:
void invoke(RefactoringResultConsumer &Consumer,
RefactoringRuleContext &Context) final override {
Expected<SymbolOccurrences> Occurrences = findSymbolOccurrences(Context);
if (!Occurrences)
Consumer.handleError(Occurrences.takeError());
else
Consumer.handle(std::move(*Occurrences));
}

private:
virtual Expected<SymbolOccurrences>
findSymbolOccurrences(RefactoringRuleContext &Context) = 0;
};

}
}

#endif
