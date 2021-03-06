










































#if !defined(LLVM_CLANG_ASTMATCHERS_ASTMATCHERS_H)
#define LLVM_CLANG_ASTMATCHERS_ASTMATCHERS_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Attr.h"
#include "clang/AST/CXXInheritance.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/LambdaCapture.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/OpenMPClause.h"
#include "clang/AST/OperationKinds.h"
#include "clang/AST/ParentMapContext.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"
#include "clang/AST/StmtObjC.h"
#include "clang/AST/StmtOpenMP.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"
#include "clang/Basic/AttrKinds.h"
#include "clang/Basic/ExceptionSpecificationType.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Basic/TypeTraits.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Regex.h"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace clang {
namespace ast_matchers {








class BoundNodes {
public:




template <typename T>
const T *getNodeAs(StringRef ID) const {
return MyBoundNodes.getNodeAs<T>(ID);
}




using IDToNodeMap = internal::BoundNodesMap::IDToNodeMap;


const IDToNodeMap &getMap() const {
return MyBoundNodes.getMap();
}

private:
friend class internal::BoundNodesTreeBuilder;


BoundNodes(internal::BoundNodesMap &MyBoundNodes)
: MyBoundNodes(MyBoundNodes) {}

internal::BoundNodesMap MyBoundNodes;
};




using DeclarationMatcher = internal::Matcher<Decl>;
using StatementMatcher = internal::Matcher<Stmt>;
using TypeMatcher = internal::Matcher<QualType>;
using TypeLocMatcher = internal::Matcher<TypeLoc>;
using NestedNameSpecifierMatcher = internal::Matcher<NestedNameSpecifier>;
using NestedNameSpecifierLocMatcher = internal::Matcher<NestedNameSpecifierLoc>;
using CXXBaseSpecifierMatcher = internal::Matcher<CXXBaseSpecifier>;
using CXXCtorInitializerMatcher = internal::Matcher<CXXCtorInitializer>;
using TemplateArgumentMatcher = internal::Matcher<TemplateArgument>;
using TemplateArgumentLocMatcher = internal::Matcher<TemplateArgumentLoc>;
















inline internal::TrueMatcher anything() { return internal::TrueMatcher(); }












extern const internal::VariadicDynCastAllOfMatcher<Decl, TranslationUnitDecl>
translationUnitDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, TypedefDecl>
typedefDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, TypedefNameDecl>
typedefNameDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, TypeAliasDecl>
typeAliasDecl;








extern const internal::VariadicDynCastAllOfMatcher<Decl, TypeAliasTemplateDecl>
typeAliasTemplateDecl;















AST_POLYMORPHIC_MATCHER(isExpansionInMainFile,
AST_POLYMORPHIC_SUPPORTED_TYPES(Decl, Stmt, TypeLoc)) {
auto &SourceManager = Finder->getASTContext().getSourceManager();
return SourceManager.isInMainFile(
SourceManager.getExpansionLoc(Node.getBeginLoc()));
}















AST_POLYMORPHIC_MATCHER(isExpansionInSystemHeader,
AST_POLYMORPHIC_SUPPORTED_TYPES(Decl, Stmt, TypeLoc)) {
auto &SourceManager = Finder->getASTContext().getSourceManager();
auto ExpansionLoc = SourceManager.getExpansionLoc(Node.getBeginLoc());
if (ExpansionLoc.isInvalid()) {
return false;
}
return SourceManager.isInSystemHeader(ExpansionLoc);
}
















AST_POLYMORPHIC_MATCHER_REGEX(isExpansionInFileMatching,
AST_POLYMORPHIC_SUPPORTED_TYPES(Decl, Stmt,
TypeLoc),
RegExp) {
auto &SourceManager = Finder->getASTContext().getSourceManager();
auto ExpansionLoc = SourceManager.getExpansionLoc(Node.getBeginLoc());
if (ExpansionLoc.isInvalid()) {
return false;
}
auto FileEntry =
SourceManager.getFileEntryForID(SourceManager.getFileID(ExpansionLoc));
if (!FileEntry) {
return false;
}

auto Filename = FileEntry->getName();
return RegExp->match(Filename);
}





AST_POLYMORPHIC_MATCHER_P(isExpandedFromMacro,
AST_POLYMORPHIC_SUPPORTED_TYPES(Decl, Stmt, TypeLoc),
std::string, MacroName) {


auto& Context = Finder->getASTContext();
llvm::Optional<SourceLocation> B =
internal::getExpansionLocOfMacro(MacroName, Node.getBeginLoc(), Context);
if (!B) return false;
llvm::Optional<SourceLocation> E =
internal::getExpansionLocOfMacro(MacroName, Node.getEndLoc(), Context);
if (!E) return false;
return *B == *E;
}










extern const internal::VariadicAllOfMatcher<Decl> decl;











extern const internal::VariadicDynCastAllOfMatcher<Decl, DecompositionDecl>
decompositionDecl;








extern const internal::VariadicDynCastAllOfMatcher<Decl, BindingDecl>
bindingDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, LinkageSpecDecl>
linkageSpecDecl;












extern const internal::VariadicDynCastAllOfMatcher<Decl, NamedDecl> namedDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, LabelDecl> labelDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, NamespaceDecl>
namespaceDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, NamespaceAliasDecl>
namespaceAliasDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, RecordDecl> recordDecl;








extern const internal::VariadicDynCastAllOfMatcher<Decl, CXXRecordDecl>
cxxRecordDecl;







extern const internal::VariadicDynCastAllOfMatcher<Decl, ClassTemplateDecl>
classTemplateDecl;











extern const internal::VariadicDynCastAllOfMatcher<
Decl, ClassTemplateSpecializationDecl>
classTemplateSpecializationDecl;
















extern const internal::VariadicDynCastAllOfMatcher<
Decl, ClassTemplatePartialSpecializationDecl>
classTemplatePartialSpecializationDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, DeclaratorDecl>
declaratorDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, ParmVarDecl>
parmVarDecl;












extern const internal::VariadicDynCastAllOfMatcher<Decl, AccessSpecDecl>
accessSpecDecl;








extern const internal::VariadicAllOfMatcher<CXXBaseSpecifier> cxxBaseSpecifier;










extern const internal::VariadicAllOfMatcher<CXXCtorInitializer>
cxxCtorInitializer;










extern const internal::VariadicAllOfMatcher<TemplateArgument> templateArgument;










extern const internal::VariadicAllOfMatcher<TemplateArgumentLoc>
templateArgumentLoc;










extern const internal::VariadicAllOfMatcher<TemplateName> templateName;









extern const internal::VariadicDynCastAllOfMatcher<Decl,
NonTypeTemplateParmDecl>
nonTypeTemplateParmDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, TemplateTypeParmDecl>
templateTypeParmDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl,
TemplateTemplateParmDecl>
templateTemplateParmDecl;


















AST_POLYMORPHIC_MATCHER(isPublic,
AST_POLYMORPHIC_SUPPORTED_TYPES(Decl,
CXXBaseSpecifier)) {
return getAccessSpecifier(Node) == AS_public;
}

















AST_POLYMORPHIC_MATCHER(isProtected,
AST_POLYMORPHIC_SUPPORTED_TYPES(Decl,
CXXBaseSpecifier)) {
return getAccessSpecifier(Node) == AS_protected;
}


















AST_POLYMORPHIC_MATCHER(isPrivate,
AST_POLYMORPHIC_SUPPORTED_TYPES(Decl,
CXXBaseSpecifier)) {
return getAccessSpecifier(Node) == AS_private;
}












AST_MATCHER(FieldDecl, isBitField) {
return Node.isBitField();
}














AST_MATCHER_P(FieldDecl, hasBitWidth, unsigned, Width) {
return Node.isBitField() &&
Node.getBitWidthValue(Finder->getASTContext()) == Width;
}















AST_MATCHER_P(FieldDecl, hasInClassInitializer, internal::Matcher<Expr>,
InnerMatcher) {
const Expr *Initializer = Node.getInClassInitializer();
return (Initializer != nullptr &&
InnerMatcher.matches(*Initializer, Finder, Builder));
}



AST_MATCHER(FunctionDecl, isMain) {
return Node.isMain();
}











AST_MATCHER_P(ClassTemplateSpecializationDecl, hasSpecializedTemplate,
internal::Matcher<ClassTemplateDecl>, InnerMatcher) {
const ClassTemplateDecl* Decl = Node.getSpecializedTemplate();
return (Decl != nullptr &&
InnerMatcher.matches(*Decl, Finder, Builder));
}



AST_MATCHER(Decl, isImplicit) {
return Node.isImplicit();
}






















AST_POLYMORPHIC_MATCHER_P(
hasAnyTemplateArgument,
AST_POLYMORPHIC_SUPPORTED_TYPES(ClassTemplateSpecializationDecl,
TemplateSpecializationType,
FunctionDecl),
internal::Matcher<TemplateArgument>, InnerMatcher) {
ArrayRef<TemplateArgument> List =
internal::getTemplateSpecializationArgs(Node);
return matchesFirstInRange(InnerMatcher, List.begin(), List.end(), Finder,
Builder) != List.end();
}

















template <typename T>
internal::Matcher<T> traverse(TraversalKind TK,
const internal::Matcher<T> &InnerMatcher) {
return internal::DynTypedMatcher::constructRestrictedWrapper(
new internal::TraversalMatcher<T>(TK, InnerMatcher),
InnerMatcher.getID().first)
.template unconditionalConvertTo<T>();
}

template <typename T>
internal::BindableMatcher<T>
traverse(TraversalKind TK, const internal::BindableMatcher<T> &InnerMatcher) {
return internal::BindableMatcher<T>(
internal::DynTypedMatcher::constructRestrictedWrapper(
new internal::TraversalMatcher<T>(TK, InnerMatcher),
InnerMatcher.getID().first)
.template unconditionalConvertTo<T>());
}

template <typename... T>
internal::TraversalWrapper<internal::VariadicOperatorMatcher<T...>>
traverse(TraversalKind TK,
const internal::VariadicOperatorMatcher<T...> &InnerMatcher) {
return internal::TraversalWrapper<internal::VariadicOperatorMatcher<T...>>(
TK, InnerMatcher);
}

template <template <typename ToArg, typename FromArg> class ArgumentAdapterT,
typename T, typename ToTypes>
internal::TraversalWrapper<
internal::ArgumentAdaptingMatcherFuncAdaptor<ArgumentAdapterT, T, ToTypes>>
traverse(TraversalKind TK, const internal::ArgumentAdaptingMatcherFuncAdaptor<
ArgumentAdapterT, T, ToTypes> &InnerMatcher) {
return internal::TraversalWrapper<
internal::ArgumentAdaptingMatcherFuncAdaptor<ArgumentAdapterT, T,
ToTypes>>(TK, InnerMatcher);
}

template <template <typename T, typename... P> class MatcherT, typename... P,
typename ReturnTypesF>
internal::TraversalWrapper<
internal::PolymorphicMatcher<MatcherT, ReturnTypesF, P...>>
traverse(TraversalKind TK,
const internal::PolymorphicMatcher<MatcherT, ReturnTypesF, P...>
&InnerMatcher) {
return internal::TraversalWrapper<
internal::PolymorphicMatcher<MatcherT, ReturnTypesF, P...>>(TK,
InnerMatcher);
}

template <typename... T>
internal::Matcher<typename internal::GetClade<T...>::Type>
traverse(TraversalKind TK, const internal::MapAnyOfHelper<T...> &InnerMatcher) {
return traverse(TK, InnerMatcher.with());
}






















AST_MATCHER_P(Expr, ignoringImplicit, internal::Matcher<Expr>,
InnerMatcher) {
return InnerMatcher.matches(*Node.IgnoreImplicit(), Finder, Builder);
}


























AST_MATCHER_P(Expr, ignoringImpCasts,
internal::Matcher<Expr>, InnerMatcher) {
return InnerMatcher.matches(*Node.IgnoreImpCasts(), Finder, Builder);
}


















AST_MATCHER_P(Expr, ignoringParenCasts, internal::Matcher<Expr>, InnerMatcher) {
return InnerMatcher.matches(*Node.IgnoreParenCasts(), Finder, Builder);
}






















AST_MATCHER_P(Expr, ignoringParenImpCasts,
internal::Matcher<Expr>, InnerMatcher) {
return InnerMatcher.matches(*Node.IgnoreParenImpCasts(), Finder, Builder);
}












AST_MATCHER_P_OVERLOAD(QualType, ignoringParens, internal::Matcher<QualType>,
InnerMatcher, 0) {
return InnerMatcher.matches(Node.IgnoreParens(), Finder, Builder);
}












AST_MATCHER_P_OVERLOAD(Expr, ignoringParens, internal::Matcher<Expr>,
InnerMatcher, 1) {
const Expr *E = Node.IgnoreParens();
return InnerMatcher.matches(*E, Finder, Builder);
}














AST_MATCHER(Expr, isInstantiationDependent) {
return Node.isInstantiationDependent();
}













AST_MATCHER(Expr, isTypeDependent) { return Node.isTypeDependent(); }










AST_MATCHER(Expr, isValueDependent) { return Node.isValueDependent(); }



















AST_POLYMORPHIC_MATCHER_P2(
hasTemplateArgument,
AST_POLYMORPHIC_SUPPORTED_TYPES(ClassTemplateSpecializationDecl,
TemplateSpecializationType,
FunctionDecl),
unsigned, N, internal::Matcher<TemplateArgument>, InnerMatcher) {
ArrayRef<TemplateArgument> List =
internal::getTemplateSpecializationArgs(Node);
if (List.size() <= N)
return false;
return InnerMatcher.matches(List[N], Finder, Builder);
}










AST_POLYMORPHIC_MATCHER_P(
templateArgumentCountIs,
AST_POLYMORPHIC_SUPPORTED_TYPES(ClassTemplateSpecializationDecl,
TemplateSpecializationType),
unsigned, N) {
return internal::getTemplateSpecializationArgs(Node).size() == N;
}












AST_MATCHER_P(TemplateArgument, refersToType,
internal::Matcher<QualType>, InnerMatcher) {
if (Node.getKind() != TemplateArgument::Type)
return false;
return InnerMatcher.matches(Node.getAsType(), Finder, Builder);
}












AST_MATCHER_P(TemplateArgument, refersToTemplate,
internal::Matcher<TemplateName>, InnerMatcher) {
if (Node.getKind() != TemplateArgument::Template)
return false;
return InnerMatcher.matches(Node.getAsTemplate(), Finder, Builder);
}














AST_MATCHER_P(TemplateArgument, refersToDeclaration,
internal::Matcher<Decl>, InnerMatcher) {
if (Node.getKind() == TemplateArgument::Declaration)
return InnerMatcher.matches(*Node.getAsDecl(), Finder, Builder);
return false;
}













AST_MATCHER_P(TemplateArgument, isExpr, internal::Matcher<Expr>, InnerMatcher) {
if (Node.getKind() == TemplateArgument::Expression)
return InnerMatcher.matches(*Node.getAsExpr(), Finder, Builder);
return false;
}












AST_MATCHER(TemplateArgument, isIntegral) {
return Node.getKind() == TemplateArgument::Integral;
}











AST_MATCHER_P(TemplateArgument, refersToIntegralType,
internal::Matcher<QualType>, InnerMatcher) {
if (Node.getKind() != TemplateArgument::Integral)
return false;
return InnerMatcher.matches(Node.getIntegralType(), Finder, Builder);
}















AST_MATCHER_P(TemplateArgument, equalsIntegralValue,
std::string, Value) {
if (Node.getKind() != TemplateArgument::Integral)
return false;
return toString(Node.getAsIntegral(), 10) == Value;
}











extern const internal::VariadicDynCastAllOfMatcher<Stmt,
ObjCAutoreleasePoolStmt> autoreleasePoolStmt;








extern const internal::VariadicDynCastAllOfMatcher<Decl, ValueDecl> valueDecl;












extern const internal::VariadicDynCastAllOfMatcher<Decl, CXXConstructorDecl>
cxxConstructorDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, CXXDestructorDecl>
cxxDestructorDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, EnumDecl> enumDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, EnumConstantDecl>
enumConstantDecl;













extern const internal::VariadicDynCastAllOfMatcher<Decl, TagDecl> tagDecl;







extern const internal::VariadicDynCastAllOfMatcher<Decl, CXXMethodDecl>
cxxMethodDecl;







extern const internal::VariadicDynCastAllOfMatcher<Decl, CXXConversionDecl>
cxxConversionDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, CXXDeductionGuideDecl>
cxxDeductionGuideDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, VarDecl> varDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, FieldDecl> fieldDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, IndirectFieldDecl>
indirectFieldDecl;







extern const internal::VariadicDynCastAllOfMatcher<Decl, FunctionDecl>
functionDecl;







extern const internal::VariadicDynCastAllOfMatcher<Decl, FunctionTemplateDecl>
functionTemplateDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, FriendDecl> friendDecl;









extern const internal::VariadicAllOfMatcher<Stmt> stmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, DeclStmt> declStmt;












extern const internal::VariadicDynCastAllOfMatcher<Stmt, MemberExpr> memberExpr;













extern const internal::VariadicDynCastAllOfMatcher<Stmt, UnresolvedMemberExpr>
unresolvedMemberExpr;










extern const internal::VariadicDynCastAllOfMatcher<Stmt,
CXXDependentScopeMemberExpr>
cxxDependentScopeMemberExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CallExpr> callExpr;





















AST_MATCHER(CallExpr, usesADL) { return Node.usesADL(); }







extern const internal::VariadicDynCastAllOfMatcher<Stmt, LambdaExpr> lambdaExpr;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXMemberCallExpr>
cxxMemberCallExpr;










extern const internal::VariadicDynCastAllOfMatcher<Stmt, ObjCMessageExpr>
objcMessageExpr;








extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCInterfaceDecl>
objcInterfaceDecl;








extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCImplementationDecl>
objcImplementationDecl;








extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCProtocolDecl>
objcProtocolDecl;








extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCCategoryDecl>
objcCategoryDecl;








extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCCategoryImplDecl>
objcCategoryImplDecl;













extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCMethodDecl>
objcMethodDecl;











extern const internal::VariadicDynCastAllOfMatcher<Decl, BlockDecl>
blockDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCIvarDecl>
objcIvarDecl;









extern const internal::VariadicDynCastAllOfMatcher<Decl, ObjCPropertyDecl>
objcPropertyDecl;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, ObjCAtThrowStmt>
objcThrowStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, ObjCAtTryStmt>
objcTryStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, ObjCAtCatchStmt>
objcCatchStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, ObjCAtFinallyStmt>
objcFinallyStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, ExprWithCleanups>
exprWithCleanups;











extern const internal::VariadicDynCastAllOfMatcher<Stmt, InitListExpr>
initListExpr;



AST_MATCHER_P(InitListExpr, hasSyntacticForm,
internal::Matcher<Expr>, InnerMatcher) {
const Expr *SyntForm = Node.getSyntacticForm();
return (SyntForm != nullptr &&
InnerMatcher.matches(*SyntForm, Finder, Builder));
}












extern const internal::VariadicDynCastAllOfMatcher<Stmt,
CXXStdInitializerListExpr>
cxxStdInitializerListExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, ImplicitValueInitExpr>
implicitValueInitExpr;
















extern const internal::VariadicDynCastAllOfMatcher<Stmt, ParenListExpr>
parenListExpr;











extern const internal::VariadicDynCastAllOfMatcher<Stmt,
SubstNonTypeTemplateParmExpr>
substNonTypeTemplateParmExpr;










extern const internal::VariadicDynCastAllOfMatcher<Decl, UsingDecl> usingDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, UsingEnumDecl>
usingEnumDecl;










extern const internal::VariadicDynCastAllOfMatcher<Decl, UsingDirectiveDecl>
usingDirectiveDecl;















extern const internal::VariadicDynCastAllOfMatcher<Stmt, UnresolvedLookupExpr>
unresolvedLookupExpr;












extern const internal::VariadicDynCastAllOfMatcher<Decl,
UnresolvedUsingValueDecl>
unresolvedUsingValueDecl;
















extern const internal::VariadicDynCastAllOfMatcher<Decl,
UnresolvedUsingTypenameDecl>
unresolvedUsingTypenameDecl;










extern const internal::VariadicDynCastAllOfMatcher<Stmt, ConstantExpr>
constantExpr;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, ParenExpr> parenExpr;











extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXConstructExpr>
cxxConstructExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt,
CXXUnresolvedConstructExpr>
cxxUnresolvedConstructExpr;











extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXThisExpr>
cxxThisExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXBindTemporaryExpr>
cxxBindTemporaryExpr;



















extern const internal::VariadicDynCastAllOfMatcher<Stmt,
MaterializeTemporaryExpr>
materializeTemporaryExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXNewExpr> cxxNewExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXDeleteExpr>
cxxDeleteExpr;














extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXNoexceptExpr>
cxxNoexceptExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, ArraySubscriptExpr>
arraySubscriptExpr;










extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXDefaultArgExpr>
cxxDefaultArgExpr;

















extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXOperatorCallExpr>
cxxOperatorCallExpr;


















extern const internal::VariadicDynCastAllOfMatcher<Stmt,
CXXRewrittenBinaryOperator>
cxxRewrittenBinaryOperator;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, Expr> expr;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, DeclRefExpr>
declRefExpr;












extern const internal::VariadicDynCastAllOfMatcher<Stmt, ObjCIvarRefExpr>
objcIvarRefExpr;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, BlockExpr> blockExpr;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, IfStmt> ifStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, ForStmt> forStmt;









AST_MATCHER_P(ForStmt, hasIncrement, internal::Matcher<Stmt>,
InnerMatcher) {
const Stmt *const Increment = Node.getInc();
return (Increment != nullptr &&
InnerMatcher.matches(*Increment, Finder, Builder));
}









AST_MATCHER_P(ForStmt, hasLoopInit, internal::Matcher<Stmt>,
InnerMatcher) {
const Stmt *const Init = Node.getInit();
return (Init != nullptr && InnerMatcher.matches(*Init, Finder, Builder));
}








extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXForRangeStmt>
cxxForRangeStmt;









AST_MATCHER_P(CXXForRangeStmt, hasLoopVariable, internal::Matcher<VarDecl>,
InnerMatcher) {
const VarDecl *const Var = Node.getLoopVariable();
return (Var != nullptr && InnerMatcher.matches(*Var, Finder, Builder));
}









AST_MATCHER_P(CXXForRangeStmt, hasRangeInit, internal::Matcher<Expr>,
InnerMatcher) {
const Expr *const Init = Node.getRangeInit();
return (Init != nullptr && InnerMatcher.matches(*Init, Finder, Builder));
}









extern const internal::VariadicDynCastAllOfMatcher<Stmt, WhileStmt> whileStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, DoStmt> doStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, BreakStmt> breakStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, ContinueStmt>
continueStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CoreturnStmt>
coreturnStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, ReturnStmt> returnStmt;










extern const internal::VariadicDynCastAllOfMatcher<Stmt, GotoStmt> gotoStmt;










extern const internal::VariadicDynCastAllOfMatcher<Stmt, LabelStmt> labelStmt;











extern const internal::VariadicDynCastAllOfMatcher<Stmt, AddrLabelExpr>
addrLabelExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, SwitchStmt> switchStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, SwitchCase> switchCase;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CaseStmt> caseStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, DefaultStmt>
defaultStmt;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, CompoundStmt>
compoundStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXCatchStmt>
cxxCatchStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXTryStmt> cxxTryStmt;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXThrowExpr>
cxxThrowExpr;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, NullStmt> nullStmt;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, AsmStmt> asmStmt;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXBoolLiteralExpr>
cxxBoolLiteral;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, StringLiteral>
stringLiteral;











extern const internal::VariadicDynCastAllOfMatcher<Stmt, CharacterLiteral>
characterLiteral;





extern const internal::VariadicDynCastAllOfMatcher<Stmt, IntegerLiteral>
integerLiteral;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, FloatingLiteral>
floatLiteral;



extern const internal::VariadicDynCastAllOfMatcher<Stmt, ImaginaryLiteral>
imaginaryLiteral;


extern const internal::VariadicDynCastAllOfMatcher<Stmt, FixedPointLiteral>
fixedPointLiteral;




extern const internal::VariadicDynCastAllOfMatcher<Stmt, UserDefinedLiteral>
userDefinedLiteral;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, CompoundLiteralExpr>
compoundLiteralExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CoawaitExpr>
coawaitExpr;

extern const internal::VariadicDynCastAllOfMatcher<Stmt, DependentCoawaitExpr>
dependentCoawaitExpr;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, CoyieldExpr>
coyieldExpr;


extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXNullPtrLiteralExpr>
cxxNullPtrLiteralExpr;


extern const internal::VariadicDynCastAllOfMatcher<Stmt, ChooseExpr>
chooseExpr;


extern const internal::VariadicDynCastAllOfMatcher<Stmt, GNUNullExpr>
gnuNullExpr;


extern const internal::VariadicDynCastAllOfMatcher<Stmt, GenericSelectionExpr>
genericSelectionExpr;






extern const internal::VariadicDynCastAllOfMatcher<Stmt, AtomicExpr> atomicExpr;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, StmtExpr> stmtExpr;








extern const internal::VariadicDynCastAllOfMatcher<Stmt, BinaryOperator>
binaryOperator;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, UnaryOperator>
unaryOperator;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, ConditionalOperator>
conditionalOperator;







extern const internal::VariadicDynCastAllOfMatcher<Stmt,
BinaryConditionalOperator>
binaryConditionalOperator;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, OpaqueValueExpr>
opaqueValueExpr;














extern const internal::VariadicDynCastAllOfMatcher<Decl, StaticAssertDecl>
staticAssertDecl;











extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXReinterpretCastExpr>
cxxReinterpretCastExpr;














extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXStaticCastExpr>
cxxStaticCastExpr;













extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXDynamicCastExpr>
cxxDynamicCastExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXConstCastExpr>
cxxConstCastExpr;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, CStyleCastExpr>
cStyleCastExpr;






















extern const internal::VariadicDynCastAllOfMatcher<Stmt, ExplicitCastExpr>
explicitCastExpr;





extern const internal::VariadicDynCastAllOfMatcher<Stmt, ImplicitCastExpr>
implicitCastExpr;














extern const internal::VariadicDynCastAllOfMatcher<Stmt, CastExpr> castExpr;









extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXFunctionalCastExpr>
cxxFunctionalCastExpr;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, CXXTemporaryObjectExpr>
cxxTemporaryObjectExpr;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, PredefinedExpr>
predefinedExpr;







extern const internal::VariadicDynCastAllOfMatcher<Stmt, DesignatedInitExpr>
designatedInitExpr;












AST_MATCHER_P(DesignatedInitExpr, designatorCountIs, unsigned, N) {
return Node.size() == N;
}


extern const internal::VariadicAllOfMatcher<QualType> qualType;


extern const internal::VariadicAllOfMatcher<Type> type;


extern const internal::VariadicAllOfMatcher<TypeLoc> typeLoc;




















extern const internal::VariadicOperatorMatcherFunc<
2, std::numeric_limits<unsigned>::max()>
eachOf;




extern const internal::VariadicOperatorMatcherFunc<
2, std::numeric_limits<unsigned>::max()>
anyOf;




extern const internal::VariadicOperatorMatcherFunc<
2, std::numeric_limits<unsigned>::max()>
allOf;

























extern const internal::VariadicOperatorMatcherFunc<1, 1> optionally;










extern const internal::VariadicDynCastAllOfMatcher<Stmt,
UnaryExprOrTypeTraitExpr>
unaryExprOrTypeTraitExpr;


























template <typename T, typename... U>
auto mapAnyOf(internal::VariadicDynCastAllOfMatcher<T, U> const &...) {
return internal::MapAnyOfHelper<U...>();
}









































































extern const internal::MapAnyOfMatcher<BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator>
binaryOperation;



































extern const internal::MapAnyOfMatcher<CallExpr, CXXConstructExpr> invocation;









AST_MATCHER_P(UnaryExprOrTypeTraitExpr, hasArgumentOfType,
internal::Matcher<QualType>, InnerMatcher) {
const QualType ArgumentType = Node.getTypeOfArgument();
return InnerMatcher.matches(ArgumentType, Finder, Builder);
}













AST_MATCHER_P(UnaryExprOrTypeTraitExpr, ofKind, UnaryExprOrTypeTrait, Kind) {
return Node.getKind() == Kind;
}



inline internal::BindableMatcher<Stmt> alignOfExpr(
const internal::Matcher<UnaryExprOrTypeTraitExpr> &InnerMatcher) {
return stmt(unaryExprOrTypeTraitExpr(
allOf(anyOf(ofKind(UETT_AlignOf), ofKind(UETT_PreferredAlignOf)),
InnerMatcher)));
}



inline internal::BindableMatcher<Stmt> sizeOfExpr(
const internal::Matcher<UnaryExprOrTypeTraitExpr> &InnerMatcher) {
return stmt(unaryExprOrTypeTraitExpr(
allOf(ofKind(UETT_SizeOf), InnerMatcher)));
}
















inline internal::Matcher<NamedDecl> hasName(StringRef Name) {
return internal::Matcher<NamedDecl>(
new internal::HasNameMatcher({std::string(Name)}));
}











extern const internal::VariadicFunction<internal::Matcher<NamedDecl>, StringRef,
internal::hasAnyNameFunc>
hasAnyName;

















AST_MATCHER_REGEX(NamedDecl, matchesName, RegExp) {
std::string FullNameString = "::" + Node.getQualifiedNameAsString();
return RegExp->match(FullNameString);
}




















inline internal::PolymorphicMatcher<
internal::HasOverloadedOperatorNameMatcher,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXOperatorCallExpr, FunctionDecl),
std::vector<std::string>>
hasOverloadedOperatorName(StringRef Name) {
return internal::PolymorphicMatcher<
internal::HasOverloadedOperatorNameMatcher,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXOperatorCallExpr, FunctionDecl),
std::vector<std::string>>({std::string(Name)});
}









extern const internal::VariadicFunction<
internal::PolymorphicMatcher<internal::HasOverloadedOperatorNameMatcher,
AST_POLYMORPHIC_SUPPORTED_TYPES(
CXXOperatorCallExpr, FunctionDecl),
std::vector<std::string>>,
StringRef, internal::hasAnyOverloadedOperatorNameFunc>
hasAnyOverloadedOperatorName;





















AST_MATCHER_P(CXXDependentScopeMemberExpr, hasMemberName, std::string, N) {
return Node.getMember().getAsString() == N;
}



































AST_MATCHER_P(CXXDependentScopeMemberExpr, memberHasSameNameAsBoundNode,
std::string, BindingID) {
auto MemberName = Node.getMember().getAsString();

return Builder->removeBindings(
[this, MemberName](const BoundNodesMap &Nodes) {
const auto &BN = Nodes.getNode(this->BindingID);
if (const auto *ND = BN.get<NamedDecl>()) {
if (!isa<FieldDecl, CXXMethodDecl, VarDecl>(ND))
return true;
return ND->getName() != MemberName;
}
return true;
});
}































AST_POLYMORPHIC_MATCHER_P(
isDerivedFrom,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXRecordDecl, ObjCInterfaceDecl),
internal::Matcher<NamedDecl>, Base) {

if (const auto *RD = dyn_cast<CXXRecordDecl>(&Node))
return Finder->classIsDerivedFrom(RD, Base, Builder, false);


const auto *InterfaceDecl = cast<ObjCInterfaceDecl>(&Node);
return Finder->objcClassIsDerivedFrom(InterfaceDecl, Base, Builder,
false);
}


AST_POLYMORPHIC_MATCHER_P_OVERLOAD(
isDerivedFrom,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXRecordDecl, ObjCInterfaceDecl),
std::string, BaseName, 1) {
if (BaseName.empty())
return false;

const auto M = isDerivedFrom(hasName(BaseName));

if (const auto *RD = dyn_cast<CXXRecordDecl>(&Node))
return Matcher<CXXRecordDecl>(M).matches(*RD, Finder, Builder);

const auto *InterfaceDecl = cast<ObjCInterfaceDecl>(&Node);
return Matcher<ObjCInterfaceDecl>(M).matches(*InterfaceDecl, Finder, Builder);
}
















AST_MATCHER_P(CXXRecordDecl, hasAnyBase, internal::Matcher<CXXBaseSpecifier>,
BaseSpecMatcher) {
return internal::matchesAnyBase(Node, BaseSpecMatcher, Finder, Builder);
}













AST_MATCHER_P(CXXRecordDecl, hasDirectBase, internal::Matcher<CXXBaseSpecifier>,
BaseSpecMatcher) {
return Node.hasDefinition() &&
llvm::any_of(Node.bases(), [&](const CXXBaseSpecifier &Base) {
return BaseSpecMatcher.matches(Base, Finder, Builder);
});
}



AST_POLYMORPHIC_MATCHER_P_OVERLOAD(
isSameOrDerivedFrom,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXRecordDecl, ObjCInterfaceDecl),
internal::Matcher<NamedDecl>, Base, 0) {
const auto M = anyOf(Base, isDerivedFrom(Base));

if (const auto *RD = dyn_cast<CXXRecordDecl>(&Node))
return Matcher<CXXRecordDecl>(M).matches(*RD, Finder, Builder);

const auto *InterfaceDecl = cast<ObjCInterfaceDecl>(&Node);
return Matcher<ObjCInterfaceDecl>(M).matches(*InterfaceDecl, Finder, Builder);
}



AST_POLYMORPHIC_MATCHER_P_OVERLOAD(
isSameOrDerivedFrom,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXRecordDecl, ObjCInterfaceDecl),
std::string, BaseName, 1) {
if (BaseName.empty())
return false;

const auto M = isSameOrDerivedFrom(hasName(BaseName));

if (const auto *RD = dyn_cast<CXXRecordDecl>(&Node))
return Matcher<CXXRecordDecl>(M).matches(*RD, Finder, Builder);

const auto *InterfaceDecl = cast<ObjCInterfaceDecl>(&Node);
return Matcher<ObjCInterfaceDecl>(M).matches(*InterfaceDecl, Finder, Builder);
}






















AST_POLYMORPHIC_MATCHER_P_OVERLOAD(
isDirectlyDerivedFrom,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXRecordDecl, ObjCInterfaceDecl),
internal::Matcher<NamedDecl>, Base, 0) {

if (const auto *RD = dyn_cast<CXXRecordDecl>(&Node))
return Finder->classIsDerivedFrom(RD, Base, Builder, true);


const auto *InterfaceDecl = cast<ObjCInterfaceDecl>(&Node);
return Finder->objcClassIsDerivedFrom(InterfaceDecl, Base, Builder,
true);
}


AST_POLYMORPHIC_MATCHER_P_OVERLOAD(
isDirectlyDerivedFrom,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXRecordDecl, ObjCInterfaceDecl),
std::string, BaseName, 1) {
if (BaseName.empty())
return false;
const auto M = isDirectlyDerivedFrom(hasName(BaseName));

if (const auto *RD = dyn_cast<CXXRecordDecl>(&Node))
return Matcher<CXXRecordDecl>(M).matches(*RD, Finder, Builder);

const auto *InterfaceDecl = cast<ObjCInterfaceDecl>(&Node);
return Matcher<ObjCInterfaceDecl>(M).matches(*InterfaceDecl, Finder, Builder);
}











AST_MATCHER_P(CXXRecordDecl, hasMethod, internal::Matcher<CXXMethodDecl>,
InnerMatcher) {
BoundNodesTreeBuilder Result(*Builder);
auto MatchIt = matchesFirstInPointerRange(InnerMatcher, Node.method_begin(),
Node.method_end(), Finder, &Result);
if (MatchIt == Node.method_end())
return false;

if (Finder->isTraversalIgnoringImplicitNodes() && (*MatchIt)->isImplicit())
return false;
*Builder = std::move(Result);
return true;
}










AST_MATCHER(CXXRecordDecl, isLambda) {
return Node.isLambda();
}



















extern const internal::ArgumentAdaptingMatcherFunc<internal::HasMatcher> has;















extern const internal::ArgumentAdaptingMatcherFunc<
internal::HasDescendantMatcher>
hasDescendant;



















extern const internal::ArgumentAdaptingMatcherFunc<internal::ForEachMatcher>
forEach;




























extern const internal::ArgumentAdaptingMatcherFunc<
internal::ForEachDescendantMatcher>
forEachDescendant;

















template <typename T>
internal::Matcher<T> findAll(const internal::Matcher<T> &Matcher) {
return eachOf(Matcher, forEachDescendant(Matcher));
}











extern const internal::ArgumentAdaptingMatcherFunc<
internal::HasParentMatcher,
internal::TypeList<Decl, NestedNameSpecifierLoc, Stmt, TypeLoc>,
internal::TypeList<Decl, NestedNameSpecifierLoc, Stmt, TypeLoc>>
hasParent;












extern const internal::ArgumentAdaptingMatcherFunc<
internal::HasAncestorMatcher,
internal::TypeList<Decl, NestedNameSpecifierLoc, Stmt, TypeLoc>,
internal::TypeList<Decl, NestedNameSpecifierLoc, Stmt, TypeLoc>>
hasAncestor;










extern const internal::VariadicOperatorMatcherFunc<1, 1> unless;



































inline internal::PolymorphicMatcher<
internal::HasDeclarationMatcher,
void(internal::HasDeclarationSupportedTypes), internal::Matcher<Decl>>
hasDeclaration(const internal::Matcher<Decl> &InnerMatcher) {
return internal::PolymorphicMatcher<
internal::HasDeclarationMatcher,
void(internal::HasDeclarationSupportedTypes), internal::Matcher<Decl>>(
InnerMatcher);
}












AST_MATCHER_P(NamedDecl, hasUnderlyingDecl, internal::Matcher<NamedDecl>,
InnerMatcher) {
const NamedDecl *UnderlyingDecl = Node.getUnderlyingDecl();

return UnderlyingDecl != nullptr &&
InnerMatcher.matches(*UnderlyingDecl, Finder, Builder);
}



















AST_MATCHER_P(CXXMemberCallExpr, on, internal::Matcher<Expr>,
InnerMatcher) {
const Expr *ExprNode = Node.getImplicitObjectArgument()
->IgnoreParenImpCasts();
return (ExprNode != nullptr &&
InnerMatcher.matches(*ExprNode, Finder, Builder));
}












AST_MATCHER_P(ObjCMessageExpr, hasReceiverType, internal::Matcher<QualType>,
InnerMatcher) {
const QualType TypeDecl = Node.getReceiverType();
return InnerMatcher.matches(TypeDecl, Finder, Builder);
}













AST_MATCHER(ObjCMethodDecl, isClassMethod) {
return Node.isClassMethod();
}













AST_MATCHER(ObjCMethodDecl, isInstanceMethod) {
return Node.isInstanceMethod();
}














AST_MATCHER(ObjCMessageExpr, isClassMessage) {
return Node.isClassMessage();
}














AST_MATCHER(ObjCMessageExpr, isInstanceMessage) {
return Node.isInstanceMessage();
}











AST_MATCHER_P(ObjCMessageExpr, hasReceiver, internal::Matcher<Expr>,
InnerMatcher) {
const Expr *ReceiverNode = Node.getInstanceReceiver();
return (ReceiverNode != nullptr &&
InnerMatcher.matches(*ReceiverNode->IgnoreParenImpCasts(), Finder,
Builder));
}









AST_MATCHER_P(ObjCMessageExpr, hasSelector, std::string, BaseName) {
Selector Sel = Node.getSelector();
return BaseName.compare(Sel.getAsString()) == 0;
}











extern const internal::VariadicFunction<internal::Matcher<ObjCMessageExpr>,
StringRef,
internal::hasAnySelectorFunc>
hasAnySelector;









AST_MATCHER_REGEX(ObjCMessageExpr, matchesSelector, RegExp) {
std::string SelectorString = Node.getSelector().getAsString();
return RegExp->match(SelectorString);
}





AST_MATCHER(ObjCMessageExpr, hasNullSelector) {
return Node.getSelector().isNull();
}









AST_MATCHER(ObjCMessageExpr, hasUnarySelector) {
return Node.getSelector().isUnarySelector();
}













AST_MATCHER(ObjCMessageExpr, hasKeywordSelector) {
return Node.getSelector().isKeywordSelector();
}












AST_MATCHER_P(ObjCMessageExpr, numSelectorArgs, unsigned, N) {
return Node.getSelector().getNumArgs() == N;
}

















AST_MATCHER_P(CallExpr, callee, internal::Matcher<Stmt>,
InnerMatcher) {
const Expr *ExprNode = Node.getCallee();
return (ExprNode != nullptr &&
InnerMatcher.matches(*ExprNode, Finder, Builder));
}










AST_MATCHER_P_OVERLOAD(CallExpr, callee, internal::Matcher<Decl>, InnerMatcher,
1) {
return callExpr(hasDeclaration(InnerMatcher)).matches(Node, Finder, Builder);
}

















AST_POLYMORPHIC_MATCHER_P_OVERLOAD(
hasType,
AST_POLYMORPHIC_SUPPORTED_TYPES(Expr, FriendDecl, TypedefNameDecl,
ValueDecl, CXXBaseSpecifier),
internal::Matcher<QualType>, InnerMatcher, 0) {
QualType QT = internal::getUnderlyingType(Node);
if (!QT.isNull())
return InnerMatcher.matches(QT, Finder, Builder);
return false;
}































AST_POLYMORPHIC_MATCHER_P_OVERLOAD(
hasType,
AST_POLYMORPHIC_SUPPORTED_TYPES(Expr, FriendDecl, ValueDecl,
CXXBaseSpecifier),
internal::Matcher<Decl>, InnerMatcher, 1) {
QualType QT = internal::getUnderlyingType(Node);
if (!QT.isNull())
return qualType(hasDeclaration(InnerMatcher)).matches(QT, Finder, Builder);
return false;
}































AST_POLYMORPHIC_MATCHER_P(
hasTypeLoc,
AST_POLYMORPHIC_SUPPORTED_TYPES(
BlockDecl, CXXBaseSpecifier, CXXCtorInitializer, CXXFunctionalCastExpr,
CXXNewExpr, CXXTemporaryObjectExpr, CXXUnresolvedConstructExpr,
ClassTemplateSpecializationDecl, CompoundLiteralExpr, DeclaratorDecl,
ExplicitCastExpr, ObjCPropertyDecl, TemplateArgumentLoc,
TypedefNameDecl),
internal::Matcher<TypeLoc>, Inner) {
TypeSourceInfo *source = internal::GetTypeSourceInfo(Node);
if (source == nullptr) {

return false;
}
return Inner.matches(source->getTypeLoc(), Finder, Builder);
}










AST_MATCHER_P(QualType, asString, std::string, Name) {
return Name == Node.getAsString();
}











AST_MATCHER_P(
QualType, pointsTo, internal::Matcher<QualType>,
InnerMatcher) {
return (!Node.isNull() && Node->isAnyPointerType() &&
InnerMatcher.matches(Node->getPointeeType(), Finder, Builder));
}


AST_MATCHER_P_OVERLOAD(QualType, pointsTo, internal::Matcher<Decl>,
InnerMatcher, 1) {
return pointsTo(qualType(hasDeclaration(InnerMatcher)))
.matches(Node, Finder, Builder);
}











AST_MATCHER_P(Type, hasUnqualifiedDesugaredType, internal::Matcher<Type>,
InnerMatcher) {
return InnerMatcher.matches(*Node.getUnqualifiedDesugaredType(), Finder,
Builder);
}














AST_MATCHER_P(QualType, references, internal::Matcher<QualType>,
InnerMatcher) {
return (!Node.isNull() && Node->isReferenceType() &&
InnerMatcher.matches(Node->getPointeeType(), Finder, Builder));
}













AST_MATCHER_P(QualType, hasCanonicalType, internal::Matcher<QualType>,
InnerMatcher) {
if (Node.isNull())
return false;
return InnerMatcher.matches(Node.getCanonicalType(), Finder, Builder);
}


AST_MATCHER_P_OVERLOAD(QualType, references, internal::Matcher<Decl>,
InnerMatcher, 1) {
return references(qualType(hasDeclaration(InnerMatcher)))
.matches(Node, Finder, Builder);
}


















AST_MATCHER_P(CXXMemberCallExpr, onImplicitObjectArgument,
internal::Matcher<Expr>, InnerMatcher) {
const Expr *ExprNode = Node.getImplicitObjectArgument();
return (ExprNode != nullptr &&
InnerMatcher.matches(*ExprNode, Finder, Builder));
}

















AST_MATCHER_P_OVERLOAD(CXXMemberCallExpr, thisPointerType,
internal::Matcher<QualType>, InnerMatcher, 0) {
return onImplicitObjectArgument(
anyOf(hasType(InnerMatcher), hasType(pointsTo(InnerMatcher))))
.matches(Node, Finder, Builder);
}


AST_MATCHER_P_OVERLOAD(CXXMemberCallExpr, thisPointerType,
internal::Matcher<Decl>, InnerMatcher, 1) {
return onImplicitObjectArgument(
anyOf(hasType(InnerMatcher), hasType(pointsTo(InnerMatcher))))
.matches(Node, Finder, Builder);
}










AST_MATCHER_P(DeclRefExpr, to, internal::Matcher<Decl>,
InnerMatcher) {
const Decl *DeclNode = Node.getDecl();
return (DeclNode != nullptr &&
InnerMatcher.matches(*DeclNode, Finder, Builder));
}















AST_MATCHER_P(DeclRefExpr, throughUsingDecl,
internal::Matcher<UsingShadowDecl>, InnerMatcher) {
const NamedDecl *FoundDecl = Node.getFoundDecl();
if (const UsingShadowDecl *UsingDecl = dyn_cast<UsingShadowDecl>(FoundDecl))
return InnerMatcher.matches(*UsingDecl, Finder, Builder);
return false;
}
















AST_MATCHER_P(OverloadExpr, hasAnyDeclaration, internal::Matcher<Decl>,
InnerMatcher) {
return matchesFirstInPointerRange(InnerMatcher, Node.decls_begin(),
Node.decls_end(), Finder,
Builder) != Node.decls_end();
}










AST_MATCHER_P(DeclStmt, hasSingleDecl, internal::Matcher<Decl>, InnerMatcher) {
if (Node.isSingleDecl()) {
const Decl *FoundDecl = Node.getSingleDecl();
return InnerMatcher.matches(*FoundDecl, Finder, Builder);
}
return false;
}









AST_MATCHER_P(
VarDecl, hasInitializer, internal::Matcher<Expr>,
InnerMatcher) {
const Expr *Initializer = Node.getAnyInitializer();
return (Initializer != nullptr &&
InnerMatcher.matches(*Initializer, Finder, Builder));
}











AST_MATCHER(VarDecl, isStaticLocal) {
return Node.isStaticLocal();
}












AST_MATCHER(VarDecl, hasLocalStorage) {
return Node.hasLocalStorage();
}











AST_MATCHER(VarDecl, hasGlobalStorage) {
return Node.hasGlobalStorage();
}













AST_MATCHER(VarDecl, hasAutomaticStorageDuration) {
return Node.getStorageDuration() == SD_Automatic;
}

















AST_MATCHER(VarDecl, hasStaticStorageDuration) {
return Node.getStorageDuration() == SD_Static;
}













AST_MATCHER(VarDecl, hasThreadStorageDuration) {
return Node.getStorageDuration() == SD_Thread;
}












AST_MATCHER(VarDecl, isExceptionVariable) {
return Node.isExceptionVariable();
}









AST_POLYMORPHIC_MATCHER_P(argumentCountIs,
AST_POLYMORPHIC_SUPPORTED_TYPES(
CallExpr, CXXConstructExpr,
CXXUnresolvedConstructExpr, ObjCMessageExpr),
unsigned, N) {
unsigned NumArgs = Node.getNumArgs();
if (!Finder->isTraversalIgnoringImplicitNodes())
return NumArgs == N;
while (NumArgs) {
if (!isa<CXXDefaultArgExpr>(Node.getArg(NumArgs - 1)))
break;
--NumArgs;
}
return NumArgs == N;
}









AST_POLYMORPHIC_MATCHER_P2(hasArgument,
AST_POLYMORPHIC_SUPPORTED_TYPES(
CallExpr, CXXConstructExpr,
CXXUnresolvedConstructExpr, ObjCMessageExpr),
unsigned, N, internal::Matcher<Expr>, InnerMatcher) {
if (N >= Node.getNumArgs())
return false;
const Expr *Arg = Node.getArg(N);
if (Finder->isTraversalIgnoringImplicitNodes() && isa<CXXDefaultArgExpr>(Arg))
return false;
return InnerMatcher.matches(*Arg->IgnoreParenImpCasts(), Finder, Builder);
}








AST_MATCHER_P2(InitListExpr, hasInit, unsigned, N,
ast_matchers::internal::Matcher<Expr>, InnerMatcher) {
return N < Node.getNumInits() &&
InnerMatcher.matches(*Node.getInit(N), Finder, Builder);
}












AST_MATCHER_P(DeclStmt, declCountIs, unsigned, N) {
return std::distance(Node.decl_begin(), Node.decl_end()) == (ptrdiff_t)N;
}




















AST_MATCHER_P2(DeclStmt, containsDeclaration, unsigned, N,
internal::Matcher<Decl>, InnerMatcher) {
const unsigned NumDecls = std::distance(Node.decl_begin(), Node.decl_end());
if (N >= NumDecls)
return false;
DeclStmt::const_decl_iterator Iterator = Node.decl_begin();
std::advance(Iterator, N);
return InnerMatcher.matches(**Iterator, Finder, Builder);
}














AST_MATCHER(CXXCatchStmt, isCatchAll) {
return Node.getExceptionDecl() == nullptr;
}














AST_MATCHER_P(CXXConstructorDecl, hasAnyConstructorInitializer,
internal::Matcher<CXXCtorInitializer>, InnerMatcher) {
auto MatchIt = matchesFirstInPointerRange(InnerMatcher, Node.init_begin(),
Node.init_end(), Finder, Builder);
if (MatchIt == Node.init_end())
return false;
return (*MatchIt)->isWritten() || !Finder->isTraversalIgnoringImplicitNodes();
}














AST_MATCHER_P(CXXCtorInitializer, forField,
internal::Matcher<FieldDecl>, InnerMatcher) {
const FieldDecl *NodeAsDecl = Node.getAnyMember();
return (NodeAsDecl != nullptr &&
InnerMatcher.matches(*NodeAsDecl, Finder, Builder));
}














AST_MATCHER_P(CXXCtorInitializer, withInitializer,
internal::Matcher<Expr>, InnerMatcher) {
const Expr* NodeAsExpr = Node.getInit();
return (NodeAsExpr != nullptr &&
InnerMatcher.matches(*NodeAsExpr, Finder, Builder));
}














AST_MATCHER(CXXCtorInitializer, isWritten) {
return Node.isWritten();
}

















AST_MATCHER(CXXCtorInitializer, isBaseInitializer) {
return Node.isBaseInitializer();
}

















AST_MATCHER(CXXCtorInitializer, isMemberInitializer) {
return Node.isMemberInitializer();
}




















AST_POLYMORPHIC_MATCHER_P(hasAnyArgument,
AST_POLYMORPHIC_SUPPORTED_TYPES(
CallExpr, CXXConstructExpr,
CXXUnresolvedConstructExpr, ObjCMessageExpr),
internal::Matcher<Expr>, InnerMatcher) {
for (const Expr *Arg : Node.arguments()) {
if (Finder->isTraversalIgnoringImplicitNodes() &&
isa<CXXDefaultArgExpr>(Arg))
break;
BoundNodesTreeBuilder Result(*Builder);
if (InnerMatcher.matches(*Arg, Finder, &Result)) {
*Builder = std::move(Result);
return true;
}
}
return false;
}












AST_MATCHER_P_OVERLOAD(LambdaExpr, hasAnyCapture, internal::Matcher<VarDecl>,
InnerMatcher, 0) {
for (const LambdaCapture &Capture : Node.captures()) {
if (Capture.capturesVariable()) {
BoundNodesTreeBuilder Result(*Builder);
if (InnerMatcher.matches(*Capture.getCapturedVar(), Finder, &Result)) {
*Builder = std::move(Result);
return true;
}
}
}
return false;
}













AST_MATCHER_P_OVERLOAD(LambdaExpr, hasAnyCapture,
internal::Matcher<CXXThisExpr>, InnerMatcher, 1) {
return llvm::any_of(Node.captures(), [](const LambdaCapture &LC) {
return LC.capturesThis();
});
}


AST_MATCHER(CXXConstructExpr, isListInitialization) {
return Node.isListInitialization();
}













AST_MATCHER(CXXConstructExpr, requiresZeroInitialization) {
return Node.requiresZeroInitialization();
}





















AST_POLYMORPHIC_MATCHER_P2(hasParameter,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl,
ObjCMethodDecl,
BlockDecl),
unsigned, N, internal::Matcher<ParmVarDecl>,
InnerMatcher) {
return (N < Node.parameters().size()
&& InnerMatcher.matches(*Node.parameters()[N], Finder, Builder));
}



















AST_POLYMORPHIC_MATCHER_P2(forEachArgumentWithParam,
AST_POLYMORPHIC_SUPPORTED_TYPES(CallExpr,
CXXConstructExpr),
internal::Matcher<Expr>, ArgMatcher,
internal::Matcher<ParmVarDecl>, ParamMatcher) {
BoundNodesTreeBuilder Result;



BoundNodesTreeBuilder Matches;
unsigned ArgIndex = cxxOperatorCallExpr(callee(cxxMethodDecl()))
.matches(Node, Finder, &Matches)
? 1
: 0;
int ParamIndex = 0;
bool Matched = false;
for (; ArgIndex < Node.getNumArgs(); ++ArgIndex) {
BoundNodesTreeBuilder ArgMatches(*Builder);
if (ArgMatcher.matches(*(Node.getArg(ArgIndex)->IgnoreParenCasts()),
Finder, &ArgMatches)) {
BoundNodesTreeBuilder ParamMatches(ArgMatches);
if (expr(anyOf(cxxConstructExpr(hasDeclaration(cxxConstructorDecl(
hasParameter(ParamIndex, ParamMatcher)))),
callExpr(callee(functionDecl(
hasParameter(ParamIndex, ParamMatcher))))))
.matches(Node, Finder, &ParamMatches)) {
Result.addMatch(ParamMatches);
Matched = true;
}
}
++ParamIndex;
}
*Builder = std::move(Result);
return Matched;
}


























AST_POLYMORPHIC_MATCHER_P2(forEachArgumentWithParamType,
AST_POLYMORPHIC_SUPPORTED_TYPES(CallExpr,
CXXConstructExpr),
internal::Matcher<Expr>, ArgMatcher,
internal::Matcher<QualType>, ParamMatcher) {
BoundNodesTreeBuilder Result;



BoundNodesTreeBuilder Matches;
unsigned ArgIndex = cxxOperatorCallExpr(callee(cxxMethodDecl()))
.matches(Node, Finder, &Matches)
? 1
: 0;

const FunctionProtoType *FProto = nullptr;

if (const auto *Call = dyn_cast<CallExpr>(&Node)) {
if (const auto *Value =
dyn_cast_or_null<ValueDecl>(Call->getCalleeDecl())) {
QualType QT = Value->getType().getCanonicalType();



if (QT->isFunctionPointerType())
FProto = QT->getPointeeType()->getAs<FunctionProtoType>();

if (QT->isMemberFunctionPointerType()) {
const auto *MP = QT->getAs<MemberPointerType>();
assert(MP && "Must be member-pointer if its a memberfunctionpointer");
FProto = MP->getPointeeType()->getAs<FunctionProtoType>();
assert(FProto &&
"The call must have happened through a member function "
"pointer");
}
}
}

int ParamIndex = 0;
bool Matched = false;
unsigned NumArgs = Node.getNumArgs();
if (FProto && FProto->isVariadic())
NumArgs = std::min(NumArgs, FProto->getNumParams());

for (; ArgIndex < NumArgs; ++ArgIndex, ++ParamIndex) {
BoundNodesTreeBuilder ArgMatches(*Builder);
if (ArgMatcher.matches(*(Node.getArg(ArgIndex)->IgnoreParenCasts()), Finder,
&ArgMatches)) {
BoundNodesTreeBuilder ParamMatches(ArgMatches);



if (FProto) {
QualType ParamType = FProto->getParamType(ParamIndex);
if (ParamMatcher.matches(ParamType, Finder, &ParamMatches)) {
Result.addMatch(ParamMatches);
Matched = true;
continue;
}
}
if (expr(anyOf(cxxConstructExpr(hasDeclaration(cxxConstructorDecl(
hasParameter(ParamIndex, hasType(ParamMatcher))))),
callExpr(callee(functionDecl(
hasParameter(ParamIndex, hasType(ParamMatcher)))))))
.matches(Node, Finder, &ParamMatches)) {
Result.addMatch(ParamMatches);
Matched = true;
continue;
}
}
}
*Builder = std::move(Result);
return Matched;
}
















AST_MATCHER_P(ParmVarDecl, isAtPosition, unsigned, N) {
const clang::DeclContext *Context = Node.getParentFunctionOrMethod();

if (const auto *Decl = dyn_cast_or_null<FunctionDecl>(Context))
return N < Decl->param_size() && Decl->getParamDecl(N) == &Node;
if (const auto *Decl = dyn_cast_or_null<BlockDecl>(Context))
return N < Decl->param_size() && Decl->getParamDecl(N) == &Node;
if (const auto *Decl = dyn_cast_or_null<ObjCMethodDecl>(Context))
return N < Decl->param_size() && Decl->getParamDecl(N) == &Node;

return false;
}
































AST_POLYMORPHIC_MATCHER_P(hasAnyParameter,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl,
ObjCMethodDecl,
BlockDecl),
internal::Matcher<ParmVarDecl>,
InnerMatcher) {
return matchesFirstInPointerRange(InnerMatcher, Node.param_begin(),
Node.param_end(), Finder,
Builder) != Node.param_end();
}


















AST_POLYMORPHIC_MATCHER_P(parameterCountIs,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl,
FunctionProtoType),
unsigned, N) {
return Node.getNumParams() == N;
}















AST_MATCHER(FunctionDecl, isNoReturn) { return Node.isNoReturn(); }









AST_MATCHER_P(FunctionDecl, returns,
internal::Matcher<QualType>, InnerMatcher) {
return InnerMatcher.matches(Node.getReturnType(), Finder, Builder);
}
















AST_POLYMORPHIC_MATCHER(isExternC, AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl,
VarDecl)) {
return Node.isExternC();
}















AST_POLYMORPHIC_MATCHER(isStaticStorageClass,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl,
VarDecl)) {
return Node.getStorageClass() == SC_Static;
}










AST_MATCHER(FunctionDecl, isDeleted) {
return Node.isDeleted();
}










AST_MATCHER(FunctionDecl, isDefaulted) {
return Node.isDefaulted();
}










AST_MATCHER(FunctionDecl, isWeak) { return Node.isWeak(); }
















AST_POLYMORPHIC_MATCHER(hasDynamicExceptionSpec,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl,
FunctionProtoType)) {
if (const FunctionProtoType *FnTy = internal::getFunctionProtoType(Node))
return FnTy->hasDynamicExceptionSpec();
return false;
}













AST_POLYMORPHIC_MATCHER(isNoThrow,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl,
FunctionProtoType)) {
const FunctionProtoType *FnTy = internal::getFunctionProtoType(Node);




if (!FnTy)
return false;


if (isUnresolvedExceptionSpec(FnTy->getExceptionSpecType()))
return true;

return FnTy->isNothrow();
}
















AST_POLYMORPHIC_MATCHER(isConstexpr,
AST_POLYMORPHIC_SUPPORTED_TYPES(VarDecl,
FunctionDecl,
IfStmt)) {
return Node.isConstexpr();
}






















AST_POLYMORPHIC_MATCHER_P(hasInitStatement,
AST_POLYMORPHIC_SUPPORTED_TYPES(IfStmt, SwitchStmt,
CXXForRangeStmt),
internal::Matcher<Stmt>, InnerMatcher) {
const Stmt *Init = Node.getInit();
return Init != nullptr && InnerMatcher.matches(*Init, Finder, Builder);
}








AST_POLYMORPHIC_MATCHER_P(
hasCondition,
AST_POLYMORPHIC_SUPPORTED_TYPES(IfStmt, ForStmt, WhileStmt, DoStmt,
SwitchStmt, AbstractConditionalOperator),
internal::Matcher<Expr>, InnerMatcher) {
const Expr *const Condition = Node.getCond();
return (Condition != nullptr &&
InnerMatcher.matches(*Condition, Finder, Builder));
}








AST_MATCHER_P(IfStmt, hasThen, internal::Matcher<Stmt>, InnerMatcher) {
const Stmt *const Then = Node.getThen();
return (Then != nullptr && InnerMatcher.matches(*Then, Finder, Builder));
}








AST_MATCHER_P(IfStmt, hasElse, internal::Matcher<Stmt>, InnerMatcher) {
const Stmt *const Else = Node.getElse();
return (Else != nullptr && InnerMatcher.matches(*Else, Finder, Builder));
}






















AST_POLYMORPHIC_MATCHER_P(equalsBoundNode,
AST_POLYMORPHIC_SUPPORTED_TYPES(Stmt, Decl, Type,
QualType),
std::string, ID) {






internal::NotEqualsBoundNodePredicate Predicate;
Predicate.ID = ID;
Predicate.Node = DynTypedNode::create(Node);
return Builder->removeBindings(Predicate);
}









AST_MATCHER_P(IfStmt, hasConditionVariableStatement,
internal::Matcher<DeclStmt>, InnerMatcher) {
const DeclStmt* const DeclarationStatement =
Node.getConditionVariableDeclStmt();
return DeclarationStatement != nullptr &&
InnerMatcher.matches(*DeclarationStatement, Finder, Builder);
}










AST_MATCHER_P(ArraySubscriptExpr, hasIndex,
internal::Matcher<Expr>, InnerMatcher) {
if (const Expr* Expression = Node.getIdx())
return InnerMatcher.matches(*Expression, Finder, Builder);
return false;
}











AST_MATCHER_P(ArraySubscriptExpr, hasBase,
internal::Matcher<Expr>, InnerMatcher) {
if (const Expr* Expression = Node.getBase())
return InnerMatcher.matches(*Expression, Finder, Builder);
return false;
}


























AST_POLYMORPHIC_MATCHER_P(hasBody,
AST_POLYMORPHIC_SUPPORTED_TYPES(DoStmt, ForStmt,
WhileStmt,
CXXForRangeStmt,
FunctionDecl),
internal::Matcher<Stmt>, InnerMatcher) {
if (Finder->isTraversalIgnoringImplicitNodes() && isDefaultedHelper(&Node))
return false;
const Stmt *const Statement = internal::GetBodyMatcher<NodeType>::get(Node);
return (Statement != nullptr &&
InnerMatcher.matches(*Statement, Finder, Builder));
}

















AST_MATCHER_P(FunctionDecl, hasAnyBody,
internal::Matcher<Stmt>, InnerMatcher) {
const Stmt *const Statement = Node.getBody();
return (Statement != nullptr &&
InnerMatcher.matches(*Statement, Finder, Builder));
}













AST_POLYMORPHIC_MATCHER_P(hasAnySubstatement,
AST_POLYMORPHIC_SUPPORTED_TYPES(CompoundStmt,
StmtExpr),
internal::Matcher<Stmt>, InnerMatcher) {
const CompoundStmt *CS = CompoundStmtMatcher<NodeType>::get(Node);
return CS && matchesFirstInPointerRange(InnerMatcher, CS->body_begin(),
CS->body_end(), Finder,
Builder) != CS->body_end();
}











AST_MATCHER_P(CompoundStmt, statementCountIs, unsigned, N) {
return Node.size() == N;
}


























template <typename ValueT>
internal::PolymorphicMatcher<internal::ValueEqualsMatcher,
void(internal::AllNodeBaseTypes), ValueT>
equals(const ValueT &Value) {
return internal::PolymorphicMatcher<internal::ValueEqualsMatcher,
void(internal::AllNodeBaseTypes), ValueT>(
Value);
}

AST_POLYMORPHIC_MATCHER_P_OVERLOAD(equals,
AST_POLYMORPHIC_SUPPORTED_TYPES(CharacterLiteral,
CXXBoolLiteralExpr,
IntegerLiteral),
bool, Value, 0) {
return internal::ValueEqualsMatcher<NodeType, ParamT>(Value)
.matchesNode(Node);
}

AST_POLYMORPHIC_MATCHER_P_OVERLOAD(equals,
AST_POLYMORPHIC_SUPPORTED_TYPES(CharacterLiteral,
CXXBoolLiteralExpr,
IntegerLiteral),
unsigned, Value, 1) {
return internal::ValueEqualsMatcher<NodeType, ParamT>(Value)
.matchesNode(Node);
}

AST_POLYMORPHIC_MATCHER_P_OVERLOAD(equals,
AST_POLYMORPHIC_SUPPORTED_TYPES(CharacterLiteral,
CXXBoolLiteralExpr,
FloatingLiteral,
IntegerLiteral),
double, Value, 2) {
return internal::ValueEqualsMatcher<NodeType, ParamT>(Value)
.matchesNode(Node);
}








AST_POLYMORPHIC_MATCHER_P(
hasOperatorName,
AST_POLYMORPHIC_SUPPORTED_TYPES(BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator, UnaryOperator),
std::string, Name) {
if (Optional<StringRef> OpName = internal::getOpName(Node))
return *OpName == Name;
return false;
}







extern const internal::VariadicFunction<
internal::PolymorphicMatcher<internal::HasAnyOperatorNameMatcher,
AST_POLYMORPHIC_SUPPORTED_TYPES(
BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator, UnaryOperator),
std::vector<std::string>>,
StringRef, internal::hasAnyOperatorNameFunc>
hasAnyOperatorName;















AST_POLYMORPHIC_MATCHER(
isAssignmentOperator,
AST_POLYMORPHIC_SUPPORTED_TYPES(BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator)) {
return Node.isAssignmentOp();
}















AST_POLYMORPHIC_MATCHER(
isComparisonOperator,
AST_POLYMORPHIC_SUPPORTED_TYPES(BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator)) {
return Node.isComparisonOp();
}







AST_POLYMORPHIC_MATCHER_P(hasLHS,
AST_POLYMORPHIC_SUPPORTED_TYPES(
BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator, ArraySubscriptExpr),
internal::Matcher<Expr>, InnerMatcher) {
const Expr *LeftHandSide = internal::getLHS(Node);
return (LeftHandSide != nullptr &&
InnerMatcher.matches(*LeftHandSide, Finder, Builder));
}







AST_POLYMORPHIC_MATCHER_P(hasRHS,
AST_POLYMORPHIC_SUPPORTED_TYPES(
BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator, ArraySubscriptExpr),
internal::Matcher<Expr>, InnerMatcher) {
const Expr *RightHandSide = internal::getRHS(Node);
return (RightHandSide != nullptr &&
InnerMatcher.matches(*RightHandSide, Finder, Builder));
}



AST_POLYMORPHIC_MATCHER_P(
hasEitherOperand,
AST_POLYMORPHIC_SUPPORTED_TYPES(BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator),
internal::Matcher<Expr>, InnerMatcher) {
return internal::VariadicDynCastAllOfMatcher<Stmt, NodeType>()(
anyOf(hasLHS(InnerMatcher), hasRHS(InnerMatcher)))
.matches(Node, Finder, Builder);
}











AST_POLYMORPHIC_MATCHER_P2(
hasOperands,
AST_POLYMORPHIC_SUPPORTED_TYPES(BinaryOperator, CXXOperatorCallExpr,
CXXRewrittenBinaryOperator),
internal::Matcher<Expr>, Matcher1, internal::Matcher<Expr>, Matcher2) {
return internal::VariadicDynCastAllOfMatcher<Stmt, NodeType>()(
anyOf(allOf(hasLHS(Matcher1), hasRHS(Matcher2)),
allOf(hasLHS(Matcher2), hasRHS(Matcher1))))
.matches(Node, Finder, Builder);
}








AST_POLYMORPHIC_MATCHER_P(hasUnaryOperand,
AST_POLYMORPHIC_SUPPORTED_TYPES(UnaryOperator,
CXXOperatorCallExpr),
internal::Matcher<Expr>, InnerMatcher) {
const Expr *const Operand = internal::getSubExpr(Node);
return (Operand != nullptr &&
InnerMatcher.matches(*Operand, Finder, Builder));
}
















AST_POLYMORPHIC_MATCHER_P(hasSourceExpression,
AST_POLYMORPHIC_SUPPORTED_TYPES(CastExpr,
OpaqueValueExpr),
internal::Matcher<Expr>, InnerMatcher) {
const Expr *const SubExpression =
internal::GetSourceExpressionMatcher<NodeType>::get(Node);
return (SubExpression != nullptr &&
InnerMatcher.matches(*SubExpression, Finder, Builder));
}











AST_MATCHER_P(CastExpr, hasCastKind, CastKind, Kind) {
return Node.getCastKind() == Kind;
}





AST_MATCHER_P(ExplicitCastExpr, hasDestinationType,
internal::Matcher<QualType>, InnerMatcher) {
const QualType NodeType = Node.getTypeAsWritten();
return InnerMatcher.matches(NodeType, Finder, Builder);
}





AST_MATCHER_P(ImplicitCastExpr, hasImplicitDestinationType,
internal::Matcher<QualType>, InnerMatcher) {
return InnerMatcher.matches(Node.getType(), Finder, Builder);
}










AST_MATCHER(TagDecl, isStruct) {
return Node.isStruct();
}










AST_MATCHER(TagDecl, isUnion) {
return Node.isUnion();
}










AST_MATCHER(TagDecl, isClass) {
return Node.isClass();
}










AST_MATCHER(TagDecl, isEnum) {
return Node.isEnum();
}












AST_MATCHER_P(AbstractConditionalOperator, hasTrueExpression,
internal::Matcher<Expr>, InnerMatcher) {
const Expr *Expression = Node.getTrueExpr();
return (Expression != nullptr &&
InnerMatcher.matches(*Expression, Finder, Builder));
}









AST_MATCHER_P(AbstractConditionalOperator, hasFalseExpression,
internal::Matcher<Expr>, InnerMatcher) {
const Expr *Expression = Node.getFalseExpr();
return (Expression != nullptr &&
InnerMatcher.matches(*Expression, Finder, Builder));
}





















AST_POLYMORPHIC_MATCHER(isDefinition,
AST_POLYMORPHIC_SUPPORTED_TYPES(TagDecl, VarDecl,
ObjCMethodDecl,
FunctionDecl)) {
return Node.isThisDeclarationADefinition();
}











AST_MATCHER(FunctionDecl, isVariadic) {
return Node.isVariadic();
}


















AST_MATCHER_P(CXXMethodDecl, ofClass,
internal::Matcher<CXXRecordDecl>, InnerMatcher) {

ASTChildrenNotSpelledInSourceScope RAII(Finder, false);

const CXXRecordDecl *Parent = Node.getParent();
return (Parent != nullptr &&
InnerMatcher.matches(*Parent, Finder, Builder));
}

























AST_MATCHER_P(CXXMethodDecl, forEachOverridden,
internal::Matcher<CXXMethodDecl>, InnerMatcher) {
BoundNodesTreeBuilder Result;
bool Matched = false;
for (const auto *Overridden : Node.overridden_methods()) {
BoundNodesTreeBuilder OverriddenBuilder(*Builder);
const bool OverriddenMatched =
InnerMatcher.matches(*Overridden, Finder, &OverriddenBuilder);
if (OverriddenMatched) {
Matched = true;
Result.addMatch(OverriddenBuilder);
}
}
*Builder = std::move(Result);
return Matched;
}




















AST_POLYMORPHIC_MATCHER(isVirtual,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXMethodDecl,
CXXBaseSpecifier)) {
return Node.isVirtual();
}















AST_MATCHER(CXXMethodDecl, isVirtualAsWritten) {
return Node.isVirtualAsWritten();
}
















AST_POLYMORPHIC_MATCHER(isFinal,
AST_POLYMORPHIC_SUPPORTED_TYPES(CXXRecordDecl,
CXXMethodDecl)) {
return Node.template hasAttr<FinalAttr>();
}











AST_MATCHER(CXXMethodDecl, isPure) {
return Node.isPure();
}












AST_MATCHER(CXXMethodDecl, isConst) {
return Node.isConst();
}














AST_MATCHER(CXXMethodDecl, isCopyAssignmentOperator) {
return Node.isCopyAssignmentOperator();
}














AST_MATCHER(CXXMethodDecl, isMoveAssignmentOperator) {
return Node.isMoveAssignmentOperator();
}















AST_MATCHER(CXXMethodDecl, isOverride) {
return Node.size_overridden_methods() > 0 || Node.hasAttr<OverrideAttr>();
}












AST_MATCHER(CXXMethodDecl, isUserProvided) {
return Node.isUserProvided();
}

























AST_POLYMORPHIC_MATCHER(
isArrow, AST_POLYMORPHIC_SUPPORTED_TYPES(MemberExpr, UnresolvedMemberExpr,
CXXDependentScopeMemberExpr)) {
return Node.isArrow();
}











AST_MATCHER(QualType, isInteger) {
return Node->isIntegerType();
}











AST_MATCHER(QualType, isUnsignedInteger) {
return Node->isUnsignedIntegerType();
}











AST_MATCHER(QualType, isSignedInteger) {
return Node->isSignedIntegerType();
}











AST_MATCHER(QualType, isAnyCharacter) {
return Node->isAnyCharacterType();
}

















AST_MATCHER(QualType, isAnyPointer) {
return Node->isAnyPointerType();
}
















AST_MATCHER(QualType, isConstQualified) {
return Node.isConstQualified();
}
















AST_MATCHER(QualType, isVolatileQualified) {
return Node.isVolatileQualified();
}














AST_MATCHER(QualType, hasLocalQualifiers) {
return Node.hasLocalQualifiers();
}













AST_MATCHER_P(MemberExpr, member,
internal::Matcher<ValueDecl>, InnerMatcher) {
return InnerMatcher.matches(*Node.getMemberDecl(), Finder, Builder);
}

















AST_POLYMORPHIC_MATCHER_P(
hasObjectExpression,
AST_POLYMORPHIC_SUPPORTED_TYPES(MemberExpr, UnresolvedMemberExpr,
CXXDependentScopeMemberExpr),
internal::Matcher<Expr>, InnerMatcher) {
if (const auto *E = dyn_cast<UnresolvedMemberExpr>(&Node))
if (E->isImplicitAccess())
return false;
if (const auto *E = dyn_cast<CXXDependentScopeMemberExpr>(&Node))
if (E->isImplicitAccess())
return false;
return InnerMatcher.matches(*Node.getBase(), Finder, Builder);
}










AST_MATCHER_P(BaseUsingDecl, hasAnyUsingShadowDecl,
internal::Matcher<UsingShadowDecl>, InnerMatcher) {
return matchesFirstInPointerRange(InnerMatcher, Node.shadow_begin(),
Node.shadow_end(), Finder,
Builder) != Node.shadow_end();
}













AST_MATCHER_P(UsingShadowDecl, hasTargetDecl,
internal::Matcher<NamedDecl>, InnerMatcher) {
return InnerMatcher.matches(*Node.getTargetDecl(), Finder, Builder);
}




























AST_POLYMORPHIC_MATCHER(isTemplateInstantiation,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl, VarDecl,
CXXRecordDecl)) {
return (Node.getTemplateSpecializationKind() == TSK_ImplicitInstantiation ||
Node.getTemplateSpecializationKind() ==
TSK_ExplicitInstantiationDefinition ||
Node.getTemplateSpecializationKind() ==
TSK_ExplicitInstantiationDeclaration);
}












AST_MATCHER_FUNCTION(internal::Matcher<Decl>, isInstantiated) {
auto IsInstantiation = decl(anyOf(cxxRecordDecl(isTemplateInstantiation()),
functionDecl(isTemplateInstantiation())));
return decl(anyOf(IsInstantiation, hasAncestor(IsInstantiation)));
}















AST_MATCHER_FUNCTION(internal::Matcher<Stmt>, isInTemplateInstantiation) {
return stmt(
hasAncestor(decl(anyOf(cxxRecordDecl(isTemplateInstantiation()),
functionDecl(isTemplateInstantiation())))));
}













AST_POLYMORPHIC_MATCHER(isExplicitTemplateSpecialization,
AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionDecl, VarDecl,
CXXRecordDecl)) {
return (Node.getTemplateSpecializationKind() == TSK_ExplicitSpecialization);
}



AST_MATCHER_FUNCTION_P_OVERLOAD(internal::BindableMatcher<TypeLoc>, loc,
internal::Matcher<QualType>, InnerMatcher, 0) {
return internal::BindableMatcher<TypeLoc>(
new internal::TypeLocTypeMatcher(InnerMatcher));
}









AST_MATCHER(Type, booleanType) {
return Node.isBooleanType();
}









AST_MATCHER(Type, voidType) {
return Node.isVoidType();
}

template <typename NodeType>
using AstTypeMatcher = internal::VariadicDynCastAllOfMatcher<Type, NodeType>;













extern const AstTypeMatcher<BuiltinType> builtinType;











extern const AstTypeMatcher<ArrayType> arrayType;









extern const AstTypeMatcher<ComplexType> complexType;










AST_MATCHER(Type, realFloatingPointType) {
return Node.isRealFloatingType();
}














AST_TYPELOC_TRAVERSE_MATCHER_DECL(hasElementType, getElement,
AST_POLYMORPHIC_SUPPORTED_TYPES(ArrayType,
ComplexType));













extern const AstTypeMatcher<ConstantArrayType> constantArrayType;
















AST_POLYMORPHIC_MATCHER_P(hasSize,
AST_POLYMORPHIC_SUPPORTED_TYPES(ConstantArrayType,
StringLiteral),
unsigned, N) {
return internal::HasSizeMatcher<NodeType>::hasSize(Node, N);
}












extern const AstTypeMatcher<DependentSizedArrayType> dependentSizedArrayType;











extern const AstTypeMatcher<IncompleteArrayType> incompleteArrayType;














extern const AstTypeMatcher<VariableArrayType> variableArrayType;













AST_MATCHER_P(VariableArrayType, hasSizeExpr,
internal::Matcher<Expr>, InnerMatcher) {
return InnerMatcher.matches(*Node.getSizeExpr(), Finder, Builder);
}









extern const AstTypeMatcher<AtomicType> atomicType;












AST_TYPELOC_TRAVERSE_MATCHER_DECL(hasValueType, getValue,
AST_POLYMORPHIC_SUPPORTED_TYPES(AtomicType));











extern const AstTypeMatcher<AutoType> autoType;











extern const AstTypeMatcher<DecltypeType> decltypeType;















AST_TYPE_TRAVERSE_MATCHER(hasDeducedType, getDeducedType,
AST_POLYMORPHIC_SUPPORTED_TYPES(AutoType));












AST_TYPE_TRAVERSE_MATCHER(hasUnderlyingType, getUnderlyingType,
AST_POLYMORPHIC_SUPPORTED_TYPES(DecltypeType));










extern const AstTypeMatcher<FunctionType> functionType;











extern const AstTypeMatcher<FunctionProtoType> functionProtoType;











extern const AstTypeMatcher<ParenType> parenType;













AST_TYPE_TRAVERSE_MATCHER(innerType, getInnerType,
AST_POLYMORPHIC_SUPPORTED_TYPES(ParenType));





extern const AstTypeMatcher<BlockPointerType> blockPointerType;









extern const AstTypeMatcher<MemberPointerType> memberPointerType;
















extern const AstTypeMatcher<PointerType> pointerType;














extern const AstTypeMatcher<ObjCObjectPointerType> objcObjectPointerType;















extern const AstTypeMatcher<ReferenceType> referenceType;
















extern const AstTypeMatcher<LValueReferenceType> lValueReferenceType;
















extern const AstTypeMatcher<RValueReferenceType> rValueReferenceType;















AST_TYPELOC_TRAVERSE_MATCHER_DECL(
pointee, getPointee,
AST_POLYMORPHIC_SUPPORTED_TYPES(BlockPointerType, MemberPointerType,
PointerType, ReferenceType));









extern const AstTypeMatcher<TypedefType> typedefType;














extern const AstTypeMatcher<EnumType> enumType;














extern const AstTypeMatcher<TemplateSpecializationType>
templateSpecializationType;













extern const AstTypeMatcher<DeducedTemplateSpecializationType>
deducedTemplateSpecializationType;









extern const AstTypeMatcher<UnaryTransformType> unaryTransformType;














extern const AstTypeMatcher<RecordType> recordType;














extern const AstTypeMatcher<TagType> tagType;



















extern const AstTypeMatcher<ElaboratedType> elaboratedType;
















AST_MATCHER_P(ElaboratedType, hasQualifier,
internal::Matcher<NestedNameSpecifier>, InnerMatcher) {
if (const NestedNameSpecifier *Qualifier = Node.getQualifier())
return InnerMatcher.matches(*Qualifier, Finder, Builder);

return false;
}
















AST_MATCHER_P(ElaboratedType, namesType, internal::Matcher<QualType>,
InnerMatcher) {
return InnerMatcher.matches(Node.getNamedType(), Finder, Builder);
}













extern const AstTypeMatcher<SubstTemplateTypeParmType>
substTemplateTypeParmType;













AST_TYPE_TRAVERSE_MATCHER(
hasReplacementType, getReplacementType,
AST_POLYMORPHIC_SUPPORTED_TYPES(SubstTemplateTypeParmType));








extern const AstTypeMatcher<TemplateTypeParmType> templateTypeParmType;











extern const AstTypeMatcher<InjectedClassNameType> injectedClassNameType;











extern const AstTypeMatcher<DecayedType> decayedType;


AST_MATCHER_P(DecayedType, hasDecayedType, internal::Matcher<QualType>,
InnerType) {
return InnerType.matches(Node.getDecayedType(), Finder, Builder);
}















AST_MATCHER_P(Decl, hasDeclContext, internal::Matcher<Decl>, InnerMatcher) {
const DeclContext *DC = Node.getDeclContext();
if (!DC) return false;
return InnerMatcher.matches(*Decl::castFromDeclContext(DC), Finder, Builder);
}














extern const internal::VariadicAllOfMatcher<NestedNameSpecifier>
nestedNameSpecifier;


extern const internal::VariadicAllOfMatcher<NestedNameSpecifierLoc>
nestedNameSpecifierLoc;



AST_MATCHER_FUNCTION_P_OVERLOAD(
internal::BindableMatcher<NestedNameSpecifierLoc>, loc,
internal::Matcher<NestedNameSpecifier>, InnerMatcher, 1) {
return internal::BindableMatcher<NestedNameSpecifierLoc>(
new internal::LocMatcher<NestedNameSpecifierLoc, NestedNameSpecifier>(
InnerMatcher));
}













AST_MATCHER_P(NestedNameSpecifier, specifiesType,
internal::Matcher<QualType>, InnerMatcher) {
if (!Node.getAsType())
return false;
return InnerMatcher.matches(QualType(Node.getAsType(), 0), Finder, Builder);
}












AST_MATCHER_P(NestedNameSpecifierLoc, specifiesTypeLoc,
internal::Matcher<TypeLoc>, InnerMatcher) {
return Node && Node.getNestedNameSpecifier()->getAsType() &&
InnerMatcher.matches(Node.getTypeLoc(), Finder, Builder);
}










AST_MATCHER_P_OVERLOAD(NestedNameSpecifier, hasPrefix,
internal::Matcher<NestedNameSpecifier>, InnerMatcher,
0) {
const NestedNameSpecifier *NextNode = Node.getPrefix();
if (!NextNode)
return false;
return InnerMatcher.matches(*NextNode, Finder, Builder);
}










AST_MATCHER_P_OVERLOAD(NestedNameSpecifierLoc, hasPrefix,
internal::Matcher<NestedNameSpecifierLoc>, InnerMatcher,
1) {
NestedNameSpecifierLoc NextNode = Node.getPrefix();
if (!NextNode)
return false;
return InnerMatcher.matches(NextNode, Finder, Builder);
}











AST_MATCHER_P(NestedNameSpecifier, specifiesNamespace,
internal::Matcher<NamespaceDecl>, InnerMatcher) {
if (!Node.getAsNamespace())
return false;
return InnerMatcher.matches(*Node.getAsNamespace(), Finder, Builder);
}








AST_MATCHER_P_OVERLOAD(Decl, equalsNode, const Decl*, Other, 0) {
return &Node == Other;
}



AST_MATCHER_P_OVERLOAD(Stmt, equalsNode, const Stmt*, Other, 1) {
return &Node == Other;
}



AST_MATCHER_P_OVERLOAD(Type, equalsNode, const Type*, Other, 2) {
return &Node == Other;
}














AST_MATCHER_P(SwitchStmt, forEachSwitchCase, internal::Matcher<SwitchCase>,
InnerMatcher) {
BoundNodesTreeBuilder Result;




bool Matched = false;
for (const SwitchCase *SC = Node.getSwitchCaseList(); SC;
SC = SC->getNextSwitchCase()) {
BoundNodesTreeBuilder CaseBuilder(*Builder);
bool CaseMatched = InnerMatcher.matches(*SC, Finder, &CaseBuilder);
if (CaseMatched) {
Matched = true;
Result.addMatch(CaseBuilder);
}
}
*Builder = std::move(Result);
return Matched;
}











AST_MATCHER_P(CXXConstructorDecl, forEachConstructorInitializer,
internal::Matcher<CXXCtorInitializer>, InnerMatcher) {
BoundNodesTreeBuilder Result;
bool Matched = false;
for (const auto *I : Node.inits()) {
if (Finder->isTraversalIgnoringImplicitNodes() && !I->isWritten())
continue;
BoundNodesTreeBuilder InitBuilder(*Builder);
if (InnerMatcher.matches(*I, Finder, &InitBuilder)) {
Matched = true;
Result.addMatch(InitBuilder);
}
}
*Builder = std::move(Result);
return Matched;
}












AST_MATCHER(CXXConstructorDecl, isCopyConstructor) {
return Node.isCopyConstructor();
}












AST_MATCHER(CXXConstructorDecl, isMoveConstructor) {
return Node.isMoveConstructor();
}












AST_MATCHER(CXXConstructorDecl, isDefaultConstructor) {
return Node.isDefaultConstructor();
}














AST_MATCHER(CXXConstructorDecl, isDelegatingConstructor) {
return Node.isDelegatingConstructor();
}























AST_POLYMORPHIC_MATCHER(isExplicit, AST_POLYMORPHIC_SUPPORTED_TYPES(
CXXConstructorDecl, CXXConversionDecl,
CXXDeductionGuideDecl)) {
return Node.isExplicit();
}






















AST_MATCHER_P(FunctionDecl, hasExplicitSpecifier, internal::Matcher<Expr>,
InnerMatcher) {
ExplicitSpecifier ES = ExplicitSpecifier::getFromDecl(&Node);
if (!ES.getExpr())
return false;

ASTChildrenNotSpelledInSourceScope RAII(Finder, false);

return InnerMatcher.matches(*ES.getExpr(), Finder, Builder);
}














AST_POLYMORPHIC_MATCHER(isInline,
AST_POLYMORPHIC_SUPPORTED_TYPES(NamespaceDecl,
FunctionDecl)) {


if (const auto *FD = dyn_cast<FunctionDecl>(&Node))
return FD->isInlineSpecified();
else if (const auto *NSD = dyn_cast<NamespaceDecl>(&Node))
return NSD->isInline();
llvm_unreachable("Not a valid polymorphic type");
}










AST_MATCHER(NamespaceDecl, isAnonymous) {
return Node.isAnonymousNamespace();
}






















AST_MATCHER(Decl, isInStdNamespace) { return Node.isInStdNamespace(); }










AST_MATCHER_P(CaseStmt, hasCaseConstant, internal::Matcher<Expr>,
InnerMatcher) {
if (Node.getRHS())
return false;

return InnerMatcher.matches(*Node.getLHS(), Finder, Builder);
}










AST_MATCHER_P(Decl, hasAttr, attr::Kind, AttrKind) {
for (const auto *Attr : Node.attrs()) {
if (Attr->getKind() == AttrKind)
return true;
}
return false;
}











AST_MATCHER_P(ReturnStmt, hasReturnValue, internal::Matcher<Expr>,
InnerMatcher) {
if (const auto *RetValue = Node.getRetValue())
return InnerMatcher.matches(*RetValue, Finder, Builder);
return false;
}







extern const internal::VariadicDynCastAllOfMatcher<Stmt, CUDAKernelCallExpr>
cudaKernelCallExpr;
















AST_MATCHER_FUNCTION(internal::Matcher<Expr>, nullPointerConstant) {
return anyOf(
gnuNullExpr(), cxxNullPtrLiteralExpr(),
integerLiteral(equals(0), hasParent(expr(hasType(pointerType())))));
}



















AST_MATCHER_P(BindingDecl, forDecomposition, internal::Matcher<ValueDecl>,
InnerMatcher) {
if (const ValueDecl *VD = Node.getDecomposedDecl())
return InnerMatcher.matches(*VD, Finder, Builder);
return false;
}



















AST_MATCHER_P2(DecompositionDecl, hasBinding, unsigned, N,
internal::Matcher<BindingDecl>, InnerMatcher) {
if (Node.bindings().size() <= N)
return false;
return InnerMatcher.matches(*Node.bindings()[N], Finder, Builder);
}


















AST_MATCHER_P(DecompositionDecl, hasAnyBinding, internal::Matcher<BindingDecl>,
InnerMatcher) {
return llvm::any_of(Node.bindings(), [&](const auto *Binding) {
return InnerMatcher.matches(*Binding, Finder, Builder);
});
}


















AST_MATCHER_P(Stmt, forFunction, internal::Matcher<FunctionDecl>,
InnerMatcher) {
const auto &Parents = Finder->getASTContext().getParents(Node);

llvm::SmallVector<DynTypedNode, 8> Stack(Parents.begin(), Parents.end());
while (!Stack.empty()) {
const auto &CurNode = Stack.back();
Stack.pop_back();
if (const auto *FuncDeclNode = CurNode.get<FunctionDecl>()) {
if (InnerMatcher.matches(*FuncDeclNode, Finder, Builder)) {
return true;
}
} else if (const auto *LambdaExprNode = CurNode.get<LambdaExpr>()) {
if (InnerMatcher.matches(*LambdaExprNode->getCallOperator(), Finder,
Builder)) {
return true;
}
} else {
for (const auto &Parent : Finder->getASTContext().getParents(CurNode))
Stack.push_back(Parent);
}
}
return false;
}




























AST_MATCHER_P(Stmt, forCallable, internal::Matcher<Decl>, InnerMatcher) {
const auto &Parents = Finder->getASTContext().getParents(Node);

llvm::SmallVector<DynTypedNode, 8> Stack(Parents.begin(), Parents.end());
while (!Stack.empty()) {
const auto &CurNode = Stack.back();
Stack.pop_back();
if (const auto *FuncDeclNode = CurNode.get<FunctionDecl>()) {
if (InnerMatcher.matches(*FuncDeclNode, Finder, Builder)) {
return true;
}
} else if (const auto *LambdaExprNode = CurNode.get<LambdaExpr>()) {
if (InnerMatcher.matches(*LambdaExprNode->getCallOperator(), Finder,
Builder)) {
return true;
}
} else if (const auto *ObjCMethodDeclNode = CurNode.get<ObjCMethodDecl>()) {
if (InnerMatcher.matches(*ObjCMethodDeclNode, Finder, Builder)) {
return true;
}
} else if (const auto *BlockDeclNode = CurNode.get<BlockDecl>()) {
if (InnerMatcher.matches(*BlockDeclNode, Finder, Builder)) {
return true;
}
} else {
for (const auto &Parent : Finder->getASTContext().getParents(CurNode))
Stack.push_back(Parent);
}
}
return false;
}





















AST_MATCHER(NamedDecl, hasExternalFormalLinkage) {
return Node.hasExternalFormalLinkage();
}






















AST_MATCHER(ParmVarDecl, hasDefaultArgument) {
return Node.hasDefaultArg();
}









AST_MATCHER(CXXNewExpr, isArray) {
return Node.isArray();
}









AST_MATCHER_P2(CXXNewExpr, hasPlacementArg, unsigned, Index,
internal::Matcher<Expr>, InnerMatcher) {
return Node.getNumPlacementArgs() > Index &&
InnerMatcher.matches(*Node.getPlacementArg(Index), Finder, Builder);
}









AST_MATCHER_P(CXXNewExpr, hasAnyPlacementArg, internal::Matcher<Expr>,
InnerMatcher) {
return llvm::any_of(Node.placement_arguments(), [&](const Expr *Arg) {
return InnerMatcher.matches(*Arg, Finder, Builder);
});
}









AST_MATCHER_P(CXXNewExpr, hasArraySize, internal::Matcher<Expr>, InnerMatcher) {
return Node.isArray() && *Node.getArraySize() &&
InnerMatcher.matches(**Node.getArraySize(), Finder, Builder);
}








AST_MATCHER(CXXRecordDecl, hasDefinition) {
return Node.hasDefinition();
}








AST_MATCHER(EnumDecl, isScoped) {
return Node.isScoped();
}








AST_MATCHER(FunctionDecl, hasTrailingReturn) {
if (const auto *F = Node.getType()->getAs<FunctionProtoType>())
return F->hasTrailingReturn();
return false;
}
























AST_MATCHER_P(Expr, ignoringElidableConstructorCall,
ast_matchers::internal::Matcher<Expr>, InnerMatcher) {

const Expr *E = &Node;





if (const auto *CleanupsExpr = dyn_cast<ExprWithCleanups>(&Node))
E = CleanupsExpr->getSubExpr();
if (const auto *CtorExpr = dyn_cast<CXXConstructExpr>(E)) {
if (CtorExpr->isElidable()) {
if (const auto *MaterializeTemp =
dyn_cast<MaterializeTemporaryExpr>(CtorExpr->getArg(0))) {
return InnerMatcher.matches(*MaterializeTemp->getSubExpr(), Finder,
Builder);
}
}
}
return InnerMatcher.matches(Node, Finder, Builder);
}

















extern const internal::VariadicDynCastAllOfMatcher<Stmt, OMPExecutableDirective>
ompExecutableDirective;














AST_MATCHER(OMPExecutableDirective, isStandaloneDirective) {
return Node.isStandaloneDirective();
}
















AST_MATCHER_P(OMPExecutableDirective, hasStructuredBlock,
internal::Matcher<Stmt>, InnerMatcher) {
if (Node.isStandaloneDirective())
return false;
return InnerMatcher.matches(*Node.getStructuredBlock(), Finder, Builder);
}












AST_MATCHER_P(OMPExecutableDirective, hasAnyClause,
internal::Matcher<OMPClause>, InnerMatcher) {
ArrayRef<OMPClause *> Clauses = Node.clauses();
return matchesFirstInPointerRange(InnerMatcher, Clauses.begin(),
Clauses.end(), Finder,
Builder) != Clauses.end();
}














extern const internal::VariadicDynCastAllOfMatcher<OMPClause, OMPDefaultClause>
ompDefaultClause;













AST_MATCHER(OMPDefaultClause, isNoneKind) {
return Node.getDefaultKind() == llvm::omp::OMP_DEFAULT_none;
}













AST_MATCHER(OMPDefaultClause, isSharedKind) {
return Node.getDefaultKind() == llvm::omp::OMP_DEFAULT_shared;
}















AST_MATCHER(OMPDefaultClause, isFirstPrivateKind) {
return Node.getDefaultKind() == llvm::omp::OMP_DEFAULT_firstprivate;
}


















AST_MATCHER_P(OMPExecutableDirective, isAllowedToContainClauseKind,
OpenMPClauseKind, CKind) {
return llvm::omp::isAllowedClauseForDirective(
Node.getDirectiveKind(), CKind,
Finder->getASTContext().getLangOpts().OpenMP);
}





}
}

#endif
