











#if !defined(LLVM_CLANG_AST_TEMPLATENAME_H)
#define LLVM_CLANG_AST_TEMPLATENAME_H

#include "clang/AST/DependenceFlags.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include <cassert>

namespace clang {

class ASTContext;
class DependentTemplateName;
class DiagnosticBuilder;
class IdentifierInfo;
class NamedDecl;
class NestedNameSpecifier;
enum OverloadedOperatorKind : int;
class OverloadedTemplateStorage;
class AssumedTemplateStorage;
class PartialDiagnostic;
struct PrintingPolicy;
class QualifiedTemplateName;
class SubstTemplateTemplateParmPackStorage;
class SubstTemplateTemplateParmStorage;
class TemplateArgument;
class TemplateDecl;
class TemplateTemplateParmDecl;



class UncommonTemplateNameStorage {
protected:
enum Kind {
Overloaded,
Assumed,
SubstTemplateTemplateParm,
SubstTemplateTemplateParmPack
};

struct BitsTag {

unsigned Kind : 2;



unsigned Size : 30;
};

union {
struct BitsTag Bits;
void *PointerAlignment;
};

UncommonTemplateNameStorage(Kind kind, unsigned size) {
Bits.Kind = kind;
Bits.Size = size;
}

public:
unsigned size() const { return Bits.Size; }

OverloadedTemplateStorage *getAsOverloadedStorage() {
return Bits.Kind == Overloaded
? reinterpret_cast<OverloadedTemplateStorage *>(this)
: nullptr;
}

AssumedTemplateStorage *getAsAssumedTemplateName() {
return Bits.Kind == Assumed
? reinterpret_cast<AssumedTemplateStorage *>(this)
: nullptr;
}

SubstTemplateTemplateParmStorage *getAsSubstTemplateTemplateParm() {
return Bits.Kind == SubstTemplateTemplateParm
? reinterpret_cast<SubstTemplateTemplateParmStorage *>(this)
: nullptr;
}

SubstTemplateTemplateParmPackStorage *getAsSubstTemplateTemplateParmPack() {
return Bits.Kind == SubstTemplateTemplateParmPack
? reinterpret_cast<SubstTemplateTemplateParmPackStorage *>(this)
: nullptr;
}
};



class OverloadedTemplateStorage : public UncommonTemplateNameStorage {
friend class ASTContext;

OverloadedTemplateStorage(unsigned size)
: UncommonTemplateNameStorage(Overloaded, size) {}

NamedDecl **getStorage() {
return reinterpret_cast<NamedDecl **>(this + 1);
}
NamedDecl * const *getStorage() const {
return reinterpret_cast<NamedDecl *const *>(this + 1);
}

public:
using iterator = NamedDecl *const *;

iterator begin() const { return getStorage(); }
iterator end() const { return getStorage() + size(); }

llvm::ArrayRef<NamedDecl*> decls() const {
return llvm::makeArrayRef(begin(), end());
}
};







class SubstTemplateTemplateParmPackStorage
: public UncommonTemplateNameStorage, public llvm::FoldingSetNode
{
TemplateTemplateParmDecl *Parameter;
const TemplateArgument *Arguments;

public:
SubstTemplateTemplateParmPackStorage(TemplateTemplateParmDecl *Parameter,
unsigned Size,
const TemplateArgument *Arguments)
: UncommonTemplateNameStorage(SubstTemplateTemplateParmPack, Size),
Parameter(Parameter), Arguments(Arguments) {}


TemplateTemplateParmDecl *getParameterPack() const {
return Parameter;
}



TemplateArgument getArgumentPack() const;

void Profile(llvm::FoldingSetNodeID &ID, ASTContext &Context);

static void Profile(llvm::FoldingSetNodeID &ID,
ASTContext &Context,
TemplateTemplateParmDecl *Parameter,
const TemplateArgument &ArgPack);
};




























class TemplateName {
using StorageType =
llvm::PointerUnion<TemplateDecl *, UncommonTemplateNameStorage *,
QualifiedTemplateName *, DependentTemplateName *>;

StorageType Storage;

explicit TemplateName(void *Ptr);

public:

enum NameKind {

Template,


OverloadedTemplate,



AssumedTemplate,



QualifiedTemplate,



DependentTemplate,



SubstTemplateTemplateParm,




SubstTemplateTemplateParmPack
};

TemplateName() = default;
explicit TemplateName(TemplateDecl *Template);
explicit TemplateName(OverloadedTemplateStorage *Storage);
explicit TemplateName(AssumedTemplateStorage *Storage);
explicit TemplateName(SubstTemplateTemplateParmStorage *Storage);
explicit TemplateName(SubstTemplateTemplateParmPackStorage *Storage);
explicit TemplateName(QualifiedTemplateName *Qual);
explicit TemplateName(DependentTemplateName *Dep);


bool isNull() const;


NameKind getKind() const;








TemplateDecl *getAsTemplateDecl() const;








OverloadedTemplateStorage *getAsOverloadedTemplate() const;



AssumedTemplateStorage *getAsAssumedTemplateName() const;






SubstTemplateTemplateParmStorage *getAsSubstTemplateTemplateParm() const;






SubstTemplateTemplateParmPackStorage *
getAsSubstTemplateTemplateParmPack() const;



QualifiedTemplateName *getAsQualifiedTemplateName() const;



DependentTemplateName *getAsDependentTemplateName() const;

TemplateName getUnderlying() const;




TemplateName getNameToSubstitute() const;

TemplateNameDependence getDependence() const;


bool isDependent() const;



bool isInstantiationDependent() const;



bool containsUnexpandedParameterPack() const;









void print(raw_ostream &OS, const PrintingPolicy &Policy,
bool SuppressNNS = false) const;


void dump(raw_ostream &OS) const;



void dump() const;

void Profile(llvm::FoldingSetNodeID &ID) {
ID.AddPointer(Storage.getOpaqueValue());
}


void *getAsVoidPointer() const { return Storage.getOpaqueValue(); }


static TemplateName getFromVoidPointer(void *Ptr) {
return TemplateName(Ptr);
}
};



const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
TemplateName N);



class SubstTemplateTemplateParmStorage
: public UncommonTemplateNameStorage, public llvm::FoldingSetNode {
friend class ASTContext;

TemplateTemplateParmDecl *Parameter;
TemplateName Replacement;

SubstTemplateTemplateParmStorage(TemplateTemplateParmDecl *parameter,
TemplateName replacement)
: UncommonTemplateNameStorage(SubstTemplateTemplateParm, 0),
Parameter(parameter), Replacement(replacement) {}

public:
TemplateTemplateParmDecl *getParameter() const { return Parameter; }
TemplateName getReplacement() const { return Replacement; }

void Profile(llvm::FoldingSetNodeID &ID);

static void Profile(llvm::FoldingSetNodeID &ID,
TemplateTemplateParmDecl *parameter,
TemplateName replacement);
};

inline TemplateName TemplateName::getUnderlying() const {
if (SubstTemplateTemplateParmStorage *subst
= getAsSubstTemplateTemplateParm())
return subst->getReplacement().getUnderlying();
return *this;
}












class QualifiedTemplateName : public llvm::FoldingSetNode {
friend class ASTContext;








llvm::PointerIntPair<NestedNameSpecifier *, 1> Qualifier;



TemplateDecl *Template;

QualifiedTemplateName(NestedNameSpecifier *NNS, bool TemplateKeyword,
TemplateDecl *Template)
: Qualifier(NNS, TemplateKeyword? 1 : 0), Template(Template) {}

public:

NestedNameSpecifier *getQualifier() const { return Qualifier.getPointer(); }



bool hasTemplateKeyword() const { return Qualifier.getInt(); }



TemplateDecl *getDecl() const { return Template; }



TemplateDecl *getTemplateDecl() const { return Template; }

void Profile(llvm::FoldingSetNodeID &ID) {
Profile(ID, getQualifier(), hasTemplateKeyword(), getTemplateDecl());
}

static void Profile(llvm::FoldingSetNodeID &ID, NestedNameSpecifier *NNS,
bool TemplateKeyword, TemplateDecl *Template) {
ID.AddPointer(NNS);
ID.AddBoolean(TemplateKeyword);
ID.AddPointer(Template);
}
};









class DependentTemplateName : public llvm::FoldingSetNode {
friend class ASTContext;







llvm::PointerIntPair<NestedNameSpecifier *, 1, bool> Qualifier;


union {



const IdentifierInfo *Identifier;




OverloadedOperatorKind Operator;
};







TemplateName CanonicalTemplateName;

DependentTemplateName(NestedNameSpecifier *Qualifier,
const IdentifierInfo *Identifier)
: Qualifier(Qualifier, false), Identifier(Identifier),
CanonicalTemplateName(this) {}

DependentTemplateName(NestedNameSpecifier *Qualifier,
const IdentifierInfo *Identifier,
TemplateName Canon)
: Qualifier(Qualifier, false), Identifier(Identifier),
CanonicalTemplateName(Canon) {}

DependentTemplateName(NestedNameSpecifier *Qualifier,
OverloadedOperatorKind Operator)
: Qualifier(Qualifier, true), Operator(Operator),
CanonicalTemplateName(this) {}

DependentTemplateName(NestedNameSpecifier *Qualifier,
OverloadedOperatorKind Operator,
TemplateName Canon)
: Qualifier(Qualifier, true), Operator(Operator),
CanonicalTemplateName(Canon) {}

public:

NestedNameSpecifier *getQualifier() const { return Qualifier.getPointer(); }


bool isIdentifier() const { return !Qualifier.getInt(); }


const IdentifierInfo *getIdentifier() const {
assert(isIdentifier() && "Template name isn't an identifier?");
return Identifier;
}



bool isOverloadedOperator() const { return Qualifier.getInt(); }


OverloadedOperatorKind getOperator() const {
assert(isOverloadedOperator() &&
"Template name isn't an overloaded operator?");
return Operator;
}

void Profile(llvm::FoldingSetNodeID &ID) {
if (isIdentifier())
Profile(ID, getQualifier(), getIdentifier());
else
Profile(ID, getQualifier(), getOperator());
}

static void Profile(llvm::FoldingSetNodeID &ID, NestedNameSpecifier *NNS,
const IdentifierInfo *Identifier) {
ID.AddPointer(NNS);
ID.AddBoolean(false);
ID.AddPointer(Identifier);
}

static void Profile(llvm::FoldingSetNodeID &ID, NestedNameSpecifier *NNS,
OverloadedOperatorKind Operator) {
ID.AddPointer(NNS);
ID.AddBoolean(true);
ID.AddInteger(Operator);
}
};

}

namespace llvm {


template<>
struct PointerLikeTypeTraits<clang::TemplateName> {
static inline void *getAsVoidPointer(clang::TemplateName TN) {
return TN.getAsVoidPointer();
}

static inline clang::TemplateName getFromVoidPointer(void *Ptr) {
return clang::TemplateName::getFromVoidPointer(Ptr);
}


static constexpr int NumLowBitsAvailable = 0;
};

}

#endif
