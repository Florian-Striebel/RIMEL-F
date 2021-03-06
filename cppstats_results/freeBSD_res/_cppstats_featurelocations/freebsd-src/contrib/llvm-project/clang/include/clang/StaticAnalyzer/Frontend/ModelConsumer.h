













#if !defined(LLVM_CLANG_GR_MODELCONSUMER_H)
#define LLVM_CLANG_GR_MODELCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "llvm/ADT/StringMap.h"

namespace clang {

class Stmt;

namespace ento {





class ModelConsumer : public ASTConsumer {
public:
ModelConsumer(llvm::StringMap<Stmt *> &Bodies);

bool HandleTopLevelDecl(DeclGroupRef D) override;

private:
llvm::StringMap<Stmt *> &Bodies;
};
}
}

#endif
