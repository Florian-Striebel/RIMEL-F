












#if !defined(LLVM_CLANG_LIB_CODEGEN_CGCXXABI_H)
#define LLVM_CLANG_LIB_CODEGEN_CGCXXABI_H

#include "CodeGenFunction.h"
#include "clang/Basic/LLVM.h"
#include "clang/CodeGen/CodeGenABITypes.h"

namespace llvm {
class Constant;
class Type;
class Value;
class CallInst;
}

namespace clang {
class CastExpr;
class CXXConstructorDecl;
class CXXDestructorDecl;
class CXXMethodDecl;
class CXXRecordDecl;
class FieldDecl;
class MangleContext;

namespace CodeGen {
class CGCallee;
class CodeGenFunction;
class CodeGenModule;
struct CatchTypeInfo;


class CGCXXABI {
protected:
CodeGenModule &CGM;
std::unique_ptr<MangleContext> MangleCtx;

CGCXXABI(CodeGenModule &CGM)
: CGM(CGM), MangleCtx(CGM.getContext().createMangleContext()) {}

protected:
ImplicitParamDecl *getThisDecl(CodeGenFunction &CGF) {
return CGF.CXXABIThisDecl;
}
llvm::Value *getThisValue(CodeGenFunction &CGF) {
return CGF.CXXABIThisValue;
}
Address getThisAddress(CodeGenFunction &CGF) {
return Address(CGF.CXXABIThisValue, CGF.CXXABIThisAlignment);
}


void ErrorUnsupportedABI(CodeGenFunction &CGF, StringRef S);


llvm::Constant *GetBogusMemberPointer(QualType T);

ImplicitParamDecl *&getStructorImplicitParamDecl(CodeGenFunction &CGF) {
return CGF.CXXStructorImplicitParamDecl;
}
llvm::Value *&getStructorImplicitParamValue(CodeGenFunction &CGF) {
return CGF.CXXStructorImplicitParamValue;
}


llvm::Value *loadIncomingCXXThis(CodeGenFunction &CGF);

void setCXXABIThisValue(CodeGenFunction &CGF, llvm::Value *ThisPtr);

ASTContext &getContext() const { return CGM.getContext(); }

virtual bool requiresArrayCookie(const CXXDeleteExpr *E, QualType eltType);
virtual bool requiresArrayCookie(const CXXNewExpr *E);





virtual bool isThisCompleteObject(GlobalDecl GD) const = 0;

public:

virtual ~CGCXXABI();


MangleContext &getMangleContext() {
return *MangleCtx;
}







virtual bool HasThisReturn(GlobalDecl GD) const { return false; }

virtual bool hasMostDerivedReturn(GlobalDecl GD) const { return false; }

virtual bool useSinitAndSterm() const { return false; }









virtual bool canCallMismatchedFunctionType() const { return true; }



virtual bool classifyReturnType(CGFunctionInfo &FI) const = 0;


enum RecordArgABI {


RAA_Default = 0,




RAA_DirectInMemory,


RAA_Indirect
};


virtual RecordArgABI getRecordArgABI(const CXXRecordDecl *RD) const = 0;



virtual bool isSRetParameterAfterThis() const { return false; }



virtual bool
isPermittedToBeHomogeneousAggregate(const CXXRecordDecl *RD) const {
return true;
};



virtual llvm::Type *
ConvertMemberPointerType(const MemberPointerType *MPT);




virtual CGCallee EmitLoadOfMemberFunctionPointer(
CodeGenFunction &CGF, const Expr *E, Address This,
llvm::Value *&ThisPtrForCall, llvm::Value *MemPtr,
const MemberPointerType *MPT);


virtual llvm::Value *
EmitMemberDataPointerAddress(CodeGenFunction &CGF, const Expr *E,
Address Base, llvm::Value *MemPtr,
const MemberPointerType *MPT);



virtual llvm::Value *EmitMemberPointerConversion(CodeGenFunction &CGF,
const CastExpr *E,
llvm::Value *Src);



virtual llvm::Constant *EmitMemberPointerConversion(const CastExpr *E,
llvm::Constant *Src);



virtual bool isZeroInitializable(const MemberPointerType *MPT);


virtual bool isMemberPointerConvertible(const MemberPointerType *MPT) const {
return true;
}


virtual llvm::Constant *EmitNullMemberPointer(const MemberPointerType *MPT);


virtual llvm::Constant *EmitMemberFunctionPointer(const CXXMethodDecl *MD);


virtual llvm::Constant *EmitMemberDataPointer(const MemberPointerType *MPT,
CharUnits offset);


virtual llvm::Constant *EmitMemberPointer(const APValue &MP, QualType MPT);


virtual llvm::Value *
EmitMemberPointerComparison(CodeGenFunction &CGF,
llvm::Value *L,
llvm::Value *R,
const MemberPointerType *MPT,
bool Inequality);


virtual llvm::Value *
EmitMemberPointerIsNotNull(CodeGenFunction &CGF,
llvm::Value *MemPtr,
const MemberPointerType *MPT);

protected:





llvm::Constant *getMemberPointerAdjustment(const CastExpr *E);

public:
virtual void emitVirtualObjectDelete(CodeGenFunction &CGF,
const CXXDeleteExpr *DE,
Address Ptr, QualType ElementType,
const CXXDestructorDecl *Dtor) = 0;
virtual void emitRethrow(CodeGenFunction &CGF, bool isNoReturn) = 0;
virtual void emitThrow(CodeGenFunction &CGF, const CXXThrowExpr *E) = 0;
virtual llvm::GlobalVariable *getThrowInfo(QualType T) { return nullptr; }




virtual bool canSpeculativelyEmitVTable(const CXXRecordDecl *RD) const = 0;

virtual void emitBeginCatch(CodeGenFunction &CGF, const CXXCatchStmt *C) = 0;

virtual llvm::CallInst *
emitTerminateForUnexpectedException(CodeGenFunction &CGF,
llvm::Value *Exn);

virtual llvm::Constant *getAddrOfRTTIDescriptor(QualType Ty) = 0;
virtual CatchTypeInfo
getAddrOfCXXCatchHandlerType(QualType Ty, QualType CatchHandlerType) = 0;
virtual CatchTypeInfo getCatchAllTypeInfo();

virtual bool shouldTypeidBeNullChecked(bool IsDeref,
QualType SrcRecordTy) = 0;
virtual void EmitBadTypeidCall(CodeGenFunction &CGF) = 0;
virtual llvm::Value *EmitTypeid(CodeGenFunction &CGF, QualType SrcRecordTy,
Address ThisPtr,
llvm::Type *StdTypeInfoPtrTy) = 0;

virtual bool shouldDynamicCastCallBeNullChecked(bool SrcIsPtr,
QualType SrcRecordTy) = 0;

virtual llvm::Value *
EmitDynamicCastCall(CodeGenFunction &CGF, Address Value,
QualType SrcRecordTy, QualType DestTy,
QualType DestRecordTy, llvm::BasicBlock *CastEnd) = 0;

virtual llvm::Value *EmitDynamicCastToVoid(CodeGenFunction &CGF,
Address Value,
QualType SrcRecordTy,
QualType DestTy) = 0;

virtual bool EmitBadCastCall(CodeGenFunction &CGF) = 0;

virtual llvm::Value *GetVirtualBaseClassOffset(CodeGenFunction &CGF,
Address This,
const CXXRecordDecl *ClassDecl,
const CXXRecordDecl *BaseClassDecl) = 0;

virtual llvm::BasicBlock *EmitCtorCompleteObjectHandler(CodeGenFunction &CGF,
const CXXRecordDecl *RD);



virtual void
initializeHiddenVirtualInheritanceMembers(CodeGenFunction &CGF,
const CXXRecordDecl *RD) {}


virtual void EmitCXXConstructors(const CXXConstructorDecl *D) = 0;






struct AddedStructorArgs {
struct Arg {
llvm::Value *Value;
QualType Type;
};
SmallVector<Arg, 1> Prefix;
SmallVector<Arg, 1> Suffix;
AddedStructorArgs() = default;
AddedStructorArgs(SmallVector<Arg, 1> P, SmallVector<Arg, 1> S)
: Prefix(std::move(P)), Suffix(std::move(S)) {}
static AddedStructorArgs prefix(SmallVector<Arg, 1> Args) {
return {std::move(Args), {}};
}
static AddedStructorArgs suffix(SmallVector<Arg, 1> Args) {
return {{}, std::move(Args)};
}
};



struct AddedStructorArgCounts {
unsigned Prefix = 0;
unsigned Suffix = 0;
AddedStructorArgCounts() = default;
AddedStructorArgCounts(unsigned P, unsigned S) : Prefix(P), Suffix(S) {}
static AddedStructorArgCounts prefix(unsigned N) { return {N, 0}; }
static AddedStructorArgCounts suffix(unsigned N) { return {0, N}; }
};




virtual AddedStructorArgCounts
buildStructorSignature(GlobalDecl GD,
SmallVectorImpl<CanQualType> &ArgTys) = 0;




virtual bool useThunkForDtorVariant(const CXXDestructorDecl *Dtor,
CXXDtorType DT) const = 0;

virtual void setCXXDestructorDLLStorage(llvm::GlobalValue *GV,
const CXXDestructorDecl *Dtor,
CXXDtorType DT) const;

virtual llvm::GlobalValue::LinkageTypes
getCXXDestructorLinkage(GVALinkage Linkage, const CXXDestructorDecl *Dtor,
CXXDtorType DT) const;


virtual void EmitCXXDestructors(const CXXDestructorDecl *D) = 0;





virtual const CXXRecordDecl *
getThisArgumentTypeForMethod(const CXXMethodDecl *MD) {
return MD->getParent();
}




virtual Address
adjustThisArgumentForVirtualFunctionCall(CodeGenFunction &CGF, GlobalDecl GD,
Address This, bool VirtualCall) {
return This;
}


void buildThisParam(CodeGenFunction &CGF, FunctionArgList &Params);








virtual void addImplicitStructorParams(CodeGenFunction &CGF, QualType &ResTy,
FunctionArgList &Params) = 0;



virtual CharUnits getVirtualFunctionPrologueThisAdjustment(GlobalDecl GD) {
return CharUnits::Zero();
}


virtual void EmitInstanceFunctionProlog(CodeGenFunction &CGF) = 0;

virtual AddedStructorArgs
getImplicitConstructorArgs(CodeGenFunction &CGF, const CXXConstructorDecl *D,
CXXCtorType Type, bool ForVirtualBase,
bool Delegating) = 0;





AddedStructorArgCounts
addImplicitConstructorArgs(CodeGenFunction &CGF, const CXXConstructorDecl *D,
CXXCtorType Type, bool ForVirtualBase,
bool Delegating, CallArgList &Args);



virtual llvm::Value *
getCXXDestructorImplicitParam(CodeGenFunction &CGF,
const CXXDestructorDecl *DD, CXXDtorType Type,
bool ForVirtualBase, bool Delegating) = 0;


virtual void EmitDestructorCall(CodeGenFunction &CGF,
const CXXDestructorDecl *DD, CXXDtorType Type,
bool ForVirtualBase, bool Delegating,
Address This, QualType ThisTy) = 0;


virtual void emitVTableDefinitions(CodeGenVTables &CGVT,
const CXXRecordDecl *RD) = 0;


virtual bool
isVirtualOffsetNeededForVTableField(CodeGenFunction &CGF,
CodeGenFunction::VPtr Vptr) = 0;


virtual bool doStructorsInitializeVPtrs(const CXXRecordDecl *VTableClass) = 0;


virtual llvm::Constant *
getVTableAddressPoint(BaseSubobject Base,
const CXXRecordDecl *VTableClass) = 0;



virtual llvm::Value *
getVTableAddressPointInStructor(CodeGenFunction &CGF, const CXXRecordDecl *RD,
BaseSubobject Base,
const CXXRecordDecl *NearestVBase) = 0;



virtual llvm::Constant *
getVTableAddressPointForConstExpr(BaseSubobject Base,
const CXXRecordDecl *VTableClass) = 0;



virtual llvm::GlobalVariable *getAddrOfVTable(const CXXRecordDecl *RD,
CharUnits VPtrOffset) = 0;


virtual CGCallee getVirtualFunctionPointer(CodeGenFunction &CGF,
GlobalDecl GD, Address This,
llvm::Type *Ty,
SourceLocation Loc) = 0;

using DeleteOrMemberCallExpr =
llvm::PointerUnion<const CXXDeleteExpr *, const CXXMemberCallExpr *>;


virtual llvm::Value *EmitVirtualDestructorCall(CodeGenFunction &CGF,
const CXXDestructorDecl *Dtor,
CXXDtorType DtorType,
Address This,
DeleteOrMemberCallExpr E) = 0;

virtual void adjustCallArgsForDestructorThunk(CodeGenFunction &CGF,
GlobalDecl GD,
CallArgList &CallArgs) {}




virtual void emitVirtualInheritanceTables(const CXXRecordDecl *RD) = 0;

virtual bool exportThunk() = 0;
virtual void setThunkLinkage(llvm::Function *Thunk, bool ForVTable,
GlobalDecl GD, bool ReturnAdjustment) = 0;

virtual llvm::Value *performThisAdjustment(CodeGenFunction &CGF,
Address This,
const ThisAdjustment &TA) = 0;

virtual llvm::Value *performReturnAdjustment(CodeGenFunction &CGF,
Address Ret,
const ReturnAdjustment &RA) = 0;

virtual void EmitReturnFromThunk(CodeGenFunction &CGF,
RValue RV, QualType ResultType);

virtual size_t getSrcArgforCopyCtor(const CXXConstructorDecl *,
FunctionArgList &Args) const = 0;


virtual std::vector<CharUnits> getVBPtrOffsets(const CXXRecordDecl *RD);


virtual StringRef GetPureVirtualCallName() = 0;


virtual StringRef GetDeletedVirtualCallName() = 0;












virtual CharUnits GetArrayCookieSize(const CXXNewExpr *expr);










virtual Address InitializeArrayCookie(CodeGenFunction &CGF,
Address NewPtr,
llvm::Value *NumElements,
const CXXNewExpr *expr,
QualType ElementType);














virtual void ReadArrayCookie(CodeGenFunction &CGF, Address Ptr,
const CXXDeleteExpr *expr,
QualType ElementType, llvm::Value *&NumElements,
llvm::Value *&AllocPtr, CharUnits &CookieSize);


virtual bool NeedsVTTParameter(GlobalDecl GD);

protected:



virtual CharUnits getArrayCookieSizeImpl(QualType elementType);










virtual llvm::Value *readArrayCookieImpl(CodeGenFunction &IGF, Address ptr,
CharUnits cookieSize);

public:











virtual void EmitGuardedInit(CodeGenFunction &CGF, const VarDecl &D,
llvm::GlobalVariable *DeclPtr,
bool PerformInit) = 0;






virtual void registerGlobalDtor(CodeGenFunction &CGF, const VarDecl &D,
llvm::FunctionCallee Dtor,
llvm::Constant *Addr) = 0;












virtual void EmitThreadLocalInitFuncs(
CodeGenModule &CGM, ArrayRef<const VarDecl *> CXXThreadLocals,
ArrayRef<llvm::Function *> CXXThreadLocalInits,
ArrayRef<const VarDecl *> CXXThreadLocalInitVars) = 0;



virtual bool usesThreadWrapperFunction(const VarDecl *VD) const = 0;




virtual LValue EmitThreadLocalVarDeclLValue(CodeGenFunction &CGF,
const VarDecl *VD,
QualType LValType) = 0;



virtual void emitCXXStructor(GlobalDecl GD) = 0;




virtual std::pair<llvm::Value *, const CXXRecordDecl *>
LoadVTablePtr(CodeGenFunction &CGF, Address This,
const CXXRecordDecl *RD) = 0;
};




CGCXXABI *CreateItaniumCXXABI(CodeGenModule &CGM);


CGCXXABI *CreateMicrosoftCXXABI(CodeGenModule &CGM);

struct CatchRetScope final : EHScopeStack::Cleanup {
llvm::CatchPadInst *CPI;

CatchRetScope(llvm::CatchPadInst *CPI) : CPI(CPI) {}

void Emit(CodeGenFunction &CGF, Flags flags) override {
llvm::BasicBlock *BB = CGF.createBasicBlock("catchret.dest");
CGF.Builder.CreateCatchRet(CPI, BB);
CGF.EmitBlock(BB);
}
};
}
}

#endif
