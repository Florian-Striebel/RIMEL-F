













#if !defined(LLVM_CLANG_LIB_CODEGEN_CGCUDARUNTIME_H)
#define LLVM_CLANG_LIB_CODEGEN_CGCUDARUNTIME_H

#include "clang/AST/GlobalDecl.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/GlobalValue.h"

namespace llvm {
class Function;
class GlobalVariable;
}

namespace clang {

class CUDAKernelCallExpr;
class NamedDecl;
class VarDecl;

namespace CodeGen {

class CodeGenFunction;
class CodeGenModule;
class FunctionArgList;
class ReturnValueSlot;
class RValue;

class CGCUDARuntime {
protected:
CodeGenModule &CGM;

public:

class DeviceVarFlags {
public:
enum DeviceVarKind {
Variable,
Surface,
Texture,
};

private:
unsigned Kind : 2;
unsigned Extern : 1;
unsigned Constant : 1;
unsigned Managed : 1;
unsigned Normalized : 1;
int SurfTexType;

public:
DeviceVarFlags(DeviceVarKind K, bool E, bool C, bool M, bool N, int T)
: Kind(K), Extern(E), Constant(C), Managed(M), Normalized(N),
SurfTexType(T) {}

DeviceVarKind getKind() const { return static_cast<DeviceVarKind>(Kind); }
bool isExtern() const { return Extern; }
bool isConstant() const { return Constant; }
bool isManaged() const { return Managed; }
bool isNormalized() const { return Normalized; }
int getSurfTexType() const { return SurfTexType; }
};

CGCUDARuntime(CodeGenModule &CGM) : CGM(CGM) {}
virtual ~CGCUDARuntime();

virtual RValue EmitCUDAKernelCallExpr(CodeGenFunction &CGF,
const CUDAKernelCallExpr *E,
ReturnValueSlot ReturnValue);


virtual void emitDeviceStub(CodeGenFunction &CGF, FunctionArgList &Args) = 0;


virtual void handleVarRegistration(const VarDecl *VD,
llvm::GlobalVariable &Var) = 0;



virtual llvm::Function *finalizeModule() = 0;



virtual std::string getDeviceSideName(const NamedDecl *ND) = 0;


virtual llvm::GlobalValue *getKernelHandle(llvm::Function *Stub,
GlobalDecl GD) = 0;


virtual llvm::Function *getKernelStub(llvm::GlobalValue *Handle) = 0;


virtual void
internalizeDeviceSideVar(const VarDecl *D,
llvm::GlobalValue::LinkageTypes &Linkage) = 0;
};


CGCUDARuntime *CreateNVCUDARuntime(CodeGenModule &CGM);

}
}

#endif
