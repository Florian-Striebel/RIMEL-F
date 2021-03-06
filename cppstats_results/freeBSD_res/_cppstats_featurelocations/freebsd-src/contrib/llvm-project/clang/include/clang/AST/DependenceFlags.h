






#if !defined(LLVM_CLANG_AST_DEPENDENCEFLAGS_H)
#define LLVM_CLANG_AST_DEPENDENCEFLAGS_H

#include "clang/Basic/BitmaskEnum.h"
#include "llvm/ADT/BitmaskEnum.h"
#include <cstdint>

namespace clang {
struct ExprDependenceScope {
enum ExprDependence : uint8_t {
UnexpandedPack = 1,








Instantiation = 2,

Type = 4,

Value = 8,



Error = 16,

None = 0,
All = 31,

TypeValue = Type | Value,
TypeInstantiation = Type | Instantiation,
ValueInstantiation = Value | Instantiation,
TypeValueInstantiation = Type | Value | Instantiation,
ErrorDependent = Error | ValueInstantiation,

LLVM_MARK_AS_BITMASK_ENUM(Error)
};
};
using ExprDependence = ExprDependenceScope::ExprDependence;

struct TypeDependenceScope {
enum TypeDependence : uint8_t {


UnexpandedPack = 1,




Instantiation = 2,



Dependent = 4,

VariablyModified = 8,



Error = 16,

None = 0,
All = 31,

DependentInstantiation = Dependent | Instantiation,

LLVM_MARK_AS_BITMASK_ENUM(Error)
};
};
using TypeDependence = TypeDependenceScope::TypeDependence;

#define LLVM_COMMON_DEPENDENCE(NAME) struct NAME##Scope { enum NAME : uint8_t { UnexpandedPack = 1, Instantiation = 2, Dependent = 4, Error = 8, None = 0, DependentInstantiation = Dependent | Instantiation, All = 15, LLVM_MARK_AS_BITMASK_ENUM(Error) }; }; using NAME = NAME##Scope::NAME;
















LLVM_COMMON_DEPENDENCE(NestedNameSpecifierDependence)
LLVM_COMMON_DEPENDENCE(TemplateNameDependence)
LLVM_COMMON_DEPENDENCE(TemplateArgumentDependence)
#undef LLVM_COMMON_DEPENDENCE



class Dependence {
public:
enum Bits : uint8_t {
None = 0,


UnexpandedPack = 1,



Instantiation = 2,


Type = 4,


Value = 8,


Dependent = Type | Value,

Error = 16,

VariablyModified = 32,


Syntactic = UnexpandedPack | Instantiation | Error,

LLVM_MARK_AS_BITMASK_ENUM(VariablyModified)
};

Dependence() : V(None) {}

Dependence(TypeDependence D)
: V(translate(D, TypeDependence::UnexpandedPack, UnexpandedPack) |
translate(D, TypeDependence::Instantiation, Instantiation) |
translate(D, TypeDependence::Dependent, Dependent) |
translate(D, TypeDependence::Error, Error) |
translate(D, TypeDependence::VariablyModified, VariablyModified)) {}

Dependence(ExprDependence D)
: V(translate(D, ExprDependence::UnexpandedPack, UnexpandedPack) |
translate(D, ExprDependence::Instantiation, Instantiation) |
translate(D, ExprDependence::Type, Type) |
translate(D, ExprDependence::Value, Value) |
translate(D, ExprDependence::Error, Error)) {}

Dependence(NestedNameSpecifierDependence D) :
V ( translate(D, NNSDependence::UnexpandedPack, UnexpandedPack) |
translate(D, NNSDependence::Instantiation, Instantiation) |
translate(D, NNSDependence::Dependent, Dependent) |
translate(D, NNSDependence::Error, Error)) {}

Dependence(TemplateArgumentDependence D)
: V(translate(D, TADependence::UnexpandedPack, UnexpandedPack) |
translate(D, TADependence::Instantiation, Instantiation) |
translate(D, TADependence::Dependent, Dependent) |
translate(D, TADependence::Error, Error)) {}

Dependence(TemplateNameDependence D)
: V(translate(D, TNDependence::UnexpandedPack, UnexpandedPack) |
translate(D, TNDependence::Instantiation, Instantiation) |
translate(D, TNDependence::Dependent, Dependent) |
translate(D, TNDependence::Error, Error)) {}


Dependence syntactic() {
Dependence Result = *this;
Result.V &= Syntactic;
return Result;
}

TypeDependence type() const {
return translate(V, UnexpandedPack, TypeDependence::UnexpandedPack) |
translate(V, Instantiation, TypeDependence::Instantiation) |
translate(V, Dependent, TypeDependence::Dependent) |
translate(V, Error, TypeDependence::Error) |
translate(V, VariablyModified, TypeDependence::VariablyModified);
}

ExprDependence expr() const {
return translate(V, UnexpandedPack, ExprDependence::UnexpandedPack) |
translate(V, Instantiation, ExprDependence::Instantiation) |
translate(V, Type, ExprDependence::Type) |
translate(V, Value, ExprDependence::Value) |
translate(V, Error, ExprDependence::Error);
}

NestedNameSpecifierDependence nestedNameSpecifier() const {
return translate(V, UnexpandedPack, NNSDependence::UnexpandedPack) |
translate(V, Instantiation, NNSDependence::Instantiation) |
translate(V, Dependent, NNSDependence::Dependent) |
translate(V, Error, NNSDependence::Error);
}

TemplateArgumentDependence templateArgument() const {
return translate(V, UnexpandedPack, TADependence::UnexpandedPack) |
translate(V, Instantiation, TADependence::Instantiation) |
translate(V, Dependent, TADependence::Dependent) |
translate(V, Error, TADependence::Error);
}

TemplateNameDependence templateName() const {
return translate(V, UnexpandedPack, TNDependence::UnexpandedPack) |
translate(V, Instantiation, TNDependence::Instantiation) |
translate(V, Dependent, TNDependence::Dependent) |
translate(V, Error, TNDependence::Error);
}

private:
Bits V;

template <typename T, typename U>
static U translate(T Bits, T FromBit, U ToBit) {
return (Bits & FromBit) ? ToBit : static_cast<U>(0);
}


using NNSDependence = NestedNameSpecifierDependence;
using TADependence = TemplateArgumentDependence;
using TNDependence = TemplateNameDependence;
};



inline ExprDependence toExprDependence(TemplateArgumentDependence TA) {
return Dependence(TA).expr();
}
inline ExprDependence toExprDependence(TypeDependence D) {
return Dependence(D).expr();
}



inline ExprDependence toExprDependence(NestedNameSpecifierDependence D) {
return Dependence(D).expr();
}
inline ExprDependence turnTypeToValueDependence(ExprDependence D) {


return D & ~ExprDependence::Type;
}
inline ExprDependence turnValueToTypeDependence(ExprDependence D) {

if (D & ExprDependence::Value)
D |= ExprDependence::Type;
return D;
}


inline TypeDependence toTypeDependence(ExprDependence D) {
return Dependence(D).type();
}
inline TypeDependence toTypeDependence(NestedNameSpecifierDependence D) {
return Dependence(D).type();
}
inline TypeDependence toTypeDependence(TemplateNameDependence D) {
return Dependence(D).type();
}
inline TypeDependence toTypeDependence(TemplateArgumentDependence D) {
return Dependence(D).type();
}

inline TypeDependence toSyntacticDependence(TypeDependence D) {
return Dependence(D).syntactic().type();
}

inline NestedNameSpecifierDependence
toNestedNameSpecifierDependendence(TypeDependence D) {
return Dependence(D).nestedNameSpecifier();
}

inline TemplateArgumentDependence
toTemplateArgumentDependence(TypeDependence D) {
return Dependence(D).templateArgument();
}
inline TemplateArgumentDependence
toTemplateArgumentDependence(TemplateNameDependence D) {
return Dependence(D).templateArgument();
}
inline TemplateArgumentDependence
toTemplateArgumentDependence(ExprDependence D) {
return Dependence(D).templateArgument();
}

inline TemplateNameDependence
toTemplateNameDependence(NestedNameSpecifierDependence D) {
return Dependence(D).templateName();
}

LLVM_ENABLE_BITMASK_ENUMS_IN_NAMESPACE();

}
#endif
