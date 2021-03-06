













#if !defined(LLVM_CLANG_AST_ASTCONCEPT_H)
#define LLVM_CLANG_AST_ASTCONCEPT_H

#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include <utility>

namespace clang {
class ConceptDecl;
class ConceptSpecializationExpr;



class ConstraintSatisfaction : public llvm::FoldingSetNode {


const NamedDecl *ConstraintOwner = nullptr;
llvm::SmallVector<TemplateArgument, 4> TemplateArgs;

public:

ConstraintSatisfaction() = default;

ConstraintSatisfaction(const NamedDecl *ConstraintOwner,
ArrayRef<TemplateArgument> TemplateArgs) :
ConstraintOwner(ConstraintOwner), TemplateArgs(TemplateArgs.begin(),
TemplateArgs.end()) { }

using SubstitutionDiagnostic = std::pair<SourceLocation, StringRef>;
using Detail = llvm::PointerUnion<Expr *, SubstitutionDiagnostic *>;

bool IsSatisfied = false;





llvm::SmallVector<std::pair<const Expr *, Detail>, 4> Details;

void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &C) {
Profile(ID, C, ConstraintOwner, TemplateArgs);
}

static void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &C,
const NamedDecl *ConstraintOwner,
ArrayRef<TemplateArgument> TemplateArgs);
};





using UnsatisfiedConstraintRecord =
std::pair<const Expr *,
llvm::PointerUnion<Expr *,
std::pair<SourceLocation, StringRef> *>>;





struct ASTConstraintSatisfaction final :
llvm::TrailingObjects<ASTConstraintSatisfaction,
UnsatisfiedConstraintRecord> {
std::size_t NumRecords;
bool IsSatisfied : 1;

const UnsatisfiedConstraintRecord *begin() const {
return getTrailingObjects<UnsatisfiedConstraintRecord>();
}

const UnsatisfiedConstraintRecord *end() const {
return getTrailingObjects<UnsatisfiedConstraintRecord>() + NumRecords;
}

ASTConstraintSatisfaction(const ASTContext &C,
const ConstraintSatisfaction &Satisfaction);

static ASTConstraintSatisfaction *
Create(const ASTContext &C, const ConstraintSatisfaction &Satisfaction);
};



class ConceptReference {
protected:

NestedNameSpecifierLoc NestedNameSpec;



SourceLocation TemplateKWLoc;


DeclarationNameInfo ConceptName;





NamedDecl *FoundDecl;


ConceptDecl *NamedConcept;



const ASTTemplateArgumentListInfo *ArgsAsWritten;

public:

ConceptReference(NestedNameSpecifierLoc NNS, SourceLocation TemplateKWLoc,
DeclarationNameInfo ConceptNameInfo, NamedDecl *FoundDecl,
ConceptDecl *NamedConcept,
const ASTTemplateArgumentListInfo *ArgsAsWritten) :
NestedNameSpec(NNS), TemplateKWLoc(TemplateKWLoc),
ConceptName(ConceptNameInfo), FoundDecl(FoundDecl),
NamedConcept(NamedConcept), ArgsAsWritten(ArgsAsWritten) {}

ConceptReference() : NestedNameSpec(), TemplateKWLoc(), ConceptName(),
FoundDecl(nullptr), NamedConcept(nullptr), ArgsAsWritten(nullptr) {}

const NestedNameSpecifierLoc &getNestedNameSpecifierLoc() const {
return NestedNameSpec;
}

const DeclarationNameInfo &getConceptNameInfo() const { return ConceptName; }

SourceLocation getConceptNameLoc() const {
return getConceptNameInfo().getLoc();
}

SourceLocation getTemplateKWLoc() const { return TemplateKWLoc; }

NamedDecl *getFoundDecl() const {
return FoundDecl;
}

ConceptDecl *getNamedConcept() const {
return NamedConcept;
}

const ASTTemplateArgumentListInfo *getTemplateArgsAsWritten() const {
return ArgsAsWritten;
}



bool hasExplicitTemplateArgs() const {
return ArgsAsWritten != nullptr;
}
};

class TypeConstraint : public ConceptReference {


Expr *ImmediatelyDeclaredConstraint = nullptr;

public:
TypeConstraint(NestedNameSpecifierLoc NNS,
DeclarationNameInfo ConceptNameInfo, NamedDecl *FoundDecl,
ConceptDecl *NamedConcept,
const ASTTemplateArgumentListInfo *ArgsAsWritten,
Expr *ImmediatelyDeclaredConstraint) :
ConceptReference(NNS, SourceLocation(), ConceptNameInfo,
FoundDecl, NamedConcept, ArgsAsWritten),
ImmediatelyDeclaredConstraint(ImmediatelyDeclaredConstraint) {}




Expr *getImmediatelyDeclaredConstraint() const {
return ImmediatelyDeclaredConstraint;
}

void print(llvm::raw_ostream &OS, PrintingPolicy Policy) const;
};

}

#endif
