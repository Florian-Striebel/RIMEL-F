







#if !defined(LLVM_CLANG_LIB_CODEGEN_ABIINFO_H)
#define LLVM_CLANG_LIB_CODEGEN_ABIINFO_H

#include "clang/AST/CharUnits.h"
#include "clang/AST/Type.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Type.h"

namespace llvm {
class Value;
class LLVMContext;
class DataLayout;
class Type;
}

namespace clang {
class ASTContext;
class CodeGenOptions;
class TargetInfo;

namespace CodeGen {
class ABIArgInfo;
class Address;
class CGCXXABI;
class CGFunctionInfo;
class CodeGenFunction;
class CodeGenTypes;
class SwiftABIInfo;

namespace swiftcall {
class SwiftAggLowering;
}









class ABIInfo {
public:
CodeGen::CodeGenTypes &CGT;
protected:
llvm::CallingConv::ID RuntimeCC;
public:
ABIInfo(CodeGen::CodeGenTypes &cgt)
: CGT(cgt), RuntimeCC(llvm::CallingConv::C) {}

virtual ~ABIInfo();

virtual bool supportsSwift() const { return false; }

virtual bool allowBFloatArgsAndRet() const { return false; }

CodeGen::CGCXXABI &getCXXABI() const;
ASTContext &getContext() const;
llvm::LLVMContext &getVMContext() const;
const llvm::DataLayout &getDataLayout() const;
const TargetInfo &getTarget() const;
const CodeGenOptions &getCodeGenOpts() const;



llvm::CallingConv::ID getRuntimeCC() const {
return RuntimeCC;
}

virtual void computeInfo(CodeGen::CGFunctionInfo &FI) const = 0;








virtual CodeGen::Address EmitVAArg(CodeGen::CodeGenFunction &CGF,
CodeGen::Address VAListAddr,
QualType Ty) const = 0;

bool isAndroid() const;



virtual CodeGen::Address EmitMSVAArg(CodeGen::CodeGenFunction &CGF,
CodeGen::Address VAListAddr,
QualType Ty) const;

virtual bool isHomogeneousAggregateBaseType(QualType Ty) const;

virtual bool isHomogeneousAggregateSmallEnough(const Type *Base,
uint64_t Members) const;

bool isHomogeneousAggregate(QualType Ty, const Type *&Base,
uint64_t &Members) const;



bool isPromotableIntegerTypeForABI(QualType Ty) const;



CodeGen::ABIArgInfo
getNaturalAlignIndirect(QualType Ty, bool ByVal = true,
bool Realign = false,
llvm::Type *Padding = nullptr) const;

CodeGen::ABIArgInfo
getNaturalAlignIndirectInReg(QualType Ty, bool Realign = false) const;


};






class SwiftABIInfo : public ABIInfo {
public:
SwiftABIInfo(CodeGen::CodeGenTypes &cgt) : ABIInfo(cgt) {}

bool supportsSwift() const final override { return true; }

virtual bool shouldPassIndirectlyForSwift(ArrayRef<llvm::Type*> types,
bool asReturnValue) const = 0;

virtual bool isLegalVectorTypeForSwift(CharUnits totalSize,
llvm::Type *eltTy,
unsigned elts) const;

virtual bool isSwiftErrorInRegister() const = 0;

static bool classof(const ABIInfo *info) {
return info->supportsSwift();
}
};
}
}

#endif
