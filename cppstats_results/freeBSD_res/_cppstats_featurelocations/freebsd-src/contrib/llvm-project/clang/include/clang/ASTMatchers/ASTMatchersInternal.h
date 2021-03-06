
































#if !defined(LLVM_CLANG_ASTMATCHERS_ASTMATCHERSINTERNAL_H)
#define LLVM_CLANG_ASTMATCHERS_ASTMATCHERSINTERNAL_H

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/OperatorKinds.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Regex.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace clang {

class ASTContext;

namespace ast_matchers {

class BoundNodes;

namespace internal {





template <typename... Ts> struct TypeList {};

template <typename T1, typename... Ts> struct TypeList<T1, Ts...> {

using head = T1;





using tail = TypeList<Ts...>;
};


using EmptyTypeList = TypeList<>;



template <typename AnyTypeList, typename T> struct TypeListContainsSuperOf {
static const bool value =
std::is_base_of<typename AnyTypeList::head, T>::value ||
TypeListContainsSuperOf<typename AnyTypeList::tail, T>::value;
};
template <typename T> struct TypeListContainsSuperOf<EmptyTypeList, T> {
static const bool value = false;
};






template <typename ResultT, typename ArgT,
ResultT (*Func)(ArrayRef<const ArgT *>)>
struct VariadicFunction {
ResultT operator()() const { return Func(None); }

template <typename... ArgsT>
ResultT operator()(const ArgT &Arg1, const ArgsT &... Args) const {
return Execute(Arg1, static_cast<const ArgT &>(Args)...);
}



ResultT operator()(ArrayRef<ArgT> Args) const {
SmallVector<const ArgT*, 8> InnerArgs;
for (const ArgT &Arg : Args)
InnerArgs.push_back(&Arg);
return Func(InnerArgs);
}

private:


template <typename... ArgsT> ResultT Execute(const ArgsT &... Args) const {
const ArgT *const ArgsArray[] = {&Args...};
return Func(ArrayRef<const ArgT *>(ArgsArray, sizeof...(ArgsT)));
}
};



inline QualType getUnderlyingType(const Expr &Node) { return Node.getType(); }

inline QualType getUnderlyingType(const ValueDecl &Node) {
return Node.getType();
}
inline QualType getUnderlyingType(const TypedefNameDecl &Node) {
return Node.getUnderlyingType();
}
inline QualType getUnderlyingType(const FriendDecl &Node) {
if (const TypeSourceInfo *TSI = Node.getFriendType())
return TSI->getType();
return QualType();
}
inline QualType getUnderlyingType(const CXXBaseSpecifier &Node) {
return Node.getType();
}


template <typename T,
std::enable_if_t<TypeListContainsSuperOf<
TypeList<CXXBaseSpecifier, CXXCtorInitializer,
CXXTemporaryObjectExpr, CXXUnresolvedConstructExpr,
CompoundLiteralExpr, DeclaratorDecl, ObjCPropertyDecl,
TemplateArgumentLoc, TypedefNameDecl>,
T>::value> * = nullptr>
inline TypeSourceInfo *GetTypeSourceInfo(const T &Node) {
return Node.getTypeSourceInfo();
}
template <typename T,
std::enable_if_t<TypeListContainsSuperOf<
TypeList<CXXFunctionalCastExpr, ExplicitCastExpr>, T>::value> * =
nullptr>
inline TypeSourceInfo *GetTypeSourceInfo(const T &Node) {
return Node.getTypeInfoAsWritten();
}
inline TypeSourceInfo *GetTypeSourceInfo(const BlockDecl &Node) {
return Node.getSignatureAsWritten();
}
inline TypeSourceInfo *GetTypeSourceInfo(const CXXNewExpr &Node) {
return Node.getAllocatedTypeSourceInfo();
}
inline TypeSourceInfo *
GetTypeSourceInfo(const ClassTemplateSpecializationDecl &Node) {
return Node.getTypeAsWritten();
}



inline const FunctionProtoType *
getFunctionProtoType(const FunctionProtoType &Node) {
return &Node;
}

inline const FunctionProtoType *getFunctionProtoType(const FunctionDecl &Node) {
return Node.getType()->getAs<FunctionProtoType>();
}


inline clang::AccessSpecifier getAccessSpecifier(const Decl &Node) {
return Node.getAccess();
}

inline clang::AccessSpecifier getAccessSpecifier(const CXXBaseSpecifier &Node) {
return Node.getAccessSpecifier();
}


class BoundNodesMap {
public:



void addNode(StringRef ID, const DynTypedNode &DynNode) {
NodeMap[std::string(ID)] = DynNode;
}





template <typename T>
const T *getNodeAs(StringRef ID) const {
IDToNodeMap::const_iterator It = NodeMap.find(ID);
if (It == NodeMap.end()) {
return nullptr;
}
return It->second.get<T>();
}

DynTypedNode getNode(StringRef ID) const {
IDToNodeMap::const_iterator It = NodeMap.find(ID);
if (It == NodeMap.end()) {
return DynTypedNode();
}
return It->second;
}


bool operator<(const BoundNodesMap &Other) const {
return NodeMap < Other.NodeMap;
}






using IDToNodeMap = std::map<std::string, DynTypedNode, std::less<>>;

const IDToNodeMap &getMap() const {
return NodeMap;
}



bool isComparable() const {
for (const auto &IDAndNode : NodeMap) {
if (!IDAndNode.second.getMemoizationData())
return false;
}
return true;
}

private:
IDToNodeMap NodeMap;
};





class BoundNodesTreeBuilder {
public:


class Visitor {
public:
virtual ~Visitor() = default;




virtual void visitMatch(const BoundNodes& BoundNodesView) = 0;
};


void setBinding(StringRef Id, const DynTypedNode &DynNode) {
if (Bindings.empty())
Bindings.emplace_back();
for (BoundNodesMap &Binding : Bindings)
Binding.addNode(Id, DynNode);
}


void addMatch(const BoundNodesTreeBuilder &Bindings);




void visitMatches(Visitor* ResultVisitor);

template <typename ExcludePredicate>
bool removeBindings(const ExcludePredicate &Predicate) {
Bindings.erase(std::remove_if(Bindings.begin(), Bindings.end(), Predicate),
Bindings.end());
return !Bindings.empty();
}


bool operator<(const BoundNodesTreeBuilder &Other) const {
return Bindings < Other.Bindings;
}



bool isComparable() const {
for (const BoundNodesMap &NodesMap : Bindings) {
if (!NodesMap.isComparable())
return false;
}
return true;
}

private:
SmallVector<BoundNodesMap, 1> Bindings;
};

class ASTMatchFinder;






class DynMatcherInterface
: public llvm::ThreadSafeRefCountedBase<DynMatcherInterface> {
public:
virtual ~DynMatcherInterface() = default;





virtual bool dynMatches(const DynTypedNode &DynNode, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const = 0;

virtual llvm::Optional<clang::TraversalKind> TraversalKind() const {
return llvm::None;
}
};








template <typename T>
class MatcherInterface : public DynMatcherInterface {
public:




virtual bool matches(const T &Node,
ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const = 0;

bool dynMatches(const DynTypedNode &DynNode, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return matches(DynNode.getUnchecked<T>(), Finder, Builder);
}
};



template <typename T>
class SingleNodeMatcherInterface : public MatcherInterface<T> {
public:



virtual bool matchesNode(const T &Node) const = 0;

private:

bool matches(const T &Node,
ASTMatchFinder * ,
BoundNodesTreeBuilder * ) const override {
return matchesNode(Node);
}
};

template <typename> class Matcher;








class DynTypedMatcher {
public:

template <typename T>
DynTypedMatcher(MatcherInterface<T> *Implementation)
: SupportedKind(ASTNodeKind::getFromNodeKind<T>()),
RestrictKind(SupportedKind), Implementation(Implementation) {}


enum VariadicOperator {

VO_AllOf,



VO_AnyOf,



VO_EachOf,



VO_Optionally,





VO_UnaryNot
};

static DynTypedMatcher
constructVariadic(VariadicOperator Op, ASTNodeKind SupportedKind,
std::vector<DynTypedMatcher> InnerMatchers);

static DynTypedMatcher
constructRestrictedWrapper(const DynTypedMatcher &InnerMatcher,
ASTNodeKind RestrictKind);




static DynTypedMatcher trueMatcher(ASTNodeKind NodeKind);

void setAllowBind(bool AB) { AllowBind = AB; }




bool canMatchNodesOfKind(ASTNodeKind Kind) const;



DynTypedMatcher dynCastTo(const ASTNodeKind Kind) const;





DynTypedMatcher withTraversalKind(TraversalKind TK);


bool matches(const DynTypedNode &DynNode, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const;





bool matchesNoKindCheck(const DynTypedNode &DynNode, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const;




llvm::Optional<DynTypedMatcher> tryBind(StringRef ID) const;








using MatcherIDType = std::pair<ASTNodeKind, uint64_t>;
MatcherIDType getID() const {


return std::make_pair(RestrictKind,
reinterpret_cast<uint64_t>(Implementation.get()));
}





ASTNodeKind getSupportedKind() const { return SupportedKind; }






template <typename T> bool canConvertTo() const {
return canConvertTo(ASTNodeKind::getFromNodeKind<T>());
}
bool canConvertTo(ASTNodeKind To) const;






template <typename T> Matcher<T> convertTo() const {
assert(canConvertTo<T>());
return unconditionalConvertTo<T>();
}





template <typename T> Matcher<T> unconditionalConvertTo() const;





llvm::Optional<clang::TraversalKind> getTraversalKind() const {
return Implementation->TraversalKind();
}

private:
DynTypedMatcher(ASTNodeKind SupportedKind, ASTNodeKind RestrictKind,
IntrusiveRefCntPtr<DynMatcherInterface> Implementation)
: SupportedKind(SupportedKind), RestrictKind(RestrictKind),
Implementation(std::move(Implementation)) {}

bool AllowBind = false;
ASTNodeKind SupportedKind;





ASTNodeKind RestrictKind;
IntrusiveRefCntPtr<DynMatcherInterface> Implementation;
};









template <typename T>
class Matcher {
public:

explicit Matcher(MatcherInterface<T> *Implementation)
: Implementation(Implementation) {}




template <typename From>
Matcher(const Matcher<From> &Other,
std::enable_if_t<std::is_base_of<From, T>::value &&
!std::is_same<From, T>::value> * = nullptr)
: Implementation(restrictMatcher(Other.Implementation)) {
assert(Implementation.getSupportedKind().isSame(
ASTNodeKind::getFromNodeKind<T>()));
}




template <typename TypeT>
Matcher(const Matcher<TypeT> &Other,
std::enable_if_t<std::is_same<T, QualType>::value &&
std::is_same<TypeT, Type>::value> * = nullptr)
: Implementation(new TypeToQualType<TypeT>(Other)) {}




template <typename To> Matcher<To> dynCastTo() const LLVM_LVALUE_FUNCTION {
static_assert(std::is_base_of<To, T>::value, "Invalid dynCast call.");
return Matcher<To>(Implementation);
}

#if LLVM_HAS_RVALUE_REFERENCE_THIS
template <typename To> Matcher<To> dynCastTo() && {
static_assert(std::is_base_of<To, T>::value, "Invalid dynCast call.");
return Matcher<To>(std::move(Implementation));
}
#endif


bool matches(const T &Node,
ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return Implementation.matches(DynTypedNode::create(Node), Finder, Builder);
}


DynTypedMatcher::MatcherIDType getID() const {
return Implementation.getID();
}





operator DynTypedMatcher() const LLVM_LVALUE_FUNCTION {
return Implementation;
}

#if LLVM_HAS_RVALUE_REFERENCE_THIS
operator DynTypedMatcher() && { return std::move(Implementation); }
#endif







template <typename TypeT>
class TypeToQualType : public MatcherInterface<QualType> {
const DynTypedMatcher InnerMatcher;

public:
TypeToQualType(const Matcher<TypeT> &InnerMatcher)
: InnerMatcher(InnerMatcher) {}

bool matches(const QualType &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
if (Node.isNull())
return false;
return this->InnerMatcher.matches(DynTypedNode::create(*Node), Finder,
Builder);
}

llvm::Optional<clang::TraversalKind> TraversalKind() const override {
return this->InnerMatcher.getTraversalKind();
}
};

private:

template <typename U> friend class Matcher;


friend class DynTypedMatcher;

static DynTypedMatcher restrictMatcher(const DynTypedMatcher &Other) {
return Other.dynCastTo(ASTNodeKind::getFromNodeKind<T>());
}

explicit Matcher(const DynTypedMatcher &Implementation)
: Implementation(restrictMatcher(Implementation)) {
assert(this->Implementation.getSupportedKind().isSame(
ASTNodeKind::getFromNodeKind<T>()));
}

DynTypedMatcher Implementation;
};



template <typename T>
inline Matcher<T> makeMatcher(MatcherInterface<T> *Implementation) {
return Matcher<T>(Implementation);
}



















class ASTMatchFinder {
public:

enum BindKind {

BK_First,


BK_All
};


enum AncestorMatchMode {

AMM_All,


AMM_ParentOnly
};

virtual ~ASTMatchFinder() = default;





virtual bool classIsDerivedFrom(const CXXRecordDecl *Declaration,
const Matcher<NamedDecl> &Base,
BoundNodesTreeBuilder *Builder,
bool Directly) = 0;





virtual bool objcClassIsDerivedFrom(const ObjCInterfaceDecl *Declaration,
const Matcher<NamedDecl> &Base,
BoundNodesTreeBuilder *Builder,
bool Directly) = 0;

template <typename T>
bool matchesChildOf(const T &Node, const DynTypedMatcher &Matcher,
BoundNodesTreeBuilder *Builder, BindKind Bind) {
static_assert(std::is_base_of<Decl, T>::value ||
std::is_base_of<Stmt, T>::value ||
std::is_base_of<NestedNameSpecifier, T>::value ||
std::is_base_of<NestedNameSpecifierLoc, T>::value ||
std::is_base_of<TypeLoc, T>::value ||
std::is_base_of<QualType, T>::value,
"unsupported type for recursive matching");
return matchesChildOf(DynTypedNode::create(Node), getASTContext(), Matcher,
Builder, Bind);
}

template <typename T>
bool matchesDescendantOf(const T &Node, const DynTypedMatcher &Matcher,
BoundNodesTreeBuilder *Builder, BindKind Bind) {
static_assert(std::is_base_of<Decl, T>::value ||
std::is_base_of<Stmt, T>::value ||
std::is_base_of<NestedNameSpecifier, T>::value ||
std::is_base_of<NestedNameSpecifierLoc, T>::value ||
std::is_base_of<TypeLoc, T>::value ||
std::is_base_of<QualType, T>::value,
"unsupported type for recursive matching");
return matchesDescendantOf(DynTypedNode::create(Node), getASTContext(),
Matcher, Builder, Bind);
}


template <typename T>
bool matchesAncestorOf(const T &Node, const DynTypedMatcher &Matcher,
BoundNodesTreeBuilder *Builder,
AncestorMatchMode MatchMode) {
static_assert(std::is_base_of<Decl, T>::value ||
std::is_base_of<NestedNameSpecifierLoc, T>::value ||
std::is_base_of<Stmt, T>::value ||
std::is_base_of<TypeLoc, T>::value,
"type not allowed for recursive matching");
return matchesAncestorOf(DynTypedNode::create(Node), getASTContext(),
Matcher, Builder, MatchMode);
}

virtual ASTContext &getASTContext() const = 0;

virtual bool IsMatchingInASTNodeNotSpelledInSource() const = 0;

virtual bool IsMatchingInASTNodeNotAsIs() const = 0;

bool isTraversalIgnoringImplicitNodes() const;

protected:
virtual bool matchesChildOf(const DynTypedNode &Node, ASTContext &Ctx,
const DynTypedMatcher &Matcher,
BoundNodesTreeBuilder *Builder,
BindKind Bind) = 0;

virtual bool matchesDescendantOf(const DynTypedNode &Node, ASTContext &Ctx,
const DynTypedMatcher &Matcher,
BoundNodesTreeBuilder *Builder,
BindKind Bind) = 0;

virtual bool matchesAncestorOf(const DynTypedNode &Node, ASTContext &Ctx,
const DynTypedMatcher &Matcher,
BoundNodesTreeBuilder *Builder,
AncestorMatchMode MatchMode) = 0;
private:
friend struct ASTChildrenNotSpelledInSourceScope;
virtual bool isMatchingChildrenNotSpelledInSource() const = 0;
virtual void setMatchingChildrenNotSpelledInSource(bool Set) = 0;
};

struct ASTChildrenNotSpelledInSourceScope {
ASTChildrenNotSpelledInSourceScope(ASTMatchFinder *V, bool B)
: MV(V), MB(V->isMatchingChildrenNotSpelledInSource()) {
V->setMatchingChildrenNotSpelledInSource(B);
}
~ASTChildrenNotSpelledInSourceScope() {
MV->setMatchingChildrenNotSpelledInSource(MB);
}

private:
ASTMatchFinder *MV;
bool MB;
};





template <>
inline Matcher<QualType> DynTypedMatcher::convertTo<QualType>() const {
assert(canConvertTo<QualType>());
const ASTNodeKind SourceKind = getSupportedKind();
if (SourceKind.isSame(ASTNodeKind::getFromNodeKind<Type>())) {

return unconditionalConvertTo<Type>();
}
return unconditionalConvertTo<QualType>();
}


template <typename MatcherT, typename IteratorT>
IteratorT matchesFirstInRange(const MatcherT &Matcher, IteratorT Start,
IteratorT End, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) {
for (IteratorT I = Start; I != End; ++I) {
BoundNodesTreeBuilder Result(*Builder);
if (Matcher.matches(*I, Finder, &Result)) {
*Builder = std::move(Result);
return I;
}
}
return End;
}



template <typename MatcherT, typename IteratorT>
IteratorT matchesFirstInPointerRange(const MatcherT &Matcher, IteratorT Start,
IteratorT End, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) {
for (IteratorT I = Start; I != End; ++I) {
BoundNodesTreeBuilder Result(*Builder);
if (Matcher.matches(**I, Finder, &Result)) {
*Builder = std::move(Result);
return I;
}
}
return End;
}

template <typename T, std::enable_if_t<!std::is_base_of<FunctionDecl, T>::value>
* = nullptr>
inline bool isDefaultedHelper(const T *) {
return false;
}
inline bool isDefaultedHelper(const FunctionDecl *FD) {
return FD->isDefaulted();
}


template <typename Ty>
class has_getDecl {
using yes = char[1];
using no = char[2];

template <typename Inner>
static yes& test(Inner *I, decltype(I->getDecl()) * = nullptr);

template <typename>
static no& test(...);

public:
static const bool value = sizeof(test<Ty>(nullptr)) == sizeof(yes);
};





template <typename T, typename ArgT>
class HasOverloadedOperatorNameMatcher : public SingleNodeMatcherInterface<T> {
static_assert(std::is_same<T, CXXOperatorCallExpr>::value ||
std::is_base_of<FunctionDecl, T>::value,
"unsupported class for matcher");
static_assert(std::is_same<ArgT, std::vector<std::string>>::value,
"argument type must be std::vector<std::string>");

public:
explicit HasOverloadedOperatorNameMatcher(std::vector<std::string> Names)
: SingleNodeMatcherInterface<T>(), Names(std::move(Names)) {}

bool matchesNode(const T &Node) const override {
return matchesSpecialized(Node);
}

private:




bool matchesSpecialized(const CXXOperatorCallExpr &Node) const {
return llvm::is_contained(Names, getOperatorSpelling(Node.getOperator()));
}



bool matchesSpecialized(const FunctionDecl &Node) const {
return Node.isOverloadedOperator() &&
llvm::is_contained(
Names, getOperatorSpelling(Node.getOverloadedOperator()));
}

std::vector<std::string> Names;
};




class HasNameMatcher : public SingleNodeMatcherInterface<NamedDecl> {
public:
explicit HasNameMatcher(std::vector<std::string> Names);

bool matchesNode(const NamedDecl &Node) const override;

private:




bool matchesNodeUnqualified(const NamedDecl &Node) const;







bool matchesNodeFullFast(const NamedDecl &Node) const;






bool matchesNodeFullSlow(const NamedDecl &Node) const;

bool UseUnqualifiedMatch;
std::vector<std::string> Names;
};



Matcher<NamedDecl> hasAnyNameFunc(ArrayRef<const StringRef *> NameRefs);



Matcher<ObjCMessageExpr> hasAnySelectorFunc(
ArrayRef<const StringRef *> NameRefs);





template <typename T, typename DeclMatcherT>
class HasDeclarationMatcher : public MatcherInterface<T> {
static_assert(std::is_same<DeclMatcherT, Matcher<Decl>>::value,
"instantiated with wrong types");

DynTypedMatcher InnerMatcher;

public:
explicit HasDeclarationMatcher(const Matcher<Decl> &InnerMatcher)
: InnerMatcher(InnerMatcher) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return matchesSpecialized(Node, Finder, Builder);
}

private:

bool matchesSpecialized(const QualType &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
if (Node.isNull())
return false;

return matchesSpecialized(*Node, Finder, Builder);
}



bool matchesSpecialized(const Type &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {


const Type *EffectiveType = &Node;
if (const auto *S = dyn_cast<DeducedType>(&Node)) {
EffectiveType = S->getDeducedType().getTypePtrOrNull();
if (!EffectiveType)
return false;
}



if (const auto *S = dyn_cast<TagType>(EffectiveType)) {
return matchesDecl(S->getDecl(), Finder, Builder);
}
if (const auto *S = dyn_cast<InjectedClassNameType>(EffectiveType)) {
return matchesDecl(S->getDecl(), Finder, Builder);
}
if (const auto *S = dyn_cast<TemplateTypeParmType>(EffectiveType)) {
return matchesDecl(S->getDecl(), Finder, Builder);
}
if (const auto *S = dyn_cast<TypedefType>(EffectiveType)) {
return matchesDecl(S->getDecl(), Finder, Builder);
}
if (const auto *S = dyn_cast<UnresolvedUsingType>(EffectiveType)) {
return matchesDecl(S->getDecl(), Finder, Builder);
}
if (const auto *S = dyn_cast<ObjCObjectType>(EffectiveType)) {
return matchesDecl(S->getInterface(), Finder, Builder);
}









if (const auto *S = dyn_cast<SubstTemplateTypeParmType>(EffectiveType)) {
return matchesSpecialized(S->getReplacementType(), Finder, Builder);
}




if (const auto *S = dyn_cast<TemplateSpecializationType>(EffectiveType)) {
if (!S->isTypeAlias() && S->isSugared()) {






return matchesSpecialized(*S->desugar(), Finder, Builder);
}


return matchesDecl(S->getTemplateName().getAsTemplateDecl(), Finder,
Builder);
}




if (const auto *S = dyn_cast<ElaboratedType>(EffectiveType)) {
return matchesSpecialized(S->desugar(), Finder, Builder);
}
return false;
}



bool matchesSpecialized(const DeclRefExpr &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getDecl(), Finder, Builder);
}



bool matchesSpecialized(const CallExpr &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getCalleeDecl(), Finder, Builder);
}



bool matchesSpecialized(const CXXConstructExpr &Node,
ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getConstructor(), Finder, Builder);
}

bool matchesSpecialized(const ObjCIvarRefExpr &Node,
ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getDecl(), Finder, Builder);
}



bool matchesSpecialized(const CXXNewExpr &Node,
ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getOperatorNew(), Finder, Builder);
}



bool matchesSpecialized(const MemberExpr &Node,
ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getMemberDecl(), Finder, Builder);
}



bool matchesSpecialized(const AddrLabelExpr &Node,
ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getLabel(), Finder, Builder);
}



bool matchesSpecialized(const LabelStmt &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return matchesDecl(Node.getDecl(), Finder, Builder);
}



bool matchesDecl(const Decl *Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const {
return Node != nullptr &&
!(Finder->isTraversalIgnoringImplicitNodes() &&
Node->isImplicit()) &&
this->InnerMatcher.matches(DynTypedNode::create(*Node), Finder,
Builder);
}
};



template <typename T>
struct IsBaseType {
static const bool value =
std::is_same<T, Decl>::value || std::is_same<T, Stmt>::value ||
std::is_same<T, QualType>::value || std::is_same<T, Type>::value ||
std::is_same<T, TypeLoc>::value ||
std::is_same<T, NestedNameSpecifier>::value ||
std::is_same<T, NestedNameSpecifierLoc>::value ||
std::is_same<T, CXXCtorInitializer>::value ||
std::is_same<T, TemplateArgumentLoc>::value;
};
template <typename T>
const bool IsBaseType<T>::value;




using AllNodeBaseTypes =
TypeList<Decl, Stmt, NestedNameSpecifier, NestedNameSpecifierLoc, QualType,
Type, TypeLoc, CXXCtorInitializer>;





template <class T> struct ExtractFunctionArgMeta;
template <class T> struct ExtractFunctionArgMeta<void(T)> {
using type = T;
};

template <class T, class Tuple, std::size_t... I>
constexpr T *new_from_tuple_impl(Tuple &&t, std::index_sequence<I...>) {
return new T(std::get<I>(std::forward<Tuple>(t))...);
}

template <class T, class Tuple> constexpr T *new_from_tuple(Tuple &&t) {
return new_from_tuple_impl<T>(
std::forward<Tuple>(t),
std::make_index_sequence<
std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}


using AdaptativeDefaultFromTypes = AllNodeBaseTypes;
using AdaptativeDefaultToTypes =
TypeList<Decl, Stmt, NestedNameSpecifier, NestedNameSpecifierLoc, TypeLoc,
QualType>;


using HasDeclarationSupportedTypes =
TypeList<CallExpr, CXXConstructExpr, CXXNewExpr, DeclRefExpr, EnumType,
ElaboratedType, InjectedClassNameType, LabelStmt, AddrLabelExpr,
MemberExpr, QualType, RecordType, TagType,
TemplateSpecializationType, TemplateTypeParmType, TypedefType,
UnresolvedUsingType, ObjCIvarRefExpr>;





template <typename T> class BindableMatcher : public Matcher<T> {
public:
explicit BindableMatcher(const Matcher<T> &M) : Matcher<T>(M) {}
explicit BindableMatcher(MatcherInterface<T> *Implementation)
: Matcher<T>(Implementation) {}





Matcher<T> bind(StringRef ID) const {
return DynTypedMatcher(*this)
.tryBind(ID)
->template unconditionalConvertTo<T>();
}



operator DynTypedMatcher() const {
DynTypedMatcher Result = static_cast<const Matcher<T> &>(*this);
Result.setAllowBind(true);
return Result;
}
};





class TrueMatcher {
public:
using ReturnTypes = AllNodeBaseTypes;

template <typename T> operator Matcher<T>() const {
return DynTypedMatcher::trueMatcher(ASTNodeKind::getFromNodeKind<T>())
.template unconditionalConvertTo<T>();
}
};


template <typename T>
BindableMatcher<T>
makeAllOfComposite(ArrayRef<const Matcher<T> *> InnerMatchers) {

if (InnerMatchers.empty()) {
return BindableMatcher<T>(TrueMatcher());
}


if (InnerMatchers.size() == 1) {
return BindableMatcher<T>(*InnerMatchers[0]);
}

using PI = llvm::pointee_iterator<const Matcher<T> *const *>;

std::vector<DynTypedMatcher> DynMatchers(PI(InnerMatchers.begin()),
PI(InnerMatchers.end()));
return BindableMatcher<T>(
DynTypedMatcher::constructVariadic(DynTypedMatcher::VO_AllOf,
ASTNodeKind::getFromNodeKind<T>(),
std::move(DynMatchers))
.template unconditionalConvertTo<T>());
}







template <typename T, typename InnerT>
BindableMatcher<T>
makeDynCastAllOfComposite(ArrayRef<const Matcher<InnerT> *> InnerMatchers) {
return BindableMatcher<T>(
makeAllOfComposite(InnerMatchers).template dynCastTo<T>());
}












template <typename SourceT, typename TargetT>
class VariadicDynCastAllOfMatcher
: public VariadicFunction<BindableMatcher<SourceT>, Matcher<TargetT>,
makeDynCastAllOfComposite<SourceT, TargetT>> {
public:
VariadicDynCastAllOfMatcher() {}
};











template <typename T>
class VariadicAllOfMatcher
: public VariadicFunction<BindableMatcher<T>, Matcher<T>,
makeAllOfComposite<T>> {
public:
VariadicAllOfMatcher() {}
};










template <typename... Ps> class VariadicOperatorMatcher {
public:
VariadicOperatorMatcher(DynTypedMatcher::VariadicOperator Op, Ps &&... Params)
: Op(Op), Params(std::forward<Ps>(Params)...) {}

template <typename T> operator Matcher<T>() const LLVM_LVALUE_FUNCTION {
return DynTypedMatcher::constructVariadic(
Op, ASTNodeKind::getFromNodeKind<T>(),
getMatchers<T>(std::index_sequence_for<Ps...>()))
.template unconditionalConvertTo<T>();
}

#if LLVM_HAS_RVALUE_REFERENCE_THIS
template <typename T> operator Matcher<T>() && {
return DynTypedMatcher::constructVariadic(
Op, ASTNodeKind::getFromNodeKind<T>(),
getMatchers<T>(std::index_sequence_for<Ps...>()))
.template unconditionalConvertTo<T>();
}
#endif
private:

template <typename T, std::size_t... Is>
std::vector<DynTypedMatcher>
getMatchers(std::index_sequence<Is...>) const LLVM_LVALUE_FUNCTION {
return {Matcher<T>(std::get<Is>(Params))...};
}

#if LLVM_HAS_RVALUE_REFERENCE_THIS
template <typename T, std::size_t... Is>
std::vector<DynTypedMatcher> getMatchers(std::index_sequence<Is...>) && {
return {Matcher<T>(std::get<Is>(std::move(Params)))...};
}
#endif

const DynTypedMatcher::VariadicOperator Op;
std::tuple<Ps...> Params;
};



template <unsigned MinCount, unsigned MaxCount>
struct VariadicOperatorMatcherFunc {
DynTypedMatcher::VariadicOperator Op;

template <typename... Ms>
VariadicOperatorMatcher<Ms...> operator()(Ms &&... Ps) const {
static_assert(MinCount <= sizeof...(Ms) && sizeof...(Ms) <= MaxCount,
"invalid number of parameters for variadic matcher");
return VariadicOperatorMatcher<Ms...>(Op, std::forward<Ms>(Ps)...);
}
};

template <typename F, typename Tuple, std::size_t... I>
constexpr auto applyMatcherImpl(F &&f, Tuple &&args,
std::index_sequence<I...>) {
return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(args))...);
}

template <typename F, typename Tuple>
constexpr auto applyMatcher(F &&f, Tuple &&args) {
return applyMatcherImpl(
std::forward<F>(f), std::forward<Tuple>(args),
std::make_index_sequence<
std::tuple_size<typename std::decay<Tuple>::type>::value>());
}

template <typename T, bool IsBaseOf, typename Head, typename Tail>
struct GetCladeImpl {
using Type = Head;
};
template <typename T, typename Head, typename Tail>
struct GetCladeImpl<T, false, Head, Tail>
: GetCladeImpl<T, std::is_base_of<typename Tail::head, T>::value,
typename Tail::head, typename Tail::tail> {};

template <typename T, typename... U>
struct GetClade : GetCladeImpl<T, false, T, AllNodeBaseTypes> {};

template <typename CladeType, typename... MatcherTypes>
struct MapAnyOfMatcherImpl {

template <typename... InnerMatchers>
BindableMatcher<CladeType>
operator()(InnerMatchers &&... InnerMatcher) const {

return VariadicAllOfMatcher<CladeType>()(applyMatcher(
internal::VariadicOperatorMatcherFunc<
0, std::numeric_limits<unsigned>::max()>{
internal::DynTypedMatcher::VO_AnyOf},
applyMatcher(
[&](auto... Matcher) {
return std::make_tuple(Matcher(InnerMatcher...)...);
},
std::tuple<
VariadicDynCastAllOfMatcher<CladeType, MatcherTypes>...>())));
}
};

template <typename... MatcherTypes>
using MapAnyOfMatcher =
MapAnyOfMatcherImpl<typename GetClade<MatcherTypes...>::Type,
MatcherTypes...>;

template <typename... MatcherTypes> struct MapAnyOfHelper {
using CladeType = typename GetClade<MatcherTypes...>::Type;

MapAnyOfMatcher<MatcherTypes...> with;

operator BindableMatcher<CladeType>() const { return with(); }

Matcher<CladeType> bind(StringRef ID) const { return with().bind(ID); }
};

template <template <typename ToArg, typename FromArg> class ArgumentAdapterT,
typename T, typename ToTypes>
class ArgumentAdaptingMatcherFuncAdaptor {
public:
explicit ArgumentAdaptingMatcherFuncAdaptor(const Matcher<T> &InnerMatcher)
: InnerMatcher(InnerMatcher) {}

using ReturnTypes = ToTypes;

template <typename To> operator Matcher<To>() const LLVM_LVALUE_FUNCTION {
return Matcher<To>(new ArgumentAdapterT<To, T>(InnerMatcher));
}

#if LLVM_HAS_RVALUE_REFERENCE_THIS
template <typename To> operator Matcher<To>() && {
return Matcher<To>(new ArgumentAdapterT<To, T>(std::move(InnerMatcher)));
}
#endif

private:
Matcher<T> InnerMatcher;
};














template <template <typename ToArg, typename FromArg> class ArgumentAdapterT,
typename FromTypes = AdaptativeDefaultFromTypes,
typename ToTypes = AdaptativeDefaultToTypes>
struct ArgumentAdaptingMatcherFunc {
template <typename T>
static ArgumentAdaptingMatcherFuncAdaptor<ArgumentAdapterT, T, ToTypes>
create(const Matcher<T> &InnerMatcher) {
return ArgumentAdaptingMatcherFuncAdaptor<ArgumentAdapterT, T, ToTypes>(
InnerMatcher);
}

template <typename T>
ArgumentAdaptingMatcherFuncAdaptor<ArgumentAdapterT, T, ToTypes>
operator()(const Matcher<T> &InnerMatcher) const {
return create(InnerMatcher);
}

template <typename... T>
ArgumentAdaptingMatcherFuncAdaptor<ArgumentAdapterT,
typename GetClade<T...>::Type, ToTypes>
operator()(const MapAnyOfHelper<T...> &InnerMatcher) const {
return create(InnerMatcher.with());
}
};

template <typename T> class TraversalMatcher : public MatcherInterface<T> {
DynTypedMatcher InnerMatcher;
clang::TraversalKind Traversal;

public:
explicit TraversalMatcher(clang::TraversalKind TK,
const Matcher<T> &InnerMatcher)
: InnerMatcher(InnerMatcher), Traversal(TK) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return this->InnerMatcher.matches(DynTypedNode::create(Node), Finder,
Builder);
}

llvm::Optional<clang::TraversalKind> TraversalKind() const override {
if (auto NestedKind = this->InnerMatcher.getTraversalKind())
return NestedKind;
return Traversal;
}
};

template <typename MatcherType> class TraversalWrapper {
public:
TraversalWrapper(TraversalKind TK, const MatcherType &InnerMatcher)
: TK(TK), InnerMatcher(InnerMatcher) {}

template <typename T> operator Matcher<T>() const LLVM_LVALUE_FUNCTION {
return internal::DynTypedMatcher::constructRestrictedWrapper(
new internal::TraversalMatcher<T>(TK, InnerMatcher),
ASTNodeKind::getFromNodeKind<T>())
.template unconditionalConvertTo<T>();
}

#if LLVM_HAS_RVALUE_REFERENCE_THIS
template <typename T> operator Matcher<T>() && {
return internal::DynTypedMatcher::constructRestrictedWrapper(
new internal::TraversalMatcher<T>(TK, std::move(InnerMatcher)),
ASTNodeKind::getFromNodeKind<T>())
.template unconditionalConvertTo<T>();
}
#endif

private:
TraversalKind TK;
MatcherType InnerMatcher;
};













template <template <typename T, typename... Params> class MatcherT,
typename ReturnTypesF, typename... ParamTypes>
class PolymorphicMatcher {
public:
PolymorphicMatcher(const ParamTypes &... Params) : Params(Params...) {}

using ReturnTypes = typename ExtractFunctionArgMeta<ReturnTypesF>::type;

template <typename T> operator Matcher<T>() const LLVM_LVALUE_FUNCTION {
static_assert(TypeListContainsSuperOf<ReturnTypes, T>::value,
"right polymorphic conversion");
return Matcher<T>(new_from_tuple<MatcherT<T, ParamTypes...>>(Params));
}

#if LLVM_HAS_RVALUE_REFERENCE_THIS
template <typename T> operator Matcher<T>() && {
static_assert(TypeListContainsSuperOf<ReturnTypes, T>::value,
"right polymorphic conversion");
return Matcher<T>(
new_from_tuple<MatcherT<T, ParamTypes...>>(std::move(Params)));
}
#endif

private:
std::tuple<ParamTypes...> Params;
};





template <typename T, typename ChildT>
class HasMatcher : public MatcherInterface<T> {
DynTypedMatcher InnerMatcher;

public:
explicit HasMatcher(const Matcher<ChildT> &InnerMatcher)
: InnerMatcher(InnerMatcher) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return Finder->matchesChildOf(Node, this->InnerMatcher, Builder,
ASTMatchFinder::BK_First);
}
};






template <typename T, typename ChildT>
class ForEachMatcher : public MatcherInterface<T> {
static_assert(IsBaseType<ChildT>::value,
"for each only accepts base type matcher");

DynTypedMatcher InnerMatcher;

public:
explicit ForEachMatcher(const Matcher<ChildT> &InnerMatcher)
: InnerMatcher(InnerMatcher) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return Finder->matchesChildOf(
Node, this->InnerMatcher, Builder,
ASTMatchFinder::BK_All);
}
};



template <typename T>
inline Matcher<T> DynTypedMatcher::unconditionalConvertTo() const {
return Matcher<T>(*this);
}





template <typename T, typename DescendantT>
class HasDescendantMatcher : public MatcherInterface<T> {
static_assert(IsBaseType<DescendantT>::value,
"has descendant only accepts base type matcher");

DynTypedMatcher DescendantMatcher;

public:
explicit HasDescendantMatcher(const Matcher<DescendantT> &DescendantMatcher)
: DescendantMatcher(DescendantMatcher) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return Finder->matchesDescendantOf(Node, this->DescendantMatcher, Builder,
ASTMatchFinder::BK_First);
}
};





template <typename T, typename ParentT>
class HasParentMatcher : public MatcherInterface<T> {
static_assert(IsBaseType<ParentT>::value,
"has parent only accepts base type matcher");

DynTypedMatcher ParentMatcher;

public:
explicit HasParentMatcher(const Matcher<ParentT> &ParentMatcher)
: ParentMatcher(ParentMatcher) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return Finder->matchesAncestorOf(Node, this->ParentMatcher, Builder,
ASTMatchFinder::AMM_ParentOnly);
}
};





template <typename T, typename AncestorT>
class HasAncestorMatcher : public MatcherInterface<T> {
static_assert(IsBaseType<AncestorT>::value,
"has ancestor only accepts base type matcher");

DynTypedMatcher AncestorMatcher;

public:
explicit HasAncestorMatcher(const Matcher<AncestorT> &AncestorMatcher)
: AncestorMatcher(AncestorMatcher) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return Finder->matchesAncestorOf(Node, this->AncestorMatcher, Builder,
ASTMatchFinder::AMM_All);
}
};







template <typename T, typename DescendantT>
class ForEachDescendantMatcher : public MatcherInterface<T> {
static_assert(IsBaseType<DescendantT>::value,
"for each descendant only accepts base type matcher");

DynTypedMatcher DescendantMatcher;

public:
explicit ForEachDescendantMatcher(
const Matcher<DescendantT> &DescendantMatcher)
: DescendantMatcher(DescendantMatcher) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
return Finder->matchesDescendantOf(Node, this->DescendantMatcher, Builder,
ASTMatchFinder::BK_All);
}
};



template <typename T, typename ValueT>
class ValueEqualsMatcher : public SingleNodeMatcherInterface<T> {
static_assert(std::is_base_of<CharacterLiteral, T>::value ||
std::is_base_of<CXXBoolLiteralExpr, T>::value ||
std::is_base_of<FloatingLiteral, T>::value ||
std::is_base_of<IntegerLiteral, T>::value,
"the node must have a getValue method");

public:
explicit ValueEqualsMatcher(const ValueT &ExpectedValue)
: ExpectedValue(ExpectedValue) {}

bool matchesNode(const T &Node) const override {
return Node.getValue() == ExpectedValue;
}

private:
ValueT ExpectedValue;
};



template <>
inline bool ValueEqualsMatcher<FloatingLiteral, double>::matchesNode(
const FloatingLiteral &Node) const {
if ((&Node.getSemantics()) == &llvm::APFloat::IEEEsingle())
return Node.getValue().convertToFloat() == ExpectedValue;
if ((&Node.getSemantics()) == &llvm::APFloat::IEEEdouble())
return Node.getValue().convertToDouble() == ExpectedValue;
return false;
}
template <>
inline bool ValueEqualsMatcher<FloatingLiteral, float>::matchesNode(
const FloatingLiteral &Node) const {
if ((&Node.getSemantics()) == &llvm::APFloat::IEEEsingle())
return Node.getValue().convertToFloat() == ExpectedValue;
if ((&Node.getSemantics()) == &llvm::APFloat::IEEEdouble())
return Node.getValue().convertToDouble() == ExpectedValue;
return false;
}
template <>
inline bool ValueEqualsMatcher<FloatingLiteral, llvm::APFloat>::matchesNode(
const FloatingLiteral &Node) const {
return ExpectedValue.compare(Node.getValue()) == llvm::APFloat::cmpEqual;
}



template <typename TLoc, typename T>
class LocMatcher : public MatcherInterface<TLoc> {
DynTypedMatcher InnerMatcher;

public:
explicit LocMatcher(const Matcher<T> &InnerMatcher)
: InnerMatcher(InnerMatcher) {}

bool matches(const TLoc &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
if (!Node)
return false;
return this->InnerMatcher.matches(extract(Node), Finder, Builder);
}

private:
static DynTypedNode extract(const NestedNameSpecifierLoc &Loc) {
return DynTypedNode::create(*Loc.getNestedNameSpecifier());
}
};





class TypeLocTypeMatcher : public MatcherInterface<TypeLoc> {
DynTypedMatcher InnerMatcher;

public:
explicit TypeLocTypeMatcher(const Matcher<QualType> &InnerMatcher)
: InnerMatcher(InnerMatcher) {}

bool matches(const TypeLoc &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
if (!Node)
return false;
return this->InnerMatcher.matches(DynTypedNode::create(Node.getType()),
Finder, Builder);
}
};




template <typename T> class TypeTraverseMatcher : public MatcherInterface<T> {
DynTypedMatcher InnerMatcher;

public:
explicit TypeTraverseMatcher(const Matcher<QualType> &InnerMatcher,
QualType (T::*TraverseFunction)() const)
: InnerMatcher(InnerMatcher), TraverseFunction(TraverseFunction) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
QualType NextNode = (Node.*TraverseFunction)();
if (NextNode.isNull())
return false;
return this->InnerMatcher.matches(DynTypedNode::create(NextNode), Finder,
Builder);
}

private:
QualType (T::*TraverseFunction)() const;
};




template <typename T>
class TypeLocTraverseMatcher : public MatcherInterface<T> {
DynTypedMatcher InnerMatcher;

public:
explicit TypeLocTraverseMatcher(const Matcher<TypeLoc> &InnerMatcher,
TypeLoc (T::*TraverseFunction)() const)
: InnerMatcher(InnerMatcher), TraverseFunction(TraverseFunction) {}

bool matches(const T &Node, ASTMatchFinder *Finder,
BoundNodesTreeBuilder *Builder) const override {
TypeLoc NextNode = (Node.*TraverseFunction)();
if (!NextNode)
return false;
return this->InnerMatcher.matches(DynTypedNode::create(NextNode), Finder,
Builder);
}

private:
TypeLoc (T::*TraverseFunction)() const;
};







template <typename InnerTBase,
template <typename OuterT> class Getter,
template <typename OuterT> class MatcherImpl,
typename ReturnTypesF>
class TypeTraversePolymorphicMatcher {
private:
using Self = TypeTraversePolymorphicMatcher<InnerTBase, Getter, MatcherImpl,
ReturnTypesF>;

static Self create(ArrayRef<const Matcher<InnerTBase> *> InnerMatchers);

public:
using ReturnTypes = typename ExtractFunctionArgMeta<ReturnTypesF>::type;

explicit TypeTraversePolymorphicMatcher(
ArrayRef<const Matcher<InnerTBase> *> InnerMatchers)
: InnerMatcher(makeAllOfComposite(InnerMatchers)) {}

template <typename OuterT> operator Matcher<OuterT>() const {
return Matcher<OuterT>(
new MatcherImpl<OuterT>(InnerMatcher, Getter<OuterT>::value()));
}

struct Func
: public VariadicFunction<Self, Matcher<InnerTBase>, &Self::create> {
Func() {}
};

private:
Matcher<InnerTBase> InnerMatcher;
};





template <typename Matcher, Matcher (*Func)()> class MemoizedMatcher {
struct Wrapper {
Wrapper() : M(Func()) {}

Matcher M;
};

public:
static const Matcher &getInstance() {
static llvm::ManagedStatic<Wrapper> Instance;
return Instance->M;
}
};




template <typename InnerTBase, template <typename OuterT> class Getter,
template <typename OuterT> class MatcherImpl, typename ReturnTypesF>
TypeTraversePolymorphicMatcher<InnerTBase, Getter, MatcherImpl, ReturnTypesF>
TypeTraversePolymorphicMatcher<
InnerTBase, Getter, MatcherImpl,
ReturnTypesF>::create(ArrayRef<const Matcher<InnerTBase> *> InnerMatchers) {
return Self(InnerMatchers);
}



inline ArrayRef<TemplateArgument>
getTemplateSpecializationArgs(const ClassTemplateSpecializationDecl &D) {
return D.getTemplateArgs().asArray();
}

inline ArrayRef<TemplateArgument>
getTemplateSpecializationArgs(const TemplateSpecializationType &T) {
return llvm::makeArrayRef(T.getArgs(), T.getNumArgs());
}

inline ArrayRef<TemplateArgument>
getTemplateSpecializationArgs(const FunctionDecl &FD) {
if (const auto* TemplateArgs = FD.getTemplateSpecializationArgs())
return TemplateArgs->asArray();
return ArrayRef<TemplateArgument>();
}

struct NotEqualsBoundNodePredicate {
bool operator()(const internal::BoundNodesMap &Nodes) const {
return Nodes.getNode(ID) != Node;
}

std::string ID;
DynTypedNode Node;
};

template <typename Ty, typename Enable = void> struct GetBodyMatcher {
static const Stmt *get(const Ty &Node) { return Node.getBody(); }
};

template <typename Ty>
struct GetBodyMatcher<Ty, typename std::enable_if<
std::is_base_of<FunctionDecl, Ty>::value>::type> {
static const Stmt *get(const Ty &Node) {
return Node.doesThisDeclarationHaveABody() ? Node.getBody() : nullptr;
}
};

template <typename NodeType>
inline Optional<BinaryOperatorKind>
equivalentBinaryOperator(const NodeType &Node) {
return Node.getOpcode();
}

template <>
inline Optional<BinaryOperatorKind>
equivalentBinaryOperator<CXXOperatorCallExpr>(const CXXOperatorCallExpr &Node) {
if (Node.getNumArgs() != 2)
return None;
switch (Node.getOperator()) {
default:
return None;
case OO_ArrowStar:
return BO_PtrMemI;
case OO_Star:
return BO_Mul;
case OO_Slash:
return BO_Div;
case OO_Percent:
return BO_Rem;
case OO_Plus:
return BO_Add;
case OO_Minus:
return BO_Sub;
case OO_LessLess:
return BO_Shl;
case OO_GreaterGreater:
return BO_Shr;
case OO_Spaceship:
return BO_Cmp;
case OO_Less:
return BO_LT;
case OO_Greater:
return BO_GT;
case OO_LessEqual:
return BO_LE;
case OO_GreaterEqual:
return BO_GE;
case OO_EqualEqual:
return BO_EQ;
case OO_ExclaimEqual:
return BO_NE;
case OO_Amp:
return BO_And;
case OO_Caret:
return BO_Xor;
case OO_Pipe:
return BO_Or;
case OO_AmpAmp:
return BO_LAnd;
case OO_PipePipe:
return BO_LOr;
case OO_Equal:
return BO_Assign;
case OO_StarEqual:
return BO_MulAssign;
case OO_SlashEqual:
return BO_DivAssign;
case OO_PercentEqual:
return BO_RemAssign;
case OO_PlusEqual:
return BO_AddAssign;
case OO_MinusEqual:
return BO_SubAssign;
case OO_LessLessEqual:
return BO_ShlAssign;
case OO_GreaterGreaterEqual:
return BO_ShrAssign;
case OO_AmpEqual:
return BO_AndAssign;
case OO_CaretEqual:
return BO_XorAssign;
case OO_PipeEqual:
return BO_OrAssign;
case OO_Comma:
return BO_Comma;
}
}

template <typename NodeType>
inline Optional<UnaryOperatorKind>
equivalentUnaryOperator(const NodeType &Node) {
return Node.getOpcode();
}

template <>
inline Optional<UnaryOperatorKind>
equivalentUnaryOperator<CXXOperatorCallExpr>(const CXXOperatorCallExpr &Node) {
if (Node.getNumArgs() != 1 && Node.getOperator() != OO_PlusPlus &&
Node.getOperator() != OO_MinusMinus)
return None;
switch (Node.getOperator()) {
default:
return None;
case OO_Plus:
return UO_Plus;
case OO_Minus:
return UO_Minus;
case OO_Amp:
return UO_AddrOf;
case OO_Star:
return UO_Deref;
case OO_Tilde:
return UO_Not;
case OO_Exclaim:
return UO_LNot;
case OO_PlusPlus: {
const auto *FD = Node.getDirectCallee();
if (!FD)
return None;
return FD->getNumParams() > 0 ? UO_PostInc : UO_PreInc;
}
case OO_MinusMinus: {
const auto *FD = Node.getDirectCallee();
if (!FD)
return None;
return FD->getNumParams() > 0 ? UO_PostDec : UO_PreDec;
}
case OO_Coawait:
return UO_Coawait;
}
}

template <typename NodeType> inline const Expr *getLHS(const NodeType &Node) {
return Node.getLHS();
}
template <>
inline const Expr *
getLHS<CXXOperatorCallExpr>(const CXXOperatorCallExpr &Node) {
if (!internal::equivalentBinaryOperator(Node))
return nullptr;
return Node.getArg(0);
}
template <typename NodeType> inline const Expr *getRHS(const NodeType &Node) {
return Node.getRHS();
}
template <>
inline const Expr *
getRHS<CXXOperatorCallExpr>(const CXXOperatorCallExpr &Node) {
if (!internal::equivalentBinaryOperator(Node))
return nullptr;
return Node.getArg(1);
}
template <typename NodeType>
inline const Expr *getSubExpr(const NodeType &Node) {
return Node.getSubExpr();
}
template <>
inline const Expr *
getSubExpr<CXXOperatorCallExpr>(const CXXOperatorCallExpr &Node) {
if (!internal::equivalentUnaryOperator(Node))
return nullptr;
return Node.getArg(0);
}

template <typename Ty>
struct HasSizeMatcher {
static bool hasSize(const Ty &Node, unsigned int N) {
return Node.getSize() == N;
}
};

template <>
inline bool HasSizeMatcher<StringLiteral>::hasSize(
const StringLiteral &Node, unsigned int N) {
return Node.getLength() == N;
}

template <typename Ty>
struct GetSourceExpressionMatcher {
static const Expr *get(const Ty &Node) {
return Node.getSubExpr();
}
};

template <>
inline const Expr *GetSourceExpressionMatcher<OpaqueValueExpr>::get(
const OpaqueValueExpr &Node) {
return Node.getSourceExpr();
}

template <typename Ty>
struct CompoundStmtMatcher {
static const CompoundStmt *get(const Ty &Node) {
return &Node;
}
};

template <>
inline const CompoundStmt *
CompoundStmtMatcher<StmtExpr>::get(const StmtExpr &Node) {
return Node.getSubStmt();
}





llvm::Optional<SourceLocation>
getExpansionLocOfMacro(StringRef MacroName, SourceLocation Loc,
const ASTContext &Context);

inline Optional<StringRef> getOpName(const UnaryOperator &Node) {
return Node.getOpcodeStr(Node.getOpcode());
}
inline Optional<StringRef> getOpName(const BinaryOperator &Node) {
return Node.getOpcodeStr();
}
inline StringRef getOpName(const CXXRewrittenBinaryOperator &Node) {
return Node.getOpcodeStr();
}
inline Optional<StringRef> getOpName(const CXXOperatorCallExpr &Node) {
auto optBinaryOpcode = equivalentBinaryOperator(Node);
if (!optBinaryOpcode) {
auto optUnaryOpcode = equivalentUnaryOperator(Node);
if (!optUnaryOpcode)
return None;
return UnaryOperator::getOpcodeStr(*optUnaryOpcode);
}
return BinaryOperator::getOpcodeStr(*optBinaryOpcode);
}





template <typename T, typename ArgT = std::vector<std::string>>
class HasAnyOperatorNameMatcher : public SingleNodeMatcherInterface<T> {
static_assert(std::is_same<T, BinaryOperator>::value ||
std::is_same<T, CXXOperatorCallExpr>::value ||
std::is_same<T, CXXRewrittenBinaryOperator>::value ||
std::is_same<T, UnaryOperator>::value,
"Matcher only supports `BinaryOperator`, `UnaryOperator`, "
"`CXXOperatorCallExpr` and `CXXRewrittenBinaryOperator`");
static_assert(std::is_same<ArgT, std::vector<std::string>>::value,
"Matcher ArgT must be std::vector<std::string>");

public:
explicit HasAnyOperatorNameMatcher(std::vector<std::string> Names)
: SingleNodeMatcherInterface<T>(), Names(std::move(Names)) {}

bool matchesNode(const T &Node) const override {
Optional<StringRef> OptOpName = getOpName(Node);
if (!OptOpName)
return false;
return llvm::any_of(Names, [OpName = *OptOpName](const std::string &Name) {
return Name == OpName;
});
}

private:
static Optional<StringRef> getOpName(const UnaryOperator &Node) {
return Node.getOpcodeStr(Node.getOpcode());
}
static Optional<StringRef> getOpName(const BinaryOperator &Node) {
return Node.getOpcodeStr();
}
static StringRef getOpName(const CXXRewrittenBinaryOperator &Node) {
return Node.getOpcodeStr();
}
static Optional<StringRef> getOpName(const CXXOperatorCallExpr &Node) {
auto optBinaryOpcode = equivalentBinaryOperator(Node);
if (!optBinaryOpcode) {
auto optUnaryOpcode = equivalentUnaryOperator(Node);
if (!optUnaryOpcode)
return None;
return UnaryOperator::getOpcodeStr(*optUnaryOpcode);
}
return BinaryOperator::getOpcodeStr(*optBinaryOpcode);
}

std::vector<std::string> Names;
};

using HasOpNameMatcher =
PolymorphicMatcher<HasAnyOperatorNameMatcher,
void(
TypeList<BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator, UnaryOperator>),
std::vector<std::string>>;

HasOpNameMatcher hasAnyOperatorNameFunc(ArrayRef<const StringRef *> NameRefs);

using HasOverloadOpNameMatcher =
PolymorphicMatcher<HasOverloadedOperatorNameMatcher,
void(TypeList<CXXOperatorCallExpr, FunctionDecl>),
std::vector<std::string>>;

HasOverloadOpNameMatcher
hasAnyOverloadedOperatorNameFunc(ArrayRef<const StringRef *> NameRefs);




bool matchesAnyBase(const CXXRecordDecl &Node,
const Matcher<CXXBaseSpecifier> &BaseSpecMatcher,
ASTMatchFinder *Finder, BoundNodesTreeBuilder *Builder);

std::shared_ptr<llvm::Regex> createAndVerifyRegex(StringRef Regex,
llvm::Regex::RegexFlags Flags,
StringRef MatcherID);

}

}

}

#endif
