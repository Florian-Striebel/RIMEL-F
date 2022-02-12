












#if !defined(LLVM_CLANG_LIB_CODEGEN_CONSTANTEMITTER_H)
#define LLVM_CLANG_LIB_CODEGEN_CONSTANTEMITTER_H

#include "CodeGenFunction.h"
#include "CodeGenModule.h"

namespace clang {
namespace CodeGen {

class ConstantEmitter {
public:
CodeGenModule &CGM;
CodeGenFunction *const CGF;

private:
bool Abstract = false;


bool InitializedNonAbstract = false;


bool Finalized = false;


bool Failed = false;


bool InConstantContext = false;



LangAS DestAddressSpace;

llvm::SmallVector<std::pair<llvm::Constant *, llvm::GlobalVariable*>, 4>
PlaceholderAddresses;

public:
ConstantEmitter(CodeGenModule &CGM, CodeGenFunction *CGF = nullptr)
: CGM(CGM), CGF(CGF) {}




ConstantEmitter(CodeGenFunction &CGF)
: CGM(CGF.CGM), CGF(&CGF) {}

ConstantEmitter(const ConstantEmitter &other) = delete;
ConstantEmitter &operator=(const ConstantEmitter &other) = delete;

~ConstantEmitter();


bool isAbstract() const {
return Abstract;
}



llvm::Constant *tryEmitForInitializer(const VarDecl &D);
llvm::Constant *tryEmitForInitializer(const Expr *E, LangAS destAddrSpace,
QualType destType);
llvm::Constant *emitForInitializer(const APValue &value, LangAS destAddrSpace,
QualType destType);

void finalize(llvm::GlobalVariable *global);

















llvm::Constant *tryEmitAbstractForInitializer(const VarDecl &D);





llvm::Constant *emitAbstract(const Expr *E, QualType T);
llvm::Constant *emitAbstract(SourceLocation loc, const APValue &value,
QualType T);


llvm::Constant *tryEmitAbstract(const Expr *E, QualType T);
llvm::Constant *tryEmitAbstractForMemory(const Expr *E, QualType T);

llvm::Constant *tryEmitAbstract(const APValue &value, QualType T);
llvm::Constant *tryEmitAbstractForMemory(const APValue &value, QualType T);

llvm::Constant *tryEmitConstantExpr(const ConstantExpr *CE);

llvm::Constant *emitNullForMemory(QualType T) {
return emitNullForMemory(CGM, T);
}
llvm::Constant *emitForMemory(llvm::Constant *C, QualType T) {
return emitForMemory(CGM, C, T);
}

static llvm::Constant *emitNullForMemory(CodeGenModule &CGM, QualType T);
static llvm::Constant *emitForMemory(CodeGenModule &CGM, llvm::Constant *C,
QualType T);





llvm::Constant *tryEmitPrivateForVarInit(const VarDecl &D);

llvm::Constant *tryEmitPrivate(const Expr *E, QualType T);
llvm::Constant *tryEmitPrivateForMemory(const Expr *E, QualType T);

llvm::Constant *tryEmitPrivate(const APValue &value, QualType T);
llvm::Constant *tryEmitPrivateForMemory(const APValue &value, QualType T);




llvm::GlobalValue *getCurrentAddrPrivate();










void registerCurrentAddrPrivate(llvm::Constant *signal,
llvm::GlobalValue *placeholder);

private:
void initializeNonAbstract(LangAS destAS) {
assert(!InitializedNonAbstract);
InitializedNonAbstract = true;
DestAddressSpace = destAS;
}
llvm::Constant *markIfFailed(llvm::Constant *init) {
if (!init)
Failed = true;
return init;
}

struct AbstractState {
bool OldValue;
size_t OldPlaceholdersSize;
};
AbstractState pushAbstract() {
AbstractState saved = { Abstract, PlaceholderAddresses.size() };
Abstract = true;
return saved;
}
llvm::Constant *validateAndPopAbstract(llvm::Constant *C, AbstractState save);
};

}
}

#endif
