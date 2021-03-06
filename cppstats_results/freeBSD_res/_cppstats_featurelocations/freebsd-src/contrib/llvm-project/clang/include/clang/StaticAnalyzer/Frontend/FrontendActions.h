







#if !defined(LLVM_CLANG_STATICANALYZER_FRONTEND_FRONTENDACTIONS_H)
#define LLVM_CLANG_STATICANALYZER_FRONTEND_FRONTENDACTIONS_H

#include "clang/Frontend/FrontendAction.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

namespace clang {

class Stmt;
class AnalyzerOptions;

namespace ento {

class CheckerManager;





class AnalysisAction : public ASTFrontendAction {
protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;
};









class ParseModelFileAction : public ASTFrontendAction {
public:
ParseModelFileAction(llvm::StringMap<Stmt *> &Bodies);
bool isModelParsingAction() const override { return true; }

protected:
std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
StringRef InFile) override;

private:
llvm::StringMap<Stmt *> &Bodies;
};

}
}

#endif
