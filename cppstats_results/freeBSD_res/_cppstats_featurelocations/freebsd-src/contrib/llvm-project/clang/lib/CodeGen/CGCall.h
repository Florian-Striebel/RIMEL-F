












#if !defined(LLVM_CLANG_LIB_CODEGEN_CGCALL_H)
#define LLVM_CLANG_LIB_CODEGEN_CGCALL_H

#include "CGValue.h"
#include "EHScopeStack.h"
#include "clang/AST/ASTFwd.h"
#include "clang/AST/CanonicalType.h"
#include "clang/AST/GlobalDecl.h"
#include "clang/AST/Type.h"
#include "llvm/IR/Value.h"


#include "ABIInfo.h"

namespace llvm {
class AttributeList;
class Function;
class Type;
class Value;
}

namespace clang {
class ASTContext;
class Decl;
class FunctionDecl;
class ObjCMethodDecl;
class VarDecl;

namespace CodeGen {


class CGCalleeInfo {

const FunctionProtoType *CalleeProtoTy;

GlobalDecl CalleeDecl;

public:
explicit CGCalleeInfo() : CalleeProtoTy(nullptr), CalleeDecl() {}
CGCalleeInfo(const FunctionProtoType *calleeProtoTy, GlobalDecl calleeDecl)
: CalleeProtoTy(calleeProtoTy), CalleeDecl(calleeDecl) {}
CGCalleeInfo(const FunctionProtoType *calleeProtoTy)
: CalleeProtoTy(calleeProtoTy), CalleeDecl() {}
CGCalleeInfo(GlobalDecl calleeDecl)
: CalleeProtoTy(nullptr), CalleeDecl(calleeDecl) {}

const FunctionProtoType *getCalleeFunctionProtoType() const {
return CalleeProtoTy;
}
const GlobalDecl getCalleeDecl() const { return CalleeDecl; }
};


class CGCallee {
enum class SpecialKind : uintptr_t {
Invalid,
Builtin,
PseudoDestructor,
Virtual,

Last = Virtual
};

struct BuiltinInfoStorage {
const FunctionDecl *Decl;
unsigned ID;
};
struct PseudoDestructorInfoStorage {
const CXXPseudoDestructorExpr *Expr;
};
struct VirtualInfoStorage {
const CallExpr *CE;
GlobalDecl MD;
Address Addr;
llvm::FunctionType *FTy;
};

SpecialKind KindOrFunctionPointer;
union {
CGCalleeInfo AbstractInfo;
BuiltinInfoStorage BuiltinInfo;
PseudoDestructorInfoStorage PseudoDestructorInfo;
VirtualInfoStorage VirtualInfo;
};

explicit CGCallee(SpecialKind kind) : KindOrFunctionPointer(kind) {}

CGCallee(const FunctionDecl *builtinDecl, unsigned builtinID)
: KindOrFunctionPointer(SpecialKind::Builtin) {
BuiltinInfo.Decl = builtinDecl;
BuiltinInfo.ID = builtinID;
}

public:
CGCallee() : KindOrFunctionPointer(SpecialKind::Invalid) {}



CGCallee(const CGCalleeInfo &abstractInfo, llvm::Value *functionPtr)
: KindOrFunctionPointer(
SpecialKind(reinterpret_cast<uintptr_t>(functionPtr))) {
AbstractInfo = abstractInfo;
assert(functionPtr && "configuring callee without function pointer");
assert(functionPtr->getType()->isPointerTy());
assert(functionPtr->getType()->getPointerElementType()->isFunctionTy());
}

static CGCallee forBuiltin(unsigned builtinID,
const FunctionDecl *builtinDecl) {
CGCallee result(SpecialKind::Builtin);
result.BuiltinInfo.Decl = builtinDecl;
result.BuiltinInfo.ID = builtinID;
return result;
}

static CGCallee forPseudoDestructor(const CXXPseudoDestructorExpr *E) {
CGCallee result(SpecialKind::PseudoDestructor);
result.PseudoDestructorInfo.Expr = E;
return result;
}

static CGCallee forDirect(llvm::Constant *functionPtr,
const CGCalleeInfo &abstractInfo = CGCalleeInfo()) {
return CGCallee(abstractInfo, functionPtr);
}

static CGCallee forDirect(llvm::FunctionCallee functionPtr,
const CGCalleeInfo &abstractInfo = CGCalleeInfo()) {
return CGCallee(abstractInfo, functionPtr.getCallee());
}

static CGCallee forVirtual(const CallExpr *CE, GlobalDecl MD, Address Addr,
llvm::FunctionType *FTy) {
CGCallee result(SpecialKind::Virtual);
result.VirtualInfo.CE = CE;
result.VirtualInfo.MD = MD;
result.VirtualInfo.Addr = Addr;
result.VirtualInfo.FTy = FTy;
return result;
}

bool isBuiltin() const {
return KindOrFunctionPointer == SpecialKind::Builtin;
}
const FunctionDecl *getBuiltinDecl() const {
assert(isBuiltin());
return BuiltinInfo.Decl;
}
unsigned getBuiltinID() const {
assert(isBuiltin());
return BuiltinInfo.ID;
}

bool isPseudoDestructor() const {
return KindOrFunctionPointer == SpecialKind::PseudoDestructor;
}
const CXXPseudoDestructorExpr *getPseudoDestructorExpr() const {
assert(isPseudoDestructor());
return PseudoDestructorInfo.Expr;
}

bool isOrdinary() const {
return uintptr_t(KindOrFunctionPointer) > uintptr_t(SpecialKind::Last);
}
CGCalleeInfo getAbstractInfo() const {
if (isVirtual())
return VirtualInfo.MD;
assert(isOrdinary());
return AbstractInfo;
}
llvm::Value *getFunctionPointer() const {
assert(isOrdinary());
return reinterpret_cast<llvm::Value *>(uintptr_t(KindOrFunctionPointer));
}
void setFunctionPointer(llvm::Value *functionPtr) {
assert(isOrdinary());
KindOrFunctionPointer =
SpecialKind(reinterpret_cast<uintptr_t>(functionPtr));
}

bool isVirtual() const {
return KindOrFunctionPointer == SpecialKind::Virtual;
}
const CallExpr *getVirtualCallExpr() const {
assert(isVirtual());
return VirtualInfo.CE;
}
GlobalDecl getVirtualMethodDecl() const {
assert(isVirtual());
return VirtualInfo.MD;
}
Address getThisAddress() const {
assert(isVirtual());
return VirtualInfo.Addr;
}
llvm::FunctionType *getVirtualFunctionType() const {
assert(isVirtual());
return VirtualInfo.FTy;
}



CGCallee prepareConcreteCallee(CodeGenFunction &CGF) const;
};

struct CallArg {
private:
union {
RValue RV;
LValue LV;
};
bool HasLV;



mutable bool IsUsed;

public:
QualType Ty;
CallArg(RValue rv, QualType ty)
: RV(rv), HasLV(false), IsUsed(false), Ty(ty) {}
CallArg(LValue lv, QualType ty)
: LV(lv), HasLV(true), IsUsed(false), Ty(ty) {}
bool hasLValue() const { return HasLV; }
QualType getType() const { return Ty; }



RValue getRValue(CodeGenFunction &CGF) const;

LValue getKnownLValue() const {
assert(HasLV && !IsUsed);
return LV;
}
RValue getKnownRValue() const {
assert(!HasLV && !IsUsed);
return RV;
}
void setRValue(RValue _RV) {
assert(!HasLV);
RV = _RV;
}

bool isAggregate() const { return HasLV || RV.isAggregate(); }

void copyInto(CodeGenFunction &CGF, Address A) const;
};



class CallArgList : public SmallVector<CallArg, 8> {
public:
CallArgList() : StackBase(nullptr) {}

struct Writeback {


LValue Source;


Address Temporary;


llvm::Value *ToUse;
};

struct CallArgCleanup {
EHScopeStack::stable_iterator Cleanup;



llvm::Instruction *IsActiveIP;
};

void add(RValue rvalue, QualType type) { push_back(CallArg(rvalue, type)); }

void addUncopiedAggregate(LValue LV, QualType type) {
push_back(CallArg(LV, type));
}




void addFrom(const CallArgList &other) {
insert(end(), other.begin(), other.end());
Writebacks.insert(Writebacks.end(), other.Writebacks.begin(),
other.Writebacks.end());
CleanupsToDeactivate.insert(CleanupsToDeactivate.end(),
other.CleanupsToDeactivate.begin(),
other.CleanupsToDeactivate.end());
assert(!(StackBase && other.StackBase) && "can't merge stackbases");
if (!StackBase)
StackBase = other.StackBase;
}

void addWriteback(LValue srcLV, Address temporary, llvm::Value *toUse) {
Writeback writeback = {srcLV, temporary, toUse};
Writebacks.push_back(writeback);
}

bool hasWritebacks() const { return !Writebacks.empty(); }

typedef llvm::iterator_range<SmallVectorImpl<Writeback>::const_iterator>
writeback_const_range;

writeback_const_range writebacks() const {
return writeback_const_range(Writebacks.begin(), Writebacks.end());
}

void addArgCleanupDeactivation(EHScopeStack::stable_iterator Cleanup,
llvm::Instruction *IsActiveIP) {
CallArgCleanup ArgCleanup;
ArgCleanup.Cleanup = Cleanup;
ArgCleanup.IsActiveIP = IsActiveIP;
CleanupsToDeactivate.push_back(ArgCleanup);
}

ArrayRef<CallArgCleanup> getCleanupsToDeactivate() const {
return CleanupsToDeactivate;
}

void allocateArgumentMemory(CodeGenFunction &CGF);
llvm::Instruction *getStackBase() const { return StackBase; }
void freeArgumentMemory(CodeGenFunction &CGF) const;



bool isUsingInAlloca() const { return StackBase; }

private:
SmallVector<Writeback, 1> Writebacks;




SmallVector<CallArgCleanup, 1> CleanupsToDeactivate;


llvm::CallInst *StackBase;
};




class FunctionArgList : public SmallVector<const VarDecl *, 16> {};



class ReturnValueSlot {
Address Addr = Address::invalid();


unsigned IsVolatile : 1;
unsigned IsUnused : 1;
unsigned IsExternallyDestructed : 1;

public:
ReturnValueSlot()
: IsVolatile(false), IsUnused(false), IsExternallyDestructed(false) {}
ReturnValueSlot(Address Addr, bool IsVolatile, bool IsUnused = false,
bool IsExternallyDestructed = false)
: Addr(Addr), IsVolatile(IsVolatile), IsUnused(IsUnused),
IsExternallyDestructed(IsExternallyDestructed) {}

bool isNull() const { return !Addr.isValid(); }
bool isVolatile() const { return IsVolatile; }
Address getValue() const { return Addr; }
bool isUnused() const { return IsUnused; }
bool isExternallyDestructed() const { return IsExternallyDestructed; }
};

}
}

#endif
