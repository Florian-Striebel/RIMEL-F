

















#if !defined(LLVM_CLANG_LIB_ASTMATCHERS_DYNAMIC_MARSHALLERS_H)
#define LLVM_CLANG_LIB_ASTMATCHERS_DYNAMIC_MARSHALLERS_H

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/OperationKinds.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/Dynamic/Diagnostics.h"
#include "clang/ASTMatchers/Dynamic/VariantValue.h"
#include "clang/Basic/AttrKinds.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/TypeTraits.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/Regex.h"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace clang {
namespace ast_matchers {
namespace dynamic {
namespace internal {




template <class T> struct ArgTypeTraits;
template <class T> struct ArgTypeTraits<const T &> : public ArgTypeTraits<T> {
};

template <> struct ArgTypeTraits<std::string> {
static bool hasCorrectType(const VariantValue &Value) {
return Value.isString();
}
static bool hasCorrectValue(const VariantValue &Value) { return true; }

static const std::string &get(const VariantValue &Value) {
return Value.getString();
}

static ArgKind getKind() {
return ArgKind(ArgKind::AK_String);
}

static llvm::Optional<std::string> getBestGuess(const VariantValue &) {
return llvm::None;
}
};

template <>
struct ArgTypeTraits<StringRef> : public ArgTypeTraits<std::string> {
};

template <class T> struct ArgTypeTraits<ast_matchers::internal::Matcher<T>> {
static bool hasCorrectType(const VariantValue& Value) {
return Value.isMatcher();
}
static bool hasCorrectValue(const VariantValue &Value) {
return Value.getMatcher().hasTypedMatcher<T>();
}

static ast_matchers::internal::Matcher<T> get(const VariantValue &Value) {
return Value.getMatcher().getTypedMatcher<T>();
}

static ArgKind getKind() {
return ArgKind::MakeMatcherArg(ASTNodeKind::getFromNodeKind<T>());
}

static llvm::Optional<std::string> getBestGuess(const VariantValue &) {
return llvm::None;
}
};

template <> struct ArgTypeTraits<bool> {
static bool hasCorrectType(const VariantValue &Value) {
return Value.isBoolean();
}
static bool hasCorrectValue(const VariantValue &Value) { return true; }

static bool get(const VariantValue &Value) {
return Value.getBoolean();
}

static ArgKind getKind() {
return ArgKind(ArgKind::AK_Boolean);
}

static llvm::Optional<std::string> getBestGuess(const VariantValue &) {
return llvm::None;
}
};

template <> struct ArgTypeTraits<double> {
static bool hasCorrectType(const VariantValue &Value) {
return Value.isDouble();
}
static bool hasCorrectValue(const VariantValue &Value) { return true; }

static double get(const VariantValue &Value) {
return Value.getDouble();
}

static ArgKind getKind() {
return ArgKind(ArgKind::AK_Double);
}

static llvm::Optional<std::string> getBestGuess(const VariantValue &) {
return llvm::None;
}
};

template <> struct ArgTypeTraits<unsigned> {
static bool hasCorrectType(const VariantValue &Value) {
return Value.isUnsigned();
}
static bool hasCorrectValue(const VariantValue &Value) { return true; }

static unsigned get(const VariantValue &Value) {
return Value.getUnsigned();
}

static ArgKind getKind() {
return ArgKind(ArgKind::AK_Unsigned);
}

static llvm::Optional<std::string> getBestGuess(const VariantValue &) {
return llvm::None;
}
};

template <> struct ArgTypeTraits<attr::Kind> {
private:
static Optional<attr::Kind> getAttrKind(llvm::StringRef AttrKind) {
if (!AttrKind.consume_front("attr::"))
return llvm::None;
return llvm::StringSwitch<Optional<attr::Kind>>(AttrKind)
#define ATTR(X) .Case(#X, attr::X)
#include "clang/Basic/AttrList.inc"
.Default(llvm::None);
}

public:
static bool hasCorrectType(const VariantValue &Value) {
return Value.isString();
}
static bool hasCorrectValue(const VariantValue& Value) {
return getAttrKind(Value.getString()).hasValue();
}

static attr::Kind get(const VariantValue &Value) {
return *getAttrKind(Value.getString());
}

static ArgKind getKind() {
return ArgKind(ArgKind::AK_String);
}

static llvm::Optional<std::string> getBestGuess(const VariantValue &Value);
};

template <> struct ArgTypeTraits<CastKind> {
private:
static Optional<CastKind> getCastKind(llvm::StringRef AttrKind) {
if (!AttrKind.consume_front("CK_"))
return llvm::None;
return llvm::StringSwitch<Optional<CastKind>>(AttrKind)
#define CAST_OPERATION(Name) .Case(#Name, CK_##Name)
#include "clang/AST/OperationKinds.def"
.Default(llvm::None);
}

public:
static bool hasCorrectType(const VariantValue &Value) {
return Value.isString();
}
static bool hasCorrectValue(const VariantValue& Value) {
return getCastKind(Value.getString()).hasValue();
}

static CastKind get(const VariantValue &Value) {
return *getCastKind(Value.getString());
}

static ArgKind getKind() {
return ArgKind(ArgKind::AK_String);
}

static llvm::Optional<std::string> getBestGuess(const VariantValue &Value);
};

template <> struct ArgTypeTraits<llvm::Regex::RegexFlags> {
private:
static Optional<llvm::Regex::RegexFlags> getFlags(llvm::StringRef Flags);

public:
static bool hasCorrectType(const VariantValue &Value) {
return Value.isString();
}
static bool hasCorrectValue(const VariantValue& Value) {
return getFlags(Value.getString()).hasValue();
}

static llvm::Regex::RegexFlags get(const VariantValue &Value) {
return *getFlags(Value.getString());
}

static ArgKind getKind() { return ArgKind(ArgKind::AK_String); }

static llvm::Optional<std::string> getBestGuess(const VariantValue &Value);
};

template <> struct ArgTypeTraits<OpenMPClauseKind> {
private:
static Optional<OpenMPClauseKind> getClauseKind(llvm::StringRef ClauseKind) {
return llvm::StringSwitch<Optional<OpenMPClauseKind>>(ClauseKind)
#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) .Case(#Enum, llvm::omp::Clause::Enum)
#include "llvm/Frontend/OpenMP/OMP.inc"
.Default(llvm::None);
}

public:
static bool hasCorrectType(const VariantValue &Value) {
return Value.isString();
}
static bool hasCorrectValue(const VariantValue& Value) {
return getClauseKind(Value.getString()).hasValue();
}

static OpenMPClauseKind get(const VariantValue &Value) {
return *getClauseKind(Value.getString());
}

static ArgKind getKind() { return ArgKind(ArgKind::AK_String); }

static llvm::Optional<std::string> getBestGuess(const VariantValue &Value);
};

template <> struct ArgTypeTraits<UnaryExprOrTypeTrait> {
private:
static Optional<UnaryExprOrTypeTrait>
getUnaryOrTypeTraitKind(llvm::StringRef ClauseKind) {
if (!ClauseKind.consume_front("UETT_"))
return llvm::None;
return llvm::StringSwitch<Optional<UnaryExprOrTypeTrait>>(ClauseKind)
#define UNARY_EXPR_OR_TYPE_TRAIT(Spelling, Name, Key) .Case(#Name, UETT_##Name)
#define CXX11_UNARY_EXPR_OR_TYPE_TRAIT(Spelling, Name, Key) .Case(#Name, UETT_##Name)

#include "clang/Basic/TokenKinds.def"
.Default(llvm::None);
}

public:
static bool hasCorrectType(const VariantValue &Value) {
return Value.isString();
}
static bool hasCorrectValue(const VariantValue& Value) {
return getUnaryOrTypeTraitKind(Value.getString()).hasValue();
}

static UnaryExprOrTypeTrait get(const VariantValue &Value) {
return *getUnaryOrTypeTraitKind(Value.getString());
}

static ArgKind getKind() { return ArgKind(ArgKind::AK_String); }

static llvm::Optional<std::string> getBestGuess(const VariantValue &Value);
};





class MatcherDescriptor {
public:
virtual ~MatcherDescriptor() = default;

virtual VariantMatcher create(SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) const = 0;

virtual ASTNodeKind nodeMatcherType() const { return ASTNodeKind(); }

virtual bool isBuilderMatcher() const { return false; }

virtual std::unique_ptr<MatcherDescriptor>
buildMatcherCtor(SourceRange NameRange, ArrayRef<ParserValue> Args,
Diagnostics *Error) const {
return {};
}



virtual bool isVariadic() const = 0;


virtual unsigned getNumArgs() const = 0;





virtual void getArgKinds(ASTNodeKind ThisKind, unsigned ArgNo,
std::vector<ArgKind> &ArgKinds) const = 0;








virtual bool
isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity = nullptr,
ASTNodeKind *LeastDerivedKind = nullptr) const = 0;



virtual bool isPolymorphic() const { return false; }
};

inline bool isRetKindConvertibleTo(ArrayRef<ASTNodeKind> RetKinds,
ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) {
for (const ASTNodeKind &NodeKind : RetKinds) {
if (ArgKind::MakeMatcherArg(NodeKind).isConvertibleTo(
ArgKind::MakeMatcherArg(Kind), Specificity)) {
if (LeastDerivedKind)
*LeastDerivedKind = NodeKind;
return true;
}
}
return false;
}







class FixedArgCountMatcherDescriptor : public MatcherDescriptor {
public:
using MarshallerType = VariantMatcher (*)(void (*Func)(),
StringRef MatcherName,
SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error);







FixedArgCountMatcherDescriptor(MarshallerType Marshaller, void (*Func)(),
StringRef MatcherName,
ArrayRef<ASTNodeKind> RetKinds,
ArrayRef<ArgKind> ArgKinds)
: Marshaller(Marshaller), Func(Func), MatcherName(MatcherName),
RetKinds(RetKinds.begin(), RetKinds.end()),
ArgKinds(ArgKinds.begin(), ArgKinds.end()) {}

VariantMatcher create(SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) const override {
return Marshaller(Func, MatcherName, NameRange, Args, Error);
}

bool isVariadic() const override { return false; }
unsigned getNumArgs() const override { return ArgKinds.size(); }

void getArgKinds(ASTNodeKind ThisKind, unsigned ArgNo,
std::vector<ArgKind> &Kinds) const override {
Kinds.push_back(ArgKinds[ArgNo]);
}

bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) const override {
return isRetKindConvertibleTo(RetKinds, Kind, Specificity,
LeastDerivedKind);
}

private:
const MarshallerType Marshaller;
void (* const Func)();
const std::string MatcherName;
const std::vector<ASTNodeKind> RetKinds;
const std::vector<ArgKind> ArgKinds;
};



template <class PolyMatcher>
static void mergePolyMatchers(const PolyMatcher &Poly,
std::vector<DynTypedMatcher> &Out,
ast_matchers::internal::EmptyTypeList) {}

template <class PolyMatcher, class TypeList>
static void mergePolyMatchers(const PolyMatcher &Poly,
std::vector<DynTypedMatcher> &Out, TypeList) {
Out.push_back(ast_matchers::internal::Matcher<typename TypeList::head>(Poly));
mergePolyMatchers(Poly, Out, typename TypeList::tail());
}







inline VariantMatcher outvalueToVariantMatcher(const DynTypedMatcher &Matcher) {
return VariantMatcher::SingleMatcher(Matcher);
}

template <typename T>
static VariantMatcher outvalueToVariantMatcher(const T &PolyMatcher,
typename T::ReturnTypes * =
nullptr) {
std::vector<DynTypedMatcher> Matchers;
mergePolyMatchers(PolyMatcher, Matchers, typename T::ReturnTypes());
VariantMatcher Out = VariantMatcher::PolymorphicMatcher(std::move(Matchers));
return Out;
}

template <typename T>
inline void
buildReturnTypeVectorFromTypeList(std::vector<ASTNodeKind> &RetTypes) {
RetTypes.push_back(ASTNodeKind::getFromNodeKind<typename T::head>());
buildReturnTypeVectorFromTypeList<typename T::tail>(RetTypes);
}

template <>
inline void
buildReturnTypeVectorFromTypeList<ast_matchers::internal::EmptyTypeList>(
std::vector<ASTNodeKind> &RetTypes) {}

template <typename T>
struct BuildReturnTypeVector {
static void build(std::vector<ASTNodeKind> &RetTypes) {
buildReturnTypeVectorFromTypeList<typename T::ReturnTypes>(RetTypes);
}
};

template <typename T>
struct BuildReturnTypeVector<ast_matchers::internal::Matcher<T>> {
static void build(std::vector<ASTNodeKind> &RetTypes) {
RetTypes.push_back(ASTNodeKind::getFromNodeKind<T>());
}
};

template <typename T>
struct BuildReturnTypeVector<ast_matchers::internal::BindableMatcher<T>> {
static void build(std::vector<ASTNodeKind> &RetTypes) {
RetTypes.push_back(ASTNodeKind::getFromNodeKind<T>());
}
};


template <typename ResultT, typename ArgT,
ResultT (*Func)(ArrayRef<const ArgT *>)>
VariantMatcher
variadicMatcherDescriptor(StringRef MatcherName, SourceRange NameRange,
ArrayRef<ParserValue> Args, Diagnostics *Error) {
SmallVector<ArgT *, 8> InnerArgsPtr;
InnerArgsPtr.resize_for_overwrite(Args.size());
SmallVector<ArgT, 8> InnerArgs;
InnerArgs.reserve(Args.size());

for (size_t i = 0, e = Args.size(); i != e; ++i) {
using ArgTraits = ArgTypeTraits<ArgT>;

const ParserValue &Arg = Args[i];
const VariantValue &Value = Arg.Value;
if (!ArgTraits::hasCorrectType(Value)) {
Error->addError(Arg.Range, Error->ET_RegistryWrongArgType)
<< (i + 1) << ArgTraits::getKind().asString() << Value.getTypeAsString();
return {};
}
if (!ArgTraits::hasCorrectValue(Value)) {
if (llvm::Optional<std::string> BestGuess =
ArgTraits::getBestGuess(Value)) {
Error->addError(Arg.Range, Error->ET_RegistryUnknownEnumWithReplace)
<< i + 1 << Value.getString() << *BestGuess;
} else if (Value.isString()) {
Error->addError(Arg.Range, Error->ET_RegistryValueNotFound)
<< Value.getString();
} else {


Error->addError(Arg.Range, Error->ET_RegistryWrongArgType)
<< (i + 1) << ArgTraits::getKind().asString()
<< Value.getTypeAsString();
}
return {};
}
InnerArgs.set_size(i + 1);
InnerArgsPtr[i] = new (&InnerArgs[i]) ArgT(ArgTraits::get(Value));
}
return outvalueToVariantMatcher(Func(InnerArgsPtr));
}








class VariadicFuncMatcherDescriptor : public MatcherDescriptor {
public:
using RunFunc = VariantMatcher (*)(StringRef MatcherName,
SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error);

template <typename ResultT, typename ArgT,
ResultT (*F)(ArrayRef<const ArgT *>)>
VariadicFuncMatcherDescriptor(
ast_matchers::internal::VariadicFunction<ResultT, ArgT, F> Func,
StringRef MatcherName)
: Func(&variadicMatcherDescriptor<ResultT, ArgT, F>),
MatcherName(MatcherName.str()),
ArgsKind(ArgTypeTraits<ArgT>::getKind()) {
BuildReturnTypeVector<ResultT>::build(RetKinds);
}

VariantMatcher create(SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) const override {
return Func(MatcherName, NameRange, Args, Error);
}

bool isVariadic() const override { return true; }
unsigned getNumArgs() const override { return 0; }

void getArgKinds(ASTNodeKind ThisKind, unsigned ArgNo,
std::vector<ArgKind> &Kinds) const override {
Kinds.push_back(ArgsKind);
}

bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) const override {
return isRetKindConvertibleTo(RetKinds, Kind, Specificity,
LeastDerivedKind);
}

ASTNodeKind nodeMatcherType() const override { return RetKinds[0]; }

private:
const RunFunc Func;
const std::string MatcherName;
std::vector<ASTNodeKind> RetKinds;
const ArgKind ArgsKind;
};


class DynCastAllOfMatcherDescriptor : public VariadicFuncMatcherDescriptor {
public:
template <typename BaseT, typename DerivedT>
DynCastAllOfMatcherDescriptor(
ast_matchers::internal::VariadicDynCastAllOfMatcher<BaseT, DerivedT> Func,
StringRef MatcherName)
: VariadicFuncMatcherDescriptor(Func, MatcherName),
DerivedKind(ASTNodeKind::getFromNodeKind<DerivedT>()) {}

bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) const override {




if (VariadicFuncMatcherDescriptor::isConvertibleTo(Kind, Specificity,
LeastDerivedKind)) {
if (Kind.isSame(DerivedKind) || !Kind.isBaseOf(DerivedKind)) {
if (Specificity)
*Specificity = 0;
}
return true;
} else {
return false;
}
}

ASTNodeKind nodeMatcherType() const override { return DerivedKind; }

private:
const ASTNodeKind DerivedKind;
};


#define CHECK_ARG_COUNT(count) if (Args.size() != count) { Error->addError(NameRange, Error->ET_RegistryWrongArgCount) << count << Args.size(); return VariantMatcher(); }






#define CHECK_ARG_TYPE(index, type) if (!ArgTypeTraits<type>::hasCorrectType(Args[index].Value)) { Error->addError(Args[index].Range, Error->ET_RegistryWrongArgType) << (index + 1) << ArgTypeTraits<type>::getKind().asString() << Args[index].Value.getTypeAsString(); return VariantMatcher(); } if (!ArgTypeTraits<type>::hasCorrectValue(Args[index].Value)) { if (llvm::Optional<std::string> BestGuess = ArgTypeTraits<type>::getBestGuess(Args[index].Value)) { Error->addError(Args[index].Range, Error->ET_RegistryUnknownEnumWithReplace) << index + 1 << Args[index].Value.getString() << *BestGuess; } else if (Args[index].Value.isString()) { Error->addError(Args[index].Range, Error->ET_RegistryValueNotFound) << Args[index].Value.getString(); } return VariantMatcher(); }




















template <typename ReturnType>
static VariantMatcher matcherMarshall0(void (*Func)(), StringRef MatcherName,
SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) {
using FuncType = ReturnType (*)();
CHECK_ARG_COUNT(0);
return outvalueToVariantMatcher(reinterpret_cast<FuncType>(Func)());
}


template <typename ReturnType, typename ArgType1>
static VariantMatcher matcherMarshall1(void (*Func)(), StringRef MatcherName,
SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) {
using FuncType = ReturnType (*)(ArgType1);
CHECK_ARG_COUNT(1);
CHECK_ARG_TYPE(0, ArgType1);
return outvalueToVariantMatcher(reinterpret_cast<FuncType>(Func)(
ArgTypeTraits<ArgType1>::get(Args[0].Value)));
}


template <typename ReturnType, typename ArgType1, typename ArgType2>
static VariantMatcher matcherMarshall2(void (*Func)(), StringRef MatcherName,
SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) {
using FuncType = ReturnType (*)(ArgType1, ArgType2);
CHECK_ARG_COUNT(2);
CHECK_ARG_TYPE(0, ArgType1);
CHECK_ARG_TYPE(1, ArgType2);
return outvalueToVariantMatcher(reinterpret_cast<FuncType>(Func)(
ArgTypeTraits<ArgType1>::get(Args[0].Value),
ArgTypeTraits<ArgType2>::get(Args[1].Value)));
}

#undef CHECK_ARG_COUNT
#undef CHECK_ARG_TYPE



template <template <typename ToArg, typename FromArg> class ArgumentAdapterT,
typename FromTypes, typename ToTypes>
class AdaptativeOverloadCollector {
public:
AdaptativeOverloadCollector(
StringRef Name, std::vector<std::unique_ptr<MatcherDescriptor>> &Out)
: Name(Name), Out(Out) {
collect(FromTypes());
}

private:
using AdaptativeFunc = ast_matchers::internal::ArgumentAdaptingMatcherFunc<
ArgumentAdapterT, FromTypes, ToTypes>;


static void collect(ast_matchers::internal::EmptyTypeList) {}



template <typename FromTypeList>
inline void collect(FromTypeList);

StringRef Name;
std::vector<std::unique_ptr<MatcherDescriptor>> &Out;
};






class OverloadedMatcherDescriptor : public MatcherDescriptor {
public:
OverloadedMatcherDescriptor(
MutableArrayRef<std::unique_ptr<MatcherDescriptor>> Callbacks)
: Overloads(std::make_move_iterator(Callbacks.begin()),
std::make_move_iterator(Callbacks.end())) {}

~OverloadedMatcherDescriptor() override = default;

VariantMatcher create(SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) const override {
std::vector<VariantMatcher> Constructed;
Diagnostics::OverloadContext Ctx(Error);
for (const auto &O : Overloads) {
VariantMatcher SubMatcher = O->create(NameRange, Args, Error);
if (!SubMatcher.isNull()) {
Constructed.push_back(SubMatcher);
}
}

if (Constructed.empty()) return VariantMatcher();

Ctx.revertErrors();
if (Constructed.size() > 1) {

Error->addError(NameRange, Error->ET_RegistryAmbiguousOverload);
return VariantMatcher();
}
return Constructed[0];
}

bool isVariadic() const override {
bool Overload0Variadic = Overloads[0]->isVariadic();
#if !defined(NDEBUG)
for (const auto &O : Overloads) {
assert(Overload0Variadic == O->isVariadic());
}
#endif
return Overload0Variadic;
}

unsigned getNumArgs() const override {
unsigned Overload0NumArgs = Overloads[0]->getNumArgs();
#if !defined(NDEBUG)
for (const auto &O : Overloads) {
assert(Overload0NumArgs == O->getNumArgs());
}
#endif
return Overload0NumArgs;
}

void getArgKinds(ASTNodeKind ThisKind, unsigned ArgNo,
std::vector<ArgKind> &Kinds) const override {
for (const auto &O : Overloads) {
if (O->isConvertibleTo(ThisKind))
O->getArgKinds(ThisKind, ArgNo, Kinds);
}
}

bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) const override {
for (const auto &O : Overloads) {
if (O->isConvertibleTo(Kind, Specificity, LeastDerivedKind))
return true;
}
return false;
}

private:
std::vector<std::unique_ptr<MatcherDescriptor>> Overloads;
};

template <typename ReturnType>
class RegexMatcherDescriptor : public MatcherDescriptor {
public:
RegexMatcherDescriptor(ReturnType (*WithFlags)(StringRef,
llvm::Regex::RegexFlags),
ReturnType (*NoFlags)(StringRef),
ArrayRef<ASTNodeKind> RetKinds)
: WithFlags(WithFlags), NoFlags(NoFlags),
RetKinds(RetKinds.begin(), RetKinds.end()) {}
bool isVariadic() const override { return true; }
unsigned getNumArgs() const override { return 0; }

void getArgKinds(ASTNodeKind ThisKind, unsigned ArgNo,
std::vector<ArgKind> &Kinds) const override {
assert(ArgNo < 2);
Kinds.push_back(ArgKind::AK_String);
}

bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) const override {
return isRetKindConvertibleTo(RetKinds, Kind, Specificity,
LeastDerivedKind);
}

VariantMatcher create(SourceRange NameRange, ArrayRef<ParserValue> Args,
Diagnostics *Error) const override {
if (Args.size() < 1 || Args.size() > 2) {
Error->addError(NameRange, Diagnostics::ET_RegistryWrongArgCount)
<< "1 or 2" << Args.size();
return VariantMatcher();
}
if (!ArgTypeTraits<StringRef>::hasCorrectType(Args[0].Value)) {
Error->addError(Args[0].Range, Error->ET_RegistryWrongArgType)
<< 1 << ArgTypeTraits<StringRef>::getKind().asString()
<< Args[0].Value.getTypeAsString();
return VariantMatcher();
}
if (Args.size() == 1) {
return outvalueToVariantMatcher(
NoFlags(ArgTypeTraits<StringRef>::get(Args[0].Value)));
}
if (!ArgTypeTraits<llvm::Regex::RegexFlags>::hasCorrectType(
Args[1].Value)) {
Error->addError(Args[1].Range, Error->ET_RegistryWrongArgType)
<< 2 << ArgTypeTraits<llvm::Regex::RegexFlags>::getKind().asString()
<< Args[1].Value.getTypeAsString();
return VariantMatcher();
}
if (!ArgTypeTraits<llvm::Regex::RegexFlags>::hasCorrectValue(
Args[1].Value)) {
if (llvm::Optional<std::string> BestGuess =
ArgTypeTraits<llvm::Regex::RegexFlags>::getBestGuess(
Args[1].Value)) {
Error->addError(Args[1].Range, Error->ET_RegistryUnknownEnumWithReplace)
<< 2 << Args[1].Value.getString() << *BestGuess;
} else {
Error->addError(Args[1].Range, Error->ET_RegistryValueNotFound)
<< Args[1].Value.getString();
}
return VariantMatcher();
}
return outvalueToVariantMatcher(
WithFlags(ArgTypeTraits<StringRef>::get(Args[0].Value),
ArgTypeTraits<llvm::Regex::RegexFlags>::get(Args[1].Value)));
}

private:
ReturnType (*const WithFlags)(StringRef, llvm::Regex::RegexFlags);
ReturnType (*const NoFlags)(StringRef);
const std::vector<ASTNodeKind> RetKinds;
};


class VariadicOperatorMatcherDescriptor : public MatcherDescriptor {
public:
using VarOp = DynTypedMatcher::VariadicOperator;

VariadicOperatorMatcherDescriptor(unsigned MinCount, unsigned MaxCount,
VarOp Op, StringRef MatcherName)
: MinCount(MinCount), MaxCount(MaxCount), Op(Op),
MatcherName(MatcherName) {}

VariantMatcher create(SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) const override {
if (Args.size() < MinCount || MaxCount < Args.size()) {
const std::string MaxStr =
(MaxCount == std::numeric_limits<unsigned>::max() ? ""
: Twine(MaxCount))
.str();
Error->addError(NameRange, Error->ET_RegistryWrongArgCount)
<< ("(" + Twine(MinCount) + ", " + MaxStr + ")") << Args.size();
return VariantMatcher();
}

std::vector<VariantMatcher> InnerArgs;
for (size_t i = 0, e = Args.size(); i != e; ++i) {
const ParserValue &Arg = Args[i];
const VariantValue &Value = Arg.Value;
if (!Value.isMatcher()) {
Error->addError(Arg.Range, Error->ET_RegistryWrongArgType)
<< (i + 1) << "Matcher<>" << Value.getTypeAsString();
return VariantMatcher();
}
InnerArgs.push_back(Value.getMatcher());
}
return VariantMatcher::VariadicOperatorMatcher(Op, std::move(InnerArgs));
}

bool isVariadic() const override { return true; }
unsigned getNumArgs() const override { return 0; }

void getArgKinds(ASTNodeKind ThisKind, unsigned ArgNo,
std::vector<ArgKind> &Kinds) const override {
Kinds.push_back(ArgKind::MakeMatcherArg(ThisKind));
}

bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) const override {
if (Specificity)
*Specificity = 1;
if (LeastDerivedKind)
*LeastDerivedKind = Kind;
return true;
}

bool isPolymorphic() const override { return true; }

private:
const unsigned MinCount;
const unsigned MaxCount;
const VarOp Op;
const StringRef MatcherName;
};

class MapAnyOfMatcherDescriptor : public MatcherDescriptor {
ASTNodeKind CladeNodeKind;
std::vector<ASTNodeKind> NodeKinds;

public:
MapAnyOfMatcherDescriptor(ASTNodeKind CladeNodeKind,
std::vector<ASTNodeKind> NodeKinds)
: CladeNodeKind(CladeNodeKind), NodeKinds(NodeKinds) {}

VariantMatcher create(SourceRange NameRange, ArrayRef<ParserValue> Args,
Diagnostics *Error) const override {

std::vector<DynTypedMatcher> NodeArgs;

for (auto NK : NodeKinds) {
std::vector<DynTypedMatcher> InnerArgs;

for (const auto &Arg : Args) {
if (!Arg.Value.isMatcher())
return {};
const VariantMatcher &VM = Arg.Value.getMatcher();
if (VM.hasTypedMatcher(NK)) {
auto DM = VM.getTypedMatcher(NK);
InnerArgs.push_back(DM);
}
}

if (InnerArgs.empty()) {
NodeArgs.push_back(
DynTypedMatcher::trueMatcher(NK).dynCastTo(CladeNodeKind));
} else {
NodeArgs.push_back(
DynTypedMatcher::constructVariadic(
ast_matchers::internal::DynTypedMatcher::VO_AllOf, NK,
InnerArgs)
.dynCastTo(CladeNodeKind));
}
}

auto Result = DynTypedMatcher::constructVariadic(
ast_matchers::internal::DynTypedMatcher::VO_AnyOf, CladeNodeKind,
NodeArgs);
Result.setAllowBind(true);
return VariantMatcher::SingleMatcher(Result);
}

bool isVariadic() const override { return true; }
unsigned getNumArgs() const override { return 0; }

void getArgKinds(ASTNodeKind ThisKind, unsigned,
std::vector<ArgKind> &Kinds) const override {
Kinds.push_back(ArgKind::MakeMatcherArg(ThisKind));
}

bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity,
ASTNodeKind *LeastDerivedKind) const override {
if (Specificity)
*Specificity = 1;
if (LeastDerivedKind)
*LeastDerivedKind = CladeNodeKind;
return true;
}
};

class MapAnyOfBuilderDescriptor : public MatcherDescriptor {
public:
VariantMatcher create(SourceRange, ArrayRef<ParserValue>,
Diagnostics *) const override {
return {};
}

bool isBuilderMatcher() const override { return true; }

std::unique_ptr<MatcherDescriptor>
buildMatcherCtor(SourceRange, ArrayRef<ParserValue> Args,
Diagnostics *) const override {

std::vector<ASTNodeKind> NodeKinds;
for (auto Arg : Args) {
if (!Arg.Value.isNodeKind())
return {};
NodeKinds.push_back(Arg.Value.getNodeKind());
}

if (NodeKinds.empty())
return {};

ASTNodeKind CladeNodeKind = NodeKinds.front().getCladeKind();

for (auto NK : NodeKinds)
{
if (!NK.getCladeKind().isSame(CladeNodeKind))
return {};
}

return std::make_unique<MapAnyOfMatcherDescriptor>(CladeNodeKind,
NodeKinds);
}

bool isVariadic() const override { return true; }

unsigned getNumArgs() const override { return 0; }

void getArgKinds(ASTNodeKind ThisKind, unsigned,
std::vector<ArgKind> &ArgKinds) const override {
ArgKinds.push_back(ArgKind::MakeNodeArg(ThisKind));
return;
}
bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity = nullptr,
ASTNodeKind *LeastDerivedKind = nullptr) const override {
if (Specificity)
*Specificity = 1;
if (LeastDerivedKind)
*LeastDerivedKind = Kind;
return true;
}

bool isPolymorphic() const override { return false; }
};





template <typename ReturnType>
std::unique_ptr<MatcherDescriptor>
makeMatcherAutoMarshall(ReturnType (*Func)(), StringRef MatcherName) {
std::vector<ASTNodeKind> RetTypes;
BuildReturnTypeVector<ReturnType>::build(RetTypes);
return std::make_unique<FixedArgCountMatcherDescriptor>(
matcherMarshall0<ReturnType>, reinterpret_cast<void (*)()>(Func),
MatcherName, RetTypes, None);
}


template <typename ReturnType, typename ArgType1>
std::unique_ptr<MatcherDescriptor>
makeMatcherAutoMarshall(ReturnType (*Func)(ArgType1), StringRef MatcherName) {
std::vector<ASTNodeKind> RetTypes;
BuildReturnTypeVector<ReturnType>::build(RetTypes);
ArgKind AK = ArgTypeTraits<ArgType1>::getKind();
return std::make_unique<FixedArgCountMatcherDescriptor>(
matcherMarshall1<ReturnType, ArgType1>,
reinterpret_cast<void (*)()>(Func), MatcherName, RetTypes, AK);
}


template <typename ReturnType, typename ArgType1, typename ArgType2>
std::unique_ptr<MatcherDescriptor>
makeMatcherAutoMarshall(ReturnType (*Func)(ArgType1, ArgType2),
StringRef MatcherName) {
std::vector<ASTNodeKind> RetTypes;
BuildReturnTypeVector<ReturnType>::build(RetTypes);
ArgKind AKs[] = { ArgTypeTraits<ArgType1>::getKind(),
ArgTypeTraits<ArgType2>::getKind() };
return std::make_unique<FixedArgCountMatcherDescriptor>(
matcherMarshall2<ReturnType, ArgType1, ArgType2>,
reinterpret_cast<void (*)()>(Func), MatcherName, RetTypes, AKs);
}

template <typename ReturnType>
std::unique_ptr<MatcherDescriptor> makeMatcherRegexMarshall(
ReturnType (*FuncFlags)(llvm::StringRef, llvm::Regex::RegexFlags),
ReturnType (*Func)(llvm::StringRef)) {
std::vector<ASTNodeKind> RetTypes;
BuildReturnTypeVector<ReturnType>::build(RetTypes);
return std::make_unique<RegexMatcherDescriptor<ReturnType>>(FuncFlags, Func,
RetTypes);
}


template <typename ResultT, typename ArgT,
ResultT (*Func)(ArrayRef<const ArgT *>)>
std::unique_ptr<MatcherDescriptor> makeMatcherAutoMarshall(
ast_matchers::internal::VariadicFunction<ResultT, ArgT, Func> VarFunc,
StringRef MatcherName) {
return std::make_unique<VariadicFuncMatcherDescriptor>(VarFunc, MatcherName);
}





template <typename BaseT, typename DerivedT>
std::unique_ptr<MatcherDescriptor> makeMatcherAutoMarshall(
ast_matchers::internal::VariadicDynCastAllOfMatcher<BaseT, DerivedT>
VarFunc,
StringRef MatcherName) {
return std::make_unique<DynCastAllOfMatcherDescriptor>(VarFunc, MatcherName);
}


template <template <typename ToArg, typename FromArg> class ArgumentAdapterT,
typename FromTypes, typename ToTypes>
std::unique_ptr<MatcherDescriptor> makeMatcherAutoMarshall(
ast_matchers::internal::ArgumentAdaptingMatcherFunc<ArgumentAdapterT,
FromTypes, ToTypes>,
StringRef MatcherName) {
std::vector<std::unique_ptr<MatcherDescriptor>> Overloads;
AdaptativeOverloadCollector<ArgumentAdapterT, FromTypes, ToTypes>(MatcherName,
Overloads);
return std::make_unique<OverloadedMatcherDescriptor>(Overloads);
}

template <template <typename ToArg, typename FromArg> class ArgumentAdapterT,
typename FromTypes, typename ToTypes>
template <typename FromTypeList>
inline void AdaptativeOverloadCollector<ArgumentAdapterT, FromTypes,
ToTypes>::collect(FromTypeList) {
Out.push_back(makeMatcherAutoMarshall(
&AdaptativeFunc::template create<typename FromTypeList::head>, Name));
collect(typename FromTypeList::tail());
}


template <unsigned MinCount, unsigned MaxCount>
std::unique_ptr<MatcherDescriptor> makeMatcherAutoMarshall(
ast_matchers::internal::VariadicOperatorMatcherFunc<MinCount, MaxCount>
Func,
StringRef MatcherName) {
return std::make_unique<VariadicOperatorMatcherDescriptor>(
MinCount, MaxCount, Func.Op, MatcherName);
}

template <typename CladeType, typename... MatcherT>
std::unique_ptr<MatcherDescriptor> makeMatcherAutoMarshall(
ast_matchers::internal::MapAnyOfMatcherImpl<CladeType, MatcherT...>,
StringRef MatcherName) {
return std::make_unique<MapAnyOfMatcherDescriptor>(
ASTNodeKind::getFromNodeKind<CladeType>(),
std::vector<ASTNodeKind>{ASTNodeKind::getFromNodeKind<MatcherT>()...});
}

}
}
}
}

#endif
