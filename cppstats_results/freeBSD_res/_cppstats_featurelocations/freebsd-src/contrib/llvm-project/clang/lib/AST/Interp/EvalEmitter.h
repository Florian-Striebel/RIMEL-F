











#if !defined(LLVM_CLANG_AST_INTERP_EVALEMITTER_H)
#define LLVM_CLANG_AST_INTERP_EVALEMITTER_H

#include "ByteCodeGenError.h"
#include "Context.h"
#include "InterpStack.h"
#include "InterpState.h"
#include "PrimType.h"
#include "Program.h"
#include "Source.h"
#include "llvm/Support/Error.h"

namespace clang {
class FunctionDecl;
namespace interp {
class Context;
class Function;
class InterpState;
class Program;
class SourceInfo;
enum Opcode : uint32_t;


class EvalEmitter : public SourceMapper {
public:
using LabelTy = uint32_t;
using AddrTy = uintptr_t;
using Local = Scope::Local;

llvm::Expected<bool> interpretExpr(const Expr *E);
llvm::Expected<bool> interpretDecl(const VarDecl *VD);

protected:
EvalEmitter(Context &Ctx, Program &P, State &Parent, InterpStack &Stk,
APValue &Result);

virtual ~EvalEmitter() {}


void emitLabel(LabelTy Label);

LabelTy getLabel();


virtual bool visitExpr(const Expr *E) = 0;
virtual bool visitDecl(const VarDecl *VD) = 0;

bool bail(const Stmt *S) { return bail(S->getBeginLoc()); }
bool bail(const Decl *D) { return bail(D->getBeginLoc()); }
bool bail(const SourceLocation &Loc);


bool jumpTrue(const LabelTy &Label);
bool jumpFalse(const LabelTy &Label);
bool jump(const LabelTy &Label);
bool fallthrough(const LabelTy &Label);


Local createLocal(Descriptor *D);


SourceInfo getSource(Function *F, CodePtr PC) const override {
return F ? F->getSource(PC) : CurrentSource;
}


llvm::DenseMap<const ParmVarDecl *, unsigned> Params;

llvm::SmallVector<SmallVector<Local, 8>, 2> Descriptors;

private:

Context &Ctx;

Program &P;

InterpState S;

APValue &Result;


llvm::DenseMap<unsigned, std::unique_ptr<char[]>> Locals;



CodePtr OpPC;

llvm::Optional<SourceLocation> BailLocation;

SourceInfo CurrentSource;


LabelTy NextLabel = 1;

LabelTy CurrentLabel = 0;

LabelTy ActiveLabel = 0;



bool isActive() { return CurrentLabel == ActiveLabel; }


bool ExecuteCall(Function *F, Pointer &&This, const SourceInfo &Info);

bool ExecuteNoCall(const FunctionDecl *F, const SourceInfo &Info);

protected:
#define GET_EVAL_PROTO
#include "Opcodes.inc"
#undef GET_EVAL_PROTO
};

}
}

#endif
