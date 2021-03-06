











#if !defined(LLVM_CLANG_AST_INTERP_LINKEMITTER_H)
#define LLVM_CLANG_AST_INTERP_LINKEMITTER_H

#include "ByteCodeGenError.h"
#include "Context.h"
#include "InterpStack.h"
#include "InterpState.h"
#include "PrimType.h"
#include "Program.h"
#include "Source.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace interp {
class Context;
class SourceInfo;
enum Opcode : uint32_t;


class ByteCodeEmitter {
protected:
using LabelTy = uint32_t;
using AddrTy = uintptr_t;
using Local = Scope::Local;

public:

llvm::Expected<Function *> compileFunc(const FunctionDecl *F);

protected:
ByteCodeEmitter(Context &Ctx, Program &P) : Ctx(Ctx), P(P) {}

virtual ~ByteCodeEmitter() {}


void emitLabel(LabelTy Label);

LabelTy getLabel() { return ++NextLabel; }


virtual bool visitFunc(const FunctionDecl *E) = 0;
virtual bool visitExpr(const Expr *E) = 0;
virtual bool visitDecl(const VarDecl *E) = 0;


bool bail(const Stmt *S) { return bail(S->getBeginLoc()); }
bool bail(const Decl *D) { return bail(D->getBeginLoc()); }
bool bail(const SourceLocation &Loc);


bool jumpTrue(const LabelTy &Label);
bool jumpFalse(const LabelTy &Label);
bool jump(const LabelTy &Label);
bool fallthrough(const LabelTy &Label);


Local createLocal(Descriptor *D);


llvm::DenseMap<const ParmVarDecl *, unsigned> Params;

llvm::SmallVector<SmallVector<Local, 8>, 2> Descriptors;

private:

Context &Ctx;

Program &P;

LabelTy NextLabel = 0;

unsigned NextLocalOffset = 0;

llvm::Optional<SourceLocation> BailLocation;

llvm::DenseMap<LabelTy, unsigned> LabelOffsets;

llvm::DenseMap<LabelTy, llvm::SmallVector<unsigned, 5>> LabelRelocs;

std::vector<char> Code;

SourceMap SrcMap;


int32_t getOffset(LabelTy Label);


template <typename... Tys>
bool emitOp(Opcode Op, const Tys &... Args, const SourceInfo &L);

protected:
#define GET_LINK_PROTO
#include "Opcodes.inc"
#undef GET_LINK_PROTO
};

}
}

#endif
