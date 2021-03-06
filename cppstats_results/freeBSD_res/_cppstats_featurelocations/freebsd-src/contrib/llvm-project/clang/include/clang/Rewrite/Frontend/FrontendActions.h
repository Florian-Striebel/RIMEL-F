







#if !defined(LLVM_CLANG_REWRITE_FRONTEND_FRONTENDACTIONS_H)
#define LLVM_CLANG_REWRITE_FRONTEND_FRONTENDACTIONS_H

#include "clang/Frontend/FrontendAction.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
class FixItRewriter;
class FixItOptions;





class HTMLPrintAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
};

class FixItAction : public ASTFrontendAction {
protected:
std::unique_ptr<FixItRewriter> Rewriter;
std::unique_ptr<FixItOptions> FixItOpts;

std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

bool BeginSourceFileAction(CompilerInstance &CI) override;

void EndSourceFileAction() override;

bool hasASTFileSupport() const override { return false; }

public:
FixItAction();
~FixItAction() override;
};



class FixItRecompile : public WrapperFrontendAction {
public:
FixItRecompile(std::unique_ptr<FrontendAction> WrappedAction)
: WrapperFrontendAction(std::move(WrappedAction)) {}

protected:
bool BeginInvocation(CompilerInstance &CI) override;
};

class RewriteObjCAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
};

class RewriteMacrosAction : public PreprocessorFrontendAction {
protected:
void ExecuteAction() override;
};

class RewriteTestAction : public PreprocessorFrontendAction {
protected:
void ExecuteAction() override;
};

class RewriteIncludesAction : public PreprocessorFrontendAction {
std::shared_ptr<raw_ostream> OutputStream;
class RewriteImportsListener;
protected:
bool BeginSourceFileAction(CompilerInstance &CI) override;
void ExecuteAction() override;
};

}

#endif
