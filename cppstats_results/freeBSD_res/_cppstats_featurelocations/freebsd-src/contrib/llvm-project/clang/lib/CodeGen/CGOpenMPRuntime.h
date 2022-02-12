











#if !defined(LLVM_CLANG_LIB_CODEGEN_CGOPENMPRUNTIME_H)
#define LLVM_CLANG_LIB_CODEGEN_CGOPENMPRUNTIME_H

#include "CGValue.h"
#include "clang/AST/DeclOpenMP.h"
#include "clang/AST/GlobalDecl.h"
#include "clang/AST/Type.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Frontend/OpenMP/OMPConstants.h"
#include "llvm/Frontend/OpenMP/OMPIRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/ValueHandle.h"
#include "llvm/Support/AtomicOrdering.h"

namespace llvm {
class ArrayType;
class Constant;
class FunctionType;
class GlobalVariable;
class StructType;
class Type;
class Value;
class OpenMPIRBuilder;
}

namespace clang {
class Expr;
class OMPDependClause;
class OMPExecutableDirective;
class OMPLoopDirective;
class VarDecl;
class OMPDeclareReductionDecl;
class IdentifierInfo;

namespace CodeGen {
class Address;
class CodeGenFunction;
class CodeGenModule;



class PrePostActionTy {
public:
explicit PrePostActionTy() {}
virtual void Enter(CodeGenFunction &CGF) {}
virtual void Exit(CodeGenFunction &CGF) {}
virtual ~PrePostActionTy() {}
};



class RegionCodeGenTy final {
intptr_t CodeGen;
typedef void (*CodeGenTy)(intptr_t, CodeGenFunction &, PrePostActionTy &);
CodeGenTy Callback;
mutable PrePostActionTy *PrePostAction;
RegionCodeGenTy() = delete;
template <typename Callable>
static void CallbackFn(intptr_t CodeGen, CodeGenFunction &CGF,
PrePostActionTy &Action) {
return (*reinterpret_cast<Callable *>(CodeGen))(CGF, Action);
}

public:
template <typename Callable>
RegionCodeGenTy(
Callable &&CodeGen,
std::enable_if_t<!std::is_same<std::remove_reference_t<Callable>,
RegionCodeGenTy>::value> * = nullptr)
: CodeGen(reinterpret_cast<intptr_t>(&CodeGen)),
Callback(CallbackFn<std::remove_reference_t<Callable>>),
PrePostAction(nullptr) {}
void setAction(PrePostActionTy &Action) const { PrePostAction = &Action; }
void operator()(CodeGenFunction &CGF) const;
};

struct OMPTaskDataTy final {
SmallVector<const Expr *, 4> PrivateVars;
SmallVector<const Expr *, 4> PrivateCopies;
SmallVector<const Expr *, 4> FirstprivateVars;
SmallVector<const Expr *, 4> FirstprivateCopies;
SmallVector<const Expr *, 4> FirstprivateInits;
SmallVector<const Expr *, 4> LastprivateVars;
SmallVector<const Expr *, 4> LastprivateCopies;
SmallVector<const Expr *, 4> ReductionVars;
SmallVector<const Expr *, 4> ReductionOrigs;
SmallVector<const Expr *, 4> ReductionCopies;
SmallVector<const Expr *, 4> ReductionOps;
SmallVector<CanonicalDeclPtr<const VarDecl>, 4> PrivateLocals;
struct DependData {
OpenMPDependClauseKind DepKind = OMPC_DEPEND_unknown;
const Expr *IteratorExpr = nullptr;
SmallVector<const Expr *, 4> DepExprs;
explicit DependData() = default;
DependData(OpenMPDependClauseKind DepKind, const Expr *IteratorExpr)
: DepKind(DepKind), IteratorExpr(IteratorExpr) {}
};
SmallVector<DependData, 4> Dependences;
llvm::PointerIntPair<llvm::Value *, 1, bool> Final;
llvm::PointerIntPair<llvm::Value *, 1, bool> Schedule;
llvm::PointerIntPair<llvm::Value *, 1, bool> Priority;
llvm::Value *Reductions = nullptr;
unsigned NumberOfParts = 0;
bool Tied = true;
bool Nogroup = false;
bool IsReductionWithTaskMod = false;
bool IsWorksharingReduction = false;
};


class ReductionCodeGen {
private:

struct ReductionData {

const Expr *Shared = nullptr;

const Expr *Ref = nullptr;

const Expr *Private = nullptr;

const Expr *ReductionOp = nullptr;
ReductionData(const Expr *Shared, const Expr *Ref, const Expr *Private,
const Expr *ReductionOp)
: Shared(Shared), Ref(Ref), Private(Private), ReductionOp(ReductionOp) {
}
};

SmallVector<ReductionData, 4> ClausesData;


SmallVector<std::pair<LValue, LValue>, 4> SharedAddresses;

SmallVector<std::pair<LValue, LValue>, 4> OrigAddresses;

SmallVector<std::pair<llvm::Value *, llvm::Value *>, 4> Sizes;

SmallVector<const VarDecl *, 4> BaseDecls;


LValue emitSharedLValue(CodeGenFunction &CGF, const Expr *E);

LValue emitSharedLValueUB(CodeGenFunction &CGF, const Expr *E);





void emitAggregateInitialization(CodeGenFunction &CGF, unsigned N,
Address PrivateAddr, LValue SharedLVal,
const OMPDeclareReductionDecl *DRD);

public:
ReductionCodeGen(ArrayRef<const Expr *> Shareds, ArrayRef<const Expr *> Origs,
ArrayRef<const Expr *> Privates,
ArrayRef<const Expr *> ReductionOps);


void emitSharedOrigLValue(CodeGenFunction &CGF, unsigned N);


void emitAggregateType(CodeGenFunction &CGF, unsigned N);



void emitAggregateType(CodeGenFunction &CGF, unsigned N, llvm::Value *Size);






void
emitInitialization(CodeGenFunction &CGF, unsigned N, Address PrivateAddr,
LValue SharedLVal,
llvm::function_ref<bool(CodeGenFunction &)> DefaultInit);

bool needCleanups(unsigned N);



void emitCleanups(CodeGenFunction &CGF, unsigned N, Address PrivateAddr);




Address adjustPrivateAddress(CodeGenFunction &CGF, unsigned N,
Address PrivateAddr);

LValue getSharedLValue(unsigned N) const { return SharedAddresses[N].first; }

LValue getOrigLValue(unsigned N) const { return OrigAddresses[N].first; }


std::pair<llvm::Value *, llvm::Value *> getSizes(unsigned N) const {
return Sizes[N];
}

const VarDecl *getBaseDecl(unsigned N) const { return BaseDecls[N]; }

const Expr *getRefExpr(unsigned N) const { return ClausesData[N].Ref; }


bool usesReductionInitializer(unsigned N) const;
};

class CGOpenMPRuntime {
public:


class DisableAutoDeclareTargetRAII {
CodeGenModule &CGM;
bool SavedShouldMarkAsGlobal;

public:
DisableAutoDeclareTargetRAII(CodeGenModule &CGM);
~DisableAutoDeclareTargetRAII();
};


class NontemporalDeclsRAII {
CodeGenModule &CGM;
const bool NeedToPush;

public:
NontemporalDeclsRAII(CodeGenModule &CGM, const OMPLoopDirective &S);
~NontemporalDeclsRAII();
};


class UntiedTaskLocalDeclsRAII {
CodeGenModule &CGM;
const bool NeedToPush;

public:
UntiedTaskLocalDeclsRAII(
CodeGenFunction &CGF,
const llvm::MapVector<CanonicalDeclPtr<const VarDecl>,
std::pair<Address, Address>> &LocalVars);
~UntiedTaskLocalDeclsRAII();
};







struct LastprivateConditionalData {
llvm::MapVector<CanonicalDeclPtr<const Decl>, SmallString<16>>
DeclToUniqueName;
LValue IVLVal;
llvm::Function *Fn = nullptr;
bool Disabled = false;
};

class LastprivateConditionalRAII {
enum class ActionToDo {
DoNotPush,
PushAsLastprivateConditional,
DisableLastprivateConditional,
};
CodeGenModule &CGM;
ActionToDo Action = ActionToDo::DoNotPush;



void tryToDisableInnerAnalysis(const OMPExecutableDirective &S,
llvm::DenseSet<CanonicalDeclPtr<const Decl>>
&NeedToAddForLPCsAsDisabled) const;

LastprivateConditionalRAII(CodeGenFunction &CGF,
const OMPExecutableDirective &S);

public:
explicit LastprivateConditionalRAII(CodeGenFunction &CGF,
const OMPExecutableDirective &S,
LValue IVLVal);
static LastprivateConditionalRAII disable(CodeGenFunction &CGF,
const OMPExecutableDirective &S);
~LastprivateConditionalRAII();
};

llvm::OpenMPIRBuilder &getOMPBuilder() { return OMPBuilder; }

protected:
CodeGenModule &CGM;
StringRef FirstSeparator, Separator;


llvm::OpenMPIRBuilder OMPBuilder;


explicit CGOpenMPRuntime(CodeGenModule &CGM, StringRef FirstSeparator,
StringRef Separator);



virtual void createOffloadEntry(llvm::Constant *ID, llvm::Constant *Addr,
uint64_t Size, int32_t Flags,
llvm::GlobalValue::LinkageTypes Linkage);










virtual void emitTargetOutlinedFunctionHelper(const OMPExecutableDirective &D,
StringRef ParentName,
llvm::Function *&OutlinedFn,
llvm::Constant *&OutlinedFnID,
bool IsOffloadEntry,
const RegionCodeGenTy &CodeGen);




llvm::Value *emitUpdateLocation(CodeGenFunction &CGF, SourceLocation Loc,
unsigned Flags = 0);









const Expr *getNumTeamsExprForTargetDirective(CodeGenFunction &CGF,
const OMPExecutableDirective &D,
int32_t &DefaultVal);
llvm::Value *emitNumTeamsForTargetDirective(CodeGenFunction &CGF,
const OMPExecutableDirective &D);








const Expr *
getNumThreadsExprForTargetDirective(CodeGenFunction &CGF,
const OMPExecutableDirective &D,
int32_t &DefaultVal);
llvm::Value *
emitNumThreadsForTargetDirective(CodeGenFunction &CGF,
const OMPExecutableDirective &D);


llvm::Type *getIdentTyPointerTy();



llvm::Value *getThreadID(CodeGenFunction &CGF, SourceLocation Loc);




virtual StringRef getOutlinedHelperName() const { return ".omp_outlined."; }


void emitCall(CodeGenFunction &CGF, SourceLocation Loc,
llvm::FunctionCallee Callee,
ArrayRef<llvm::Value *> Args = llvm::None) const;



virtual Address emitThreadIDAddress(CodeGenFunction &CGF, SourceLocation Loc);

void setLocThreadIdInsertPt(CodeGenFunction &CGF,
bool AtCurrentPoint = false);
void clearLocThreadIdInsertPt(CodeGenFunction &CGF);



virtual bool isDefaultLocationConstant() const { return false; }



virtual unsigned getDefaultLocationReserved2Flags() const { return 0; }



static unsigned getDefaultFlagsForBarriers(OpenMPDirectiveKind Kind);


llvm::ArrayType *getKmpCriticalNameTy() const {return KmpCriticalNameTy;}






llvm::Value *getCriticalRegionLock(StringRef CriticalName);

private:


typedef llvm::DenseMap<SourceLocation, llvm::Value *> OpenMPDebugLocMapTy;
OpenMPDebugLocMapTy OpenMPDebugLocMap;



llvm::FunctionType *Kmpc_MicroTy = nullptr;

struct DebugLocThreadIdTy {
llvm::Value *DebugLoc;
llvm::Value *ThreadID;

llvm::AssertingVH<llvm::Instruction> ServiceInsertPt = nullptr;
};

typedef llvm::DenseMap<llvm::Function *, DebugLocThreadIdTy>
OpenMPLocThreadIDMapTy;
OpenMPLocThreadIDMapTy OpenMPLocThreadIDMap;

typedef llvm::DenseMap<const OMPDeclareReductionDecl *,
std::pair<llvm::Function *, llvm::Function *>>
UDRMapTy;
UDRMapTy UDRMap;

typedef llvm::DenseMap<llvm::Function *,
SmallVector<const OMPDeclareReductionDecl *, 4>>
FunctionUDRMapTy;
FunctionUDRMapTy FunctionUDRMap;


llvm::DenseMap<const OMPDeclareMapperDecl *, llvm::Function *> UDMMap;

using FunctionUDMMapTy =
llvm::DenseMap<llvm::Function *,
SmallVector<const OMPDeclareMapperDecl *, 4>>;
FunctionUDMMapTy FunctionUDMMap;


llvm::DenseMap<llvm::Function *,
llvm::DenseMap<CanonicalDeclPtr<const Decl>,
std::tuple<QualType, const FieldDecl *,
const FieldDecl *, LValue>>>
LastprivateConditionalToTypes;

llvm::DenseMap<llvm::Function *, unsigned> FunctionToUntiedTaskStackMap;


llvm::ArrayType *KmpCriticalNameTy;





llvm::StringMap<llvm::AssertingVH<llvm::Constant>, llvm::BumpPtrAllocator>
InternalVars;

llvm::Type *KmpRoutineEntryPtrTy = nullptr;
QualType KmpRoutineEntryPtrQTy;









QualType KmpTaskTQTy;

QualType SavedKmpTaskTQTy;

QualType SavedKmpTaskloopTQTy;








QualType KmpDependInfoTy;









QualType KmpTaskAffinityInfoTy;





QualType KmpDimTy;








QualType TgtOffloadEntryQTy;


class OffloadEntriesInfoManagerTy {
CodeGenModule &CGM;


unsigned OffloadingEntriesNum = 0;

public:

class OffloadEntryInfo {
public:

enum OffloadingEntryInfoKinds : unsigned {

OffloadingEntryInfoTargetRegion = 0,

OffloadingEntryInfoDeviceGlobalVar = 1,

OffloadingEntryInfoInvalid = ~0u
};

protected:
OffloadEntryInfo() = delete;
explicit OffloadEntryInfo(OffloadingEntryInfoKinds Kind) : Kind(Kind) {}
explicit OffloadEntryInfo(OffloadingEntryInfoKinds Kind, unsigned Order,
uint32_t Flags)
: Flags(Flags), Order(Order), Kind(Kind) {}
~OffloadEntryInfo() = default;

public:
bool isValid() const { return Order != ~0u; }
unsigned getOrder() const { return Order; }
OffloadingEntryInfoKinds getKind() const { return Kind; }
uint32_t getFlags() const { return Flags; }
void setFlags(uint32_t NewFlags) { Flags = NewFlags; }
llvm::Constant *getAddress() const {
return cast_or_null<llvm::Constant>(Addr);
}
void setAddress(llvm::Constant *V) {
assert(!Addr.pointsToAliveValue() && "Address has been set before!");
Addr = V;
}
static bool classof(const OffloadEntryInfo *Info) { return true; }

private:

llvm::WeakTrackingVH Addr;


uint32_t Flags = 0u;


unsigned Order = ~0u;

OffloadingEntryInfoKinds Kind = OffloadingEntryInfoInvalid;
};


bool empty() const;

unsigned size() const { return OffloadingEntriesNum; }
OffloadEntriesInfoManagerTy(CodeGenModule &CGM) : CGM(CGM) {}






enum OMPTargetRegionEntryKind : uint32_t {

OMPTargetRegionEntryTargetRegion = 0x0,

OMPTargetRegionEntryCtor = 0x02,

OMPTargetRegionEntryDtor = 0x04,
};


class OffloadEntryInfoTargetRegion final : public OffloadEntryInfo {

llvm::Constant *ID = nullptr;

public:
OffloadEntryInfoTargetRegion()
: OffloadEntryInfo(OffloadingEntryInfoTargetRegion) {}
explicit OffloadEntryInfoTargetRegion(unsigned Order,
llvm::Constant *Addr,
llvm::Constant *ID,
OMPTargetRegionEntryKind Flags)
: OffloadEntryInfo(OffloadingEntryInfoTargetRegion, Order, Flags),
ID(ID) {
setAddress(Addr);
}

llvm::Constant *getID() const { return ID; }
void setID(llvm::Constant *V) {
assert(!ID && "ID has been set before!");
ID = V;
}
static bool classof(const OffloadEntryInfo *Info) {
return Info->getKind() == OffloadingEntryInfoTargetRegion;
}
};


void initializeTargetRegionEntryInfo(unsigned DeviceID, unsigned FileID,
StringRef ParentName, unsigned LineNum,
unsigned Order);

void registerTargetRegionEntryInfo(unsigned DeviceID, unsigned FileID,
StringRef ParentName, unsigned LineNum,
llvm::Constant *Addr, llvm::Constant *ID,
OMPTargetRegionEntryKind Flags);


bool hasTargetRegionEntryInfo(unsigned DeviceID, unsigned FileID,
StringRef ParentName, unsigned LineNum,
bool IgnoreAddressId = false) const;

typedef llvm::function_ref<void(unsigned, unsigned, StringRef, unsigned,
const OffloadEntryInfoTargetRegion &)>
OffloadTargetRegionEntryInfoActTy;
void actOnTargetRegionEntriesInfo(
const OffloadTargetRegionEntryInfoActTy &Action);






enum OMPTargetGlobalVarEntryKind : uint32_t {

OMPTargetGlobalVarEntryTo = 0x0,

OMPTargetGlobalVarEntryLink = 0x1,
};


class OffloadEntryInfoDeviceGlobalVar final : public OffloadEntryInfo {

CharUnits VarSize;
llvm::GlobalValue::LinkageTypes Linkage;

public:
OffloadEntryInfoDeviceGlobalVar()
: OffloadEntryInfo(OffloadingEntryInfoDeviceGlobalVar) {}
explicit OffloadEntryInfoDeviceGlobalVar(unsigned Order,
OMPTargetGlobalVarEntryKind Flags)
: OffloadEntryInfo(OffloadingEntryInfoDeviceGlobalVar, Order, Flags) {}
explicit OffloadEntryInfoDeviceGlobalVar(
unsigned Order, llvm::Constant *Addr, CharUnits VarSize,
OMPTargetGlobalVarEntryKind Flags,
llvm::GlobalValue::LinkageTypes Linkage)
: OffloadEntryInfo(OffloadingEntryInfoDeviceGlobalVar, Order, Flags),
VarSize(VarSize), Linkage(Linkage) {
setAddress(Addr);
}

CharUnits getVarSize() const { return VarSize; }
void setVarSize(CharUnits Size) { VarSize = Size; }
llvm::GlobalValue::LinkageTypes getLinkage() const { return Linkage; }
void setLinkage(llvm::GlobalValue::LinkageTypes LT) { Linkage = LT; }
static bool classof(const OffloadEntryInfo *Info) {
return Info->getKind() == OffloadingEntryInfoDeviceGlobalVar;
}
};


void initializeDeviceGlobalVarEntryInfo(StringRef Name,
OMPTargetGlobalVarEntryKind Flags,
unsigned Order);


void
registerDeviceGlobalVarEntryInfo(StringRef VarName, llvm::Constant *Addr,
CharUnits VarSize,
OMPTargetGlobalVarEntryKind Flags,
llvm::GlobalValue::LinkageTypes Linkage);

bool hasDeviceGlobalVarEntryInfo(StringRef VarName) const {
return OffloadEntriesDeviceGlobalVar.count(VarName) > 0;
}

typedef llvm::function_ref<void(StringRef,
const OffloadEntryInfoDeviceGlobalVar &)>
OffloadDeviceGlobalVarEntryInfoActTy;
void actOnDeviceGlobalVarEntriesInfo(
const OffloadDeviceGlobalVarEntryInfoActTy &Action);

private:


typedef llvm::DenseMap<unsigned, OffloadEntryInfoTargetRegion>
OffloadEntriesTargetRegionPerLine;
typedef llvm::StringMap<OffloadEntriesTargetRegionPerLine>
OffloadEntriesTargetRegionPerParentName;
typedef llvm::DenseMap<unsigned, OffloadEntriesTargetRegionPerParentName>
OffloadEntriesTargetRegionPerFile;
typedef llvm::DenseMap<unsigned, OffloadEntriesTargetRegionPerFile>
OffloadEntriesTargetRegionPerDevice;
typedef OffloadEntriesTargetRegionPerDevice OffloadEntriesTargetRegionTy;
OffloadEntriesTargetRegionTy OffloadEntriesTargetRegion;


typedef llvm::StringMap<OffloadEntryInfoDeviceGlobalVar>
OffloadEntriesDeviceGlobalVarTy;
OffloadEntriesDeviceGlobalVarTy OffloadEntriesDeviceGlobalVar;
};
OffloadEntriesInfoManagerTy OffloadEntriesInfoManager;

bool ShouldMarkAsGlobal = true;

llvm::DenseSet<CanonicalDeclPtr<const Decl>> AlreadyEmittedTargetDecls;


llvm::StringMap<llvm::WeakTrackingVH> EmittedNonTargetVariables;



llvm::SmallDenseSet<const VarDecl *> DeferredGlobalVariables;

using NontemporalDeclsSet = llvm::SmallDenseSet<CanonicalDeclPtr<const Decl>>;


llvm::SmallVector<NontemporalDeclsSet, 4> NontemporalDeclsStack;

using UntiedLocalVarsAddressesMap =
llvm::MapVector<CanonicalDeclPtr<const VarDecl>,
std::pair<Address, Address>>;
llvm::SmallVector<UntiedLocalVarsAddressesMap, 4> UntiedLocalVarsStack;




llvm::SmallVector<LastprivateConditionalData, 4> LastprivateConditionalStack;



bool HasRequiresUnifiedSharedMemory = false;


llvm::AtomicOrdering RequiresAtomicOrdering = llvm::AtomicOrdering::Monotonic;


bool HasEmittedTargetRegion = false;



bool HasEmittedDeclareTargetRegion = false;



void loadOffloadInfoMetadata();


QualType getTgtOffloadEntryQTy();





void scanForTargetRegionsFunctions(const Stmt *S, StringRef ParentName);


void emitKmpRoutineEntryT(QualType KmpInt32Ty);


llvm::Type *getKmpc_MicroPointerTy();



llvm::FunctionCallee createForStaticInitFunction(unsigned IVSize,
bool IVSigned);



llvm::FunctionCallee createDispatchInitFunction(unsigned IVSize,
bool IVSigned);



llvm::FunctionCallee createDispatchNextFunction(unsigned IVSize,
bool IVSigned);



llvm::FunctionCallee createDispatchFiniFunction(unsigned IVSize,
bool IVSigned);






llvm::Constant *getOrCreateThreadPrivateCache(const VarDecl *VD);







llvm::Constant *getOrCreateInternalVariable(llvm::Type *Ty,
const llvm::Twine &Name,
unsigned AddressSpace = 0);


llvm::StringSet<> ThreadPrivateWithDefinition;


llvm::StringSet<> DeclareTargetWithDefinition;







void emitThreadPrivateVarInit(CodeGenFunction &CGF, Address VDAddr,
llvm::Value *Ctor, llvm::Value *CopyCtor,
llvm::Value *Dtor, SourceLocation Loc);



void emitUDMapperArrayInitOrDel(CodeGenFunction &MapperCGF,
llvm::Value *Handle, llvm::Value *BasePtr,
llvm::Value *Ptr, llvm::Value *Size,
llvm::Value *MapType, llvm::Value *MapName,
CharUnits ElementSize,
llvm::BasicBlock *ExitBB, bool IsInit);

struct TaskResultTy {
llvm::Value *NewTask = nullptr;
llvm::Function *TaskEntry = nullptr;
llvm::Value *NewTaskNewTaskTTy = nullptr;
LValue TDBase;
const RecordDecl *KmpTaskTQTyRD = nullptr;
llvm::Value *TaskDupFn = nullptr;
};






















TaskResultTy emitTaskInit(CodeGenFunction &CGF, SourceLocation Loc,
const OMPExecutableDirective &D,
llvm::Function *TaskFunction, QualType SharedsTy,
Address Shareds, const OMPTaskDataTy &Data);





void emitTargetNumIterationsCall(
CodeGenFunction &CGF, const OMPExecutableDirective &D,
llvm::Value *DeviceID,
llvm::function_ref<llvm::Value *(CodeGenFunction &CGF,
const OMPLoopDirective &D)>
SizeEmitter);


void emitLastprivateConditionalUpdate(CodeGenFunction &CGF, LValue IVLVal,
StringRef UniqueDeclName, LValue LVal,
SourceLocation Loc);





std::pair<llvm::Value *, LValue> getDepobjElements(CodeGenFunction &CGF,
LValue DepobjLVal,
SourceLocation Loc);

public:
explicit CGOpenMPRuntime(CodeGenModule &CGM)
: CGOpenMPRuntime(CGM, ".", ".") {}
virtual ~CGOpenMPRuntime() {}
virtual void clear();








void emitIfClause(CodeGenFunction &CGF, const Expr *Cond,
const RegionCodeGenTy &ThenGen,
const RegionCodeGenTy &ElseGen);




static const Stmt *getSingleCompoundChild(ASTContext &Ctx, const Stmt *Body);


std::string getName(ArrayRef<StringRef> Parts) const;


virtual void emitUserDefinedReduction(CodeGenFunction *CGF,
const OMPDeclareReductionDecl *D);

virtual std::pair<llvm::Function *, llvm::Function *>
getUserDefinedReduction(const OMPDeclareReductionDecl *D);


void emitUserDefinedMapper(const OMPDeclareMapperDecl *D,
CodeGenFunction *CGF = nullptr);


llvm::Function *
getOrCreateUserDefinedMapperFunc(const OMPDeclareMapperDecl *D);









virtual llvm::Function *emitParallelOutlinedFunction(
const OMPExecutableDirective &D, const VarDecl *ThreadIDVar,
OpenMPDirectiveKind InnermostKind, const RegionCodeGenTy &CodeGen);









virtual llvm::Function *emitTeamsOutlinedFunction(
const OMPExecutableDirective &D, const VarDecl *ThreadIDVar,
OpenMPDirectiveKind InnermostKind, const RegionCodeGenTy &CodeGen);
















virtual llvm::Function *emitTaskOutlinedFunction(
const OMPExecutableDirective &D, const VarDecl *ThreadIDVar,
const VarDecl *PartIDVar, const VarDecl *TaskTVar,
OpenMPDirectiveKind InnermostKind, const RegionCodeGenTy &CodeGen,
bool Tied, unsigned &NumberOfParts);



virtual void functionFinished(CodeGenFunction &CGF);











virtual void emitParallelCall(CodeGenFunction &CGF, SourceLocation Loc,
llvm::Function *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars,
const Expr *IfCond);






virtual void emitCriticalRegion(CodeGenFunction &CGF, StringRef CriticalName,
const RegionCodeGenTy &CriticalOpGen,
SourceLocation Loc,
const Expr *Hint = nullptr);




virtual void emitMasterRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &MasterOpGen,
SourceLocation Loc);




virtual void emitMaskedRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &MaskedOpGen,
SourceLocation Loc,
const Expr *Filter = nullptr);


virtual void emitTaskyieldCall(CodeGenFunction &CGF, SourceLocation Loc);




virtual void emitTaskgroupRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &TaskgroupOpGen,
SourceLocation Loc);




virtual void emitSingleRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &SingleOpGen,
SourceLocation Loc,
ArrayRef<const Expr *> CopyprivateVars,
ArrayRef<const Expr *> DestExprs,
ArrayRef<const Expr *> SrcExprs,
ArrayRef<const Expr *> AssignmentOps);




virtual void emitOrderedRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &OrderedOpGen,
SourceLocation Loc, bool IsThreads);









virtual void emitBarrierCall(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind Kind,
bool EmitChecks = true,
bool ForceSimpleCall = false);






virtual bool isStaticNonchunked(OpenMPScheduleClauseKind ScheduleKind,
bool Chunked) const;






virtual bool isStaticNonchunked(OpenMPDistScheduleClauseKind ScheduleKind,
bool Chunked) const;





virtual bool isStaticChunked(OpenMPScheduleClauseKind ScheduleKind,
bool Chunked) const;





virtual bool isStaticChunked(OpenMPDistScheduleClauseKind ScheduleKind,
bool Chunked) const;





virtual bool isDynamic(OpenMPScheduleClauseKind ScheduleKind) const;


struct DispatchRTInput {

llvm::Value *LB = nullptr;

llvm::Value *UB = nullptr;


llvm::Value *Chunk = nullptr;
DispatchRTInput() = default;
DispatchRTInput(llvm::Value *LB, llvm::Value *UB, llvm::Value *Chunk)
: LB(LB), UB(UB), Chunk(Chunk) {}
};




















virtual void emitForDispatchInit(CodeGenFunction &CGF, SourceLocation Loc,
const OpenMPScheduleTy &ScheduleKind,
unsigned IVSize, bool IVSigned, bool Ordered,
const DispatchRTInput &DispatchValues);


struct StaticRTInput {

unsigned IVSize = 0;

bool IVSigned = false;

bool Ordered = false;


Address IL = Address::invalid();


Address LB = Address::invalid();


Address UB = Address::invalid();


Address ST = Address::invalid();


llvm::Value *Chunk = nullptr;
StaticRTInput(unsigned IVSize, bool IVSigned, bool Ordered, Address IL,
Address LB, Address UB, Address ST,
llvm::Value *Chunk = nullptr)
: IVSize(IVSize), IVSigned(IVSigned), Ordered(Ordered), IL(IL), LB(LB),
UB(UB), ST(ST), Chunk(Chunk) {}
};















virtual void emitForStaticInit(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind DKind,
const OpenMPScheduleTy &ScheduleKind,
const StaticRTInput &Values);







virtual void emitDistributeStaticInit(CodeGenFunction &CGF,
SourceLocation Loc,
OpenMPDistScheduleClauseKind SchedKind,
const StaticRTInput &Values);









virtual void emitForOrderedIterationEnd(CodeGenFunction &CGF,
SourceLocation Loc, unsigned IVSize,
bool IVSigned);








virtual void emitForStaticFinish(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind DKind);















virtual llvm::Value *emitForNext(CodeGenFunction &CGF, SourceLocation Loc,
unsigned IVSize, bool IVSigned,
Address IL, Address LB,
Address UB, Address ST);





virtual void emitNumThreadsClause(CodeGenFunction &CGF,
llvm::Value *NumThreads,
SourceLocation Loc);



virtual void emitProcBindClause(CodeGenFunction &CGF,
llvm::omp::ProcBindKind ProcBind,
SourceLocation Loc);







virtual Address getAddrOfThreadPrivate(CodeGenFunction &CGF,
const VarDecl *VD,
Address VDAddr,
SourceLocation Loc);



virtual Address getAddrOfDeclareTargetVar(const VarDecl *VD);









virtual llvm::Function *
emitThreadPrivateVarDefinition(const VarDecl *VD, Address VDAddr,
SourceLocation Loc, bool PerformInit,
CodeGenFunction *CGF = nullptr);





virtual bool emitDeclareTargetVarDefinition(const VarDecl *VD,
llvm::GlobalVariable *Addr,
bool PerformInit);





virtual Address getAddrOfArtificialThreadPrivate(CodeGenFunction &CGF,
QualType VarType,
StringRef Name);



virtual void emitFlush(CodeGenFunction &CGF, ArrayRef<const Expr *> Vars,
SourceLocation Loc, llvm::AtomicOrdering AO);




























virtual void emitTaskCall(CodeGenFunction &CGF, SourceLocation Loc,
const OMPExecutableDirective &D,
llvm::Function *TaskFunction, QualType SharedsTy,
Address Shareds, const Expr *IfCond,
const OMPTaskDataTy &Data);






























virtual void emitTaskLoopCall(CodeGenFunction &CGF, SourceLocation Loc,
const OMPLoopDirective &D,
llvm::Function *TaskFunction,
QualType SharedsTy, Address Shareds,
const Expr *IfCond, const OMPTaskDataTy &Data);








virtual void emitInlinedDirective(CodeGenFunction &CGF,
OpenMPDirectiveKind InnermostKind,
const RegionCodeGenTy &CodeGen,
bool HasCancel = false);








llvm::Function *emitReductionFunction(SourceLocation Loc,
llvm::Type *ArgsType,
ArrayRef<const Expr *> Privates,
ArrayRef<const Expr *> LHSExprs,
ArrayRef<const Expr *> RHSExprs,
ArrayRef<const Expr *> ReductionOps);


void emitSingleReductionCombiner(CodeGenFunction &CGF,
const Expr *ReductionOp,
const Expr *PrivateRef,
const DeclRefExpr *LHS,
const DeclRefExpr *RHS);

struct ReductionOptionsTy {
bool WithNowait;
bool SimpleReduction;
OpenMPDirectiveKind ReductionKind;
};










































virtual void emitReduction(CodeGenFunction &CGF, SourceLocation Loc,
ArrayRef<const Expr *> Privates,
ArrayRef<const Expr *> LHSExprs,
ArrayRef<const Expr *> RHSExprs,
ArrayRef<const Expr *> ReductionOps,
ReductionOptionsTy Options);





































virtual llvm::Value *emitTaskReductionInit(CodeGenFunction &CGF,
SourceLocation Loc,
ArrayRef<const Expr *> LHSExprs,
ArrayRef<const Expr *> RHSExprs,
const OMPTaskDataTy &Data);





virtual void emitTaskReductionFini(CodeGenFunction &CGF, SourceLocation Loc,
bool IsWorksharingReduction);






virtual void emitTaskReductionFixups(CodeGenFunction &CGF, SourceLocation Loc,
ReductionCodeGen &RCG, unsigned N);






virtual Address getTaskReductionItem(CodeGenFunction &CGF, SourceLocation Loc,
llvm::Value *ReductionsPtr,
LValue SharedLVal);


virtual void emitTaskwaitCall(CodeGenFunction &CGF, SourceLocation Loc);





virtual void emitCancellationPointCall(CodeGenFunction &CGF,
SourceLocation Loc,
OpenMPDirectiveKind CancelRegion);






virtual void emitCancelCall(CodeGenFunction &CGF, SourceLocation Loc,
const Expr *IfCond,
OpenMPDirectiveKind CancelRegion);










virtual void emitTargetOutlinedFunction(const OMPExecutableDirective &D,
StringRef ParentName,
llvm::Function *&OutlinedFn,
llvm::Constant *&OutlinedFnID,
bool IsOffloadEntry,
const RegionCodeGenTy &CodeGen);













virtual void emitTargetCall(
CodeGenFunction &CGF, const OMPExecutableDirective &D,
llvm::Function *OutlinedFn, llvm::Value *OutlinedFnID, const Expr *IfCond,
llvm::PointerIntPair<const Expr *, 2, OpenMPDeviceClauseModifier> Device,
llvm::function_ref<llvm::Value *(CodeGenFunction &CGF,
const OMPLoopDirective &D)>
SizeEmitter);





virtual bool emitTargetFunctions(GlobalDecl GD);




virtual bool emitTargetGlobalVariable(GlobalDecl GD);



virtual void registerTargetGlobalVariable(const VarDecl *VD,
llvm::Constant *Addr);




virtual bool emitTargetGlobal(GlobalDecl GD);



llvm::Function *emitRequiresDirectiveRegFun();



void createOffloadEntriesAndInfoMetadata();









virtual void emitTeamsCall(CodeGenFunction &CGF,
const OMPExecutableDirective &D,
SourceLocation Loc, llvm::Function *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars);






virtual void emitNumTeamsClause(CodeGenFunction &CGF, const Expr *NumTeams,
const Expr *ThreadLimit, SourceLocation Loc);



class TargetDataInfo {

bool RequiresDevicePointerInfo = false;


bool SeparateBeginEndCalls = false;

public:

llvm::Value *BasePointersArray = nullptr;

llvm::Value *PointersArray = nullptr;

llvm::Value *SizesArray = nullptr;



llvm::Value *MapTypesArray = nullptr;



llvm::Value *MapTypesArrayEnd = nullptr;

llvm::Value *MappersArray = nullptr;


llvm::Value *MapNamesArray = nullptr;

bool HasMapper = false;

unsigned NumberOfPtrs = 0u;


llvm::DenseMap<const ValueDecl *, Address> CaptureDeviceAddrMap;

explicit TargetDataInfo() {}
explicit TargetDataInfo(bool RequiresDevicePointerInfo,
bool SeparateBeginEndCalls)
: RequiresDevicePointerInfo(RequiresDevicePointerInfo),
SeparateBeginEndCalls(SeparateBeginEndCalls) {}

void clearArrayInfo() {
BasePointersArray = nullptr;
PointersArray = nullptr;
SizesArray = nullptr;
MapTypesArray = nullptr;
MapTypesArrayEnd = nullptr;
MapNamesArray = nullptr;
MappersArray = nullptr;
HasMapper = false;
NumberOfPtrs = 0u;
}

bool isValid() {
return BasePointersArray && PointersArray && SizesArray &&
MapTypesArray && (!HasMapper || MappersArray) && NumberOfPtrs;
}
bool requiresDevicePointerInfo() { return RequiresDevicePointerInfo; }
bool separateBeginEndCalls() { return SeparateBeginEndCalls; }
};









virtual void emitTargetDataCalls(CodeGenFunction &CGF,
const OMPExecutableDirective &D,
const Expr *IfCond, const Expr *Device,
const RegionCodeGenTy &CodeGen,
TargetDataInfo &Info);








virtual void emitTargetDataStandAloneCall(CodeGenFunction &CGF,
const OMPExecutableDirective &D,
const Expr *IfCond,
const Expr *Device);





virtual void emitDeclareSimdFunction(const FunctionDecl *FD,
llvm::Function *Fn);



virtual void emitDoacrossInit(CodeGenFunction &CGF, const OMPLoopDirective &D,
ArrayRef<Expr *> NumIterations);



virtual void emitDoacrossOrdered(CodeGenFunction &CGF,
const OMPDependClause *C);





virtual const VarDecl *translateParameter(const FieldDecl *FD,
const VarDecl *NativeParam) const {
return NativeParam;
}





virtual Address getParameterAddress(CodeGenFunction &CGF,
const VarDecl *NativeParam,
const VarDecl *TargetParam) const;



virtual void getDefaultDistScheduleAndChunk(CodeGenFunction &CGF,
const OMPLoopDirective &S, OpenMPDistScheduleClauseKind &ScheduleKind,
llvm::Value *&Chunk) const {}



virtual void getDefaultScheduleAndChunk(CodeGenFunction &CGF,
const OMPLoopDirective &S, OpenMPScheduleClauseKind &ScheduleKind,
const Expr *&ChunkExpr) const;



virtual void
emitOutlinedFunctionCall(CodeGenFunction &CGF, SourceLocation Loc,
llvm::FunctionCallee OutlinedFn,
ArrayRef<llvm::Value *> Args = llvm::None) const;



virtual void emitFunctionProlog(CodeGenFunction &CGF, const Decl *D);


virtual Address getAddressOfLocalVariable(CodeGenFunction &CGF,
const VarDecl *VD);



bool markAsGlobalTarget(GlobalDecl GD);


void emitDeferredTargetDecls() const;



virtual void
adjustTargetSpecificDataForLambdas(CodeGenFunction &CGF,
const OMPExecutableDirective &D) const;



virtual void processRequiresDirective(const OMPRequiresDecl *D);


llvm::AtomicOrdering getDefaultMemoryOrdering() const;




virtual bool hasAllocateAttributeForGlobalVar(const VarDecl *VD, LangAS &AS);


bool hasRequiresUnifiedSharedMemory() const;



bool isNontemporalDecl(const ValueDecl *VD) const;


Address emitLastprivateConditionalInit(CodeGenFunction &CGF,
const VarDecl *VD);














virtual void checkAndEmitLastprivateConditional(CodeGenFunction &CGF,
const Expr *LHS);





















virtual void checkAndEmitSharedLastprivateConditional(
CodeGenFunction &CGF, const OMPExecutableDirective &D,
const llvm::DenseSet<CanonicalDeclPtr<const VarDecl>> &IgnoredDecls);





virtual void emitLastprivateConditionalFinalUpdate(CodeGenFunction &CGF,
LValue PrivLVal,
const VarDecl *VD,
SourceLocation Loc);




std::pair<llvm::Value *, Address>
emitDependClause(CodeGenFunction &CGF,
ArrayRef<OMPTaskDataTy::DependData> Dependencies,
SourceLocation Loc);





Address emitDepobjDependClause(CodeGenFunction &CGF,
const OMPTaskDataTy::DependData &Dependencies,
SourceLocation Loc);



void emitDestroyClause(CodeGenFunction &CGF, LValue DepobjLVal,
SourceLocation Loc);




void emitUpdateClause(CodeGenFunction &CGF, LValue DepobjLVal,
OpenMPDependClauseKind NewDepKind, SourceLocation Loc);



void emitUsesAllocatorsInit(CodeGenFunction &CGF, const Expr *Allocator,
const Expr *AllocatorTraits);


void emitUsesAllocatorsFini(CodeGenFunction &CGF, const Expr *Allocator);


bool isLocalVarInUntiedTask(CodeGenFunction &CGF, const VarDecl *VD) const;
};


class CGOpenMPSIMDRuntime final : public CGOpenMPRuntime {
public:
explicit CGOpenMPSIMDRuntime(CodeGenModule &CGM) : CGOpenMPRuntime(CGM) {}
~CGOpenMPSIMDRuntime() override {}









llvm::Function *
emitParallelOutlinedFunction(const OMPExecutableDirective &D,
const VarDecl *ThreadIDVar,
OpenMPDirectiveKind InnermostKind,
const RegionCodeGenTy &CodeGen) override;









llvm::Function *
emitTeamsOutlinedFunction(const OMPExecutableDirective &D,
const VarDecl *ThreadIDVar,
OpenMPDirectiveKind InnermostKind,
const RegionCodeGenTy &CodeGen) override;
















llvm::Function *emitTaskOutlinedFunction(
const OMPExecutableDirective &D, const VarDecl *ThreadIDVar,
const VarDecl *PartIDVar, const VarDecl *TaskTVar,
OpenMPDirectiveKind InnermostKind, const RegionCodeGenTy &CodeGen,
bool Tied, unsigned &NumberOfParts) override;











void emitParallelCall(CodeGenFunction &CGF, SourceLocation Loc,
llvm::Function *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars,
const Expr *IfCond) override;






void emitCriticalRegion(CodeGenFunction &CGF, StringRef CriticalName,
const RegionCodeGenTy &CriticalOpGen,
SourceLocation Loc,
const Expr *Hint = nullptr) override;




void emitMasterRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &MasterOpGen,
SourceLocation Loc) override;




void emitMaskedRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &MaskedOpGen, SourceLocation Loc,
const Expr *Filter = nullptr) override;






void emitTaskyieldCall(CodeGenFunction &CGF, SourceLocation Loc) override;




void emitTaskgroupRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &TaskgroupOpGen,
SourceLocation Loc) override;




void emitSingleRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &SingleOpGen, SourceLocation Loc,
ArrayRef<const Expr *> CopyprivateVars,
ArrayRef<const Expr *> DestExprs,
ArrayRef<const Expr *> SrcExprs,
ArrayRef<const Expr *> AssignmentOps) override;




void emitOrderedRegion(CodeGenFunction &CGF,
const RegionCodeGenTy &OrderedOpGen,
SourceLocation Loc, bool IsThreads) override;









void emitBarrierCall(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind Kind, bool EmitChecks = true,
bool ForceSimpleCall = false) override;

















void emitForDispatchInit(CodeGenFunction &CGF, SourceLocation Loc,
const OpenMPScheduleTy &ScheduleKind,
unsigned IVSize, bool IVSigned, bool Ordered,
const DispatchRTInput &DispatchValues) override;
















void emitForStaticInit(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind DKind,
const OpenMPScheduleTy &ScheduleKind,
const StaticRTInput &Values) override;







void emitDistributeStaticInit(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDistScheduleClauseKind SchedKind,
const StaticRTInput &Values) override;









void emitForOrderedIterationEnd(CodeGenFunction &CGF, SourceLocation Loc,
unsigned IVSize, bool IVSigned) override;








void emitForStaticFinish(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind DKind) override;















llvm::Value *emitForNext(CodeGenFunction &CGF, SourceLocation Loc,
unsigned IVSize, bool IVSigned, Address IL,
Address LB, Address UB, Address ST) override;





void emitNumThreadsClause(CodeGenFunction &CGF, llvm::Value *NumThreads,
SourceLocation Loc) override;



void emitProcBindClause(CodeGenFunction &CGF,
llvm::omp::ProcBindKind ProcBind,
SourceLocation Loc) override;







Address getAddrOfThreadPrivate(CodeGenFunction &CGF, const VarDecl *VD,
Address VDAddr, SourceLocation Loc) override;









llvm::Function *
emitThreadPrivateVarDefinition(const VarDecl *VD, Address VDAddr,
SourceLocation Loc, bool PerformInit,
CodeGenFunction *CGF = nullptr) override;





Address getAddrOfArtificialThreadPrivate(CodeGenFunction &CGF,
QualType VarType,
StringRef Name) override;



void emitFlush(CodeGenFunction &CGF, ArrayRef<const Expr *> Vars,
SourceLocation Loc, llvm::AtomicOrdering AO) override;




























void emitTaskCall(CodeGenFunction &CGF, SourceLocation Loc,
const OMPExecutableDirective &D,
llvm::Function *TaskFunction, QualType SharedsTy,
Address Shareds, const Expr *IfCond,
const OMPTaskDataTy &Data) override;






























void emitTaskLoopCall(CodeGenFunction &CGF, SourceLocation Loc,
const OMPLoopDirective &D, llvm::Function *TaskFunction,
QualType SharedsTy, Address Shareds, const Expr *IfCond,
const OMPTaskDataTy &Data) override;











































void emitReduction(CodeGenFunction &CGF, SourceLocation Loc,
ArrayRef<const Expr *> Privates,
ArrayRef<const Expr *> LHSExprs,
ArrayRef<const Expr *> RHSExprs,
ArrayRef<const Expr *> ReductionOps,
ReductionOptionsTy Options) override;





































llvm::Value *emitTaskReductionInit(CodeGenFunction &CGF, SourceLocation Loc,
ArrayRef<const Expr *> LHSExprs,
ArrayRef<const Expr *> RHSExprs,
const OMPTaskDataTy &Data) override;





void emitTaskReductionFini(CodeGenFunction &CGF, SourceLocation Loc,
bool IsWorksharingReduction) override;








void emitTaskReductionFixups(CodeGenFunction &CGF, SourceLocation Loc,
ReductionCodeGen &RCG, unsigned N) override;






Address getTaskReductionItem(CodeGenFunction &CGF, SourceLocation Loc,
llvm::Value *ReductionsPtr,
LValue SharedLVal) override;


void emitTaskwaitCall(CodeGenFunction &CGF, SourceLocation Loc) override;





void emitCancellationPointCall(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind CancelRegion) override;






void emitCancelCall(CodeGenFunction &CGF, SourceLocation Loc,
const Expr *IfCond,
OpenMPDirectiveKind CancelRegion) override;










void emitTargetOutlinedFunction(const OMPExecutableDirective &D,
StringRef ParentName,
llvm::Function *&OutlinedFn,
llvm::Constant *&OutlinedFnID,
bool IsOffloadEntry,
const RegionCodeGenTy &CodeGen) override;











void emitTargetCall(
CodeGenFunction &CGF, const OMPExecutableDirective &D,
llvm::Function *OutlinedFn, llvm::Value *OutlinedFnID, const Expr *IfCond,
llvm::PointerIntPair<const Expr *, 2, OpenMPDeviceClauseModifier> Device,
llvm::function_ref<llvm::Value *(CodeGenFunction &CGF,
const OMPLoopDirective &D)>
SizeEmitter) override;





bool emitTargetFunctions(GlobalDecl GD) override;




bool emitTargetGlobalVariable(GlobalDecl GD) override;




bool emitTargetGlobal(GlobalDecl GD) override;









void emitTeamsCall(CodeGenFunction &CGF, const OMPExecutableDirective &D,
SourceLocation Loc, llvm::Function *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars) override;






void emitNumTeamsClause(CodeGenFunction &CGF, const Expr *NumTeams,
const Expr *ThreadLimit, SourceLocation Loc) override;









void emitTargetDataCalls(CodeGenFunction &CGF,
const OMPExecutableDirective &D, const Expr *IfCond,
const Expr *Device, const RegionCodeGenTy &CodeGen,
TargetDataInfo &Info) override;








void emitTargetDataStandAloneCall(CodeGenFunction &CGF,
const OMPExecutableDirective &D,
const Expr *IfCond,
const Expr *Device) override;



void emitDoacrossInit(CodeGenFunction &CGF, const OMPLoopDirective &D,
ArrayRef<Expr *> NumIterations) override;



void emitDoacrossOrdered(CodeGenFunction &CGF,
const OMPDependClause *C) override;





const VarDecl *translateParameter(const FieldDecl *FD,
const VarDecl *NativeParam) const override;





Address getParameterAddress(CodeGenFunction &CGF, const VarDecl *NativeParam,
const VarDecl *TargetParam) const override;


Address getAddressOfLocalVariable(CodeGenFunction &CGF,
const VarDecl *VD) override {
return Address::invalid();
}
};

}
}

#endif
