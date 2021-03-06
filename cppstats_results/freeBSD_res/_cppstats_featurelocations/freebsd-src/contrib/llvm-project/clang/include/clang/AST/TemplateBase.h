












#if !defined(LLVM_CLANG_AST_TEMPLATEBASE_H)
#define LLVM_CLANG_AST_TEMPLATEBASE_H

#include "clang/AST/DependenceFlags.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace llvm {

class FoldingSetNodeID;




template <> struct PointerLikeTypeTraits<clang::Expr *> {
static inline void *getAsVoidPointer(clang::Expr *P) { return P; }
static inline clang::Expr *getFromVoidPointer(void *P) {
return static_cast<clang::Expr *>(P);
}
static constexpr int NumLowBitsAvailable = 2;
};

}

namespace clang {

class ASTContext;
class DiagnosticBuilder;
class Expr;
struct PrintingPolicy;
class TypeSourceInfo;
class ValueDecl;


class TemplateArgument {
public:

enum ArgKind {


Null = 0,


Type,



Declaration,



NullPtr,



Integral,



Template,



TemplateExpansion,





Expression,



Pack
};

private:


struct DA {
unsigned Kind;
void *QT;
ValueDecl *D;
};
struct I {
unsigned Kind;



unsigned BitWidth : 31;
unsigned IsUnsigned : 1;
union {

uint64_t VAL;


const uint64_t *pVal;
};
void *Type;
};
struct A {
unsigned Kind;
unsigned NumArgs;
const TemplateArgument *Args;
};
struct TA {
unsigned Kind;
unsigned NumExpansions;
void *Name;
};
struct TV {
unsigned Kind;
uintptr_t V;
};
union {
struct DA DeclArg;
struct I Integer;
struct A Args;
struct TA TemplateArg;
struct TV TypeOrValue;
};

public:

constexpr TemplateArgument() : TypeOrValue({Null, 0}) {}


TemplateArgument(QualType T, bool isNullPtr = false) {
TypeOrValue.Kind = isNullPtr ? NullPtr : Type;
TypeOrValue.V = reinterpret_cast<uintptr_t>(T.getAsOpaquePtr());
}




TemplateArgument(ValueDecl *D, QualType QT) {
assert(D && "Expected decl");
DeclArg.Kind = Declaration;
DeclArg.QT = QT.getAsOpaquePtr();
DeclArg.D = D;
}



TemplateArgument(ASTContext &Ctx, const llvm::APSInt &Value, QualType Type);



TemplateArgument(const TemplateArgument &Other, QualType Type) {
Integer = Other.Integer;
Integer.Type = Type.getAsOpaquePtr();
}









TemplateArgument(TemplateName Name) {
TemplateArg.Kind = Template;
TemplateArg.Name = Name.getAsVoidPointer();
TemplateArg.NumExpansions = 0;
}












TemplateArgument(TemplateName Name, Optional<unsigned> NumExpansions) {
TemplateArg.Kind = TemplateExpansion;
TemplateArg.Name = Name.getAsVoidPointer();
if (NumExpansions)
TemplateArg.NumExpansions = *NumExpansions + 1;
else
TemplateArg.NumExpansions = 0;
}






TemplateArgument(Expr *E) {
TypeOrValue.Kind = Expression;
TypeOrValue.V = reinterpret_cast<uintptr_t>(E);
}





explicit TemplateArgument(ArrayRef<TemplateArgument> Args) {
this->Args.Kind = Pack;
this->Args.Args = Args.data();
this->Args.NumArgs = Args.size();
}

TemplateArgument(TemplateName, bool) = delete;

static TemplateArgument getEmptyPack() { return TemplateArgument(None); }



static TemplateArgument CreatePackCopy(ASTContext &Context,
ArrayRef<TemplateArgument> Args);


ArgKind getKind() const { return (ArgKind)TypeOrValue.Kind; }


bool isNull() const { return getKind() == Null; }

TemplateArgumentDependence getDependence() const;




bool isDependent() const;



bool isInstantiationDependent() const;



bool containsUnexpandedParameterPack() const;


bool isPackExpansion() const;


QualType getAsType() const {
assert(getKind() == Type && "Unexpected kind");
return QualType::getFromOpaquePtr(reinterpret_cast<void*>(TypeOrValue.V));
}



ValueDecl *getAsDecl() const {
assert(getKind() == Declaration && "Unexpected kind");
return DeclArg.D;
}

QualType getParamTypeForDecl() const {
assert(getKind() == Declaration && "Unexpected kind");
return QualType::getFromOpaquePtr(DeclArg.QT);
}


QualType getNullPtrType() const {
assert(getKind() == NullPtr && "Unexpected kind");
return QualType::getFromOpaquePtr(reinterpret_cast<void*>(TypeOrValue.V));
}


TemplateName getAsTemplate() const {
assert(getKind() == Template && "Unexpected kind");
return TemplateName::getFromVoidPointer(TemplateArg.Name);
}



TemplateName getAsTemplateOrTemplatePattern() const {
assert((getKind() == Template || getKind() == TemplateExpansion) &&
"Unexpected kind");

return TemplateName::getFromVoidPointer(TemplateArg.Name);
}



Optional<unsigned> getNumTemplateExpansions() const;



llvm::APSInt getAsIntegral() const {
assert(getKind() == Integral && "Unexpected kind");

using namespace llvm;

if (Integer.BitWidth <= 64)
return APSInt(APInt(Integer.BitWidth, Integer.VAL), Integer.IsUnsigned);

unsigned NumWords = APInt::getNumWords(Integer.BitWidth);
return APSInt(APInt(Integer.BitWidth, makeArrayRef(Integer.pVal, NumWords)),
Integer.IsUnsigned);
}


QualType getIntegralType() const {
assert(getKind() == Integral && "Unexpected kind");
return QualType::getFromOpaquePtr(Integer.Type);
}

void setIntegralType(QualType T) {
assert(getKind() == Integral && "Unexpected kind");
Integer.Type = T.getAsOpaquePtr();
}



QualType getNonTypeTemplateArgumentType() const;


Expr *getAsExpr() const {
assert(getKind() == Expression && "Unexpected kind");
return reinterpret_cast<Expr *>(TypeOrValue.V);
}


using pack_iterator = const TemplateArgument *;



pack_iterator pack_begin() const {
assert(getKind() == Pack);
return Args.Args;
}



pack_iterator pack_end() const {
assert(getKind() == Pack);
return Args.Args + Args.NumArgs;
}



ArrayRef<TemplateArgument> pack_elements() const {
return llvm::makeArrayRef(pack_begin(), pack_end());
}



unsigned pack_size() const {
assert(getKind() == Pack);
return Args.NumArgs;
}


ArrayRef<TemplateArgument> getPackAsArray() const {
assert(getKind() == Pack);
return llvm::makeArrayRef(Args.Args, Args.NumArgs);
}



bool structurallyEquals(const TemplateArgument &Other) const;



TemplateArgument getPackExpansionPattern() const;


void print(const PrintingPolicy &Policy, raw_ostream &Out,
bool IncludeType) const;


void dump(raw_ostream &Out) const;


void dump() const;


void Profile(llvm::FoldingSetNodeID &ID, const ASTContext &Context) const;
};


struct TemplateArgumentLocInfo {
private:
struct TemplateTemplateArgLocInfo {


NestedNameSpecifier *Qualifier;
void *QualifierLocData;
SourceLocation TemplateNameLoc;
SourceLocation EllipsisLoc;
};

llvm::PointerUnion<TemplateTemplateArgLocInfo *, Expr *, TypeSourceInfo *>
Pointer;

TemplateTemplateArgLocInfo *getTemplate() const {
return Pointer.get<TemplateTemplateArgLocInfo *>();
}

public:
TemplateArgumentLocInfo() {}
TemplateArgumentLocInfo(TypeSourceInfo *Declarator) { Pointer = Declarator; }

TemplateArgumentLocInfo(Expr *E) { Pointer = E; }


TemplateArgumentLocInfo(ASTContext &Ctx, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateNameLoc,
SourceLocation EllipsisLoc);

TypeSourceInfo *getAsTypeSourceInfo() const {
return Pointer.get<TypeSourceInfo *>();
}

Expr *getAsExpr() const { return Pointer.get<Expr *>(); }

NestedNameSpecifierLoc getTemplateQualifierLoc() const {
const auto *Template = getTemplate();
return NestedNameSpecifierLoc(Template->Qualifier,
Template->QualifierLocData);
}

SourceLocation getTemplateNameLoc() const {
return getTemplate()->TemplateNameLoc;
}

SourceLocation getTemplateEllipsisLoc() const {
return getTemplate()->EllipsisLoc;
}
};



class TemplateArgumentLoc {
TemplateArgument Argument;
TemplateArgumentLocInfo LocInfo;

public:
TemplateArgumentLoc() {}

TemplateArgumentLoc(const TemplateArgument &Argument,
TemplateArgumentLocInfo Opaque)
: Argument(Argument), LocInfo(Opaque) {}

TemplateArgumentLoc(const TemplateArgument &Argument, TypeSourceInfo *TInfo)
: Argument(Argument), LocInfo(TInfo) {
assert(Argument.getKind() == TemplateArgument::Type);
}

TemplateArgumentLoc(const TemplateArgument &Argument, Expr *E)
: Argument(Argument), LocInfo(E) {



assert(Argument.getKind() == TemplateArgument::NullPtr ||
Argument.getKind() == TemplateArgument::Integral ||
Argument.getKind() == TemplateArgument::Declaration ||
Argument.getKind() == TemplateArgument::Expression);
}

TemplateArgumentLoc(ASTContext &Ctx, const TemplateArgument &Argument,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation TemplateNameLoc,
SourceLocation EllipsisLoc = SourceLocation())
: Argument(Argument),
LocInfo(Ctx, QualifierLoc, TemplateNameLoc, EllipsisLoc) {
assert(Argument.getKind() == TemplateArgument::Template ||
Argument.getKind() == TemplateArgument::TemplateExpansion);
}


SourceLocation getLocation() const {
if (Argument.getKind() == TemplateArgument::Template ||
Argument.getKind() == TemplateArgument::TemplateExpansion)
return getTemplateNameLoc();

return getSourceRange().getBegin();
}


SourceRange getSourceRange() const LLVM_READONLY;

const TemplateArgument &getArgument() const {
return Argument;
}

TemplateArgumentLocInfo getLocInfo() const {
return LocInfo;
}

TypeSourceInfo *getTypeSourceInfo() const {
if (Argument.getKind() != TemplateArgument::Type)
return nullptr;
return LocInfo.getAsTypeSourceInfo();
}

Expr *getSourceExpression() const {
assert(Argument.getKind() == TemplateArgument::Expression);
return LocInfo.getAsExpr();
}

Expr *getSourceDeclExpression() const {
assert(Argument.getKind() == TemplateArgument::Declaration);
return LocInfo.getAsExpr();
}

Expr *getSourceNullPtrExpression() const {
assert(Argument.getKind() == TemplateArgument::NullPtr);
return LocInfo.getAsExpr();
}

Expr *getSourceIntegralExpression() const {
assert(Argument.getKind() == TemplateArgument::Integral);
return LocInfo.getAsExpr();
}

NestedNameSpecifierLoc getTemplateQualifierLoc() const {
if (Argument.getKind() != TemplateArgument::Template &&
Argument.getKind() != TemplateArgument::TemplateExpansion)
return NestedNameSpecifierLoc();
return LocInfo.getTemplateQualifierLoc();
}

SourceLocation getTemplateNameLoc() const {
if (Argument.getKind() != TemplateArgument::Template &&
Argument.getKind() != TemplateArgument::TemplateExpansion)
return SourceLocation();
return LocInfo.getTemplateNameLoc();
}

SourceLocation getTemplateEllipsisLoc() const {
if (Argument.getKind() != TemplateArgument::TemplateExpansion)
return SourceLocation();
return LocInfo.getTemplateEllipsisLoc();
}
};



class TemplateArgumentListInfo {
SmallVector<TemplateArgumentLoc, 8> Arguments;
SourceLocation LAngleLoc;
SourceLocation RAngleLoc;

public:
TemplateArgumentListInfo() = default;

TemplateArgumentListInfo(SourceLocation LAngleLoc,
SourceLocation RAngleLoc)
: LAngleLoc(LAngleLoc), RAngleLoc(RAngleLoc) {}



void *operator new(size_t bytes, ASTContext &C) = delete;

SourceLocation getLAngleLoc() const { return LAngleLoc; }
SourceLocation getRAngleLoc() const { return RAngleLoc; }

void setLAngleLoc(SourceLocation Loc) { LAngleLoc = Loc; }
void setRAngleLoc(SourceLocation Loc) { RAngleLoc = Loc; }

unsigned size() const { return Arguments.size(); }

const TemplateArgumentLoc *getArgumentArray() const {
return Arguments.data();
}

llvm::ArrayRef<TemplateArgumentLoc> arguments() const {
return Arguments;
}

const TemplateArgumentLoc &operator[](unsigned I) const {
return Arguments[I];
}

TemplateArgumentLoc &operator[](unsigned I) {
return Arguments[I];
}

void addArgument(const TemplateArgumentLoc &Loc) {
Arguments.push_back(Loc);
}
};





struct ASTTemplateArgumentListInfo final
: private llvm::TrailingObjects<ASTTemplateArgumentListInfo,
TemplateArgumentLoc> {
private:
friend class ASTNodeImporter;
friend TrailingObjects;

ASTTemplateArgumentListInfo(const TemplateArgumentListInfo &List);

public:

SourceLocation LAngleLoc;


SourceLocation RAngleLoc;


unsigned NumTemplateArgs;

SourceLocation getLAngleLoc() const { return LAngleLoc; }
SourceLocation getRAngleLoc() const { return RAngleLoc; }


const TemplateArgumentLoc *getTemplateArgs() const {
return getTrailingObjects<TemplateArgumentLoc>();
}
unsigned getNumTemplateArgs() const { return NumTemplateArgs; }

llvm::ArrayRef<TemplateArgumentLoc> arguments() const {
return llvm::makeArrayRef(getTemplateArgs(), getNumTemplateArgs());
}

const TemplateArgumentLoc &operator[](unsigned I) const {
return getTemplateArgs()[I];
}

static const ASTTemplateArgumentListInfo *
Create(const ASTContext &C, const TemplateArgumentListInfo &List);
};








struct alignas(void *) ASTTemplateKWAndArgsInfo {

SourceLocation LAngleLoc;


SourceLocation RAngleLoc;





SourceLocation TemplateKWLoc;


unsigned NumTemplateArgs;

void initializeFrom(SourceLocation TemplateKWLoc,
const TemplateArgumentListInfo &List,
TemplateArgumentLoc *OutArgArray);



void initializeFrom(SourceLocation TemplateKWLoc,
const TemplateArgumentListInfo &List,
TemplateArgumentLoc *OutArgArray,
TemplateArgumentDependence &Deps);
void initializeFrom(SourceLocation TemplateKWLoc);

void copyInto(const TemplateArgumentLoc *ArgArray,
TemplateArgumentListInfo &List) const;
};

const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
const TemplateArgument &Arg);

inline TemplateSpecializationType::iterator
TemplateSpecializationType::end() const {
return getArgs() + getNumArgs();
}

inline DependentTemplateSpecializationType::iterator
DependentTemplateSpecializationType::end() const {
return getArgs() + getNumArgs();
}

inline const TemplateArgument &
TemplateSpecializationType::getArg(unsigned Idx) const {
assert(Idx < getNumArgs() && "Template argument out of range");
return getArgs()[Idx];
}

inline const TemplateArgument &
DependentTemplateSpecializationType::getArg(unsigned Idx) const {
assert(Idx < getNumArgs() && "Template argument out of range");
return getArgs()[Idx];
}

inline const TemplateArgument &AutoType::getArg(unsigned Idx) const {
assert(Idx < getNumArgs() && "Template argument out of range");
return getArgs()[Idx];
}

}

#endif
