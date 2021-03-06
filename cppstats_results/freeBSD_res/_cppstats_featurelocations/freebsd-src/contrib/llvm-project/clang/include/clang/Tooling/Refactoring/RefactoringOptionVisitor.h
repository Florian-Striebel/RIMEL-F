







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_OPTION_VISITOR_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_OPTION_VISITOR_H

#include "clang/Basic/LLVM.h"
#include <type_traits>

namespace clang {
namespace tooling {

class RefactoringOption;






class RefactoringOptionVisitor {
public:
virtual ~RefactoringOptionVisitor() {}

virtual void visit(const RefactoringOption &Opt,
Optional<std::string> &Value) = 0;
};

namespace traits {
namespace internal {

template <typename T> struct HasHandle {
private:
template <typename ClassT>
static auto check(ClassT *) -> typename std::is_same<
decltype(std::declval<RefactoringOptionVisitor>().visit(
std::declval<RefactoringOption>(), *std::declval<Optional<T> *>())),
void>::type;

template <typename> static std::false_type check(...);

public:
using Type = decltype(check<RefactoringOptionVisitor>(nullptr));
};

}



template <typename T>
struct IsValidOptionType : internal::HasHandle<T>::Type {};

}
}
}

#endif
