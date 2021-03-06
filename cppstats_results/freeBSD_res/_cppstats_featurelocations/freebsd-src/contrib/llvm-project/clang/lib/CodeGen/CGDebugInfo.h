











#if !defined(LLVM_CLANG_LIB_CODEGEN_CGDEBUGINFO_H)
#define LLVM_CLANG_LIB_CODEGEN_CGDEBUGINFO_H

#include "CGBuilder.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeOrdering.h"
#include "clang/Basic/CodeGenOptions.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/ValueHandle.h"
#include "llvm/Support/Allocator.h"

namespace llvm {
class MDNode;
}

namespace clang {
class ClassTemplateSpecializationDecl;
class GlobalDecl;
class ModuleMap;
class ObjCInterfaceDecl;
class ObjCIvarDecl;
class UsingDecl;
class VarDecl;
enum class DynamicInitKind : unsigned;

namespace CodeGen {
class CodeGenModule;
class CodeGenFunction;
class CGBlockInfo;




class CGDebugInfo {
friend class ApplyDebugLocation;
friend class SaveAndRestoreLocation;
CodeGenModule &CGM;
const codegenoptions::DebugInfoKind DebugKind;
bool DebugTypeExtRefs;
llvm::DIBuilder DBuilder;
llvm::DICompileUnit *TheCU = nullptr;
ModuleMap *ClangModuleMap = nullptr;
ASTSourceDescriptor PCHDescriptor;
SourceLocation CurLoc;
llvm::MDNode *CurInlinedAt = nullptr;
llvm::DIType *VTablePtrType = nullptr;
llvm::DIType *ClassTy = nullptr;
llvm::DICompositeType *ObjTy = nullptr;
llvm::DIType *SelTy = nullptr;
#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) llvm::DIType *SingletonId = nullptr;

#include "clang/Basic/OpenCLImageTypes.def"
llvm::DIType *OCLSamplerDITy = nullptr;
llvm::DIType *OCLEventDITy = nullptr;
llvm::DIType *OCLClkEventDITy = nullptr;
llvm::DIType *OCLQueueDITy = nullptr;
llvm::DIType *OCLNDRangeDITy = nullptr;
llvm::DIType *OCLReserveIDDITy = nullptr;
#define EXT_OPAQUE_TYPE(ExtType, Id, Ext) llvm::DIType *Id##Ty = nullptr;

#include "clang/Basic/OpenCLExtensionTypes.def"


llvm::DenseMap<const void *, llvm::TrackingMDRef> TypeCache;

std::map<llvm::StringRef, llvm::StringRef, std::greater<llvm::StringRef>>
DebugPrefixMap;



llvm::SmallDenseMap<QualType, llvm::Metadata *> SizeExprCache;


class PrintingCallbacks final : public clang::PrintingCallbacks {
const CGDebugInfo &Self;

public:
PrintingCallbacks(const CGDebugInfo &Self) : Self(Self) {}
std::string remapPath(StringRef Path) const override {
return Self.remapDIPath(Path);
}
};
PrintingCallbacks PrintCB = {*this};

struct ObjCInterfaceCacheEntry {
const ObjCInterfaceType *Type;
llvm::DIType *Decl;
llvm::DIFile *Unit;
ObjCInterfaceCacheEntry(const ObjCInterfaceType *Type, llvm::DIType *Decl,
llvm::DIFile *Unit)
: Type(Type), Decl(Decl), Unit(Unit) {}
};


llvm::SmallVector<ObjCInterfaceCacheEntry, 32> ObjCInterfaceCache;




llvm::DenseMap<const ObjCInterfaceDecl *,
std::vector<llvm::PointerIntPair<llvm::DISubprogram *, 1>>>
ObjCMethodCache;


llvm::DenseMap<const Module *, llvm::TrackingMDRef> ModuleCache;


std::vector<void *> RetainedTypes;


std::vector<std::pair<const TagType *, llvm::TrackingMDRef>> ReplaceMap;



std::vector<std::pair<const DeclaratorDecl *, llvm::TrackingMDRef>>
FwdDeclReplaceMap;


std::vector<llvm::TypedTrackingMDRef<llvm::DIScope>> LexicalBlockStack;
llvm::DenseMap<const Decl *, llvm::TrackingMDRef> RegionMap;



std::vector<unsigned> FnBeginRegionCount;



llvm::BumpPtrAllocator DebugInfoNames;
StringRef CWDName;

llvm::DenseMap<const char *, llvm::TrackingMDRef> DIFileCache;
llvm::DenseMap<const FunctionDecl *, llvm::TrackingMDRef> SPCache;


llvm::DenseMap<const Decl *, llvm::TrackingMDRef> DeclCache;
llvm::DenseMap<const NamespaceDecl *, llvm::TrackingMDRef> NamespaceCache;
llvm::DenseMap<const NamespaceAliasDecl *, llvm::TrackingMDRef>
NamespaceAliasCache;
llvm::DenseMap<const Decl *, llvm::TypedTrackingMDRef<llvm::DIDerivedType>>
StaticDataMemberCache;

using ParamDecl2StmtTy = llvm::DenseMap<const ParmVarDecl *, const Stmt *>;
using Param2DILocTy =
llvm::DenseMap<const ParmVarDecl *, llvm::DILocalVariable *>;


ParamDecl2StmtTy CoroutineParameterMappings;

Param2DILocTy ParamDbgMappings;





llvm::DIType *CreateType(const BuiltinType *Ty);
llvm::DIType *CreateType(const ComplexType *Ty);
llvm::DIType *CreateType(const AutoType *Ty);
llvm::DIType *CreateType(const ExtIntType *Ty);
llvm::DIType *CreateQualifiedType(QualType Ty, llvm::DIFile *Fg);
llvm::DIType *CreateType(const TypedefType *Ty, llvm::DIFile *Fg);
llvm::DIType *CreateType(const TemplateSpecializationType *Ty,
llvm::DIFile *Fg);
llvm::DIType *CreateType(const ObjCObjectPointerType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const PointerType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const BlockPointerType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const FunctionType *Ty, llvm::DIFile *F);

llvm::DIType *CreateType(const RecordType *Tyg);
llvm::DIType *CreateTypeDefinition(const RecordType *Ty);
llvm::DICompositeType *CreateLimitedType(const RecordType *Ty);
void CollectContainingType(const CXXRecordDecl *RD,
llvm::DICompositeType *CT);

llvm::DIType *CreateType(const ObjCInterfaceType *Ty, llvm::DIFile *F);
llvm::DIType *CreateTypeDefinition(const ObjCInterfaceType *Ty,
llvm::DIFile *F);

llvm::DIType *CreateType(const ObjCObjectType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const ObjCTypeParamType *Ty, llvm::DIFile *Unit);

llvm::DIType *CreateType(const VectorType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const ConstantMatrixType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const ArrayType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const LValueReferenceType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const RValueReferenceType *Ty, llvm::DIFile *Unit);
llvm::DIType *CreateType(const MemberPointerType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const AtomicType *Ty, llvm::DIFile *F);
llvm::DIType *CreateType(const PipeType *Ty, llvm::DIFile *F);

llvm::DIType *CreateEnumType(const EnumType *Ty);
llvm::DIType *CreateTypeDefinition(const EnumType *Ty);






llvm::DIType *CreateSelfType(const QualType &QualTy, llvm::DIType *Ty);




llvm::DIType *getTypeOrNull(const QualType);




llvm::DISubroutineType *getOrCreateMethodType(const CXXMethodDecl *Method,
llvm::DIFile *F, bool decl);
llvm::DISubroutineType *
getOrCreateInstanceMethodType(QualType ThisPtr, const FunctionProtoType *Func,
llvm::DIFile *Unit, bool decl);
llvm::DISubroutineType *
getOrCreateFunctionType(const Decl *D, QualType FnType, llvm::DIFile *F);

llvm::DIType *getOrCreateVTablePtrType(llvm::DIFile *F);


llvm::DINamespace *getOrCreateNamespace(const NamespaceDecl *N);
llvm::DIType *CreatePointerLikeType(llvm::dwarf::Tag Tag, const Type *Ty,
QualType PointeeTy, llvm::DIFile *F);
llvm::DIType *getOrCreateStructPtrType(StringRef Name, llvm::DIType *&Cache);



llvm::DISubprogram *CreateCXXMemberFunction(const CXXMethodDecl *Method,
llvm::DIFile *F,
llvm::DIType *RecordTy);




void CollectCXXMemberFunctions(const CXXRecordDecl *Decl, llvm::DIFile *F,
SmallVectorImpl<llvm::Metadata *> &E,
llvm::DIType *T);




void CollectCXXBases(const CXXRecordDecl *Decl, llvm::DIFile *F,
SmallVectorImpl<llvm::Metadata *> &EltTys,
llvm::DIType *RecordTy);



void CollectCXXBasesAux(
const CXXRecordDecl *RD, llvm::DIFile *Unit,
SmallVectorImpl<llvm::Metadata *> &EltTys, llvm::DIType *RecordTy,
const CXXRecordDecl::base_class_const_range &Bases,
llvm::DenseSet<CanonicalDeclPtr<const CXXRecordDecl>> &SeenTypes,
llvm::DINode::DIFlags StartingFlags);


llvm::DINodeArray CollectTemplateParams(const TemplateParameterList *TPList,
ArrayRef<TemplateArgument> TAList,
llvm::DIFile *Unit);


llvm::DINodeArray CollectFunctionTemplateParams(const FunctionDecl *FD,
llvm::DIFile *Unit);



llvm::DINodeArray CollectVarTemplateParams(const VarDecl *VD,
llvm::DIFile *Unit);



llvm::DINodeArray
CollectCXXTemplateParams(const ClassTemplateSpecializationDecl *TS,
llvm::DIFile *F);

llvm::DIType *createFieldType(StringRef name, QualType type,
SourceLocation loc, AccessSpecifier AS,
uint64_t offsetInBits, uint32_t AlignInBits,
llvm::DIFile *tunit, llvm::DIScope *scope,
const RecordDecl *RD = nullptr);

llvm::DIType *createFieldType(StringRef name, QualType type,
SourceLocation loc, AccessSpecifier AS,
uint64_t offsetInBits, llvm::DIFile *tunit,
llvm::DIScope *scope,
const RecordDecl *RD = nullptr) {
return createFieldType(name, type, loc, AS, offsetInBits, 0, tunit, scope,
RD);
}


llvm::DIType *createBitFieldType(const FieldDecl *BitFieldDecl,
llvm::DIScope *RecordTy,
const RecordDecl *RD);



void CollectRecordLambdaFields(const CXXRecordDecl *CXXDecl,
SmallVectorImpl<llvm::Metadata *> &E,
llvm::DIType *RecordTy);
llvm::DIDerivedType *CreateRecordStaticField(const VarDecl *Var,
llvm::DIType *RecordTy,
const RecordDecl *RD);
void CollectRecordNormalField(const FieldDecl *Field, uint64_t OffsetInBits,
llvm::DIFile *F,
SmallVectorImpl<llvm::Metadata *> &E,
llvm::DIType *RecordTy, const RecordDecl *RD);
void CollectRecordNestedType(const TypeDecl *RD,
SmallVectorImpl<llvm::Metadata *> &E);
void CollectRecordFields(const RecordDecl *Decl, llvm::DIFile *F,
SmallVectorImpl<llvm::Metadata *> &E,
llvm::DICompositeType *RecordTy);



void CollectVTableInfo(const CXXRecordDecl *Decl, llvm::DIFile *F,
SmallVectorImpl<llvm::Metadata *> &EltTys);



void CreateLexicalBlock(SourceLocation Loc);







void AppendAddressSpaceXDeref(unsigned AddressSpace,
SmallVectorImpl<int64_t> &Expr) const;





uint64_t collectDefaultElementTypesForBlockPointer(
const BlockPointerType *Ty, llvm::DIFile *Unit,
llvm::DIDerivedType *DescTy, unsigned LineNo,
SmallVectorImpl<llvm::Metadata *> &EltTys);



void collectDefaultFieldsForBlockLiteralDeclare(
const CGBlockInfo &Block, const ASTContext &Context, SourceLocation Loc,
const llvm::StructLayout &BlockLayout, llvm::DIFile *Unit,
SmallVectorImpl<llvm::Metadata *> &Fields);

public:
CGDebugInfo(CodeGenModule &CGM);
~CGDebugInfo();

void finalize();


std::string remapDIPath(StringRef) const;


void registerVLASizeExpression(QualType Ty, llvm::Metadata *SizeExpr) {
SizeExprCache[Ty] = SizeExpr;
}




void setDwoId(uint64_t Signature);




void setModuleMap(ModuleMap &MMap) { ClangModuleMap = &MMap; }




void setPCHDescriptor(ASTSourceDescriptor PCH) { PCHDescriptor = PCH; }




void setLocation(SourceLocation Loc);



SourceLocation getLocation() const { return CurLoc; }



void setInlinedAt(llvm::MDNode *InlinedAt) { CurInlinedAt = InlinedAt; }


llvm::MDNode *getInlinedAt() const { return CurInlinedAt; }


llvm::DebugLoc SourceLocToDebugLoc(SourceLocation Loc);




void EmitLocation(CGBuilderTy &Builder, SourceLocation Loc);





void emitFunctionStart(GlobalDecl GD, SourceLocation Loc,
SourceLocation ScopeLoc, QualType FnType,
llvm::Function *Fn, bool CurFnIsThunk);


void EmitInlineFunctionStart(CGBuilderTy &Builder, GlobalDecl GD);

void EmitInlineFunctionEnd(CGBuilderTy &Builder);



void EmitFunctionDecl(GlobalDecl GD, SourceLocation Loc,
QualType FnType, llvm::Function *Fn = nullptr);



void EmitFuncDeclForCallSite(llvm::CallBase *CallOrInvoke,
QualType CalleeType,
const FunctionDecl *CalleeDecl);


void EmitFunctionEnd(CGBuilderTy &Builder, llvm::Function *Fn);



void EmitLexicalBlockStart(CGBuilderTy &Builder, SourceLocation Loc);



void EmitLexicalBlockEnd(CGBuilderTy &Builder, SourceLocation Loc);





llvm::DILocalVariable *
EmitDeclareOfAutoVariable(const VarDecl *Decl, llvm::Value *AI,
CGBuilderTy &Builder,
const bool UsePointerValue = false);


void EmitLabel(const LabelDecl *D, CGBuilderTy &Builder);



void EmitDeclareOfBlockDeclRefVariable(
const VarDecl *variable, llvm::Value *storage, CGBuilderTy &Builder,
const CGBlockInfo &blockInfo, llvm::Instruction *InsertPoint = nullptr);



llvm::DILocalVariable *EmitDeclareOfArgVariable(const VarDecl *Decl,
llvm::Value *AI,
unsigned ArgNo,
CGBuilderTy &Builder);



void EmitDeclareOfBlockLiteralArgVariable(const CGBlockInfo &block,
StringRef Name, unsigned ArgNo,
llvm::AllocaInst *LocalAddr,
CGBuilderTy &Builder);


void EmitGlobalVariable(llvm::GlobalVariable *GV, const VarDecl *Decl);


void EmitGlobalVariable(const ValueDecl *VD, const APValue &Init);


void EmitExternalVariable(llvm::GlobalVariable *GV, const VarDecl *Decl);


void EmitUsingDirective(const UsingDirectiveDecl &UD);


void EmitExplicitCastType(QualType Ty);


void EmitAndRetainType(QualType Ty);


void EmitUsingShadowDecl(const UsingShadowDecl &USD);


void EmitUsingDecl(const UsingDecl &UD);


void EmitUsingEnumDecl(const UsingEnumDecl &UD);


void EmitImportDecl(const ImportDecl &ID);


llvm::DIImportedEntity *EmitNamespaceAlias(const NamespaceAliasDecl &NA);


llvm::DIType *getOrCreateRecordType(QualType Ty, SourceLocation L);


llvm::DIType *getOrCreateInterfaceType(QualType Ty, SourceLocation Loc);


llvm::DIType *getOrCreateStandaloneType(QualType Ty, SourceLocation Loc);


void addHeapAllocSiteMetadata(llvm::CallBase *CallSite, QualType AllocatedTy,
SourceLocation Loc);

void completeType(const EnumDecl *ED);
void completeType(const RecordDecl *RD);
void completeRequiredType(const RecordDecl *RD);
void completeClassData(const RecordDecl *RD);
void completeClass(const RecordDecl *RD);

void completeTemplateDefinition(const ClassTemplateSpecializationDecl &SD);
void completeUnusedClass(const CXXRecordDecl &D);



llvm::DIMacro *CreateMacro(llvm::DIMacroFile *Parent, unsigned MType,
SourceLocation LineLoc, StringRef Name,
StringRef Value);


llvm::DIMacroFile *CreateTempMacroFile(llvm::DIMacroFile *Parent,
SourceLocation LineLoc,
SourceLocation FileLoc);

Param2DILocTy &getParamDbgMappings() { return ParamDbgMappings; }
ParamDecl2StmtTy &getCoroutineParameterMappings() {
return CoroutineParameterMappings;
}

private:



llvm::DILocalVariable *EmitDeclare(const VarDecl *decl, llvm::Value *AI,
llvm::Optional<unsigned> ArgNo,
CGBuilderTy &Builder,
const bool UsePointerValue = false);

struct BlockByRefType {

llvm::DIType *BlockByRefWrapper;

llvm::DIType *WrappedType;
};

std::string GetName(const Decl*, bool Qualified = false) const;


BlockByRefType EmitTypeForVarWithBlocksAttr(const VarDecl *VD,
uint64_t *OffSet);


llvm::DIScope *getDeclContextDescriptor(const Decl *D);

llvm::DIScope *getContextDescriptor(const Decl *Context,
llvm::DIScope *Default);

llvm::DIScope *getCurrentContextDescriptor(const Decl *Decl);


llvm::DICompositeType *getOrCreateRecordFwdDecl(const RecordType *,
llvm::DIScope *);


StringRef getCurrentDirname();


void CreateCompileUnit();


Optional<llvm::DIFile::ChecksumKind>
computeChecksum(FileID FID, SmallString<32> &Checksum) const;


Optional<StringRef> getSource(const SourceManager &SM, FileID FID);



llvm::DIFile *getOrCreateFile(SourceLocation Loc);


llvm::DIFile *
createFile(StringRef FileName,
Optional<llvm::DIFile::ChecksumInfo<StringRef>> CSInfo,
Optional<StringRef> Source);


llvm::DIType *getOrCreateType(QualType Ty, llvm::DIFile *Fg);



llvm::DIModule *getOrCreateModuleRef(ASTSourceDescriptor Mod,
bool CreateSkeletonCU);


llvm::DIModule *getParentModuleOrNull(const Decl *D);



llvm::DICompositeType *getOrCreateLimitedType(const RecordType *Ty);


llvm::DIType *CreateTypeNode(QualType Ty, llvm::DIFile *Fg);


llvm::DIType *CreateMemberType(llvm::DIFile *Unit, QualType FType,
StringRef Name, uint64_t *Offset);



llvm::DINode *getDeclarationOrDefinition(const Decl *D);



llvm::DISubprogram *getFunctionDeclaration(const Decl *D);







llvm::DISubprogram *
getObjCMethodDeclaration(const Decl *D, llvm::DISubroutineType *FnType,
unsigned LineNo, llvm::DINode::DIFlags Flags,
llvm::DISubprogram::DISPFlags SPFlags);





llvm::DIDerivedType *
getOrCreateStaticDataMemberDeclarationOrNull(const VarDecl *D);


llvm::DISubprogram *getFunctionFwdDeclOrStub(GlobalDecl GD, bool Stub);



llvm::DISubprogram *getFunctionForwardDeclaration(GlobalDecl GD);



llvm::DISubprogram *getFunctionStub(GlobalDecl GD);



llvm::DIGlobalVariable *
getGlobalVariableForwardDeclaration(const VarDecl *VD);








llvm::DIGlobalVariableExpression *
CollectAnonRecordDecls(const RecordDecl *RD, llvm::DIFile *Unit,
unsigned LineNo, StringRef LinkageName,
llvm::GlobalVariable *Var, llvm::DIScope *DContext);




llvm::DINode::DIFlags getCallSiteRelatedAttrs() const;


PrintingPolicy getPrintingPolicy() const;




StringRef getFunctionName(const FunctionDecl *FD);



StringRef getObjCMethodName(const ObjCMethodDecl *FD);



StringRef getSelectorName(Selector S);


StringRef getClassName(const RecordDecl *RD);


StringRef getVTableName(const CXXRecordDecl *Decl);



StringRef getDynamicInitializerName(const VarDecl *VD,
DynamicInitKind StubKind,
llvm::Function *InitFn);



unsigned getLineNumber(SourceLocation Loc);




unsigned getColumnNumber(SourceLocation Loc, bool Force = false);



void collectFunctionDeclProps(GlobalDecl GD, llvm::DIFile *Unit,
StringRef &Name, StringRef &LinkageName,
llvm::DIScope *&FDContext,
llvm::DINodeArray &TParamsArray,
llvm::DINode::DIFlags &Flags);


void collectVarDeclProps(const VarDecl *VD, llvm::DIFile *&Unit,
unsigned &LineNo, QualType &T, StringRef &Name,
StringRef &LinkageName,
llvm::MDTuple *&TemplateParameters,
llvm::DIScope *&VDContext);




StringRef internString(StringRef A, StringRef B = StringRef()) {
char *Data = DebugInfoNames.Allocate<char>(A.size() + B.size());
if (!A.empty())
std::memcpy(Data, A.data(), A.size());
if (!B.empty())
std::memcpy(Data + A.size(), B.data(), B.size());
return StringRef(Data, A.size() + B.size());
}
};



class ApplyDebugLocation {
private:
void init(SourceLocation TemporaryLocation, bool DefaultToEmpty = false);
ApplyDebugLocation(CodeGenFunction &CGF, bool DefaultToEmpty,
SourceLocation TemporaryLocation);

llvm::DebugLoc OriginalLocation;
CodeGenFunction *CGF;

public:

ApplyDebugLocation(CodeGenFunction &CGF, SourceLocation TemporaryLocation);
ApplyDebugLocation(CodeGenFunction &CGF, const Expr *E);
ApplyDebugLocation(CodeGenFunction &CGF, llvm::DebugLoc Loc);
ApplyDebugLocation(ApplyDebugLocation &&Other) : CGF(Other.CGF) {
Other.CGF = nullptr;
}
ApplyDebugLocation &operator=(ApplyDebugLocation &&) = default;

~ApplyDebugLocation();












static ApplyDebugLocation CreateArtificial(CodeGenFunction &CGF) {
return ApplyDebugLocation(CGF, false, SourceLocation());
}



static ApplyDebugLocation
CreateDefaultArtificial(CodeGenFunction &CGF,
SourceLocation TemporaryLocation) {
return ApplyDebugLocation(CGF, false, TemporaryLocation);
}






static ApplyDebugLocation CreateEmpty(CodeGenFunction &CGF) {
return ApplyDebugLocation(CGF, true, SourceLocation());
}
};


class ApplyInlineDebugLocation {
SourceLocation SavedLocation;
CodeGenFunction *CGF;

public:



ApplyInlineDebugLocation(CodeGenFunction &CGF, GlobalDecl InlinedFn);

~ApplyInlineDebugLocation();
};

}
}

#endif
