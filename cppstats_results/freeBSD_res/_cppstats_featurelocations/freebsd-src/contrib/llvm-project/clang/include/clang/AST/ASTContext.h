












#if !defined(LLVM_CLANG_AST_ASTCONTEXT_H)
#define LLVM_CLANG_AST_ASTCONTEXT_H

#include "clang/AST/ASTContextAllocate.h"
#include "clang/AST/ASTFwd.h"
#include "clang/AST/CanonicalType.h"
#include "clang/AST/CommentCommandTraits.h"
#include "clang/AST/ComparisonCategories.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RawCommentList.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/Basic/AddressSpaces.h"
#include "clang/Basic/AttrKinds.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/Linkage.h"
#include "clang/Basic/NoSanitizeList.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/ProfileList.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Basic/TargetCXXABI.h"
#include "clang/Basic/XRayLists.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/AlignOf.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TypeSize.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace llvm {

class APFixedPoint;
class FixedPointSemantics;
struct fltSemantics;
template <typename T, unsigned N> class SmallPtrSet;

}

namespace clang {

class APValue;
class ASTMutationListener;
class ASTRecordLayout;
class AtomicExpr;
class BlockExpr;
class BuiltinTemplateDecl;
class CharUnits;
class ConceptDecl;
class CXXABI;
class CXXConstructorDecl;
class CXXMethodDecl;
class CXXRecordDecl;
class DiagnosticsEngine;
class ParentMapContext;
class DynTypedNode;
class DynTypedNodeList;
class Expr;
class GlobalDecl;
class ItaniumMangleContext;
class MangleContext;
class MangleNumberingContext;
class MaterializeTemporaryExpr;
class MemberSpecializationInfo;
class Module;
struct MSGuidDeclParts;
class ObjCCategoryDecl;
class ObjCCategoryImplDecl;
class ObjCContainerDecl;
class ObjCImplDecl;
class ObjCImplementationDecl;
class ObjCInterfaceDecl;
class ObjCIvarDecl;
class ObjCMethodDecl;
class ObjCPropertyDecl;
class ObjCPropertyImplDecl;
class ObjCProtocolDecl;
class ObjCTypeParamDecl;
class OMPTraitInfo;
struct ParsedTargetAttr;
class Preprocessor;
class Stmt;
class StoredDeclsMap;
class TargetAttr;
class TargetInfo;
class TemplateDecl;
class TemplateParameterList;
class TemplateTemplateParmDecl;
class TemplateTypeParmDecl;
class UnresolvedSetIterator;
class UsingShadowDecl;
class VarTemplateDecl;
class VTableContextBase;
struct BlockVarCopyInit;

namespace Builtin {

class Context;

}

enum BuiltinTemplateKind : int;
enum OpenCLTypeKind : uint8_t;

namespace comments {

class FullComment;

}

namespace interp {

class Context;

}

namespace serialization {
template <class> class AbstractTypeReader;
}

struct TypeInfo {
uint64_t Width = 0;
unsigned Align = 0;
bool AlignIsRequired : 1;

TypeInfo() : AlignIsRequired(false) {}
TypeInfo(uint64_t Width, unsigned Align, bool AlignIsRequired)
: Width(Width), Align(Align), AlignIsRequired(AlignIsRequired) {}
};

struct TypeInfoChars {
CharUnits Width;
CharUnits Align;
bool AlignIsRequired : 1;

TypeInfoChars() : AlignIsRequired(false) {}
TypeInfoChars(CharUnits Width, CharUnits Align, bool AlignIsRequired)
: Width(Width), Align(Align), AlignIsRequired(AlignIsRequired) {}
};



class ASTContext : public RefCountedBase<ASTContext> {
friend class NestedNameSpecifier;

mutable SmallVector<Type *, 0> Types;
mutable llvm::FoldingSet<ExtQuals> ExtQualNodes;
mutable llvm::FoldingSet<ComplexType> ComplexTypes;
mutable llvm::FoldingSet<PointerType> PointerTypes;
mutable llvm::FoldingSet<AdjustedType> AdjustedTypes;
mutable llvm::FoldingSet<BlockPointerType> BlockPointerTypes;
mutable llvm::FoldingSet<LValueReferenceType> LValueReferenceTypes;
mutable llvm::FoldingSet<RValueReferenceType> RValueReferenceTypes;
mutable llvm::FoldingSet<MemberPointerType> MemberPointerTypes;
mutable llvm::ContextualFoldingSet<ConstantArrayType, ASTContext &>
ConstantArrayTypes;
mutable llvm::FoldingSet<IncompleteArrayType> IncompleteArrayTypes;
mutable std::vector<VariableArrayType*> VariableArrayTypes;
mutable llvm::FoldingSet<DependentSizedArrayType> DependentSizedArrayTypes;
mutable llvm::FoldingSet<DependentSizedExtVectorType>
DependentSizedExtVectorTypes;
mutable llvm::FoldingSet<DependentAddressSpaceType>
DependentAddressSpaceTypes;
mutable llvm::FoldingSet<VectorType> VectorTypes;
mutable llvm::FoldingSet<DependentVectorType> DependentVectorTypes;
mutable llvm::FoldingSet<ConstantMatrixType> MatrixTypes;
mutable llvm::FoldingSet<DependentSizedMatrixType> DependentSizedMatrixTypes;
mutable llvm::FoldingSet<FunctionNoProtoType> FunctionNoProtoTypes;
mutable llvm::ContextualFoldingSet<FunctionProtoType, ASTContext&>
FunctionProtoTypes;
mutable llvm::FoldingSet<DependentTypeOfExprType> DependentTypeOfExprTypes;
mutable llvm::FoldingSet<DependentDecltypeType> DependentDecltypeTypes;
mutable llvm::FoldingSet<TemplateTypeParmType> TemplateTypeParmTypes;
mutable llvm::FoldingSet<ObjCTypeParamType> ObjCTypeParamTypes;
mutable llvm::FoldingSet<SubstTemplateTypeParmType>
SubstTemplateTypeParmTypes;
mutable llvm::FoldingSet<SubstTemplateTypeParmPackType>
SubstTemplateTypeParmPackTypes;
mutable llvm::ContextualFoldingSet<TemplateSpecializationType, ASTContext&>
TemplateSpecializationTypes;
mutable llvm::FoldingSet<ParenType> ParenTypes;
mutable llvm::FoldingSet<ElaboratedType> ElaboratedTypes;
mutable llvm::FoldingSet<DependentNameType> DependentNameTypes;
mutable llvm::ContextualFoldingSet<DependentTemplateSpecializationType,
ASTContext&>
DependentTemplateSpecializationTypes;
llvm::FoldingSet<PackExpansionType> PackExpansionTypes;
mutable llvm::FoldingSet<ObjCObjectTypeImpl> ObjCObjectTypes;
mutable llvm::FoldingSet<ObjCObjectPointerType> ObjCObjectPointerTypes;
mutable llvm::FoldingSet<DependentUnaryTransformType>
DependentUnaryTransformTypes;
mutable llvm::ContextualFoldingSet<AutoType, ASTContext&> AutoTypes;
mutable llvm::FoldingSet<DeducedTemplateSpecializationType>
DeducedTemplateSpecializationTypes;
mutable llvm::FoldingSet<AtomicType> AtomicTypes;
llvm::FoldingSet<AttributedType> AttributedTypes;
mutable llvm::FoldingSet<PipeType> PipeTypes;
mutable llvm::FoldingSet<ExtIntType> ExtIntTypes;
mutable llvm::FoldingSet<DependentExtIntType> DependentExtIntTypes;

mutable llvm::FoldingSet<QualifiedTemplateName> QualifiedTemplateNames;
mutable llvm::FoldingSet<DependentTemplateName> DependentTemplateNames;
mutable llvm::FoldingSet<SubstTemplateTemplateParmStorage>
SubstTemplateTemplateParms;
mutable llvm::ContextualFoldingSet<SubstTemplateTemplateParmPackStorage,
ASTContext&>
SubstTemplateTemplateParmPacks;




mutable llvm::FoldingSet<NestedNameSpecifier> NestedNameSpecifiers;
mutable NestedNameSpecifier *GlobalNestedNameSpecifier = nullptr;




mutable llvm::DenseMap<const RecordDecl*, const ASTRecordLayout*>
ASTRecordLayouts;
mutable llvm::DenseMap<const ObjCContainerDecl*, const ASTRecordLayout*>
ObjCLayouts;


using TypeInfoMap = llvm::DenseMap<const Type *, struct TypeInfo>;
mutable TypeInfoMap MemoizedTypeInfo;




using UnadjustedAlignMap = llvm::DenseMap<const Type *, unsigned>;
mutable UnadjustedAlignMap MemoizedUnadjustedAlign;


llvm::DenseMap<const CXXRecordDecl*, LazyDeclPtr> KeyFunctions;


llvm::DenseMap<ObjCContainerDecl*, ObjCImplDecl*> ObjCImpls;



llvm::DenseMap<const ObjCMethodDecl*,const ObjCMethodDecl*> ObjCMethodRedecls;


llvm::DenseMap<const VarDecl *, BlockVarCopyInit> BlockVarCopyInits;


mutable llvm::FoldingSet<MSGuidDecl> MSGuidDecls;


mutable llvm::FoldingSet<TemplateParamObjectDecl> TemplateParamObjectDecls;





mutable llvm::StringMap<StringLiteral *> StringLiteralCache;



mutable std::string CUIDHash;



class CanonicalTemplateTemplateParm : public llvm::FoldingSetNode {
TemplateTemplateParmDecl *Parm;

public:
CanonicalTemplateTemplateParm(TemplateTemplateParmDecl *Parm)
: Parm(Parm) {}

TemplateTemplateParmDecl *getParam() const { return Parm; }

void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &C) {
Profile(ID, C, Parm);
}

static void Profile(llvm::FoldingSetNodeID &ID,
const ASTContext &C,
TemplateTemplateParmDecl *Parm);
};
mutable llvm::ContextualFoldingSet<CanonicalTemplateTemplateParm,
const ASTContext&>
CanonTemplateTemplateParms;

TemplateTemplateParmDecl *
getCanonicalTemplateTemplateParmDecl(TemplateTemplateParmDecl *TTP) const;


mutable TypedefDecl *Int128Decl = nullptr;


mutable TypedefDecl *UInt128Decl = nullptr;



mutable TypedefDecl *BuiltinVaListDecl = nullptr;


mutable TypedefDecl *BuiltinMSVaListDecl = nullptr;


mutable TypedefDecl *ObjCIdDecl = nullptr;


mutable TypedefDecl *ObjCSelDecl = nullptr;


mutable TypedefDecl *ObjCClassDecl = nullptr;


mutable ObjCInterfaceDecl *ObjCProtocolClassDecl = nullptr;


mutable TypedefDecl *BOOLDecl = nullptr;



QualType ObjCIdRedefinitionType;
QualType ObjCClassRedefinitionType;
QualType ObjCSelRedefinitionType;


mutable IdentifierInfo *BoolName = nullptr;


mutable IdentifierInfo *NSObjectName = nullptr;


IdentifierInfo *NSCopyingName = nullptr;


mutable IdentifierInfo *MakeIntegerSeqName = nullptr;


mutable IdentifierInfo *TypePackElementName = nullptr;

QualType ObjCConstantStringType;
mutable RecordDecl *CFConstantStringTagDecl = nullptr;
mutable TypedefDecl *CFConstantStringTypeDecl = nullptr;

mutable QualType ObjCSuperType;

QualType ObjCNSStringType;


TypedefDecl *ObjCInstanceTypeDecl = nullptr;


TypeDecl *FILEDecl = nullptr;


TypeDecl *jmp_bufDecl = nullptr;


TypeDecl *sigjmp_bufDecl = nullptr;


TypeDecl *ucontext_tDecl = nullptr;





mutable RecordDecl *BlockDescriptorType = nullptr;





mutable RecordDecl *BlockDescriptorExtendedType = nullptr;


FunctionDecl *cudaConfigureCallDecl = nullptr;





llvm::DenseMap<const Decl*, AttrVec*> DeclAttrs;




llvm::DenseMap<Decl*, Decl*> MergedDecls;




llvm::DenseMap<NamedDecl*, llvm::TinyPtrVector<Module*>> MergedDefModules;





struct PerModuleInitializers {
llvm::SmallVector<Decl*, 4> Initializers;
llvm::SmallVector<uint32_t, 4> LazyInitializers;

void resolve(ASTContext &Ctx);
};
llvm::DenseMap<Module*, PerModuleInitializers*> ModuleInitializers;

ASTContext &this_() { return *this; }

public:

using TemplateOrSpecializationInfo =
llvm::PointerUnion<VarTemplateDecl *, MemberSpecializationInfo *>;

private:
friend class ASTDeclReader;
friend class ASTReader;
friend class ASTWriter;
template <class> friend class serialization::AbstractTypeReader;
friend class CXXRecordDecl;
friend class IncrementalParser;































llvm::DenseMap<const VarDecl *, TemplateOrSpecializationInfo>
TemplateOrInstantiation;
























llvm::DenseMap<NamedDecl *, NamedDecl *> InstantiatedFromUsingDecl;








llvm::DenseMap<UsingEnumDecl *, UsingEnumDecl *>
InstantiatedFromUsingEnumDecl;


llvm::DenseMap<UsingShadowDecl*, UsingShadowDecl*>
InstantiatedFromUsingShadowDecl;

llvm::DenseMap<FieldDecl *, FieldDecl *> InstantiatedFromUnnamedFieldDecl;







using CXXMethodVector = llvm::TinyPtrVector<const CXXMethodDecl *>;
llvm::DenseMap<const CXXMethodDecl *, CXXMethodVector> OverriddenMethods;




llvm::DenseMap<const DeclContext *, std::unique_ptr<MangleNumberingContext>>
MangleNumberingContexts;
llvm::DenseMap<const Decl *, std::unique_ptr<MangleNumberingContext>>
ExtraMangleNumberingContexts;



llvm::MapVector<const NamedDecl *, unsigned> MangleNumbers;
llvm::MapVector<const VarDecl *, unsigned> StaticLocalNumbers;

mutable llvm::DenseMap<const CXXRecordDecl *, unsigned>
DeviceLambdaManglingNumbers;



using ParameterIndexTable = llvm::DenseMap<const VarDecl *, unsigned>;
ParameterIndexTable ParamIndices;

ImportDecl *FirstLocalImport = nullptr;
ImportDecl *LastLocalImport = nullptr;

TranslationUnitDecl *TUDecl = nullptr;
mutable ExternCContextDecl *ExternCContext = nullptr;
mutable BuiltinTemplateDecl *MakeIntegerSeqDecl = nullptr;
mutable BuiltinTemplateDecl *TypePackElementDecl = nullptr;


SourceManager &SourceMgr;



LangOptions &LangOpts;



std::unique_ptr<NoSanitizeList> NoSanitizeL;



std::unique_ptr<XRayFunctionFilter> XRayFilter;



std::unique_ptr<ProfileList> ProfList;





mutable llvm::BumpPtrAllocator BumpAlloc;


PartialDiagnostic::DiagStorageAllocator DiagAllocator;


std::unique_ptr<CXXABI> ABI;
CXXABI *createCXXABI(const TargetInfo &T);


const LangASMap *AddrSpaceMap = nullptr;



bool AddrSpaceMapMangling;

const TargetInfo *Target = nullptr;
const TargetInfo *AuxTarget = nullptr;
clang::PrintingPolicy PrintingPolicy;
std::unique_ptr<interp::Context> InterpContext;
std::unique_ptr<ParentMapContext> ParentMapCtx;


DeclListNode *ListNodeFreeList = nullptr;

public:
IdentifierTable &Idents;
SelectorTable &Selectors;
Builtin::Context &BuiltinInfo;
const TranslationUnitKind TUKind;
mutable DeclarationNameTable DeclarationNames;
IntrusiveRefCntPtr<ExternalASTSource> ExternalSource;
ASTMutationListener *Listener = nullptr;


interp::Context &getInterpContext();


ParentMapContext &getParentMapContext();



















std::vector<Decl *> getTraversalScope() const { return TraversalScope; }
void setTraversalScope(const std::vector<Decl *> &);



template <typename NodeT> DynTypedNodeList getParents(const NodeT &Node);

const clang::PrintingPolicy &getPrintingPolicy() const {
return PrintingPolicy;
}

void setPrintingPolicy(const clang::PrintingPolicy &Policy) {
PrintingPolicy = Policy;
}

SourceManager& getSourceManager() { return SourceMgr; }
const SourceManager& getSourceManager() const { return SourceMgr; }

llvm::BumpPtrAllocator &getAllocator() const {
return BumpAlloc;
}

void *Allocate(size_t Size, unsigned Align = 8) const {
return BumpAlloc.Allocate(Size, Align);
}
template <typename T> T *Allocate(size_t Num = 1) const {
return static_cast<T *>(Allocate(Num * sizeof(T), alignof(T)));
}
void Deallocate(void *Ptr) const {}



DeclListNode *AllocateDeclListNode(clang::NamedDecl *ND) {
if (DeclListNode *Alloc = ListNodeFreeList) {
ListNodeFreeList = Alloc->Rest.dyn_cast<DeclListNode*>();
Alloc->D = ND;
Alloc->Rest = nullptr;
return Alloc;
}
return new (*this) DeclListNode(ND);
}


void DeallocateDeclListNode(DeclListNode *N) {
N->Rest = ListNodeFreeList;
ListNodeFreeList = N;
}



size_t getASTAllocatedMemory() const {
return BumpAlloc.getTotalMemory();
}


size_t getSideTableAllocatedMemory() const;

PartialDiagnostic::DiagStorageAllocator &getDiagAllocator() {
return DiagAllocator;
}

const TargetInfo &getTargetInfo() const { return *Target; }
const TargetInfo *getAuxTargetInfo() const { return AuxTarget; }





QualType getIntTypeForBitwidth(unsigned DestWidth,
unsigned Signed) const;




QualType getRealTypeForBitwidth(unsigned DestWidth, bool ExplicitIEEE) const;

bool AtomicUsesUnsupportedLibcall(const AtomicExpr *E) const;

const LangOptions& getLangOpts() const { return LangOpts; }





bool isDependenceAllowed() const {
return LangOpts.CPlusPlus || LangOpts.RecoveryAST;
}

const NoSanitizeList &getNoSanitizeList() const { return *NoSanitizeL; }

const XRayFunctionFilter &getXRayFilter() const {
return *XRayFilter;
}

const ProfileList &getProfileList() const { return *ProfList; }

DiagnosticsEngine &getDiagnostics() const;

FullSourceLoc getFullLoc(SourceLocation Loc) const {
return FullSourceLoc(Loc,SourceMgr);
}




TargetCXXABI::Kind getCXXABIKind() const;


RawCommentList Comments;


mutable bool CommentsLoaded = false;





mutable llvm::DenseMap<const Decl *, const RawComment *> DeclRawComments;






mutable llvm::DenseMap<const Decl *, const Decl *> RedeclChainComments;








mutable llvm::DenseMap<const Decl *, const Decl *> CommentlessRedeclChains;



mutable llvm::DenseMap<const Decl *, comments::FullComment *> ParsedComments;






void cacheRawCommentForDecl(const Decl &OriginalD,
const RawComment &Comment) const;






RawComment *getRawCommentForDeclNoCacheImpl(
const Decl *D, const SourceLocation RepresentativeLocForDecl,
const std::map<unsigned, RawComment *> &CommentsInFile) const;



RawComment *getRawCommentForDeclNoCache(const Decl *D) const;

public:
void addComment(const RawComment &RC);






const RawComment *
getRawCommentForAnyRedecl(const Decl *D,
const Decl **OriginalDecl = nullptr) const;








void attachCommentsToJustParsedDecls(ArrayRef<Decl *> Decls,
const Preprocessor *PP);






comments::FullComment *getCommentForDecl(const Decl *D,
const Preprocessor *PP) const;




comments::FullComment *getLocalCommentForDeclUncached(const Decl *D) const;

comments::FullComment *cloneFullComment(comments::FullComment *FC,
const Decl *D) const;

private:
mutable comments::CommandTraits CommentCommandTraits;


class import_iterator {
ImportDecl *Import = nullptr;

public:
using value_type = ImportDecl *;
using reference = ImportDecl *;
using pointer = ImportDecl *;
using difference_type = int;
using iterator_category = std::forward_iterator_tag;

import_iterator() = default;
explicit import_iterator(ImportDecl *Import) : Import(Import) {}

reference operator*() const { return Import; }
pointer operator->() const { return Import; }

import_iterator &operator++() {
Import = ASTContext::getNextLocalImport(Import);
return *this;
}

import_iterator operator++(int) {
import_iterator Other(*this);
++(*this);
return Other;
}

friend bool operator==(import_iterator X, import_iterator Y) {
return X.Import == Y.Import;
}

friend bool operator!=(import_iterator X, import_iterator Y) {
return X.Import != Y.Import;
}
};

public:
comments::CommandTraits &getCommentCommandTraits() const {
return CommentCommandTraits;
}


AttrVec& getDeclAttrs(const Decl *D);


void eraseDeclAttrs(const Decl *D);





MemberSpecializationInfo *getInstantiatedFromStaticDataMember(
const VarDecl *Var);



void setInstantiatedFromStaticDataMember(VarDecl *Inst, VarDecl *Tmpl,
TemplateSpecializationKind TSK,
SourceLocation PointOfInstantiation = SourceLocation());

TemplateOrSpecializationInfo
getTemplateOrSpecializationInfo(const VarDecl *Var);

void setTemplateOrSpecializationInfo(VarDecl *Inst,
TemplateOrSpecializationInfo TSI);



NamedDecl *getInstantiatedFromUsingDecl(NamedDecl *Inst);



void setInstantiatedFromUsingDecl(NamedDecl *Inst, NamedDecl *Pattern);



UsingEnumDecl *getInstantiatedFromUsingEnumDecl(UsingEnumDecl *Inst);



void setInstantiatedFromUsingEnumDecl(UsingEnumDecl *Inst,
UsingEnumDecl *Pattern);

UsingShadowDecl *getInstantiatedFromUsingShadowDecl(UsingShadowDecl *Inst);
void setInstantiatedFromUsingShadowDecl(UsingShadowDecl *Inst,
UsingShadowDecl *Pattern);

FieldDecl *getInstantiatedFromUnnamedFieldDecl(FieldDecl *Field);

void setInstantiatedFromUnnamedFieldDecl(FieldDecl *Inst, FieldDecl *Tmpl);


using overridden_cxx_method_iterator = CXXMethodVector::const_iterator;
overridden_cxx_method_iterator
overridden_methods_begin(const CXXMethodDecl *Method) const;

overridden_cxx_method_iterator
overridden_methods_end(const CXXMethodDecl *Method) const;

unsigned overridden_methods_size(const CXXMethodDecl *Method) const;

using overridden_method_range =
llvm::iterator_range<overridden_cxx_method_iterator>;

overridden_method_range overridden_methods(const CXXMethodDecl *Method) const;



void addOverriddenMethod(const CXXMethodDecl *Method,
const CXXMethodDecl *Overridden);








void getOverriddenMethods(
const NamedDecl *Method,
SmallVectorImpl<const NamedDecl *> &Overridden) const;



void addedLocalImportDecl(ImportDecl *Import);

static ImportDecl *getNextLocalImport(ImportDecl *Import) {
return Import->getNextLocalImport();
}

using import_range = llvm::iterator_range<import_iterator>;

import_range local_imports() const {
return import_range(import_iterator(FirstLocalImport), import_iterator());
}

Decl *getPrimaryMergedDecl(Decl *D) {
Decl *Result = MergedDecls.lookup(D);
return Result ? Result : D;
}
void setPrimaryMergedDecl(Decl *D, Decl *Primary) {
MergedDecls[D] = Primary;
}



void mergeDefinitionIntoModule(NamedDecl *ND, Module *M,
bool NotifyListeners = true);



void deduplicateMergedDefinitonsFor(NamedDecl *ND);



ArrayRef<Module*> getModulesWithMergedDefinition(const NamedDecl *Def);





void addModuleInitializer(Module *M, Decl *Init);

void addLazyModuleInitializers(Module *M, ArrayRef<uint32_t> IDs);


ArrayRef<Decl*> getModuleInitializers(Module *M);

TranslationUnitDecl *getTranslationUnitDecl() const {
return TUDecl->getMostRecentDecl();
}
void addTranslationUnitDecl() {
assert(!TUDecl || TUKind == TU_Incremental);
TranslationUnitDecl *NewTUDecl = TranslationUnitDecl::Create(*this);
if (TraversalScope.empty() || TraversalScope.back() == TUDecl)
TraversalScope = {NewTUDecl};
if (TUDecl)
NewTUDecl->setPreviousDecl(TUDecl);
TUDecl = NewTUDecl;
}

ExternCContextDecl *getExternCContextDecl() const;
BuiltinTemplateDecl *getMakeIntegerSeqDecl() const;
BuiltinTemplateDecl *getTypePackElementDecl() const;


CanQualType VoidTy;
CanQualType BoolTy;
CanQualType CharTy;
CanQualType WCharTy;
CanQualType WideCharTy;
CanQualType WIntTy;
CanQualType Char8Ty;
CanQualType Char16Ty;
CanQualType Char32Ty;
CanQualType SignedCharTy, ShortTy, IntTy, LongTy, LongLongTy, Int128Ty;
CanQualType UnsignedCharTy, UnsignedShortTy, UnsignedIntTy, UnsignedLongTy;
CanQualType UnsignedLongLongTy, UnsignedInt128Ty;
CanQualType FloatTy, DoubleTy, LongDoubleTy, Float128Ty;
CanQualType ShortAccumTy, AccumTy,
LongAccumTy;
CanQualType UnsignedShortAccumTy, UnsignedAccumTy, UnsignedLongAccumTy;
CanQualType ShortFractTy, FractTy, LongFractTy;
CanQualType UnsignedShortFractTy, UnsignedFractTy, UnsignedLongFractTy;
CanQualType SatShortAccumTy, SatAccumTy, SatLongAccumTy;
CanQualType SatUnsignedShortAccumTy, SatUnsignedAccumTy,
SatUnsignedLongAccumTy;
CanQualType SatShortFractTy, SatFractTy, SatLongFractTy;
CanQualType SatUnsignedShortFractTy, SatUnsignedFractTy,
SatUnsignedLongFractTy;
CanQualType HalfTy;
CanQualType BFloat16Ty;
CanQualType Float16Ty;
CanQualType FloatComplexTy, DoubleComplexTy, LongDoubleComplexTy;
CanQualType Float128ComplexTy;
CanQualType VoidPtrTy, NullPtrTy;
CanQualType DependentTy, OverloadTy, BoundMemberTy, UnknownAnyTy;
CanQualType BuiltinFnTy;
CanQualType PseudoObjectTy, ARCUnbridgedCastTy;
CanQualType ObjCBuiltinIdTy, ObjCBuiltinClassTy, ObjCBuiltinSelTy;
CanQualType ObjCBuiltinBoolTy;
#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) CanQualType SingletonId;

#include "clang/Basic/OpenCLImageTypes.def"
CanQualType OCLSamplerTy, OCLEventTy, OCLClkEventTy;
CanQualType OCLQueueTy, OCLReserveIDTy;
CanQualType IncompleteMatrixIdxTy;
CanQualType OMPArraySectionTy, OMPArrayShapingTy, OMPIteratorTy;
#define EXT_OPAQUE_TYPE(ExtType, Id, Ext) CanQualType Id##Ty;

#include "clang/Basic/OpenCLExtensionTypes.def"
#define SVE_TYPE(Name, Id, SingletonId) CanQualType SingletonId;

#include "clang/Basic/AArch64SVEACLETypes.def"
#define PPC_VECTOR_TYPE(Name, Id, Size) CanQualType Id##Ty;

#include "clang/Basic/PPCTypes.def"
#define RVV_TYPE(Name, Id, SingletonId) CanQualType SingletonId;

#include "clang/Basic/RISCVVTypes.def"


mutable QualType AutoDeductTy;
mutable QualType AutoRRefDeductTy;



mutable Decl *VaListTagDecl = nullptr;


mutable TagDecl *MSGuidTagDecl = nullptr;


llvm::DenseSet<const VarDecl *> CUDADeviceVarODRUsedByHost;

ASTContext(LangOptions &LOpts, SourceManager &SM, IdentifierTable &idents,
SelectorTable &sels, Builtin::Context &builtins,
TranslationUnitKind TUKind);
ASTContext(const ASTContext &) = delete;
ASTContext &operator=(const ASTContext &) = delete;
~ASTContext();






void setExternalSource(IntrusiveRefCntPtr<ExternalASTSource> Source);



ExternalASTSource *getExternalSource() const {
return ExternalSource.get();
}






void setASTMutationListener(ASTMutationListener *Listener) {
this->Listener = Listener;
}



ASTMutationListener *getASTMutationListener() const { return Listener; }

void PrintStats() const;
const SmallVectorImpl<Type *>& getTypes() const { return Types; }

BuiltinTemplateDecl *buildBuiltinTemplateDecl(BuiltinTemplateKind BTK,
const IdentifierInfo *II) const;



RecordDecl *buildImplicitRecord(StringRef Name,
RecordDecl::TagKind TK = TTK_Struct) const;


TypedefDecl *buildImplicitTypedef(QualType T, StringRef Name) const;


TypedefDecl *getInt128Decl() const;


TypedefDecl *getUInt128Decl() const;





private:

QualType getExtQualType(const Type *Base, Qualifiers Quals) const;

QualType getTypeDeclTypeSlow(const TypeDecl *Decl) const;

QualType getPipeType(QualType T, bool ReadOnly) const;

public:






QualType getAddrSpaceQualType(QualType T, LangAS AddressSpace) const;






QualType removeAddrSpaceQualType(QualType T) const;





QualType applyObjCProtocolQualifiers(QualType type,
ArrayRef<ObjCProtocolDecl *> protocols, bool &hasError,
bool allowOnPointerType = false) const;






QualType getObjCGCQualType(QualType T, Qualifiers::GC gcAttr) const;



QualType removePtrSizeAddrSpace(QualType T) const;






QualType getRestrictType(QualType T) const {
return T.withFastQualifiers(Qualifiers::Restrict);
}






QualType getVolatileType(QualType T) const {
return T.withFastQualifiers(Qualifiers::Volatile);
}








QualType getConstType(QualType T) const { return T.withConst(); }


const FunctionType *adjustFunctionType(const FunctionType *Fn,
FunctionType::ExtInfo EInfo);


CanQualType getCanonicalFunctionResultType(QualType ResultType) const;


void adjustDeducedFunctionResultType(FunctionDecl *FD, QualType ResultType);





QualType getFunctionTypeWithExceptionSpec(
QualType Orig, const FunctionProtoType::ExceptionSpecInfo &ESI);



bool hasSameFunctionTypeIgnoringExceptionSpec(QualType T, QualType U);



void adjustExceptionSpec(FunctionDecl *FD,
const FunctionProtoType::ExceptionSpecInfo &ESI,
bool AsWritten = false);




QualType getFunctionTypeWithoutPtrSizes(QualType T);



bool hasSameFunctionTypeIgnoringPtrSizes(QualType T, QualType U);



QualType getComplexType(QualType T) const;
CanQualType getComplexType(CanQualType T) const {
return CanQualType::CreateUnsafe(getComplexType((QualType) T));
}



QualType getPointerType(QualType T) const;
CanQualType getPointerType(CanQualType T) const {
return CanQualType::CreateUnsafe(getPointerType((QualType) T));
}



QualType getAdjustedType(QualType Orig, QualType New) const;
CanQualType getAdjustedType(CanQualType Orig, CanQualType New) const {
return CanQualType::CreateUnsafe(
getAdjustedType((QualType)Orig, (QualType)New));
}




QualType getDecayedType(QualType T) const;
CanQualType getDecayedType(CanQualType T) const {
return CanQualType::CreateUnsafe(getDecayedType((QualType) T));
}



QualType getAtomicType(QualType T) const;



QualType getBlockPointerType(QualType T) const;



QualType getBlockDescriptorType() const;


QualType getReadPipeType(QualType T) const;


QualType getWritePipeType(QualType T) const;



QualType getExtIntType(bool Unsigned, unsigned NumBits) const;



QualType getDependentExtIntType(bool Unsigned, Expr *BitsExpr) const;



QualType getBlockDescriptorExtendedType() const;


OpenCLTypeKind getOpenCLTypeKind(const Type *T) const;


LangAS getOpenCLTypeAddrSpace(const Type *T) const;

void setcudaConfigureCallDecl(FunctionDecl *FD) {
cudaConfigureCallDecl = FD;
}

FunctionDecl *getcudaConfigureCallDecl() {
return cudaConfigureCallDecl;
}


bool BlockRequiresCopying(QualType Ty, const VarDecl *D);




bool getByrefLifetime(QualType Ty,
Qualifiers::ObjCLifetime &Lifetime,
bool &HasByrefExtendedLayout) const;



QualType getLValueReferenceType(QualType T, bool SpelledAsLValue = true)
const;



QualType getRValueReferenceType(QualType T) const;





QualType getMemberPointerType(QualType T, const Type *Cls) const;



QualType getVariableArrayType(QualType EltTy, Expr *NumElts,
ArrayType::ArraySizeModifier ASM,
unsigned IndexTypeQuals,
SourceRange Brackets) const;






QualType getDependentSizedArrayType(QualType EltTy, Expr *NumElts,
ArrayType::ArraySizeModifier ASM,
unsigned IndexTypeQuals,
SourceRange Brackets) const;



QualType getIncompleteArrayType(QualType EltTy,
ArrayType::ArraySizeModifier ASM,
unsigned IndexTypeQuals) const;



QualType getConstantArrayType(QualType EltTy, const llvm::APInt &ArySize,
const Expr *SizeExpr,
ArrayType::ArraySizeModifier ASM,
unsigned IndexTypeQuals) const;



QualType getStringLiteralArrayType(QualType EltTy, unsigned Length) const;


QualType getVariableArrayDecayedType(QualType Ty) const;


struct BuiltinVectorTypeInfo {
QualType ElementType;
llvm::ElementCount EC;
unsigned NumVectors;
BuiltinVectorTypeInfo(QualType ElementType, llvm::ElementCount EC,
unsigned NumVectors)
: ElementType(ElementType), EC(EC), NumVectors(NumVectors) {}
};



BuiltinVectorTypeInfo
getBuiltinVectorTypeInfo(const BuiltinType *VecTy) const;





QualType getScalableVectorType(QualType EltTy, unsigned NumElts) const;





QualType getVectorType(QualType VectorType, unsigned NumElts,
VectorType::VectorKind VecKind) const;


QualType getDependentVectorType(QualType VectorType, Expr *SizeExpr,
SourceLocation AttrLoc,
VectorType::VectorKind VecKind) const;





QualType getExtVectorType(QualType VectorType, unsigned NumElts) const;






QualType getDependentSizedExtVectorType(QualType VectorType,
Expr *SizeExpr,
SourceLocation AttrLoc) const;






QualType getConstantMatrixType(QualType ElementType, unsigned NumRows,
unsigned NumColumns) const;



QualType getDependentSizedMatrixType(QualType ElementType, Expr *RowExpr,
Expr *ColumnExpr,
SourceLocation AttrLoc) const;

QualType getDependentAddressSpaceType(QualType PointeeType,
Expr *AddrSpaceExpr,
SourceLocation AttrLoc) const;


QualType getFunctionNoProtoType(QualType ResultTy,
const FunctionType::ExtInfo &Info) const;

QualType getFunctionNoProtoType(QualType ResultTy) const {
return getFunctionNoProtoType(ResultTy, FunctionType::ExtInfo());
}


QualType getFunctionType(QualType ResultTy, ArrayRef<QualType> Args,
const FunctionProtoType::ExtProtoInfo &EPI) const {
return getFunctionTypeInternal(ResultTy, Args, EPI, false);
}

QualType adjustStringLiteralBaseType(QualType StrLTy) const;

private:

QualType getFunctionTypeInternal(QualType ResultTy, ArrayRef<QualType> Args,
const FunctionProtoType::ExtProtoInfo &EPI,
bool OnlyWantCanonical) const;

public:


QualType getTypeDeclType(const TypeDecl *Decl,
const TypeDecl *PrevDecl = nullptr) const {
assert(Decl && "Passed null for Decl param");
if (Decl->TypeForDecl) return QualType(Decl->TypeForDecl, 0);

if (PrevDecl) {
assert(PrevDecl->TypeForDecl && "previous decl has no TypeForDecl");
Decl->TypeForDecl = PrevDecl->TypeForDecl;
return QualType(PrevDecl->TypeForDecl, 0);
}

return getTypeDeclTypeSlow(Decl);
}



QualType getTypedefType(const TypedefNameDecl *Decl,
QualType Underlying = QualType()) const;

QualType getRecordType(const RecordDecl *Decl) const;

QualType getEnumType(const EnumDecl *Decl) const;

QualType getInjectedClassNameType(CXXRecordDecl *Decl, QualType TST) const;

QualType getAttributedType(attr::Kind attrKind,
QualType modifiedType,
QualType equivalentType);

QualType getSubstTemplateTypeParmType(const TemplateTypeParmType *Replaced,
QualType Replacement) const;
QualType getSubstTemplateTypeParmPackType(
const TemplateTypeParmType *Replaced,
const TemplateArgument &ArgPack);

QualType
getTemplateTypeParmType(unsigned Depth, unsigned Index,
bool ParameterPack,
TemplateTypeParmDecl *ParmDecl = nullptr) const;

QualType getTemplateSpecializationType(TemplateName T,
ArrayRef<TemplateArgument> Args,
QualType Canon = QualType()) const;

QualType
getCanonicalTemplateSpecializationType(TemplateName T,
ArrayRef<TemplateArgument> Args) const;

QualType getTemplateSpecializationType(TemplateName T,
const TemplateArgumentListInfo &Args,
QualType Canon = QualType()) const;

TypeSourceInfo *
getTemplateSpecializationTypeInfo(TemplateName T, SourceLocation TLoc,
const TemplateArgumentListInfo &Args,
QualType Canon = QualType()) const;

QualType getParenType(QualType NamedType) const;

QualType getMacroQualifiedType(QualType UnderlyingTy,
const IdentifierInfo *MacroII) const;

QualType getElaboratedType(ElaboratedTypeKeyword Keyword,
NestedNameSpecifier *NNS, QualType NamedType,
TagDecl *OwnedTagDecl = nullptr) const;
QualType getDependentNameType(ElaboratedTypeKeyword Keyword,
NestedNameSpecifier *NNS,
const IdentifierInfo *Name,
QualType Canon = QualType()) const;

QualType getDependentTemplateSpecializationType(ElaboratedTypeKeyword Keyword,
NestedNameSpecifier *NNS,
const IdentifierInfo *Name,
const TemplateArgumentListInfo &Args) const;
QualType getDependentTemplateSpecializationType(
ElaboratedTypeKeyword Keyword, NestedNameSpecifier *NNS,
const IdentifierInfo *Name, ArrayRef<TemplateArgument> Args) const;

TemplateArgument getInjectedTemplateArg(NamedDecl *ParamDecl);




void getInjectedTemplateArgs(const TemplateParameterList *Params,
SmallVectorImpl<TemplateArgument> &Args);








QualType getPackExpansionType(QualType Pattern,
Optional<unsigned> NumExpansions,
bool ExpectPackInType = true);

QualType getObjCInterfaceType(const ObjCInterfaceDecl *Decl,
ObjCInterfaceDecl *PrevDecl = nullptr) const;


QualType getObjCObjectType(QualType Base,
ObjCProtocolDecl * const *Protocols,
unsigned NumProtocols) const;

QualType getObjCObjectType(QualType Base,
ArrayRef<QualType> typeArgs,
ArrayRef<ObjCProtocolDecl *> protocols,
bool isKindOf) const;

QualType getObjCTypeParamType(const ObjCTypeParamDecl *Decl,
ArrayRef<ObjCProtocolDecl *> protocols) const;
void adjustObjCTypeParamBoundType(const ObjCTypeParamDecl *Orig,
ObjCTypeParamDecl *New) const;

bool ObjCObjectAdoptsQTypeProtocols(QualType QT, ObjCInterfaceDecl *Decl);




bool QIdProtocolsAdoptObjCObjectProtocols(QualType QT,
ObjCInterfaceDecl *IDecl);


QualType getObjCObjectPointerType(QualType OIT) const;


QualType getTypeOfExprType(Expr *e) const;
QualType getTypeOfType(QualType t) const;


QualType getDecltypeType(Expr *e, QualType UnderlyingType) const;


QualType getUnaryTransformType(QualType BaseType, QualType UnderlyingType,
UnaryTransformType::UTTKind UKind) const;


QualType getAutoType(QualType DeducedType, AutoTypeKeyword Keyword,
bool IsDependent, bool IsPack = false,
ConceptDecl *TypeConstraintConcept = nullptr,
ArrayRef<TemplateArgument> TypeConstraintArgs ={}) const;


QualType getAutoDeductType() const;


QualType getAutoRRefDeductType() const;


QualType getDeducedTemplateSpecializationType(TemplateName Template,
QualType DeducedType,
bool IsDependent) const;



QualType getTagDeclType(const TagDecl *Decl) const;





CanQualType getSizeType() const;



CanQualType getSignedSizeType() const;



CanQualType getIntMaxType() const;



CanQualType getUIntMaxType() const;



QualType getWCharType() const { return WCharTy; }




QualType getWideCharType() const { return WideCharTy; }




QualType getSignedWCharType() const;




QualType getUnsignedWCharType() const;



QualType getWIntType() const { return WIntTy; }



QualType getIntPtrType() const;



QualType getUIntPtrType() const;



QualType getPointerDiffType() const;




QualType getUnsignedPointerDiffType() const;



QualType getProcessIDType() const;


QualType getCFConstantStringType() const;


QualType getObjCSuperType() const;
void setObjCSuperType(QualType ST) { ObjCSuperType = ST; }



QualType getRawCFConstantStringType() const {
if (CFConstantStringTypeDecl)
return getTypedefType(CFConstantStringTypeDecl);
return QualType();
}
void setCFConstantStringType(QualType T);
TypedefDecl *getCFConstantStringDecl() const;
RecordDecl *getCFConstantStringTagDecl() const;


void setObjCConstantStringInterface(ObjCInterfaceDecl *Decl);
QualType getObjCConstantStringInterface() const {
return ObjCConstantStringType;
}

QualType getObjCNSStringType() const {
return ObjCNSStringType;
}

void setObjCNSStringType(QualType T) {
ObjCNSStringType = T;
}



QualType getObjCIdRedefinitionType() const {
if (ObjCIdRedefinitionType.isNull())
return getObjCIdType();
return ObjCIdRedefinitionType;
}


void setObjCIdRedefinitionType(QualType RedefType) {
ObjCIdRedefinitionType = RedefType;
}



QualType getObjCClassRedefinitionType() const {
if (ObjCClassRedefinitionType.isNull())
return getObjCClassType();
return ObjCClassRedefinitionType;
}


void setObjCClassRedefinitionType(QualType RedefType) {
ObjCClassRedefinitionType = RedefType;
}



QualType getObjCSelRedefinitionType() const {
if (ObjCSelRedefinitionType.isNull())
return getObjCSelType();
return ObjCSelRedefinitionType;
}


void setObjCSelRedefinitionType(QualType RedefType) {
ObjCSelRedefinitionType = RedefType;
}


IdentifierInfo *getNSObjectName() const {
if (!NSObjectName) {
NSObjectName = &Idents.get("NSObject");
}

return NSObjectName;
}


IdentifierInfo *getNSCopyingName() {
if (!NSCopyingName) {
NSCopyingName = &Idents.get("NSCopying");
}

return NSCopyingName;
}

CanQualType getNSUIntegerType() const;

CanQualType getNSIntegerType() const;


IdentifierInfo *getBoolName() const {
if (!BoolName)
BoolName = &Idents.get("bool");
return BoolName;
}

IdentifierInfo *getMakeIntegerSeqName() const {
if (!MakeIntegerSeqName)
MakeIntegerSeqName = &Idents.get("__make_integer_seq");
return MakeIntegerSeqName;
}

IdentifierInfo *getTypePackElementName() const {
if (!TypePackElementName)
TypePackElementName = &Idents.get("__type_pack_element");
return TypePackElementName;
}



QualType getObjCInstanceType() {
return getTypeDeclType(getObjCInstanceTypeDecl());
}



TypedefDecl *getObjCInstanceTypeDecl();


void setFILEDecl(TypeDecl *FILEDecl) { this->FILEDecl = FILEDecl; }


QualType getFILEType() const {
if (FILEDecl)
return getTypeDeclType(FILEDecl);
return QualType();
}


void setjmp_bufDecl(TypeDecl *jmp_bufDecl) {
this->jmp_bufDecl = jmp_bufDecl;
}


QualType getjmp_bufType() const {
if (jmp_bufDecl)
return getTypeDeclType(jmp_bufDecl);
return QualType();
}


void setsigjmp_bufDecl(TypeDecl *sigjmp_bufDecl) {
this->sigjmp_bufDecl = sigjmp_bufDecl;
}


QualType getsigjmp_bufType() const {
if (sigjmp_bufDecl)
return getTypeDeclType(sigjmp_bufDecl);
return QualType();
}


void setucontext_tDecl(TypeDecl *ucontext_tDecl) {
this->ucontext_tDecl = ucontext_tDecl;
}


QualType getucontext_tType() const {
if (ucontext_tDecl)
return getTypeDeclType(ucontext_tDecl);
return QualType();
}


QualType getLogicalOperationType() const {
return getLangOpts().CPlusPlus ? BoolTy : IntTy;
}





void getObjCEncodingForType(QualType T, std::string &S,
const FieldDecl *Field=nullptr,
QualType *NotEncodedT=nullptr) const;



void getObjCEncodingForPropertyType(QualType T, std::string &S) const;

void getLegacyIntegralTypeEncoding(QualType &t) const;


void getObjCEncodingForTypeQualifier(Decl::ObjCDeclQualifier QT,
std::string &S) const;







std::string getObjCEncodingForFunctionDecl(const FunctionDecl *Decl) const;



std::string getObjCEncodingForMethodDecl(const ObjCMethodDecl *Decl,
bool Extended = false) const;


std::string getObjCEncodingForBlock(const BlockExpr *blockExpr) const;





std::string getObjCEncodingForPropertyDecl(const ObjCPropertyDecl *PD,
const Decl *Container) const;

bool ProtocolCompatibleWithProtocol(ObjCProtocolDecl *lProto,
ObjCProtocolDecl *rProto) const;

ObjCPropertyImplDecl *getObjCPropertyImplDeclForPropertyDecl(
const ObjCPropertyDecl *PD,
const Decl *Container) const;



CharUnits getObjCEncodingTypeSize(QualType T) const;



TypedefDecl *getObjCIdDecl() const;





QualType getObjCIdType() const {
return getTypeDeclType(getObjCIdDecl());
}



TypedefDecl *getObjCSelDecl() const;



QualType getObjCSelType() const {
return getTypeDeclType(getObjCSelDecl());
}



TypedefDecl *getObjCClassDecl() const;





QualType getObjCClassType() const {
return getTypeDeclType(getObjCClassDecl());
}



ObjCInterfaceDecl *getObjCProtocolDecl() const;


TypedefDecl *getBOOLDecl() const {
return BOOLDecl;
}


void setBOOLDecl(TypedefDecl *TD) {
BOOLDecl = TD;
}


QualType getBOOLType() const {
return getTypeDeclType(getBOOLDecl());
}


QualType getObjCProtoType() const {
return getObjCInterfaceType(getObjCProtocolDecl());
}



TypedefDecl *getBuiltinVaListDecl() const;


QualType getBuiltinVaListType() const {
return getTypeDeclType(getBuiltinVaListDecl());
}




Decl *getVaListTagDecl() const;



TypedefDecl *getBuiltinMSVaListDecl() const;


QualType getBuiltinMSVaListType() const {
return getTypeDeclType(getBuiltinMSVaListDecl());
}


TagDecl *getMSGuidTagDecl() const { return MSGuidTagDecl; }


QualType getMSGuidType() const {
assert(MSGuidTagDecl && "asked for GUID type but MS extensions disabled");
return getTagDeclType(MSGuidTagDecl);
}



bool canBuiltinBeRedeclared(const FunctionDecl *) const;



QualType getCVRQualifiedType(QualType T, unsigned CVR) const {
return getQualifiedType(T, Qualifiers::fromCVRMask(CVR));
}


QualType getQualifiedType(SplitQualType split) const {
return getQualifiedType(split.Ty, split.Quals);
}


QualType getQualifiedType(QualType T, Qualifiers Qs) const {
if (!Qs.hasNonFastQualifiers())
return T.withFastQualifiers(Qs.getFastQualifiers());
QualifierCollector Qc(Qs);
const Type *Ptr = Qc.strip(T);
return getExtQualType(Ptr, Qc);
}


QualType getQualifiedType(const Type *T, Qualifiers Qs) const {
if (!Qs.hasNonFastQualifiers())
return QualType(T, Qs.getFastQualifiers());
return getExtQualType(T, Qs);
}




QualType getLifetimeQualifiedType(QualType type,
Qualifiers::ObjCLifetime lifetime) {
assert(type.getObjCLifetime() == Qualifiers::OCL_None);
assert(lifetime != Qualifiers::OCL_None);

Qualifiers qs;
qs.addObjCLifetime(lifetime);
return getQualifiedType(type, qs);
}



QualType getUnqualifiedObjCPointerType(QualType type) const {
if (!type.getTypePtr()->isObjCObjectPointerType() ||
!type.getQualifiers().hasObjCLifetime())
return type;
Qualifiers Qs = type.getQualifiers();
Qs.removeObjCLifetime();
return getQualifiedType(type.getUnqualifiedType(), Qs);
}

unsigned char getFixedPointScale(QualType Ty) const;
unsigned char getFixedPointIBits(QualType Ty) const;
llvm::FixedPointSemantics getFixedPointSemantics(QualType Ty) const;
llvm::APFixedPoint getFixedPointMax(QualType Ty) const;
llvm::APFixedPoint getFixedPointMin(QualType Ty) const;

DeclarationNameInfo getNameForTemplate(TemplateName Name,
SourceLocation NameLoc) const;

TemplateName getOverloadedTemplateName(UnresolvedSetIterator Begin,
UnresolvedSetIterator End) const;
TemplateName getAssumedTemplateName(DeclarationName Name) const;

TemplateName getQualifiedTemplateName(NestedNameSpecifier *NNS,
bool TemplateKeyword,
TemplateDecl *Template) const;

TemplateName getDependentTemplateName(NestedNameSpecifier *NNS,
const IdentifierInfo *Name) const;
TemplateName getDependentTemplateName(NestedNameSpecifier *NNS,
OverloadedOperatorKind Operator) const;
TemplateName getSubstTemplateTemplateParm(TemplateTemplateParmDecl *param,
TemplateName replacement) const;
TemplateName getSubstTemplateTemplateParmPack(TemplateTemplateParmDecl *Param,
const TemplateArgument &ArgPack) const;

enum GetBuiltinTypeError {

GE_None,


GE_Missing_type,


GE_Missing_stdio,


GE_Missing_setjmp,


GE_Missing_ucontext
};

QualType DecodeTypeStr(const char *&Str, const ASTContext &Context,
ASTContext::GetBuiltinTypeError &Error,
bool &RequireICE, bool AllowTypeModifiers) const;






QualType GetBuiltinType(unsigned ID, GetBuiltinTypeError &Error,
unsigned *IntegerConstantArgs = nullptr) const;



ComparisonCategories CompCategories;

private:
CanQualType getFromTargetType(unsigned Type) const;
TypeInfo getTypeInfoImpl(const Type *T) const;





public:


Qualifiers::GC getObjCGCAttrKind(QualType Ty) const;






bool areCompatibleVectorTypes(QualType FirstVec, QualType SecondVec);




bool areCompatibleSveTypes(QualType FirstType, QualType SecondType);



bool areLaxCompatibleSveTypes(QualType FirstType, QualType SecondType);




bool hasDirectOwnershipQualifier(QualType Ty) const;



static bool isObjCNSObjectType(QualType Ty) {
return Ty->isObjCNSObjectType();
}







const llvm::fltSemantics &getFloatTypeSemantics(QualType T) const;


TypeInfo getTypeInfo(const Type *T) const;
TypeInfo getTypeInfo(QualType T) const { return getTypeInfo(T.getTypePtr()); }


unsigned getOpenMPDefaultSimdAlign(QualType T) const;


uint64_t getTypeSize(QualType T) const { return getTypeInfo(T).Width; }
uint64_t getTypeSize(const Type *T) const { return getTypeInfo(T).Width; }


uint64_t getCharWidth() const {
return getTypeSize(CharTy);
}


CharUnits toCharUnitsFromBits(int64_t BitSize) const;


int64_t toBits(CharUnits CharSize) const;



CharUnits getTypeSizeInChars(QualType T) const;
CharUnits getTypeSizeInChars(const Type *T) const;

Optional<CharUnits> getTypeSizeInCharsIfKnown(QualType Ty) const {
if (Ty->isIncompleteType() || Ty->isDependentType())
return None;
return getTypeSizeInChars(Ty);
}

Optional<CharUnits> getTypeSizeInCharsIfKnown(const Type *Ty) const {
return getTypeSizeInCharsIfKnown(QualType(Ty, 0));
}



unsigned getTypeAlign(QualType T) const { return getTypeInfo(T).Align; }
unsigned getTypeAlign(const Type *T) const { return getTypeInfo(T).Align; }






unsigned getTypeUnadjustedAlign(QualType T) const {
return getTypeUnadjustedAlign(T.getTypePtr());
}
unsigned getTypeUnadjustedAlign(const Type *T) const;






unsigned getTypeAlignIfKnown(QualType T,
bool NeedsPreferredAlignment = false) const;



CharUnits getTypeAlignInChars(QualType T) const;
CharUnits getTypeAlignInChars(const Type *T) const;



CharUnits getPreferredTypeAlignInChars(QualType T) const {
return toCharUnitsFromBits(getPreferredTypeAlign(T));
}




CharUnits getTypeUnadjustedAlignInChars(QualType T) const;
CharUnits getTypeUnadjustedAlignInChars(const Type *T) const;



TypeInfoChars getTypeInfoDataSizeInChars(QualType T) const;

TypeInfoChars getTypeInfoInChars(const Type *T) const;
TypeInfoChars getTypeInfoInChars(QualType T) const;



bool isAlignmentRequired(const Type *T) const;
bool isAlignmentRequired(QualType T) const;








unsigned getPreferredTypeAlign(QualType T) const {
return getPreferredTypeAlign(T.getTypePtr());
}
unsigned getPreferredTypeAlign(const Type *T) const;



unsigned getTargetDefaultAlignForAttributeAligned() const;



unsigned getAlignOfGlobalVar(QualType T) const;



CharUnits getAlignOfGlobalVarInChars(QualType T) const;











CharUnits getDeclAlign(const Decl *D, bool ForAlignof = false) const;




CharUnits getExnObjectAlignment() const;




const ASTRecordLayout &getASTRecordLayout(const RecordDecl *D) const;



const ASTRecordLayout &getASTObjCInterfaceLayout(const ObjCInterfaceDecl *D)
const;

void DumpRecordLayout(const RecordDecl *RD, raw_ostream &OS,
bool Simple = false) const;





const ASTRecordLayout &
getASTObjCImplementationLayout(const ObjCImplementationDecl *D) const;











const CXXMethodDecl *getCurrentKeyFunction(const CXXRecordDecl *RD);









void setNonKeyFunction(const CXXMethodDecl *method);










CharUnits getOffsetOfBaseWithVBPtr(const CXXRecordDecl *RD) const;


uint64_t getFieldOffset(const ValueDecl *FD) const;


uint64_t lookupFieldBitOffset(const ObjCInterfaceDecl *OID,
const ObjCImplementationDecl *ID,
const ObjCIvarDecl *Ivar) const;



CharUnits getMemberPointerPathAdjustment(const APValue &MP) const;

bool isNearlyEmpty(const CXXRecordDecl *RD) const;

VTableContextBase *getVTableContext();


MangleContext *createMangleContext(const TargetInfo *T = nullptr);





MangleContext *createDeviceMangleContext(const TargetInfo &T);

void DeepCollectObjCIvars(const ObjCInterfaceDecl *OI, bool leafClass,
SmallVectorImpl<const ObjCIvarDecl*> &Ivars) const;

unsigned CountNonClassIvars(const ObjCInterfaceDecl *OI) const;
void CollectInheritedProtocols(const Decl *CDecl,
llvm::SmallPtrSet<ObjCProtocolDecl*, 8> &Protocols);



bool hasUniqueObjectRepresentations(QualType Ty) const;













CanQualType getCanonicalType(QualType T) const {
return CanQualType::CreateUnsafe(T.getCanonicalType());
}

const Type *getCanonicalType(const Type *T) const {
return T->getCanonicalTypeInternal().getTypePtr();
}






CanQualType getCanonicalParamType(QualType T) const;


bool hasSameType(QualType T1, QualType T2) const {
return getCanonicalType(T1) == getCanonicalType(T2);
}
bool hasSameType(const Type *T1, const Type *T2) const {
return getCanonicalType(T1) == getCanonicalType(T2);
}














QualType getUnqualifiedArrayType(QualType T, Qualifiers &Quals);



bool hasSameUnqualifiedType(QualType T1, QualType T2) const {
return getCanonicalType(T1).getTypePtr() ==
getCanonicalType(T2).getTypePtr();
}

bool hasSameNullabilityTypeQualifier(QualType SubT, QualType SuperT,
bool IsParam) const {
auto SubTnullability = SubT->getNullability(*this);
auto SuperTnullability = SuperT->getNullability(*this);
if (SubTnullability.hasValue() == SuperTnullability.hasValue()) {

if (!SubTnullability)
return true;

if (*SubTnullability == *SuperTnullability ||
*SubTnullability == NullabilityKind::Unspecified ||
*SuperTnullability == NullabilityKind::Unspecified)
return true;

if (IsParam) {


return (*SuperTnullability == NullabilityKind::NonNull &&
*SubTnullability == NullabilityKind::Nullable);
}


return (*SuperTnullability == NullabilityKind::Nullable &&
*SubTnullability == NullabilityKind::NonNull);
}
return true;
}

bool ObjCMethodsAreEqual(const ObjCMethodDecl *MethodDecl,
const ObjCMethodDecl *MethodImp);

bool UnwrapSimilarTypes(QualType &T1, QualType &T2);
void UnwrapSimilarArrayTypes(QualType &T1, QualType &T2);








bool hasSimilarType(QualType T1, QualType T2);


bool hasCvrSimilarType(QualType T1, QualType T2);
























NestedNameSpecifier *
getCanonicalNestedNameSpecifier(NestedNameSpecifier *NNS) const;


CallingConv getDefaultCallingConvention(bool IsVariadic,
bool IsCXXMethod,
bool IsBuiltin = false) const;



















TemplateName getCanonicalTemplateName(TemplateName Name) const;



bool hasSameTemplateName(TemplateName X, TemplateName Y);






TemplateArgument getCanonicalTemplateArgument(const TemplateArgument &Arg)
const;





const ArrayType *getAsArrayType(QualType T) const;
const ConstantArrayType *getAsConstantArrayType(QualType T) const {
return dyn_cast_or_null<ConstantArrayType>(getAsArrayType(T));
}
const VariableArrayType *getAsVariableArrayType(QualType T) const {
return dyn_cast_or_null<VariableArrayType>(getAsArrayType(T));
}
const IncompleteArrayType *getAsIncompleteArrayType(QualType T) const {
return dyn_cast_or_null<IncompleteArrayType>(getAsArrayType(T));
}
const DependentSizedArrayType *getAsDependentSizedArrayType(QualType T)
const {
return dyn_cast_or_null<DependentSizedArrayType>(getAsArrayType(T));
}




QualType getBaseElementType(const ArrayType *VAT) const;



QualType getBaseElementType(QualType QT) const;


uint64_t getConstantArrayElementCount(const ConstantArrayType *CA) const;






QualType getAdjustedParameterType(QualType T) const;




QualType getSignatureParameterType(QualType T) const;

QualType getExceptionObjectType(QualType T) const;









QualType getArrayDecayedType(QualType T) const;



QualType getPromotedIntegerType(QualType PromotableType) const;



Qualifiers::ObjCLifetime getInnerObjCOwnership(QualType T) const;






QualType isPromotableBitField(Expr *E) const;





int getIntegerTypeOrder(QualType LHS, QualType RHS) const;






int getFloatingTypeOrder(QualType LHS, QualType RHS) const;




int getFloatingTypeSemanticOrder(QualType LHS, QualType RHS) const;






QualType getFloatingTypeOfSizeWithinDomain(QualType typeSize,
QualType typeDomain) const;

unsigned getTargetAddressSpace(QualType T) const {
return getTargetAddressSpace(T.getQualifiers());
}

unsigned getTargetAddressSpace(Qualifiers Q) const {
return getTargetAddressSpace(Q.getAddressSpace());
}

unsigned getTargetAddressSpace(LangAS AS) const;

LangAS getLangASForBuiltinAddressSpace(unsigned AS) const;



uint64_t getTargetNullPointerValue(QualType QT) const;

bool addressSpaceMapManglingFor(LangAS AS) const {
return AddrSpaceMapMangling || isTargetAddressSpace(AS);
}

private:

unsigned getIntegerRank(const Type *T) const;

public:





bool typesAreCompatible(QualType T1, QualType T2,
bool CompareUnqualified = false);

bool propertyTypesAreCompatible(QualType, QualType);
bool typesAreBlockPointerCompatible(QualType, QualType);

bool isObjCIdType(QualType T) const {
return T == getObjCIdType();
}

bool isObjCClassType(QualType T) const {
return T == getObjCClassType();
}

bool isObjCSelType(QualType T) const {
return T == getObjCSelType();
}

bool ObjCQualifiedIdTypesAreCompatible(const ObjCObjectPointerType *LHS,
const ObjCObjectPointerType *RHS,
bool ForCompare);

bool ObjCQualifiedClassTypesAreCompatible(const ObjCObjectPointerType *LHS,
const ObjCObjectPointerType *RHS);


bool canAssignObjCInterfaces(const ObjCObjectPointerType *LHSOPT,
const ObjCObjectPointerType *RHSOPT);
bool canAssignObjCInterfaces(const ObjCObjectType *LHS,
const ObjCObjectType *RHS);
bool canAssignObjCInterfacesInBlockPointer(
const ObjCObjectPointerType *LHSOPT,
const ObjCObjectPointerType *RHSOPT,
bool BlockReturnType);
bool areComparableObjCPointerTypes(QualType LHS, QualType RHS);
QualType areCommonBaseCompatible(const ObjCObjectPointerType *LHSOPT,
const ObjCObjectPointerType *RHSOPT);
bool canBindObjCObjectType(QualType To, QualType From);


QualType mergeTypes(QualType, QualType, bool OfBlockPointer=false,
bool Unqualified = false, bool BlockReturnType = false);
QualType mergeFunctionTypes(QualType, QualType, bool OfBlockPointer=false,
bool Unqualified = false, bool AllowCXX = false);
QualType mergeFunctionParameterTypes(QualType, QualType,
bool OfBlockPointer = false,
bool Unqualified = false);
QualType mergeTransparentUnionType(QualType, QualType,
bool OfBlockPointer=false,
bool Unqualified = false);

QualType mergeObjCGCQualifiers(QualType, QualType);




















bool mergeExtParameterInfo(
const FunctionProtoType *FirstFnType,
const FunctionProtoType *SecondFnType,
bool &CanUseFirst, bool &CanUseSecond,
SmallVectorImpl<FunctionProtoType::ExtParameterInfo> &NewParamInfos);

void ResetObjCLayout(const ObjCContainerDecl *CD);







unsigned getIntWidth(QualType T) const;







QualType getCorrespondingUnsignedType(QualType T) const;







QualType getCorrespondingSignedType(QualType T) const;



QualType getCorrespondingSaturatedType(QualType Ty) const;





QualType getCorrespondingSignedFixedPointType(QualType Ty) const;







llvm::APSInt MakeIntValue(uint64_t Value, QualType Type) const {


llvm::APSInt Res(64, !Type->isSignedIntegerOrEnumerationType());
Res = Value;
unsigned Width = getIntWidth(Type);
if (Width != Res.getBitWidth())
return Res.extOrTrunc(Width);
return Res;
}

bool isSentinelNullExpr(const Expr *E);



ObjCImplementationDecl *getObjCImplementation(ObjCInterfaceDecl *D);



ObjCCategoryImplDecl *getObjCImplementation(ObjCCategoryDecl *D);


bool AnyObjCImplementation() {
return !ObjCImpls.empty();
}


void setObjCImplementation(ObjCInterfaceDecl *IFaceD,
ObjCImplementationDecl *ImplD);


void setObjCImplementation(ObjCCategoryDecl *CatD,
ObjCCategoryImplDecl *ImplD);



const ObjCMethodDecl *
getObjCMethodRedeclaration(const ObjCMethodDecl *MD) const;

void setObjCMethodRedeclaration(const ObjCMethodDecl *MD,
const ObjCMethodDecl *Redecl);




const ObjCInterfaceDecl *getObjContainingInterface(const NamedDecl *ND) const;



void setBlockVarCopyInit(const VarDecl* VD, Expr *CopyExpr, bool CanThrow);



BlockVarCopyInit getBlockVarCopyInit(const VarDecl* VD) const;












TypeSourceInfo *CreateTypeSourceInfo(QualType T, unsigned Size = 0) const;




TypeSourceInfo *
getTrivialTypeSourceInfo(QualType T,
SourceLocation Loc = SourceLocation()) const;








void AddDeallocation(void (*Callback)(void *), void *Data) const;



template <typename T> void addDestruction(T *Ptr) const {
if (!std::is_trivially_destructible<T>::value) {
auto DestroyPtr = [](void *V) { static_cast<T *>(V)->~T(); };
AddDeallocation(DestroyPtr, Ptr);
}
}

GVALinkage GetGVALinkageForFunction(const FunctionDecl *FD) const;
GVALinkage GetGVALinkageForVariable(const VarDecl *VD);







bool DeclMustBeEmitted(const Decl *D);



void forEachMultiversionedFunctionVersion(
const FunctionDecl *FD,
llvm::function_ref<void(FunctionDecl *)> Pred) const;

const CXXConstructorDecl *
getCopyConstructorForExceptionObject(CXXRecordDecl *RD);

void addCopyConstructorForExceptionObject(CXXRecordDecl *RD,
CXXConstructorDecl *CD);

void addTypedefNameForUnnamedTagDecl(TagDecl *TD, TypedefNameDecl *TND);

TypedefNameDecl *getTypedefNameForUnnamedTagDecl(const TagDecl *TD);

void addDeclaratorForUnnamedTagDecl(TagDecl *TD, DeclaratorDecl *DD);

DeclaratorDecl *getDeclaratorForUnnamedTagDecl(const TagDecl *TD);

void setManglingNumber(const NamedDecl *ND, unsigned Number);
unsigned getManglingNumber(const NamedDecl *ND) const;

void setStaticLocalNumber(const VarDecl *VD, unsigned Number);
unsigned getStaticLocalNumber(const VarDecl *VD) const;



MangleNumberingContext &getManglingNumberContext(const DeclContext *DC);
enum NeedExtraManglingDecl_t { NeedExtraManglingDecl };
MangleNumberingContext &getManglingNumberContext(NeedExtraManglingDecl_t,
const Decl *D);

std::unique_ptr<MangleNumberingContext> createMangleNumberingContext() const;



void setParameterIndex(const ParmVarDecl *D, unsigned index);



unsigned getParameterIndex(const ParmVarDecl *D) const;




StringLiteral *getPredefinedStringLiteralFromCache(StringRef Key) const;



MSGuidDecl *getMSGuidDecl(MSGuidDeclParts Parts) const;



TemplateParamObjectDecl *getTemplateParamObjectDecl(QualType T,
const APValue &V) const;



ParsedTargetAttr filterFunctionTargetAttrs(const TargetAttr *TD) const;

void getFunctionFeatureMap(llvm::StringMap<bool> &FeatureMap,
const FunctionDecl *) const;
void getFunctionFeatureMap(llvm::StringMap<bool> &FeatureMap,
GlobalDecl GD) const;






unsigned NumImplicitDefaultConstructors = 0;



unsigned NumImplicitDefaultConstructorsDeclared = 0;


unsigned NumImplicitCopyConstructors = 0;



unsigned NumImplicitCopyConstructorsDeclared = 0;


unsigned NumImplicitMoveConstructors = 0;



unsigned NumImplicitMoveConstructorsDeclared = 0;


unsigned NumImplicitCopyAssignmentOperators = 0;



unsigned NumImplicitCopyAssignmentOperatorsDeclared = 0;


unsigned NumImplicitMoveAssignmentOperators = 0;



unsigned NumImplicitMoveAssignmentOperatorsDeclared = 0;


unsigned NumImplicitDestructors = 0;



unsigned NumImplicitDestructorsDeclared = 0;

public:






void InitBuiltinTypes(const TargetInfo &Target,
const TargetInfo *AuxTarget = nullptr);

private:
void InitBuiltinType(CanQualType &R, BuiltinType::Kind K);

class ObjCEncOptions {
unsigned Bits;

ObjCEncOptions(unsigned Bits) : Bits(Bits) {}

public:
ObjCEncOptions() : Bits(0) {}
ObjCEncOptions(const ObjCEncOptions &RHS) : Bits(RHS.Bits) {}

#define OPT_LIST(V) V(ExpandPointedToStructures, 0) V(ExpandStructures, 1) V(IsOutermostType, 2) V(EncodingProperty, 3) V(IsStructField, 4) V(EncodeBlockParameters, 5) V(EncodeClassNames, 6)








#define V(N,I) ObjCEncOptions& set##N() { Bits |= 1 << I; return *this; }
OPT_LIST(V)
#undef V

#define V(N,I) bool N() const { return Bits & 1 << I; }
OPT_LIST(V)
#undef V

#undef OPT_LIST

LLVM_NODISCARD ObjCEncOptions keepingOnly(ObjCEncOptions Mask) const {
return Bits & Mask.Bits;
}

LLVM_NODISCARD ObjCEncOptions forComponentType() const {
ObjCEncOptions Mask = ObjCEncOptions()
.setIsOutermostType()
.setIsStructField();
return Bits & ~Mask.Bits;
}
};


void getObjCEncodingForTypeImpl(QualType t, std::string &S,
ObjCEncOptions Options,
const FieldDecl *Field,
QualType *NotEncodedT = nullptr) const;


void getObjCEncodingForStructureImpl(RecordDecl *RD, std::string &S,
const FieldDecl *Field,
bool includeVBases = true,
QualType *NotEncodedT=nullptr) const;

public:

void getObjCEncodingForMethodParameter(Decl::ObjCDeclQualifier QT,
QualType T, std::string& S,
bool Extended) const;



bool isMSStaticDataMemberInlineDefinition(const VarDecl *VD) const;

enum class InlineVariableDefinitionKind {

None,


Weak,


WeakUnknown,


Strong
};






InlineVariableDefinitionKind
getInlineVariableDefinitionKind(const VarDecl *VD) const;

private:
friend class DeclarationNameTable;
friend class DeclContext;

const ASTRecordLayout &
getObjCLayout(const ObjCInterfaceDecl *D,
const ObjCImplementationDecl *Impl) const;









using DeallocationFunctionsAndArguments =
llvm::SmallVector<std::pair<void (*)(void *), void *>, 16>;
mutable DeallocationFunctionsAndArguments Deallocations;




llvm::PointerIntPair<StoredDeclsMap *, 1> LastSDM;

std::vector<Decl *> TraversalScope;

std::unique_ptr<VTableContextBase> VTContext;

void ReleaseDeclContextMaps();

public:
enum PragmaSectionFlag : unsigned {
PSF_None = 0,
PSF_Read = 0x1,
PSF_Write = 0x2,
PSF_Execute = 0x4,
PSF_Implicit = 0x8,
PSF_ZeroInit = 0x10,
PSF_Invalid = 0x80000000U,
};

struct SectionInfo {
NamedDecl *Decl;
SourceLocation PragmaSectionLocation;
int SectionFlags;

SectionInfo() = default;
SectionInfo(NamedDecl *Decl, SourceLocation PragmaSectionLocation,
int SectionFlags)
: Decl(Decl), PragmaSectionLocation(PragmaSectionLocation),
SectionFlags(SectionFlags) {}
};

llvm::StringMap<SectionInfo> SectionInfos;


OMPTraitInfo &getNewOMPTraitInfo();


bool mayExternalizeStaticVar(const Decl *D) const;


bool shouldExternalizeStaticVar(const Decl *D) const;

StringRef getCUIDHash() const;

void AddSYCLKernelNamingDecl(const CXXRecordDecl *RD);
bool IsSYCLKernelNamingDecl(const NamedDecl *RD) const;
unsigned GetSYCLKernelNamingIndex(const NamedDecl *RD);



llvm::MapVector<const SYCLUniqueStableNameExpr *, std::string>
SYCLUniqueStableNameEvaluatedValues;

private:


SmallVector<std::unique_ptr<OMPTraitInfo>, 4> OMPTraitInfoVector;





llvm::DenseMap<const DeclContext *,
llvm::SmallPtrSet<const CXXRecordDecl *, 4>>
SYCLKernelNamingTypes;
std::unique_ptr<ItaniumMangleContext> SYCLKernelFilterContext;
void FilterSYCLKernelNamingDecls(
const CXXRecordDecl *RD,
llvm::SmallVectorImpl<const CXXRecordDecl *> &Decls);
};


const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
const ASTContext::SectionInfo &Section);


inline Selector GetNullarySelector(StringRef name, ASTContext &Ctx) {
IdentifierInfo* II = &Ctx.Idents.get(name);
return Ctx.Selectors.getSelector(0, &II);
}


inline Selector GetUnarySelector(StringRef name, ASTContext &Ctx) {
IdentifierInfo* II = &Ctx.Idents.get(name);
return Ctx.Selectors.getSelector(1, &II);
}

}































inline void *operator new(size_t Bytes, const clang::ASTContext &C,
size_t Alignment ) {
return C.Allocate(Bytes, Alignment);
}







inline void operator delete(void *Ptr, const clang::ASTContext &C, size_t) {
C.Deallocate(Ptr);
}
























inline void *operator new[](size_t Bytes, const clang::ASTContext& C,
size_t Alignment ) {
return C.Allocate(Bytes, Alignment);
}







inline void operator delete[](void *Ptr, const clang::ASTContext &C, size_t) {
C.Deallocate(Ptr);
}


template <typename Owner, typename T,
void (clang::ExternalASTSource::*Update)(Owner)>
typename clang::LazyGenerationalUpdatePtr<Owner, T, Update>::ValueType
clang::LazyGenerationalUpdatePtr<Owner, T, Update>::makeValue(
const clang::ASTContext &Ctx, T Value) {



if (auto *Source = Ctx.getExternalSource())
return new (Ctx) LazyData(Source, Value);
return Value;
}

#endif
