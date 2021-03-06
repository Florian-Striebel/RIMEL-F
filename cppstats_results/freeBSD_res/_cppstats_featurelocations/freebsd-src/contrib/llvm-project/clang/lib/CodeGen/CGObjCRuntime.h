













#if !defined(LLVM_CLANG_LIB_CODEGEN_CGOBJCRUNTIME_H)
#define LLVM_CLANG_LIB_CODEGEN_CGOBJCRUNTIME_H
#include "CGBuilder.h"
#include "CGCall.h"
#include "CGCleanup.h"
#include "CGValue.h"
#include "clang/AST/DeclObjC.h"
#include "clang/Basic/IdentifierTable.h"
#include "llvm/ADT/UniqueVector.h"

namespace llvm {
class Constant;
class Function;
class Module;
class StructLayout;
class StructType;
class Type;
class Value;
}

namespace clang {
namespace CodeGen {
class CodeGenFunction;
}

class FieldDecl;
class ObjCAtTryStmt;
class ObjCAtThrowStmt;
class ObjCAtSynchronizedStmt;
class ObjCContainerDecl;
class ObjCCategoryImplDecl;
class ObjCImplementationDecl;
class ObjCInterfaceDecl;
class ObjCMessageExpr;
class ObjCMethodDecl;
class ObjCProtocolDecl;
class Selector;
class ObjCIvarDecl;
class ObjCStringLiteral;
class BlockDeclRefExpr;

namespace CodeGen {
class CodeGenModule;
class CGBlockInfo;





class CGObjCRuntime {
protected:
CodeGen::CodeGenModule &CGM;
CGObjCRuntime(CodeGen::CodeGenModule &CGM) : CGM(CGM) {}











uint64_t ComputeIvarBaseOffset(CodeGen::CodeGenModule &CGM,
const ObjCInterfaceDecl *OID,
const ObjCIvarDecl *Ivar);
uint64_t ComputeIvarBaseOffset(CodeGen::CodeGenModule &CGM,
const ObjCImplementationDecl *OID,
const ObjCIvarDecl *Ivar);

LValue EmitValueForIvarAtOffset(CodeGen::CodeGenFunction &CGF,
const ObjCInterfaceDecl *OID,
llvm::Value *BaseValue,
const ObjCIvarDecl *Ivar,
unsigned CVRQualifiers,
llvm::Value *Offset);







void EmitTryCatchStmt(CodeGenFunction &CGF, const ObjCAtTryStmt &S,
llvm::FunctionCallee beginCatchFn,
llvm::FunctionCallee endCatchFn,
llvm::FunctionCallee exceptionRethrowFn);

void EmitInitOfCatchParam(CodeGenFunction &CGF, llvm::Value *exn,
const VarDecl *paramDecl);





void EmitAtSynchronizedStmt(CodeGenFunction &CGF,
const ObjCAtSynchronizedStmt &S,
llvm::FunctionCallee syncEnterFn,
llvm::FunctionCallee syncExitFn);

public:
virtual ~CGObjCRuntime();

std::string getSymbolNameForMethod(const ObjCMethodDecl *method,
bool includeCategoryName = true);



virtual llvm::Function *ModuleInitFunction() = 0;



virtual llvm::Value *GetSelector(CodeGenFunction &CGF, Selector Sel) = 0;






virtual Address GetAddrOfSelector(CodeGenFunction &CGF, Selector Sel) = 0;


virtual llvm::Value *GetSelector(CodeGenFunction &CGF,
const ObjCMethodDecl *Method) = 0;





virtual llvm::Constant *GetEHType(QualType T) = 0;

virtual CatchTypeInfo getCatchAllTypeInfo() { return { nullptr, 0 }; }


virtual ConstantAddress GenerateConstantString(const StringLiteral *) = 0;



virtual void GenerateCategory(const ObjCCategoryImplDecl *OCD) = 0;


virtual void GenerateClass(const ObjCImplementationDecl *OID) = 0;


virtual void RegisterAlias(const ObjCCompatibleAliasDecl *OAD) = 0;





virtual CodeGen::RValue
GenerateMessageSend(CodeGen::CodeGenFunction &CGF,
ReturnValueSlot ReturnSlot,
QualType ResultType,
Selector Sel,
llvm::Value *Receiver,
const CallArgList &CallArgs,
const ObjCInterfaceDecl *Class = nullptr,
const ObjCMethodDecl *Method = nullptr) = 0;





CodeGen::RValue
GeneratePossiblySpecializedMessageSend(CodeGenFunction &CGF,
ReturnValueSlot Return,
QualType ResultType,
Selector Sel,
llvm::Value *Receiver,
const CallArgList& Args,
const ObjCInterfaceDecl *OID,
const ObjCMethodDecl *Method,
bool isClassMessage);







virtual CodeGen::RValue
GenerateMessageSendSuper(CodeGen::CodeGenFunction &CGF,
ReturnValueSlot ReturnSlot,
QualType ResultType,
Selector Sel,
const ObjCInterfaceDecl *Class,
bool isCategoryImpl,
llvm::Value *Self,
bool IsClassMessage,
const CallArgList &CallArgs,
const ObjCMethodDecl *Method = nullptr) = 0;







std::vector<const ObjCProtocolDecl *>
GetRuntimeProtocolList(ObjCProtocolDecl::protocol_iterator begin,
ObjCProtocolDecl::protocol_iterator end);



virtual llvm::Value *GenerateProtocolRef(CodeGenFunction &CGF,
const ObjCProtocolDecl *OPD) = 0;



virtual void GenerateProtocol(const ObjCProtocolDecl *OPD) = 0;




virtual llvm::Constant *GetOrEmitProtocol(const ObjCProtocolDecl *PD) = 0;







virtual llvm::Function *GenerateMethod(const ObjCMethodDecl *OMD,
const ObjCContainerDecl *CD) = 0;


virtual void GenerateDirectMethodPrologue(CodeGenFunction &CGF,
llvm::Function *Fn,
const ObjCMethodDecl *OMD,
const ObjCContainerDecl *CD) = 0;


virtual llvm::FunctionCallee GetPropertyGetFunction() = 0;


virtual llvm::FunctionCallee GetPropertySetFunction() = 0;


virtual llvm::FunctionCallee GetOptimizedPropertySetFunction(bool atomic,
bool copy) = 0;


virtual llvm::FunctionCallee GetGetStructFunction() = 0;

virtual llvm::FunctionCallee GetSetStructFunction() = 0;


virtual llvm::FunctionCallee GetCppAtomicObjectSetFunction() = 0;


virtual llvm::FunctionCallee GetCppAtomicObjectGetFunction() = 0;



virtual llvm::Value *GetClass(CodeGenFunction &CGF,
const ObjCInterfaceDecl *OID) = 0;


virtual llvm::Value *EmitNSAutoreleasePoolClassRef(CodeGenFunction &CGF) {
llvm_unreachable("autoreleasepool unsupported in this ABI");
}



virtual llvm::FunctionCallee EnumerationMutationFunction() = 0;

virtual void EmitSynchronizedStmt(CodeGen::CodeGenFunction &CGF,
const ObjCAtSynchronizedStmt &S) = 0;
virtual void EmitTryStmt(CodeGen::CodeGenFunction &CGF,
const ObjCAtTryStmt &S) = 0;
virtual void EmitThrowStmt(CodeGen::CodeGenFunction &CGF,
const ObjCAtThrowStmt &S,
bool ClearInsertionPoint=true) = 0;
virtual llvm::Value *EmitObjCWeakRead(CodeGen::CodeGenFunction &CGF,
Address AddrWeakObj) = 0;
virtual void EmitObjCWeakAssign(CodeGen::CodeGenFunction &CGF,
llvm::Value *src, Address dest) = 0;
virtual void EmitObjCGlobalAssign(CodeGen::CodeGenFunction &CGF,
llvm::Value *src, Address dest,
bool threadlocal=false) = 0;
virtual void EmitObjCIvarAssign(CodeGen::CodeGenFunction &CGF,
llvm::Value *src, Address dest,
llvm::Value *ivarOffset) = 0;
virtual void EmitObjCStrongCastAssign(CodeGen::CodeGenFunction &CGF,
llvm::Value *src, Address dest) = 0;

virtual LValue EmitObjCValueForIvar(CodeGen::CodeGenFunction &CGF,
QualType ObjectTy,
llvm::Value *BaseValue,
const ObjCIvarDecl *Ivar,
unsigned CVRQualifiers) = 0;
virtual llvm::Value *EmitIvarOffset(CodeGen::CodeGenFunction &CGF,
const ObjCInterfaceDecl *Interface,
const ObjCIvarDecl *Ivar) = 0;
virtual void EmitGCMemmoveCollectable(CodeGen::CodeGenFunction &CGF,
Address DestPtr,
Address SrcPtr,
llvm::Value *Size) = 0;
virtual llvm::Constant *BuildGCBlockLayout(CodeGen::CodeGenModule &CGM,
const CodeGen::CGBlockInfo &blockInfo) = 0;
virtual llvm::Constant *BuildRCBlockLayout(CodeGen::CodeGenModule &CGM,
const CodeGen::CGBlockInfo &blockInfo) = 0;
virtual std::string getRCBlockLayoutStr(CodeGen::CodeGenModule &CGM,
const CGBlockInfo &blockInfo) {
return {};
}


virtual llvm::Constant *BuildByrefLayout(CodeGen::CodeGenModule &CGM,
QualType T) = 0;

struct MessageSendInfo {
const CGFunctionInfo &CallInfo;
llvm::PointerType *MessengerType;

MessageSendInfo(const CGFunctionInfo &callInfo,
llvm::PointerType *messengerType)
: CallInfo(callInfo), MessengerType(messengerType) {}
};

MessageSendInfo getMessageSendInfo(const ObjCMethodDecl *method,
QualType resultType,
CallArgList &callArgs);



unsigned ComputeBitfieldBitOffset(CodeGen::CodeGenModule &CGM,
const ObjCInterfaceDecl *ID,
const ObjCIvarDecl *Ivar);
};



CGObjCRuntime *CreateGNUObjCRuntime(CodeGenModule &CGM);
CGObjCRuntime *CreateMacObjCRuntime(CodeGenModule &CGM);
}
}
#endif
