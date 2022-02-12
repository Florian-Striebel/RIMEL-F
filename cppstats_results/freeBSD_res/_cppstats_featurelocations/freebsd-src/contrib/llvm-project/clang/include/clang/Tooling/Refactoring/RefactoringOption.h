







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_OPTION_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_OPTION_H

#include "clang/Basic/LLVM.h"
#include <memory>
#include <type_traits>

namespace clang {
namespace tooling {

class RefactoringOptionVisitor;






class RefactoringOption {
public:
virtual ~RefactoringOption() {}




virtual StringRef getName() const = 0;

virtual StringRef getDescription() const = 0;



virtual bool isRequired() const = 0;






virtual void passToVisitor(RefactoringOptionVisitor &Visitor) = 0;
};





template <typename OptionType>
std::shared_ptr<OptionType> createRefactoringOption() {
static_assert(std::is_base_of<RefactoringOption, OptionType>::value,
"invalid option type");
return std::make_shared<OptionType>();
}

}
}

#endif
