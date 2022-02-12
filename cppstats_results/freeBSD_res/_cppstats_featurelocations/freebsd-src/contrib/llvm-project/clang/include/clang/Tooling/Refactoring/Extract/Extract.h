







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_EXTRACT_EXTRACT_H)
#define LLVM_CLANG_TOOLING_REFACTOR_EXTRACT_EXTRACT_H

#include "clang/Tooling/Refactoring/ASTSelection.h"
#include "clang/Tooling/Refactoring/RefactoringActionRules.h"

namespace clang {
namespace tooling {



class ExtractFunction final : public SourceChangeRefactoringRule {
public:





static Expected<ExtractFunction> initiate(RefactoringRuleContext &Context,
CodeRangeASTSelection Code,
Optional<std::string> DeclName);

static const RefactoringDescriptor &describe();

private:
ExtractFunction(CodeRangeASTSelection Code, Optional<std::string> DeclName)
: Code(std::move(Code)),
DeclName(DeclName ? std::move(*DeclName) : "extracted") {}

Expected<AtomicChanges>
createSourceReplacements(RefactoringRuleContext &Context) override;

CodeRangeASTSelection Code;




std::string DeclName;
};

}
}

#endif
