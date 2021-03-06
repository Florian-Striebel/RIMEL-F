





















#if !defined(LLVM_CLANG_SA_FRONTEND_MODELINJECTOR_H)
#define LLVM_CLANG_SA_FRONTEND_MODELINJECTOR_H

#include "clang/Analysis/CodeInjector.h"
#include "llvm/ADT/StringMap.h"

namespace clang {

class CompilerInstance;
class ASTUnit;
class ASTReader;
class NamedDecl;
class Module;

namespace ento {
class ModelInjector : public CodeInjector {
public:
ModelInjector(CompilerInstance &CI);
Stmt *getBody(const FunctionDecl *D) override;
Stmt *getBody(const ObjCMethodDecl *D) override;

private:













void onBodySynthesis(const NamedDecl *D);

CompilerInstance &CI;



llvm::StringMap<Stmt *> Bodies;
};
}
}

#endif
