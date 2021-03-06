







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULE_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_ACTION_RULE_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
namespace tooling {

class RefactoringOptionVisitor;
class RefactoringResultConsumer;
class RefactoringRuleContext;

struct RefactoringDescriptor {

StringRef Name;

StringRef Title;

StringRef Description;
};




class RefactoringActionRuleBase {
public:
virtual ~RefactoringActionRuleBase() {}





virtual void invoke(RefactoringResultConsumer &Consumer,
RefactoringRuleContext &Context) = 0;



};





class RefactoringActionRule : public RefactoringActionRuleBase {
public:


virtual bool hasSelectionRequirement() = 0;







virtual void visitRefactoringOptions(RefactoringOptionVisitor &Visitor) = 0;
};

}
}

#endif
