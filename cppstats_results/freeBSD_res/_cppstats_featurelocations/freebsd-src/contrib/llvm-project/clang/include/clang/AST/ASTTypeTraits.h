













#if !defined(LLVM_CLANG_AST_ASTTYPETRAITS_H)
#define LLVM_CLANG_AST_ASTTYPETRAITS_H

#include "clang/AST/ASTFwd.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/TypeLoc.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Support/AlignOf.h"

namespace llvm {

class raw_ostream;

}

namespace clang {

struct PrintingPolicy;



enum TraversalKind {

TK_AsIs,


TK_IgnoreUnlessSpelledInSource
};






class ASTNodeKind {
public:

ASTNodeKind() : KindId(NKI_None) {}


template <class T>
static ASTNodeKind getFromNodeKind() {
return ASTNodeKind(KindToKindId<T>::Id);
}



static ASTNodeKind getFromNode(const Decl &D);
static ASTNodeKind getFromNode(const Stmt &S);
static ASTNodeKind getFromNode(const Type &T);
static ASTNodeKind getFromNode(const OMPClause &C);



bool isSame(ASTNodeKind Other) const {
return KindId != NKI_None && KindId == Other.KindId;
}


bool isNone() const { return KindId == NKI_None; }




bool isBaseOf(ASTNodeKind Other, unsigned *Distance = nullptr) const;


StringRef asStringRef() const;


bool operator<(const ASTNodeKind &Other) const {
return KindId < Other.KindId;
}




static ASTNodeKind getMostDerivedType(ASTNodeKind Kind1, ASTNodeKind Kind2);




static ASTNodeKind getMostDerivedCommonAncestor(ASTNodeKind Kind1,
ASTNodeKind Kind2);

ASTNodeKind getCladeKind() const;


struct DenseMapInfo {

static inline ASTNodeKind getEmptyKey() { return ASTNodeKind(); }


static inline ASTNodeKind getTombstoneKey() {
return ASTNodeKind(NKI_NumberOfKinds);
}
static unsigned getHashValue(const ASTNodeKind &Val) { return Val.KindId; }
static bool isEqual(const ASTNodeKind &LHS, const ASTNodeKind &RHS) {
return LHS.KindId == RHS.KindId;
}
};



bool hasPointerIdentity() const {
return KindId > NKI_LastKindWithoutPointerIdentity;
}

private:



enum NodeKindId {
NKI_None,
NKI_TemplateArgument,
NKI_TemplateArgumentLoc,
NKI_TemplateName,
NKI_NestedNameSpecifierLoc,
NKI_QualType,
NKI_TypeLoc,
NKI_LastKindWithoutPointerIdentity = NKI_TypeLoc,
NKI_CXXBaseSpecifier,
NKI_CXXCtorInitializer,
NKI_NestedNameSpecifier,
NKI_Decl,
#define DECL(DERIVED, BASE) NKI_##DERIVED##Decl,
#include "clang/AST/DeclNodes.inc"
NKI_Stmt,
#define STMT(DERIVED, BASE) NKI_##DERIVED,
#include "clang/AST/StmtNodes.inc"
NKI_Type,
#define TYPE(DERIVED, BASE) NKI_##DERIVED##Type,
#include "clang/AST/TypeNodes.inc"
NKI_OMPClause,
#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) NKI_##Class,
#include "llvm/Frontend/OpenMP/OMP.inc"
NKI_NumberOfKinds
};


ASTNodeKind(NodeKindId KindId) : KindId(KindId) {}





static bool isBaseOf(NodeKindId Base, NodeKindId Derived, unsigned *Distance);




template <class T> struct KindToKindId {
static const NodeKindId Id = NKI_None;
};
template <class T>
struct KindToKindId<const T> : KindToKindId<T> {};


struct KindInfo {

NodeKindId ParentId;

const char *Name;
};
static const KindInfo AllKindInfo[NKI_NumberOfKinds];

NodeKindId KindId;
};

#define KIND_TO_KIND_ID(Class) template <> struct ASTNodeKind::KindToKindId<Class> { static const NodeKindId Id = NKI_##Class; };



KIND_TO_KIND_ID(CXXCtorInitializer)
KIND_TO_KIND_ID(TemplateArgument)
KIND_TO_KIND_ID(TemplateArgumentLoc)
KIND_TO_KIND_ID(TemplateName)
KIND_TO_KIND_ID(NestedNameSpecifier)
KIND_TO_KIND_ID(NestedNameSpecifierLoc)
KIND_TO_KIND_ID(QualType)
KIND_TO_KIND_ID(TypeLoc)
KIND_TO_KIND_ID(Decl)
KIND_TO_KIND_ID(Stmt)
KIND_TO_KIND_ID(Type)
KIND_TO_KIND_ID(OMPClause)
KIND_TO_KIND_ID(CXXBaseSpecifier)
#define DECL(DERIVED, BASE) KIND_TO_KIND_ID(DERIVED##Decl)
#include "clang/AST/DeclNodes.inc"
#define STMT(DERIVED, BASE) KIND_TO_KIND_ID(DERIVED)
#include "clang/AST/StmtNodes.inc"
#define TYPE(DERIVED, BASE) KIND_TO_KIND_ID(DERIVED##Type)
#include "clang/AST/TypeNodes.inc"
#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) KIND_TO_KIND_ID(Class)
#include "llvm/Frontend/OpenMP/OMP.inc"
#undef KIND_TO_KIND_ID

inline raw_ostream &operator<<(raw_ostream &OS, ASTNodeKind K) {
OS << K.asStringRef();
return OS;
}













class DynTypedNode {
public:

template <typename T>
static DynTypedNode create(const T &Node) {
return BaseConverter<T>::create(Node);
}













template <typename T> const T *get() const {
return BaseConverter<T>::get(NodeKind, &Storage);
}




template <typename T>
const T &getUnchecked() const {
return BaseConverter<T>::getUnchecked(NodeKind, &Storage);
}

ASTNodeKind getNodeKind() const { return NodeKind; }






const void *getMemoizationData() const {
return NodeKind.hasPointerIdentity()
? *reinterpret_cast<void *const *>(&Storage)
: nullptr;
}


void print(llvm::raw_ostream &OS, const PrintingPolicy &PP) const;


void dump(llvm::raw_ostream &OS, const ASTContext &Context) const;



SourceRange getSourceRange() const;







bool operator<(const DynTypedNode &Other) const {
if (!NodeKind.isSame(Other.NodeKind))
return NodeKind < Other.NodeKind;

if (ASTNodeKind::getFromNodeKind<QualType>().isSame(NodeKind))
return getUnchecked<QualType>().getAsOpaquePtr() <
Other.getUnchecked<QualType>().getAsOpaquePtr();

if (ASTNodeKind::getFromNodeKind<TypeLoc>().isSame(NodeKind)) {
auto TLA = getUnchecked<TypeLoc>();
auto TLB = Other.getUnchecked<TypeLoc>();
return std::make_pair(TLA.getType().getAsOpaquePtr(),
TLA.getOpaqueData()) <
std::make_pair(TLB.getType().getAsOpaquePtr(),
TLB.getOpaqueData());
}

if (ASTNodeKind::getFromNodeKind<NestedNameSpecifierLoc>().isSame(
NodeKind)) {
auto NNSLA = getUnchecked<NestedNameSpecifierLoc>();
auto NNSLB = Other.getUnchecked<NestedNameSpecifierLoc>();
return std::make_pair(NNSLA.getNestedNameSpecifier(),
NNSLA.getOpaqueData()) <
std::make_pair(NNSLB.getNestedNameSpecifier(),
NNSLB.getOpaqueData());
}

assert(getMemoizationData() && Other.getMemoizationData());
return getMemoizationData() < Other.getMemoizationData();
}
bool operator==(const DynTypedNode &Other) const {


if (!NodeKind.isSame(Other.NodeKind))
return false;


if (ASTNodeKind::getFromNodeKind<QualType>().isSame(NodeKind))
return getUnchecked<QualType>() == Other.getUnchecked<QualType>();

if (ASTNodeKind::getFromNodeKind<TypeLoc>().isSame(NodeKind))
return getUnchecked<TypeLoc>() == Other.getUnchecked<TypeLoc>();

if (ASTNodeKind::getFromNodeKind<NestedNameSpecifierLoc>().isSame(NodeKind))
return getUnchecked<NestedNameSpecifierLoc>() ==
Other.getUnchecked<NestedNameSpecifierLoc>();

assert(getMemoizationData() && Other.getMemoizationData());
return getMemoizationData() == Other.getMemoizationData();
}
bool operator!=(const DynTypedNode &Other) const {
return !operator==(Other);
}



struct DenseMapInfo {
static inline DynTypedNode getEmptyKey() {
DynTypedNode Node;
Node.NodeKind = ASTNodeKind::DenseMapInfo::getEmptyKey();
return Node;
}
static inline DynTypedNode getTombstoneKey() {
DynTypedNode Node;
Node.NodeKind = ASTNodeKind::DenseMapInfo::getTombstoneKey();
return Node;
}
static unsigned getHashValue(const DynTypedNode &Val) {

if (ASTNodeKind::getFromNodeKind<TypeLoc>().isSame(Val.NodeKind)) {
auto TL = Val.getUnchecked<TypeLoc>();
return llvm::hash_combine(TL.getType().getAsOpaquePtr(),
TL.getOpaqueData());
}

if (ASTNodeKind::getFromNodeKind<NestedNameSpecifierLoc>().isSame(
Val.NodeKind)) {
auto NNSL = Val.getUnchecked<NestedNameSpecifierLoc>();
return llvm::hash_combine(NNSL.getNestedNameSpecifier(),
NNSL.getOpaqueData());
}

assert(Val.getMemoizationData());
return llvm::hash_value(Val.getMemoizationData());
}
static bool isEqual(const DynTypedNode &LHS, const DynTypedNode &RHS) {
auto Empty = ASTNodeKind::DenseMapInfo::getEmptyKey();
auto TombStone = ASTNodeKind::DenseMapInfo::getTombstoneKey();
return (ASTNodeKind::DenseMapInfo::isEqual(LHS.NodeKind, Empty) &&
ASTNodeKind::DenseMapInfo::isEqual(RHS.NodeKind, Empty)) ||
(ASTNodeKind::DenseMapInfo::isEqual(LHS.NodeKind, TombStone) &&
ASTNodeKind::DenseMapInfo::isEqual(RHS.NodeKind, TombStone)) ||
LHS == RHS;
}
};

private:

template <typename T, typename EnablerT = void> struct BaseConverter;


template <typename T, typename BaseT> struct DynCastPtrConverter {
static const T *get(ASTNodeKind NodeKind, const void *Storage) {
if (ASTNodeKind::getFromNodeKind<T>().isBaseOf(NodeKind))
return &getUnchecked(NodeKind, Storage);
return nullptr;
}
static const T &getUnchecked(ASTNodeKind NodeKind, const void *Storage) {
assert(ASTNodeKind::getFromNodeKind<T>().isBaseOf(NodeKind));
return *cast<T>(static_cast<const BaseT *>(
*reinterpret_cast<const void *const *>(Storage)));
}
static DynTypedNode create(const BaseT &Node) {
DynTypedNode Result;
Result.NodeKind = ASTNodeKind::getFromNode(Node);
new (&Result.Storage) const void *(&Node);
return Result;
}
};


template <typename T> struct PtrConverter {
static const T *get(ASTNodeKind NodeKind, const void *Storage) {
if (ASTNodeKind::getFromNodeKind<T>().isSame(NodeKind))
return &getUnchecked(NodeKind, Storage);
return nullptr;
}
static const T &getUnchecked(ASTNodeKind NodeKind, const void *Storage) {
assert(ASTNodeKind::getFromNodeKind<T>().isSame(NodeKind));
return *static_cast<const T *>(
*reinterpret_cast<const void *const *>(Storage));
}
static DynTypedNode create(const T &Node) {
DynTypedNode Result;
Result.NodeKind = ASTNodeKind::getFromNodeKind<T>();
new (&Result.Storage) const void *(&Node);
return Result;
}
};


template <typename T> struct ValueConverter {
static const T *get(ASTNodeKind NodeKind, const void *Storage) {
if (ASTNodeKind::getFromNodeKind<T>().isSame(NodeKind))
return reinterpret_cast<const T *>(Storage);
return nullptr;
}
static const T &getUnchecked(ASTNodeKind NodeKind, const void *Storage) {
assert(ASTNodeKind::getFromNodeKind<T>().isSame(NodeKind));
return *reinterpret_cast<const T *>(Storage);
}
static DynTypedNode create(const T &Node) {
DynTypedNode Result;
Result.NodeKind = ASTNodeKind::getFromNodeKind<T>();
new (&Result.Storage) T(Node);
return Result;
}
};

ASTNodeKind NodeKind;









llvm::AlignedCharArrayUnion<const void *, TemplateArgument,
TemplateArgumentLoc, NestedNameSpecifierLoc,
QualType, TypeLoc>
Storage;
};

template <typename T>
struct DynTypedNode::BaseConverter<
T, std::enable_if_t<std::is_base_of<Decl, T>::value>>
: public DynCastPtrConverter<T, Decl> {};

template <typename T>
struct DynTypedNode::BaseConverter<
T, std::enable_if_t<std::is_base_of<Stmt, T>::value>>
: public DynCastPtrConverter<T, Stmt> {};

template <typename T>
struct DynTypedNode::BaseConverter<
T, std::enable_if_t<std::is_base_of<Type, T>::value>>
: public DynCastPtrConverter<T, Type> {};

template <typename T>
struct DynTypedNode::BaseConverter<
T, std::enable_if_t<std::is_base_of<OMPClause, T>::value>>
: public DynCastPtrConverter<T, OMPClause> {};

template <>
struct DynTypedNode::BaseConverter<
NestedNameSpecifier, void> : public PtrConverter<NestedNameSpecifier> {};

template <>
struct DynTypedNode::BaseConverter<
CXXCtorInitializer, void> : public PtrConverter<CXXCtorInitializer> {};

template <>
struct DynTypedNode::BaseConverter<
TemplateArgument, void> : public ValueConverter<TemplateArgument> {};

template <>
struct DynTypedNode::BaseConverter<TemplateArgumentLoc, void>
: public ValueConverter<TemplateArgumentLoc> {};

template <>
struct DynTypedNode::BaseConverter<
TemplateName, void> : public ValueConverter<TemplateName> {};

template <>
struct DynTypedNode::BaseConverter<
NestedNameSpecifierLoc,
void> : public ValueConverter<NestedNameSpecifierLoc> {};

template <>
struct DynTypedNode::BaseConverter<QualType,
void> : public ValueConverter<QualType> {};

template <>
struct DynTypedNode::BaseConverter<
TypeLoc, void> : public ValueConverter<TypeLoc> {};

template <>
struct DynTypedNode::BaseConverter<CXXBaseSpecifier, void>
: public PtrConverter<CXXBaseSpecifier> {};





template <typename T, typename EnablerT> struct DynTypedNode::BaseConverter {
static const T *get(ASTNodeKind NodeKind, const char Storage[]) {
return NULL;
}
};

}

namespace llvm {

template <>
struct DenseMapInfo<clang::ASTNodeKind> : clang::ASTNodeKind::DenseMapInfo {};

template <>
struct DenseMapInfo<clang::DynTypedNode> : clang::DynTypedNode::DenseMapInfo {};

}

#endif
