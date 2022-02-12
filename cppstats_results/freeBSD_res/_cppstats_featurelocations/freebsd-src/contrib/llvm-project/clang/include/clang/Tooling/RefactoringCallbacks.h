


























#if !defined(LLVM_CLANG_TOOLING_REFACTORINGCALLBACKS_H)
#define LLVM_CLANG_TOOLING_REFACTORINGCALLBACKS_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Refactoring.h"

namespace clang {
namespace tooling {




class RefactoringCallback : public ast_matchers::MatchFinder::MatchCallback {
public:
RefactoringCallback();
Replacements &getReplacements();

protected:
Replacements Replace;
};





class ASTMatchRefactorer {
public:
explicit ASTMatchRefactorer(
std::map<std::string, Replacements> &FileToReplaces);

template <typename T>
void addMatcher(const T &Matcher, RefactoringCallback *Callback) {
MatchFinder.addMatcher(Matcher, Callback);
Callbacks.push_back(Callback);
}

void addDynamicMatcher(const ast_matchers::internal::DynTypedMatcher &Matcher,
RefactoringCallback *Callback);

std::unique_ptr<ASTConsumer> newASTConsumer();

private:
friend class RefactoringASTConsumer;
std::vector<RefactoringCallback *> Callbacks;
ast_matchers::MatchFinder MatchFinder;
std::map<std::string, Replacements> &FileToReplaces;
};



class ReplaceStmtWithText : public RefactoringCallback {
public:
ReplaceStmtWithText(StringRef FromId, StringRef ToText);
void run(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
std::string FromId;
std::string ToText;
};







class ReplaceNodeWithTemplate : public RefactoringCallback {
public:
static llvm::Expected<std::unique_ptr<ReplaceNodeWithTemplate>>
create(StringRef FromId, StringRef ToTemplate);
void run(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
struct TemplateElement {
enum { Literal, Identifier } Type;
std::string Value;
};
ReplaceNodeWithTemplate(llvm::StringRef FromId,
std::vector<TemplateElement> Template);
std::string FromId;
std::vector<TemplateElement> Template;
};



class ReplaceStmtWithStmt : public RefactoringCallback {
public:
ReplaceStmtWithStmt(StringRef FromId, StringRef ToId);
void run(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
std::string FromId;
std::string ToId;
};




class ReplaceIfStmtWithItsBody : public RefactoringCallback {
public:
ReplaceIfStmtWithItsBody(StringRef Id, bool PickTrueBranch);
void run(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
std::string Id;
const bool PickTrueBranch;
};

}
}

#endif
