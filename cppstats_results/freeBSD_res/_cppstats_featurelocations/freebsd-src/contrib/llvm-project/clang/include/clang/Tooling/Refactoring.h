
















#if !defined(LLVM_CLANG_TOOLING_REFACTORING_H)
#define LLVM_CLANG_TOOLING_REFACTORING_H

#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"
#include <map>
#include <string>

namespace clang {

class Rewriter;

namespace tooling {






class RefactoringTool : public ClangTool {
public:

RefactoringTool(const CompilationDatabase &Compilations,
ArrayRef<std::string> SourcePaths,
std::shared_ptr<PCHContainerOperations> PCHContainerOps =
std::make_shared<PCHContainerOperations>());



std::map<std::string, Replacements> &getReplacements();





int runAndSave(FrontendActionFactory *ActionFactory);










bool applyAllReplacements(Rewriter &Rewrite);

private:

int saveRewrittenFiles(Rewriter &Rewrite);

private:
std::map<std::string, Replacements> FileToReplaces;
};



















bool formatAndApplyAllReplacements(
const std::map<std::string, Replacements> &FileToReplaces,
Rewriter &Rewrite, StringRef Style = "file");

}
}

#endif
