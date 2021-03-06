












#if !defined(LLVM_CLANG_SEMA_PARSEDTEMPLATE_H)
#define LLVM_CLANG_SEMA_PARSEDTEMPLATE_H

#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/TemplateKinds.h"
#include "clang/Sema/DeclSpec.h"
#include "clang/Sema/Ownership.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <cstdlib>
#include <new>

namespace clang {

class ParsedTemplateArgument {
public:

enum KindType {

Type,

NonType,

Template
};




ParsedTemplateArgument() : Kind(Type), Arg(nullptr) { }





ParsedTemplateArgument(KindType Kind, void *Arg, SourceLocation Loc)
: Kind(Kind), Arg(Arg), Loc(Loc) { }










ParsedTemplateArgument(const CXXScopeSpec &SS,
ParsedTemplateTy Template,
SourceLocation TemplateLoc)
: Kind(ParsedTemplateArgument::Template),
Arg(Template.getAsOpaquePtr()),
SS(SS), Loc(TemplateLoc), EllipsisLoc() { }


bool isInvalid() const { return Arg == nullptr; }


KindType getKind() const { return Kind; }


ParsedType getAsType() const {
assert(Kind == Type && "Not a template type argument");
return ParsedType::getFromOpaquePtr(Arg);
}


Expr *getAsExpr() const {
assert(Kind == NonType && "Not a non-type template argument");
return static_cast<Expr*>(Arg);
}


ParsedTemplateTy getAsTemplate() const {
assert(Kind == Template && "Not a template template argument");
return ParsedTemplateTy::getFromOpaquePtr(Arg);
}


SourceLocation getLocation() const { return Loc; }



const CXXScopeSpec &getScopeSpec() const {
assert(Kind == Template &&
"Only template template arguments can have a scope specifier");
return SS;
}



SourceLocation getEllipsisLoc() const {
assert(Kind == Template &&
"Only template template arguments can have an ellipsis");
return EllipsisLoc;
}





ParsedTemplateArgument getTemplatePackExpansion(
SourceLocation EllipsisLoc) const;

private:
KindType Kind;




void *Arg;



CXXScopeSpec SS;


SourceLocation Loc;



SourceLocation EllipsisLoc;
};












struct TemplateIdAnnotation final
: private llvm::TrailingObjects<TemplateIdAnnotation,
ParsedTemplateArgument> {
friend TrailingObjects;


SourceLocation TemplateKWLoc;



SourceLocation TemplateNameLoc;


IdentifierInfo *Name;


OverloadedOperatorKind Operator;



ParsedTemplateTy Template;




TemplateNameKind Kind;



SourceLocation LAngleLoc;



SourceLocation RAngleLoc;


unsigned NumArgs;



bool ArgsInvalid;


ParsedTemplateArgument *getTemplateArgs() {
return getTrailingObjects<ParsedTemplateArgument>();
}



static TemplateIdAnnotation *
Create(SourceLocation TemplateKWLoc, SourceLocation TemplateNameLoc,
IdentifierInfo *Name, OverloadedOperatorKind OperatorKind,
ParsedTemplateTy OpaqueTemplateName, TemplateNameKind TemplateKind,
SourceLocation LAngleLoc, SourceLocation RAngleLoc,
ArrayRef<ParsedTemplateArgument> TemplateArgs, bool ArgsInvalid,
SmallVectorImpl<TemplateIdAnnotation *> &CleanupList) {
TemplateIdAnnotation *TemplateId = new (llvm::safe_malloc(
totalSizeToAlloc<ParsedTemplateArgument>(TemplateArgs.size())))
TemplateIdAnnotation(TemplateKWLoc, TemplateNameLoc, Name,
OperatorKind, OpaqueTemplateName, TemplateKind,
LAngleLoc, RAngleLoc, TemplateArgs, ArgsInvalid);
CleanupList.push_back(TemplateId);
return TemplateId;
}

void Destroy() {
std::for_each(
getTemplateArgs(), getTemplateArgs() + NumArgs,
[](ParsedTemplateArgument &A) { A.~ParsedTemplateArgument(); });
this->~TemplateIdAnnotation();
free(this);
}


bool mightBeType() const {
return Kind == TNK_Non_template ||
Kind == TNK_Type_template ||
Kind == TNK_Dependent_template_name ||
Kind == TNK_Undeclared_template;
}

bool hasInvalidName() const { return Kind == TNK_Non_template; }
bool hasInvalidArgs() const { return ArgsInvalid; }

bool isInvalid() const { return hasInvalidName() || hasInvalidArgs(); }

private:
TemplateIdAnnotation(const TemplateIdAnnotation &) = delete;

TemplateIdAnnotation(SourceLocation TemplateKWLoc,
SourceLocation TemplateNameLoc, IdentifierInfo *Name,
OverloadedOperatorKind OperatorKind,
ParsedTemplateTy OpaqueTemplateName,
TemplateNameKind TemplateKind,
SourceLocation LAngleLoc, SourceLocation RAngleLoc,
ArrayRef<ParsedTemplateArgument> TemplateArgs,
bool ArgsInvalid) noexcept
: TemplateKWLoc(TemplateKWLoc), TemplateNameLoc(TemplateNameLoc),
Name(Name), Operator(OperatorKind), Template(OpaqueTemplateName),
Kind(TemplateKind), LAngleLoc(LAngleLoc), RAngleLoc(RAngleLoc),
NumArgs(TemplateArgs.size()), ArgsInvalid(ArgsInvalid) {

std::uninitialized_copy(TemplateArgs.begin(), TemplateArgs.end(),
getTemplateArgs());
}
~TemplateIdAnnotation() = default;
};


SourceRange getTemplateParamsRange(TemplateParameterList const *const *Params,
unsigned NumParams);
}

#endif
