











#if !defined(LLVM_CLANG_LIB_CODEGEN_CODEGENMODULE_H)
#define LLVM_CLANG_LIB_CODEGEN_CODEGENMODULE_H

#include "CGVTables.h"
#include "CodeGenTypeCache.h"
#include "CodeGenTypes.h"
#include "SanitizerMetadata.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclOpenMP.h"
#include "clang/AST/GlobalDecl.h"
#include "clang/AST/Mangle.h"
#include "clang/Basic/ABI.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/NoSanitizeList.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/XRayLists.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ValueHandle.h"
#include "llvm/Transforms/Utils/SanitizerStats.h"

namespace llvm {
class Module;
class Constant;
class ConstantInt;
class Function;
class GlobalValue;
class DataLayout;
class FunctionType;
class LLVMContext;
class OpenMPIRBuilder;
class IndexedInstrProfReader;
}

namespace clang {
class ASTContext;
class AtomicType;
class FunctionDecl;
class IdentifierInfo;
class ObjCMethodDecl;
class ObjCImplementationDecl;
class ObjCCategoryImplDecl;
class ObjCProtocolDecl;
class ObjCEncodeExpr;
class BlockExpr;
class CharUnits;
class Decl;
class Expr;
class Stmt;
class InitListExpr;
class StringLiteral;
class NamedDecl;
class ValueDecl;
class VarDecl;
class LangOptions;
class CodeGenOptions;
class HeaderSearchOptions;
class DiagnosticsEngine;
class AnnotateAttr;
class CXXDestructorDecl;
class Module;
class CoverageSourceInfo;
class TargetAttr;
class InitSegAttr;
struct ParsedTargetAttr;

namespace CodeGen {

class CallArgList;
class CodeGenFunction;
class CodeGenTBAA;
class CGCXXABI;
class CGDebugInfo;
class CGObjCRuntime;
class CGOpenCLRuntime;
class CGOpenMPRuntime;
class CGCUDARuntime;
class BlockFieldFlags;
class FunctionArgList;
class CoverageMappingModuleGen;
class TargetCodeGenInfo;

enum ForDefinition_t : bool {
NotForDefinition = false,
ForDefinition = true
};

struct OrderGlobalInitsOrStermFinalizers {
unsigned int priority;
unsigned int lex_order;
OrderGlobalInitsOrStermFinalizers(unsigned int p, unsigned int l)
: priority(p), lex_order(l) {}

bool operator==(const OrderGlobalInitsOrStermFinalizers &RHS) const {
return priority == RHS.priority && lex_order == RHS.lex_order;
}

bool operator<(const OrderGlobalInitsOrStermFinalizers &RHS) const {
return std::tie(priority, lex_order) <
std::tie(RHS.priority, RHS.lex_order);
}
};

struct ObjCEntrypoints {
ObjCEntrypoints() { memset(this, 0, sizeof(*this)); }


llvm::FunctionCallee objc_alloc;


llvm::FunctionCallee objc_allocWithZone;


llvm::FunctionCallee objc_alloc_init;


llvm::FunctionCallee objc_autoreleasePoolPop;



llvm::FunctionCallee objc_autoreleasePoolPopInvoke;


llvm::Function *objc_autoreleasePoolPush;


llvm::Function *objc_autorelease;



llvm::FunctionCallee objc_autoreleaseRuntimeFunction;


llvm::Function *objc_autoreleaseReturnValue;


llvm::Function *objc_copyWeak;


llvm::Function *objc_destroyWeak;


llvm::Function *objc_initWeak;


llvm::Function *objc_loadWeak;


llvm::Function *objc_loadWeakRetained;


llvm::Function *objc_moveWeak;


llvm::Function *objc_retain;



llvm::FunctionCallee objc_retainRuntimeFunction;


llvm::Function *objc_retainAutorelease;


llvm::Function *objc_retainAutoreleaseReturnValue;


llvm::Function *objc_retainAutoreleasedReturnValue;


llvm::Function *objc_retainBlock;


llvm::Function *objc_release;



llvm::FunctionCallee objc_releaseRuntimeFunction;


llvm::Function *objc_storeStrong;


llvm::Function *objc_storeWeak;


llvm::Function *objc_unsafeClaimAutoreleasedReturnValue;



llvm::InlineAsm *retainAutoreleasedReturnValueMarker;


llvm::Function *clang_arc_use;


llvm::Function *clang_arc_noop_use;
};


class InstrProfStats {
uint32_t VisitedInMainFile;
uint32_t MissingInMainFile;
uint32_t Visited;
uint32_t Missing;
uint32_t Mismatched;

public:
InstrProfStats()
: VisitedInMainFile(0), MissingInMainFile(0), Visited(0), Missing(0),
Mismatched(0) {}


void addVisited(bool MainFile) {
if (MainFile)
++VisitedInMainFile;
++Visited;
}

void addMissing(bool MainFile) {
if (MainFile)
++MissingInMainFile;
++Missing;
}

void addMismatched(bool MainFile) { ++Mismatched; }

bool hasDiagnostics() { return Missing || Mismatched; }

void reportDiagnostics(DiagnosticsEngine &Diags, StringRef MainFile);
};


class BlockByrefHelpers : public llvm::FoldingSetNode {


public:
llvm::Constant *CopyHelper;
llvm::Constant *DisposeHelper;




CharUnits Alignment;

BlockByrefHelpers(CharUnits alignment)
: CopyHelper(nullptr), DisposeHelper(nullptr), Alignment(alignment) {}
BlockByrefHelpers(const BlockByrefHelpers &) = default;
virtual ~BlockByrefHelpers();

void Profile(llvm::FoldingSetNodeID &id) const {
id.AddInteger(Alignment.getQuantity());
profileImpl(id);
}
virtual void profileImpl(llvm::FoldingSetNodeID &id) const = 0;

virtual bool needsCopy() const { return true; }
virtual void emitCopy(CodeGenFunction &CGF, Address dest, Address src) = 0;

virtual bool needsDispose() const { return true; }
virtual void emitDispose(CodeGenFunction &CGF, Address field) = 0;
};



class CodeGenModule : public CodeGenTypeCache {
CodeGenModule(const CodeGenModule &) = delete;
void operator=(const CodeGenModule &) = delete;

public:
struct Structor {
Structor() : Priority(0), Initializer(nullptr), AssociatedData(nullptr) {}
Structor(int Priority, llvm::Constant *Initializer,
llvm::Constant *AssociatedData)
: Priority(Priority), Initializer(Initializer),
AssociatedData(AssociatedData) {}
int Priority;
llvm::Constant *Initializer;
llvm::Constant *AssociatedData;
};

typedef std::vector<Structor> CtorList;

private:
ASTContext &Context;
const LangOptions &LangOpts;
const HeaderSearchOptions &HeaderSearchOpts;
const PreprocessorOptions &PreprocessorOpts;
const CodeGenOptions &CodeGenOpts;
unsigned NumAutoVarInit = 0;
llvm::Module &TheModule;
DiagnosticsEngine &Diags;
const TargetInfo &Target;
std::unique_ptr<CGCXXABI> ABI;
llvm::LLVMContext &VMContext;
std::string ModuleNameHash = "";

std::unique_ptr<CodeGenTBAA> TBAA;

mutable std::unique_ptr<TargetCodeGenInfo> TheTargetCodeGenInfo;




CodeGenTypes Types;


CodeGenVTables VTables;

std::unique_ptr<CGObjCRuntime> ObjCRuntime;
std::unique_ptr<CGOpenCLRuntime> OpenCLRuntime;
std::unique_ptr<CGOpenMPRuntime> OpenMPRuntime;
std::unique_ptr<CGCUDARuntime> CUDARuntime;
std::unique_ptr<CGDebugInfo> DebugInfo;
std::unique_ptr<ObjCEntrypoints> ObjCData;
llvm::MDNode *NoObjCARCExceptionsMetadata = nullptr;
std::unique_ptr<llvm::IndexedInstrProfReader> PGOReader;
InstrProfStats PGOStats;
std::unique_ptr<llvm::SanitizerStatReport> SanStats;




llvm::SmallPtrSet<llvm::GlobalValue*, 10> WeakRefReferences;





std::map<StringRef, GlobalDecl> DeferredDecls;



std::vector<GlobalDecl> DeferredDeclsToEmit;
void addDeferredDeclToEmit(GlobalDecl GD) {
DeferredDeclsToEmit.emplace_back(GD);
}



std::vector<GlobalDecl> Aliases;



std::vector<GlobalDecl> MultiVersionFuncs;

typedef llvm::StringMap<llvm::TrackingVH<llvm::Constant> > ReplacementsTy;
ReplacementsTy Replacements;




llvm::SmallVector<std::pair<llvm::GlobalValue *, llvm::Constant *>, 8>
GlobalValReplacements;



llvm::DenseMap<const VarDecl*, llvm::GlobalVariable*> InitializerConstants;




llvm::DenseSet<GlobalDecl> DiagnosedConflictingDefinitions;


std::vector<const CXXRecordDecl*> DeferredVTables;


std::vector<const CXXRecordDecl *> OpportunisticVTables;




std::vector<llvm::WeakTrackingVH> LLVMUsed;
std::vector<llvm::WeakTrackingVH> LLVMCompilerUsed;



CtorList GlobalCtors;



CtorList GlobalDtors;


llvm::MapVector<GlobalDecl, StringRef> MangledDeclNames;
llvm::StringMap<GlobalDecl, llvm::BumpPtrAllocator> Manglings;



llvm::MapVector<std::pair<GlobalDecl, unsigned>, StringRef>
CPUSpecificMangledDeclNames;
llvm::StringMap<std::pair<GlobalDecl, unsigned>, llvm::BumpPtrAllocator>
CPUSpecificManglings;


std::vector<llvm::Constant*> Annotations;


llvm::StringMap<llvm::Constant*> AnnotationStrings;


llvm::DenseMap<unsigned, llvm::Constant *> AnnotationArgs;

llvm::StringMap<llvm::GlobalVariable *> CFConstantStringMap;

llvm::DenseMap<llvm::Constant *, llvm::GlobalVariable *> ConstantStringMap;
llvm::DenseMap<const Decl*, llvm::Constant *> StaticLocalDeclMap;
llvm::DenseMap<const Decl*, llvm::GlobalVariable*> StaticLocalDeclGuardMap;
llvm::DenseMap<const Expr*, llvm::Constant *> MaterializedGlobalTemporaryMap;

llvm::DenseMap<QualType, llvm::Constant *> AtomicSetterHelperFnMap;
llvm::DenseMap<QualType, llvm::Constant *> AtomicGetterHelperFnMap;


llvm::DenseMap<QualType, llvm::Constant *> TypeDescriptorMap;



typedef llvm::MapVector<IdentifierInfo *,
llvm::GlobalValue *> StaticExternCMap;
StaticExternCMap StaticExternCValues;


std::vector<const VarDecl *> CXXThreadLocals;



std::vector<llvm::Function *> CXXThreadLocalInits;
std::vector<const VarDecl *> CXXThreadLocalInitVars;


std::vector<llvm::Function *> CXXGlobalInits;






llvm::DenseMap<const Decl*, unsigned> DelayedCXXInitPosition;

typedef std::pair<OrderGlobalInitsOrStermFinalizers, llvm::Function *>
GlobalInitData;

struct GlobalInitPriorityCmp {
bool operator()(const GlobalInitData &LHS,
const GlobalInitData &RHS) const {
return LHS.first.priority < RHS.first.priority;
}
};



SmallVector<GlobalInitData, 8> PrioritizedCXXGlobalInits;




typedef std::tuple<llvm::FunctionType *, llvm::WeakTrackingVH,
llvm::Constant *>
CXXGlobalDtorsOrStermFinalizer_t;
SmallVector<CXXGlobalDtorsOrStermFinalizer_t, 8>
CXXGlobalDtorsOrStermFinalizers;

typedef std::pair<OrderGlobalInitsOrStermFinalizers, llvm::Function *>
StermFinalizerData;

struct StermFinalizerPriorityCmp {
bool operator()(const StermFinalizerData &LHS,
const StermFinalizerData &RHS) const {
return LHS.first.priority < RHS.first.priority;
}
};



SmallVector<StermFinalizerData, 8> PrioritizedCXXStermFinalizers;


llvm::SetVector<clang::Module *> ImportedModules;



llvm::SmallPtrSet<clang::Module *, 16> EmittedModuleInitializers;


SmallVector<llvm::MDNode *, 16> LinkerOptionsMetadata;


SmallVector<llvm::MDNode *, 16> ELFDependentLibraries;






llvm::WeakTrackingVH CFConstantStringClassRef;



QualType ObjCFastEnumerationStateType;




void createObjCRuntime();

void createOpenCLRuntime();
void createOpenMPRuntime();
void createCUDARuntime();

bool isTriviallyRecursive(const FunctionDecl *F);
bool shouldEmitFunction(GlobalDecl GD);
bool shouldOpportunisticallyEmitVTables();

llvm::DenseMap<const CompoundLiteralExpr *, llvm::GlobalVariable *>
EmittedCompoundLiterals;



llvm::DenseMap<const BlockExpr *, llvm::Constant *> EmittedGlobalBlocks;




llvm::Constant *NSConcreteGlobalBlock = nullptr;
llvm::Constant *NSConcreteStackBlock = nullptr;

llvm::FunctionCallee BlockObjectAssign = nullptr;
llvm::FunctionCallee BlockObjectDispose = nullptr;

llvm::Type *BlockDescriptorType = nullptr;
llvm::Type *GenericBlockLiteralType = nullptr;

struct {
int GlobalUniqueCount;
} Block;

GlobalDecl initializedGlobalDecl;




llvm::Function *LifetimeStartFn = nullptr;


llvm::Function *LifetimeEndFn = nullptr;

std::unique_ptr<SanitizerMetadata> SanitizerMD;

llvm::MapVector<const Decl *, bool> DeferredEmptyCoverageMappingDecls;

std::unique_ptr<CoverageMappingModuleGen> CoverageMapping;




typedef llvm::DenseMap<QualType, llvm::Metadata *> MetadataTypeMap;
MetadataTypeMap MetadataIdMap;
MetadataTypeMap VirtualMetadataIdMap;
MetadataTypeMap GeneralizedMetadataIdMap;

public:
CodeGenModule(ASTContext &C, const HeaderSearchOptions &headersearchopts,
const PreprocessorOptions &ppopts,
const CodeGenOptions &CodeGenOpts, llvm::Module &M,
DiagnosticsEngine &Diags,
CoverageSourceInfo *CoverageInfo = nullptr);

~CodeGenModule();

void clear();


void Release();


bool getExpressionLocationsEnabled() const;


CGObjCRuntime &getObjCRuntime() {
if (!ObjCRuntime) createObjCRuntime();
return *ObjCRuntime;
}


bool hasObjCRuntime() { return !!ObjCRuntime; }

const std::string &getModuleNameHash() const { return ModuleNameHash; }


CGOpenCLRuntime &getOpenCLRuntime() {
assert(OpenCLRuntime != nullptr);
return *OpenCLRuntime;
}


CGOpenMPRuntime &getOpenMPRuntime() {
assert(OpenMPRuntime != nullptr);
return *OpenMPRuntime;
}


CGCUDARuntime &getCUDARuntime() {
assert(CUDARuntime != nullptr);
return *CUDARuntime;
}

ObjCEntrypoints &getObjCEntrypoints() const {
assert(ObjCData != nullptr);
return *ObjCData;
}



llvm::FunctionCallee IsOSVersionAtLeastFn = nullptr;

llvm::FunctionCallee IsPlatformVersionAtLeastFn = nullptr;

InstrProfStats &getPGOStats() { return PGOStats; }
llvm::IndexedInstrProfReader *getPGOReader() const { return PGOReader.get(); }

CoverageMappingModuleGen *getCoverageMapping() const {
return CoverageMapping.get();
}

llvm::Constant *getStaticLocalDeclAddress(const VarDecl *D) {
return StaticLocalDeclMap[D];
}
void setStaticLocalDeclAddress(const VarDecl *D,
llvm::Constant *C) {
StaticLocalDeclMap[D] = C;
}

llvm::Constant *
getOrCreateStaticVarDecl(const VarDecl &D,
llvm::GlobalValue::LinkageTypes Linkage);

llvm::GlobalVariable *getStaticLocalDeclGuardAddress(const VarDecl *D) {
return StaticLocalDeclGuardMap[D];
}
void setStaticLocalDeclGuardAddress(const VarDecl *D,
llvm::GlobalVariable *C) {
StaticLocalDeclGuardMap[D] = C;
}

Address createUnnamedGlobalFrom(const VarDecl &D, llvm::Constant *Constant,
CharUnits Align);

bool lookupRepresentativeDecl(StringRef MangledName,
GlobalDecl &Result) const;

llvm::Constant *getAtomicSetterHelperFnMap(QualType Ty) {
return AtomicSetterHelperFnMap[Ty];
}
void setAtomicSetterHelperFnMap(QualType Ty,
llvm::Constant *Fn) {
AtomicSetterHelperFnMap[Ty] = Fn;
}

llvm::Constant *getAtomicGetterHelperFnMap(QualType Ty) {
return AtomicGetterHelperFnMap[Ty];
}
void setAtomicGetterHelperFnMap(QualType Ty,
llvm::Constant *Fn) {
AtomicGetterHelperFnMap[Ty] = Fn;
}

llvm::Constant *getTypeDescriptorFromMap(QualType Ty) {
return TypeDescriptorMap[Ty];
}
void setTypeDescriptorInMap(QualType Ty, llvm::Constant *C) {
TypeDescriptorMap[Ty] = C;
}

CGDebugInfo *getModuleDebugInfo() { return DebugInfo.get(); }

llvm::MDNode *getNoObjCARCExceptionsMetadata() {
if (!NoObjCARCExceptionsMetadata)
NoObjCARCExceptionsMetadata = llvm::MDNode::get(getLLVMContext(), None);
return NoObjCARCExceptionsMetadata;
}

ASTContext &getContext() const { return Context; }
const LangOptions &getLangOpts() const { return LangOpts; }
const HeaderSearchOptions &getHeaderSearchOpts()
const { return HeaderSearchOpts; }
const PreprocessorOptions &getPreprocessorOpts()
const { return PreprocessorOpts; }
const CodeGenOptions &getCodeGenOpts() const { return CodeGenOpts; }
llvm::Module &getModule() const { return TheModule; }
DiagnosticsEngine &getDiags() const { return Diags; }
const llvm::DataLayout &getDataLayout() const {
return TheModule.getDataLayout();
}
const TargetInfo &getTarget() const { return Target; }
const llvm::Triple &getTriple() const { return Target.getTriple(); }
bool supportsCOMDAT() const;
void maybeSetTrivialComdat(const Decl &D, llvm::GlobalObject &GO);

CGCXXABI &getCXXABI() const { return *ABI; }
llvm::LLVMContext &getLLVMContext() { return VMContext; }

bool shouldUseTBAA() const { return TBAA != nullptr; }

const TargetCodeGenInfo &getTargetCodeGenInfo();

CodeGenTypes &getTypes() { return Types; }

CodeGenVTables &getVTables() { return VTables; }

ItaniumVTableContext &getItaniumVTableContext() {
return VTables.getItaniumVTableContext();
}

MicrosoftVTableContext &getMicrosoftVTableContext() {
return VTables.getMicrosoftVTableContext();
}

CtorList &getGlobalCtors() { return GlobalCtors; }
CtorList &getGlobalDtors() { return GlobalDtors; }



llvm::MDNode *getTBAATypeInfo(QualType QTy);



TBAAAccessInfo getTBAAAccessInfo(QualType AccessType);



TBAAAccessInfo getTBAAVTablePtrAccessInfo(llvm::Type *VTablePtrType);

llvm::MDNode *getTBAAStructInfo(QualType QTy);



llvm::MDNode *getTBAABaseTypeInfo(QualType QTy);


llvm::MDNode *getTBAAAccessTagInfo(TBAAAccessInfo Info);



TBAAAccessInfo mergeTBAAInfoForCast(TBAAAccessInfo SourceInfo,
TBAAAccessInfo TargetInfo);



TBAAAccessInfo mergeTBAAInfoForConditionalOperator(TBAAAccessInfo InfoA,
TBAAAccessInfo InfoB);



TBAAAccessInfo mergeTBAAInfoForMemoryTransfer(TBAAAccessInfo DestInfo,
TBAAAccessInfo SrcInfo);



TBAAAccessInfo getTBAAInfoForSubobject(LValue Base, QualType AccessType) {
if (Base.getTBAAInfo().isMayAlias())
return TBAAAccessInfo::getMayAliasInfo();
return getTBAAAccessInfo(AccessType);
}

bool isTypeConstant(QualType QTy, bool ExcludeCtorDtor);

bool isPaddedAtomicType(QualType type);
bool isPaddedAtomicType(const AtomicType *type);


void DecorateInstructionWithTBAA(llvm::Instruction *Inst,
TBAAAccessInfo TBAAInfo);


void DecorateInstructionWithInvariantGroup(llvm::Instruction *I,
const CXXRecordDecl *RD);


llvm::ConstantInt *getSize(CharUnits numChars);


void setGlobalVisibility(llvm::GlobalValue *GV, const NamedDecl *D) const;

void setDSOLocal(llvm::GlobalValue *GV) const;

void setDLLImportDLLExport(llvm::GlobalValue *GV, GlobalDecl D) const;
void setDLLImportDLLExport(llvm::GlobalValue *GV, const NamedDecl *D) const;


void setGVProperties(llvm::GlobalValue *GV, GlobalDecl GD) const;
void setGVProperties(llvm::GlobalValue *GV, const NamedDecl *D) const;

void setGVPropertiesAux(llvm::GlobalValue *GV, const NamedDecl *D) const;



void setTLSMode(llvm::GlobalValue *GV, const VarDecl &D) const;


llvm::GlobalVariable::ThreadLocalMode GetDefaultLLVMTLSModel() const;

static llvm::GlobalValue::VisibilityTypes GetLLVMVisibility(Visibility V) {
switch (V) {
case DefaultVisibility: return llvm::GlobalValue::DefaultVisibility;
case HiddenVisibility: return llvm::GlobalValue::HiddenVisibility;
case ProtectedVisibility: return llvm::GlobalValue::ProtectedVisibility;
}
llvm_unreachable("unknown visibility!");
}

llvm::Constant *GetAddrOfGlobal(GlobalDecl GD,
ForDefinition_t IsForDefinition
= NotForDefinition);





llvm::GlobalVariable *
CreateOrReplaceCXXRuntimeVariable(StringRef Name, llvm::Type *Ty,
llvm::GlobalValue::LinkageTypes Linkage,
unsigned Alignment);

llvm::Function *CreateGlobalInitOrCleanUpFunction(
llvm::FunctionType *ty, const Twine &name, const CGFunctionInfo &FI,
SourceLocation Loc = SourceLocation(), bool TLS = false);









LangAS GetGlobalVarAddressSpace(const VarDecl *D);






LangAS GetGlobalConstantAddressSpace() const;







llvm::Constant *GetAddrOfGlobalVar(const VarDecl *D,
llvm::Type *Ty = nullptr,
ForDefinition_t IsForDefinition
= NotForDefinition);



llvm::Constant *GetAddrOfFunction(GlobalDecl GD, llvm::Type *Ty = nullptr,
bool ForVTable = false,
bool DontDefer = false,
ForDefinition_t IsForDefinition
= NotForDefinition);


llvm::Constant *GetAddrOfRTTIDescriptor(QualType Ty, bool ForEH = false);


ConstantAddress GetAddrOfMSGuidDecl(const MSGuidDecl *GD);


ConstantAddress
GetAddrOfTemplateParamObject(const TemplateParamObjectDecl *TPO);


llvm::Constant *GetAddrOfThunk(StringRef Name, llvm::Type *FnTy,
GlobalDecl GD);


ConstantAddress GetWeakRefReference(const ValueDecl *VD);


CharUnits getClassPointerAlignment(const CXXRecordDecl *CD);



CharUnits getMinimumClassObjectSize(const CXXRecordDecl *CD);


CharUnits getMinimumObjectSize(QualType Ty) {
if (CXXRecordDecl *RD = Ty->getAsCXXRecordDecl())
return getMinimumClassObjectSize(RD);
return getContext().getTypeSizeInChars(Ty);
}


CharUnits getVBaseAlignment(CharUnits DerivedAlign,
const CXXRecordDecl *Derived,
const CXXRecordDecl *VBase);




CharUnits getDynamicOffsetAlignment(CharUnits ActualAlign,
const CXXRecordDecl *Class,
CharUnits ExpectedTargetAlign);

CharUnits
computeNonVirtualBaseClassOffset(const CXXRecordDecl *DerivedClass,
CastExpr::path_const_iterator Start,
CastExpr::path_const_iterator End);



llvm::Constant *
GetNonVirtualBaseClassOffset(const CXXRecordDecl *ClassDecl,
CastExpr::path_const_iterator PathBegin,
CastExpr::path_const_iterator PathEnd);

llvm::FoldingSet<BlockByrefHelpers> ByrefHelpersCache;


int getUniqueBlockCount() { return ++Block.GlobalUniqueCount; }


llvm::Type *getBlockDescriptorType();


llvm::Type *getGenericBlockLiteralType();


llvm::Constant *GetAddrOfGlobalBlock(const BlockExpr *BE, StringRef Name);



llvm::Constant *getAddrOfGlobalBlockIfEmitted(const BlockExpr *BE) {
return EmittedGlobalBlocks.lookup(BE);
}



void setAddrOfGlobalBlock(const BlockExpr *BE, llvm::Constant *Addr);


ConstantAddress GetAddrOfConstantCFString(const StringLiteral *Literal);




ConstantAddress GetAddrOfConstantString(const StringLiteral *Literal);


llvm::Constant *GetConstantArrayFromStringLiteral(const StringLiteral *E);


ConstantAddress
GetAddrOfConstantStringFromLiteral(const StringLiteral *S,
StringRef Name = ".str");


ConstantAddress
GetAddrOfConstantStringFromObjCEncode(const ObjCEncodeExpr *);






ConstantAddress
GetAddrOfConstantCString(const std::string &Str,
const char *GlobalName = nullptr);



ConstantAddress GetAddrOfConstantCompoundLiteral(const CompoundLiteralExpr*E);



llvm::GlobalVariable *
getAddrOfConstantCompoundLiteralIfEmitted(const CompoundLiteralExpr *E);



void setAddrOfConstantCompoundLiteral(const CompoundLiteralExpr *CLE,
llvm::GlobalVariable *GV);



ConstantAddress GetAddrOfGlobalTemporary(const MaterializeTemporaryExpr *E,
const Expr *Inner);



QualType getObjCFastEnumerationStateType();




llvm::Function *codegenCXXStructor(GlobalDecl GD);


llvm::Constant *
getAddrOfCXXStructor(GlobalDecl GD, const CGFunctionInfo *FnInfo = nullptr,
llvm::FunctionType *FnType = nullptr,
bool DontDefer = false,
ForDefinition_t IsForDefinition = NotForDefinition) {
return cast<llvm::Constant>(getAddrAndTypeOfCXXStructor(GD, FnInfo, FnType,
DontDefer,
IsForDefinition)
.getCallee());
}

llvm::FunctionCallee getAddrAndTypeOfCXXStructor(
GlobalDecl GD, const CGFunctionInfo *FnInfo = nullptr,
llvm::FunctionType *FnType = nullptr, bool DontDefer = false,
ForDefinition_t IsForDefinition = NotForDefinition);



llvm::Constant *getBuiltinLibFunction(const FunctionDecl *FD,
unsigned BuiltinID);

llvm::Function *getIntrinsic(unsigned IID, ArrayRef<llvm::Type*> Tys = None);


void EmitTopLevelDecl(Decl *D);



void AddDeferredUnusedCoverageMapping(Decl *D);



void ClearUnusedCoverageMapping(const Decl *D);



void EmitDeferredUnusedCoverageMappings();


void EmitMainVoidAlias();


void HandleCXXStaticMemberVarInstantiation(VarDecl *VD);




template<typename SomeDecl>
void MaybeHandleStaticInExternC(const SomeDecl *D, llvm::GlobalValue *GV);


void addUsedGlobal(llvm::GlobalValue *GV);


void addCompilerUsedGlobal(llvm::GlobalValue *GV);


void addUsedOrCompilerUsedGlobal(llvm::GlobalValue *GV);


void AddCXXDtorEntry(llvm::FunctionCallee DtorFn, llvm::Constant *Object) {
CXXGlobalDtorsOrStermFinalizers.emplace_back(DtorFn.getFunctionType(),
DtorFn.getCallee(), Object);
}


void AddCXXStermFinalizerEntry(llvm::FunctionCallee DtorFn) {
CXXGlobalDtorsOrStermFinalizers.emplace_back(DtorFn.getFunctionType(),
DtorFn.getCallee(), nullptr);
}


void AddCXXStermFinalizerToGlobalDtor(llvm::Function *StermFinalizer,
int Priority) {
AddGlobalDtor(StermFinalizer, Priority);
}

void AddCXXPrioritizedStermFinalizerEntry(llvm::Function *StermFinalizer,
int Priority) {
OrderGlobalInitsOrStermFinalizers Key(Priority,
PrioritizedCXXStermFinalizers.size());
PrioritizedCXXStermFinalizers.push_back(
std::make_pair(Key, StermFinalizer));
}




llvm::FunctionCallee
CreateRuntimeFunction(llvm::FunctionType *Ty, StringRef Name,
llvm::AttributeList ExtraAttrs = llvm::AttributeList(),
bool Local = false, bool AssumeConvergent = false);


llvm::Constant *CreateRuntimeVariable(llvm::Type *Ty,
StringRef Name);




llvm::Constant *getNSConcreteGlobalBlock();
llvm::Constant *getNSConcreteStackBlock();
llvm::FunctionCallee getBlockObjectAssign();
llvm::FunctionCallee getBlockObjectDispose();



llvm::Function *getLLVMLifetimeStartFn();
llvm::Function *getLLVMLifetimeEndFn();


void UpdateCompletedType(const TagDecl *TD);

llvm::Constant *getMemberPointerConstant(const UnaryOperator *e);



void EmitExplicitCastExprType(const ExplicitCastExpr *E,
CodeGenFunction *CGF = nullptr);




llvm::Constant *EmitNullConstant(QualType T);



llvm::Constant *EmitNullConstantForBase(const CXXRecordDecl *Record);


void Error(SourceLocation loc, StringRef error);


void ErrorUnsupported(const Stmt *S, const char *Type);


void ErrorUnsupported(const Decl *D, const char *Type);




void SetInternalFunctionAttributes(GlobalDecl GD, llvm::Function *F,
const CGFunctionInfo &FI);


void SetLLVMFunctionAttributes(GlobalDecl GD, const CGFunctionInfo &Info,
llvm::Function *F, bool IsThunk);



void SetLLVMFunctionAttributesForDefinition(const Decl *D, llvm::Function *F);



void setLLVMFunctionFEnvAttributes(const FunctionDecl *D, llvm::Function *F);


bool ReturnTypeUsesSRet(const CGFunctionInfo &FI);



bool ReturnSlotInterferesWithArgs(const CGFunctionInfo &FI);


bool ReturnTypeUsesFPRet(QualType ResultType);


bool ReturnTypeUsesFP2Ret(QualType ResultType);











void ConstructAttributeList(StringRef Name, const CGFunctionInfo &Info,
CGCalleeInfo CalleeInfo,
llvm::AttributeList &Attrs, unsigned &CallingConv,
bool AttrOnCallSite, bool IsThunk);


















void addDefaultFunctionDefinitionAttributes(llvm::Function &F);



void addDefaultFunctionDefinitionAttributes(llvm::AttrBuilder &attrs);

StringRef getMangledName(GlobalDecl GD);
StringRef getBlockMangledName(GlobalDecl GD, const BlockDecl *BD);

void EmitTentativeDefinition(const VarDecl *D);

void EmitExternalDeclaration(const VarDecl *D);

void EmitVTable(CXXRecordDecl *Class);

void RefreshTypeCacheForClass(const CXXRecordDecl *Class);


void AppendLinkerOptions(StringRef Opts);


void AddDetectMismatch(StringRef Name, StringRef Value);


void AddDependentLib(StringRef Lib);


llvm::GlobalVariable::LinkageTypes getFunctionLinkage(GlobalDecl GD);

void setFunctionLinkage(GlobalDecl GD, llvm::Function *F) {
F->setLinkage(getFunctionLinkage(GD));
}



llvm::GlobalVariable::LinkageTypes getVTableLinkage(const CXXRecordDecl *RD);


CharUnits GetTargetTypeStoreSize(llvm::Type *Ty) const;


llvm::GlobalValue::LinkageTypes
getLLVMLinkageForDeclarator(const DeclaratorDecl *D, GVALinkage Linkage,
bool IsConstantVariable);


llvm::GlobalValue::LinkageTypes
getLLVMLinkageVarDefinition(const VarDecl *VD, bool IsConstant);


void EmitGlobalAnnotations();


llvm::Constant *EmitAnnotationString(StringRef Str);


llvm::Constant *EmitAnnotationUnit(SourceLocation Loc);


llvm::Constant *EmitAnnotationLineNo(SourceLocation L);


llvm::Constant *EmitAnnotationArgs(const AnnotateAttr *Attr);








llvm::Constant *EmitAnnotateAttr(llvm::GlobalValue *GV,
const AnnotateAttr *AA,
SourceLocation L);



void AddGlobalAnnotations(const ValueDecl *D, llvm::GlobalValue *GV);

bool isInNoSanitizeList(SanitizerMask Kind, llvm::Function *Fn,
SourceLocation Loc) const;

bool isInNoSanitizeList(llvm::GlobalVariable *GV, SourceLocation Loc,
QualType Ty, StringRef Category = StringRef()) const;




bool imbueXRayAttrs(llvm::Function *Fn, SourceLocation Loc,
StringRef Category = StringRef()) const;



bool isProfileInstrExcluded(llvm::Function *Fn, SourceLocation Loc) const;

SanitizerMetadata *getSanitizerMetadata() {
return SanitizerMD.get();
}

void addDeferredVTable(const CXXRecordDecl *RD) {
DeferredVTables.push_back(RD);
}



void EmitGlobal(GlobalDecl D);

bool TryEmitBaseDestructorAsAlias(const CXXDestructorDecl *D);

llvm::GlobalValue *GetGlobalValue(StringRef Ref);





void SetCommonAttributes(GlobalDecl GD, llvm::GlobalValue *GV);

void addReplacement(StringRef Name, llvm::Constant *C);

void addGlobalValReplacement(llvm::GlobalValue *GV, llvm::Constant *C);



void EmitOMPThreadPrivateDecl(const OMPThreadPrivateDecl *D);


void EmitOMPDeclareReduction(const OMPDeclareReductionDecl *D,
CodeGenFunction *CGF = nullptr);


void EmitOMPDeclareMapper(const OMPDeclareMapperDecl *D,
CodeGenFunction *CGF = nullptr);



void EmitOMPRequiresDecl(const OMPRequiresDecl *D);



void EmitOMPAllocateDecl(const OMPAllocateDecl *D);




bool HasHiddenLTOVisibility(const CXXRecordDecl *RD);




bool HasLTOVisibilityPublicStd(const CXXRecordDecl *RD);







llvm::GlobalObject::VCallVisibility
GetVCallVisibilityLevel(const CXXRecordDecl *RD,
llvm::DenseSet<const CXXRecordDecl *> &Visited);


void EmitVTableTypeMetadata(const CXXRecordDecl *RD,
llvm::GlobalVariable *VTable,
const VTableLayout &VTLayout);


llvm::ConstantInt *CreateCrossDsoCfiTypeId(llvm::Metadata *MD);




llvm::Metadata *CreateMetadataIdentifierForType(QualType T);



llvm::Metadata *CreateMetadataIdentifierForVirtualMemPtrType(QualType T);




llvm::Metadata *CreateMetadataIdentifierGeneralized(QualType T);


void CreateFunctionTypeMetadataForIcall(const FunctionDecl *FD,
llvm::Function *F);



bool MayDropFunctionReturn(const ASTContext &Context, QualType ReturnType);


bool NeedAllVtablesTypeId() const;


void AddVTableTypeMetadata(llvm::GlobalVariable *VTable, CharUnits Offset,
const CXXRecordDecl *RD);






std::vector<const CXXRecordDecl *>
getMostBaseClasses(const CXXRecordDecl *RD);


llvm::FunctionCallee getTerminateFn();

llvm::SanitizerStatReport &getSanStats();

llvm::Value *
createOpenCLIntToSamplerConversion(const Expr *E, CodeGenFunction &CGF);











void GenOpenCLArgMetadata(llvm::Function *FN,
const FunctionDecl *FD = nullptr,
CodeGenFunction *CGF = nullptr);




llvm::Constant *getNullPointer(llvm::PointerType *T, QualType QT);

CharUnits getNaturalTypeAlignment(QualType T,
LValueBaseInfo *BaseInfo = nullptr,
TBAAAccessInfo *TBAAInfo = nullptr,
bool forPointeeType = false);
CharUnits getNaturalPointeeTypeAlignment(QualType T,
LValueBaseInfo *BaseInfo = nullptr,
TBAAAccessInfo *TBAAInfo = nullptr);
bool stopAutoInit();



void printPostfixForExternalizedStaticVar(llvm::raw_ostream &OS) const;

private:
llvm::Constant *GetOrCreateLLVMFunction(
StringRef MangledName, llvm::Type *Ty, GlobalDecl D, bool ForVTable,
bool DontDefer = false, bool IsThunk = false,
llvm::AttributeList ExtraAttrs = llvm::AttributeList(),
ForDefinition_t IsForDefinition = NotForDefinition);

llvm::Constant *GetOrCreateMultiVersionResolver(GlobalDecl GD,
llvm::Type *DeclTy,
const FunctionDecl *FD);
void UpdateMultiVersionNames(GlobalDecl GD, const FunctionDecl *FD);

llvm::Constant *
GetOrCreateLLVMGlobal(StringRef MangledName, llvm::Type *Ty,
unsigned AddrSpace, const VarDecl *D,
ForDefinition_t IsForDefinition = NotForDefinition);

bool GetCPUAndFeaturesAttributes(GlobalDecl GD,
llvm::AttrBuilder &AttrBuilder);
void setNonAliasAttributes(GlobalDecl GD, llvm::GlobalObject *GO);


void SetFunctionAttributes(GlobalDecl GD, llvm::Function *F,
bool IsIncompleteFunction, bool IsThunk);

void EmitGlobalDefinition(GlobalDecl D, llvm::GlobalValue *GV = nullptr);

void EmitGlobalFunctionDefinition(GlobalDecl GD, llvm::GlobalValue *GV);
void EmitMultiVersionFunctionDefinition(GlobalDecl GD, llvm::GlobalValue *GV);

void EmitGlobalVarDefinition(const VarDecl *D, bool IsTentative = false);
void EmitExternalVarDeclaration(const VarDecl *D);
void EmitAliasDefinition(GlobalDecl GD);
void emitIFuncDefinition(GlobalDecl GD);
void emitCPUDispatchDefinition(GlobalDecl GD);
void EmitObjCPropertyImplementations(const ObjCImplementationDecl *D);
void EmitObjCIvarInitializations(ObjCImplementationDecl *D);



void EmitDeclContext(const DeclContext *DC);
void EmitLinkageSpec(const LinkageSpecDecl *D);


void EmitCXXThreadLocalInitFunc();


void EmitCXXGlobalInitFunc();


void EmitCXXGlobalCleanUpFunc();



void EmitCXXGlobalVarDeclInitFunc(const VarDecl *D,
llvm::GlobalVariable *Addr,
bool PerformInit);

void EmitPointerToInitFunc(const VarDecl *VD, llvm::GlobalVariable *Addr,
llvm::Function *InitFunc, InitSegAttr *ISA);


void AddGlobalCtor(llvm::Function *Ctor, int Priority = 65535,
llvm::Constant *AssociatedData = nullptr);
void AddGlobalDtor(llvm::Function *Dtor, int Priority = 65535,
bool IsDtorAttrFunc = false);




void EmitCtorList(CtorList &Fns, const char *GlobalName);


void EmitDeferred();





void EmitVTablesOpportunistically();


void applyReplacements();


void applyGlobalValReplacements();

void checkAliases();

std::map<int, llvm::TinyPtrVector<llvm::Function *>> DtorsUsingAtExit;



void registerGlobalDtorsWithAtExit();




void unregisterGlobalDtorsWithUnAtExit();

void emitMultiVersionFunctions();


void EmitDeferredVTables();



void emitAtAvailableLinkGuard();


void emitLLVMUsed();


void EmitModuleLinkOptions();



void EmitStaticExternCAliases();

void EmitDeclMetadata();


void EmitVersionIdentMetadata();


void EmitCommandLineMetadata();



void EmitBackendOptionsMetadata(const CodeGenOptions CodeGenOpts);


void EmitOpenCLMetadata();



void EmitCoverageFile();



bool MustBeEmitted(const ValueDecl *D);





bool MayBeEmittedEagerly(const ValueDecl *D);



void SimplifyPersonality();




void getDefaultFunctionAttributes(StringRef Name, bool HasOptnone,
bool AttrOnCallSite,
llvm::AttrBuilder &FuncAttrs);

llvm::Metadata *CreateMetadataIdentifierImpl(QualType T, MetadataTypeMap &Map,
StringRef Suffix);
};

}
}

#endif
