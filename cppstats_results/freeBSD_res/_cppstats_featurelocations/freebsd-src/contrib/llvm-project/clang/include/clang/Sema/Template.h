










#if !defined(LLVM_CLANG_SEMA_TEMPLATE_H)
#define LLVM_CLANG_SEMA_TEMPLATE_H

#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Sema/Sema.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <utility>

namespace clang {

class ASTContext;
class BindingDecl;
class CXXMethodDecl;
class Decl;
class DeclaratorDecl;
class DeclContext;
class EnumDecl;
class FunctionDecl;
class NamedDecl;
class ParmVarDecl;
class TagDecl;
class TypedefNameDecl;
class TypeSourceInfo;
class VarDecl;


enum class TemplateSubstitutionKind : char {


Specialization,



Rewrite,
};




















class MultiLevelTemplateArgumentList {

using ArgList = ArrayRef<TemplateArgument>;



SmallVector<ArgList, 4> TemplateArgumentLists;



unsigned NumRetainedOuterLevels = 0;


TemplateSubstitutionKind Kind = TemplateSubstitutionKind::Specialization;

public:

MultiLevelTemplateArgumentList() = default;


explicit
MultiLevelTemplateArgumentList(const TemplateArgumentList &TemplateArgs) {
addOuterTemplateArguments(&TemplateArgs);
}

void setKind(TemplateSubstitutionKind K) { Kind = K; }


TemplateSubstitutionKind getKind() const { return Kind; }




bool isRewrite() const {
return Kind == TemplateSubstitutionKind::Rewrite;
}



unsigned getNumLevels() const {
return TemplateArgumentLists.size() + NumRetainedOuterLevels;
}



unsigned getNumSubstitutedLevels() const {
return TemplateArgumentLists.size();
}

unsigned getNumRetainedOuterLevels() const {
return NumRetainedOuterLevels;
}



unsigned getNewDepth(unsigned OldDepth) const {
if (OldDepth < NumRetainedOuterLevels)
return OldDepth;
if (OldDepth < getNumLevels())
return NumRetainedOuterLevels;
return OldDepth - TemplateArgumentLists.size();
}


const TemplateArgument &operator()(unsigned Depth, unsigned Index) const {
assert(NumRetainedOuterLevels <= Depth && Depth < getNumLevels());
assert(Index < TemplateArgumentLists[getNumLevels() - Depth - 1].size());
return TemplateArgumentLists[getNumLevels() - Depth - 1][Index];
}





bool hasTemplateArgument(unsigned Depth, unsigned Index) const {
assert(Depth < getNumLevels());

if (Depth < NumRetainedOuterLevels)
return false;

if (Index >= TemplateArgumentLists[getNumLevels() - Depth - 1].size())
return false;

return !(*this)(Depth, Index).isNull();
}


void setArgument(unsigned Depth, unsigned Index,
TemplateArgument Arg) {
assert(NumRetainedOuterLevels <= Depth && Depth < getNumLevels());
assert(Index < TemplateArgumentLists[getNumLevels() - Depth - 1].size());
const_cast<TemplateArgument&>(
TemplateArgumentLists[getNumLevels() - Depth - 1][Index])
= Arg;
}



void addOuterTemplateArguments(const TemplateArgumentList *TemplateArgs) {
addOuterTemplateArguments(ArgList(TemplateArgs->data(),
TemplateArgs->size()));
}



void addOuterTemplateArguments(ArgList Args) {
assert(!NumRetainedOuterLevels &&
"substituted args outside retained args?");
TemplateArgumentLists.push_back(Args);
}




void addOuterRetainedLevel() {
++NumRetainedOuterLevels;
}
void addOuterRetainedLevels(unsigned Num) {
NumRetainedOuterLevels += Num;
}


const ArgList &getInnermost() const {
return TemplateArgumentLists.front();
}
};


enum TPOC {

TPOC_Call,



TPOC_Conversion,




TPOC_Other
};





class TemplatePartialOrderingContext {
TPOC Value;

public:
TemplatePartialOrderingContext(TPOC Value) : Value(Value) {}

operator TPOC() const { return Value; }
};



class DeducedTemplateArgument : public TemplateArgument {


bool DeducedFromArrayBound = false;

public:
DeducedTemplateArgument() = default;

DeducedTemplateArgument(const TemplateArgument &Arg,
bool DeducedFromArrayBound = false)
: TemplateArgument(Arg), DeducedFromArrayBound(DeducedFromArrayBound) {}



DeducedTemplateArgument(ASTContext &Ctx,
const llvm::APSInt &Value,
QualType ValueType,
bool DeducedFromArrayBound)
: TemplateArgument(Ctx, Value, ValueType),
DeducedFromArrayBound(DeducedFromArrayBound) {}



bool wasDeducedFromArrayBound() const { return DeducedFromArrayBound; }



void setDeducedFromArrayBound(bool Deduced) {
DeducedFromArrayBound = Deduced;
}
};







class LocalInstantiationScope {
public:

using DeclArgumentPack = SmallVector<VarDecl *, 4>;

private:


Sema &SemaRef;

using LocalDeclsMap =
llvm::SmallDenseMap<const Decl *,
llvm::PointerUnion<Decl *, DeclArgumentPack *>, 4>;



















LocalDeclsMap LocalDecls;


SmallVector<DeclArgumentPack *, 1> ArgumentPacks;




LocalInstantiationScope *Outer;


bool Exited = false;



bool CombineWithOuterScope;



NamedDecl *PartiallySubstitutedPack = nullptr;



const TemplateArgument *ArgsInPartiallySubstitutedPack;




unsigned NumArgsInPartiallySubstitutedPack;

public:
LocalInstantiationScope(Sema &SemaRef, bool CombineWithOuterScope = false)
: SemaRef(SemaRef), Outer(SemaRef.CurrentInstantiationScope),
CombineWithOuterScope(CombineWithOuterScope) {
SemaRef.CurrentInstantiationScope = this;
}

LocalInstantiationScope(const LocalInstantiationScope &) = delete;
LocalInstantiationScope &
operator=(const LocalInstantiationScope &) = delete;

~LocalInstantiationScope() {
Exit();
}

const Sema &getSema() const { return SemaRef; }


void Exit() {
if (Exited)
return;

for (unsigned I = 0, N = ArgumentPacks.size(); I != N; ++I)
delete ArgumentPacks[I];

SemaRef.CurrentInstantiationScope = Outer;
Exited = true;
}



LocalInstantiationScope *cloneScopes(LocalInstantiationScope *Outermost) {
if (this == Outermost) return this;



LocalInstantiationScope *oldScope = SemaRef.CurrentInstantiationScope;

LocalInstantiationScope *newScope =
new LocalInstantiationScope(SemaRef, CombineWithOuterScope);

newScope->Outer = nullptr;
if (Outer)
newScope->Outer = Outer->cloneScopes(Outermost);

newScope->PartiallySubstitutedPack = PartiallySubstitutedPack;
newScope->ArgsInPartiallySubstitutedPack = ArgsInPartiallySubstitutedPack;
newScope->NumArgsInPartiallySubstitutedPack =
NumArgsInPartiallySubstitutedPack;

for (LocalDeclsMap::iterator I = LocalDecls.begin(), E = LocalDecls.end();
I != E; ++I) {
const Decl *D = I->first;
llvm::PointerUnion<Decl *, DeclArgumentPack *> &Stored =
newScope->LocalDecls[D];
if (I->second.is<Decl *>()) {
Stored = I->second.get<Decl *>();
} else {
DeclArgumentPack *OldPack = I->second.get<DeclArgumentPack *>();
DeclArgumentPack *NewPack = new DeclArgumentPack(*OldPack);
Stored = NewPack;
newScope->ArgumentPacks.push_back(NewPack);
}
}

SemaRef.CurrentInstantiationScope = oldScope;
return newScope;
}



static void deleteScopes(LocalInstantiationScope *Scope,
LocalInstantiationScope *Outermost) {
while (Scope && Scope != Outermost) {
LocalInstantiationScope *Out = Scope->Outer;
delete Scope;
Scope = Out;
}
}









llvm::PointerUnion<Decl *, DeclArgumentPack *> *
findInstantiationOf(const Decl *D);

void InstantiatedLocal(const Decl *D, Decl *Inst);
void InstantiatedLocalPackArg(const Decl *D, VarDecl *Inst);
void MakeInstantiatedLocalArgPack(const Decl *D);













void SetPartiallySubstitutedPack(NamedDecl *Pack,
const TemplateArgument *ExplicitArgs,
unsigned NumExplicitArgs);



void ResetPartiallySubstitutedPack() {
assert(PartiallySubstitutedPack && "No partially-substituted pack");
PartiallySubstitutedPack = nullptr;
ArgsInPartiallySubstitutedPack = nullptr;
NumArgsInPartiallySubstitutedPack = 0;
}




NamedDecl *
getPartiallySubstitutedPack(const TemplateArgument **ExplicitArgs = nullptr,
unsigned *NumExplicitArgs = nullptr) const;


bool isLocalPackExpansion(const Decl *D);
};

class TemplateDeclInstantiator
: public DeclVisitor<TemplateDeclInstantiator, Decl *>
{
Sema &SemaRef;
Sema::ArgumentPackSubstitutionIndexRAII SubstIndex;
DeclContext *Owner;
const MultiLevelTemplateArgumentList &TemplateArgs;
Sema::LateInstantiatedAttrVec* LateAttrs = nullptr;
LocalInstantiationScope *StartingScope = nullptr;




SmallVector<std::pair<ClassTemplateDecl *,
ClassTemplatePartialSpecializationDecl *>, 4>
OutOfLinePartialSpecs;





SmallVector<
std::pair<VarTemplateDecl *, VarTemplatePartialSpecializationDecl *>, 4>
OutOfLineVarPartialSpecs;

public:
TemplateDeclInstantiator(Sema &SemaRef, DeclContext *Owner,
const MultiLevelTemplateArgumentList &TemplateArgs)
: SemaRef(SemaRef),
SubstIndex(SemaRef, SemaRef.ArgumentPackSubstitutionIndex),
Owner(Owner), TemplateArgs(TemplateArgs) {}


#define DECL(DERIVED, BASE) Decl *Visit ##DERIVED ##Decl(DERIVED ##Decl *D);

#define ABSTRACT_DECL(DECL)


#define OBJCCONTAINER(DERIVED, BASE)
#define FILESCOPEASM(DERIVED, BASE)
#define IMPORT(DERIVED, BASE)
#define EXPORT(DERIVED, BASE)
#define LINKAGESPEC(DERIVED, BASE)
#define OBJCCOMPATIBLEALIAS(DERIVED, BASE)
#define OBJCMETHOD(DERIVED, BASE)
#define OBJCTYPEPARAM(DERIVED, BASE)
#define OBJCIVAR(DERIVED, BASE)
#define OBJCPROPERTY(DERIVED, BASE)
#define OBJCPROPERTYIMPL(DERIVED, BASE)
#define EMPTY(DERIVED, BASE)
#define LIFETIMEEXTENDEDTEMPORARY(DERIVED, BASE)


#define BLOCK(DERIVED, BASE)
#define CAPTURED(DERIVED, BASE)
#define IMPLICITPARAM(DERIVED, BASE)

#include "clang/AST/DeclNodes.inc"

enum class RewriteKind { None, RewriteSpaceshipAsEqualEqual };

void adjustForRewrite(RewriteKind RK, FunctionDecl *Orig, QualType &T,
TypeSourceInfo *&TInfo,
DeclarationNameInfo &NameInfo);


Decl *VisitCXXMethodDecl(CXXMethodDecl *D,
TemplateParameterList *TemplateParams,
Optional<const ASTTemplateArgumentListInfo *>
ClassScopeSpecializationArgs = llvm::None,
RewriteKind RK = RewriteKind::None);
Decl *VisitFunctionDecl(FunctionDecl *D,
TemplateParameterList *TemplateParams,
RewriteKind RK = RewriteKind::None);
Decl *VisitDecl(Decl *D);
Decl *VisitVarDecl(VarDecl *D, bool InstantiatingVarTemplate,
ArrayRef<BindingDecl *> *Bindings = nullptr);
Decl *VisitBaseUsingDecls(BaseUsingDecl *D, BaseUsingDecl *Inst,
LookupResult *Lookup);



void enableLateAttributeInstantiation(Sema::LateInstantiatedAttrVec *LA) {
LateAttrs = LA;
StartingScope = SemaRef.CurrentInstantiationScope;
}


void disableLateAttributeInstantiation() {
LateAttrs = nullptr;
StartingScope = nullptr;
}

LocalInstantiationScope *getStartingScope() const { return StartingScope; }

using delayed_partial_spec_iterator = SmallVectorImpl<std::pair<
ClassTemplateDecl *, ClassTemplatePartialSpecializationDecl *>>::iterator;

using delayed_var_partial_spec_iterator = SmallVectorImpl<std::pair<
VarTemplateDecl *, VarTemplatePartialSpecializationDecl *>>::iterator;





delayed_partial_spec_iterator delayed_partial_spec_begin() {
return OutOfLinePartialSpecs.begin();
}

delayed_var_partial_spec_iterator delayed_var_partial_spec_begin() {
return OutOfLineVarPartialSpecs.begin();
}





delayed_partial_spec_iterator delayed_partial_spec_end() {
return OutOfLinePartialSpecs.end();
}

delayed_var_partial_spec_iterator delayed_var_partial_spec_end() {
return OutOfLineVarPartialSpecs.end();
}


TypeSourceInfo *SubstFunctionType(FunctionDecl *D,
SmallVectorImpl<ParmVarDecl *> &Params);
bool InitFunctionInstantiation(FunctionDecl *New, FunctionDecl *Tmpl);
bool InitMethodInstantiation(CXXMethodDecl *New, CXXMethodDecl *Tmpl);

bool SubstDefaultedFunction(FunctionDecl *New, FunctionDecl *Tmpl);

TemplateParameterList *
SubstTemplateParams(TemplateParameterList *List);

bool SubstQualifier(const DeclaratorDecl *OldDecl,
DeclaratorDecl *NewDecl);
bool SubstQualifier(const TagDecl *OldDecl,
TagDecl *NewDecl);

Decl *VisitVarTemplateSpecializationDecl(
VarTemplateDecl *VarTemplate, VarDecl *FromVar,
const TemplateArgumentListInfo &TemplateArgsInfo,
ArrayRef<TemplateArgument> Converted,
VarTemplateSpecializationDecl *PrevDecl = nullptr);

Decl *InstantiateTypedefNameDecl(TypedefNameDecl *D, bool IsTypeAlias);
ClassTemplatePartialSpecializationDecl *
InstantiateClassTemplatePartialSpecialization(
ClassTemplateDecl *ClassTemplate,
ClassTemplatePartialSpecializationDecl *PartialSpec);
VarTemplatePartialSpecializationDecl *
InstantiateVarTemplatePartialSpecialization(
VarTemplateDecl *VarTemplate,
VarTemplatePartialSpecializationDecl *PartialSpec);
void InstantiateEnumDefinition(EnumDecl *Enum, EnumDecl *Pattern);

private:
template<typename T>
Decl *instantiateUnresolvedUsingDecl(T *D,
bool InstantiatingPackElement = false);
};

}

#endif
