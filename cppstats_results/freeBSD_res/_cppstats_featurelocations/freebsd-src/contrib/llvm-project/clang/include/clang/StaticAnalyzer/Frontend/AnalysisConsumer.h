












#if !defined(LLVM_CLANG_STATICANALYZER_FRONTEND_ANALYSISCONSUMER_H)
#define LLVM_CLANG_STATICANALYZER_FRONTEND_ANALYSISCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/LLVM.h"
#include <functional>
#include <memory>

namespace clang {

class Preprocessor;
class DiagnosticsEngine;
class CodeInjector;
class CompilerInstance;

namespace ento {
class PathDiagnosticConsumer;
class CheckerManager;
class CheckerRegistry;

class AnalysisASTConsumer : public ASTConsumer {
public:
virtual void AddDiagnosticConsumer(PathDiagnosticConsumer *Consumer) = 0;











virtual void
AddCheckerRegistrationFn(std::function<void(CheckerRegistry &)> Fn) = 0;
};




std::unique_ptr<AnalysisASTConsumer>
CreateAnalysisConsumer(CompilerInstance &CI);

}

}

#endif
