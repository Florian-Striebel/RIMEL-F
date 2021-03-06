







#if !defined(LLVM_CLANG_AST_INTERP_BYTECODEGENERROR_H)
#define LLVM_CLANG_AST_INTERP_BYTECODEGENERROR_H

#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace interp {


struct ByteCodeGenError : public llvm::ErrorInfo<ByteCodeGenError> {
public:
ByteCodeGenError(SourceLocation Loc) : Loc(Loc) {}
ByteCodeGenError(const Stmt *S) : ByteCodeGenError(S->getBeginLoc()) {}
ByteCodeGenError(const Decl *D) : ByteCodeGenError(D->getBeginLoc()) {}

void log(raw_ostream &OS) const override { OS << "unimplemented feature"; }

const SourceLocation &getLoc() const { return Loc; }

static char ID;

private:

SourceLocation Loc;


std::error_code convertToErrorCode() const override {
return llvm::inconvertibleErrorCode();
}
};

}
}

#endif
