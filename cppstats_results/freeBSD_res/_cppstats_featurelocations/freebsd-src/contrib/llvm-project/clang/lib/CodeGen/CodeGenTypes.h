











#if !defined(LLVM_CLANG_LIB_CODEGEN_CODEGENTYPES_H)
#define LLVM_CLANG_LIB_CODEGEN_CODEGENTYPES_H

#include "CGCall.h"
#include "clang/Basic/ABI.h"
#include "clang/CodeGen/CGFunctionInfo.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Module.h"

namespace llvm {
class FunctionType;
class DataLayout;
class Type;
class LLVMContext;
class StructType;
}

namespace clang {
class ASTContext;
template <typename> class CanQual;
class CXXConstructorDecl;
class CXXDestructorDecl;
class CXXMethodDecl;
class CodeGenOptions;
class FieldDecl;
class FunctionProtoType;
class ObjCInterfaceDecl;
class ObjCIvarDecl;
class PointerType;
class QualType;
class RecordDecl;
class TagDecl;
class TargetInfo;
class Type;
typedef CanQual<Type> CanQualType;
class GlobalDecl;

namespace CodeGen {
class ABIInfo;
class CGCXXABI;
class CGRecordLayout;
class CodeGenModule;
class RequiredArgs;



class CodeGenTypes {
CodeGenModule &CGM;

ASTContext &Context;
llvm::Module &TheModule;
const TargetInfo &Target;
CGCXXABI &TheCXXABI;



const ABIInfo &TheABIInfo;





llvm::DenseMap<const ObjCInterfaceType*, llvm::Type *> InterfaceTypes;


llvm::DenseMap<const Type*, std::unique_ptr<CGRecordLayout>> CGRecordLayouts;


llvm::DenseMap<const Type*, llvm::StructType *> RecordDeclTypes;


llvm::FoldingSet<CGFunctionInfo> FunctionInfos;





llvm::SmallPtrSet<const Type*, 4> RecordsBeingLaidOut;

llvm::SmallPtrSet<const CGFunctionInfo*, 4> FunctionsBeingProcessed;



bool SkippedLayout;

SmallVector<const RecordDecl *, 8> DeferredRecords;



llvm::DenseMap<const Type *, llvm::Type *> TypeCache;

llvm::SmallSet<const Type *, 8> RecordsWithOpaqueMemberPointers;


llvm::Type *ConvertFunctionTypeInternal(QualType FT);

public:
CodeGenTypes(CodeGenModule &cgm);
~CodeGenTypes();

const llvm::DataLayout &getDataLayout() const {
return TheModule.getDataLayout();
}
ASTContext &getContext() const { return Context; }
const ABIInfo &getABIInfo() const { return TheABIInfo; }
const TargetInfo &getTarget() const { return Target; }
CGCXXABI &getCXXABI() const { return TheCXXABI; }
llvm::LLVMContext &getLLVMContext() { return TheModule.getContext(); }
const CodeGenOptions &getCodeGenOpts() const;


unsigned ClangCallConvToLLVMCallConv(CallingConv CC);



CanQualType DeriveThisType(const CXXRecordDecl *RD, const CXXMethodDecl *MD);


llvm::Type *ConvertType(QualType T);





llvm::Type *ConvertTypeForMem(QualType T, bool ForBitField = false);


llvm::FunctionType *GetFunctionType(const CGFunctionInfo &Info);

llvm::FunctionType *GetFunctionType(GlobalDecl GD);




bool isFuncTypeConvertible(const FunctionType *FT);
bool isFuncParamTypeConvertible(QualType Ty);



bool inheritingCtorHasParams(const InheritedConstructor &Inherited,
CXXCtorType Type);




llvm::Type *GetFunctionTypeForVTable(GlobalDecl GD);

const CGRecordLayout &getCGRecordLayout(const RecordDecl*);



void UpdateCompletedType(const TagDecl *TD);



void RefreshTypeCacheForClass(const CXXRecordDecl *RD);





















const CGFunctionInfo &arrangeGlobalDeclaration(GlobalDecl GD);





const CGFunctionInfo &arrangeCall(const CGFunctionInfo &declFI,
const CallArgList &args);



const CGFunctionInfo &arrangeFunctionDeclaration(const FunctionDecl *FD);
const CGFunctionInfo &arrangeFreeFunctionCall(const CallArgList &Args,
const FunctionType *Ty,
bool ChainCall);
const CGFunctionInfo &arrangeFreeFunctionType(CanQual<FunctionProtoType> Ty);
const CGFunctionInfo &arrangeFreeFunctionType(CanQual<FunctionNoProtoType> Ty);



const CGFunctionInfo &arrangeNullaryFunction();



const CGFunctionInfo &
arrangeBuiltinFunctionDeclaration(QualType resultType,
const FunctionArgList &args);
const CGFunctionInfo &
arrangeBuiltinFunctionDeclaration(CanQualType resultType,
ArrayRef<CanQualType> argTypes);
const CGFunctionInfo &arrangeBuiltinFunctionCall(QualType resultType,
const CallArgList &args);


const CGFunctionInfo &arrangeObjCMethodDeclaration(const ObjCMethodDecl *MD);
const CGFunctionInfo &arrangeObjCMessageSendSignature(const ObjCMethodDecl *MD,
QualType receiverType);
const CGFunctionInfo &arrangeUnprototypedObjCMessageSend(
QualType returnType,
const CallArgList &args);


const CGFunctionInfo &arrangeBlockFunctionDeclaration(
const FunctionProtoType *type,
const FunctionArgList &args);
const CGFunctionInfo &arrangeBlockFunctionCall(const CallArgList &args,
const FunctionType *type);


const CGFunctionInfo &arrangeCXXMethodDeclaration(const CXXMethodDecl *MD);
const CGFunctionInfo &arrangeCXXStructorDeclaration(GlobalDecl GD);
const CGFunctionInfo &arrangeCXXConstructorCall(const CallArgList &Args,
const CXXConstructorDecl *D,
CXXCtorType CtorKind,
unsigned ExtraPrefixArgs,
unsigned ExtraSuffixArgs,
bool PassProtoArgs = true);

const CGFunctionInfo &arrangeCXXMethodCall(const CallArgList &args,
const FunctionProtoType *type,
RequiredArgs required,
unsigned numPrefixArgs);
const CGFunctionInfo &
arrangeUnprototypedMustTailThunk(const CXXMethodDecl *MD);
const CGFunctionInfo &arrangeMSCtorClosure(const CXXConstructorDecl *CD,
CXXCtorType CT);
const CGFunctionInfo &arrangeCXXMethodType(const CXXRecordDecl *RD,
const FunctionProtoType *FTP,
const CXXMethodDecl *MD);







const CGFunctionInfo &arrangeLLVMFunctionInfo(CanQualType returnType,
bool instanceMethod,
bool chainCall,
ArrayRef<CanQualType> argTypes,
FunctionType::ExtInfo info,
ArrayRef<FunctionProtoType::ExtParameterInfo> paramInfos,
RequiredArgs args);


std::unique_ptr<CGRecordLayout> ComputeRecordLayout(const RecordDecl *D,
llvm::StructType *Ty);



void addRecordTypeName(const RecordDecl *RD, llvm::StructType *Ty,
StringRef suffix);


public:

llvm::StructType *ConvertRecordDeclType(const RecordDecl *TD);



void getExpandedTypes(QualType Ty,
SmallVectorImpl<llvm::Type *>::iterator &TI);



bool isZeroInitializable(QualType T);



bool isPointerZeroInitializable(QualType T);



bool isZeroInitializable(const RecordDecl *RD);

bool isRecordLayoutComplete(const Type *Ty) const;
bool noRecordsBeingLaidOut() const {
return RecordsBeingLaidOut.empty();
}
bool isRecordBeingLaidOut(const Type *Ty) const {
return RecordsBeingLaidOut.count(Ty);
}

};

}
}

#endif
