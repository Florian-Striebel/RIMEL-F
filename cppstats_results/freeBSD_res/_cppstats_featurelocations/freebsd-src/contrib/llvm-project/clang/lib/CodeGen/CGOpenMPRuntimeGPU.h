












#if !defined(LLVM_CLANG_LIB_CODEGEN_CGOPENMPRUNTIMEGPU_H)
#define LLVM_CLANG_LIB_CODEGEN_CGOPENMPRUNTIMEGPU_H

#include "CGOpenMPRuntime.h"
#include "CodeGenFunction.h"
#include "clang/AST/StmtOpenMP.h"
#include "llvm/Frontend/OpenMP/OMPGridValues.h"

namespace clang {
namespace CodeGen {

class CGOpenMPRuntimeGPU : public CGOpenMPRuntime {
public:

enum ExecutionMode {

EM_SPMD,

EM_NonSPMD,

EM_Unknown,
};
private:

llvm::SmallVector<llvm::Function *, 16> Work;

struct EntryFunctionState {
SourceLocation Loc;
};

ExecutionMode getExecutionMode() const;

bool requiresFullRuntime() const { return RequiresFullRuntime; }


void syncCTAThreads(CodeGenFunction &CGF);


void emitKernelInit(CodeGenFunction &CGF, EntryFunctionState &EST,
bool IsSPMD);


void emitKernelDeinit(CodeGenFunction &CGF, EntryFunctionState &EST,
bool IsSPMD);


void emitGenericVarsProlog(CodeGenFunction &CGF, SourceLocation Loc,
bool WithSPMDCheck = false);


void emitGenericVarsEpilog(CodeGenFunction &CGF, bool WithSPMDCheck = false);







void createOffloadEntry(llvm::Constant *ID, llvm::Constant *Addr,
uint64_t Size, int32_t Flags,
llvm::GlobalValue::LinkageTypes Linkage) override;










void emitNonSPMDKernel(const OMPExecutableDirective &D, StringRef ParentName,
llvm::Function *&OutlinedFn,
llvm::Constant *&OutlinedFnID, bool IsOffloadEntry,
const RegionCodeGenTy &CodeGen);












void emitSPMDKernel(const OMPExecutableDirective &D, StringRef ParentName,
llvm::Function *&OutlinedFn,
llvm::Constant *&OutlinedFnID, bool IsOffloadEntry,
const RegionCodeGenTy &CodeGen);










void emitTargetOutlinedFunction(const OMPExecutableDirective &D,
StringRef ParentName,
llvm::Function *&OutlinedFn,
llvm::Constant *&OutlinedFnID,
bool IsOffloadEntry,
const RegionCodeGenTy &CodeGen) override;











void emitNonSPMDParallelCall(CodeGenFunction &CGF, SourceLocation Loc,
llvm::Value *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars,
const Expr *IfCond);












void emitSPMDParallelCall(CodeGenFunction &CGF, SourceLocation Loc,
llvm::Function *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars,
const Expr *IfCond);

protected:



StringRef getOutlinedHelperName() const override {
return "__omp_outlined__";
}



bool isDefaultLocationConstant() const override { return true; }





unsigned getDefaultLocationReserved2Flags() const override;

public:
explicit CGOpenMPRuntimeGPU(CodeGenModule &CGM);
void clear() override;






virtual llvm::Value *getGPUWarpSize(CodeGenFunction &CGF) = 0;


virtual llvm::Value *getGPUThreadID(CodeGenFunction &CGF) = 0;


virtual llvm::Value *getGPUNumThreads(CodeGenFunction &CGF) = 0;



virtual void emitProcBindClause(CodeGenFunction &CGF,
llvm::omp::ProcBindKind ProcBind,
SourceLocation Loc) override;





virtual void emitNumThreadsClause(CodeGenFunction &CGF,
llvm::Value *NumThreads,
SourceLocation Loc) override;






void emitNumTeamsClause(CodeGenFunction &CGF, const Expr *NumTeams,
const Expr *ThreadLimit, SourceLocation Loc) override;










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









void emitTeamsCall(CodeGenFunction &CGF, const OMPExecutableDirective &D,
SourceLocation Loc, llvm::Function *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars) override;










void emitParallelCall(CodeGenFunction &CGF, SourceLocation Loc,
llvm::Function *OutlinedFn,
ArrayRef<llvm::Value *> CapturedVars,
const Expr *IfCond) override;









void emitBarrierCall(CodeGenFunction &CGF, SourceLocation Loc,
OpenMPDirectiveKind Kind, bool EmitChecks = true,
bool ForceSimpleCall = false) override;






void emitCriticalRegion(CodeGenFunction &CGF, StringRef CriticalName,
const RegionCodeGenTy &CriticalOpGen,
SourceLocation Loc,
const Expr *Hint = nullptr) override;














virtual void emitReduction(CodeGenFunction &CGF, SourceLocation Loc,
ArrayRef<const Expr *> Privates,
ArrayRef<const Expr *> LHSExprs,
ArrayRef<const Expr *> RHSExprs,
ArrayRef<const Expr *> ReductionOps,
ReductionOptionsTy Options) override;





llvm::FunctionCallee createNVPTXRuntimeFunction(unsigned Function);





const VarDecl *translateParameter(const FieldDecl *FD,
const VarDecl *NativeParam) const override;





Address getParameterAddress(CodeGenFunction &CGF, const VarDecl *NativeParam,
const VarDecl *TargetParam) const override;



void emitOutlinedFunctionCall(
CodeGenFunction &CGF, SourceLocation Loc, llvm::FunctionCallee OutlinedFn,
ArrayRef<llvm::Value *> Args = llvm::None) const override;



void emitFunctionProlog(CodeGenFunction &CGF, const Decl *D) override;


Address getAddressOfLocalVariable(CodeGenFunction &CGF,
const VarDecl *VD) override;





enum DataSharingMode {

CUDA,

Generic,
};



void functionFinished(CodeGenFunction &CGF) override;


void getDefaultDistScheduleAndChunk(CodeGenFunction &CGF,
const OMPLoopDirective &S, OpenMPDistScheduleClauseKind &ScheduleKind,
llvm::Value *&Chunk) const override;


void getDefaultScheduleAndChunk(CodeGenFunction &CGF,
const OMPLoopDirective &S, OpenMPScheduleClauseKind &ScheduleKind,
const Expr *&ChunkExpr) const override;



void adjustTargetSpecificDataForLambdas(
CodeGenFunction &CGF, const OMPExecutableDirective &D) const override;



void processRequiresDirective(const OMPRequiresDecl *D) override;




bool hasAllocateAttributeForGlobalVar(const VarDecl *VD, LangAS &AS) override;

private:




ExecutionMode CurrentExecutionMode = EM_Unknown;


bool RequiresFullRuntime = true;



bool IsInTargetMasterThreadRegion = false;


bool IsInTTDRegion = false;

bool IsInParallelRegion = false;


llvm::DenseMap<llvm::Function *, llvm::Function *> WrapperFunctionsMap;





llvm::Function *createParallelDataSharingWrapper(
llvm::Function *OutlinedParallelFn, const OMPExecutableDirective &D);


struct MappedVarData {

llvm::Value *GlobalizedVal = nullptr;

Address PrivateAddr = Address::invalid();
};

using DeclToAddrMapTy = llvm::MapVector<const Decl *, MappedVarData>;

using EscapedParamsTy = llvm::SmallPtrSet<const Decl *, 4>;
struct FunctionData {
DeclToAddrMapTy LocalVarData;
llvm::Optional<DeclToAddrMapTy> SecondaryLocalVarData = llvm::None;
EscapedParamsTy EscapedParameters;
llvm::SmallVector<const ValueDecl*, 4> EscapedVariableLengthDecls;
llvm::SmallVector<std::pair<llvm::Value *, llvm::Value *>, 4>
EscapedVariableLengthDeclsAddrs;
llvm::Value *IsInSPMDModeFlag = nullptr;
std::unique_ptr<CodeGenFunction::OMPMapVars> MappedParams;
};


llvm::SmallDenseMap<llvm::Function *, FunctionData> FunctionGlobalizedDecls;
llvm::GlobalVariable *KernelTeamsReductionPtr = nullptr;




llvm::SmallVector<const RecordDecl *, 4> TeamsReductions;


llvm::GlobalVariable *KernelStaticGlobalized = nullptr;


std::pair<const Decl *, llvm::SmallVector<const ValueDecl *, 4>>
TeamAndReductions;
};

}
}

#endif
