












#if !defined(LLVM_CLANG_LIB_CODEGEN_TARGETINFO_H)
#define LLVM_CLANG_LIB_CODEGEN_TARGETINFO_H

#include "CGBuilder.h"
#include "CodeGenModule.h"
#include "CGValue.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SyncScope.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {
class Constant;
class GlobalValue;
class Type;
class Value;
}

namespace clang {
class Decl;

namespace CodeGen {
class ABIInfo;
class CallArgList;
class CodeGenFunction;
class CGBlockInfo;
class CGFunctionInfo;




class TargetCodeGenInfo {
std::unique_ptr<ABIInfo> Info = nullptr;

public:
TargetCodeGenInfo(std::unique_ptr<ABIInfo> Info) : Info(std::move(Info)) {}
virtual ~TargetCodeGenInfo();


const ABIInfo &getABIInfo() const { return *Info; }



virtual void setTargetAttributes(const Decl *D, llvm::GlobalValue *GV,
CodeGen::CodeGenModule &M) const {}



virtual void emitTargetMetadata(
CodeGen::CodeGenModule &CGM,
const llvm::MapVector<GlobalDecl, StringRef> &MangledDeclNames) const {}



virtual void checkFunctionCallABI(CodeGenModule &CGM, SourceLocation CallLoc,
const FunctionDecl *Caller,
const FunctionDecl *Callee,
const CallArgList &Args) const {}









virtual unsigned getSizeOfUnwindException() const;








virtual bool extendPointerWithSExt() const { return false; }





virtual int getDwarfEHStackPointer(CodeGen::CodeGenModule &M) const {
return -1;
}





virtual bool initDwarfEHRegSizeTable(CodeGen::CodeGenFunction &CGF,
llvm::Value *Address) const {
return true;
}






virtual llvm::Value *decodeReturnAddress(CodeGen::CodeGenFunction &CGF,
llvm::Value *Address) const {
return Address;
}






virtual llvm::Value *encodeReturnAddress(CodeGen::CodeGenFunction &CGF,
llvm::Value *Address) const {
return Address;
}




virtual llvm::Value *
testFPKind(llvm::Value *V, unsigned BuiltinID, CGBuilderTy &Builder,
CodeGenModule &CGM) const {
assert(V->getType()->isFloatingPointTy() && "V should have an FP type.");
return nullptr;
}






virtual llvm::Type *adjustInlineAsmType(CodeGen::CodeGenFunction &CGF,
StringRef Constraint,
llvm::Type *Ty) const {
return Ty;
}



virtual bool isScalarizableAsmOperand(CodeGen::CodeGenFunction &CGF,
llvm::Type *Ty) const {
return false;
}


virtual void addReturnRegisterOutputs(
CodeGen::CodeGenFunction &CGF, CodeGen::LValue ReturnValue,
std::string &Constraints, std::vector<llvm::Type *> &ResultRegTypes,
std::vector<llvm::Type *> &ResultTruncRegTypes,
std::vector<CodeGen::LValue> &ResultRegDests, std::string &AsmString,
unsigned NumOutputs) const {}



virtual bool doesReturnSlotInterfereWithArgs() const { return true; }











virtual StringRef getARCRetainAutoreleasedReturnValueMarker() const {
return "";
}



virtual bool markARCOptimizedReturnCallsAsNoTail() const { return false; }



virtual llvm::Constant *
getUBSanFunctionSignature(CodeGen::CodeGenModule &CGM) const {
return nullptr;
}











































virtual bool isNoProtoCallVariadic(const CodeGen::CallArgList &args,
const FunctionNoProtoType *fnType) const;



virtual void getDependentLibraryOption(llvm::StringRef Lib,
llvm::SmallString<24> &Opt) const;



virtual void getDetectMismatchOption(llvm::StringRef Name,
llvm::StringRef Value,
llvm::SmallString<32> &Opt) const {}


virtual unsigned getOpenCLKernelCallingConv() const;






virtual llvm::Constant *getNullPointer(const CodeGen::CodeGenModule &CGM,
llvm::PointerType *T, QualType QT) const;





virtual LangAS getGlobalVarAddressSpace(CodeGenModule &CGM,
const VarDecl *D) const;


virtual LangAS getASTAllocaAddressSpace() const { return LangAS::Default; }







virtual llvm::Value *performAddrSpaceCast(CodeGen::CodeGenFunction &CGF,
llvm::Value *V, LangAS SrcAddr,
LangAS DestAddr, llvm::Type *DestTy,
bool IsNonNull = false) const;






virtual llvm::Constant *performAddrSpaceCast(CodeGenModule &CGM,
llvm::Constant *V,
LangAS SrcAddr, LangAS DestAddr,
llvm::Type *DestTy) const;


virtual LangAS getAddrSpaceOfCxaAtexitPtrParam() const {
return LangAS::Default;
}


virtual llvm::SyncScope::ID getLLVMSyncScopeID(const LangOptions &LangOpts,
SyncScope Scope,
llvm::AtomicOrdering Ordering,
llvm::LLVMContext &Ctx) const;


class TargetOpenCLBlockHelper {
public:
typedef std::pair<llvm::Value *, StringRef> ValueTy;
TargetOpenCLBlockHelper() {}
virtual ~TargetOpenCLBlockHelper() {}

virtual llvm::SmallVector<llvm::Type *, 1> getCustomFieldTypes() = 0;

virtual llvm::SmallVector<ValueTy, 1>
getCustomFieldValues(CodeGenFunction &CGF, const CGBlockInfo &Info) = 0;
virtual bool areAllCustomFieldValuesConstant(const CGBlockInfo &Info) = 0;


virtual llvm::SmallVector<llvm::Constant *, 1>
getCustomFieldValues(CodeGenModule &CGM, const CGBlockInfo &Info) = 0;
};
virtual TargetOpenCLBlockHelper *getTargetOpenCLBlockHelper() const {
return nullptr;
}






virtual llvm::Function *
createEnqueuedBlockKernel(CodeGenFunction &CGF,
llvm::Function *BlockInvokeFunc,
llvm::Value *BlockLiteral) const;




virtual bool shouldEmitStaticExternCAliases() const { return true; }

virtual void setCUDAKernelCallingConvention(const FunctionType *&FT) const {}


virtual llvm::Type *getCUDADeviceBuiltinSurfaceDeviceType() const {

return nullptr;
}

virtual llvm::Type *getCUDADeviceBuiltinTextureDeviceType() const {

return nullptr;
}


virtual bool emitCUDADeviceBuiltinSurfaceDeviceCopy(CodeGenFunction &CGF,
LValue Dst,
LValue Src) const {

return false;
}

virtual bool emitCUDADeviceBuiltinTextureDeviceCopy(CodeGenFunction &CGF,
LValue Dst,
LValue Src) const {

return false;
}
};

}
}

#endif
