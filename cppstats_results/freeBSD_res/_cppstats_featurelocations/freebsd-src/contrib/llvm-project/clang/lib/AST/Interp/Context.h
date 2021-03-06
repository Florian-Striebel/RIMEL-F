














#if !defined(LLVM_CLANG_AST_INTERP_CONTEXT_H)
#define LLVM_CLANG_AST_INTERP_CONTEXT_H

#include "InterpStack.h"
#include "clang/AST/APValue.h"
#include "llvm/ADT/PointerIntPair.h"

namespace clang {
class ASTContext;
class LangOptions;
class Stmt;
class FunctionDecl;
class VarDecl;

namespace interp {
class Function;
class Program;
class State;
enum PrimType : unsigned;


class Context {
public:

Context(ASTContext &Ctx);


~Context();


bool isPotentialConstantExpr(State &Parent, const FunctionDecl *FnDecl);


bool evaluateAsRValue(State &Parent, const Expr *E, APValue &Result);


bool evaluateAsInitializer(State &Parent, const VarDecl *VD, APValue &Result);


ASTContext &getASTContext() const { return Ctx; }

const LangOptions &getLangOpts() const;

InterpStack &getStack() { return Stk; }

unsigned getCharBit() const;


llvm::Optional<PrimType> classify(QualType T);

private:

bool Run(State &Parent, Function *Func, APValue &Result);


bool Check(State &Parent, llvm::Expected<bool> &&R);

private:

ASTContext &Ctx;

InterpStack Stk;

std::unique_ptr<Program> P;
};

}
}

#endif
