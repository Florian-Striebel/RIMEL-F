







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULES_INTERNAL_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULES_INTERNAL_H

#include "clang/Basic/LLVM.h"
#include "clang/Tooling/Refactoring/RefactoringActionRule.h"
#include "clang/Tooling/Refactoring/RefactoringActionRuleRequirements.h"
#include "clang/Tooling/Refactoring/RefactoringResultConsumer.h"
#include "clang/Tooling/Refactoring/RefactoringRuleContext.h"
#include "llvm/Support/Error.h"
#include <type_traits>

namespace clang {
namespace tooling {
namespace internal {

inline llvm::Error findError() { return llvm::Error::success(); }

inline void ignoreError() {}

template <typename FirstT, typename... RestT>
void ignoreError(Expected<FirstT> &First, Expected<RestT> &... Rest) {
if (!First)
llvm::consumeError(First.takeError());
ignoreError(Rest...);
}



template <typename FirstT, typename... RestT>
llvm::Error findError(Expected<FirstT> &First, Expected<RestT> &... Rest) {
if (!First) {
ignoreError(Rest...);
return First.takeError();
}
return findError(Rest...);
}

template <typename RuleType, typename... RequirementTypes, size_t... Is>
void invokeRuleAfterValidatingRequirements(
RefactoringResultConsumer &Consumer, RefactoringRuleContext &Context,
const std::tuple<RequirementTypes...> &Requirements,
std::index_sequence<Is...>) {

auto Values =
std::make_tuple(std::get<Is>(Requirements).evaluate(Context)...);
auto Err = findError(std::get<Is>(Values)...);
if (Err)
return Consumer.handleError(std::move(Err));


auto Rule =
RuleType::initiate(Context, std::move((*std::get<Is>(Values)))...);
if (!Rule)
return Consumer.handleError(Rule.takeError());
Rule->invoke(Consumer, Context);
}

inline void visitRefactoringOptionsImpl(RefactoringOptionVisitor &) {}



template <typename FirstT, typename... RestT>
void visitRefactoringOptionsImpl(RefactoringOptionVisitor &Visitor,
const FirstT &First, const RestT &... Rest) {
struct OptionGatherer {
RefactoringOptionVisitor &Visitor;

void operator()(const RefactoringOptionsRequirement &Requirement) {
for (const auto &Option : Requirement.getRefactoringOptions())
Option->passToVisitor(Visitor);
}
void operator()(const RefactoringActionRuleRequirement &) {}
};
(OptionGatherer{Visitor})(First);
return visitRefactoringOptionsImpl(Visitor, Rest...);
}

template <typename... RequirementTypes, size_t... Is>
void visitRefactoringOptions(
RefactoringOptionVisitor &Visitor,
const std::tuple<RequirementTypes...> &Requirements,
std::index_sequence<Is...>) {
visitRefactoringOptionsImpl(Visitor, std::get<Is>(Requirements)...);
}



template <typename Base, typename First, typename... Rest>
struct HasBaseOf : std::conditional<HasBaseOf<Base, First>::value ||
HasBaseOf<Base, Rest...>::value,
std::true_type, std::false_type>::type {};

template <typename Base, typename T>
struct HasBaseOf<Base, T> : std::is_base_of<Base, T> {};



template <typename Base, typename First, typename... Rest>
struct AreBaseOf : std::conditional<AreBaseOf<Base, First>::value &&
AreBaseOf<Base, Rest...>::value,
std::true_type, std::false_type>::type {};

template <typename Base, typename T>
struct AreBaseOf<Base, T> : std::is_base_of<Base, T> {};

}

template <typename RuleType, typename... RequirementTypes>
std::unique_ptr<RefactoringActionRule>
createRefactoringActionRule(const RequirementTypes &... Requirements) {
static_assert(std::is_base_of<RefactoringActionRuleBase, RuleType>::value,
"Expected a refactoring action rule type");
static_assert(internal::AreBaseOf<RefactoringActionRuleRequirement,
RequirementTypes...>::value,
"Expected a list of refactoring action rules");

class Rule final : public RefactoringActionRule {
public:
Rule(std::tuple<RequirementTypes...> Requirements)
: Requirements(Requirements) {}

void invoke(RefactoringResultConsumer &Consumer,
RefactoringRuleContext &Context) override {
internal::invokeRuleAfterValidatingRequirements<RuleType>(
Consumer, Context, Requirements,
std::index_sequence_for<RequirementTypes...>());
}

bool hasSelectionRequirement() override {
return internal::HasBaseOf<SourceSelectionRequirement,
RequirementTypes...>::value;
}

void visitRefactoringOptions(RefactoringOptionVisitor &Visitor) override {
internal::visitRefactoringOptions(
Visitor, Requirements,
std::index_sequence_for<RequirementTypes...>());
}
private:
std::tuple<RequirementTypes...> Requirements;
};

return std::make_unique<Rule>(std::make_tuple(Requirements...));
}

}
}

#endif
