











#if !defined(LLVM_CLANG_LIB_CODEGEN_CODEGENFUNCTION_H)
#define LLVM_CLANG_LIB_CODEGEN_CODEGENFUNCTION_H

#include "CGBuilder.h"
#include "CGDebugInfo.h"
#include "CGLoopInfo.h"
#include "CGValue.h"
#include "CodeGenModule.h"
#include "CodeGenPGO.h"
#include "EHScopeStack.h"
#include "VarBypassDetector.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/CurrentSourceLocExprScope.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/ExprOpenMP.h"
#include "clang/AST/StmtOpenMP.h"
#include "clang/AST/Type.h"
#include "clang/Basic/ABI.h"
#include "clang/Basic/CapturedStmt.h"
#include "clang/Basic/CodeGenOptions.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/TargetInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Frontend/OpenMP/OMPIRBuilder.h"
#include "llvm/IR/ValueHandle.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/SanitizerStats.h"

namespace llvm {
class BasicBlock;
class LLVMContext;
class MDNode;
class Module;
class SwitchInst;
class Twine;
class Value;
class CanonicalLoopInfo;
}

namespace clang {
class ASTContext;
class BlockDecl;
class CXXDestructorDecl;
class CXXForRangeStmt;
class CXXTryStmt;
class Decl;
class LabelDecl;
class EnumConstantDecl;
class FunctionDecl;
class FunctionProtoType;
class LabelStmt;
class ObjCContainerDecl;
class ObjCInterfaceDecl;
class ObjCIvarDecl;
class ObjCMethodDecl;
class ObjCImplementationDecl;
class ObjCPropertyImplDecl;
class TargetInfo;
class VarDecl;
class ObjCForCollectionStmt;
class ObjCAtTryStmt;
class ObjCAtThrowStmt;
class ObjCAtSynchronizedStmt;
class ObjCAutoreleasePoolStmt;
class OMPUseDevicePtrClause;
class OMPUseDeviceAddrClause;
class ReturnsNonNullAttr;
class SVETypeFlags;
class OMPExecutableDirective;

namespace analyze_os_log {
class OSLogBufferLayout;
}

namespace CodeGen {
class CodeGenTypes;
class CGCallee;
class CGFunctionInfo;
class CGRecordLayout;
class CGBlockInfo;
class CGCXXABI;
class BlockByrefHelpers;
class BlockByrefInfo;
class BlockFlags;
class BlockFieldFlags;
class RegionCodeGenTy;
class TargetCodeGenInfo;
struct OMPTaskDataTy;
struct CGCoroData;






enum TypeEvaluationKind {
TEK_Scalar,
TEK_Complex,
TEK_Aggregate
};

#define LIST_SANITIZER_CHECKS SANITIZER_CHECK(AddOverflow, add_overflow, 0) SANITIZER_CHECK(BuiltinUnreachable, builtin_unreachable, 0) SANITIZER_CHECK(CFICheckFail, cfi_check_fail, 0) SANITIZER_CHECK(DivremOverflow, divrem_overflow, 0) SANITIZER_CHECK(DynamicTypeCacheMiss, dynamic_type_cache_miss, 0) SANITIZER_CHECK(FloatCastOverflow, float_cast_overflow, 0) SANITIZER_CHECK(FunctionTypeMismatch, function_type_mismatch, 1) SANITIZER_CHECK(ImplicitConversion, implicit_conversion, 0) SANITIZER_CHECK(InvalidBuiltin, invalid_builtin, 0) SANITIZER_CHECK(InvalidObjCCast, invalid_objc_cast, 0) SANITIZER_CHECK(LoadInvalidValue, load_invalid_value, 0) SANITIZER_CHECK(MissingReturn, missing_return, 0) SANITIZER_CHECK(MulOverflow, mul_overflow, 0) SANITIZER_CHECK(NegateOverflow, negate_overflow, 0) SANITIZER_CHECK(NullabilityArg, nullability_arg, 0) SANITIZER_CHECK(NullabilityReturn, nullability_return, 1) SANITIZER_CHECK(NonnullArg, nonnull_arg, 0) SANITIZER_CHECK(NonnullReturn, nonnull_return, 1) SANITIZER_CHECK(OutOfBounds, out_of_bounds, 0) SANITIZER_CHECK(PointerOverflow, pointer_overflow, 0) SANITIZER_CHECK(ShiftOutOfBounds, shift_out_of_bounds, 0) SANITIZER_CHECK(SubOverflow, sub_overflow, 0) SANITIZER_CHECK(TypeMismatch, type_mismatch, 1) SANITIZER_CHECK(AlignmentAssumption, alignment_assumption, 0) SANITIZER_CHECK(VLABoundNotPositive, vla_bound_not_positive, 0)


























enum SanitizerHandler {
#define SANITIZER_CHECK(Enum, Name, Version) Enum,
LIST_SANITIZER_CHECKS
#undef SANITIZER_CHECK
};



struct DominatingLLVMValue {
typedef llvm::PointerIntPair<llvm::Value*, 1, bool> saved_type;


static bool needsSaving(llvm::Value *value) {

if (!isa<llvm::Instruction>(value)) return false;


llvm::BasicBlock *block = cast<llvm::Instruction>(value)->getParent();
return (block != &block->getParent()->getEntryBlock());
}

static saved_type save(CodeGenFunction &CGF, llvm::Value *value);
static llvm::Value *restore(CodeGenFunction &CGF, saved_type value);
};



template <class T> struct DominatingPointer<T,true> : DominatingLLVMValue {
typedef T *type;
static type restore(CodeGenFunction &CGF, saved_type value) {
return static_cast<T*>(DominatingLLVMValue::restore(CGF, value));
}
};


template <> struct DominatingValue<Address> {
typedef Address type;

struct saved_type {
DominatingLLVMValue::saved_type SavedValue;
CharUnits Alignment;
};

static bool needsSaving(type value) {
return DominatingLLVMValue::needsSaving(value.getPointer());
}
static saved_type save(CodeGenFunction &CGF, type value) {
return { DominatingLLVMValue::save(CGF, value.getPointer()),
value.getAlignment() };
}
static type restore(CodeGenFunction &CGF, saved_type value) {
return Address(DominatingLLVMValue::restore(CGF, value.SavedValue),
value.Alignment);
}
};


template <> struct DominatingValue<RValue> {
typedef RValue type;
class saved_type {
enum Kind { ScalarLiteral, ScalarAddress, AggregateLiteral,
AggregateAddress, ComplexAddress };

llvm::Value *Value;
unsigned K : 3;
unsigned Align : 29;
saved_type(llvm::Value *v, Kind k, unsigned a = 0)
: Value(v), K(k), Align(a) {}

public:
static bool needsSaving(RValue value);
static saved_type save(CodeGenFunction &CGF, RValue value);
RValue restore(CodeGenFunction &CGF);


};

static bool needsSaving(type value) {
return saved_type::needsSaving(value);
}
static saved_type save(CodeGenFunction &CGF, type value) {
return saved_type::save(CGF, value);
}
static type restore(CodeGenFunction &CGF, saved_type value) {
return value.restore(CGF);
}
};



class CodeGenFunction : public CodeGenTypeCache {
CodeGenFunction(const CodeGenFunction &) = delete;
void operator=(const CodeGenFunction &) = delete;

friend class CGCXXABI;
public:


struct JumpDest {
JumpDest() : Block(nullptr), ScopeDepth(), Index(0) {}
JumpDest(llvm::BasicBlock *Block,
EHScopeStack::stable_iterator Depth,
unsigned Index)
: Block(Block), ScopeDepth(Depth), Index(Index) {}

bool isValid() const { return Block != nullptr; }
llvm::BasicBlock *getBlock() const { return Block; }
EHScopeStack::stable_iterator getScopeDepth() const { return ScopeDepth; }
unsigned getDestIndex() const { return Index; }


void setScopeDepth(EHScopeStack::stable_iterator depth) {
ScopeDepth = depth;
}

private:
llvm::BasicBlock *Block;
EHScopeStack::stable_iterator ScopeDepth;
unsigned Index;
};

CodeGenModule &CGM;
const TargetInfo &Target;


CodeGenFunction *ParentCGF = nullptr;

typedef std::pair<llvm::Value *, llvm::Value *> ComplexPairTy;
LoopInfoStack LoopStack;
CGBuilderTy Builder;



VarBypassDetector Bypasses;













SmallVector<llvm::CanonicalLoopInfo *, 4> OMPLoopNestStack;


typedef llvm::function_ref<void(CodeGenFunction &, const OMPLoopDirective &,
JumpDest)>
CodeGenLoopTy;
typedef llvm::function_ref<void(CodeGenFunction &, SourceLocation,
const unsigned, const bool)>
CodeGenOrderedTy;


typedef llvm::function_ref<std::pair<LValue, LValue>(
CodeGenFunction &, const OMPExecutableDirective &S)>
CodeGenLoopBoundsTy;


typedef llvm::function_ref<std::pair<llvm::Value *, llvm::Value *>(
CodeGenFunction &, const OMPExecutableDirective &S, Address LB,
Address UB)>
CodeGenDispatchBoundsTy;



void InsertHelper(llvm::Instruction *I, const llvm::Twine &Name,
llvm::BasicBlock *BB,
llvm::BasicBlock::iterator InsertPt) const;



const Decl *CurFuncDecl;

const Decl *CurCodeDecl;
const CGFunctionInfo *CurFnInfo;
QualType FnRetTy;
llvm::Function *CurFn = nullptr;


llvm::SmallVector<const ParmVarDecl *, 4> FnArgs;




struct CGCoroInfo {
std::unique_ptr<CGCoroData> Data;
CGCoroInfo();
~CGCoroInfo();
};
CGCoroInfo CurCoro;

bool isCoroutine() const {
return CurCoro.Data != nullptr;
}


GlobalDecl CurGD;



EHScopeStack::stable_iterator PrologueCleanupDepth;


JumpDest ReturnBlock;



Address ReturnValue = Address::invalid();



Address ReturnValuePointer = Address::invalid();



const Expr *RetExpr = nullptr;


bool hasLabelBeenSeenInCurrentScope() const {
if (CurLexicalScope)
return CurLexicalScope->hasLabels();
return !LabelMap.empty();
}



llvm::AssertingVH<llvm::Instruction> AllocaInsertPt;


class CGCapturedStmtInfo {
public:
explicit CGCapturedStmtInfo(CapturedRegionKind K = CR_Default)
: Kind(K), ThisValue(nullptr), CXXThisFieldDecl(nullptr) {}
explicit CGCapturedStmtInfo(const CapturedStmt &S,
CapturedRegionKind K = CR_Default)
: Kind(K), ThisValue(nullptr), CXXThisFieldDecl(nullptr) {

RecordDecl::field_iterator Field =
S.getCapturedRecordDecl()->field_begin();
for (CapturedStmt::const_capture_iterator I = S.capture_begin(),
E = S.capture_end();
I != E; ++I, ++Field) {
if (I->capturesThis())
CXXThisFieldDecl = *Field;
else if (I->capturesVariable())
CaptureFields[I->getCapturedVar()->getCanonicalDecl()] = *Field;
else if (I->capturesVariableByCopy())
CaptureFields[I->getCapturedVar()->getCanonicalDecl()] = *Field;
}
}

virtual ~CGCapturedStmtInfo();

CapturedRegionKind getKind() const { return Kind; }

virtual void setContextValue(llvm::Value *V) { ThisValue = V; }

virtual llvm::Value *getContextValue() const { return ThisValue; }


virtual const FieldDecl *lookup(const VarDecl *VD) const {
return CaptureFields.lookup(VD->getCanonicalDecl());
}

bool isCXXThisExprCaptured() const { return getThisFieldDecl() != nullptr; }
virtual FieldDecl *getThisFieldDecl() const { return CXXThisFieldDecl; }

static bool classof(const CGCapturedStmtInfo *) {
return true;
}


virtual void EmitBody(CodeGenFunction &CGF, const Stmt *S) {
CGF.incrementProfileCounter(S);
CGF.EmitStmt(S);
}


virtual StringRef getHelperName() const { return "__captured_stmt"; }

private:

CapturedRegionKind Kind;


llvm::SmallDenseMap<const VarDecl *, FieldDecl *> CaptureFields;



llvm::Value *ThisValue;


FieldDecl *CXXThisFieldDecl;
};
CGCapturedStmtInfo *CapturedStmtInfo = nullptr;


class CGCapturedStmtRAII {
private:
CodeGenFunction &CGF;
CGCapturedStmtInfo *PrevCapturedStmtInfo;
public:
CGCapturedStmtRAII(CodeGenFunction &CGF,
CGCapturedStmtInfo *NewCapturedStmtInfo)
: CGF(CGF), PrevCapturedStmtInfo(CGF.CapturedStmtInfo) {
CGF.CapturedStmtInfo = NewCapturedStmtInfo;
}
~CGCapturedStmtRAII() { CGF.CapturedStmtInfo = PrevCapturedStmtInfo; }
};


class AbstractCallee {

const Decl *CalleeDecl;

public:
AbstractCallee() : CalleeDecl(nullptr) {}
AbstractCallee(const FunctionDecl *FD) : CalleeDecl(FD) {}
AbstractCallee(const ObjCMethodDecl *OMD) : CalleeDecl(OMD) {}
bool hasFunctionDecl() const {
return dyn_cast_or_null<FunctionDecl>(CalleeDecl);
}
const Decl *getDecl() const { return CalleeDecl; }
unsigned getNumParams() const {
if (const auto *FD = dyn_cast<FunctionDecl>(CalleeDecl))
return FD->getNumParams();
return cast<ObjCMethodDecl>(CalleeDecl)->param_size();
}
const ParmVarDecl *getParamDecl(unsigned I) const {
if (const auto *FD = dyn_cast<FunctionDecl>(CalleeDecl))
return FD->getParamDecl(I);
return *(cast<ObjCMethodDecl>(CalleeDecl)->param_begin() + I);
}
};


SanitizerSet SanOpts;


bool IsSanitizerScope = false;


class SanitizerScope {
CodeGenFunction *CGF;
public:
SanitizerScope(CodeGenFunction *CGF);
~SanitizerScope();
};



bool CurFuncIsThunk = false;


bool AutoreleaseResult = false;



bool SawAsmBlock = false;

const NamedDecl *CurSEHParent = nullptr;



bool IsOutlinedSEHHelper = false;



bool IsInPreservedAIRegion = false;


bool InNoMergeAttributedStmt = false;



const CallExpr *MustTailCall = nullptr;



bool checkIfFunctionMustProgress() {
if (CGM.getCodeGenOpts().getFiniteLoops() ==
CodeGenOptions::FiniteLoopsKind::Never)
return false;









return getLangOpts().CPlusPlus11;
}




bool checkIfLoopMustProgress(bool HasConstantCond) {
if (CGM.getCodeGenOpts().getFiniteLoops() ==
CodeGenOptions::FiniteLoopsKind::Always)
return true;
if (CGM.getCodeGenOpts().getFiniteLoops() ==
CodeGenOptions::FiniteLoopsKind::Never)
return false;



if (checkIfFunctionMustProgress())
return true;




if (HasConstantCond)
return false;


return getLangOpts().C11;
}

const CodeGen::CGBlockInfo *BlockInfo = nullptr;
llvm::Value *BlockPointer = nullptr;

llvm::DenseMap<const VarDecl *, FieldDecl *> LambdaCaptureFields;
FieldDecl *LambdaThisCaptureField = nullptr;



llvm::DenseMap<const VarDecl *, llvm::Value *> NRVOFlags;

EHScopeStack EHStack;
llvm::SmallVector<char, 256> LifetimeExtendedCleanupStack;
llvm::SmallVector<const JumpDest *, 2> SEHTryEpilogueStack;

llvm::Instruction *CurrentFuncletPad = nullptr;

class CallLifetimeEnd final : public EHScopeStack::Cleanup {
bool isRedundantBeforeReturn() override { return true; }

llvm::Value *Addr;
llvm::Value *Size;

public:
CallLifetimeEnd(Address addr, llvm::Value *size)
: Addr(addr.getPointer()), Size(size) {}

void Emit(CodeGenFunction &CGF, Flags flags) override {
CGF.EmitLifetimeEnd(Size, Addr);
}
};


struct LifetimeExtendedCleanupHeader {

unsigned Size;

unsigned Kind : 31;

unsigned IsConditional : 1;

size_t getSize() const { return Size; }
CleanupKind getKind() const { return (CleanupKind)Kind; }
bool isConditional() const { return IsConditional; }
};


Address NormalCleanupDest = Address::invalid();

unsigned NextCleanupDestIndex = 1;


llvm::BasicBlock *EHResumeBlock = nullptr;



llvm::Value *ExceptionSlot = nullptr;



llvm::AllocaInst *EHSelectorSlot = nullptr;




SmallVector<Address, 1> SEHCodeSlotStack;


llvm::Value *SEHInfo = nullptr;


llvm::BasicBlock *EmitLandingPad();

llvm::BasicBlock *getInvokeDestImpl();


const OMPExecutableDirective *OMPParentLoopDirectiveForScan = nullptr;
llvm::BasicBlock *OMPBeforeScanBlock = nullptr;
llvm::BasicBlock *OMPAfterScanBlock = nullptr;
llvm::BasicBlock *OMPScanExitBlock = nullptr;
llvm::BasicBlock *OMPScanDispatch = nullptr;
bool OMPFirstScanLoop = false;


class ParentLoopDirectiveForScanRegion {
CodeGenFunction &CGF;
const OMPExecutableDirective *ParentLoopDirectiveForScan;

public:
ParentLoopDirectiveForScanRegion(
CodeGenFunction &CGF,
const OMPExecutableDirective &ParentLoopDirectiveForScan)
: CGF(CGF),
ParentLoopDirectiveForScan(CGF.OMPParentLoopDirectiveForScan) {
CGF.OMPParentLoopDirectiveForScan = &ParentLoopDirectiveForScan;
}
~ParentLoopDirectiveForScanRegion() {
CGF.OMPParentLoopDirectiveForScan = ParentLoopDirectiveForScan;
}
};

template <class T>
typename DominatingValue<T>::saved_type saveValueInCond(T value) {
return DominatingValue<T>::save(*this, value);
}

class CGFPOptionsRAII {
public:
CGFPOptionsRAII(CodeGenFunction &CGF, FPOptions FPFeatures);
CGFPOptionsRAII(CodeGenFunction &CGF, const Expr *E);
~CGFPOptionsRAII();

private:
void ConstructorHelper(FPOptions FPFeatures);
CodeGenFunction &CGF;
FPOptions OldFPFeatures;
llvm::fp::ExceptionBehavior OldExcept;
llvm::RoundingMode OldRounding;
Optional<CGBuilderTy::FastMathFlagGuard> FMFGuard;
};
FPOptions CurFPFeatures;

public:


SmallVector<llvm::Value*, 8> ObjCEHValueStack;


class FinallyInfo {

JumpDest RethrowDest;


llvm::FunctionCallee BeginCatchFn;



llvm::AllocaInst *ForEHVar;



llvm::AllocaInst *SavedExnVar;

public:
void enter(CodeGenFunction &CGF, const Stmt *Finally,
llvm::FunctionCallee beginCatchFn,
llvm::FunctionCallee endCatchFn, llvm::FunctionCallee rethrowFn);
void exit(CodeGenFunction &CGF);
};


bool isSEHTryScope() const { return !SEHTryEpilogueStack.empty(); }


bool isCleanupPadScope() const {
return CurrentFuncletPad && isa<llvm::CleanupPadInst>(CurrentFuncletPad);
}




template <class T, class... As>
void pushFullExprCleanup(CleanupKind kind, As... A) {


if (!isInConditionalBranch())
return EHStack.pushCleanup<T>(kind, A...);


typedef std::tuple<typename DominatingValue<As>::saved_type...> SavedTuple;
SavedTuple Saved{saveValueInCond(A)...};

typedef EHScopeStack::ConditionalCleanup<T, As...> CleanupType;
EHStack.pushCleanupTuple<CleanupType>(kind, Saved);
initFullExprCleanup();
}



template <class T, class... As>
void pushCleanupAfterFullExpr(CleanupKind Kind, As... A) {
if (!isInConditionalBranch())
return pushCleanupAfterFullExprWithActiveFlag<T>(Kind, Address::invalid(),
A...);

Address ActiveFlag = createCleanupActiveFlag();
assert(!DominatingValue<Address>::needsSaving(ActiveFlag) &&
"cleanup active flag should never need saving");

typedef std::tuple<typename DominatingValue<As>::saved_type...> SavedTuple;
SavedTuple Saved{saveValueInCond(A)...};

typedef EHScopeStack::ConditionalCleanup<T, As...> CleanupType;
pushCleanupAfterFullExprWithActiveFlag<CleanupType>(Kind, ActiveFlag, Saved);
}

template <class T, class... As>
void pushCleanupAfterFullExprWithActiveFlag(CleanupKind Kind,
Address ActiveFlag, As... A) {
LifetimeExtendedCleanupHeader Header = {sizeof(T), Kind,
ActiveFlag.isValid()};

size_t OldSize = LifetimeExtendedCleanupStack.size();
LifetimeExtendedCleanupStack.resize(
LifetimeExtendedCleanupStack.size() + sizeof(Header) + Header.Size +
(Header.IsConditional ? sizeof(ActiveFlag) : 0));

static_assert(sizeof(Header) % alignof(T) == 0,
"Cleanup will be allocated on misaligned address");
char *Buffer = &LifetimeExtendedCleanupStack[OldSize];
new (Buffer) LifetimeExtendedCleanupHeader(Header);
new (Buffer + sizeof(Header)) T(A...);
if (Header.IsConditional)
new (Buffer + sizeof(Header) + sizeof(T)) Address(ActiveFlag);
}



void initFullExprCleanup() {
initFullExprCleanupWithFlag(createCleanupActiveFlag());
}

void initFullExprCleanupWithFlag(Address ActiveFlag);
Address createCleanupActiveFlag();





void PushDestructorCleanup(QualType T, Address Addr);




void PushDestructorCleanup(const CXXDestructorDecl *Dtor, QualType T,
Address Addr);



void PopCleanupBlock(bool FallThroughIsBranchThrough = false);









void DeactivateCleanupBlock(EHScopeStack::stable_iterator Cleanup,
llvm::Instruction *DominatingIP);








void ActivateCleanupBlock(EHScopeStack::stable_iterator Cleanup,
llvm::Instruction *DominatingIP);



class RunCleanupsScope {
EHScopeStack::stable_iterator CleanupStackDepth, OldCleanupScopeDepth;
size_t LifetimeExtendedCleanupStackSize;
bool OldDidCallStackSave;
protected:
bool PerformCleanup;
private:

RunCleanupsScope(const RunCleanupsScope &) = delete;
void operator=(const RunCleanupsScope &) = delete;

protected:
CodeGenFunction& CGF;

public:

explicit RunCleanupsScope(CodeGenFunction &CGF)
: PerformCleanup(true), CGF(CGF)
{
CleanupStackDepth = CGF.EHStack.stable_begin();
LifetimeExtendedCleanupStackSize =
CGF.LifetimeExtendedCleanupStack.size();
OldDidCallStackSave = CGF.DidCallStackSave;
CGF.DidCallStackSave = false;
OldCleanupScopeDepth = CGF.CurrentCleanupScopeDepth;
CGF.CurrentCleanupScopeDepth = CleanupStackDepth;
}


~RunCleanupsScope() {
if (PerformCleanup)
ForceCleanup();
}


bool requiresCleanups() const {
return CGF.EHStack.stable_begin() != CleanupStackDepth;
}







void ForceCleanup(std::initializer_list<llvm::Value**> ValuesToReload = {}) {
assert(PerformCleanup && "Already forced cleanup");
CGF.DidCallStackSave = OldDidCallStackSave;
CGF.PopCleanupBlocks(CleanupStackDepth, LifetimeExtendedCleanupStackSize,
ValuesToReload);
PerformCleanup = false;
CGF.CurrentCleanupScopeDepth = OldCleanupScopeDepth;
}
};


EHScopeStack::stable_iterator CurrentCleanupScopeDepth =
EHScopeStack::stable_end();

class LexicalScope : public RunCleanupsScope {
SourceRange Range;
SmallVector<const LabelDecl*, 4> Labels;
LexicalScope *ParentScope;

LexicalScope(const LexicalScope &) = delete;
void operator=(const LexicalScope &) = delete;

public:

explicit LexicalScope(CodeGenFunction &CGF, SourceRange Range)
: RunCleanupsScope(CGF), Range(Range), ParentScope(CGF.CurLexicalScope) {
CGF.CurLexicalScope = this;
if (CGDebugInfo *DI = CGF.getDebugInfo())
DI->EmitLexicalBlockStart(CGF.Builder, Range.getBegin());
}

void addLabel(const LabelDecl *label) {
assert(PerformCleanup && "adding label to dead scope?");
Labels.push_back(label);
}



~LexicalScope() {
if (CGDebugInfo *DI = CGF.getDebugInfo())
DI->EmitLexicalBlockEnd(CGF.Builder, Range.getEnd());



if (PerformCleanup) {
ApplyDebugLocation DL(CGF, Range.getEnd());
ForceCleanup();
}
}



void ForceCleanup() {
CGF.CurLexicalScope = ParentScope;
RunCleanupsScope::ForceCleanup();

if (!Labels.empty())
rescopeLabels();
}

bool hasLabels() const {
return !Labels.empty();
}

void rescopeLabels();
};

typedef llvm::DenseMap<const Decl *, Address> DeclMapTy;


class OMPMapVars {
DeclMapTy SavedLocals;
DeclMapTy SavedTempAddresses;
OMPMapVars(const OMPMapVars &) = delete;
void operator=(const OMPMapVars &) = delete;

public:
explicit OMPMapVars() = default;
~OMPMapVars() {
assert(SavedLocals.empty() && "Did not restored original addresses.");
};




bool setVarAddr(CodeGenFunction &CGF, const VarDecl *LocalVD,
Address TempAddr) {
LocalVD = LocalVD->getCanonicalDecl();

if (SavedLocals.count(LocalVD)) return false;


auto it = CGF.LocalDeclMap.find(LocalVD);
if (it != CGF.LocalDeclMap.end())
SavedLocals.try_emplace(LocalVD, it->second);
else
SavedLocals.try_emplace(LocalVD, Address::invalid());


QualType VarTy = LocalVD->getType();
if (VarTy->isReferenceType()) {
Address Temp = CGF.CreateMemTemp(VarTy);
CGF.Builder.CreateStore(TempAddr.getPointer(), Temp);
TempAddr = Temp;
}
SavedTempAddresses.try_emplace(LocalVD, TempAddr);

return true;
}




bool apply(CodeGenFunction &CGF) {
copyInto(SavedTempAddresses, CGF.LocalDeclMap);
SavedTempAddresses.clear();
return !SavedLocals.empty();
}


void restore(CodeGenFunction &CGF) {
if (!SavedLocals.empty()) {
copyInto(SavedLocals, CGF.LocalDeclMap);
SavedLocals.clear();
}
}

private:


static void copyInto(const DeclMapTy &Src, DeclMapTy &Dest) {
for (auto &Pair : Src) {
if (!Pair.second.isValid()) {
Dest.erase(Pair.first);
continue;
}

auto I = Dest.find(Pair.first);
if (I != Dest.end())
I->second = Pair.second;
else
Dest.insert(Pair);
}
}
};




class OMPPrivateScope : public RunCleanupsScope {
OMPMapVars MappedVars;
OMPPrivateScope(const OMPPrivateScope &) = delete;
void operator=(const OMPPrivateScope &) = delete;

public:

explicit OMPPrivateScope(CodeGenFunction &CGF) : RunCleanupsScope(CGF) {}






bool addPrivate(const VarDecl *LocalVD,
const llvm::function_ref<Address()> PrivateGen) {
assert(PerformCleanup && "adding private to dead scope");
return MappedVars.setVarAddr(CGF, LocalVD, PrivateGen());
}









bool Privatize() { return MappedVars.apply(CGF); }

void ForceCleanup() {
RunCleanupsScope::ForceCleanup();
MappedVars.restore(CGF);
}


~OMPPrivateScope() {
if (PerformCleanup)
ForceCleanup();
}


bool isGlobalVarCaptured(const VarDecl *VD) const {
VD = VD->getCanonicalDecl();
return !VD->isLocalVarDeclOrParm() && CGF.LocalDeclMap.count(VD) > 0;
}
};




class OMPLocalDeclMapRAII {
CodeGenFunction &CGF;
DeclMapTy SavedMap;

public:
OMPLocalDeclMapRAII(CodeGenFunction &CGF)
: CGF(CGF), SavedMap(CGF.LocalDeclMap) {}
~OMPLocalDeclMapRAII() { SavedMap.swap(CGF.LocalDeclMap); }
};



void
PopCleanupBlocks(EHScopeStack::stable_iterator OldCleanupStackSize,
std::initializer_list<llvm::Value **> ValuesToReload = {});




void
PopCleanupBlocks(EHScopeStack::stable_iterator OldCleanupStackSize,
size_t OldLifetimeExtendedStackSize,
std::initializer_list<llvm::Value **> ValuesToReload = {});

void ResolveBranchFixups(llvm::BasicBlock *Target);




JumpDest getJumpDestInCurrentScope(llvm::BasicBlock *Target) {
return JumpDest(Target,
EHStack.getInnermostNormalCleanup(),
NextCleanupDestIndex++);
}




JumpDest getJumpDestInCurrentScope(StringRef Name = StringRef()) {
return getJumpDestInCurrentScope(createBasicBlock(Name));
}




void EmitBranchThroughCleanup(JumpDest Dest);




bool isObviouslyBranchWithoutCleanups(JumpDest Dest) const;




void popCatchScope();

llvm::BasicBlock *getEHResumeBlock(bool isCleanup);
llvm::BasicBlock *getEHDispatchBlock(EHScopeStack::stable_iterator scope);
llvm::BasicBlock *
getFuncletEHDispatchBlock(EHScopeStack::stable_iterator scope);


class ConditionalEvaluation {
llvm::BasicBlock *StartBB;

public:
ConditionalEvaluation(CodeGenFunction &CGF)
: StartBB(CGF.Builder.GetInsertBlock()) {}

void begin(CodeGenFunction &CGF) {
assert(CGF.OutermostConditional != this);
if (!CGF.OutermostConditional)
CGF.OutermostConditional = this;
}

void end(CodeGenFunction &CGF) {
assert(CGF.OutermostConditional != nullptr);
if (CGF.OutermostConditional == this)
CGF.OutermostConditional = nullptr;
}



llvm::BasicBlock *getStartingBlock() const {
return StartBB;
}
};



bool isInConditionalBranch() const { return OutermostConditional != nullptr; }

void setBeforeOutermostConditional(llvm::Value *value, Address addr) {
assert(isInConditionalBranch());
llvm::BasicBlock *block = OutermostConditional->getStartingBlock();
auto store = new llvm::StoreInst(value, addr.getPointer(), &block->back());
store->setAlignment(addr.getAlignment().getAsAlign());
}



class StmtExprEvaluation {
CodeGenFunction &CGF;




ConditionalEvaluation *SavedOutermostConditional;

public:
StmtExprEvaluation(CodeGenFunction &CGF)
: CGF(CGF), SavedOutermostConditional(CGF.OutermostConditional) {
CGF.OutermostConditional = nullptr;
}

~StmtExprEvaluation() {
CGF.OutermostConditional = SavedOutermostConditional;
CGF.EnsureInsertPoint();
}
};




class PeepholeProtection {
llvm::Instruction *Inst;
friend class CodeGenFunction;

public:
PeepholeProtection() : Inst(nullptr) {}
};







class OpaqueValueMappingData {
const OpaqueValueExpr *OpaqueValue;
bool BoundLValue;
CodeGenFunction::PeepholeProtection Protection;

OpaqueValueMappingData(const OpaqueValueExpr *ov,
bool boundLValue)
: OpaqueValue(ov), BoundLValue(boundLValue) {}
public:
OpaqueValueMappingData() : OpaqueValue(nullptr) {}

static bool shouldBindAsLValue(const Expr *expr) {





return expr->isGLValue() ||
expr->getType()->isFunctionType() ||
hasAggregateEvaluationKind(expr->getType());
}

static OpaqueValueMappingData bind(CodeGenFunction &CGF,
const OpaqueValueExpr *ov,
const Expr *e) {
if (shouldBindAsLValue(ov))
return bind(CGF, ov, CGF.EmitLValue(e));
return bind(CGF, ov, CGF.EmitAnyExpr(e));
}

static OpaqueValueMappingData bind(CodeGenFunction &CGF,
const OpaqueValueExpr *ov,
const LValue &lv) {
assert(shouldBindAsLValue(ov));
CGF.OpaqueLValues.insert(std::make_pair(ov, lv));
return OpaqueValueMappingData(ov, true);
}

static OpaqueValueMappingData bind(CodeGenFunction &CGF,
const OpaqueValueExpr *ov,
const RValue &rv) {
assert(!shouldBindAsLValue(ov));
CGF.OpaqueRValues.insert(std::make_pair(ov, rv));

OpaqueValueMappingData data(ov, false);




data.Protection = CGF.protectFromPeepholes(rv);

return data;
}

bool isValid() const { return OpaqueValue != nullptr; }
void clear() { OpaqueValue = nullptr; }

void unbind(CodeGenFunction &CGF) {
assert(OpaqueValue && "no data to unbind!");

if (BoundLValue) {
CGF.OpaqueLValues.erase(OpaqueValue);
} else {
CGF.OpaqueRValues.erase(OpaqueValue);
CGF.unprotectFromPeepholes(Protection);
}
}
};


class OpaqueValueMapping {
CodeGenFunction &CGF;
OpaqueValueMappingData Data;

public:
static bool shouldBindAsLValue(const Expr *expr) {
return OpaqueValueMappingData::shouldBindAsLValue(expr);
}






OpaqueValueMapping(CodeGenFunction &CGF,
const AbstractConditionalOperator *op) : CGF(CGF) {
if (isa<ConditionalOperator>(op))

return;

const BinaryConditionalOperator *e = cast<BinaryConditionalOperator>(op);
Data = OpaqueValueMappingData::bind(CGF, e->getOpaqueValue(),
e->getCommon());
}



OpaqueValueMapping(CodeGenFunction &CGF, const OpaqueValueExpr *OV)
: CGF(CGF) {
if (OV) {
assert(OV->getSourceExpr() && "wrong form of OpaqueValueMapping used "
"for OVE with no source expression");
Data = OpaqueValueMappingData::bind(CGF, OV, OV->getSourceExpr());
}
}

OpaqueValueMapping(CodeGenFunction &CGF,
const OpaqueValueExpr *opaqueValue,
LValue lvalue)
: CGF(CGF), Data(OpaqueValueMappingData::bind(CGF, opaqueValue, lvalue)) {
}

OpaqueValueMapping(CodeGenFunction &CGF,
const OpaqueValueExpr *opaqueValue,
RValue rvalue)
: CGF(CGF), Data(OpaqueValueMappingData::bind(CGF, opaqueValue, rvalue)) {
}

void pop() {
Data.unbind(CGF);
Data.clear();
}

~OpaqueValueMapping() {
if (Data.isValid()) Data.unbind(CGF);
}
};

private:
CGDebugInfo *DebugInfo;

unsigned VLAExprCounter = 0;
bool DisableDebugInfo = false;



bool DidCallStackSave = false;





llvm::IndirectBrInst *IndirectBranch = nullptr;



DeclMapTy LocalDeclMap;



llvm::DenseMap<const ParmVarDecl *, EHScopeStack::stable_iterator>
CalleeDestructedParamCleanups;




llvm::SmallDenseMap<const ParmVarDecl *, const ImplicitParamDecl *, 2>
SizeArguments;



llvm::DenseMap<llvm::AllocaInst *, int> EscapedLocals;


llvm::DenseMap<const LabelDecl*, JumpDest> LabelMap;



struct BreakContinue {
BreakContinue(JumpDest Break, JumpDest Continue)
: BreakBlock(Break), ContinueBlock(Continue) {}

JumpDest BreakBlock;
JumpDest ContinueBlock;
};
SmallVector<BreakContinue, 8> BreakContinueStack;


class OpenMPCancelExitStack {


struct CancelExit {
CancelExit() = default;
CancelExit(OpenMPDirectiveKind Kind, JumpDest ExitBlock,
JumpDest ContBlock)
: Kind(Kind), ExitBlock(ExitBlock), ContBlock(ContBlock) {}
OpenMPDirectiveKind Kind = llvm::omp::OMPD_unknown;


bool HasBeenEmitted = false;
JumpDest ExitBlock;
JumpDest ContBlock;
};

SmallVector<CancelExit, 8> Stack;

public:
OpenMPCancelExitStack() : Stack(1) {}
~OpenMPCancelExitStack() = default;

JumpDest getExitBlock() const { return Stack.back().ExitBlock; }


void emitExit(CodeGenFunction &CGF, OpenMPDirectiveKind Kind,
const llvm::function_ref<void(CodeGenFunction &)> CodeGen) {
if (Stack.back().Kind == Kind && getExitBlock().isValid()) {
assert(CGF.getOMPCancelDestination(Kind).isValid());
assert(CGF.HaveInsertPoint());
assert(!Stack.back().HasBeenEmitted);
auto IP = CGF.Builder.saveAndClearIP();
CGF.EmitBlock(Stack.back().ExitBlock.getBlock());
CodeGen(CGF);
CGF.EmitBranch(Stack.back().ContBlock.getBlock());
CGF.Builder.restoreIP(IP);
Stack.back().HasBeenEmitted = true;
}
CodeGen(CGF);
}




void enter(CodeGenFunction &CGF, OpenMPDirectiveKind Kind, bool HasCancel) {
Stack.push_back({Kind,
HasCancel ? CGF.getJumpDestInCurrentScope("cancel.exit")
: JumpDest(),
HasCancel ? CGF.getJumpDestInCurrentScope("cancel.cont")
: JumpDest()});
}


void exit(CodeGenFunction &CGF) {
if (getExitBlock().isValid()) {
assert(CGF.getOMPCancelDestination(Stack.back().Kind).isValid());
bool HaveIP = CGF.HaveInsertPoint();
if (!Stack.back().HasBeenEmitted) {
if (HaveIP)
CGF.EmitBranchThroughCleanup(Stack.back().ContBlock);
CGF.EmitBlock(Stack.back().ExitBlock.getBlock());
CGF.EmitBranchThroughCleanup(Stack.back().ContBlock);
}
CGF.EmitBlock(Stack.back().ContBlock.getBlock());
if (!HaveIP) {
CGF.Builder.CreateUnreachable();
CGF.Builder.ClearInsertionPoint();
}
}
Stack.pop_back();
}
};
OpenMPCancelExitStack OMPCancelStack;


llvm::Value *emitCondLikelihoodViaExpectIntrinsic(llvm::Value *Cond,
Stmt::Likelihood LH);

CodeGenPGO PGO;


llvm::MDNode *createProfileWeights(uint64_t TrueCount,
uint64_t FalseCount) const;
llvm::MDNode *createProfileWeights(ArrayRef<uint64_t> Weights) const;
llvm::MDNode *createProfileWeightsForLoop(const Stmt *Cond,
uint64_t LoopCount) const;

public:


void incrementProfileCounter(const Stmt *S, llvm::Value *StepV = nullptr) {
if (CGM.getCodeGenOpts().hasProfileClangInstr() &&
!CurFn->hasFnAttribute(llvm::Attribute::NoProfile))
PGO.emitCounterIncrement(Builder, S, StepV);
PGO.setCurrentStmt(S);
}


uint64_t getProfileCount(const Stmt *S) {
Optional<uint64_t> Count = PGO.getStmtCount(S);
if (!Count.hasValue())
return 0;
return *Count;
}


void setCurrentProfileCount(uint64_t Count) {
PGO.setCurrentRegionCount(Count);
}



uint64_t getCurrentProfileCount() {
return PGO.getCurrentRegionCount();
}

private:



llvm::SwitchInst *SwitchInsn = nullptr;

SmallVector<uint64_t, 16> *SwitchWeights = nullptr;


SmallVector<Stmt::Likelihood, 16> *SwitchLikelihood = nullptr;



llvm::BasicBlock *CaseRangeBlock = nullptr;



llvm::DenseMap<const OpaqueValueExpr *, LValue> OpaqueLValues;
llvm::DenseMap<const OpaqueValueExpr *, RValue> OpaqueRValues;







llvm::DenseMap<const Expr*, llvm::Value*> VLASizeMap;



llvm::BasicBlock *UnreachableBlock = nullptr;


unsigned NumReturnExprs = 0;


unsigned NumSimpleReturnExprs = 0;


SourceLocation LastStopPoint;

public:


CurrentSourceLocExprScope CurSourceLocExprScope;
using SourceLocExprScopeGuard =
CurrentSourceLocExprScope::SourceLocExprScopeGuard;




class FieldConstructionScope {
public:
FieldConstructionScope(CodeGenFunction &CGF, Address This)
: CGF(CGF), OldCXXDefaultInitExprThis(CGF.CXXDefaultInitExprThis) {
CGF.CXXDefaultInitExprThis = This;
}
~FieldConstructionScope() {
CGF.CXXDefaultInitExprThis = OldCXXDefaultInitExprThis;
}

private:
CodeGenFunction &CGF;
Address OldCXXDefaultInitExprThis;
};



class CXXDefaultInitExprScope {
public:
CXXDefaultInitExprScope(CodeGenFunction &CGF, const CXXDefaultInitExpr *E)
: CGF(CGF), OldCXXThisValue(CGF.CXXThisValue),
OldCXXThisAlignment(CGF.CXXThisAlignment),
SourceLocScope(E, CGF.CurSourceLocExprScope) {
CGF.CXXThisValue = CGF.CXXDefaultInitExprThis.getPointer();
CGF.CXXThisAlignment = CGF.CXXDefaultInitExprThis.getAlignment();
}
~CXXDefaultInitExprScope() {
CGF.CXXThisValue = OldCXXThisValue;
CGF.CXXThisAlignment = OldCXXThisAlignment;
}

public:
CodeGenFunction &CGF;
llvm::Value *OldCXXThisValue;
CharUnits OldCXXThisAlignment;
SourceLocExprScopeGuard SourceLocScope;
};

struct CXXDefaultArgExprScope : SourceLocExprScopeGuard {
CXXDefaultArgExprScope(CodeGenFunction &CGF, const CXXDefaultArgExpr *E)
: SourceLocExprScopeGuard(E, CGF.CurSourceLocExprScope) {}
};



class ArrayInitLoopExprScope {
public:
ArrayInitLoopExprScope(CodeGenFunction &CGF, llvm::Value *Index)
: CGF(CGF), OldArrayInitIndex(CGF.ArrayInitIndex) {
CGF.ArrayInitIndex = Index;
}
~ArrayInitLoopExprScope() {
CGF.ArrayInitIndex = OldArrayInitIndex;
}

private:
CodeGenFunction &CGF;
llvm::Value *OldArrayInitIndex;
};

class InlinedInheritingConstructorScope {
public:
InlinedInheritingConstructorScope(CodeGenFunction &CGF, GlobalDecl GD)
: CGF(CGF), OldCurGD(CGF.CurGD), OldCurFuncDecl(CGF.CurFuncDecl),
OldCurCodeDecl(CGF.CurCodeDecl),
OldCXXABIThisDecl(CGF.CXXABIThisDecl),
OldCXXABIThisValue(CGF.CXXABIThisValue),
OldCXXThisValue(CGF.CXXThisValue),
OldCXXABIThisAlignment(CGF.CXXABIThisAlignment),
OldCXXThisAlignment(CGF.CXXThisAlignment),
OldReturnValue(CGF.ReturnValue), OldFnRetTy(CGF.FnRetTy),
OldCXXInheritedCtorInitExprArgs(
std::move(CGF.CXXInheritedCtorInitExprArgs)) {
CGF.CurGD = GD;
CGF.CurFuncDecl = CGF.CurCodeDecl =
cast<CXXConstructorDecl>(GD.getDecl());
CGF.CXXABIThisDecl = nullptr;
CGF.CXXABIThisValue = nullptr;
CGF.CXXThisValue = nullptr;
CGF.CXXABIThisAlignment = CharUnits();
CGF.CXXThisAlignment = CharUnits();
CGF.ReturnValue = Address::invalid();
CGF.FnRetTy = QualType();
CGF.CXXInheritedCtorInitExprArgs.clear();
}
~InlinedInheritingConstructorScope() {
CGF.CurGD = OldCurGD;
CGF.CurFuncDecl = OldCurFuncDecl;
CGF.CurCodeDecl = OldCurCodeDecl;
CGF.CXXABIThisDecl = OldCXXABIThisDecl;
CGF.CXXABIThisValue = OldCXXABIThisValue;
CGF.CXXThisValue = OldCXXThisValue;
CGF.CXXABIThisAlignment = OldCXXABIThisAlignment;
CGF.CXXThisAlignment = OldCXXThisAlignment;
CGF.ReturnValue = OldReturnValue;
CGF.FnRetTy = OldFnRetTy;
CGF.CXXInheritedCtorInitExprArgs =
std::move(OldCXXInheritedCtorInitExprArgs);
}

private:
CodeGenFunction &CGF;
GlobalDecl OldCurGD;
const Decl *OldCurFuncDecl;
const Decl *OldCurCodeDecl;
ImplicitParamDecl *OldCXXABIThisDecl;
llvm::Value *OldCXXABIThisValue;
llvm::Value *OldCXXThisValue;
CharUnits OldCXXABIThisAlignment;
CharUnits OldCXXThisAlignment;
Address OldReturnValue;
QualType OldFnRetTy;
CallArgList OldCXXInheritedCtorInitExprArgs;
};












struct OMPBuilderCBHelpers {

OMPBuilderCBHelpers() = delete;
OMPBuilderCBHelpers(const OMPBuilderCBHelpers &) = delete;
OMPBuilderCBHelpers &operator=(const OMPBuilderCBHelpers &) = delete;

using InsertPointTy = llvm::OpenMPIRBuilder::InsertPointTy;


class OMPAllocateCleanupTy final : public EHScopeStack::Cleanup {

private:
llvm::CallInst *RTLFnCI;

public:
OMPAllocateCleanupTy(llvm::CallInst *RLFnCI) : RTLFnCI(RLFnCI) {
RLFnCI->removeFromParent();
}

void Emit(CodeGenFunction &CGF, Flags ) override {
if (!CGF.HaveInsertPoint())
return;
CGF.Builder.Insert(RTLFnCI);
}
};







static Address getAddrOfThreadPrivate(CodeGenFunction &CGF,
const VarDecl *VD, Address VDAddr,
SourceLocation Loc);


static Address getAddressOfLocalVariable(CodeGenFunction &CGF,
const VarDecl *VD);






static std::string getNameWithSeparators(ArrayRef<StringRef> Parts,
StringRef FirstSeparator = ".",
StringRef Separator = ".");



static void FinalizeOMPRegion(CodeGenFunction &CGF, InsertPointTy IP) {
CGBuilderTy::InsertPointGuard IPG(CGF.Builder);
assert(IP.getBlock()->end() != IP.getPoint() &&
"OpenMP IR Builder should cause terminated block!");

llvm::BasicBlock *IPBB = IP.getBlock();
llvm::BasicBlock *DestBB = IPBB->getUniqueSuccessor();
assert(DestBB && "Finalization block should have one successor!");


IPBB->getTerminator()->eraseFromParent();
CGF.Builder.SetInsertPoint(IPBB);
CodeGenFunction::JumpDest Dest = CGF.getJumpDestInCurrentScope(DestBB);
CGF.EmitBranchThroughCleanup(Dest);
}







static void EmitOMPRegionBody(CodeGenFunction &CGF,
const Stmt *RegionBodyStmt,
InsertPointTy CodeGenIP,
llvm::BasicBlock &FiniBB) {
llvm::BasicBlock *CodeGenIPBB = CodeGenIP.getBlock();
if (llvm::Instruction *CodeGenIPBBTI = CodeGenIPBB->getTerminator())
CodeGenIPBBTI->eraseFromParent();

CGF.Builder.SetInsertPoint(CodeGenIPBB);

CGF.EmitStmt(RegionBodyStmt);

if (CGF.Builder.saveIP().isSet())
CGF.Builder.CreateBr(&FiniBB);
}


class OutlinedRegionBodyRAII {

llvm::AssertingVH<llvm::Instruction> OldAllocaIP;
CodeGenFunction::JumpDest OldReturnBlock;
CGBuilderTy::InsertPoint IP;
CodeGenFunction &CGF;

public:
OutlinedRegionBodyRAII(CodeGenFunction &cgf, InsertPointTy &AllocaIP,
llvm::BasicBlock &RetBB)
: CGF(cgf) {
assert(AllocaIP.isSet() &&
"Must specify Insertion point for allocas of outlined function");
OldAllocaIP = CGF.AllocaInsertPt;
CGF.AllocaInsertPt = &*AllocaIP.getPoint();
IP = CGF.Builder.saveIP();

OldReturnBlock = CGF.ReturnBlock;
CGF.ReturnBlock = CGF.getJumpDestInCurrentScope(&RetBB);
}

~OutlinedRegionBodyRAII() {
CGF.AllocaInsertPt = OldAllocaIP;
CGF.ReturnBlock = OldReturnBlock;
CGF.Builder.restoreIP(IP);
}
};


class InlinedRegionBodyRAII {

llvm::AssertingVH<llvm::Instruction> OldAllocaIP;
CodeGenFunction &CGF;

public:
InlinedRegionBodyRAII(CodeGenFunction &cgf, InsertPointTy &AllocaIP,
llvm::BasicBlock &FiniBB)
: CGF(cgf) {




assert((!AllocaIP.isSet() ||
CGF.AllocaInsertPt->getParent() == AllocaIP.getBlock()) &&
"Insertion point should be in the entry block of containing "
"function!");
OldAllocaIP = CGF.AllocaInsertPt;
if (AllocaIP.isSet())
CGF.AllocaInsertPt = &*AllocaIP.getPoint();






(void)CGF.getJumpDestInCurrentScope(&FiniBB);
}

~InlinedRegionBodyRAII() { CGF.AllocaInsertPt = OldAllocaIP; }
};
};

private:


ImplicitParamDecl *CXXABIThisDecl = nullptr;
llvm::Value *CXXABIThisValue = nullptr;
llvm::Value *CXXThisValue = nullptr;
CharUnits CXXABIThisAlignment;
CharUnits CXXThisAlignment;



Address CXXDefaultInitExprThis = Address::invalid();



llvm::Value *ArrayInitIndex = nullptr;



CallArgList CXXInheritedCtorInitExprArgs;



ImplicitParamDecl *CXXStructorImplicitParamDecl = nullptr;
llvm::Value *CXXStructorImplicitParamValue = nullptr;




ConditionalEvaluation *OutermostConditional = nullptr;


LexicalScope *CurLexicalScope = nullptr;



SourceLocation CurEHLocation;



llvm::DenseMap<const ValueDecl *, BlockByrefInfo> BlockByrefInfos;



llvm::Value *RetValNullabilityPrecondition = nullptr;



bool requiresReturnValueNullabilityCheck() const {
return RetValNullabilityPrecondition;
}



Address ReturnLocation = Address::invalid();


bool requiresReturnValueCheck() const;

llvm::BasicBlock *TerminateLandingPad = nullptr;
llvm::BasicBlock *TerminateHandler = nullptr;
llvm::SmallVector<llvm::BasicBlock *, 2> TrapBBs;


llvm::MapVector<llvm::Value *, llvm::BasicBlock *> TerminateFunclets;



unsigned LargestVectorWidth = 0;



bool ShouldEmitLifetimeMarkers;



void EmitOpenCLKernelMetadata(const FunctionDecl *FD,
llvm::Function *Fn);

public:
CodeGenFunction(CodeGenModule &cgm, bool suppressNewContext=false);
~CodeGenFunction();

CodeGenTypes &getTypes() const { return CGM.getTypes(); }
ASTContext &getContext() const { return CGM.getContext(); }
CGDebugInfo *getDebugInfo() {
if (DisableDebugInfo)
return nullptr;
return DebugInfo;
}
void disableDebugInfo() { DisableDebugInfo = true; }
void enableDebugInfo() { DisableDebugInfo = false; }

bool shouldUseFusedARCCalls() {
return CGM.getCodeGenOpts().OptimizationLevel == 0;
}

const LangOptions &getLangOpts() const { return CGM.getLangOpts(); }



Address getExceptionSlot();
Address getEHSelectorSlot();



llvm::Value *getExceptionFromSlot();
llvm::Value *getSelectorFromSlot();

Address getNormalCleanupDestSlot();

llvm::BasicBlock *getUnreachableBlock() {
if (!UnreachableBlock) {
UnreachableBlock = createBasicBlock("unreachable");
new llvm::UnreachableInst(getLLVMContext(), UnreachableBlock);
}
return UnreachableBlock;
}

llvm::BasicBlock *getInvokeDest() {
if (!EHStack.requiresLandingPad()) return nullptr;
return getInvokeDestImpl();
}

bool currentFunctionUsesSEHTry() const { return CurSEHParent != nullptr; }

const TargetInfo &getTarget() const { return Target; }
llvm::LLVMContext &getLLVMContext() { return CGM.getLLVMContext(); }
const TargetCodeGenInfo &getTargetHooks() const {
return CGM.getTargetCodeGenInfo();
}





typedef void Destroyer(CodeGenFunction &CGF, Address addr, QualType ty);

void pushIrregularPartialArrayCleanup(llvm::Value *arrayBegin,
Address arrayEndPointer,
QualType elementType,
CharUnits elementAlignment,
Destroyer *destroyer);
void pushRegularPartialArrayCleanup(llvm::Value *arrayBegin,
llvm::Value *arrayEnd,
QualType elementType,
CharUnits elementAlignment,
Destroyer *destroyer);

void pushDestroy(QualType::DestructionKind dtorKind,
Address addr, QualType type);
void pushEHDestroy(QualType::DestructionKind dtorKind,
Address addr, QualType type);
void pushDestroy(CleanupKind kind, Address addr, QualType type,
Destroyer *destroyer, bool useEHCleanupForArray);
void pushLifetimeExtendedDestroy(CleanupKind kind, Address addr,
QualType type, Destroyer *destroyer,
bool useEHCleanupForArray);
void pushCallObjectDeleteCleanup(const FunctionDecl *OperatorDelete,
llvm::Value *CompletePtr,
QualType ElementType);
void pushStackRestore(CleanupKind kind, Address SPMem);
void emitDestroy(Address addr, QualType type, Destroyer *destroyer,
bool useEHCleanupForArray);
llvm::Function *generateDestroyHelper(Address addr, QualType type,
Destroyer *destroyer,
bool useEHCleanupForArray,
const VarDecl *VD);
void emitArrayDestroy(llvm::Value *begin, llvm::Value *end,
QualType elementType, CharUnits elementAlign,
Destroyer *destroyer,
bool checkZeroLength, bool useEHCleanup);

Destroyer *getDestroyer(QualType::DestructionKind destructionKind);



bool needsEHCleanup(QualType::DestructionKind kind) {
switch (kind) {
case QualType::DK_none:
return false;
case QualType::DK_cxx_destructor:
case QualType::DK_objc_weak_lifetime:
case QualType::DK_nontrivial_c_struct:
return getLangOpts().Exceptions;
case QualType::DK_objc_strong_lifetime:
return getLangOpts().Exceptions &&
CGM.getCodeGenOpts().ObjCAutoRefCountExceptions;
}
llvm_unreachable("bad destruction kind");
}

CleanupKind getCleanupKind(QualType::DestructionKind kind) {
return (needsEHCleanup(kind) ? NormalAndEHCleanup : NormalCleanup);
}





void GenerateObjCMethod(const ObjCMethodDecl *OMD);

void StartObjCMethod(const ObjCMethodDecl *MD, const ObjCContainerDecl *CD);


void GenerateObjCGetter(ObjCImplementationDecl *IMP,
const ObjCPropertyImplDecl *PID);
void generateObjCGetterBody(const ObjCImplementationDecl *classImpl,
const ObjCPropertyImplDecl *propImpl,
const ObjCMethodDecl *GetterMothodDecl,
llvm::Constant *AtomicHelperFn);

void GenerateObjCCtorDtorMethod(ObjCImplementationDecl *IMP,
ObjCMethodDecl *MD, bool ctor);



void GenerateObjCSetter(ObjCImplementationDecl *IMP,
const ObjCPropertyImplDecl *PID);
void generateObjCSetterBody(const ObjCImplementationDecl *classImpl,
const ObjCPropertyImplDecl *propImpl,
llvm::Constant *AtomicHelperFn);









llvm::Value *EmitBlockLiteral(const BlockExpr *);

llvm::Function *GenerateBlockFunction(GlobalDecl GD,
const CGBlockInfo &Info,
const DeclMapTy &ldm,
bool IsLambdaConversionToBlock,
bool BuildGlobalBlock);


static bool cxxDestructorCanThrow(QualType T);

llvm::Constant *GenerateCopyHelperFunction(const CGBlockInfo &blockInfo);
llvm::Constant *GenerateDestroyHelperFunction(const CGBlockInfo &blockInfo);
llvm::Constant *GenerateObjCAtomicSetterCopyHelperFunction(
const ObjCPropertyImplDecl *PID);
llvm::Constant *GenerateObjCAtomicGetterCopyHelperFunction(
const ObjCPropertyImplDecl *PID);
llvm::Value *EmitBlockCopyAndAutorelease(llvm::Value *Block, QualType Ty);

void BuildBlockRelease(llvm::Value *DeclPtr, BlockFieldFlags flags,
bool CanThrow);

class AutoVarEmission;

void emitByrefStructureInit(const AutoVarEmission &emission);

















void enterByrefCleanup(CleanupKind Kind, Address Addr, BlockFieldFlags Flags,
bool LoadBlockVarAddr, bool CanThrow);

void setBlockContextParameter(const ImplicitParamDecl *D, unsigned argNum,
llvm::Value *ptr);

Address LoadBlockStruct();
Address GetAddrOfBlockDecl(const VarDecl *var);



Address emitBlockByrefAddress(Address baseAddr, const VarDecl *V,
bool followForward = true);
Address emitBlockByrefAddress(Address baseAddr,
const BlockByrefInfo &info,
bool followForward,
const llvm::Twine &name);

const BlockByrefInfo &getBlockByrefInfo(const VarDecl *var);

QualType BuildFunctionArgList(GlobalDecl GD, FunctionArgList &Args);

void GenerateCode(GlobalDecl GD, llvm::Function *Fn,
const CGFunctionInfo &FnInfo);



void markAsIgnoreThreadCheckingAtRuntime(llvm::Function *Fn);




void StartFunction(GlobalDecl GD,
QualType RetTy,
llvm::Function *Fn,
const CGFunctionInfo &FnInfo,
const FunctionArgList &Args,
SourceLocation Loc = SourceLocation(),
SourceLocation StartLoc = SourceLocation());

static bool IsConstructorDelegationValid(const CXXConstructorDecl *Ctor);

void EmitConstructorBody(FunctionArgList &Args);
void EmitDestructorBody(FunctionArgList &Args);
void emitImplicitAssignmentOperatorBody(FunctionArgList &Args);
void EmitFunctionBody(const Stmt *Body);
void EmitBlockWithFallThrough(llvm::BasicBlock *BB, const Stmt *S);

void EmitForwardingCallToLambda(const CXXMethodDecl *LambdaCallOperator,
CallArgList &CallArgs);
void EmitLambdaBlockInvokeBody();
void EmitLambdaDelegatingInvokeBody(const CXXMethodDecl *MD);
void EmitLambdaStaticInvokeBody(const CXXMethodDecl *MD);
void EmitLambdaVLACapture(const VariableArrayType *VAT, LValue LV) {
EmitStoreThroughLValue(RValue::get(VLASizeMap[VAT->getSizeExpr()]), LV);
}
void EmitAsanPrologueOrEpilogue(bool Prologue);





llvm::DebugLoc EmitReturnBlock();



void FinishFunction(SourceLocation EndLoc=SourceLocation());

void StartThunk(llvm::Function *Fn, GlobalDecl GD,
const CGFunctionInfo &FnInfo, bool IsUnprototyped);

void EmitCallAndReturnForThunk(llvm::FunctionCallee Callee,
const ThunkInfo *Thunk, bool IsUnprototyped);

void FinishThunk();


void EmitMustTailThunk(GlobalDecl GD, llvm::Value *AdjustedThisPtr,
llvm::FunctionCallee Callee);


void generateThunk(llvm::Function *Fn, const CGFunctionInfo &FnInfo,
GlobalDecl GD, const ThunkInfo &Thunk,
bool IsUnprototyped);

llvm::Function *GenerateVarArgsThunk(llvm::Function *Fn,
const CGFunctionInfo &FnInfo,
GlobalDecl GD, const ThunkInfo &Thunk);

void EmitCtorPrologue(const CXXConstructorDecl *CD, CXXCtorType Type,
FunctionArgList &Args);

void EmitInitializerForField(FieldDecl *Field, LValue LHS, Expr *Init);


struct VPtr {
BaseSubobject Base;
const CXXRecordDecl *NearestVBase;
CharUnits OffsetFromNearestVBase;
const CXXRecordDecl *VTableClass;
};


void InitializeVTablePointer(const VPtr &vptr);

typedef llvm::SmallVector<VPtr, 4> VPtrsVector;

typedef llvm::SmallPtrSet<const CXXRecordDecl *, 4> VisitedVirtualBasesSetTy;
VPtrsVector getVTablePointers(const CXXRecordDecl *VTableClass);

void getVTablePointers(BaseSubobject Base, const CXXRecordDecl *NearestVBase,
CharUnits OffsetFromNearestVBase,
bool BaseIsNonVirtualPrimaryBase,
const CXXRecordDecl *VTableClass,
VisitedVirtualBasesSetTy &VBases, VPtrsVector &vptrs);

void InitializeVTablePointers(const CXXRecordDecl *ClassDecl);



llvm::Value *GetVTablePtr(Address This, llvm::Type *VTableTy,
const CXXRecordDecl *VTableClass);

enum CFITypeCheckKind {
CFITCK_VCall,
CFITCK_NVCall,
CFITCK_DerivedCast,
CFITCK_UnrelatedCast,
CFITCK_ICall,
CFITCK_NVMFCall,
CFITCK_VMFCall,
};




void EmitVTablePtrCheckForCast(QualType T, llvm::Value *Derived,
bool MayBeNull, CFITypeCheckKind TCK,
SourceLocation Loc);



void EmitVTablePtrCheckForCall(const CXXRecordDecl *RD, llvm::Value *VTable,
CFITypeCheckKind TCK, SourceLocation Loc);



void EmitVTablePtrCheck(const CXXRecordDecl *RD, llvm::Value *VTable,
CFITypeCheckKind TCK, SourceLocation Loc);




void EmitTypeMetadataCodeForVCall(const CXXRecordDecl *RD,
llvm::Value *VTable, SourceLocation Loc);




bool ShouldEmitVTableTypeCheckedLoad(const CXXRecordDecl *RD);


llvm::Value *EmitVTableTypeCheckedLoad(const CXXRecordDecl *RD, llvm::Value *VTable,
uint64_t VTableByteOffset);





void EnterDtorCleanups(const CXXDestructorDecl *Dtor, CXXDtorType Type);



bool ShouldInstrumentFunction();



bool ShouldXRayInstrumentFunction() const;



bool AlwaysEmitXRayCustomEvents() const;



bool AlwaysEmitXRayTypedEvents() const;


llvm::Constant *EncodeAddrForUseInPrologue(llvm::Function *F,
llvm::Constant *Addr);



llvm::Value *DecodeAddrUsedInPrologue(llvm::Value *F,
llvm::Value *EncodedAddr);




void EmitFunctionProlog(const CGFunctionInfo &FI,
llvm::Function *Fn,
const FunctionArgList &Args);



void EmitFunctionEpilog(const CGFunctionInfo &FI, bool EmitRetDbgLoc,
SourceLocation EndLoc);


void EmitReturnValueCheck(llvm::Value *RV);


void EmitStartEHSpec(const Decl *D);


void EmitEndEHSpec(const Decl *D);


llvm::BasicBlock *getTerminateLandingPad();



llvm::BasicBlock *getTerminateFunclet();




llvm::BasicBlock *getTerminateHandler();

llvm::Type *ConvertTypeForMem(QualType T);
llvm::Type *ConvertType(QualType T);
llvm::Type *ConvertType(const TypeDecl *T) {
return ConvertType(getContext().getTypeDeclType(T));
}



llvm::Value *LoadObjCSelf();


QualType TypeOfSelfObject();


static TypeEvaluationKind getEvaluationKind(QualType T);

static bool hasScalarEvaluationKind(QualType T) {
return getEvaluationKind(T) == TEK_Scalar;
}

static bool hasAggregateEvaluationKind(QualType T) {
return getEvaluationKind(T) == TEK_Aggregate;
}


llvm::BasicBlock *createBasicBlock(const Twine &name = "",
llvm::Function *parent = nullptr,
llvm::BasicBlock *before = nullptr) {
return llvm::BasicBlock::Create(getLLVMContext(), name, parent, before);
}



JumpDest getJumpDestForLabel(const LabelDecl *S);




void SimplifyForwardingBlocks(llvm::BasicBlock *BB);









void EmitBlock(llvm::BasicBlock *BB, bool IsFinished=false);



void EmitBlockAfterUses(llvm::BasicBlock *BB);









void EmitBranch(llvm::BasicBlock *Block);



bool HaveInsertPoint() const {
return Builder.GetInsertBlock() != nullptr;
}





void EnsureInsertPoint() {
if (!HaveInsertPoint())
EmitBlock(createBasicBlock());
}



void ErrorUnsupported(const Stmt *S, const char *Type);





LValue MakeAddrLValue(Address Addr, QualType T,
AlignmentSource Source = AlignmentSource::Type) {
return LValue::MakeAddr(Addr, T, getContext(), LValueBaseInfo(Source),
CGM.getTBAAAccessInfo(T));
}

LValue MakeAddrLValue(Address Addr, QualType T, LValueBaseInfo BaseInfo,
TBAAAccessInfo TBAAInfo) {
return LValue::MakeAddr(Addr, T, getContext(), BaseInfo, TBAAInfo);
}

LValue MakeAddrLValue(llvm::Value *V, QualType T, CharUnits Alignment,
AlignmentSource Source = AlignmentSource::Type) {
return LValue::MakeAddr(Address(V, Alignment), T, getContext(),
LValueBaseInfo(Source), CGM.getTBAAAccessInfo(T));
}

LValue MakeAddrLValue(llvm::Value *V, QualType T, CharUnits Alignment,
LValueBaseInfo BaseInfo, TBAAAccessInfo TBAAInfo) {
return LValue::MakeAddr(Address(V, Alignment), T, getContext(),
BaseInfo, TBAAInfo);
}

LValue MakeNaturalAlignPointeeAddrLValue(llvm::Value *V, QualType T);
LValue MakeNaturalAlignAddrLValue(llvm::Value *V, QualType T);

Address EmitLoadOfReference(LValue RefLVal,
LValueBaseInfo *PointeeBaseInfo = nullptr,
TBAAAccessInfo *PointeeTBAAInfo = nullptr);
LValue EmitLoadOfReferenceLValue(LValue RefLVal);
LValue EmitLoadOfReferenceLValue(Address RefAddr, QualType RefTy,
AlignmentSource Source =
AlignmentSource::Type) {
LValue RefLVal = MakeAddrLValue(RefAddr, RefTy, LValueBaseInfo(Source),
CGM.getTBAAAccessInfo(RefTy));
return EmitLoadOfReferenceLValue(RefLVal);
}

Address EmitLoadOfPointer(Address Ptr, const PointerType *PtrTy,
LValueBaseInfo *BaseInfo = nullptr,
TBAAAccessInfo *TBAAInfo = nullptr);
LValue EmitLoadOfPointerLValue(Address Ptr, const PointerType *PtrTy);



























llvm::AllocaInst *CreateTempAlloca(llvm::Type *Ty, const Twine &Name = "tmp",
llvm::Value *ArraySize = nullptr);
Address CreateTempAlloca(llvm::Type *Ty, CharUnits align,
const Twine &Name = "tmp",
llvm::Value *ArraySize = nullptr,
Address *Alloca = nullptr);
Address CreateTempAllocaWithoutCast(llvm::Type *Ty, CharUnits align,
const Twine &Name = "tmp",
llvm::Value *ArraySize = nullptr);











Address CreateDefaultAlignTempAlloca(llvm::Type *Ty,
const Twine &Name = "tmp");








void InitTempAlloca(Address Alloca, llvm::Value *Value);









Address CreateIRTemp(QualType T, const Twine &Name = "tmp");




Address CreateMemTemp(QualType T, const Twine &Name = "tmp",
Address *Alloca = nullptr);
Address CreateMemTemp(QualType T, CharUnits Align, const Twine &Name = "tmp",
Address *Alloca = nullptr);



Address CreateMemTempWithoutCast(QualType T, const Twine &Name = "tmp");
Address CreateMemTempWithoutCast(QualType T, CharUnits Align,
const Twine &Name = "tmp");



AggValueSlot CreateAggTemp(QualType T, const Twine &Name = "tmp",
Address *Alloca = nullptr) {
return AggValueSlot::forAddr(CreateMemTemp(T, Name, Alloca),
T.getQualifiers(),
AggValueSlot::IsNotDestructed,
AggValueSlot::DoesNotNeedGCBarriers,
AggValueSlot::IsNotAliased,
AggValueSlot::DoesNotOverlap);
}


llvm::Value *EmitCastToVoidPtr(llvm::Value *value);



llvm::Value *EvaluateExprAsBool(const Expr *E);


void EmitIgnoredExpr(const Expr *E);







RValue EmitAnyExpr(const Expr *E,
AggValueSlot aggSlot = AggValueSlot::ignored(),
bool ignoreResult = false);



Address EmitVAListRef(const Expr *E);




Address EmitMSVAListRef(const Expr *E);



RValue EmitAnyExprToTemp(const Expr *E);



void EmitAnyExprToMem(const Expr *E, Address Location,
Qualifiers Quals, bool IsInitializer);

void EmitAnyExprToExn(const Expr *E, Address Addr);



void EmitExprAsInit(const Expr *init, const ValueDecl *D, LValue lvalue,
bool capturedByInit);



bool hasVolatileMember(QualType T) {
if (const RecordType *RT = T->getAs<RecordType>()) {
const RecordDecl *RD = cast<RecordDecl>(RT->getDecl());
return RD->hasVolatileMember();
}
return false;
}


AggValueSlot::Overlap_t getOverlapForReturnValue() {



return AggValueSlot::DoesNotOverlap;
}


AggValueSlot::Overlap_t getOverlapForFieldInit(const FieldDecl *FD);



AggValueSlot::Overlap_t getOverlapForBaseInit(const CXXRecordDecl *RD,
const CXXRecordDecl *BaseRD,
bool IsVirtual);


void EmitAggregateAssign(LValue Dest, LValue Src, QualType EltTy) {
bool IsVolatile = hasVolatileMember(EltTy);
EmitAggregateCopy(Dest, Src, EltTy, AggValueSlot::MayOverlap, IsVolatile);
}

void EmitAggregateCopyCtor(LValue Dest, LValue Src,
AggValueSlot::Overlap_t MayOverlap) {
EmitAggregateCopy(Dest, Src, Src.getType(), MayOverlap);
}








void EmitAggregateCopy(LValue Dest, LValue Src, QualType EltTy,
AggValueSlot::Overlap_t MayOverlap,
bool isVolatile = false);


Address GetAddrOfLocalVar(const VarDecl *VD) {
auto it = LocalDeclMap.find(VD);
assert(it != LocalDeclMap.end() &&
"Invalid argument to GetAddrOfLocalVar(), no decl!");
return it->second;
}



LValue getOrCreateOpaqueLValueMapping(const OpaqueValueExpr *e);



RValue getOrCreateOpaqueRValueMapping(const OpaqueValueExpr *e);


llvm::Value *getArrayInitIndex() { return ArrayInitIndex; }



static unsigned getAccessedFieldNo(unsigned Idx, const llvm::Constant *Elts);

llvm::BlockAddress *GetAddrOfLabel(const LabelDecl *L);
llvm::BasicBlock *GetIndirectGotoBlock();


static bool IsWrappedCXXThis(const Expr *E);




void EmitNullInitialization(Address DestPtr, QualType Ty);







llvm::Value *EmitVAStartEnd(llvm::Value *ArgValue, bool IsStart);









Address EmitVAArg(VAArgExpr *VE, Address &VAListAddr);



llvm::Value *emitArrayLength(const ArrayType *arrayType,
QualType &baseType,
Address &addr);





void EmitVariablyModifiedType(QualType Ty);

struct VlaSizePair {
llvm::Value *NumElts;
QualType Type;

VlaSizePair(llvm::Value *NE, QualType T) : NumElts(NE), Type(T) {}
};



VlaSizePair getVLAElements1D(const VariableArrayType *vla);
VlaSizePair getVLAElements1D(QualType vla);





VlaSizePair getVLASize(const VariableArrayType *vla);
VlaSizePair getVLASize(QualType vla);



llvm::Value *LoadCXXThis() {
assert(CXXThisValue && "no 'this' value for this function");
return CXXThisValue;
}
Address LoadCXXThisAddress();





llvm::Value *LoadCXXVTT() {
assert(CXXStructorImplicitParamValue && "no VTT value for this function");
return CXXStructorImplicitParamValue;
}



Address
GetAddressOfDirectBaseInCompleteClass(Address Value,
const CXXRecordDecl *Derived,
const CXXRecordDecl *Base,
bool BaseIsVirtual);

static bool ShouldNullCheckClassCastValue(const CastExpr *Cast);



Address GetAddressOfBaseClass(Address Value,
const CXXRecordDecl *Derived,
CastExpr::path_const_iterator PathBegin,
CastExpr::path_const_iterator PathEnd,
bool NullCheckValue, SourceLocation Loc);

Address GetAddressOfDerivedClass(Address Value,
const CXXRecordDecl *Derived,
CastExpr::path_const_iterator PathBegin,
CastExpr::path_const_iterator PathEnd,
bool NullCheckValue);





llvm::Value *GetVTTParameter(GlobalDecl GD, bool ForVirtualBase,
bool Delegating);

void EmitDelegateCXXConstructorCall(const CXXConstructorDecl *Ctor,
CXXCtorType CtorType,
const FunctionArgList &Args,
SourceLocation Loc);




void EmitDelegatingCXXConstructorCall(const CXXConstructorDecl *Ctor,
const FunctionArgList &Args);





void EmitInlinedInheritingCXXConstructorCall(const CXXConstructorDecl *Ctor,
CXXCtorType CtorType,
bool ForVirtualBase,
bool Delegating,
CallArgList &Args);




void EmitInheritedCXXConstructorCall(const CXXConstructorDecl *D,
bool ForVirtualBase, Address This,
bool InheritedFromVBase,
const CXXInheritedCtorInitExpr *E);

void EmitCXXConstructorCall(const CXXConstructorDecl *D, CXXCtorType Type,
bool ForVirtualBase, bool Delegating,
AggValueSlot ThisAVS, const CXXConstructExpr *E);

void EmitCXXConstructorCall(const CXXConstructorDecl *D, CXXCtorType Type,
bool ForVirtualBase, bool Delegating,
Address This, CallArgList &Args,
AggValueSlot::Overlap_t Overlap,
SourceLocation Loc, bool NewPointerIsChecked);



void EmitVTableAssumptionLoads(const CXXRecordDecl *ClassDecl, Address This);


void EmitVTableAssumptionLoad(const VPtr &vptr, Address This);

void EmitSynthesizedCXXCopyCtorCall(const CXXConstructorDecl *D,
Address This, Address Src,
const CXXConstructExpr *E);

void EmitCXXAggrConstructorCall(const CXXConstructorDecl *D,
const ArrayType *ArrayTy,
Address ArrayPtr,
const CXXConstructExpr *E,
bool NewPointerIsChecked,
bool ZeroInitialization = false);

void EmitCXXAggrConstructorCall(const CXXConstructorDecl *D,
llvm::Value *NumElements,
Address ArrayPtr,
const CXXConstructExpr *E,
bool NewPointerIsChecked,
bool ZeroInitialization = false);

static Destroyer destroyCXXObject;

void EmitCXXDestructorCall(const CXXDestructorDecl *D, CXXDtorType Type,
bool ForVirtualBase, bool Delegating, Address This,
QualType ThisTy);

void EmitNewArrayInitializer(const CXXNewExpr *E, QualType elementType,
llvm::Type *ElementTy, Address NewPtr,
llvm::Value *NumElements,
llvm::Value *AllocSizeWithoutCookie);

void EmitCXXTemporary(const CXXTemporary *Temporary, QualType TempType,
Address Ptr);

void EmitSehCppScopeBegin();
void EmitSehCppScopeEnd();
void EmitSehTryScopeBegin();
void EmitSehTryScopeEnd();

llvm::Value *EmitLifetimeStart(llvm::TypeSize Size, llvm::Value *Addr);
void EmitLifetimeEnd(llvm::Value *Size, llvm::Value *Addr);

llvm::Value *EmitCXXNewExpr(const CXXNewExpr *E);
void EmitCXXDeleteExpr(const CXXDeleteExpr *E);

void EmitDeleteCall(const FunctionDecl *DeleteFD, llvm::Value *Ptr,
QualType DeleteTy, llvm::Value *NumElements = nullptr,
CharUnits CookieSize = CharUnits());

RValue EmitBuiltinNewDeleteCall(const FunctionProtoType *Type,
const CallExpr *TheCallExpr, bool IsDelete);

llvm::Value *EmitCXXTypeidExpr(const CXXTypeidExpr *E);
llvm::Value *EmitDynamicCast(Address V, const CXXDynamicCastExpr *DCE);
Address EmitCXXUuidofExpr(const CXXUuidofExpr *E);




enum TypeCheckKind {

TCK_Load,

TCK_Store,



TCK_ReferenceBinding,


TCK_MemberAccess,


TCK_MemberCall,

TCK_ConstructorCall,


TCK_DowncastPointer,


TCK_DowncastReference,


TCK_Upcast,


TCK_UpcastToVirtualBase,

TCK_NonnullAssign,


TCK_DynamicOperation
};


static bool isNullPointerAllowed(TypeCheckKind TCK);


static bool isVptrCheckRequired(TypeCheckKind TCK, QualType Ty);



bool sanitizePerformTypeCheck() const;




void EmitTypeCheck(TypeCheckKind TCK, SourceLocation Loc, llvm::Value *V,
QualType Type, CharUnits Alignment = CharUnits::Zero(),
SanitizerSet SkippedChecks = SanitizerSet(),
llvm::Value *ArraySize = nullptr);




void EmitBoundsCheck(const Expr *E, const Expr *Base, llvm::Value *Index,
QualType IndexType, bool Accessed);

llvm::Value *EmitScalarPrePostIncDec(const UnaryOperator *E, LValue LV,
bool isInc, bool isPre);
ComplexPairTy EmitComplexPrePostIncDec(const UnaryOperator *E, LValue LV,
bool isInc, bool isPre);


llvm::DebugLoc SourceLocToDebugLoc(SourceLocation Location);


unsigned getDebugInfoFIndex(const RecordDecl *Rec, unsigned FieldIndex);









void EmitDecl(const Decl &D);




void EmitVarDecl(const VarDecl &D);

void EmitScalarInit(const Expr *init, const ValueDecl *D, LValue lvalue,
bool capturedByInit);

typedef void SpecialInitFn(CodeGenFunction &Init, const VarDecl &D,
llvm::Value *Address);



bool isTrivialInitializer(const Expr *Init);




void EmitAutoVarDecl(const VarDecl &D);

class AutoVarEmission {
friend class CodeGenFunction;

const VarDecl *Variable;





Address Addr;

llvm::Value *NRVOFlag;



bool IsEscapingByRef;



bool IsConstantAggregate;


llvm::Value *SizeForLifetimeMarkers;



Address AllocaAddr;

struct Invalid {};
AutoVarEmission(Invalid)
: Variable(nullptr), Addr(Address::invalid()),
AllocaAddr(Address::invalid()) {}

AutoVarEmission(const VarDecl &variable)
: Variable(&variable), Addr(Address::invalid()), NRVOFlag(nullptr),
IsEscapingByRef(false), IsConstantAggregate(false),
SizeForLifetimeMarkers(nullptr), AllocaAddr(Address::invalid()) {}

bool wasEmittedAsGlobal() const { return !Addr.isValid(); }

public:
static AutoVarEmission invalid() { return AutoVarEmission(Invalid()); }

bool useLifetimeMarkers() const {
return SizeForLifetimeMarkers != nullptr;
}
llvm::Value *getSizeForLifetimeMarkers() const {
assert(useLifetimeMarkers());
return SizeForLifetimeMarkers;
}




Address getAllocatedAddress() const {
return Addr;
}


Address getOriginalAllocatedAddress() const { return AllocaAddr; }




Address getObjectAddress(CodeGenFunction &CGF) const {
if (!IsEscapingByRef) return Addr;

return CGF.emitBlockByrefAddress(Addr, Variable, false);
}
};
AutoVarEmission EmitAutoVarAlloca(const VarDecl &var);
void EmitAutoVarInit(const AutoVarEmission &emission);
void EmitAutoVarCleanups(const AutoVarEmission &emission);
void emitAutoVarTypeCleanup(const AutoVarEmission &emission,
QualType::DestructionKind dtorKind);






void EmitAndRegisterVariableArrayDimensions(CGDebugInfo *DI,
const VarDecl &D,
bool EmitDebugInfo);

void EmitStaticVarDecl(const VarDecl &D,
llvm::GlobalValue::LinkageTypes Linkage);

class ParamValue {
llvm::Value *Value;
unsigned Alignment;
ParamValue(llvm::Value *V, unsigned A) : Value(V), Alignment(A) {}
public:
static ParamValue forDirect(llvm::Value *value) {
return ParamValue(value, 0);
}
static ParamValue forIndirect(Address addr) {
assert(!addr.getAlignment().isZero());
return ParamValue(addr.getPointer(), addr.getAlignment().getQuantity());
}

bool isIndirect() const { return Alignment != 0; }
llvm::Value *getAnyValue() const { return Value; }

llvm::Value *getDirectValue() const {
assert(!isIndirect());
return Value;
}

Address getIndirectAddress() const {
assert(isIndirect());
return Address(Value, CharUnits::fromQuantity(Alignment));
}
};


void EmitParmDecl(const VarDecl &D, ParamValue Arg, unsigned ArgNo);










PeepholeProtection protectFromPeepholes(RValue rvalue);
void unprotectFromPeepholes(PeepholeProtection protection);

void emitAlignmentAssumptionCheck(llvm::Value *Ptr, QualType Ty,
SourceLocation Loc,
SourceLocation AssumptionLoc,
llvm::Value *Alignment,
llvm::Value *OffsetValue,
llvm::Value *TheCheck,
llvm::Instruction *Assumption);

void emitAlignmentAssumption(llvm::Value *PtrValue, QualType Ty,
SourceLocation Loc, SourceLocation AssumptionLoc,
llvm::Value *Alignment,
llvm::Value *OffsetValue = nullptr);

void emitAlignmentAssumption(llvm::Value *PtrValue, const Expr *E,
SourceLocation AssumptionLoc,
llvm::Value *Alignment,
llvm::Value *OffsetValue = nullptr);






void EmitStopPoint(const Stmt *S);







void EmitStmt(const Stmt *S, ArrayRef<const Attr *> Attrs = None);







bool EmitSimpleStmt(const Stmt *S, ArrayRef<const Attr *> Attrs);

Address EmitCompoundStmt(const CompoundStmt &S, bool GetLast = false,
AggValueSlot AVS = AggValueSlot::ignored());
Address EmitCompoundStmtWithoutScope(const CompoundStmt &S,
bool GetLast = false,
AggValueSlot AVS =
AggValueSlot::ignored());



void EmitLabel(const LabelDecl *D);

void EmitLabelStmt(const LabelStmt &S);
void EmitAttributedStmt(const AttributedStmt &S);
void EmitGotoStmt(const GotoStmt &S);
void EmitIndirectGotoStmt(const IndirectGotoStmt &S);
void EmitIfStmt(const IfStmt &S);

void EmitWhileStmt(const WhileStmt &S,
ArrayRef<const Attr *> Attrs = None);
void EmitDoStmt(const DoStmt &S, ArrayRef<const Attr *> Attrs = None);
void EmitForStmt(const ForStmt &S,
ArrayRef<const Attr *> Attrs = None);
void EmitReturnStmt(const ReturnStmt &S);
void EmitDeclStmt(const DeclStmt &S);
void EmitBreakStmt(const BreakStmt &S);
void EmitContinueStmt(const ContinueStmt &S);
void EmitSwitchStmt(const SwitchStmt &S);
void EmitDefaultStmt(const DefaultStmt &S, ArrayRef<const Attr *> Attrs);
void EmitCaseStmt(const CaseStmt &S, ArrayRef<const Attr *> Attrs);
void EmitCaseStmtRange(const CaseStmt &S, ArrayRef<const Attr *> Attrs);
void EmitAsmStmt(const AsmStmt &S);

void EmitObjCForCollectionStmt(const ObjCForCollectionStmt &S);
void EmitObjCAtTryStmt(const ObjCAtTryStmt &S);
void EmitObjCAtThrowStmt(const ObjCAtThrowStmt &S);
void EmitObjCAtSynchronizedStmt(const ObjCAtSynchronizedStmt &S);
void EmitObjCAutoreleasePoolStmt(const ObjCAutoreleasePoolStmt &S);

void EmitCoroutineBody(const CoroutineBodyStmt &S);
void EmitCoreturnStmt(const CoreturnStmt &S);
RValue EmitCoawaitExpr(const CoawaitExpr &E,
AggValueSlot aggSlot = AggValueSlot::ignored(),
bool ignoreResult = false);
LValue EmitCoawaitLValue(const CoawaitExpr *E);
RValue EmitCoyieldExpr(const CoyieldExpr &E,
AggValueSlot aggSlot = AggValueSlot::ignored(),
bool ignoreResult = false);
LValue EmitCoyieldLValue(const CoyieldExpr *E);
RValue EmitCoroutineIntrinsic(const CallExpr *E, unsigned int IID);

void EnterCXXTryStmt(const CXXTryStmt &S, bool IsFnTryBlock = false);
void ExitCXXTryStmt(const CXXTryStmt &S, bool IsFnTryBlock = false);

void EmitCXXTryStmt(const CXXTryStmt &S);
void EmitSEHTryStmt(const SEHTryStmt &S);
void EmitSEHLeaveStmt(const SEHLeaveStmt &S);
void EnterSEHTryStmt(const SEHTryStmt &S);
void ExitSEHTryStmt(const SEHTryStmt &S);
void VolatilizeTryBlocks(llvm::BasicBlock *BB,
llvm::SmallPtrSet<llvm::BasicBlock *, 10> &V);

void pushSEHCleanup(CleanupKind kind,
llvm::Function *FinallyFunc);
void startOutlinedSEHHelper(CodeGenFunction &ParentCGF, bool IsFilter,
const Stmt *OutlinedStmt);

llvm::Function *GenerateSEHFilterFunction(CodeGenFunction &ParentCGF,
const SEHExceptStmt &Except);

llvm::Function *GenerateSEHFinallyFunction(CodeGenFunction &ParentCGF,
const SEHFinallyStmt &Finally);

void EmitSEHExceptionCodeSave(CodeGenFunction &ParentCGF,
llvm::Value *ParentFP,
llvm::Value *EntryEBP);
llvm::Value *EmitSEHExceptionCode();
llvm::Value *EmitSEHExceptionInfo();
llvm::Value *EmitSEHAbnormalTermination();


void EmitSimpleOMPExecutableDirective(const OMPExecutableDirective &D);




void EmitCapturedLocals(CodeGenFunction &ParentCGF, const Stmt *OutlinedStmt,
bool IsFilter);






Address recoverAddrOfEscapedLocal(CodeGenFunction &ParentCGF,
Address ParentVar,
llvm::Value *ParentFP);

void EmitCXXForRangeStmt(const CXXForRangeStmt &S,
ArrayRef<const Attr *> Attrs = None);


class OMPCancelStackRAII {
CodeGenFunction &CGF;

public:
OMPCancelStackRAII(CodeGenFunction &CGF, OpenMPDirectiveKind Kind,
bool HasCancel)
: CGF(CGF) {
CGF.OMPCancelStack.enter(CGF, Kind, HasCancel);
}
~OMPCancelStackRAII() { CGF.OMPCancelStack.exit(CGF); }
};


llvm::Value *getTypeSize(QualType Ty);
LValue InitCapturedStruct(const CapturedStmt &S);
llvm::Function *EmitCapturedStmt(const CapturedStmt &S, CapturedRegionKind K);
llvm::Function *GenerateCapturedStmtFunction(const CapturedStmt &S);
Address GenerateCapturedStmtArgument(const CapturedStmt &S);
llvm::Function *GenerateOpenMPCapturedStmtFunction(const CapturedStmt &S,
SourceLocation Loc);
void GenerateOpenMPCapturedVars(const CapturedStmt &S,
SmallVectorImpl<llvm::Value *> &CapturedVars);
void emitOMPSimpleStore(LValue LVal, RValue RVal, QualType RValTy,
SourceLocation Loc);









void EmitOMPAggregateAssign(
Address DestAddr, Address SrcAddr, QualType OriginalType,
const llvm::function_ref<void(Address, Address)> CopyGen);











void EmitOMPCopy(QualType OriginalType,
Address DestAddr, Address SrcAddr,
const VarDecl *DestVD, const VarDecl *SrcVD,
const Expr *Copy);













std::pair<bool, RValue> EmitOMPAtomicSimpleUpdateExpr(
LValue X, RValue E, BinaryOperatorKind BO, bool IsXLHSInRHSPart,
llvm::AtomicOrdering AO, SourceLocation Loc,
const llvm::function_ref<RValue(RValue)> CommonGen);
bool EmitOMPFirstprivateClause(const OMPExecutableDirective &D,
OMPPrivateScope &PrivateScope);
void EmitOMPPrivateClause(const OMPExecutableDirective &D,
OMPPrivateScope &PrivateScope);
void EmitOMPUseDevicePtrClause(
const OMPUseDevicePtrClause &C, OMPPrivateScope &PrivateScope,
const llvm::DenseMap<const ValueDecl *, Address> &CaptureDeviceAddrMap);
void EmitOMPUseDeviceAddrClause(
const OMPUseDeviceAddrClause &C, OMPPrivateScope &PrivateScope,
const llvm::DenseMap<const ValueDecl *, Address> &CaptureDeviceAddrMap);











bool EmitOMPCopyinClause(const OMPExecutableDirective &D);











bool EmitOMPLastprivateClauseInit(const OMPExecutableDirective &D,
OMPPrivateScope &PrivateScope);







void EmitOMPLastprivateClauseFinal(const OMPExecutableDirective &D,
bool NoFinals,
llvm::Value *IsLastIterCond = nullptr);

void EmitOMPLinearClause(const OMPLoopDirective &D,
CodeGenFunction::OMPPrivateScope &PrivateScope);



void EmitOMPLinearClauseFinal(
const OMPLoopDirective &D,
const llvm::function_ref<llvm::Value *(CodeGenFunction &)> CondGen);







void EmitOMPReductionClauseInit(const OMPExecutableDirective &D,
OMPPrivateScope &PrivateScope,
bool ForInscan = false);





void EmitOMPReductionClauseFinal(const OMPExecutableDirective &D,
const OpenMPDirectiveKind ReductionKind);






bool EmitOMPLinearClauseInit(const OMPLoopDirective &D);

typedef const llvm::function_ref<void(CodeGenFunction & ,
llvm::Function * ,
const OMPTaskDataTy & )>
TaskGenTy;
void EmitOMPTaskBasedDirective(const OMPExecutableDirective &S,
const OpenMPDirectiveKind CapturedRegion,
const RegionCodeGenTy &BodyGen,
const TaskGenTy &TaskGen, OMPTaskDataTy &Data);
struct OMPTargetDataInfo {
Address BasePointersArray = Address::invalid();
Address PointersArray = Address::invalid();
Address SizesArray = Address::invalid();
Address MappersArray = Address::invalid();
unsigned NumberOfTargetItems = 0;
explicit OMPTargetDataInfo() = default;
OMPTargetDataInfo(Address BasePointersArray, Address PointersArray,
Address SizesArray, Address MappersArray,
unsigned NumberOfTargetItems)
: BasePointersArray(BasePointersArray), PointersArray(PointersArray),
SizesArray(SizesArray), MappersArray(MappersArray),
NumberOfTargetItems(NumberOfTargetItems) {}
};
void EmitOMPTargetTaskBasedDirective(const OMPExecutableDirective &S,
const RegionCodeGenTy &BodyGen,
OMPTargetDataInfo &InputInfo);

void EmitOMPParallelDirective(const OMPParallelDirective &S);
void EmitOMPSimdDirective(const OMPSimdDirective &S);
void EmitOMPTileDirective(const OMPTileDirective &S);
void EmitOMPUnrollDirective(const OMPUnrollDirective &S);
void EmitOMPForDirective(const OMPForDirective &S);
void EmitOMPForSimdDirective(const OMPForSimdDirective &S);
void EmitOMPSectionsDirective(const OMPSectionsDirective &S);
void EmitOMPSectionDirective(const OMPSectionDirective &S);
void EmitOMPSingleDirective(const OMPSingleDirective &S);
void EmitOMPMasterDirective(const OMPMasterDirective &S);
void EmitOMPMaskedDirective(const OMPMaskedDirective &S);
void EmitOMPCriticalDirective(const OMPCriticalDirective &S);
void EmitOMPParallelForDirective(const OMPParallelForDirective &S);
void EmitOMPParallelForSimdDirective(const OMPParallelForSimdDirective &S);
void EmitOMPParallelSectionsDirective(const OMPParallelSectionsDirective &S);
void EmitOMPParallelMasterDirective(const OMPParallelMasterDirective &S);
void EmitOMPTaskDirective(const OMPTaskDirective &S);
void EmitOMPTaskyieldDirective(const OMPTaskyieldDirective &S);
void EmitOMPBarrierDirective(const OMPBarrierDirective &S);
void EmitOMPTaskwaitDirective(const OMPTaskwaitDirective &S);
void EmitOMPTaskgroupDirective(const OMPTaskgroupDirective &S);
void EmitOMPFlushDirective(const OMPFlushDirective &S);
void EmitOMPDepobjDirective(const OMPDepobjDirective &S);
void EmitOMPScanDirective(const OMPScanDirective &S);
void EmitOMPOrderedDirective(const OMPOrderedDirective &S);
void EmitOMPAtomicDirective(const OMPAtomicDirective &S);
void EmitOMPTargetDirective(const OMPTargetDirective &S);
void EmitOMPTargetDataDirective(const OMPTargetDataDirective &S);
void EmitOMPTargetEnterDataDirective(const OMPTargetEnterDataDirective &S);
void EmitOMPTargetExitDataDirective(const OMPTargetExitDataDirective &S);
void EmitOMPTargetUpdateDirective(const OMPTargetUpdateDirective &S);
void EmitOMPTargetParallelDirective(const OMPTargetParallelDirective &S);
void
EmitOMPTargetParallelForDirective(const OMPTargetParallelForDirective &S);
void EmitOMPTeamsDirective(const OMPTeamsDirective &S);
void
EmitOMPCancellationPointDirective(const OMPCancellationPointDirective &S);
void EmitOMPCancelDirective(const OMPCancelDirective &S);
void EmitOMPTaskLoopBasedDirective(const OMPLoopDirective &S);
void EmitOMPTaskLoopDirective(const OMPTaskLoopDirective &S);
void EmitOMPTaskLoopSimdDirective(const OMPTaskLoopSimdDirective &S);
void EmitOMPMasterTaskLoopDirective(const OMPMasterTaskLoopDirective &S);
void
EmitOMPMasterTaskLoopSimdDirective(const OMPMasterTaskLoopSimdDirective &S);
void EmitOMPParallelMasterTaskLoopDirective(
const OMPParallelMasterTaskLoopDirective &S);
void EmitOMPParallelMasterTaskLoopSimdDirective(
const OMPParallelMasterTaskLoopSimdDirective &S);
void EmitOMPDistributeDirective(const OMPDistributeDirective &S);
void EmitOMPDistributeParallelForDirective(
const OMPDistributeParallelForDirective &S);
void EmitOMPDistributeParallelForSimdDirective(
const OMPDistributeParallelForSimdDirective &S);
void EmitOMPDistributeSimdDirective(const OMPDistributeSimdDirective &S);
void EmitOMPTargetParallelForSimdDirective(
const OMPTargetParallelForSimdDirective &S);
void EmitOMPTargetSimdDirective(const OMPTargetSimdDirective &S);
void EmitOMPTeamsDistributeDirective(const OMPTeamsDistributeDirective &S);
void
EmitOMPTeamsDistributeSimdDirective(const OMPTeamsDistributeSimdDirective &S);
void EmitOMPTeamsDistributeParallelForSimdDirective(
const OMPTeamsDistributeParallelForSimdDirective &S);
void EmitOMPTeamsDistributeParallelForDirective(
const OMPTeamsDistributeParallelForDirective &S);
void EmitOMPTargetTeamsDirective(const OMPTargetTeamsDirective &S);
void EmitOMPTargetTeamsDistributeDirective(
const OMPTargetTeamsDistributeDirective &S);
void EmitOMPTargetTeamsDistributeParallelForDirective(
const OMPTargetTeamsDistributeParallelForDirective &S);
void EmitOMPTargetTeamsDistributeParallelForSimdDirective(
const OMPTargetTeamsDistributeParallelForSimdDirective &S);
void EmitOMPTargetTeamsDistributeSimdDirective(
const OMPTargetTeamsDistributeSimdDirective &S);


static void EmitOMPTargetDeviceFunction(CodeGenModule &CGM,
StringRef ParentName,
const OMPTargetDirective &S);
static void
EmitOMPTargetParallelDeviceFunction(CodeGenModule &CGM, StringRef ParentName,
const OMPTargetParallelDirective &S);

static void EmitOMPTargetParallelForDeviceFunction(
CodeGenModule &CGM, StringRef ParentName,
const OMPTargetParallelForDirective &S);

static void EmitOMPTargetParallelForSimdDeviceFunction(
CodeGenModule &CGM, StringRef ParentName,
const OMPTargetParallelForSimdDirective &S);

static void
EmitOMPTargetTeamsDeviceFunction(CodeGenModule &CGM, StringRef ParentName,
const OMPTargetTeamsDirective &S);

static void EmitOMPTargetTeamsDistributeDeviceFunction(
CodeGenModule &CGM, StringRef ParentName,
const OMPTargetTeamsDistributeDirective &S);

static void EmitOMPTargetTeamsDistributeSimdDeviceFunction(
CodeGenModule &CGM, StringRef ParentName,
const OMPTargetTeamsDistributeSimdDirective &S);

static void EmitOMPTargetSimdDeviceFunction(CodeGenModule &CGM,
StringRef ParentName,
const OMPTargetSimdDirective &S);


static void EmitOMPTargetTeamsDistributeParallelForSimdDeviceFunction(
CodeGenModule &CGM, StringRef ParentName,
const OMPTargetTeamsDistributeParallelForSimdDirective &S);

static void EmitOMPTargetTeamsDistributeParallelForDeviceFunction(
CodeGenModule &CGM, StringRef ParentName,
const OMPTargetTeamsDistributeParallelForDirective &S);






llvm::CanonicalLoopInfo *EmitOMPCollapsedCanonicalLoopNest(const Stmt *S,
int Depth);


void EmitOMPCanonicalLoop(const OMPCanonicalLoop *S);











void EmitOMPInnerLoop(
const OMPExecutableDirective &S, bool RequiresCleanup,
const Expr *LoopCond, const Expr *IncExpr,
const llvm::function_ref<void(CodeGenFunction &)> BodyGen,
const llvm::function_ref<void(CodeGenFunction &)> PostIncGen);

JumpDest getOMPCancelDestination(OpenMPDirectiveKind Kind);

void EmitOMPPrivateLoopCounters(const OMPLoopDirective &S,
OMPPrivateScope &LoopScope);


void EmitOMPLoopBody(const OMPLoopDirective &D, JumpDest LoopExit);




bool EmitOMPWorksharingLoop(const OMPLoopDirective &S, Expr *EUB,
const CodeGenLoopBoundsTy &CodeGenLoopBounds,
const CodeGenDispatchBoundsTy &CGDispatchBounds);


void EmitOMPDistributeLoop(const OMPLoopDirective &S,
const CodeGenLoopTy &CodeGenLoop, Expr *IncExpr);


void EmitOMPSimdInit(const OMPLoopDirective &D);
void EmitOMPSimdFinal(
const OMPLoopDirective &D,
const llvm::function_ref<llvm::Value *(CodeGenFunction &)> CondGen);


LValue EmitOMPSharedLValue(const Expr *E);

private:

llvm::Value *EmitBlockLiteral(const CGBlockInfo &Info);


struct OMPLoopArguments {

Address LB = Address::invalid();

Address UB = Address::invalid();

Address ST = Address::invalid();

Address IL = Address::invalid();

llvm::Value *Chunk = nullptr;

Expr *EUB = nullptr;

Expr *IncExpr = nullptr;

Expr *Init = nullptr;

Expr *Cond = nullptr;

Expr *NextLB = nullptr;

Expr *NextUB = nullptr;
OMPLoopArguments() = default;
OMPLoopArguments(Address LB, Address UB, Address ST, Address IL,
llvm::Value *Chunk = nullptr, Expr *EUB = nullptr,
Expr *IncExpr = nullptr, Expr *Init = nullptr,
Expr *Cond = nullptr, Expr *NextLB = nullptr,
Expr *NextUB = nullptr)
: LB(LB), UB(UB), ST(ST), IL(IL), Chunk(Chunk), EUB(EUB),
IncExpr(IncExpr), Init(Init), Cond(Cond), NextLB(NextLB),
NextUB(NextUB) {}
};
void EmitOMPOuterLoop(bool DynamicOrOrdered, bool IsMonotonic,
const OMPLoopDirective &S, OMPPrivateScope &LoopScope,
const OMPLoopArguments &LoopArgs,
const CodeGenLoopTy &CodeGenLoop,
const CodeGenOrderedTy &CodeGenOrdered);
void EmitOMPForOuterLoop(const OpenMPScheduleTy &ScheduleKind,
bool IsMonotonic, const OMPLoopDirective &S,
OMPPrivateScope &LoopScope, bool Ordered,
const OMPLoopArguments &LoopArgs,
const CodeGenDispatchBoundsTy &CGDispatchBounds);
void EmitOMPDistributeOuterLoop(OpenMPDistScheduleClauseKind ScheduleKind,
const OMPLoopDirective &S,
OMPPrivateScope &LoopScope,
const OMPLoopArguments &LoopArgs,
const CodeGenLoopTy &CodeGenLoopContent);

void EmitSections(const OMPExecutableDirective &S);

public:






llvm::Value *EmitNonNullRValueCheck(RValue RV, QualType T);


RValue GetUndefRValue(QualType Ty);




RValue EmitUnsupportedRValue(const Expr *E,
const char *Name);



LValue EmitUnsupportedLValue(const Expr *E,
const char *Name);

















LValue EmitLValue(const Expr *E);




LValue EmitCheckedLValue(const Expr *E, TypeCheckKind TCK);

RValue convertTempToRValue(Address addr, QualType type,
SourceLocation Loc);

void EmitAtomicInit(Expr *E, LValue lvalue);

bool LValueIsSuitableForInlineAtomic(LValue Src);

RValue EmitAtomicLoad(LValue LV, SourceLocation SL,
AggValueSlot Slot = AggValueSlot::ignored());

RValue EmitAtomicLoad(LValue lvalue, SourceLocation loc,
llvm::AtomicOrdering AO, bool IsVolatile = false,
AggValueSlot slot = AggValueSlot::ignored());

void EmitAtomicStore(RValue rvalue, LValue lvalue, bool isInit);

void EmitAtomicStore(RValue rvalue, LValue lvalue, llvm::AtomicOrdering AO,
bool IsVolatile, bool isInit);

std::pair<RValue, llvm::Value *> EmitAtomicCompareExchange(
LValue Obj, RValue Expected, RValue Desired, SourceLocation Loc,
llvm::AtomicOrdering Success =
llvm::AtomicOrdering::SequentiallyConsistent,
llvm::AtomicOrdering Failure =
llvm::AtomicOrdering::SequentiallyConsistent,
bool IsWeak = false, AggValueSlot Slot = AggValueSlot::ignored());

void EmitAtomicUpdate(LValue LVal, llvm::AtomicOrdering AO,
const llvm::function_ref<RValue(RValue)> &UpdateOp,
bool IsVolatile);



llvm::Value *EmitToMemory(llvm::Value *Value, QualType Ty);



llvm::Value *EmitFromMemory(llvm::Value *Value, QualType Ty);





bool EmitScalarRangeCheck(llvm::Value *Value, QualType Ty,
SourceLocation Loc);




llvm::Value *EmitLoadOfScalar(Address Addr, bool Volatile, QualType Ty,
SourceLocation Loc,
AlignmentSource Source = AlignmentSource::Type,
bool isNontemporal = false) {
return EmitLoadOfScalar(Addr, Volatile, Ty, Loc, LValueBaseInfo(Source),
CGM.getTBAAAccessInfo(Ty), isNontemporal);
}

llvm::Value *EmitLoadOfScalar(Address Addr, bool Volatile, QualType Ty,
SourceLocation Loc, LValueBaseInfo BaseInfo,
TBAAAccessInfo TBAAInfo,
bool isNontemporal = false);





llvm::Value *EmitLoadOfScalar(LValue lvalue, SourceLocation Loc);




void EmitStoreOfScalar(llvm::Value *Value, Address Addr,
bool Volatile, QualType Ty,
AlignmentSource Source = AlignmentSource::Type,
bool isInit = false, bool isNontemporal = false) {
EmitStoreOfScalar(Value, Addr, Volatile, Ty, LValueBaseInfo(Source),
CGM.getTBAAAccessInfo(Ty), isInit, isNontemporal);
}

void EmitStoreOfScalar(llvm::Value *Value, Address Addr,
bool Volatile, QualType Ty,
LValueBaseInfo BaseInfo, TBAAAccessInfo TBAAInfo,
bool isInit = false, bool isNontemporal = false);






void EmitStoreOfScalar(llvm::Value *value, LValue lvalue, bool isInit=false);




RValue EmitLoadOfLValue(LValue V, SourceLocation Loc);
RValue EmitLoadOfExtVectorElementLValue(LValue V);
RValue EmitLoadOfBitfieldLValue(LValue LV, SourceLocation Loc);
RValue EmitLoadOfGlobalRegLValue(LValue LV);




void EmitStoreThroughLValue(RValue Src, LValue Dst, bool isInit = false);
void EmitStoreThroughExtVectorComponentLValue(RValue Src, LValue Dst);
void EmitStoreThroughGlobalRegLValue(RValue Src, LValue Dst);







void EmitStoreThroughBitfieldLValue(RValue Src, LValue Dst,
llvm::Value **Result=nullptr);


LValue EmitComplexAssignmentLValue(const BinaryOperator *E);
LValue EmitComplexCompoundAssignmentLValue(const CompoundAssignOperator *E);
LValue EmitScalarCompoundAssignWithComplex(const CompoundAssignOperator *E,
llvm::Value *&Result);


LValue EmitBinaryOperatorLValue(const BinaryOperator *E);
LValue EmitCompoundAssignmentLValue(const CompoundAssignOperator *E);

LValue EmitCallExprLValue(const CallExpr *E);

LValue EmitVAArgExprLValue(const VAArgExpr *E);
LValue EmitDeclRefLValue(const DeclRefExpr *E);
LValue EmitStringLiteralLValue(const StringLiteral *E);
LValue EmitObjCEncodeExprLValue(const ObjCEncodeExpr *E);
LValue EmitPredefinedLValue(const PredefinedExpr *E);
LValue EmitUnaryOpLValue(const UnaryOperator *E);
LValue EmitArraySubscriptExpr(const ArraySubscriptExpr *E,
bool Accessed = false);
LValue EmitMatrixSubscriptExpr(const MatrixSubscriptExpr *E);
LValue EmitOMPArraySectionExpr(const OMPArraySectionExpr *E,
bool IsLowerBound = true);
LValue EmitExtVectorElementExpr(const ExtVectorElementExpr *E);
LValue EmitMemberExpr(const MemberExpr *E);
LValue EmitObjCIsaExpr(const ObjCIsaExpr *E);
LValue EmitCompoundLiteralLValue(const CompoundLiteralExpr *E);
LValue EmitInitListLValue(const InitListExpr *E);
LValue EmitConditionalOperatorLValue(const AbstractConditionalOperator *E);
LValue EmitCastLValue(const CastExpr *E);
LValue EmitMaterializeTemporaryExpr(const MaterializeTemporaryExpr *E);
LValue EmitOpaqueValueLValue(const OpaqueValueExpr *e);

Address EmitExtVectorElementLValue(LValue V);

RValue EmitRValueForField(LValue LV, const FieldDecl *FD, SourceLocation Loc);

Address EmitArrayToPointerDecay(const Expr *Array,
LValueBaseInfo *BaseInfo = nullptr,
TBAAAccessInfo *TBAAInfo = nullptr);

class ConstantEmission {
llvm::PointerIntPair<llvm::Constant*, 1, bool> ValueAndIsReference;
ConstantEmission(llvm::Constant *C, bool isReference)
: ValueAndIsReference(C, isReference) {}
public:
ConstantEmission() {}
static ConstantEmission forReference(llvm::Constant *C) {
return ConstantEmission(C, true);
}
static ConstantEmission forValue(llvm::Constant *C) {
return ConstantEmission(C, false);
}

explicit operator bool() const {
return ValueAndIsReference.getOpaqueValue() != nullptr;
}

bool isReference() const { return ValueAndIsReference.getInt(); }
LValue getReferenceLValue(CodeGenFunction &CGF, Expr *refExpr) const {
assert(isReference());
return CGF.MakeNaturalAlignAddrLValue(ValueAndIsReference.getPointer(),
refExpr->getType());
}

llvm::Constant *getValue() const {
assert(!isReference());
return ValueAndIsReference.getPointer();
}
};

ConstantEmission tryEmitAsConstant(DeclRefExpr *refExpr);
ConstantEmission tryEmitAsConstant(const MemberExpr *ME);
llvm::Value *emitScalarConstant(const ConstantEmission &Constant, Expr *E);

RValue EmitPseudoObjectRValue(const PseudoObjectExpr *e,
AggValueSlot slot = AggValueSlot::ignored());
LValue EmitPseudoObjectLValue(const PseudoObjectExpr *e);

llvm::Value *EmitIvarOffset(const ObjCInterfaceDecl *Interface,
const ObjCIvarDecl *Ivar);
LValue EmitLValueForField(LValue Base, const FieldDecl* Field);
LValue EmitLValueForLambdaField(const FieldDecl *Field);




LValue EmitLValueForFieldInitialization(LValue Base,
const FieldDecl* Field);

LValue EmitLValueForIvar(QualType ObjectTy,
llvm::Value* Base, const ObjCIvarDecl *Ivar,
unsigned CVRQualifiers);

LValue EmitCXXConstructLValue(const CXXConstructExpr *E);
LValue EmitCXXBindTemporaryLValue(const CXXBindTemporaryExpr *E);
LValue EmitCXXTypeidLValue(const CXXTypeidExpr *E);
LValue EmitCXXUuidofLValue(const CXXUuidofExpr *E);

LValue EmitObjCMessageExprLValue(const ObjCMessageExpr *E);
LValue EmitObjCIvarRefLValue(const ObjCIvarRefExpr *E);
LValue EmitStmtExprLValue(const StmtExpr *E);
LValue EmitPointerToDataMemberBinaryExpr(const BinaryOperator *E);
LValue EmitObjCSelectorLValue(const ObjCSelectorExpr *E);
void EmitDeclRefExprDbgValue(const DeclRefExpr *E, const APValue &Init);








RValue EmitCall(const CGFunctionInfo &CallInfo, const CGCallee &Callee,
ReturnValueSlot ReturnValue, const CallArgList &Args,
llvm::CallBase **callOrInvoke, bool IsMustTail,
SourceLocation Loc);
RValue EmitCall(const CGFunctionInfo &CallInfo, const CGCallee &Callee,
ReturnValueSlot ReturnValue, const CallArgList &Args,
llvm::CallBase **callOrInvoke = nullptr,
bool IsMustTail = false) {
return EmitCall(CallInfo, Callee, ReturnValue, Args, callOrInvoke,
IsMustTail, SourceLocation());
}
RValue EmitCall(QualType FnType, const CGCallee &Callee, const CallExpr *E,
ReturnValueSlot ReturnValue, llvm::Value *Chain = nullptr);
RValue EmitCallExpr(const CallExpr *E,
ReturnValueSlot ReturnValue = ReturnValueSlot());
RValue EmitSimpleCallExpr(const CallExpr *E, ReturnValueSlot ReturnValue);
CGCallee EmitCallee(const Expr *E);

void checkTargetFeatures(const CallExpr *E, const FunctionDecl *TargetDecl);
void checkTargetFeatures(SourceLocation Loc, const FunctionDecl *TargetDecl);

llvm::CallInst *EmitRuntimeCall(llvm::FunctionCallee callee,
const Twine &name = "");
llvm::CallInst *EmitRuntimeCall(llvm::FunctionCallee callee,
ArrayRef<llvm::Value *> args,
const Twine &name = "");
llvm::CallInst *EmitNounwindRuntimeCall(llvm::FunctionCallee callee,
const Twine &name = "");
llvm::CallInst *EmitNounwindRuntimeCall(llvm::FunctionCallee callee,
ArrayRef<llvm::Value *> args,
const Twine &name = "");

SmallVector<llvm::OperandBundleDef, 1>
getBundlesForFunclet(llvm::Value *Callee);

llvm::CallBase *EmitCallOrInvoke(llvm::FunctionCallee Callee,
ArrayRef<llvm::Value *> Args,
const Twine &Name = "");
llvm::CallBase *EmitRuntimeCallOrInvoke(llvm::FunctionCallee callee,
ArrayRef<llvm::Value *> args,
const Twine &name = "");
llvm::CallBase *EmitRuntimeCallOrInvoke(llvm::FunctionCallee callee,
const Twine &name = "");
void EmitNoreturnRuntimeCallOrInvoke(llvm::FunctionCallee callee,
ArrayRef<llvm::Value *> args);

CGCallee BuildAppleKextVirtualCall(const CXXMethodDecl *MD,
NestedNameSpecifier *Qual,
llvm::Type *Ty);

CGCallee BuildAppleKextVirtualDestructorCall(const CXXDestructorDecl *DD,
CXXDtorType Type,
const CXXRecordDecl *RD);



static std::string getNonTrivialCopyConstructorStr(QualType QT,
CharUnits Alignment,
bool IsVolatile,
ASTContext &Ctx);


static std::string getNonTrivialDestructorStr(QualType QT,
CharUnits Alignment,
bool IsVolatile,
ASTContext &Ctx);



void defaultInitNonTrivialCStructVar(LValue Dst);
void callCStructDefaultConstructor(LValue Dst);
void callCStructDestructor(LValue Dst);
void callCStructCopyConstructor(LValue Dst, LValue Src);
void callCStructMoveConstructor(LValue Dst, LValue Src);
void callCStructCopyAssignmentOperator(LValue Dst, LValue Src);
void callCStructMoveAssignmentOperator(LValue Dst, LValue Src);

RValue
EmitCXXMemberOrOperatorCall(const CXXMethodDecl *Method,
const CGCallee &Callee,
ReturnValueSlot ReturnValue, llvm::Value *This,
llvm::Value *ImplicitParam,
QualType ImplicitParamTy, const CallExpr *E,
CallArgList *RtlArgs);
RValue EmitCXXDestructorCall(GlobalDecl Dtor, const CGCallee &Callee,
llvm::Value *This, QualType ThisTy,
llvm::Value *ImplicitParam,
QualType ImplicitParamTy, const CallExpr *E);
RValue EmitCXXMemberCallExpr(const CXXMemberCallExpr *E,
ReturnValueSlot ReturnValue);
RValue EmitCXXMemberOrOperatorMemberCallExpr(const CallExpr *CE,
const CXXMethodDecl *MD,
ReturnValueSlot ReturnValue,
bool HasQualifier,
NestedNameSpecifier *Qualifier,
bool IsArrow, const Expr *Base);

Address EmitCXXMemberDataPointerAddress(const Expr *E, Address base,
llvm::Value *memberPtr,
const MemberPointerType *memberPtrType,
LValueBaseInfo *BaseInfo = nullptr,
TBAAAccessInfo *TBAAInfo = nullptr);
RValue EmitCXXMemberPointerCallExpr(const CXXMemberCallExpr *E,
ReturnValueSlot ReturnValue);

RValue EmitCXXOperatorMemberCallExpr(const CXXOperatorCallExpr *E,
const CXXMethodDecl *MD,
ReturnValueSlot ReturnValue);
RValue EmitCXXPseudoDestructorExpr(const CXXPseudoDestructorExpr *E);

RValue EmitCUDAKernelCallExpr(const CUDAKernelCallExpr *E,
ReturnValueSlot ReturnValue);

RValue EmitNVPTXDevicePrintfCallExpr(const CallExpr *E,
ReturnValueSlot ReturnValue);
RValue EmitAMDGPUDevicePrintfCallExpr(const CallExpr *E,
ReturnValueSlot ReturnValue);

RValue EmitBuiltinExpr(const GlobalDecl GD, unsigned BuiltinID,
const CallExpr *E, ReturnValueSlot ReturnValue);

RValue emitRotate(const CallExpr *E, bool IsRotateRight);


RValue emitBuiltinOSLogFormat(const CallExpr &E);


RValue EmitBuiltinIsAligned(const CallExpr *E);

RValue EmitBuiltinAlignTo(const CallExpr *E, bool AlignUp);

llvm::Function *generateBuiltinOSLogHelperFunction(
const analyze_os_log::OSLogBufferLayout &Layout,
CharUnits BufferAlignment);

RValue EmitBlockCallExpr(const CallExpr *E, ReturnValueSlot ReturnValue);



llvm::Value *EmitTargetBuiltinExpr(unsigned BuiltinID, const CallExpr *E,
ReturnValueSlot ReturnValue);

llvm::Value *EmitAArch64CompareBuiltinExpr(llvm::Value *Op, llvm::Type *Ty,
const llvm::CmpInst::Predicate Fp,
const llvm::CmpInst::Predicate Ip,
const llvm::Twine &Name = "");
llvm::Value *EmitARMBuiltinExpr(unsigned BuiltinID, const CallExpr *E,
ReturnValueSlot ReturnValue,
llvm::Triple::ArchType Arch);
llvm::Value *EmitARMMVEBuiltinExpr(unsigned BuiltinID, const CallExpr *E,
ReturnValueSlot ReturnValue,
llvm::Triple::ArchType Arch);
llvm::Value *EmitARMCDEBuiltinExpr(unsigned BuiltinID, const CallExpr *E,
ReturnValueSlot ReturnValue,
llvm::Triple::ArchType Arch);
llvm::Value *EmitCMSEClearRecord(llvm::Value *V, llvm::IntegerType *ITy,
QualType RTy);
llvm::Value *EmitCMSEClearRecord(llvm::Value *V, llvm::ArrayType *ATy,
QualType RTy);

llvm::Value *EmitCommonNeonBuiltinExpr(unsigned BuiltinID,
unsigned LLVMIntrinsic,
unsigned AltLLVMIntrinsic,
const char *NameHint,
unsigned Modifier,
const CallExpr *E,
SmallVectorImpl<llvm::Value *> &Ops,
Address PtrOp0, Address PtrOp1,
llvm::Triple::ArchType Arch);

llvm::Function *LookupNeonLLVMIntrinsic(unsigned IntrinsicID,
unsigned Modifier, llvm::Type *ArgTy,
const CallExpr *E);
llvm::Value *EmitNeonCall(llvm::Function *F,
SmallVectorImpl<llvm::Value*> &O,
const char *name,
unsigned shift = 0, bool rightshift = false);
llvm::Value *EmitNeonSplat(llvm::Value *V, llvm::Constant *Idx,
const llvm::ElementCount &Count);
llvm::Value *EmitNeonSplat(llvm::Value *V, llvm::Constant *Idx);
llvm::Value *EmitNeonShiftVector(llvm::Value *V, llvm::Type *Ty,
bool negateForRightShift);
llvm::Value *EmitNeonRShiftImm(llvm::Value *Vec, llvm::Value *Amt,
llvm::Type *Ty, bool usgn, const char *name);
llvm::Value *vectorWrapScalar16(llvm::Value *Op);



llvm::Type *SVEBuiltinMemEltTy(SVETypeFlags TypeFlags);

SmallVector<llvm::Type *, 2> getSVEOverloadTypes(SVETypeFlags TypeFlags,
llvm::Type *ReturnType,
ArrayRef<llvm::Value *> Ops);
llvm::Type *getEltType(SVETypeFlags TypeFlags);
llvm::ScalableVectorType *getSVEType(const SVETypeFlags &TypeFlags);
llvm::ScalableVectorType *getSVEPredType(SVETypeFlags TypeFlags);
llvm::Value *EmitSVEAllTruePred(SVETypeFlags TypeFlags);
llvm::Value *EmitSVEDupX(llvm::Value *Scalar);
llvm::Value *EmitSVEDupX(llvm::Value *Scalar, llvm::Type *Ty);
llvm::Value *EmitSVEReinterpret(llvm::Value *Val, llvm::Type *Ty);
llvm::Value *EmitSVEPMull(SVETypeFlags TypeFlags,
llvm::SmallVectorImpl<llvm::Value *> &Ops,
unsigned BuiltinID);
llvm::Value *EmitSVEMovl(SVETypeFlags TypeFlags,
llvm::ArrayRef<llvm::Value *> Ops,
unsigned BuiltinID);
llvm::Value *EmitSVEPredicateCast(llvm::Value *Pred,
llvm::ScalableVectorType *VTy);
llvm::Value *EmitSVEGatherLoad(SVETypeFlags TypeFlags,
llvm::SmallVectorImpl<llvm::Value *> &Ops,
unsigned IntID);
llvm::Value *EmitSVEScatterStore(SVETypeFlags TypeFlags,
llvm::SmallVectorImpl<llvm::Value *> &Ops,
unsigned IntID);
llvm::Value *EmitSVEMaskedLoad(const CallExpr *, llvm::Type *ReturnTy,
SmallVectorImpl<llvm::Value *> &Ops,
unsigned BuiltinID, bool IsZExtReturn);
llvm::Value *EmitSVEMaskedStore(const CallExpr *,
SmallVectorImpl<llvm::Value *> &Ops,
unsigned BuiltinID);
llvm::Value *EmitSVEPrefetchLoad(SVETypeFlags TypeFlags,
SmallVectorImpl<llvm::Value *> &Ops,
unsigned BuiltinID);
llvm::Value *EmitSVEGatherPrefetch(SVETypeFlags TypeFlags,
SmallVectorImpl<llvm::Value *> &Ops,
unsigned IntID);
llvm::Value *EmitSVEStructLoad(SVETypeFlags TypeFlags,
SmallVectorImpl<llvm::Value *> &Ops, unsigned IntID);
llvm::Value *EmitSVEStructStore(SVETypeFlags TypeFlags,
SmallVectorImpl<llvm::Value *> &Ops,
unsigned IntID);
llvm::Value *EmitAArch64SVEBuiltinExpr(unsigned BuiltinID, const CallExpr *E);

llvm::Value *EmitAArch64BuiltinExpr(unsigned BuiltinID, const CallExpr *E,
llvm::Triple::ArchType Arch);
llvm::Value *EmitBPFBuiltinExpr(unsigned BuiltinID, const CallExpr *E);

llvm::Value *BuildVector(ArrayRef<llvm::Value*> Ops);
llvm::Value *EmitX86BuiltinExpr(unsigned BuiltinID, const CallExpr *E);
llvm::Value *EmitPPCBuiltinExpr(unsigned BuiltinID, const CallExpr *E);
llvm::Value *EmitAMDGPUBuiltinExpr(unsigned BuiltinID, const CallExpr *E);
llvm::Value *EmitSystemZBuiltinExpr(unsigned BuiltinID, const CallExpr *E);
llvm::Value *EmitNVPTXBuiltinExpr(unsigned BuiltinID, const CallExpr *E);
llvm::Value *EmitWebAssemblyBuiltinExpr(unsigned BuiltinID,
const CallExpr *E);
llvm::Value *EmitHexagonBuiltinExpr(unsigned BuiltinID, const CallExpr *E);
llvm::Value *EmitRISCVBuiltinExpr(unsigned BuiltinID, const CallExpr *E,
ReturnValueSlot ReturnValue);
bool ProcessOrderScopeAMDGCN(llvm::Value *Order, llvm::Value *Scope,
llvm::AtomicOrdering &AO,
llvm::SyncScope::ID &SSID);

enum class MSVCIntrin;
llvm::Value *EmitMSVCBuiltinExpr(MSVCIntrin BuiltinID, const CallExpr *E);

llvm::Value *EmitBuiltinAvailable(const VersionTuple &Version);

llvm::Value *EmitObjCProtocolExpr(const ObjCProtocolExpr *E);
llvm::Value *EmitObjCStringLiteral(const ObjCStringLiteral *E);
llvm::Value *EmitObjCBoxedExpr(const ObjCBoxedExpr *E);
llvm::Value *EmitObjCArrayLiteral(const ObjCArrayLiteral *E);
llvm::Value *EmitObjCDictionaryLiteral(const ObjCDictionaryLiteral *E);
llvm::Value *EmitObjCCollectionLiteral(const Expr *E,
const ObjCMethodDecl *MethodWithObjects);
llvm::Value *EmitObjCSelectorExpr(const ObjCSelectorExpr *E);
RValue EmitObjCMessageExpr(const ObjCMessageExpr *E,
ReturnValueSlot Return = ReturnValueSlot());



CleanupKind getARCCleanupKind() {
return CGM.getCodeGenOpts().ObjCAutoRefCountExceptions
? NormalAndEHCleanup : NormalCleanup;
}


void EmitARCInitWeak(Address addr, llvm::Value *value);
void EmitARCDestroyWeak(Address addr);
llvm::Value *EmitARCLoadWeak(Address addr);
llvm::Value *EmitARCLoadWeakRetained(Address addr);
llvm::Value *EmitARCStoreWeak(Address addr, llvm::Value *value, bool ignored);
void emitARCCopyAssignWeak(QualType Ty, Address DstAddr, Address SrcAddr);
void emitARCMoveAssignWeak(QualType Ty, Address DstAddr, Address SrcAddr);
void EmitARCCopyWeak(Address dst, Address src);
void EmitARCMoveWeak(Address dst, Address src);
llvm::Value *EmitARCRetainAutorelease(QualType type, llvm::Value *value);
llvm::Value *EmitARCRetainAutoreleaseNonBlock(llvm::Value *value);
llvm::Value *EmitARCStoreStrong(LValue lvalue, llvm::Value *value,
bool resultIgnored);
llvm::Value *EmitARCStoreStrongCall(Address addr, llvm::Value *value,
bool resultIgnored);
llvm::Value *EmitARCRetain(QualType type, llvm::Value *value);
llvm::Value *EmitARCRetainNonBlock(llvm::Value *value);
llvm::Value *EmitARCRetainBlock(llvm::Value *value, bool mandatory);
void EmitARCDestroyStrong(Address addr, ARCPreciseLifetime_t precise);
void EmitARCRelease(llvm::Value *value, ARCPreciseLifetime_t precise);
llvm::Value *EmitARCAutorelease(llvm::Value *value);
llvm::Value *EmitARCAutoreleaseReturnValue(llvm::Value *value);
llvm::Value *EmitARCRetainAutoreleaseReturnValue(llvm::Value *value);
llvm::Value *EmitARCRetainAutoreleasedReturnValue(llvm::Value *value);
llvm::Value *EmitARCUnsafeClaimAutoreleasedReturnValue(llvm::Value *value);

llvm::Value *EmitObjCAutorelease(llvm::Value *value, llvm::Type *returnType);
llvm::Value *EmitObjCRetainNonBlock(llvm::Value *value,
llvm::Type *returnType);
void EmitObjCRelease(llvm::Value *value, ARCPreciseLifetime_t precise);

std::pair<LValue,llvm::Value*>
EmitARCStoreAutoreleasing(const BinaryOperator *e);
std::pair<LValue,llvm::Value*>
EmitARCStoreStrong(const BinaryOperator *e, bool ignored);
std::pair<LValue,llvm::Value*>
EmitARCStoreUnsafeUnretained(const BinaryOperator *e, bool ignored);

llvm::Value *EmitObjCAlloc(llvm::Value *value,
llvm::Type *returnType);
llvm::Value *EmitObjCAllocWithZone(llvm::Value *value,
llvm::Type *returnType);
llvm::Value *EmitObjCAllocInit(llvm::Value *value, llvm::Type *resultType);

llvm::Value *EmitObjCThrowOperand(const Expr *expr);
llvm::Value *EmitObjCConsumeObject(QualType T, llvm::Value *Ptr);
llvm::Value *EmitObjCExtendObjectLifetime(QualType T, llvm::Value *Ptr);

llvm::Value *EmitARCExtendBlockObject(const Expr *expr);
llvm::Value *EmitARCReclaimReturnedObject(const Expr *e,
bool allowUnsafeClaim);
llvm::Value *EmitARCRetainScalarExpr(const Expr *expr);
llvm::Value *EmitARCRetainAutoreleaseScalarExpr(const Expr *expr);
llvm::Value *EmitARCUnsafeUnretainedScalarExpr(const Expr *expr);

void EmitARCIntrinsicUse(ArrayRef<llvm::Value*> values);

void EmitARCNoopIntrinsicUse(ArrayRef<llvm::Value *> values);

static Destroyer destroyARCStrongImprecise;
static Destroyer destroyARCStrongPrecise;
static Destroyer destroyARCWeak;
static Destroyer emitARCIntrinsicUse;
static Destroyer destroyNonTrivialCStruct;

void EmitObjCAutoreleasePoolPop(llvm::Value *Ptr);
llvm::Value *EmitObjCAutoreleasePoolPush();
llvm::Value *EmitObjCMRRAutoreleasePoolPush();
void EmitObjCAutoreleasePoolCleanup(llvm::Value *Ptr);
void EmitObjCMRRAutoreleasePoolPop(llvm::Value *Ptr);


RValue EmitReferenceBindingToExpr(const Expr *E);









llvm::Value *EmitScalarExpr(const Expr *E , bool IgnoreResultAssign = false);



llvm::Value *EmitScalarConversion(llvm::Value *Src, QualType SrcTy,
QualType DstTy, SourceLocation Loc);



llvm::Value *EmitComplexToScalarConversion(ComplexPairTy Src, QualType SrcTy,
QualType DstTy,
SourceLocation Loc);




void EmitAggExpr(const Expr *E, AggValueSlot AS);



LValue EmitAggExprToLValue(const Expr *E);



void EmitAggregateStore(llvm::Value *Val, Address Dest, bool DestIsVolatile);



void EmitExtendGCLifetime(llvm::Value *object);



ComplexPairTy EmitComplexExpr(const Expr *E,
bool IgnoreReal = false,
bool IgnoreImag = false);



void EmitComplexExprIntoLValue(const Expr *E, LValue dest, bool isInit);


void EmitStoreOfComplex(ComplexPairTy V, LValue dest, bool isInit);


ComplexPairTy EmitLoadOfComplex(LValue src, SourceLocation loc);

Address emitAddrOfRealComponent(Address complex, QualType complexType);
Address emitAddrOfImagComponent(Address complex, QualType complexType);





llvm::GlobalVariable *
AddInitializerToStaticVarDecl(const VarDecl &D,
llvm::GlobalVariable *GV);


void EmitInvariantStart(llvm::Constant *Addr, CharUnits Size);



void EmitCXXGlobalVarDeclInit(const VarDecl &D, llvm::Constant *DeclPtr,
bool PerformInit);

llvm::Function *createAtExitStub(const VarDecl &VD, llvm::FunctionCallee Dtor,
llvm::Constant *Addr);

llvm::Function *createTLSAtExitStub(const VarDecl &VD,
llvm::FunctionCallee Dtor,
llvm::Constant *Addr,
llvm::FunctionCallee &AtExit);



void registerGlobalDtorWithAtExit(const VarDecl &D, llvm::FunctionCallee fn,
llvm::Constant *addr);


void registerGlobalDtorWithAtExit(llvm::Constant *dtorStub);


llvm::Value *unregisterGlobalDtorWithUnAtExit(llvm::Constant *dtorStub);






void EmitCXXGuardedInit(const VarDecl &D, llvm::GlobalVariable *DeclPtr,
bool PerformInit);

enum class GuardKind { VariableGuard, TlsGuard };


void EmitCXXGuardedInitBranch(llvm::Value *NeedsInit,
llvm::BasicBlock *InitBlock,
llvm::BasicBlock *NoInitBlock,
GuardKind Kind, const VarDecl *D);



void
GenerateCXXGlobalInitFunc(llvm::Function *Fn,
ArrayRef<llvm::Function *> CXXThreadLocals,
ConstantAddress Guard = ConstantAddress::invalid());



void GenerateCXXGlobalCleanUpFunc(
llvm::Function *Fn,
ArrayRef<std::tuple<llvm::FunctionType *, llvm::WeakTrackingVH,
llvm::Constant *>>
DtorsOrStermFinalizers);

void GenerateCXXGlobalVarDeclInitFunc(llvm::Function *Fn,
const VarDecl *D,
llvm::GlobalVariable *Addr,
bool PerformInit);

void EmitCXXConstructExpr(const CXXConstructExpr *E, AggValueSlot Dest);

void EmitSynthesizedCXXCopyCtor(Address Dest, Address Src, const Expr *Exp);

void EmitCXXThrowExpr(const CXXThrowExpr *E, bool KeepInsertionPoint = true);

RValue EmitAtomicExpr(AtomicExpr *E);






llvm::Value *EmitAnnotationCall(llvm::Function *AnnotationFn,
llvm::Value *AnnotatedVal,
StringRef AnnotationStr,
SourceLocation Location,
const AnnotateAttr *Attr);


void EmitVarAnnotations(const VarDecl *D, llvm::Value *V);



Address EmitFieldAnnotations(const FieldDecl *D, Address V);








static bool ContainsLabel(const Stmt *S, bool IgnoreCaseStmts = false);




static bool containsBreak(const Stmt *S);



static bool mightAddDeclToScope(const Stmt *S);




bool ConstantFoldsToSimpleInteger(const Expr *Cond, bool &Result,
bool AllowLabels = false);




bool ConstantFoldsToSimpleInteger(const Expr *Cond, llvm::APSInt &Result,
bool AllowLabels = false);



static bool isInstrumentedCondition(const Expr *C);





void EmitBranchToCounterBlock(const Expr *Cond, BinaryOperator::Opcode LOp,
llvm::BasicBlock *TrueBlock,
llvm::BasicBlock *FalseBlock,
uint64_t TrueCount = 0,
Stmt::Likelihood LH = Stmt::LH_None,
const Expr *CntrIdx = nullptr);






void EmitBranchOnBoolExpr(const Expr *Cond, llvm::BasicBlock *TrueBlock,
llvm::BasicBlock *FalseBlock, uint64_t TrueCount,
Stmt::Likelihood LH = Stmt::LH_None);



void EmitNullabilityCheck(LValue LHS, llvm::Value *RHS, SourceLocation Loc);



enum { NotSubtraction = false, IsSubtraction = true };






llvm::Value *EmitCheckedInBoundsGEP(llvm::Value *Ptr,
ArrayRef<llvm::Value *> IdxList,
bool SignedIndices,
bool IsSubtraction,
SourceLocation Loc,
const Twine &Name = "");



enum BuiltinCheckKind {
BCK_CTZPassedZero,
BCK_CLZPassedZero,
};



llvm::Value *EmitCheckedArgForBuiltin(const Expr *E, BuiltinCheckKind Kind);



llvm::Constant *EmitCheckTypeDescriptor(QualType T);



llvm::Value *EmitCheckValue(llvm::Value *V);



llvm::Constant *EmitCheckSourceLocation(SourceLocation Loc);




void EmitCheck(ArrayRef<std::pair<llvm::Value *, SanitizerMask>> Checked,
SanitizerHandler Check, ArrayRef<llvm::Constant *> StaticArgs,
ArrayRef<llvm::Value *> DynamicArgs);



void EmitCfiSlowPathCheck(SanitizerMask Kind, llvm::Value *Cond,
llvm::ConstantInt *TypeId, llvm::Value *Ptr,
ArrayRef<llvm::Constant *> StaticArgs);



void EmitUnreachable(SourceLocation Loc);



void EmitTrapCheck(llvm::Value *Checked, SanitizerHandler CheckHandlerID);



llvm::CallInst *EmitTrapCall(llvm::Intrinsic::ID IntrID);


void EmitCfiCheckStub();


void EmitCfiCheckFail();



void EmitNonNullArgCheck(RValue RV, QualType ArgType, SourceLocation ArgLoc,
AbstractCallee AC, unsigned ParmNum);


void EmitCallArg(CallArgList &args, const Expr *E, QualType ArgType);




void EmitDelegateCallArg(CallArgList &args, const VarDecl *param,
SourceLocation loc);



void SetFPAccuracy(llvm::Value *Val, float Accuracy);


void SetFPModel();


void SetFastMathFlags(FPOptions FPFeatures);

private:
llvm::MDNode *getRangeForLoadFromType(QualType Ty);
void EmitReturnOfRValue(RValue RV, QualType Ty);

void deferPlaceholderReplacement(llvm::Instruction *Old, llvm::Value *New);

llvm::SmallVector<std::pair<llvm::WeakTrackingVH, llvm::Value *>, 4>
DeferredReplacements;


void setAddrOfLocalVar(const VarDecl *VD, Address Addr) {
assert(!LocalDeclMap.count(VD) && "Decl already exists in LocalDeclMap!");
LocalDeclMap.insert({VD, Addr});
}





void ExpandTypeFromArgs(QualType Ty, LValue Dst,
llvm::Function::arg_iterator &AI);




void ExpandTypeToArgs(QualType Ty, CallArg Arg, llvm::FunctionType *IRFuncTy,
SmallVectorImpl<llvm::Value *> &IRCallArgs,
unsigned &IRCallArgPos);

llvm::Value* EmitAsmInput(const TargetInfo::ConstraintInfo &Info,
const Expr *InputExpr, std::string &ConstraintStr);

llvm::Value* EmitAsmInputLValue(const TargetInfo::ConstraintInfo &Info,
LValue InputValue, QualType InputType,
std::string &ConstraintStr,
SourceLocation Loc);






llvm::Value *evaluateOrEmitBuiltinObjectSize(const Expr *E, unsigned Type,
llvm::IntegerType *ResType,
llvm::Value *EmittedE,
bool IsDynamic);




llvm::Value *emitBuiltinObjectSize(const Expr *E, unsigned Type,
llvm::IntegerType *ResType,
llvm::Value *EmittedE,
bool IsDynamic);

void emitZeroOrPatternForAutoVarInit(QualType type, const VarDecl &D,
Address Loc);

public:
enum class EvaluationOrder {

Default,

ForceLeftToRight,

ForceRightToLeft
};



struct PrototypeWrapper {
llvm::PointerUnion<const FunctionProtoType *, const ObjCMethodDecl *> P;

PrototypeWrapper(const FunctionProtoType *FT) : P(FT) {}
PrototypeWrapper(const ObjCMethodDecl *MD) : P(MD) {}
};

void EmitCallArgs(CallArgList &Args, PrototypeWrapper Prototype,
llvm::iterator_range<CallExpr::const_arg_iterator> ArgRange,
AbstractCallee AC = AbstractCallee(),
unsigned ParamsToSkip = 0,
EvaluationOrder Order = EvaluationOrder::Default);


















Address EmitPointerWithAlignment(const Expr *Addr,
LValueBaseInfo *BaseInfo = nullptr,
TBAAAccessInfo *TBAAInfo = nullptr);




llvm::Value *LoadPassedObjectSize(const Expr *E, QualType EltTy);

void EmitSanitizerStatReport(llvm::SanitizerStatKind SSK);

struct MultiVersionResolverOption {
llvm::Function *Function;
struct Conds {
StringRef Architecture;
llvm::SmallVector<StringRef, 8> Features;

Conds(StringRef Arch, ArrayRef<StringRef> Feats)
: Architecture(Arch), Features(Feats.begin(), Feats.end()) {}
} Conditions;

MultiVersionResolverOption(llvm::Function *F, StringRef Arch,
ArrayRef<StringRef> Feats)
: Function(F), Conditions(Arch, Feats) {}
};




void EmitMultiVersionResolver(llvm::Function *Resolver,
ArrayRef<MultiVersionResolverOption> Options);

static uint64_t GetX86CpuSupportsMask(ArrayRef<StringRef> FeatureStrs);

private:
QualType getVarArgType(const Expr *Arg);

void EmitDeclMetadata();

BlockByrefHelpers *buildByrefHelpers(llvm::StructType &byrefType,
const AutoVarEmission &emission);

void AddObjCARCExceptionMetadata(llvm::Instruction *Inst);

llvm::Value *GetValueForARMHint(unsigned BuiltinID);
llvm::Value *EmitX86CpuIs(const CallExpr *E);
llvm::Value *EmitX86CpuIs(StringRef CPUStr);
llvm::Value *EmitX86CpuSupports(const CallExpr *E);
llvm::Value *EmitX86CpuSupports(ArrayRef<StringRef> FeatureStrs);
llvm::Value *EmitX86CpuSupports(uint64_t Mask);
llvm::Value *EmitX86CpuInit();
llvm::Value *FormResolverCondition(const MultiVersionResolverOption &RO);
};











class TargetFeatures {
struct FeatureListStatus {
bool HasFeatures;
StringRef CurFeaturesList;
};

const llvm::StringMap<bool> &CallerFeatureMap;

FeatureListStatus getAndFeatures(StringRef FeatureList) {
int InParentheses = 0;
bool HasFeatures = true;
size_t SubexpressionStart = 0;
for (size_t i = 0, e = FeatureList.size(); i < e; ++i) {
char CurrentToken = FeatureList[i];
switch (CurrentToken) {
default:
break;
case '(':
if (InParentheses == 0)
SubexpressionStart = i + 1;
++InParentheses;
break;
case ')':
--InParentheses;
assert(InParentheses >= 0 && "Parentheses are not in pair");
LLVM_FALLTHROUGH;
case '|':
case ',':
if (InParentheses == 0) {
if (HasFeatures && i != SubexpressionStart) {
StringRef F = FeatureList.slice(SubexpressionStart, i);
HasFeatures = CurrentToken == ')' ? hasRequiredFeatures(F)
: CallerFeatureMap.lookup(F);
}
SubexpressionStart = i + 1;
if (CurrentToken == '|') {
return {HasFeatures, FeatureList.substr(SubexpressionStart)};
}
}
break;
}
}
assert(InParentheses == 0 && "Parentheses are not in pair");
if (HasFeatures && SubexpressionStart != FeatureList.size())
HasFeatures =
CallerFeatureMap.lookup(FeatureList.substr(SubexpressionStart));
return {HasFeatures, StringRef()};
}

public:
bool hasRequiredFeatures(StringRef FeatureList) {
FeatureListStatus FS = {false, FeatureList};
while (!FS.HasFeatures && !FS.CurFeaturesList.empty())
FS = getAndFeatures(FS.CurFeaturesList);
return FS.HasFeatures;
}

TargetFeatures(const llvm::StringMap<bool> &CallerFeatureMap)
: CallerFeatureMap(CallerFeatureMap) {}
};

inline DominatingLLVMValue::saved_type
DominatingLLVMValue::save(CodeGenFunction &CGF, llvm::Value *value) {
if (!needsSaving(value)) return saved_type(value, false);


auto align = CharUnits::fromQuantity(
CGF.CGM.getDataLayout().getPrefTypeAlignment(value->getType()));
Address alloca =
CGF.CreateTempAlloca(value->getType(), align, "cond-cleanup.save");
CGF.Builder.CreateStore(value, alloca);

return saved_type(alloca.getPointer(), true);
}

inline llvm::Value *DominatingLLVMValue::restore(CodeGenFunction &CGF,
saved_type value) {

if (!value.getInt()) return value.getPointer();


auto alloca = cast<llvm::AllocaInst>(value.getPointer());
return CGF.Builder.CreateAlignedLoad(alloca->getAllocatedType(), alloca,
alloca->getAlign());
}

}



llvm::fp::ExceptionBehavior
ToConstrainedExceptMD(LangOptions::FPExceptionModeKind Kind);
}

#endif
