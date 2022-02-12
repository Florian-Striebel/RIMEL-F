







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_OPTIONS_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_OPTIONS_H

#include "clang/Basic/LLVM.h"
#include "clang/Tooling/Refactoring/RefactoringActionRuleRequirements.h"
#include "clang/Tooling/Refactoring/RefactoringOption.h"
#include "clang/Tooling/Refactoring/RefactoringOptionVisitor.h"
#include "llvm/Support/Error.h"
#include <type_traits>

namespace clang {
namespace tooling {


template <typename T,
typename = std::enable_if_t<traits::IsValidOptionType<T>::value>>
class OptionalRefactoringOption : public RefactoringOption {
public:
void passToVisitor(RefactoringOptionVisitor &Visitor) final override {
Visitor.visit(*this, Value);
}

bool isRequired() const override { return false; }

using ValueType = Optional<T>;

const ValueType &getValue() const { return Value; }

protected:
Optional<T> Value;
};


template <typename T,
typename = std::enable_if_t<traits::IsValidOptionType<T>::value>>
class RequiredRefactoringOption : public OptionalRefactoringOption<T> {
public:
using ValueType = T;

const ValueType &getValue() const {
return *OptionalRefactoringOption<T>::Value;
}
bool isRequired() const final override { return true; }
};

}
}

#endif
