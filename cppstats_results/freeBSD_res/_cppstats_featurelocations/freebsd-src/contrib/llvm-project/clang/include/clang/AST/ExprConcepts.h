












#if !defined(LLVM_CLANG_AST_EXPRCONCEPTS_H)
#define LLVM_CLANG_AST_EXPRCONCEPTS_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConcept.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/Support/TrailingObjects.h"
#include <utility>
#include <string>

namespace clang {
class ASTStmtReader;
class ASTStmtWriter;






class ConceptSpecializationExpr final : public Expr, public ConceptReference,
private llvm::TrailingObjects<ConceptSpecializationExpr,
TemplateArgument> {
friend class ASTStmtReader;
friend TrailingObjects;
public:
using SubstitutionDiagnostic = std::pair<SourceLocation, std::string>;

protected:


unsigned NumTemplateArgs;




ASTConstraintSatisfaction *Satisfaction;

ConceptSpecializationExpr(const ASTContext &C, NestedNameSpecifierLoc NNS,
SourceLocation TemplateKWLoc,
DeclarationNameInfo ConceptNameInfo,
NamedDecl *FoundDecl, ConceptDecl *NamedConcept,
const ASTTemplateArgumentListInfo *ArgsAsWritten,
ArrayRef<TemplateArgument> ConvertedArgs,
const ConstraintSatisfaction *Satisfaction);

ConceptSpecializationExpr(const ASTContext &C, ConceptDecl *NamedConcept,
ArrayRef<TemplateArgument> ConvertedArgs,
const ConstraintSatisfaction *Satisfaction,
bool Dependent,
bool ContainsUnexpandedParameterPack);

ConceptSpecializationExpr(EmptyShell Empty, unsigned NumTemplateArgs);

public:

static ConceptSpecializationExpr *
Create(const ASTContext &C, NestedNameSpecifierLoc NNS,
SourceLocation TemplateKWLoc, DeclarationNameInfo ConceptNameInfo,
NamedDecl *FoundDecl, ConceptDecl *NamedConcept,
const ASTTemplateArgumentListInfo *ArgsAsWritten,
ArrayRef<TemplateArgument> ConvertedArgs,
const ConstraintSatisfaction *Satisfaction);

static ConceptSpecializationExpr *
Create(const ASTContext &C, ConceptDecl *NamedConcept,
ArrayRef<TemplateArgument> ConvertedArgs,
const ConstraintSatisfaction *Satisfaction,
bool Dependent,
bool ContainsUnexpandedParameterPack);

static ConceptSpecializationExpr *
Create(ASTContext &C, EmptyShell Empty, unsigned NumTemplateArgs);

ArrayRef<TemplateArgument> getTemplateArguments() const {
return ArrayRef<TemplateArgument>(getTrailingObjects<TemplateArgument>(),
NumTemplateArgs);
}


void setTemplateArguments(ArrayRef<TemplateArgument> Converted);




bool isSatisfied() const {
assert(!isValueDependent()
&& "isSatisfied called on a dependent ConceptSpecializationExpr");
return Satisfaction->IsSatisfied;
}




const ASTConstraintSatisfaction &getSatisfaction() const {
assert(!isValueDependent()
&& "getSatisfaction called on dependent ConceptSpecializationExpr");
return *Satisfaction;
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ConceptSpecializationExprClass;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return ConceptName.getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY {



return ArgsAsWritten->RAngleLoc.isValid() ? ArgsAsWritten->RAngleLoc
: ConceptName.getEndLoc();
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};

namespace concepts {



class Requirement {
public:


enum RequirementKind { RK_Type, RK_Simple, RK_Compound, RK_Nested };
private:
const RequirementKind Kind;

bool Dependent : 1;
bool ContainsUnexpandedParameterPack : 1;
bool Satisfied : 1;
public:
struct SubstitutionDiagnostic {
StringRef SubstitutedEntity;



SourceLocation DiagLoc;
StringRef DiagMessage;
};

Requirement(RequirementKind Kind, bool IsDependent,
bool ContainsUnexpandedParameterPack, bool IsSatisfied = true) :
Kind(Kind), Dependent(IsDependent),
ContainsUnexpandedParameterPack(ContainsUnexpandedParameterPack),
Satisfied(IsSatisfied) {}

RequirementKind getKind() const { return Kind; }

bool isSatisfied() const {
assert(!Dependent &&
"isSatisfied can only be called on non-dependent requirements.");
return Satisfied;
}

void setSatisfied(bool IsSatisfied) {
assert(!Dependent &&
"setSatisfied can only be called on non-dependent requirements.");
Satisfied = IsSatisfied;
}

void setDependent(bool IsDependent) { Dependent = IsDependent; }
bool isDependent() const { return Dependent; }

void setContainsUnexpandedParameterPack(bool Contains) {
ContainsUnexpandedParameterPack = Contains;
}
bool containsUnexpandedParameterPack() const {
return ContainsUnexpandedParameterPack;
}
};



class TypeRequirement : public Requirement {
public:
enum SatisfactionStatus {
SS_Dependent,
SS_SubstitutionFailure,
SS_Satisfied
};
private:
llvm::PointerUnion<SubstitutionDiagnostic *, TypeSourceInfo *> Value;
SatisfactionStatus Status;
public:
friend ASTStmtReader;
friend ASTStmtWriter;





TypeRequirement(TypeSourceInfo *T);



TypeRequirement(SubstitutionDiagnostic *Diagnostic) :
Requirement(RK_Type, false, false, false), Value(Diagnostic),
Status(SS_SubstitutionFailure) {}

SatisfactionStatus getSatisfactionStatus() const { return Status; }
void setSatisfactionStatus(SatisfactionStatus Status) {
this->Status = Status;
}

bool isSubstitutionFailure() const {
return Status == SS_SubstitutionFailure;
}

SubstitutionDiagnostic *getSubstitutionDiagnostic() const {
assert(Status == SS_SubstitutionFailure &&
"Attempted to get substitution diagnostic when there has been no "
"substitution failure.");
return Value.get<SubstitutionDiagnostic *>();
}

TypeSourceInfo *getType() const {
assert(!isSubstitutionFailure() &&
"Attempted to get type when there has been a substitution failure.");
return Value.get<TypeSourceInfo *>();
}

static bool classof(const Requirement *R) {
return R->getKind() == RK_Type;
}
};



class ExprRequirement : public Requirement {
public:
enum SatisfactionStatus {
SS_Dependent,
SS_ExprSubstitutionFailure,
SS_NoexceptNotMet,
SS_TypeRequirementSubstitutionFailure,
SS_ConstraintsNotSatisfied,
SS_Satisfied
};
class ReturnTypeRequirement {
llvm::PointerIntPair<
llvm::PointerUnion<TemplateParameterList *, SubstitutionDiagnostic *>,
1, bool>
TypeConstraintInfo;
public:
friend ASTStmtReader;
friend ASTStmtWriter;


ReturnTypeRequirement() : TypeConstraintInfo(nullptr, 0) {}



ReturnTypeRequirement(SubstitutionDiagnostic *SubstDiag) :
TypeConstraintInfo(SubstDiag, 0) {}







ReturnTypeRequirement(TemplateParameterList *TPL);

bool isDependent() const {
return TypeConstraintInfo.getInt();
}

bool containsUnexpandedParameterPack() const {
if (!isTypeConstraint())
return false;
return getTypeConstraintTemplateParameterList()
->containsUnexpandedParameterPack();
}

bool isEmpty() const {
return TypeConstraintInfo.getPointer().isNull();
}

bool isSubstitutionFailure() const {
return !isEmpty() &&
TypeConstraintInfo.getPointer().is<SubstitutionDiagnostic *>();
}

bool isTypeConstraint() const {
return !isEmpty() &&
TypeConstraintInfo.getPointer().is<TemplateParameterList *>();
}

SubstitutionDiagnostic *getSubstitutionDiagnostic() const {
assert(isSubstitutionFailure());
return TypeConstraintInfo.getPointer().get<SubstitutionDiagnostic *>();
}

const TypeConstraint *getTypeConstraint() const;

TemplateParameterList *getTypeConstraintTemplateParameterList() const {
assert(isTypeConstraint());
return TypeConstraintInfo.getPointer().get<TemplateParameterList *>();
}
};
private:
llvm::PointerUnion<Expr *, SubstitutionDiagnostic *> Value;
SourceLocation NoexceptLoc;
ReturnTypeRequirement TypeReq;
ConceptSpecializationExpr *SubstitutedConstraintExpr;
SatisfactionStatus Status;
public:
friend ASTStmtReader;
friend ASTStmtWriter;








ExprRequirement(
Expr *E, bool IsSimple, SourceLocation NoexceptLoc,
ReturnTypeRequirement Req, SatisfactionStatus Status,
ConceptSpecializationExpr *SubstitutedConstraintExpr = nullptr);










ExprRequirement(SubstitutionDiagnostic *E, bool IsSimple,
SourceLocation NoexceptLoc, ReturnTypeRequirement Req = {});

bool isSimple() const { return getKind() == RK_Simple; }
bool isCompound() const { return getKind() == RK_Compound; }

bool hasNoexceptRequirement() const { return NoexceptLoc.isValid(); }
SourceLocation getNoexceptLoc() const { return NoexceptLoc; }

SatisfactionStatus getSatisfactionStatus() const { return Status; }

bool isExprSubstitutionFailure() const {
return Status == SS_ExprSubstitutionFailure;
}

const ReturnTypeRequirement &getReturnTypeRequirement() const {
return TypeReq;
}

ConceptSpecializationExpr *
getReturnTypeRequirementSubstitutedConstraintExpr() const {
assert(Status >= SS_TypeRequirementSubstitutionFailure);
return SubstitutedConstraintExpr;
}

SubstitutionDiagnostic *getExprSubstitutionDiagnostic() const {
assert(isExprSubstitutionFailure() &&
"Attempted to get expression substitution diagnostic when there has "
"been no expression substitution failure");
return Value.get<SubstitutionDiagnostic *>();
}

Expr *getExpr() const {
assert(!isExprSubstitutionFailure() &&
"ExprRequirement has no expression because there has been a "
"substitution failure.");
return Value.get<Expr *>();
}

static bool classof(const Requirement *R) {
return R->getKind() == RK_Compound || R->getKind() == RK_Simple;
}
};



class NestedRequirement : public Requirement {
llvm::PointerUnion<Expr *, SubstitutionDiagnostic *> Value;
const ASTConstraintSatisfaction *Satisfaction = nullptr;

public:
friend ASTStmtReader;
friend ASTStmtWriter;

NestedRequirement(SubstitutionDiagnostic *SubstDiag) :
Requirement(RK_Nested, false,
false,
false), Value(SubstDiag) {}

NestedRequirement(Expr *Constraint) :
Requirement(RK_Nested, true,
Constraint->containsUnexpandedParameterPack()),
Value(Constraint) {
assert(Constraint->isInstantiationDependent() &&
"Nested requirement with non-dependent constraint must be "
"constructed with a ConstraintSatisfaction object");
}

NestedRequirement(ASTContext &C, Expr *Constraint,
const ConstraintSatisfaction &Satisfaction) :
Requirement(RK_Nested, Constraint->isInstantiationDependent(),
Constraint->containsUnexpandedParameterPack(),
Satisfaction.IsSatisfied),
Value(Constraint),
Satisfaction(ASTConstraintSatisfaction::Create(C, Satisfaction)) {}

bool isSubstitutionFailure() const {
return Value.is<SubstitutionDiagnostic *>();
}

SubstitutionDiagnostic *getSubstitutionDiagnostic() const {
assert(isSubstitutionFailure() &&
"getSubstitutionDiagnostic() may not be called when there was no "
"substitution failure.");
return Value.get<SubstitutionDiagnostic *>();
}

Expr *getConstraintExpr() const {
assert(!isSubstitutionFailure() && "getConstraintExpr() may not be called "
"on nested requirements with "
"substitution failures.");
return Value.get<Expr *>();
}

const ASTConstraintSatisfaction &getConstraintSatisfaction() const {
assert(!isSubstitutionFailure() && "getConstraintSatisfaction() may not be "
"called on nested requirements with "
"substitution failures.");
return *Satisfaction;
}

static bool classof(const Requirement *R) {
return R->getKind() == RK_Nested;
}
};

}







class RequiresExpr final : public Expr,
llvm::TrailingObjects<RequiresExpr, ParmVarDecl *,
concepts::Requirement *> {
friend TrailingObjects;
friend class ASTStmtReader;

unsigned NumLocalParameters;
unsigned NumRequirements;
RequiresExprBodyDecl *Body;
SourceLocation RBraceLoc;

unsigned numTrailingObjects(OverloadToken<ParmVarDecl *>) const {
return NumLocalParameters;
}

unsigned numTrailingObjects(OverloadToken<concepts::Requirement *>) const {
return NumRequirements;
}

RequiresExpr(ASTContext &C, SourceLocation RequiresKWLoc,
RequiresExprBodyDecl *Body,
ArrayRef<ParmVarDecl *> LocalParameters,
ArrayRef<concepts::Requirement *> Requirements,
SourceLocation RBraceLoc);
RequiresExpr(ASTContext &C, EmptyShell Empty, unsigned NumLocalParameters,
unsigned NumRequirements);

public:
static RequiresExpr *
Create(ASTContext &C, SourceLocation RequiresKWLoc,
RequiresExprBodyDecl *Body, ArrayRef<ParmVarDecl *> LocalParameters,
ArrayRef<concepts::Requirement *> Requirements,
SourceLocation RBraceLoc);
static RequiresExpr *
Create(ASTContext &C, EmptyShell Empty, unsigned NumLocalParameters,
unsigned NumRequirements);

ArrayRef<ParmVarDecl *> getLocalParameters() const {
return {getTrailingObjects<ParmVarDecl *>(), NumLocalParameters};
}

RequiresExprBodyDecl *getBody() const { return Body; }

ArrayRef<concepts::Requirement *> getRequirements() const {
return {getTrailingObjects<concepts::Requirement *>(), NumRequirements};
}



bool isSatisfied() const {
assert(!isValueDependent()
&& "isSatisfied called on a dependent RequiresExpr");
return RequiresExprBits.IsSatisfied;
}

SourceLocation getRequiresKWLoc() const {
return RequiresExprBits.RequiresKWLoc;
}

SourceLocation getRBraceLoc() const { return RBraceLoc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == RequiresExprClass;
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return RequiresExprBits.RequiresKWLoc;
}
SourceLocation getEndLoc() const LLVM_READONLY {
return RBraceLoc;
}


child_range children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}
};

}

#endif
