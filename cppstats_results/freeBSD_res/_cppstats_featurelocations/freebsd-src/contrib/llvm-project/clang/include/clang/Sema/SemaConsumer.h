












#if !defined(LLVM_CLANG_SEMA_SEMACONSUMER_H)
#define LLVM_CLANG_SEMA_SEMACONSUMER_H

#include "clang/AST/ASTConsumer.h"

namespace clang {
class Sema;




class SemaConsumer : public ASTConsumer {
virtual void anchor();
public:
SemaConsumer() {
ASTConsumer::SemaConsumer = true;
}




virtual void InitializeSema(Sema &S) {}


virtual void ForgetSema() {}


static bool classof(const ASTConsumer *Consumer) {
return Consumer->SemaConsumer;
}
};
}

#endif
