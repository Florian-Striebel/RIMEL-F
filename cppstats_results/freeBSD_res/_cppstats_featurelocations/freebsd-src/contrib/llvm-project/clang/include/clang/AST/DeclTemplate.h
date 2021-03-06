












#if !defined(LLVM_CLANG_AST_DECLTEMPLATE_H)
#define LLVM_CLANG_AST_DECLTEMPLATE_H

#include "clang/AST/ASTConcept.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Redeclarable.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>

namespace clang {

enum BuiltinTemplateKind : int;
class ClassTemplateDecl;
class ClassTemplatePartialSpecializationDecl;
class Expr;
class FunctionTemplateDecl;
class IdentifierInfo;
class NonTypeTemplateParmDecl;
class TemplateDecl;
class TemplateTemplateParmDecl;
class TemplateTypeParmDecl;
class ConceptDecl;
class UnresolvedSetImpl;
class VarTemplateDecl;
class VarTemplatePartialSpecializationDecl;


using TemplateParameter =
llvm::PointerUnion<TemplateTypeParmDecl *, NonTypeTemplateParmDecl *,
TemplateTemplateParmDecl *>;

NamedDecl *getAsNamedDecl(TemplateParameter P);



class TemplateParameterList final
: private llvm::TrailingObjects<TemplateParameterList, NamedDecl *,
Expr *> {

SourceLocation TemplateLoc;


SourceLocation LAngleLoc, RAngleLoc;



unsigned NumParams : 29;



unsigned ContainsUnexpandedParameterPack : 1;


unsigned HasRequiresClause : 1;



unsigned HasConstrainedParameters : 1;

protected:
TemplateParameterList(const ASTContext& C, SourceLocation TemplateLoc,
SourceLocation LAngleLoc, ArrayRef<NamedDecl *> Params,
SourceLocation RAngleLoc, Expr *RequiresClause);

size_t numTrailingObjects(OverloadToken<NamedDecl *>) const {
return NumParams;
}

size_t numTrailingObjects(OverloadToken<Expr *>) const {
return HasRequiresClause ? 1 : 0;
}

public:
template <size_t N, bool HasRequiresClause>
friend class FixedSizeTemplateParameterListStorage;
friend TrailingObjects;

static TemplateParameterList *Create(const ASTContext &C,
SourceLocation TemplateLoc,
SourceLocation LAngleLoc,
ArrayRef<NamedDecl *> Params,
SourceLocation RAngleLoc,
Expr *RequiresClause);


using iterator = NamedDecl **;


using const_iterator = NamedDecl * const *;

iterator begin() { return getTrailingObjects<NamedDecl *>(); }
const_iterator begin() const { return getTrailingObjects<NamedDecl *>(); }
iterator end() { return begin() + NumParams; }
const_iterator end() const { return begin() + NumParams; }

unsigned size() const { return NumParams; }

ArrayRef<NamedDecl*> asArray() {
return llvm::makeArrayRef(begin(), end());
}
ArrayRef<const NamedDecl*> asArray() const {
return llvm::makeArrayRef(begin(), size());
}

NamedDecl* getParam(unsigned Idx) {
assert(Idx < size() && "Template parameter index out-of-range");
return begin()[Idx];
}
const NamedDecl* getParam(unsigned Idx) const {
assert(Idx < size() && "Template parameter index out-of-range");
return begin()[Idx];
}






unsigned getMinRequiredArguments() const;






unsigned getDepth() const;



bool containsUnexpandedParameterPack() const;


bool hasParameterPack() const {
for (const NamedDecl *P : asArray())
if (P->isParameterPack())
return true;
return false;
}


Expr *getRequiresClause() {
return HasRequiresClause ? getTrailingObjects<Expr *>()[0] : nullptr;
}


const Expr *getRequiresClause() const {
return HasRequiresClause ? getTrailingObjects<Expr *>()[0] : nullptr;
}







void getAssociatedConstraints(llvm::SmallVectorImpl<const Expr *> &AC) const;

bool hasAssociatedConstraints() const;

SourceLocation getTemplateLoc() const { return TemplateLoc; }
SourceLocation getLAngleLoc() const { return LAngleLoc; }
SourceLocation getRAngleLoc() const { return RAngleLoc; }

SourceRange getSourceRange() const LLVM_READONLY {
return SourceRange(TemplateLoc, RAngleLoc);
}

void print(raw_ostream &Out, const ASTContext &Context,
bool OmitTemplateKW = false) const;
void print(raw_ostream &Out, const ASTContext &Context,
const PrintingPolicy &Policy, bool OmitTemplateKW = false) const;

static bool shouldIncludeTypeForArgument(const TemplateParameterList *TPL,
unsigned Idx);
};




template <size_t N, bool HasRequiresClause>
class FixedSizeTemplateParameterListStorage
: public TemplateParameterList::FixedSizeStorageOwner {
typename TemplateParameterList::FixedSizeStorage<
NamedDecl *, Expr *>::with_counts<
N, HasRequiresClause ? 1u : 0u
>::type storage;

public:
FixedSizeTemplateParameterListStorage(const ASTContext &C,
SourceLocation TemplateLoc,
SourceLocation LAngleLoc,
ArrayRef<NamedDecl *> Params,
SourceLocation RAngleLoc,
Expr *RequiresClause)
: FixedSizeStorageOwner(
(assert(N == Params.size()),
assert(HasRequiresClause == (RequiresClause != nullptr)),
new (static_cast<void *>(&storage)) TemplateParameterList(C,
TemplateLoc, LAngleLoc, Params, RAngleLoc, RequiresClause))) {}
};


class TemplateArgumentList final
: private llvm::TrailingObjects<TemplateArgumentList, TemplateArgument> {

const TemplateArgument *Arguments;



unsigned NumArguments;



TemplateArgumentList(ArrayRef<TemplateArgument> Args);

public:
friend TrailingObjects;

TemplateArgumentList(const TemplateArgumentList &) = delete;
TemplateArgumentList &operator=(const TemplateArgumentList &) = delete;



enum OnStackType { OnStack };



static TemplateArgumentList *CreateCopy(ASTContext &Context,
ArrayRef<TemplateArgument> Args);





explicit TemplateArgumentList(OnStackType, ArrayRef<TemplateArgument> Args)
: Arguments(Args.data()), NumArguments(Args.size()) {}







explicit TemplateArgumentList(const TemplateArgumentList *Other)
: Arguments(Other->data()), NumArguments(Other->size()) {}


const TemplateArgument &get(unsigned Idx) const {
assert(Idx < NumArguments && "Invalid template argument index");
return data()[Idx];
}


const TemplateArgument &operator[](unsigned Idx) const { return get(Idx); }


ArrayRef<TemplateArgument> asArray() const {
return llvm::makeArrayRef(data(), size());
}



unsigned size() const { return NumArguments; }


const TemplateArgument *data() const { return Arguments; }
};

void *allocateDefaultArgStorageChain(const ASTContext &C);









template<typename ParmDecl, typename ArgType>
class DefaultArgStorage {



struct Chain {
ParmDecl *PrevDeclWithDefaultArg;
ArgType Value;
};
static_assert(sizeof(Chain) == sizeof(void *) * 2,
"non-pointer argument type?");

llvm::PointerUnion<ArgType, ParmDecl*, Chain*> ValueOrInherited;

static ParmDecl *getParmOwningDefaultArg(ParmDecl *Parm) {
const DefaultArgStorage &Storage = Parm->getDefaultArgStorage();
if (auto *Prev = Storage.ValueOrInherited.template dyn_cast<ParmDecl *>())
Parm = Prev;
assert(!Parm->getDefaultArgStorage()
.ValueOrInherited.template is<ParmDecl *>() &&
"should only be one level of indirection");
return Parm;
}

public:
DefaultArgStorage() : ValueOrInherited(ArgType()) {}


bool isSet() const { return !ValueOrInherited.isNull(); }



bool isInherited() const { return ValueOrInherited.template is<ParmDecl*>(); }



ArgType get() const {
const DefaultArgStorage *Storage = this;
if (const auto *Prev = ValueOrInherited.template dyn_cast<ParmDecl *>())
Storage = &Prev->getDefaultArgStorage();
if (const auto *C = Storage->ValueOrInherited.template dyn_cast<Chain *>())
return C->Value;
return Storage->ValueOrInherited.template get<ArgType>();
}



const ParmDecl *getInheritedFrom() const {
if (const auto *D = ValueOrInherited.template dyn_cast<ParmDecl *>())
return D;
if (const auto *C = ValueOrInherited.template dyn_cast<Chain *>())
return C->PrevDeclWithDefaultArg;
return nullptr;
}


void set(ArgType Arg) {
assert(!isSet() && "default argument already set");
ValueOrInherited = Arg;
}


void setInherited(const ASTContext &C, ParmDecl *InheritedFrom) {
assert(!isInherited() && "default argument already inherited");
InheritedFrom = getParmOwningDefaultArg(InheritedFrom);
if (!isSet())
ValueOrInherited = InheritedFrom;
else
ValueOrInherited = new (allocateDefaultArgStorageChain(C))
Chain{InheritedFrom, ValueOrInherited.template get<ArgType>()};
}


void clear() {
ValueOrInherited = ArgType();
}
};










class TemplateDecl : public NamedDecl {
void anchor() override;

protected:

TemplateDecl(Kind DK, DeclContext *DC, SourceLocation L, DeclarationName Name,
TemplateParameterList *Params, NamedDecl *Decl);



TemplateDecl(Kind DK, DeclContext *DC, SourceLocation L, DeclarationName Name,
TemplateParameterList *Params)
: TemplateDecl(DK, DC, L, Name, Params, nullptr) {}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


TemplateParameterList *getTemplateParameters() const {
return TemplateParams;
}





void getAssociatedConstraints(llvm::SmallVectorImpl<const Expr *> &AC) const;

bool hasAssociatedConstraints() const;


NamedDecl *getTemplatedDecl() const { return TemplatedDecl; }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K >= firstTemplate && K <= lastTemplate;
}

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getTemplateParameters()->getTemplateLoc(),
TemplatedDecl->getSourceRange().getEnd());
}

protected:
NamedDecl *TemplatedDecl;
TemplateParameterList *TemplateParams;

void setTemplateParameters(TemplateParameterList *TParams) {
TemplateParams = TParams;
}

public:


void init(NamedDecl *templatedDecl, TemplateParameterList* templateParams) {
assert(!TemplatedDecl && "TemplatedDecl already set!");
assert(!TemplateParams && "TemplateParams already set!");
TemplatedDecl = templatedDecl;
TemplateParams = templateParams;
}
};




class FunctionTemplateSpecializationInfo final
: public llvm::FoldingSetNode,
private llvm::TrailingObjects<FunctionTemplateSpecializationInfo,
MemberSpecializationInfo *> {


llvm::PointerIntPair<FunctionDecl *, 1, bool> Function;





llvm::PointerIntPair<FunctionTemplateDecl *, 2> Template;

public:


const TemplateArgumentList *TemplateArguments;



const ASTTemplateArgumentListInfo *TemplateArgumentsAsWritten;



SourceLocation PointOfInstantiation;

private:
FunctionTemplateSpecializationInfo(
FunctionDecl *FD, FunctionTemplateDecl *Template,
TemplateSpecializationKind TSK, const TemplateArgumentList *TemplateArgs,
const ASTTemplateArgumentListInfo *TemplateArgsAsWritten,
SourceLocation POI, MemberSpecializationInfo *MSInfo)
: Function(FD, MSInfo ? 1 : 0), Template(Template, TSK - 1),
TemplateArguments(TemplateArgs),
TemplateArgumentsAsWritten(TemplateArgsAsWritten),
PointOfInstantiation(POI) {
if (MSInfo)
getTrailingObjects<MemberSpecializationInfo *>()[0] = MSInfo;
}

size_t numTrailingObjects(OverloadToken<MemberSpecializationInfo*>) const {
return Function.getInt();
}

public:
friend TrailingObjects;

static FunctionTemplateSpecializationInfo *
Create(ASTContext &C, FunctionDecl *FD, FunctionTemplateDecl *Template,
TemplateSpecializationKind TSK,
const TemplateArgumentList *TemplateArgs,
const TemplateArgumentListInfo *TemplateArgsAsWritten,
SourceLocation POI, MemberSpecializationInfo *MSInfo);


FunctionDecl *getFunction() const { return Function.getPointer(); }


FunctionTemplateDecl *getTemplate() const { return Template.getPointer(); }


TemplateSpecializationKind getTemplateSpecializationKind() const {
return (TemplateSpecializationKind)(Template.getInt() + 1);
}

bool isExplicitSpecialization() const {
return getTemplateSpecializationKind() == TSK_ExplicitSpecialization;
}




bool isExplicitInstantiationOrSpecialization() const {
return isTemplateExplicitInstantiationOrSpecialization(
getTemplateSpecializationKind());
}


void setTemplateSpecializationKind(TemplateSpecializationKind TSK) {
assert(TSK != TSK_Undeclared &&
"Cannot encode TSK_Undeclared for a function template specialization");
Template.setInt(TSK - 1);
}






SourceLocation getPointOfInstantiation() const {
return PointOfInstantiation;
}



void setPointOfInstantiation(SourceLocation POI) {
PointOfInstantiation = POI;
}































MemberSpecializationInfo *getMemberSpecializationInfo() const {
return numTrailingObjects(OverloadToken<MemberSpecializationInfo *>())
? getTrailingObjects<MemberSpecializationInfo *>()[0]
: nullptr;
}

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, TemplateArguments->asArray(), getFunction()->getASTContext());
}

static void
Profile(llvm::FoldingSetNodeID &ID, ArrayRef<TemplateArgument> TemplateArgs,
ASTContext &Context) {
ID.AddInteger(TemplateArgs.size());
for (const TemplateArgument &TemplateArg : TemplateArgs)
TemplateArg.Profile(ID, Context);
}
};




class MemberSpecializationInfo {


llvm::PointerIntPair<NamedDecl *, 2> MemberAndTSK;


SourceLocation PointOfInstantiation;

public:
explicit
MemberSpecializationInfo(NamedDecl *IF, TemplateSpecializationKind TSK,
SourceLocation POI = SourceLocation())
: MemberAndTSK(IF, TSK - 1), PointOfInstantiation(POI) {
assert(TSK != TSK_Undeclared &&
"Cannot encode undeclared template specializations for members");
}



NamedDecl *getInstantiatedFrom() const { return MemberAndTSK.getPointer(); }


TemplateSpecializationKind getTemplateSpecializationKind() const {
return (TemplateSpecializationKind)(MemberAndTSK.getInt() + 1);
}

bool isExplicitSpecialization() const {
return getTemplateSpecializationKind() == TSK_ExplicitSpecialization;
}


void setTemplateSpecializationKind(TemplateSpecializationKind TSK) {
assert(TSK != TSK_Undeclared &&
"Cannot encode undeclared template specializations for members");
MemberAndTSK.setInt(TSK - 1);
}




SourceLocation getPointOfInstantiation() const {
return PointOfInstantiation;
}


void setPointOfInstantiation(SourceLocation POI) {
PointOfInstantiation = POI;
}
};















class DependentFunctionTemplateSpecializationInfo final
: private llvm::TrailingObjects<DependentFunctionTemplateSpecializationInfo,
TemplateArgumentLoc,
FunctionTemplateDecl *> {

unsigned NumTemplates;


unsigned NumArgs;


SourceRange AngleLocs;

size_t numTrailingObjects(OverloadToken<TemplateArgumentLoc>) const {
return NumArgs;
}
size_t numTrailingObjects(OverloadToken<FunctionTemplateDecl *>) const {
return NumTemplates;
}

DependentFunctionTemplateSpecializationInfo(
const UnresolvedSetImpl &Templates,
const TemplateArgumentListInfo &TemplateArgs);

public:
friend TrailingObjects;

static DependentFunctionTemplateSpecializationInfo *
Create(ASTContext &Context, const UnresolvedSetImpl &Templates,
const TemplateArgumentListInfo &TemplateArgs);



unsigned getNumTemplates() const { return NumTemplates; }


FunctionTemplateDecl *getTemplate(unsigned I) const {
assert(I < getNumTemplates() && "template index out of range");
return getTrailingObjects<FunctionTemplateDecl *>()[I];
}


const TemplateArgumentLoc *getTemplateArgs() const {
return getTrailingObjects<TemplateArgumentLoc>();
}


unsigned getNumTemplateArgs() const { return NumArgs; }


const TemplateArgumentLoc &getTemplateArg(unsigned I) const {
assert(I < getNumTemplateArgs() && "template arg index out of range");
return getTemplateArgs()[I];
}

SourceLocation getLAngleLoc() const {
return AngleLocs.getBegin();
}

SourceLocation getRAngleLoc() const {
return AngleLocs.getEnd();
}
};


class RedeclarableTemplateDecl : public TemplateDecl,
public Redeclarable<RedeclarableTemplateDecl>
{
using redeclarable_base = Redeclarable<RedeclarableTemplateDecl>;

RedeclarableTemplateDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

RedeclarableTemplateDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

RedeclarableTemplateDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

void anchor() override;
protected:
template <typename EntryType> struct SpecEntryTraits {
using DeclType = EntryType;

static DeclType *getDecl(EntryType *D) {
return D;
}

static ArrayRef<TemplateArgument> getTemplateArgs(EntryType *D) {
return D->getTemplateArgs().asArray();
}
};

template <typename EntryType, typename SETraits = SpecEntryTraits<EntryType>,
typename DeclType = typename SETraits::DeclType>
struct SpecIterator
: llvm::iterator_adaptor_base<
SpecIterator<EntryType, SETraits, DeclType>,
typename llvm::FoldingSetVector<EntryType>::iterator,
typename std::iterator_traits<typename llvm::FoldingSetVector<
EntryType>::iterator>::iterator_category,
DeclType *, ptrdiff_t, DeclType *, DeclType *> {
SpecIterator() = default;
explicit SpecIterator(
typename llvm::FoldingSetVector<EntryType>::iterator SetIter)
: SpecIterator::iterator_adaptor_base(std::move(SetIter)) {}

DeclType *operator*() const {
return SETraits::getDecl(&*this->I)->getMostRecentDecl();
}

DeclType *operator->() const { return **this; }
};

template <typename EntryType>
static SpecIterator<EntryType>
makeSpecIterator(llvm::FoldingSetVector<EntryType> &Specs, bool isEnd) {
return SpecIterator<EntryType>(isEnd ? Specs.end() : Specs.begin());
}

void loadLazySpecializationsImpl() const;

template <class EntryType, typename ...ProfileArguments>
typename SpecEntryTraits<EntryType>::DeclType*
findSpecializationImpl(llvm::FoldingSetVector<EntryType> &Specs,
void *&InsertPos, ProfileArguments &&...ProfileArgs);

template <class Derived, class EntryType>
void addSpecializationImpl(llvm::FoldingSetVector<EntryType> &Specs,
EntryType *Entry, void *InsertPos);

struct CommonBase {
CommonBase() : InstantiatedFromMember(nullptr, false) {}






llvm::PointerIntPair<RedeclarableTemplateDecl*, 1, bool>
InstantiatedFromMember;






uint32_t *LazySpecializations = nullptr;
};



mutable CommonBase *Common = nullptr;




CommonBase *getCommonPtr() const;

virtual CommonBase *newCommon(ASTContext &C) const = 0;


RedeclarableTemplateDecl(Kind DK, ASTContext &C, DeclContext *DC,
SourceLocation L, DeclarationName Name,
TemplateParameterList *Params, NamedDecl *Decl)
: TemplateDecl(DK, DC, L, Name, Params, Decl), redeclarable_base(C) {}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend class ASTReader;
template <class decl_type> friend class RedeclarableTemplate;


RedeclarableTemplateDecl *getCanonicalDecl() override {
return getFirstDecl();
}
const RedeclarableTemplateDecl *getCanonicalDecl() const {
return getFirstDecl();
}



















bool isMemberSpecialization() const {
return getCommonPtr()->InstantiatedFromMember.getInt();
}


void setMemberSpecialization() {
assert(getCommonPtr()->InstantiatedFromMember.getPointer() &&
"Only member templates can be member template specializations");
getCommonPtr()->InstantiatedFromMember.setInt(true);
}





































RedeclarableTemplateDecl *getInstantiatedFromMemberTemplate() const {
return getCommonPtr()->InstantiatedFromMember.getPointer();
}

void setInstantiatedFromMemberTemplate(RedeclarableTemplateDecl *TD) {
assert(!getCommonPtr()->InstantiatedFromMember.getPointer());
getCommonPtr()->InstantiatedFromMember.setPointer(TD);
}

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K >= firstRedeclarableTemplate && K <= lastRedeclarableTemplate;
}
};

template <> struct RedeclarableTemplateDecl::
SpecEntryTraits<FunctionTemplateSpecializationInfo> {
using DeclType = FunctionDecl;

static DeclType *getDecl(FunctionTemplateSpecializationInfo *I) {
return I->getFunction();
}

static ArrayRef<TemplateArgument>
getTemplateArgs(FunctionTemplateSpecializationInfo *I) {
return I->TemplateArguments->asArray();
}
};


class FunctionTemplateDecl : public RedeclarableTemplateDecl {
protected:
friend class FunctionDecl;



struct Common : CommonBase {


llvm::FoldingSetVector<FunctionTemplateSpecializationInfo> Specializations;








TemplateArgument *InjectedArgs = nullptr;

Common() = default;
};

FunctionTemplateDecl(ASTContext &C, DeclContext *DC, SourceLocation L,
DeclarationName Name, TemplateParameterList *Params,
NamedDecl *Decl)
: RedeclarableTemplateDecl(FunctionTemplate, C, DC, L, Name, Params,
Decl) {}

CommonBase *newCommon(ASTContext &C) const override;

Common *getCommonPtr() const {
return static_cast<Common *>(RedeclarableTemplateDecl::getCommonPtr());
}



llvm::FoldingSetVector<FunctionTemplateSpecializationInfo> &
getSpecializations() const;





void addSpecialization(FunctionTemplateSpecializationInfo* Info,
void *InsertPos);

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


void LoadLazySpecializations() const;


FunctionDecl *getTemplatedDecl() const {
return static_cast<FunctionDecl *>(TemplatedDecl);
}



bool isThisDeclarationADefinition() const {
return getTemplatedDecl()->isThisDeclarationADefinition();
}



FunctionDecl *findSpecialization(ArrayRef<TemplateArgument> Args,
void *&InsertPos);

FunctionTemplateDecl *getCanonicalDecl() override {
return cast<FunctionTemplateDecl>(
RedeclarableTemplateDecl::getCanonicalDecl());
}
const FunctionTemplateDecl *getCanonicalDecl() const {
return cast<FunctionTemplateDecl>(
RedeclarableTemplateDecl::getCanonicalDecl());
}



FunctionTemplateDecl *getPreviousDecl() {
return cast_or_null<FunctionTemplateDecl>(
static_cast<RedeclarableTemplateDecl *>(this)->getPreviousDecl());
}
const FunctionTemplateDecl *getPreviousDecl() const {
return cast_or_null<FunctionTemplateDecl>(
static_cast<const RedeclarableTemplateDecl *>(this)->getPreviousDecl());
}

FunctionTemplateDecl *getMostRecentDecl() {
return cast<FunctionTemplateDecl>(
static_cast<RedeclarableTemplateDecl *>(this)
->getMostRecentDecl());
}
const FunctionTemplateDecl *getMostRecentDecl() const {
return const_cast<FunctionTemplateDecl*>(this)->getMostRecentDecl();
}

FunctionTemplateDecl *getInstantiatedFromMemberTemplate() const {
return cast_or_null<FunctionTemplateDecl>(
RedeclarableTemplateDecl::getInstantiatedFromMemberTemplate());
}

using spec_iterator = SpecIterator<FunctionTemplateSpecializationInfo>;
using spec_range = llvm::iterator_range<spec_iterator>;

spec_range specializations() const {
return spec_range(spec_begin(), spec_end());
}

spec_iterator spec_begin() const {
return makeSpecIterator(getSpecializations(), false);
}

spec_iterator spec_end() const {
return makeSpecIterator(getSpecializations(), true);
}








ArrayRef<TemplateArgument> getInjectedTemplateArgs();



bool isAbbreviated() const {




const TemplateParameterList *TPL = getTemplateParameters();
return TPL->getParam(TPL->size() - 1)->isImplicit();
}


void mergePrevDecl(FunctionTemplateDecl *Prev);


static FunctionTemplateDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L,
DeclarationName Name,
TemplateParameterList *Params,
NamedDecl *Decl);


static FunctionTemplateDecl *CreateDeserialized(ASTContext &C, unsigned ID);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == FunctionTemplate; }
};














class TemplateParmPosition {
protected:


unsigned Depth;
unsigned Position;

TemplateParmPosition(unsigned D, unsigned P) : Depth(D), Position(P) {}

public:
TemplateParmPosition() = delete;


unsigned getDepth() const { return Depth; }
void setDepth(unsigned D) { Depth = D; }


unsigned getPosition() const { return Position; }
void setPosition(unsigned P) { Position = P; }


unsigned getIndex() const { return Position; }
};







class TemplateTypeParmDecl final : public TypeDecl,
private llvm::TrailingObjects<TemplateTypeParmDecl, TypeConstraint> {

friend class Sema;
friend TrailingObjects;
friend class ASTDeclReader;





bool Typename : 1;


bool HasTypeConstraint : 1;




bool TypeConstraintInitialized : 1;




bool ExpandedParameterPack : 1;


unsigned NumExpanded = 0;


using DefArgStorage =
DefaultArgStorage<TemplateTypeParmDecl, TypeSourceInfo *>;
DefArgStorage DefaultArgument;

TemplateTypeParmDecl(DeclContext *DC, SourceLocation KeyLoc,
SourceLocation IdLoc, IdentifierInfo *Id,
bool Typename, bool HasTypeConstraint,
Optional<unsigned> NumExpanded)
: TypeDecl(TemplateTypeParm, DC, IdLoc, Id, KeyLoc), Typename(Typename),
HasTypeConstraint(HasTypeConstraint), TypeConstraintInitialized(false),
ExpandedParameterPack(NumExpanded),
NumExpanded(NumExpanded ? *NumExpanded : 0) {}

public:
static TemplateTypeParmDecl *Create(const ASTContext &C, DeclContext *DC,
SourceLocation KeyLoc,
SourceLocation NameLoc,
unsigned D, unsigned P,
IdentifierInfo *Id, bool Typename,
bool ParameterPack,
bool HasTypeConstraint = false,
Optional<unsigned> NumExpanded = None);
static TemplateTypeParmDecl *CreateDeserialized(const ASTContext &C,
unsigned ID);
static TemplateTypeParmDecl *CreateDeserialized(const ASTContext &C,
unsigned ID,
bool HasTypeConstraint);






bool wasDeclaredWithTypename() const {
return Typename && !HasTypeConstraint;
}

const DefArgStorage &getDefaultArgStorage() const { return DefaultArgument; }



bool hasDefaultArgument() const { return DefaultArgument.isSet(); }


QualType getDefaultArgument() const {
return DefaultArgument.get()->getType();
}


TypeSourceInfo *getDefaultArgumentInfo() const {
return DefaultArgument.get();
}


SourceLocation getDefaultArgumentLoc() const;



bool defaultArgumentWasInherited() const {
return DefaultArgument.isInherited();
}


void setDefaultArgument(TypeSourceInfo *DefArg) {
DefaultArgument.set(DefArg);
}



void setInheritedDefaultArgument(const ASTContext &C,
TemplateTypeParmDecl *Prev) {
DefaultArgument.setInherited(C, Prev);
}


void removeDefaultArgument() {
DefaultArgument.clear();
}



void setDeclaredWithTypename(bool withTypename) { Typename = withTypename; }


unsigned getDepth() const;


unsigned getIndex() const;


bool isParameterPack() const;





bool isPackExpansion() const {
if (!isParameterPack())
return false;
if (const TypeConstraint *TC = getTypeConstraint())
if (TC->hasExplicitTemplateArgs())
for (const auto &ArgLoc : TC->getTemplateArgsAsWritten()->arguments())
if (ArgLoc.getArgument().containsUnexpandedParameterPack())
return true;
return false;
}






















bool isExpandedParameterPack() const { return ExpandedParameterPack; }


unsigned getNumExpansionParameters() const {
assert(ExpandedParameterPack && "Not an expansion parameter pack");
return NumExpanded;
}



const TypeConstraint *getTypeConstraint() const {
return TypeConstraintInitialized ? getTrailingObjects<TypeConstraint>() :
nullptr;
}

void setTypeConstraint(NestedNameSpecifierLoc NNS,
DeclarationNameInfo NameInfo, NamedDecl *FoundDecl,
ConceptDecl *CD,
const ASTTemplateArgumentListInfo *ArgsAsWritten,
Expr *ImmediatelyDeclaredConstraint);


bool hasTypeConstraint() const {
return HasTypeConstraint;
}






void getAssociatedConstraints(llvm::SmallVectorImpl<const Expr *> &AC) const {
if (HasTypeConstraint)
AC.push_back(getTypeConstraint()->getImmediatelyDeclaredConstraint());
}

SourceRange getSourceRange() const override LLVM_READONLY;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == TemplateTypeParm; }
};






class NonTypeTemplateParmDecl final
: public DeclaratorDecl,
protected TemplateParmPosition,
private llvm::TrailingObjects<NonTypeTemplateParmDecl,
std::pair<QualType, TypeSourceInfo *>,
Expr *> {
friend class ASTDeclReader;
friend TrailingObjects;



using DefArgStorage = DefaultArgStorage<NonTypeTemplateParmDecl, Expr *>;
DefArgStorage DefaultArgument;





bool ParameterPack;




bool ExpandedParameterPack = false;


unsigned NumExpandedTypes = 0;

size_t numTrailingObjects(
OverloadToken<std::pair<QualType, TypeSourceInfo *>>) const {
return NumExpandedTypes;
}

NonTypeTemplateParmDecl(DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, unsigned D, unsigned P,
IdentifierInfo *Id, QualType T,
bool ParameterPack, TypeSourceInfo *TInfo)
: DeclaratorDecl(NonTypeTemplateParm, DC, IdLoc, Id, T, TInfo, StartLoc),
TemplateParmPosition(D, P), ParameterPack(ParameterPack) {}

NonTypeTemplateParmDecl(DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, unsigned D, unsigned P,
IdentifierInfo *Id, QualType T,
TypeSourceInfo *TInfo,
ArrayRef<QualType> ExpandedTypes,
ArrayRef<TypeSourceInfo *> ExpandedTInfos);

public:
static NonTypeTemplateParmDecl *
Create(const ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, unsigned D, unsigned P, IdentifierInfo *Id,
QualType T, bool ParameterPack, TypeSourceInfo *TInfo);

static NonTypeTemplateParmDecl *
Create(const ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, unsigned D, unsigned P, IdentifierInfo *Id,
QualType T, TypeSourceInfo *TInfo, ArrayRef<QualType> ExpandedTypes,
ArrayRef<TypeSourceInfo *> ExpandedTInfos);

static NonTypeTemplateParmDecl *CreateDeserialized(ASTContext &C,
unsigned ID,
bool HasTypeConstraint);
static NonTypeTemplateParmDecl *CreateDeserialized(ASTContext &C,
unsigned ID,
unsigned NumExpandedTypes,
bool HasTypeConstraint);

using TemplateParmPosition::getDepth;
using TemplateParmPosition::setDepth;
using TemplateParmPosition::getPosition;
using TemplateParmPosition::setPosition;
using TemplateParmPosition::getIndex;

SourceRange getSourceRange() const override LLVM_READONLY;

const DefArgStorage &getDefaultArgStorage() const { return DefaultArgument; }



bool hasDefaultArgument() const { return DefaultArgument.isSet(); }


Expr *getDefaultArgument() const { return DefaultArgument.get(); }


SourceLocation getDefaultArgumentLoc() const;



bool defaultArgumentWasInherited() const {
return DefaultArgument.isInherited();
}




void setDefaultArgument(Expr *DefArg) { DefaultArgument.set(DefArg); }
void setInheritedDefaultArgument(const ASTContext &C,
NonTypeTemplateParmDecl *Parm) {
DefaultArgument.setInherited(C, Parm);
}


void removeDefaultArgument() { DefaultArgument.clear(); }










bool isParameterPack() const { return ParameterPack; }






bool isPackExpansion() const {
return ParameterPack && getType()->getAs<PackExpansionType>();
}

























bool isExpandedParameterPack() const { return ExpandedParameterPack; }



unsigned getNumExpansionTypes() const {
assert(ExpandedParameterPack && "Not an expansion parameter pack");
return NumExpandedTypes;
}



QualType getExpansionType(unsigned I) const {
assert(I < NumExpandedTypes && "Out-of-range expansion type index");
auto TypesAndInfos =
getTrailingObjects<std::pair<QualType, TypeSourceInfo *>>();
return TypesAndInfos[I].first;
}



TypeSourceInfo *getExpansionTypeSourceInfo(unsigned I) const {
assert(I < NumExpandedTypes && "Out-of-range expansion type index");
auto TypesAndInfos =
getTrailingObjects<std::pair<QualType, TypeSourceInfo *>>();
return TypesAndInfos[I].second;
}



Expr *getPlaceholderTypeConstraint() const {
return hasPlaceholderTypeConstraint() ? *getTrailingObjects<Expr *>() :
nullptr;
}

void setPlaceholderTypeConstraint(Expr *E) {
*getTrailingObjects<Expr *>() = E;
}



bool hasPlaceholderTypeConstraint() const {
auto *AT = getType()->getContainedAutoType();
return AT && AT->isConstrained();
}







void getAssociatedConstraints(llvm::SmallVectorImpl<const Expr *> &AC) const {
if (Expr *E = getPlaceholderTypeConstraint())
AC.push_back(E);
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == NonTypeTemplateParm; }
};








class TemplateTemplateParmDecl final
: public TemplateDecl,
protected TemplateParmPosition,
private llvm::TrailingObjects<TemplateTemplateParmDecl,
TemplateParameterList *> {

using DefArgStorage =
DefaultArgStorage<TemplateTemplateParmDecl, TemplateArgumentLoc *>;
DefArgStorage DefaultArgument;


bool ParameterPack;




bool ExpandedParameterPack = false;


unsigned NumExpandedParams = 0;

TemplateTemplateParmDecl(DeclContext *DC, SourceLocation L,
unsigned D, unsigned P, bool ParameterPack,
IdentifierInfo *Id, TemplateParameterList *Params)
: TemplateDecl(TemplateTemplateParm, DC, L, Id, Params),
TemplateParmPosition(D, P), ParameterPack(ParameterPack) {}

TemplateTemplateParmDecl(DeclContext *DC, SourceLocation L,
unsigned D, unsigned P,
IdentifierInfo *Id, TemplateParameterList *Params,
ArrayRef<TemplateParameterList *> Expansions);

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend TrailingObjects;

static TemplateTemplateParmDecl *Create(const ASTContext &C, DeclContext *DC,
SourceLocation L, unsigned D,
unsigned P, bool ParameterPack,
IdentifierInfo *Id,
TemplateParameterList *Params);
static TemplateTemplateParmDecl *Create(const ASTContext &C, DeclContext *DC,
SourceLocation L, unsigned D,
unsigned P,
IdentifierInfo *Id,
TemplateParameterList *Params,
ArrayRef<TemplateParameterList *> Expansions);

static TemplateTemplateParmDecl *CreateDeserialized(ASTContext &C,
unsigned ID);
static TemplateTemplateParmDecl *CreateDeserialized(ASTContext &C,
unsigned ID,
unsigned NumExpansions);

using TemplateParmPosition::getDepth;
using TemplateParmPosition::setDepth;
using TemplateParmPosition::getPosition;
using TemplateParmPosition::setPosition;
using TemplateParmPosition::getIndex;







bool isParameterPack() const { return ParameterPack; }





bool isPackExpansion() const {
return ParameterPack &&
getTemplateParameters()->containsUnexpandedParameterPack();
}



















bool isExpandedParameterPack() const { return ExpandedParameterPack; }



unsigned getNumExpansionTemplateParameters() const {
assert(ExpandedParameterPack && "Not an expansion parameter pack");
return NumExpandedParams;
}



TemplateParameterList *getExpansionTemplateParameters(unsigned I) const {
assert(I < NumExpandedParams && "Out-of-range expansion type index");
return getTrailingObjects<TemplateParameterList *>()[I];
}

const DefArgStorage &getDefaultArgStorage() const { return DefaultArgument; }



bool hasDefaultArgument() const { return DefaultArgument.isSet(); }


const TemplateArgumentLoc &getDefaultArgument() const {
static const TemplateArgumentLoc NoneLoc;
return DefaultArgument.isSet() ? *DefaultArgument.get() : NoneLoc;
}


SourceLocation getDefaultArgumentLoc() const;



bool defaultArgumentWasInherited() const {
return DefaultArgument.isInherited();
}




void setDefaultArgument(const ASTContext &C,
const TemplateArgumentLoc &DefArg);
void setInheritedDefaultArgument(const ASTContext &C,
TemplateTemplateParmDecl *Prev) {
DefaultArgument.setInherited(C, Prev);
}


void removeDefaultArgument() { DefaultArgument.clear(); }

SourceRange getSourceRange() const override LLVM_READONLY {
SourceLocation End = getLocation();
if (hasDefaultArgument() && !defaultArgumentWasInherited())
End = getDefaultArgument().getSourceRange().getEnd();
return SourceRange(getTemplateParameters()->getTemplateLoc(), End);
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == TemplateTemplateParm; }
};




class BuiltinTemplateDecl : public TemplateDecl {
BuiltinTemplateKind BTK;

BuiltinTemplateDecl(const ASTContext &C, DeclContext *DC,
DeclarationName Name, BuiltinTemplateKind BTK);

void anchor() override;

public:

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == BuiltinTemplate; }

static BuiltinTemplateDecl *Create(const ASTContext &C, DeclContext *DC,
DeclarationName Name,
BuiltinTemplateKind BTK) {
return new (C, DC) BuiltinTemplateDecl(C, DC, Name, BTK);
}

SourceRange getSourceRange() const override LLVM_READONLY {
return {};
}

BuiltinTemplateKind getBuiltinTemplateKind() const { return BTK; }
};














class ClassTemplateSpecializationDecl
: public CXXRecordDecl, public llvm::FoldingSetNode {



struct SpecializedPartialSpecialization {


ClassTemplatePartialSpecializationDecl *PartialSpecialization;



const TemplateArgumentList *TemplateArgs;
};


llvm::PointerUnion<ClassTemplateDecl *, SpecializedPartialSpecialization *>
SpecializedTemplate;


struct ExplicitSpecializationInfo {

TypeSourceInfo *TypeAsWritten = nullptr;


SourceLocation ExternLoc;


SourceLocation TemplateKeywordLoc;

ExplicitSpecializationInfo() = default;
};



ExplicitSpecializationInfo *ExplicitInfo = nullptr;


const TemplateArgumentList *TemplateArgs;


SourceLocation PointOfInstantiation;



unsigned SpecializationKind : 3;

protected:
ClassTemplateSpecializationDecl(ASTContext &Context, Kind DK, TagKind TK,
DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc,
ClassTemplateDecl *SpecializedTemplate,
ArrayRef<TemplateArgument> Args,
ClassTemplateSpecializationDecl *PrevDecl);

explicit ClassTemplateSpecializationDecl(ASTContext &C, Kind DK);

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ClassTemplateSpecializationDecl *
Create(ASTContext &Context, TagKind TK, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
ClassTemplateDecl *SpecializedTemplate,
ArrayRef<TemplateArgument> Args,
ClassTemplateSpecializationDecl *PrevDecl);
static ClassTemplateSpecializationDecl *
CreateDeserialized(ASTContext &C, unsigned ID);

void getNameForDiagnostic(raw_ostream &OS, const PrintingPolicy &Policy,
bool Qualified) const override;






ClassTemplateSpecializationDecl *getMostRecentDecl() {
return cast<ClassTemplateSpecializationDecl>(
getMostRecentNonInjectedDecl());
}


ClassTemplateDecl *getSpecializedTemplate() const;



const TemplateArgumentList &getTemplateArgs() const {
return *TemplateArgs;
}

void setTemplateArgs(TemplateArgumentList *Args) {
TemplateArgs = Args;
}



TemplateSpecializationKind getSpecializationKind() const {
return static_cast<TemplateSpecializationKind>(SpecializationKind);
}

bool isExplicitSpecialization() const {
return getSpecializationKind() == TSK_ExplicitSpecialization;
}










bool isClassScopeExplicitSpecialization() const {
return isExplicitSpecialization() &&
isa<CXXRecordDecl>(getLexicalDeclContext());
}




bool isExplicitInstantiationOrSpecialization() const {
return isTemplateExplicitInstantiationOrSpecialization(
getTemplateSpecializationKind());
}

void setSpecializedTemplate(ClassTemplateDecl *Specialized) {
SpecializedTemplate = Specialized;
}

void setSpecializationKind(TemplateSpecializationKind TSK) {
SpecializationKind = TSK;
}


SourceLocation getPointOfInstantiation() const {
return PointOfInstantiation;
}

void setPointOfInstantiation(SourceLocation Loc) {
assert(Loc.isValid() && "point of instantiation must be valid!");
PointOfInstantiation = Loc;
}





llvm::PointerUnion<ClassTemplateDecl *,
ClassTemplatePartialSpecializationDecl *>
getInstantiatedFrom() const {
if (!isTemplateInstantiation(getSpecializationKind()))
return llvm::PointerUnion<ClassTemplateDecl *,
ClassTemplatePartialSpecializationDecl *>();

return getSpecializedTemplateOrPartial();
}



llvm::PointerUnion<ClassTemplateDecl *,
ClassTemplatePartialSpecializationDecl *>
getSpecializedTemplateOrPartial() const {
if (const auto *PartialSpec =
SpecializedTemplate.dyn_cast<SpecializedPartialSpecialization *>())
return PartialSpec->PartialSpecialization;

return SpecializedTemplate.get<ClassTemplateDecl*>();
}












const TemplateArgumentList &getTemplateInstantiationArgs() const {
if (const auto *PartialSpec =
SpecializedTemplate.dyn_cast<SpecializedPartialSpecialization *>())
return *PartialSpec->TemplateArgs;

return getTemplateArgs();
}




void setInstantiationOf(ClassTemplatePartialSpecializationDecl *PartialSpec,
const TemplateArgumentList *TemplateArgs) {
assert(!SpecializedTemplate.is<SpecializedPartialSpecialization*>() &&
"Already set to a class template partial specialization!");
auto *PS = new (getASTContext()) SpecializedPartialSpecialization();
PS->PartialSpecialization = PartialSpec;
PS->TemplateArgs = TemplateArgs;
SpecializedTemplate = PS;
}



void setInstantiationOf(ClassTemplateDecl *TemplDecl) {
assert(!SpecializedTemplate.is<SpecializedPartialSpecialization*>() &&
"Previously set to a class template partial specialization!");
SpecializedTemplate = TemplDecl;
}



void setTypeAsWritten(TypeSourceInfo *T) {
if (!ExplicitInfo)
ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
ExplicitInfo->TypeAsWritten = T;
}



TypeSourceInfo *getTypeAsWritten() const {
return ExplicitInfo ? ExplicitInfo->TypeAsWritten : nullptr;
}


SourceLocation getExternLoc() const {
return ExplicitInfo ? ExplicitInfo->ExternLoc : SourceLocation();
}


void setExternLoc(SourceLocation Loc) {
if (!ExplicitInfo)
ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
ExplicitInfo->ExternLoc = Loc;
}


void setTemplateKeywordLoc(SourceLocation Loc) {
if (!ExplicitInfo)
ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
ExplicitInfo->TemplateKeywordLoc = Loc;
}


SourceLocation getTemplateKeywordLoc() const {
return ExplicitInfo ? ExplicitInfo->TemplateKeywordLoc : SourceLocation();
}

SourceRange getSourceRange() const override LLVM_READONLY;

void Profile(llvm::FoldingSetNodeID &ID) const {
Profile(ID, TemplateArgs->asArray(), getASTContext());
}

static void
Profile(llvm::FoldingSetNodeID &ID, ArrayRef<TemplateArgument> TemplateArgs,
ASTContext &Context) {
ID.AddInteger(TemplateArgs.size());
for (const TemplateArgument &TemplateArg : TemplateArgs)
TemplateArg.Profile(ID, Context);
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K >= firstClassTemplateSpecialization &&
K <= lastClassTemplateSpecialization;
}
};

class ClassTemplatePartialSpecializationDecl
: public ClassTemplateSpecializationDecl {

TemplateParameterList* TemplateParams = nullptr;



const ASTTemplateArgumentListInfo *ArgsAsWritten = nullptr;






llvm::PointerIntPair<ClassTemplatePartialSpecializationDecl *, 1, bool>
InstantiatedFromMember;

ClassTemplatePartialSpecializationDecl(ASTContext &Context, TagKind TK,
DeclContext *DC,
SourceLocation StartLoc,
SourceLocation IdLoc,
TemplateParameterList *Params,
ClassTemplateDecl *SpecializedTemplate,
ArrayRef<TemplateArgument> Args,
const ASTTemplateArgumentListInfo *ArgsAsWritten,
ClassTemplatePartialSpecializationDecl *PrevDecl);

ClassTemplatePartialSpecializationDecl(ASTContext &C)
: ClassTemplateSpecializationDecl(C, ClassTemplatePartialSpecialization),
InstantiatedFromMember(nullptr, false) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ClassTemplatePartialSpecializationDecl *
Create(ASTContext &Context, TagKind TK, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
TemplateParameterList *Params,
ClassTemplateDecl *SpecializedTemplate,
ArrayRef<TemplateArgument> Args,
const TemplateArgumentListInfo &ArgInfos,
QualType CanonInjectedType,
ClassTemplatePartialSpecializationDecl *PrevDecl);

static ClassTemplatePartialSpecializationDecl *
CreateDeserialized(ASTContext &C, unsigned ID);

ClassTemplatePartialSpecializationDecl *getMostRecentDecl() {
return cast<ClassTemplatePartialSpecializationDecl>(
static_cast<ClassTemplateSpecializationDecl *>(
this)->getMostRecentDecl());
}


TemplateParameterList *getTemplateParameters() const {
return TemplateParams;
}







void getAssociatedConstraints(llvm::SmallVectorImpl<const Expr *> &AC) const {
TemplateParams->getAssociatedConstraints(AC);
}

bool hasAssociatedConstraints() const {
return TemplateParams->hasAssociatedConstraints();
}


const ASTTemplateArgumentListInfo *getTemplateArgsAsWritten() const {
return ArgsAsWritten;
}





















ClassTemplatePartialSpecializationDecl *getInstantiatedFromMember() const {
const auto *First =
cast<ClassTemplatePartialSpecializationDecl>(getFirstDecl());
return First->InstantiatedFromMember.getPointer();
}
ClassTemplatePartialSpecializationDecl *
getInstantiatedFromMemberTemplate() const {
return getInstantiatedFromMember();
}

void setInstantiatedFromMember(
ClassTemplatePartialSpecializationDecl *PartialSpec) {
auto *First = cast<ClassTemplatePartialSpecializationDecl>(getFirstDecl());
First->InstantiatedFromMember.setPointer(PartialSpec);
}

















bool isMemberSpecialization() {
const auto *First =
cast<ClassTemplatePartialSpecializationDecl>(getFirstDecl());
return First->InstantiatedFromMember.getInt();
}


void setMemberSpecialization() {
auto *First = cast<ClassTemplatePartialSpecializationDecl>(getFirstDecl());
assert(First->InstantiatedFromMember.getPointer() &&
"Only member templates can be member template specializations");
return First->InstantiatedFromMember.setInt(true);
}




QualType getInjectedSpecializationType() const {
assert(getTypeForDecl() && "partial specialization has no type set!");
return cast<InjectedClassNameType>(getTypeForDecl())
->getInjectedSpecializationType();
}

void Profile(llvm::FoldingSetNodeID &ID) const {
Profile(ID, getTemplateArgs().asArray(), getTemplateParameters(),
getASTContext());
}

static void
Profile(llvm::FoldingSetNodeID &ID, ArrayRef<TemplateArgument> TemplateArgs,
TemplateParameterList *TPL, ASTContext &Context);

static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K == ClassTemplatePartialSpecialization;
}
};


class ClassTemplateDecl : public RedeclarableTemplateDecl {
protected:


struct Common : CommonBase {


llvm::FoldingSetVector<ClassTemplateSpecializationDecl> Specializations;



llvm::FoldingSetVector<ClassTemplatePartialSpecializationDecl>
PartialSpecializations;


QualType InjectedClassNameType;

Common() = default;
};


llvm::FoldingSetVector<ClassTemplateSpecializationDecl> &
getSpecializations() const;



llvm::FoldingSetVector<ClassTemplatePartialSpecializationDecl> &
getPartialSpecializations() const;

ClassTemplateDecl(ASTContext &C, DeclContext *DC, SourceLocation L,
DeclarationName Name, TemplateParameterList *Params,
NamedDecl *Decl)
: RedeclarableTemplateDecl(ClassTemplate, C, DC, L, Name, Params, Decl) {}

CommonBase *newCommon(ASTContext &C) const override;

Common *getCommonPtr() const {
return static_cast<Common *>(RedeclarableTemplateDecl::getCommonPtr());
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


void LoadLazySpecializations() const;


CXXRecordDecl *getTemplatedDecl() const {
return static_cast<CXXRecordDecl *>(TemplatedDecl);
}



bool isThisDeclarationADefinition() const {
return getTemplatedDecl()->isThisDeclarationADefinition();
}


static ClassTemplateDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L,
DeclarationName Name,
TemplateParameterList *Params,
NamedDecl *Decl);


static ClassTemplateDecl *CreateDeserialized(ASTContext &C, unsigned ID);



ClassTemplateSpecializationDecl *
findSpecialization(ArrayRef<TemplateArgument> Args, void *&InsertPos);



void AddSpecialization(ClassTemplateSpecializationDecl *D, void *InsertPos);

ClassTemplateDecl *getCanonicalDecl() override {
return cast<ClassTemplateDecl>(
RedeclarableTemplateDecl::getCanonicalDecl());
}
const ClassTemplateDecl *getCanonicalDecl() const {
return cast<ClassTemplateDecl>(
RedeclarableTemplateDecl::getCanonicalDecl());
}



ClassTemplateDecl *getPreviousDecl() {
return cast_or_null<ClassTemplateDecl>(
static_cast<RedeclarableTemplateDecl *>(this)->getPreviousDecl());
}
const ClassTemplateDecl *getPreviousDecl() const {
return cast_or_null<ClassTemplateDecl>(
static_cast<const RedeclarableTemplateDecl *>(
this)->getPreviousDecl());
}

ClassTemplateDecl *getMostRecentDecl() {
return cast<ClassTemplateDecl>(
static_cast<RedeclarableTemplateDecl *>(this)->getMostRecentDecl());
}
const ClassTemplateDecl *getMostRecentDecl() const {
return const_cast<ClassTemplateDecl*>(this)->getMostRecentDecl();
}

ClassTemplateDecl *getInstantiatedFromMemberTemplate() const {
return cast_or_null<ClassTemplateDecl>(
RedeclarableTemplateDecl::getInstantiatedFromMemberTemplate());
}



ClassTemplatePartialSpecializationDecl *
findPartialSpecialization(ArrayRef<TemplateArgument> Args,
TemplateParameterList *TPL, void *&InsertPos);



void AddPartialSpecialization(ClassTemplatePartialSpecializationDecl *D,
void *InsertPos);


void getPartialSpecializations(
SmallVectorImpl<ClassTemplatePartialSpecializationDecl *> &PS) const;









ClassTemplatePartialSpecializationDecl *findPartialSpecialization(QualType T);









ClassTemplatePartialSpecializationDecl *
findPartialSpecInstantiatedFromMember(
ClassTemplatePartialSpecializationDecl *D);















QualType getInjectedClassNameSpecialization();

using spec_iterator = SpecIterator<ClassTemplateSpecializationDecl>;
using spec_range = llvm::iterator_range<spec_iterator>;

spec_range specializations() const {
return spec_range(spec_begin(), spec_end());
}

spec_iterator spec_begin() const {
return makeSpecIterator(getSpecializations(), false);
}

spec_iterator spec_end() const {
return makeSpecIterator(getSpecializations(), true);
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ClassTemplate; }
};














class FriendTemplateDecl : public Decl {
virtual void anchor();

public:
using FriendUnion = llvm::PointerUnion<NamedDecl *,TypeSourceInfo *>;

private:

unsigned NumParams = 0;


TemplateParameterList **Params = nullptr;


FriendUnion Friend;


SourceLocation FriendLoc;

FriendTemplateDecl(DeclContext *DC, SourceLocation Loc,
MutableArrayRef<TemplateParameterList *> Params,
FriendUnion Friend, SourceLocation FriendLoc)
: Decl(Decl::FriendTemplate, DC, Loc), NumParams(Params.size()),
Params(Params.data()), Friend(Friend), FriendLoc(FriendLoc) {}

FriendTemplateDecl(EmptyShell Empty) : Decl(Decl::FriendTemplate, Empty) {}

public:
friend class ASTDeclReader;

static FriendTemplateDecl *
Create(ASTContext &Context, DeclContext *DC, SourceLocation Loc,
MutableArrayRef<TemplateParameterList *> Params, FriendUnion Friend,
SourceLocation FriendLoc);

static FriendTemplateDecl *CreateDeserialized(ASTContext &C, unsigned ID);




TypeSourceInfo *getFriendType() const {
return Friend.dyn_cast<TypeSourceInfo*>();
}




NamedDecl *getFriendDecl() const {
return Friend.dyn_cast<NamedDecl*>();
}


SourceLocation getFriendLoc() const {
return FriendLoc;
}

TemplateParameterList *getTemplateParameterList(unsigned i) const {
assert(i <= NumParams);
return Params[i];
}

unsigned getNumTemplateParameters() const {
return NumParams;
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Decl::FriendTemplate; }
};







class TypeAliasTemplateDecl : public RedeclarableTemplateDecl {
protected:
using Common = CommonBase;

TypeAliasTemplateDecl(ASTContext &C, DeclContext *DC, SourceLocation L,
DeclarationName Name, TemplateParameterList *Params,
NamedDecl *Decl)
: RedeclarableTemplateDecl(TypeAliasTemplate, C, DC, L, Name, Params,
Decl) {}

CommonBase *newCommon(ASTContext &C) const override;

Common *getCommonPtr() {
return static_cast<Common *>(RedeclarableTemplateDecl::getCommonPtr());
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


TypeAliasDecl *getTemplatedDecl() const {
return static_cast<TypeAliasDecl *>(TemplatedDecl);
}


TypeAliasTemplateDecl *getCanonicalDecl() override {
return cast<TypeAliasTemplateDecl>(
RedeclarableTemplateDecl::getCanonicalDecl());
}
const TypeAliasTemplateDecl *getCanonicalDecl() const {
return cast<TypeAliasTemplateDecl>(
RedeclarableTemplateDecl::getCanonicalDecl());
}



TypeAliasTemplateDecl *getPreviousDecl() {
return cast_or_null<TypeAliasTemplateDecl>(
static_cast<RedeclarableTemplateDecl *>(this)->getPreviousDecl());
}
const TypeAliasTemplateDecl *getPreviousDecl() const {
return cast_or_null<TypeAliasTemplateDecl>(
static_cast<const RedeclarableTemplateDecl *>(
this)->getPreviousDecl());
}

TypeAliasTemplateDecl *getInstantiatedFromMemberTemplate() const {
return cast_or_null<TypeAliasTemplateDecl>(
RedeclarableTemplateDecl::getInstantiatedFromMemberTemplate());
}


static TypeAliasTemplateDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L,
DeclarationName Name,
TemplateParameterList *Params,
NamedDecl *Decl);


static TypeAliasTemplateDecl *CreateDeserialized(ASTContext &C, unsigned ID);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == TypeAliasTemplate; }
};


















class ClassScopeFunctionSpecializationDecl : public Decl {
CXXMethodDecl *Specialization;
const ASTTemplateArgumentListInfo *TemplateArgs;

ClassScopeFunctionSpecializationDecl(
DeclContext *DC, SourceLocation Loc, CXXMethodDecl *FD,
const ASTTemplateArgumentListInfo *TemplArgs)
: Decl(Decl::ClassScopeFunctionSpecialization, DC, Loc),
Specialization(FD), TemplateArgs(TemplArgs) {}

ClassScopeFunctionSpecializationDecl(EmptyShell Empty)
: Decl(Decl::ClassScopeFunctionSpecialization, Empty) {}

virtual void anchor();

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

CXXMethodDecl *getSpecialization() const { return Specialization; }
bool hasExplicitTemplateArgs() const { return TemplateArgs; }
const ASTTemplateArgumentListInfo *getTemplateArgsAsWritten() const {
return TemplateArgs;
}

static ClassScopeFunctionSpecializationDecl *
Create(ASTContext &C, DeclContext *DC, SourceLocation Loc, CXXMethodDecl *FD,
bool HasExplicitTemplateArgs,
const TemplateArgumentListInfo &TemplateArgs) {
return new (C, DC) ClassScopeFunctionSpecializationDecl(
DC, Loc, FD,
HasExplicitTemplateArgs
? ASTTemplateArgumentListInfo::Create(C, TemplateArgs)
: nullptr);
}

static ClassScopeFunctionSpecializationDecl *
CreateDeserialized(ASTContext &Context, unsigned ID);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K == Decl::ClassScopeFunctionSpecialization;
}
};














class VarTemplateSpecializationDecl : public VarDecl,
public llvm::FoldingSetNode {




struct SpecializedPartialSpecialization {


VarTemplatePartialSpecializationDecl *PartialSpecialization;



const TemplateArgumentList *TemplateArgs;
};


llvm::PointerUnion<VarTemplateDecl *, SpecializedPartialSpecialization *>
SpecializedTemplate;


struct ExplicitSpecializationInfo {

TypeSourceInfo *TypeAsWritten = nullptr;


SourceLocation ExternLoc;


SourceLocation TemplateKeywordLoc;

ExplicitSpecializationInfo() = default;
};



ExplicitSpecializationInfo *ExplicitInfo = nullptr;


const TemplateArgumentList *TemplateArgs;
TemplateArgumentListInfo TemplateArgsInfo;


SourceLocation PointOfInstantiation;



unsigned SpecializationKind : 3;





unsigned IsCompleteDefinition : 1;

protected:
VarTemplateSpecializationDecl(Kind DK, ASTContext &Context, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
VarTemplateDecl *SpecializedTemplate,
QualType T, TypeSourceInfo *TInfo,
StorageClass S,
ArrayRef<TemplateArgument> Args);

explicit VarTemplateSpecializationDecl(Kind DK, ASTContext &Context);

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend class VarDecl;

static VarTemplateSpecializationDecl *
Create(ASTContext &Context, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, VarTemplateDecl *SpecializedTemplate, QualType T,
TypeSourceInfo *TInfo, StorageClass S,
ArrayRef<TemplateArgument> Args);
static VarTemplateSpecializationDecl *CreateDeserialized(ASTContext &C,
unsigned ID);

void getNameForDiagnostic(raw_ostream &OS, const PrintingPolicy &Policy,
bool Qualified) const override;

VarTemplateSpecializationDecl *getMostRecentDecl() {
VarDecl *Recent = static_cast<VarDecl *>(this)->getMostRecentDecl();
return cast<VarTemplateSpecializationDecl>(Recent);
}


VarTemplateDecl *getSpecializedTemplate() const;



const TemplateArgumentList &getTemplateArgs() const { return *TemplateArgs; }


void setTemplateArgsInfo(const TemplateArgumentListInfo &ArgsInfo);

const TemplateArgumentListInfo &getTemplateArgsInfo() const {
return TemplateArgsInfo;
}



TemplateSpecializationKind getSpecializationKind() const {
return static_cast<TemplateSpecializationKind>(SpecializationKind);
}

bool isExplicitSpecialization() const {
return getSpecializationKind() == TSK_ExplicitSpecialization;
}

bool isClassScopeExplicitSpecialization() const {
return isExplicitSpecialization() &&
isa<CXXRecordDecl>(getLexicalDeclContext());
}




bool isExplicitInstantiationOrSpecialization() const {
return isTemplateExplicitInstantiationOrSpecialization(
getTemplateSpecializationKind());
}

void setSpecializationKind(TemplateSpecializationKind TSK) {
SpecializationKind = TSK;
}


SourceLocation getPointOfInstantiation() const {
return PointOfInstantiation;
}

void setPointOfInstantiation(SourceLocation Loc) {
assert(Loc.isValid() && "point of instantiation must be valid!");
PointOfInstantiation = Loc;
}

void setCompleteDefinition() { IsCompleteDefinition = true; }





llvm::PointerUnion<VarTemplateDecl *, VarTemplatePartialSpecializationDecl *>
getInstantiatedFrom() const {
if (!isTemplateInstantiation(getSpecializationKind()))
return llvm::PointerUnion<VarTemplateDecl *,
VarTemplatePartialSpecializationDecl *>();

return getSpecializedTemplateOrPartial();
}



llvm::PointerUnion<VarTemplateDecl *, VarTemplatePartialSpecializationDecl *>
getSpecializedTemplateOrPartial() const {
if (const auto *PartialSpec =
SpecializedTemplate.dyn_cast<SpecializedPartialSpecialization *>())
return PartialSpec->PartialSpecialization;

return SpecializedTemplate.get<VarTemplateDecl *>();
}












const TemplateArgumentList &getTemplateInstantiationArgs() const {
if (const auto *PartialSpec =
SpecializedTemplate.dyn_cast<SpecializedPartialSpecialization *>())
return *PartialSpec->TemplateArgs;

return getTemplateArgs();
}




void setInstantiationOf(VarTemplatePartialSpecializationDecl *PartialSpec,
const TemplateArgumentList *TemplateArgs) {
assert(!SpecializedTemplate.is<SpecializedPartialSpecialization *>() &&
"Already set to a variable template partial specialization!");
auto *PS = new (getASTContext()) SpecializedPartialSpecialization();
PS->PartialSpecialization = PartialSpec;
PS->TemplateArgs = TemplateArgs;
SpecializedTemplate = PS;
}



void setInstantiationOf(VarTemplateDecl *TemplDecl) {
assert(!SpecializedTemplate.is<SpecializedPartialSpecialization *>() &&
"Previously set to a variable template partial specialization!");
SpecializedTemplate = TemplDecl;
}



void setTypeAsWritten(TypeSourceInfo *T) {
if (!ExplicitInfo)
ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
ExplicitInfo->TypeAsWritten = T;
}



TypeSourceInfo *getTypeAsWritten() const {
return ExplicitInfo ? ExplicitInfo->TypeAsWritten : nullptr;
}


SourceLocation getExternLoc() const {
return ExplicitInfo ? ExplicitInfo->ExternLoc : SourceLocation();
}


void setExternLoc(SourceLocation Loc) {
if (!ExplicitInfo)
ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
ExplicitInfo->ExternLoc = Loc;
}


void setTemplateKeywordLoc(SourceLocation Loc) {
if (!ExplicitInfo)
ExplicitInfo = new (getASTContext()) ExplicitSpecializationInfo;
ExplicitInfo->TemplateKeywordLoc = Loc;
}


SourceLocation getTemplateKeywordLoc() const {
return ExplicitInfo ? ExplicitInfo->TemplateKeywordLoc : SourceLocation();
}

void Profile(llvm::FoldingSetNodeID &ID) const {
Profile(ID, TemplateArgs->asArray(), getASTContext());
}

static void Profile(llvm::FoldingSetNodeID &ID,
ArrayRef<TemplateArgument> TemplateArgs,
ASTContext &Context) {
ID.AddInteger(TemplateArgs.size());
for (const TemplateArgument &TemplateArg : TemplateArgs)
TemplateArg.Profile(ID, Context);
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K >= firstVarTemplateSpecialization &&
K <= lastVarTemplateSpecialization;
}
};

class VarTemplatePartialSpecializationDecl
: public VarTemplateSpecializationDecl {

TemplateParameterList *TemplateParams = nullptr;



const ASTTemplateArgumentListInfo *ArgsAsWritten = nullptr;






llvm::PointerIntPair<VarTemplatePartialSpecializationDecl *, 1, bool>
InstantiatedFromMember;

VarTemplatePartialSpecializationDecl(
ASTContext &Context, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, TemplateParameterList *Params,
VarTemplateDecl *SpecializedTemplate, QualType T, TypeSourceInfo *TInfo,
StorageClass S, ArrayRef<TemplateArgument> Args,
const ASTTemplateArgumentListInfo *ArgInfos);

VarTemplatePartialSpecializationDecl(ASTContext &Context)
: VarTemplateSpecializationDecl(VarTemplatePartialSpecialization,
Context),
InstantiatedFromMember(nullptr, false) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static VarTemplatePartialSpecializationDecl *
Create(ASTContext &Context, DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, TemplateParameterList *Params,
VarTemplateDecl *SpecializedTemplate, QualType T,
TypeSourceInfo *TInfo, StorageClass S, ArrayRef<TemplateArgument> Args,
const TemplateArgumentListInfo &ArgInfos);

static VarTemplatePartialSpecializationDecl *CreateDeserialized(ASTContext &C,
unsigned ID);

VarTemplatePartialSpecializationDecl *getMostRecentDecl() {
return cast<VarTemplatePartialSpecializationDecl>(
static_cast<VarTemplateSpecializationDecl *>(
this)->getMostRecentDecl());
}


TemplateParameterList *getTemplateParameters() const {
return TemplateParams;
}


const ASTTemplateArgumentListInfo *getTemplateArgsAsWritten() const {
return ArgsAsWritten;
}







void getAssociatedConstraints(llvm::SmallVectorImpl<const Expr *> &AC) const {
TemplateParams->getAssociatedConstraints(AC);
}

bool hasAssociatedConstraints() const {
return TemplateParams->hasAssociatedConstraints();
}





















VarTemplatePartialSpecializationDecl *getInstantiatedFromMember() const {
const auto *First =
cast<VarTemplatePartialSpecializationDecl>(getFirstDecl());
return First->InstantiatedFromMember.getPointer();
}

void
setInstantiatedFromMember(VarTemplatePartialSpecializationDecl *PartialSpec) {
auto *First = cast<VarTemplatePartialSpecializationDecl>(getFirstDecl());
First->InstantiatedFromMember.setPointer(PartialSpec);
}

















bool isMemberSpecialization() {
const auto *First =
cast<VarTemplatePartialSpecializationDecl>(getFirstDecl());
return First->InstantiatedFromMember.getInt();
}


void setMemberSpecialization() {
auto *First = cast<VarTemplatePartialSpecializationDecl>(getFirstDecl());
assert(First->InstantiatedFromMember.getPointer() &&
"Only member templates can be member template specializations");
return First->InstantiatedFromMember.setInt(true);
}

void Profile(llvm::FoldingSetNodeID &ID) const {
Profile(ID, getTemplateArgs().asArray(), getTemplateParameters(),
getASTContext());
}

static void
Profile(llvm::FoldingSetNodeID &ID, ArrayRef<TemplateArgument> TemplateArgs,
TemplateParameterList *TPL, ASTContext &Context);

static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K == VarTemplatePartialSpecialization;
}
};


class VarTemplateDecl : public RedeclarableTemplateDecl {
protected:


struct Common : CommonBase {


llvm::FoldingSetVector<VarTemplateSpecializationDecl> Specializations;



llvm::FoldingSetVector<VarTemplatePartialSpecializationDecl>
PartialSpecializations;

Common() = default;
};


llvm::FoldingSetVector<VarTemplateSpecializationDecl> &
getSpecializations() const;



llvm::FoldingSetVector<VarTemplatePartialSpecializationDecl> &
getPartialSpecializations() const;

VarTemplateDecl(ASTContext &C, DeclContext *DC, SourceLocation L,
DeclarationName Name, TemplateParameterList *Params,
NamedDecl *Decl)
: RedeclarableTemplateDecl(VarTemplate, C, DC, L, Name, Params, Decl) {}

CommonBase *newCommon(ASTContext &C) const override;

Common *getCommonPtr() const {
return static_cast<Common *>(RedeclarableTemplateDecl::getCommonPtr());
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


void LoadLazySpecializations() const;


VarDecl *getTemplatedDecl() const {
return static_cast<VarDecl *>(TemplatedDecl);
}



bool isThisDeclarationADefinition() const {
return getTemplatedDecl()->isThisDeclarationADefinition();
}

VarTemplateDecl *getDefinition();


static VarTemplateDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, DeclarationName Name,
TemplateParameterList *Params,
VarDecl *Decl);


static VarTemplateDecl *CreateDeserialized(ASTContext &C, unsigned ID);



VarTemplateSpecializationDecl *
findSpecialization(ArrayRef<TemplateArgument> Args, void *&InsertPos);



void AddSpecialization(VarTemplateSpecializationDecl *D, void *InsertPos);

VarTemplateDecl *getCanonicalDecl() override {
return cast<VarTemplateDecl>(RedeclarableTemplateDecl::getCanonicalDecl());
}
const VarTemplateDecl *getCanonicalDecl() const {
return cast<VarTemplateDecl>(RedeclarableTemplateDecl::getCanonicalDecl());
}



VarTemplateDecl *getPreviousDecl() {
return cast_or_null<VarTemplateDecl>(
static_cast<RedeclarableTemplateDecl *>(this)->getPreviousDecl());
}
const VarTemplateDecl *getPreviousDecl() const {
return cast_or_null<VarTemplateDecl>(
static_cast<const RedeclarableTemplateDecl *>(
this)->getPreviousDecl());
}

VarTemplateDecl *getMostRecentDecl() {
return cast<VarTemplateDecl>(
static_cast<RedeclarableTemplateDecl *>(this)->getMostRecentDecl());
}
const VarTemplateDecl *getMostRecentDecl() const {
return const_cast<VarTemplateDecl *>(this)->getMostRecentDecl();
}

VarTemplateDecl *getInstantiatedFromMemberTemplate() const {
return cast_or_null<VarTemplateDecl>(
RedeclarableTemplateDecl::getInstantiatedFromMemberTemplate());
}



VarTemplatePartialSpecializationDecl *
findPartialSpecialization(ArrayRef<TemplateArgument> Args,
TemplateParameterList *TPL, void *&InsertPos);



void AddPartialSpecialization(VarTemplatePartialSpecializationDecl *D,
void *InsertPos);


void getPartialSpecializations(
SmallVectorImpl<VarTemplatePartialSpecializationDecl *> &PS) const;











VarTemplatePartialSpecializationDecl *findPartialSpecInstantiatedFromMember(
VarTemplatePartialSpecializationDecl *D);

using spec_iterator = SpecIterator<VarTemplateSpecializationDecl>;
using spec_range = llvm::iterator_range<spec_iterator>;

spec_range specializations() const {
return spec_range(spec_begin(), spec_end());
}

spec_iterator spec_begin() const {
return makeSpecIterator(getSpecializations(), false);
}

spec_iterator spec_end() const {
return makeSpecIterator(getSpecializations(), true);
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == VarTemplate; }
};


class ConceptDecl : public TemplateDecl, public Mergeable<ConceptDecl> {
protected:
Expr *ConstraintExpr;

ConceptDecl(DeclContext *DC, SourceLocation L, DeclarationName Name,
TemplateParameterList *Params, Expr *ConstraintExpr)
: TemplateDecl(Concept, DC, L, Name, Params),
ConstraintExpr(ConstraintExpr) {};
public:
static ConceptDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, DeclarationName Name,
TemplateParameterList *Params,
Expr *ConstraintExpr);
static ConceptDecl *CreateDeserialized(ASTContext &C, unsigned ID);

Expr *getConstraintExpr() const {
return ConstraintExpr;
}

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getTemplateParameters()->getTemplateLoc(),
ConstraintExpr->getEndLoc());
}

bool isTypeConcept() const {
return isa<TemplateTypeParmDecl>(getTemplateParameters()->getParam(0));
}

ConceptDecl *getCanonicalDecl() override { return getFirstDecl(); }
const ConceptDecl *getCanonicalDecl() const { return getFirstDecl(); }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Concept; }

friend class ASTReader;
friend class ASTDeclReader;
friend class ASTDeclWriter;
};













class TemplateParamObjectDecl : public ValueDecl,
public Mergeable<TemplateParamObjectDecl>,
public llvm::FoldingSetNode {
private:

APValue Value;

TemplateParamObjectDecl(DeclContext *DC, QualType T, const APValue &V)
: ValueDecl(TemplateParamObject, DC, SourceLocation(), DeclarationName(),
T),
Value(V) {}

static TemplateParamObjectDecl *Create(const ASTContext &C, QualType T,
const APValue &V);
static TemplateParamObjectDecl *CreateDeserialized(ASTContext &C,
unsigned ID);



friend class ASTContext;
friend class ASTReader;
friend class ASTDeclReader;

public:

void printName(llvm::raw_ostream &OS) const override;


void printAsExpr(llvm::raw_ostream &OS) const;



void printAsInit(llvm::raw_ostream &OS) const;

const APValue &getValue() const { return Value; }

static void Profile(llvm::FoldingSetNodeID &ID, QualType T,
const APValue &V) {
ID.AddPointer(T.getCanonicalType().getAsOpaquePtr());
V.Profile(ID);
}
void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getType(), getValue());
}

TemplateParamObjectDecl *getCanonicalDecl() override {
return getFirstDecl();
}
const TemplateParamObjectDecl *getCanonicalDecl() const {
return getFirstDecl();
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == TemplateParamObject; }
};

inline NamedDecl *getAsNamedDecl(TemplateParameter P) {
if (auto *PD = P.dyn_cast<TemplateTypeParmDecl *>())
return PD;
if (auto *PD = P.dyn_cast<NonTypeTemplateParmDecl *>())
return PD;
return P.get<TemplateTemplateParmDecl *>();
}

inline TemplateDecl *getAsTypeTemplateDecl(Decl *D) {
auto *TD = dyn_cast<TemplateDecl>(D);
return TD && (isa<ClassTemplateDecl>(TD) ||
isa<ClassTemplatePartialSpecializationDecl>(TD) ||
isa<TypeAliasTemplateDecl>(TD) ||
isa<TemplateTemplateParmDecl>(TD))
? TD
: nullptr;
}












inline Optional<unsigned> getExpandedPackSize(const NamedDecl *Param) {
if (const auto *TTP = dyn_cast<TemplateTypeParmDecl>(Param)) {
if (TTP->isExpandedParameterPack())
return TTP->getNumExpansionParameters();
}

if (const auto *NTTP = dyn_cast<NonTypeTemplateParmDecl>(Param)) {
if (NTTP->isExpandedParameterPack())
return NTTP->getNumExpansionTypes();
}

if (const auto *TTP = dyn_cast<TemplateTemplateParmDecl>(Param)) {
if (TTP->isExpandedParameterPack())
return TTP->getNumExpansionTemplateParameters();
}

return None;
}

}

#endif
