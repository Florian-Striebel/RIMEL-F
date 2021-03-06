





















#if !defined(LLVM_CLANG_CODEGEN_CODEGENABITYPES_H)
#define LLVM_CLANG_CODEGEN_CODEGENABITYPES_H

#include "clang/AST/CanonicalType.h"
#include "clang/AST/Type.h"
#include "clang/Basic/ABI.h"
#include "clang/CodeGen/CGFunctionInfo.h"
#include "llvm/IR/BasicBlock.h"

namespace llvm {
class AttrBuilder;
class Constant;
class DataLayout;
class Module;
class Function;
class FunctionType;
class Type;
}

namespace clang {
class ASTContext;
class CXXConstructorDecl;
class CXXDestructorDecl;
class CXXRecordDecl;
class CXXMethodDecl;
class CodeGenOptions;
class CoverageSourceInfo;
class DiagnosticsEngine;
class HeaderSearchOptions;
class ObjCMethodDecl;
class ObjCProtocolDecl;
class PreprocessorOptions;

namespace CodeGen {
class CGFunctionInfo;
class CodeGenModule;


struct ImplicitCXXConstructorArgs {


SmallVector<llvm::Value *, 1> Prefix;


SmallVector<llvm::Value *, 1> Suffix;
};

const CGFunctionInfo &arrangeObjCMessageSendSignature(CodeGenModule &CGM,
const ObjCMethodDecl *MD,
QualType receiverType);

const CGFunctionInfo &arrangeFreeFunctionType(CodeGenModule &CGM,
CanQual<FunctionProtoType> Ty);

const CGFunctionInfo &arrangeFreeFunctionType(CodeGenModule &CGM,
CanQual<FunctionNoProtoType> Ty);

const CGFunctionInfo &arrangeCXXMethodType(CodeGenModule &CGM,
const CXXRecordDecl *RD,
const FunctionProtoType *FTP,
const CXXMethodDecl *MD);

const CGFunctionInfo &arrangeFreeFunctionCall(CodeGenModule &CGM,
CanQualType returnType,
ArrayRef<CanQualType> argTypes,
FunctionType::ExtInfo info,
RequiredArgs args);



ImplicitCXXConstructorArgs
getImplicitCXXConstructorArgs(CodeGenModule &CGM, const CXXConstructorDecl *D);

llvm::Value *
getCXXDestructorImplicitParam(CodeGenModule &CGM, llvm::BasicBlock *InsertBlock,
llvm::BasicBlock::iterator InsertPoint,
const CXXDestructorDecl *D, CXXDtorType Type,
bool ForVirtualBase, bool Delegating);


llvm::FunctionType *convertFreeFunctionType(CodeGenModule &CGM,
const FunctionDecl *FD);

llvm::Type *convertTypeForMemory(CodeGenModule &CGM, QualType T);





unsigned getLLVMFieldNumber(CodeGenModule &CGM,
const RecordDecl *RD, const FieldDecl *FD);

















void addDefaultFunctionDefinitionAttributes(CodeGenModule &CGM,
llvm::AttrBuilder &attrs);





llvm::Function *getNonTrivialCStructDefaultConstructor(CodeGenModule &GCM,
CharUnits DstAlignment,
bool IsVolatile,
QualType QT);





llvm::Function *getNonTrivialCStructCopyConstructor(CodeGenModule &CGM,
CharUnits DstAlignment,
CharUnits SrcAlignment,
bool IsVolatile,
QualType QT);





llvm::Function *getNonTrivialCStructMoveConstructor(CodeGenModule &CGM,
CharUnits DstAlignment,
CharUnits SrcAlignment,
bool IsVolatile,
QualType QT);





llvm::Function *getNonTrivialCStructCopyAssignmentOperator(
CodeGenModule &CGM, CharUnits DstAlignment, CharUnits SrcAlignment,
bool IsVolatile, QualType QT);





llvm::Function *getNonTrivialCStructMoveAssignmentOperator(
CodeGenModule &CGM, CharUnits DstAlignment, CharUnits SrcAlignment,
bool IsVolatile, QualType QT);





llvm::Function *getNonTrivialCStructDestructor(CodeGenModule &CGM,
CharUnits DstAlignment,
bool IsVolatile, QualType QT);






llvm::Constant *emitObjCProtocolObject(CodeGenModule &CGM,
const ObjCProtocolDecl *p);
}
}

#endif
