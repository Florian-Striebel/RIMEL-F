













#if !defined(LLVM_CLANG_LIB_CODEGEN_CGOPENCLRUNTIME_H)
#define LLVM_CLANG_LIB_CODEGEN_CGOPENCLRUNTIME_H

#include "clang/AST/Expr.h"
#include "clang/AST/Type.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

namespace clang {

class BlockExpr;
class Expr;
class VarDecl;

namespace CodeGen {

class CodeGenFunction;
class CodeGenModule;

class CGOpenCLRuntime {
protected:
CodeGenModule &CGM;
llvm::Type *PipeROTy;
llvm::Type *PipeWOTy;
llvm::PointerType *SamplerTy;


struct EnqueuedBlockInfo {
llvm::Function *InvokeFunc;
llvm::Function *Kernel;
llvm::Value *BlockArg;
};

llvm::DenseMap<const Expr *, EnqueuedBlockInfo> EnqueuedBlockMap;

virtual llvm::Type *getPipeType(const PipeType *T, StringRef Name,
llvm::Type *&PipeTy);

public:
CGOpenCLRuntime(CodeGenModule &CGM) : CGM(CGM),
PipeROTy(nullptr), PipeWOTy(nullptr), SamplerTy(nullptr) {}
virtual ~CGOpenCLRuntime();




virtual void EmitWorkGroupLocalVarDecl(CodeGenFunction &CGF,
const VarDecl &D);

virtual llvm::Type *convertOpenCLSpecificType(const Type *T);

virtual llvm::Type *getPipeType(const PipeType *T);

llvm::PointerType *getSamplerType(const Type *T);



virtual llvm::Value *getPipeElemSize(const Expr *PipeArg);



virtual llvm::Value *getPipeElemAlign(const Expr *PipeArg);


llvm::PointerType *getGenericVoidPointerType();


EnqueuedBlockInfo emitOpenCLEnqueuedBlock(CodeGenFunction &CGF,
const Expr *E);







void recordBlockInfo(const BlockExpr *E, llvm::Function *InvokeF,
llvm::Value *Block);



llvm::Function *getInvokeFunction(const Expr *E);
};

}
}

#endif
