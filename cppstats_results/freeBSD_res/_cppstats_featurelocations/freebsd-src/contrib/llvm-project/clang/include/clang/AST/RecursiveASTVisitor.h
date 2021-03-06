











#if !defined(LLVM_CLANG_AST_RECURSIVEASTVISITOR_H)
#define LLVM_CLANG_AST_RECURSIVEASTVISITOR_H

#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclOpenMP.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprConcepts.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/ExprOpenMP.h"
#include "clang/AST/LambdaCapture.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/OpenMPClause.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"
#include "clang/AST/StmtObjC.h"
#include "clang/AST/StmtOpenMP.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include <algorithm>
#include <cstddef>
#include <type_traits>

namespace clang {





#define TRY_TO(CALL_EXPR) do { if (!getDerived().CALL_EXPR) return false; } while (false)





namespace detail {

template <typename T, typename U>
struct has_same_member_pointer_type : std::false_type {};
template <typename T, typename U, typename R, typename... P>
struct has_same_member_pointer_type<R (T::*)(P...), R (U::*)(P...)>
: std::true_type {};

template <bool has_same_type> struct is_same_method_impl {
template <typename FirstMethodPtrTy, typename SecondMethodPtrTy>
static bool isSameMethod(FirstMethodPtrTy FirstMethodPtr,
SecondMethodPtrTy SecondMethodPtr) {
return false;
}
};

template <> struct is_same_method_impl<true> {
template <typename FirstMethodPtrTy, typename SecondMethodPtrTy>
static bool isSameMethod(FirstMethodPtrTy FirstMethodPtr,
SecondMethodPtrTy SecondMethodPtr) {
return FirstMethodPtr == SecondMethodPtr;
}
};



template <typename FirstMethodPtrTy, typename SecondMethodPtrTy>
bool isSameMethod(FirstMethodPtrTy FirstMethodPtr,
SecondMethodPtrTy SecondMethodPtr) {
return is_same_method_impl<has_same_member_pointer_type<
FirstMethodPtrTy,
SecondMethodPtrTy>::value>::isSameMethod(FirstMethodPtr, SecondMethodPtr);
}

}


































































template <typename Derived> class RecursiveASTVisitor {
public:





typedef SmallVectorImpl<llvm::PointerIntPair<Stmt *, 1, bool>>
DataRecursionQueue;


Derived &getDerived() { return *static_cast<Derived *>(this); }



bool shouldVisitTemplateInstantiations() const { return false; }



bool shouldWalkTypesOfTypeLocs() const { return true; }



bool shouldVisitImplicitCode() const { return false; }


bool shouldVisitLambdaBody() const { return true; }


bool shouldTraversePostOrder() const { return false; }



bool TraverseAST(ASTContext &AST) {


return getDerived().TraverseDecl(AST.getTranslationUnitDecl());
}






bool TraverseStmt(Stmt *S, DataRecursionQueue *Queue = nullptr);




bool dataTraverseStmtPre(Stmt *S) { return true; }






bool dataTraverseStmtPost(Stmt *S) { return true; }






bool TraverseType(QualType T);






bool TraverseTypeLoc(TypeLoc TL);






bool TraverseAttr(Attr *At);






bool TraverseDecl(Decl *D);




bool TraverseNestedNameSpecifier(NestedNameSpecifier *NNS);





bool TraverseNestedNameSpecifierLoc(NestedNameSpecifierLoc NNS);




bool TraverseDeclarationNameInfo(DeclarationNameInfo NameInfo);





bool TraverseTemplateName(TemplateName Template);






bool TraverseTemplateArgument(const TemplateArgument &Arg);





bool TraverseTemplateArgumentLoc(const TemplateArgumentLoc &ArgLoc);







bool TraverseTemplateArguments(const TemplateArgument *Args,
unsigned NumArgs);





bool TraverseCXXBaseSpecifier(const CXXBaseSpecifier &Base);







bool TraverseConstructorInitializer(CXXCtorInitializer *Init);





bool TraverseLambdaCapture(LambdaExpr *LE, const LambdaCapture *C,
Expr *Init);





bool TraverseSynOrSemInitListExpr(InitListExpr *S,
DataRecursionQueue *Queue = nullptr);




bool TraverseConceptReference(const ConceptReference &C);




bool VisitAttr(Attr *A) { return true; }


#define ATTR_VISITOR_DECLS_ONLY
#include "clang/AST/AttrVisitor.inc"
#undef ATTR_VISITOR_DECLS_ONLY



Stmt::child_range getStmtChildren(Stmt *S) { return S->children(); }

private:





#define TRAVERSE_STMT_BASE(NAME, CLASS, VAR, QUEUE) (::clang::detail::has_same_member_pointer_type< decltype(&RecursiveASTVisitor::Traverse##NAME), decltype(&Derived::Traverse##NAME)>::value ? static_cast<std::conditional_t< ::clang::detail::has_same_member_pointer_type< decltype(&RecursiveASTVisitor::Traverse##NAME), decltype(&Derived::Traverse##NAME)>::value, Derived &, RecursiveASTVisitor &>>(*this) .Traverse##NAME(static_cast<CLASS *>(VAR), QUEUE) : getDerived().Traverse##NAME(static_cast<CLASS *>(VAR)))














#define TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S) do { if (!TRAVERSE_STMT_BASE(Stmt, Stmt, S, Queue)) return false; } while (false)





public:

#define ABSTRACT_STMT(STMT)
#define STMT(CLASS, PARENT) bool Traverse##CLASS(CLASS *S, DataRecursionQueue *Queue = nullptr);

#include "clang/AST/StmtNodes.inc"



bool WalkUpFromStmt(Stmt *S) { return getDerived().VisitStmt(S); }
bool VisitStmt(Stmt *S) { return true; }
#define STMT(CLASS, PARENT) bool WalkUpFrom##CLASS(CLASS *S) { TRY_TO(WalkUpFrom##PARENT(S)); TRY_TO(Visit##CLASS(S)); return true; } bool Visit##CLASS(CLASS *S) { return true; }






#include "clang/AST/StmtNodes.inc"





#define ABSTRACT_TYPE(CLASS, BASE)
#define TYPE(CLASS, BASE) bool Traverse##CLASS##Type(CLASS##Type *T);
#include "clang/AST/TypeNodes.inc"



bool WalkUpFromType(Type *T) { return getDerived().VisitType(T); }
bool VisitType(Type *T) { return true; }
#define TYPE(CLASS, BASE) bool WalkUpFrom##CLASS##Type(CLASS##Type *T) { TRY_TO(WalkUpFrom##BASE(T)); TRY_TO(Visit##CLASS##Type(T)); return true; } bool Visit##CLASS##Type(CLASS##Type *T) { return true; }






#include "clang/AST/TypeNodes.inc"





#define ABSTRACT_TYPELOC(CLASS, BASE)
#define TYPELOC(CLASS, BASE) bool Traverse##CLASS##TypeLoc(CLASS##TypeLoc TL);
#include "clang/AST/TypeLocNodes.def"



bool WalkUpFromTypeLoc(TypeLoc TL) { return getDerived().VisitTypeLoc(TL); }
bool VisitTypeLoc(TypeLoc TL) { return true; }



bool WalkUpFromQualifiedTypeLoc(QualifiedTypeLoc TL) {
return getDerived().VisitUnqualTypeLoc(TL.getUnqualifiedLoc());
}
bool VisitQualifiedTypeLoc(QualifiedTypeLoc TL) { return true; }
bool WalkUpFromUnqualTypeLoc(UnqualTypeLoc TL) {
return getDerived().VisitUnqualTypeLoc(TL.getUnqualifiedLoc());
}
bool VisitUnqualTypeLoc(UnqualTypeLoc TL) { return true; }


#define TYPE(CLASS, BASE) bool WalkUpFrom##CLASS##TypeLoc(CLASS##TypeLoc TL) { TRY_TO(WalkUpFrom##BASE##Loc(TL)); TRY_TO(Visit##CLASS##TypeLoc(TL)); return true; } bool Visit##CLASS##TypeLoc(CLASS##TypeLoc TL) { return true; }






#include "clang/AST/TypeNodes.inc"




#define ABSTRACT_DECL(DECL)
#define DECL(CLASS, BASE) bool Traverse##CLASS##Decl(CLASS##Decl *D);
#include "clang/AST/DeclNodes.inc"



bool WalkUpFromDecl(Decl *D) { return getDerived().VisitDecl(D); }
bool VisitDecl(Decl *D) { return true; }
#define DECL(CLASS, BASE) bool WalkUpFrom##CLASS##Decl(CLASS##Decl *D) { TRY_TO(WalkUpFrom##BASE(D)); TRY_TO(Visit##CLASS##Decl(D)); return true; } bool Visit##CLASS##Decl(CLASS##Decl *D) { return true; }






#include "clang/AST/DeclNodes.inc"

bool canIgnoreChildDeclWhileTraversingDeclContext(const Decl *Child);

#define DEF_TRAVERSE_TMPL_INST(TMPLDECLKIND) bool TraverseTemplateInstantiations(TMPLDECLKIND##TemplateDecl *D);

DEF_TRAVERSE_TMPL_INST(Class)
DEF_TRAVERSE_TMPL_INST(Var)
DEF_TRAVERSE_TMPL_INST(Function)
#undef DEF_TRAVERSE_TMPL_INST

bool dataTraverseNode(Stmt *S, DataRecursionQueue *Queue);

private:

bool TraverseTemplateParameterListHelper(TemplateParameterList *TPL);


template <typename T>
bool TraverseDeclTemplateParameterLists(T *D);

bool TraverseTemplateTypeParamDeclConstraints(const TemplateTypeParmDecl *D);

bool TraverseTemplateArgumentLocsHelper(const TemplateArgumentLoc *TAL,
unsigned Count);
bool TraverseArrayTypeLocHelper(ArrayTypeLoc TL);
bool TraverseRecordHelper(RecordDecl *D);
bool TraverseCXXRecordHelper(CXXRecordDecl *D);
bool TraverseDeclaratorHelper(DeclaratorDecl *D);
bool TraverseDeclContextHelper(DeclContext *DC);
bool TraverseFunctionHelper(FunctionDecl *D);
bool TraverseVarHelper(VarDecl *D);
bool TraverseOMPExecutableDirective(OMPExecutableDirective *S);
bool TraverseOMPLoopDirective(OMPLoopDirective *S);
bool TraverseOMPClause(OMPClause *C);
#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) bool Visit##Class(Class *C);
#include "llvm/Frontend/OpenMP/OMP.inc"

template <typename T> bool VisitOMPClauseList(T *Node);

bool VisitOMPClauseWithPreInit(OMPClauseWithPreInit *Node);
bool VisitOMPClauseWithPostUpdate(OMPClauseWithPostUpdate *Node);

bool PostVisitStmt(Stmt *S);
};

template <typename Derived>
bool RecursiveASTVisitor<Derived>::dataTraverseNode(Stmt *S,
DataRecursionQueue *Queue) {

switch (S->getStmtClass()) {
case Stmt::NoStmtClass:
break;
#define ABSTRACT_STMT(STMT)
#define STMT(CLASS, PARENT) case Stmt::CLASS##Class: return TRAVERSE_STMT_BASE(CLASS, CLASS, S, Queue);


#include "clang/AST/StmtNodes.inc"
}

return true;
}

#undef DISPATCH_STMT

template <typename Derived>
bool RecursiveASTVisitor<Derived>::PostVisitStmt(Stmt *S) {
















switch (S->getStmtClass()) {
case Stmt::NoStmtClass:
break;
#define ABSTRACT_STMT(STMT)
#define STMT(CLASS, PARENT) case Stmt::CLASS##Class: if (::clang::detail::isSameMethod(&RecursiveASTVisitor::Traverse##CLASS, &Derived::Traverse##CLASS)) { TRY_TO(WalkUpFrom##CLASS(static_cast<CLASS *>(S))); } break;






#define INITLISTEXPR(CLASS, PARENT) case Stmt::CLASS##Class: if (::clang::detail::isSameMethod(&RecursiveASTVisitor::Traverse##CLASS, &Derived::Traverse##CLASS)) { auto ILE = static_cast<CLASS *>(S); if (auto Syn = ILE->isSemanticForm() ? ILE->getSyntacticForm() : ILE) TRY_TO(WalkUpFrom##CLASS(Syn)); if (auto Sem = ILE->isSemanticForm() ? ILE : ILE->getSemanticForm()) TRY_TO(WalkUpFrom##CLASS(Sem)); } break;










#include "clang/AST/StmtNodes.inc"
}

return true;
}

#undef DISPATCH_STMT

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseStmt(Stmt *S,
DataRecursionQueue *Queue) {
if (!S)
return true;

if (Queue) {
Queue->push_back({S, false});
return true;
}

SmallVector<llvm::PointerIntPair<Stmt *, 1, bool>, 8> LocalQueue;
LocalQueue.push_back({S, false});

while (!LocalQueue.empty()) {
auto &CurrSAndVisited = LocalQueue.back();
Stmt *CurrS = CurrSAndVisited.getPointer();
bool Visited = CurrSAndVisited.getInt();
if (Visited) {
LocalQueue.pop_back();
TRY_TO(dataTraverseStmtPost(CurrS));
if (getDerived().shouldTraversePostOrder()) {
TRY_TO(PostVisitStmt(CurrS));
}
continue;
}

if (getDerived().dataTraverseStmtPre(CurrS)) {
CurrSAndVisited.setInt(true);
size_t N = LocalQueue.size();
TRY_TO(dataTraverseNode(CurrS, &LocalQueue));

std::reverse(LocalQueue.begin() + N, LocalQueue.end());
} else {
LocalQueue.pop_back();
}
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseType(QualType T) {
if (T.isNull())
return true;

switch (T->getTypeClass()) {
#define ABSTRACT_TYPE(CLASS, BASE)
#define TYPE(CLASS, BASE) case Type::CLASS: return getDerived().Traverse##CLASS##Type( static_cast<CLASS##Type *>(const_cast<Type *>(T.getTypePtr())));



#include "clang/AST/TypeNodes.inc"
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTypeLoc(TypeLoc TL) {
if (TL.isNull())
return true;

switch (TL.getTypeLocClass()) {
#define ABSTRACT_TYPELOC(CLASS, BASE)
#define TYPELOC(CLASS, BASE) case TypeLoc::CLASS: return getDerived().Traverse##CLASS##TypeLoc(TL.castAs<CLASS##TypeLoc>());


#include "clang/AST/TypeLocNodes.def"
}

return true;
}


#define VISITORCLASS RecursiveASTVisitor
#include "clang/AST/AttrVisitor.inc"
#undef VISITORCLASS

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseDecl(Decl *D) {
if (!D)
return true;



if (!getDerived().shouldVisitImplicitCode() && D->isImplicit()) {



if (auto *TTPD = dyn_cast<TemplateTypeParmDecl>(D))
return TraverseTemplateTypeParamDeclConstraints(TTPD);
return true;
}

switch (D->getKind()) {
#define ABSTRACT_DECL(DECL)
#define DECL(CLASS, BASE) case Decl::CLASS: if (!getDerived().Traverse##CLASS##Decl(static_cast<CLASS##Decl *>(D))) return false; break;




#include "clang/AST/DeclNodes.inc"
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseNestedNameSpecifier(
NestedNameSpecifier *NNS) {
if (!NNS)
return true;

if (NNS->getPrefix())
TRY_TO(TraverseNestedNameSpecifier(NNS->getPrefix()));

switch (NNS->getKind()) {
case NestedNameSpecifier::Identifier:
case NestedNameSpecifier::Namespace:
case NestedNameSpecifier::NamespaceAlias:
case NestedNameSpecifier::Global:
case NestedNameSpecifier::Super:
return true;

case NestedNameSpecifier::TypeSpec:
case NestedNameSpecifier::TypeSpecWithTemplate:
TRY_TO(TraverseType(QualType(NNS->getAsType(), 0)));
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseNestedNameSpecifierLoc(
NestedNameSpecifierLoc NNS) {
if (!NNS)
return true;

if (NestedNameSpecifierLoc Prefix = NNS.getPrefix())
TRY_TO(TraverseNestedNameSpecifierLoc(Prefix));

switch (NNS.getNestedNameSpecifier()->getKind()) {
case NestedNameSpecifier::Identifier:
case NestedNameSpecifier::Namespace:
case NestedNameSpecifier::NamespaceAlias:
case NestedNameSpecifier::Global:
case NestedNameSpecifier::Super:
return true;

case NestedNameSpecifier::TypeSpec:
case NestedNameSpecifier::TypeSpecWithTemplate:
TRY_TO(TraverseTypeLoc(NNS.getTypeLoc()));
break;
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseDeclarationNameInfo(
DeclarationNameInfo NameInfo) {
switch (NameInfo.getName().getNameKind()) {
case DeclarationName::CXXConstructorName:
case DeclarationName::CXXDestructorName:
case DeclarationName::CXXConversionFunctionName:
if (TypeSourceInfo *TSInfo = NameInfo.getNamedTypeInfo())
TRY_TO(TraverseTypeLoc(TSInfo->getTypeLoc()));
break;

case DeclarationName::CXXDeductionGuideName:
TRY_TO(TraverseTemplateName(
TemplateName(NameInfo.getName().getCXXDeductionGuideTemplate())));
break;

case DeclarationName::Identifier:
case DeclarationName::ObjCZeroArgSelector:
case DeclarationName::ObjCOneArgSelector:
case DeclarationName::ObjCMultiArgSelector:
case DeclarationName::CXXOperatorName:
case DeclarationName::CXXLiteralOperatorName:
case DeclarationName::CXXUsingDirective:
break;
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateName(TemplateName Template) {
if (DependentTemplateName *DTN = Template.getAsDependentTemplateName())
TRY_TO(TraverseNestedNameSpecifier(DTN->getQualifier()));
else if (QualifiedTemplateName *QTN = Template.getAsQualifiedTemplateName())
TRY_TO(TraverseNestedNameSpecifier(QTN->getQualifier()));

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateArgument(
const TemplateArgument &Arg) {
switch (Arg.getKind()) {
case TemplateArgument::Null:
case TemplateArgument::Declaration:
case TemplateArgument::Integral:
case TemplateArgument::NullPtr:
return true;

case TemplateArgument::Type:
return getDerived().TraverseType(Arg.getAsType());

case TemplateArgument::Template:
case TemplateArgument::TemplateExpansion:
return getDerived().TraverseTemplateName(
Arg.getAsTemplateOrTemplatePattern());

case TemplateArgument::Expression:
return getDerived().TraverseStmt(Arg.getAsExpr());

case TemplateArgument::Pack:
return getDerived().TraverseTemplateArguments(Arg.pack_begin(),
Arg.pack_size());
}

return true;
}



template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateArgumentLoc(
const TemplateArgumentLoc &ArgLoc) {
const TemplateArgument &Arg = ArgLoc.getArgument();

switch (Arg.getKind()) {
case TemplateArgument::Null:
case TemplateArgument::Declaration:
case TemplateArgument::Integral:
case TemplateArgument::NullPtr:
return true;

case TemplateArgument::Type: {

if (TypeSourceInfo *TSI = ArgLoc.getTypeSourceInfo())
return getDerived().TraverseTypeLoc(TSI->getTypeLoc());
else
return getDerived().TraverseType(Arg.getAsType());
}

case TemplateArgument::Template:
case TemplateArgument::TemplateExpansion:
if (ArgLoc.getTemplateQualifierLoc())
TRY_TO(getDerived().TraverseNestedNameSpecifierLoc(
ArgLoc.getTemplateQualifierLoc()));
return getDerived().TraverseTemplateName(
Arg.getAsTemplateOrTemplatePattern());

case TemplateArgument::Expression:
return getDerived().TraverseStmt(ArgLoc.getSourceExpression());

case TemplateArgument::Pack:
return getDerived().TraverseTemplateArguments(Arg.pack_begin(),
Arg.pack_size());
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateArguments(
const TemplateArgument *Args, unsigned NumArgs) {
for (unsigned I = 0; I != NumArgs; ++I) {
TRY_TO(TraverseTemplateArgument(Args[I]));
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseConstructorInitializer(
CXXCtorInitializer *Init) {
if (TypeSourceInfo *TInfo = Init->getTypeSourceInfo())
TRY_TO(TraverseTypeLoc(TInfo->getTypeLoc()));

if (Init->isWritten() || getDerived().shouldVisitImplicitCode())
TRY_TO(TraverseStmt(Init->getInit()));

return true;
}

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::TraverseLambdaCapture(LambdaExpr *LE,
const LambdaCapture *C,
Expr *Init) {
if (LE->isInitCapture(C))
TRY_TO(TraverseDecl(C->getCapturedVar()));
else
TRY_TO(TraverseStmt(Init));
return true;
}




#define DEF_TRAVERSE_TYPE(TYPE, CODE) template <typename Derived> bool RecursiveASTVisitor<Derived>::Traverse##TYPE(TYPE *T) { if (!getDerived().shouldTraversePostOrder()) TRY_TO(WalkUpFrom##TYPE(T)); { CODE; } if (getDerived().shouldTraversePostOrder()) TRY_TO(WalkUpFrom##TYPE(T)); return true; }










DEF_TRAVERSE_TYPE(BuiltinType, {})

DEF_TRAVERSE_TYPE(ComplexType, { TRY_TO(TraverseType(T->getElementType())); })

DEF_TRAVERSE_TYPE(PointerType, { TRY_TO(TraverseType(T->getPointeeType())); })

DEF_TRAVERSE_TYPE(BlockPointerType,
{ TRY_TO(TraverseType(T->getPointeeType())); })

DEF_TRAVERSE_TYPE(LValueReferenceType,
{ TRY_TO(TraverseType(T->getPointeeType())); })

DEF_TRAVERSE_TYPE(RValueReferenceType,
{ TRY_TO(TraverseType(T->getPointeeType())); })

DEF_TRAVERSE_TYPE(MemberPointerType, {
TRY_TO(TraverseType(QualType(T->getClass(), 0)));
TRY_TO(TraverseType(T->getPointeeType()));
})

DEF_TRAVERSE_TYPE(AdjustedType, { TRY_TO(TraverseType(T->getOriginalType())); })

DEF_TRAVERSE_TYPE(DecayedType, { TRY_TO(TraverseType(T->getOriginalType())); })

DEF_TRAVERSE_TYPE(ConstantArrayType, {
TRY_TO(TraverseType(T->getElementType()));
if (T->getSizeExpr())
TRY_TO(TraverseStmt(const_cast<Expr*>(T->getSizeExpr())));
})

DEF_TRAVERSE_TYPE(IncompleteArrayType,
{ TRY_TO(TraverseType(T->getElementType())); })

DEF_TRAVERSE_TYPE(VariableArrayType, {
TRY_TO(TraverseType(T->getElementType()));
TRY_TO(TraverseStmt(T->getSizeExpr()));
})

DEF_TRAVERSE_TYPE(DependentSizedArrayType, {
TRY_TO(TraverseType(T->getElementType()));
if (T->getSizeExpr())
TRY_TO(TraverseStmt(T->getSizeExpr()));
})

DEF_TRAVERSE_TYPE(DependentAddressSpaceType, {
TRY_TO(TraverseStmt(T->getAddrSpaceExpr()));
TRY_TO(TraverseType(T->getPointeeType()));
})

DEF_TRAVERSE_TYPE(DependentVectorType, {
if (T->getSizeExpr())
TRY_TO(TraverseStmt(T->getSizeExpr()));
TRY_TO(TraverseType(T->getElementType()));
})

DEF_TRAVERSE_TYPE(DependentSizedExtVectorType, {
if (T->getSizeExpr())
TRY_TO(TraverseStmt(T->getSizeExpr()));
TRY_TO(TraverseType(T->getElementType()));
})

DEF_TRAVERSE_TYPE(VectorType, { TRY_TO(TraverseType(T->getElementType())); })

DEF_TRAVERSE_TYPE(ExtVectorType, { TRY_TO(TraverseType(T->getElementType())); })

DEF_TRAVERSE_TYPE(ConstantMatrixType,
{ TRY_TO(TraverseType(T->getElementType())); })

DEF_TRAVERSE_TYPE(DependentSizedMatrixType, {
if (T->getRowExpr())
TRY_TO(TraverseStmt(T->getRowExpr()));
if (T->getColumnExpr())
TRY_TO(TraverseStmt(T->getColumnExpr()));
TRY_TO(TraverseType(T->getElementType()));
})

DEF_TRAVERSE_TYPE(FunctionNoProtoType,
{ TRY_TO(TraverseType(T->getReturnType())); })

DEF_TRAVERSE_TYPE(FunctionProtoType, {
TRY_TO(TraverseType(T->getReturnType()));

for (const auto &A : T->param_types()) {
TRY_TO(TraverseType(A));
}

for (const auto &E : T->exceptions()) {
TRY_TO(TraverseType(E));
}

if (Expr *NE = T->getNoexceptExpr())
TRY_TO(TraverseStmt(NE));
})

DEF_TRAVERSE_TYPE(UnresolvedUsingType, {})
DEF_TRAVERSE_TYPE(TypedefType, {})

DEF_TRAVERSE_TYPE(TypeOfExprType,
{ TRY_TO(TraverseStmt(T->getUnderlyingExpr())); })

DEF_TRAVERSE_TYPE(TypeOfType, { TRY_TO(TraverseType(T->getUnderlyingType())); })

DEF_TRAVERSE_TYPE(DecltypeType,
{ TRY_TO(TraverseStmt(T->getUnderlyingExpr())); })

DEF_TRAVERSE_TYPE(UnaryTransformType, {
TRY_TO(TraverseType(T->getBaseType()));
TRY_TO(TraverseType(T->getUnderlyingType()));
})

DEF_TRAVERSE_TYPE(AutoType, {
TRY_TO(TraverseType(T->getDeducedType()));
if (T->isConstrained()) {
TRY_TO(TraverseDecl(T->getTypeConstraintConcept()));
TRY_TO(TraverseTemplateArguments(T->getArgs(), T->getNumArgs()));
}
})
DEF_TRAVERSE_TYPE(DeducedTemplateSpecializationType, {
TRY_TO(TraverseTemplateName(T->getTemplateName()));
TRY_TO(TraverseType(T->getDeducedType()));
})

DEF_TRAVERSE_TYPE(RecordType, {})
DEF_TRAVERSE_TYPE(EnumType, {})
DEF_TRAVERSE_TYPE(TemplateTypeParmType, {})
DEF_TRAVERSE_TYPE(SubstTemplateTypeParmType, {
TRY_TO(TraverseType(T->getReplacementType()));
})
DEF_TRAVERSE_TYPE(SubstTemplateTypeParmPackType, {
TRY_TO(TraverseTemplateArgument(T->getArgumentPack()));
})

DEF_TRAVERSE_TYPE(TemplateSpecializationType, {
TRY_TO(TraverseTemplateName(T->getTemplateName()));
TRY_TO(TraverseTemplateArguments(T->getArgs(), T->getNumArgs()));
})

DEF_TRAVERSE_TYPE(InjectedClassNameType, {})

DEF_TRAVERSE_TYPE(AttributedType,
{ TRY_TO(TraverseType(T->getModifiedType())); })

DEF_TRAVERSE_TYPE(ParenType, { TRY_TO(TraverseType(T->getInnerType())); })

DEF_TRAVERSE_TYPE(MacroQualifiedType,
{ TRY_TO(TraverseType(T->getUnderlyingType())); })

DEF_TRAVERSE_TYPE(ElaboratedType, {
if (T->getQualifier()) {
TRY_TO(TraverseNestedNameSpecifier(T->getQualifier()));
}
TRY_TO(TraverseType(T->getNamedType()));
})

DEF_TRAVERSE_TYPE(DependentNameType,
{ TRY_TO(TraverseNestedNameSpecifier(T->getQualifier())); })

DEF_TRAVERSE_TYPE(DependentTemplateSpecializationType, {
TRY_TO(TraverseNestedNameSpecifier(T->getQualifier()));
TRY_TO(TraverseTemplateArguments(T->getArgs(), T->getNumArgs()));
})

DEF_TRAVERSE_TYPE(PackExpansionType, { TRY_TO(TraverseType(T->getPattern())); })

DEF_TRAVERSE_TYPE(ObjCTypeParamType, {})

DEF_TRAVERSE_TYPE(ObjCInterfaceType, {})

DEF_TRAVERSE_TYPE(ObjCObjectType, {


if (T->getBaseType().getTypePtr() != T)
TRY_TO(TraverseType(T->getBaseType()));
for (auto typeArg : T->getTypeArgsAsWritten()) {
TRY_TO(TraverseType(typeArg));
}
})

DEF_TRAVERSE_TYPE(ObjCObjectPointerType,
{ TRY_TO(TraverseType(T->getPointeeType())); })

DEF_TRAVERSE_TYPE(AtomicType, { TRY_TO(TraverseType(T->getValueType())); })

DEF_TRAVERSE_TYPE(PipeType, { TRY_TO(TraverseType(T->getElementType())); })

DEF_TRAVERSE_TYPE(ExtIntType, {})
DEF_TRAVERSE_TYPE(DependentExtIntType,
{ TRY_TO(TraverseStmt(T->getNumBitsExpr())); })

#undef DEF_TRAVERSE_TYPE








#define DEF_TRAVERSE_TYPELOC(TYPE, CODE) template <typename Derived> bool RecursiveASTVisitor<Derived>::Traverse##TYPE##Loc(TYPE##Loc TL) { if (!getDerived().shouldTraversePostOrder()) { TRY_TO(WalkUpFrom##TYPE##Loc(TL)); if (getDerived().shouldWalkTypesOfTypeLocs()) TRY_TO(WalkUpFrom##TYPE(const_cast<TYPE *>(TL.getTypePtr()))); } { CODE; } if (getDerived().shouldTraversePostOrder()) { TRY_TO(WalkUpFrom##TYPE##Loc(TL)); if (getDerived().shouldWalkTypesOfTypeLocs()) TRY_TO(WalkUpFrom##TYPE(const_cast<TYPE *>(TL.getTypePtr()))); } return true; }
















template <typename Derived>
bool
RecursiveASTVisitor<Derived>::TraverseQualifiedTypeLoc(QualifiedTypeLoc TL) {















return TraverseTypeLoc(TL.getUnqualifiedLoc());
}

DEF_TRAVERSE_TYPELOC(BuiltinType, {})


DEF_TRAVERSE_TYPELOC(ComplexType, {
TRY_TO(TraverseType(TL.getTypePtr()->getElementType()));
})

DEF_TRAVERSE_TYPELOC(PointerType,
{ TRY_TO(TraverseTypeLoc(TL.getPointeeLoc())); })

DEF_TRAVERSE_TYPELOC(BlockPointerType,
{ TRY_TO(TraverseTypeLoc(TL.getPointeeLoc())); })

DEF_TRAVERSE_TYPELOC(LValueReferenceType,
{ TRY_TO(TraverseTypeLoc(TL.getPointeeLoc())); })

DEF_TRAVERSE_TYPELOC(RValueReferenceType,
{ TRY_TO(TraverseTypeLoc(TL.getPointeeLoc())); })



DEF_TRAVERSE_TYPELOC(MemberPointerType, {
if (auto *TSI = TL.getClassTInfo())
TRY_TO(TraverseTypeLoc(TSI->getTypeLoc()));
else
TRY_TO(TraverseType(QualType(TL.getTypePtr()->getClass(), 0)));
TRY_TO(TraverseTypeLoc(TL.getPointeeLoc()));
})

DEF_TRAVERSE_TYPELOC(AdjustedType,
{ TRY_TO(TraverseTypeLoc(TL.getOriginalLoc())); })

DEF_TRAVERSE_TYPELOC(DecayedType,
{ TRY_TO(TraverseTypeLoc(TL.getOriginalLoc())); })

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseArrayTypeLocHelper(ArrayTypeLoc TL) {

TRY_TO(TraverseStmt(TL.getSizeExpr()));
return true;
}

DEF_TRAVERSE_TYPELOC(ConstantArrayType, {
TRY_TO(TraverseTypeLoc(TL.getElementLoc()));
TRY_TO(TraverseArrayTypeLocHelper(TL));
})

DEF_TRAVERSE_TYPELOC(IncompleteArrayType, {
TRY_TO(TraverseTypeLoc(TL.getElementLoc()));
TRY_TO(TraverseArrayTypeLocHelper(TL));
})

DEF_TRAVERSE_TYPELOC(VariableArrayType, {
TRY_TO(TraverseTypeLoc(TL.getElementLoc()));
TRY_TO(TraverseArrayTypeLocHelper(TL));
})

DEF_TRAVERSE_TYPELOC(DependentSizedArrayType, {
TRY_TO(TraverseTypeLoc(TL.getElementLoc()));
TRY_TO(TraverseArrayTypeLocHelper(TL));
})

DEF_TRAVERSE_TYPELOC(DependentAddressSpaceType, {
TRY_TO(TraverseStmt(TL.getTypePtr()->getAddrSpaceExpr()));
TRY_TO(TraverseType(TL.getTypePtr()->getPointeeType()));
})



DEF_TRAVERSE_TYPELOC(DependentSizedExtVectorType, {
if (TL.getTypePtr()->getSizeExpr())
TRY_TO(TraverseStmt(TL.getTypePtr()->getSizeExpr()));
TRY_TO(TraverseType(TL.getTypePtr()->getElementType()));
})


DEF_TRAVERSE_TYPELOC(VectorType, {
TRY_TO(TraverseType(TL.getTypePtr()->getElementType()));
})

DEF_TRAVERSE_TYPELOC(DependentVectorType, {
if (TL.getTypePtr()->getSizeExpr())
TRY_TO(TraverseStmt(TL.getTypePtr()->getSizeExpr()));
TRY_TO(TraverseType(TL.getTypePtr()->getElementType()));
})



DEF_TRAVERSE_TYPELOC(ExtVectorType, {
TRY_TO(TraverseType(TL.getTypePtr()->getElementType()));
})

DEF_TRAVERSE_TYPELOC(ConstantMatrixType, {
TRY_TO(TraverseStmt(TL.getAttrRowOperand()));
TRY_TO(TraverseStmt(TL.getAttrColumnOperand()));
TRY_TO(TraverseType(TL.getTypePtr()->getElementType()));
})

DEF_TRAVERSE_TYPELOC(DependentSizedMatrixType, {
TRY_TO(TraverseStmt(TL.getAttrRowOperand()));
TRY_TO(TraverseStmt(TL.getAttrColumnOperand()));
TRY_TO(TraverseType(TL.getTypePtr()->getElementType()));
})

DEF_TRAVERSE_TYPELOC(FunctionNoProtoType,
{ TRY_TO(TraverseTypeLoc(TL.getReturnLoc())); })


DEF_TRAVERSE_TYPELOC(FunctionProtoType, {
TRY_TO(TraverseTypeLoc(TL.getReturnLoc()));

const FunctionProtoType *T = TL.getTypePtr();

for (unsigned I = 0, E = TL.getNumParams(); I != E; ++I) {
if (TL.getParam(I)) {
TRY_TO(TraverseDecl(TL.getParam(I)));
} else if (I < T->getNumParams()) {
TRY_TO(TraverseType(T->getParamType(I)));
}
}

for (const auto &E : T->exceptions()) {
TRY_TO(TraverseType(E));
}

if (Expr *NE = T->getNoexceptExpr())
TRY_TO(TraverseStmt(NE));
})

DEF_TRAVERSE_TYPELOC(UnresolvedUsingType, {})
DEF_TRAVERSE_TYPELOC(TypedefType, {})

DEF_TRAVERSE_TYPELOC(TypeOfExprType,
{ TRY_TO(TraverseStmt(TL.getUnderlyingExpr())); })

DEF_TRAVERSE_TYPELOC(TypeOfType, {
TRY_TO(TraverseTypeLoc(TL.getUnderlyingTInfo()->getTypeLoc()));
})


DEF_TRAVERSE_TYPELOC(DecltypeType, {
TRY_TO(TraverseStmt(TL.getTypePtr()->getUnderlyingExpr()));
})

DEF_TRAVERSE_TYPELOC(UnaryTransformType, {
TRY_TO(TraverseTypeLoc(TL.getUnderlyingTInfo()->getTypeLoc()));
})

DEF_TRAVERSE_TYPELOC(AutoType, {
TRY_TO(TraverseType(TL.getTypePtr()->getDeducedType()));
if (TL.isConstrained()) {
TRY_TO(TraverseNestedNameSpecifierLoc(TL.getNestedNameSpecifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(TL.getConceptNameInfo()));
for (unsigned I = 0, E = TL.getNumArgs(); I != E; ++I)
TRY_TO(TraverseTemplateArgumentLoc(TL.getArgLoc(I)));
}
})

DEF_TRAVERSE_TYPELOC(DeducedTemplateSpecializationType, {
TRY_TO(TraverseTemplateName(TL.getTypePtr()->getTemplateName()));
TRY_TO(TraverseType(TL.getTypePtr()->getDeducedType()));
})

DEF_TRAVERSE_TYPELOC(RecordType, {})
DEF_TRAVERSE_TYPELOC(EnumType, {})
DEF_TRAVERSE_TYPELOC(TemplateTypeParmType, {})
DEF_TRAVERSE_TYPELOC(SubstTemplateTypeParmType, {
TRY_TO(TraverseType(TL.getTypePtr()->getReplacementType()));
})
DEF_TRAVERSE_TYPELOC(SubstTemplateTypeParmPackType, {
TRY_TO(TraverseTemplateArgument(TL.getTypePtr()->getArgumentPack()));
})


DEF_TRAVERSE_TYPELOC(TemplateSpecializationType, {
TRY_TO(TraverseTemplateName(TL.getTypePtr()->getTemplateName()));
for (unsigned I = 0, E = TL.getNumArgs(); I != E; ++I) {
TRY_TO(TraverseTemplateArgumentLoc(TL.getArgLoc(I)));
}
})

DEF_TRAVERSE_TYPELOC(InjectedClassNameType, {})

DEF_TRAVERSE_TYPELOC(ParenType, { TRY_TO(TraverseTypeLoc(TL.getInnerLoc())); })

DEF_TRAVERSE_TYPELOC(MacroQualifiedType,
{ TRY_TO(TraverseTypeLoc(TL.getInnerLoc())); })

DEF_TRAVERSE_TYPELOC(AttributedType,
{ TRY_TO(TraverseTypeLoc(TL.getModifiedLoc())); })

DEF_TRAVERSE_TYPELOC(ElaboratedType, {
if (TL.getQualifierLoc()) {
TRY_TO(TraverseNestedNameSpecifierLoc(TL.getQualifierLoc()));
}
TRY_TO(TraverseTypeLoc(TL.getNamedTypeLoc()));
})

DEF_TRAVERSE_TYPELOC(DependentNameType, {
TRY_TO(TraverseNestedNameSpecifierLoc(TL.getQualifierLoc()));
})

DEF_TRAVERSE_TYPELOC(DependentTemplateSpecializationType, {
if (TL.getQualifierLoc()) {
TRY_TO(TraverseNestedNameSpecifierLoc(TL.getQualifierLoc()));
}

for (unsigned I = 0, E = TL.getNumArgs(); I != E; ++I) {
TRY_TO(TraverseTemplateArgumentLoc(TL.getArgLoc(I)));
}
})

DEF_TRAVERSE_TYPELOC(PackExpansionType,
{ TRY_TO(TraverseTypeLoc(TL.getPatternLoc())); })

DEF_TRAVERSE_TYPELOC(ObjCTypeParamType, {})

DEF_TRAVERSE_TYPELOC(ObjCInterfaceType, {})

DEF_TRAVERSE_TYPELOC(ObjCObjectType, {


if (TL.getTypePtr()->getBaseType().getTypePtr() != TL.getTypePtr())
TRY_TO(TraverseTypeLoc(TL.getBaseLoc()));
for (unsigned i = 0, n = TL.getNumTypeArgs(); i != n; ++i)
TRY_TO(TraverseTypeLoc(TL.getTypeArgTInfo(i)->getTypeLoc()));
})

DEF_TRAVERSE_TYPELOC(ObjCObjectPointerType,
{ TRY_TO(TraverseTypeLoc(TL.getPointeeLoc())); })

DEF_TRAVERSE_TYPELOC(AtomicType, { TRY_TO(TraverseTypeLoc(TL.getValueLoc())); })

DEF_TRAVERSE_TYPELOC(PipeType, { TRY_TO(TraverseTypeLoc(TL.getValueLoc())); })

DEF_TRAVERSE_TYPELOC(ExtIntType, {})
DEF_TRAVERSE_TYPELOC(DependentExtIntType, {
TRY_TO(TraverseStmt(TL.getTypePtr()->getNumBitsExpr()));
})

#undef DEF_TRAVERSE_TYPELOC








template <typename Derived>
bool RecursiveASTVisitor<Derived>::canIgnoreChildDeclWhileTraversingDeclContext(
const Decl *Child) {


if (isa<BlockDecl>(Child) || isa<CapturedDecl>(Child))
return true;

if (const CXXRecordDecl* Cls = dyn_cast<CXXRecordDecl>(Child))
return Cls->isLambda();
return false;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseDeclContextHelper(DeclContext *DC) {
if (!DC)
return true;

for (auto *Child : DC->decls()) {
if (!canIgnoreChildDeclWhileTraversingDeclContext(Child))
TRY_TO(TraverseDecl(Child));
}

return true;
}


#define DEF_TRAVERSE_DECL(DECL, CODE) template <typename Derived> bool RecursiveASTVisitor<Derived>::Traverse##DECL(DECL *D) { bool ShouldVisitChildren = true; bool ReturnValue = true; if (!getDerived().shouldTraversePostOrder()) TRY_TO(WalkUpFrom##DECL(D)); { CODE; } if (ReturnValue && ShouldVisitChildren) TRY_TO(TraverseDeclContextHelper(dyn_cast<DeclContext>(D))); if (ReturnValue) { for (auto *I : D->attrs()) TRY_TO(getDerived().TraverseAttr(I)); } if (ReturnValue && getDerived().shouldTraversePostOrder()) TRY_TO(WalkUpFrom##DECL(D)); return ReturnValue; }



















DEF_TRAVERSE_DECL(AccessSpecDecl, {})

DEF_TRAVERSE_DECL(BlockDecl, {
if (TypeSourceInfo *TInfo = D->getSignatureAsWritten())
TRY_TO(TraverseTypeLoc(TInfo->getTypeLoc()));
TRY_TO(TraverseStmt(D->getBody()));
for (const auto &I : D->captures()) {
if (I.hasCopyExpr()) {
TRY_TO(TraverseStmt(I.getCopyExpr()));
}
}
ShouldVisitChildren = false;
})

DEF_TRAVERSE_DECL(CapturedDecl, {
TRY_TO(TraverseStmt(D->getBody()));
ShouldVisitChildren = false;
})

DEF_TRAVERSE_DECL(EmptyDecl, {})

DEF_TRAVERSE_DECL(LifetimeExtendedTemporaryDecl, {
TRY_TO(TraverseStmt(D->getTemporaryExpr()));
})

DEF_TRAVERSE_DECL(FileScopeAsmDecl,
{ TRY_TO(TraverseStmt(D->getAsmString())); })

DEF_TRAVERSE_DECL(ImportDecl, {})

DEF_TRAVERSE_DECL(FriendDecl, {

if (D->getFriendType())
TRY_TO(TraverseTypeLoc(D->getFriendType()->getTypeLoc()));
else
TRY_TO(TraverseDecl(D->getFriendDecl()));
})

DEF_TRAVERSE_DECL(FriendTemplateDecl, {
if (D->getFriendType())
TRY_TO(TraverseTypeLoc(D->getFriendType()->getTypeLoc()));
else
TRY_TO(TraverseDecl(D->getFriendDecl()));
for (unsigned I = 0, E = D->getNumTemplateParameters(); I < E; ++I) {
TemplateParameterList *TPL = D->getTemplateParameterList(I);
for (TemplateParameterList::iterator ITPL = TPL->begin(), ETPL = TPL->end();
ITPL != ETPL; ++ITPL) {
TRY_TO(TraverseDecl(*ITPL));
}
}
})

DEF_TRAVERSE_DECL(ClassScopeFunctionSpecializationDecl, {
TRY_TO(TraverseDecl(D->getSpecialization()));

if (D->hasExplicitTemplateArgs()) {
TRY_TO(TraverseTemplateArgumentLocsHelper(
D->getTemplateArgsAsWritten()->getTemplateArgs(),
D->getTemplateArgsAsWritten()->NumTemplateArgs));
}
})

DEF_TRAVERSE_DECL(LinkageSpecDecl, {})

DEF_TRAVERSE_DECL(ExportDecl, {})

DEF_TRAVERSE_DECL(ObjCPropertyImplDecl, {
})

DEF_TRAVERSE_DECL(StaticAssertDecl, {
TRY_TO(TraverseStmt(D->getAssertExpr()));
TRY_TO(TraverseStmt(D->getMessage()));
})

DEF_TRAVERSE_DECL(TranslationUnitDecl, {






auto Scope = D->getASTContext().getTraversalScope();
bool HasLimitedScope =
Scope.size() != 1 || !isa<TranslationUnitDecl>(Scope.front());
if (HasLimitedScope) {
ShouldVisitChildren = false;
for (auto *Child : Scope) {
if (!canIgnoreChildDeclWhileTraversingDeclContext(Child))
TRY_TO(TraverseDecl(Child));
}
}
})

DEF_TRAVERSE_DECL(PragmaCommentDecl, {})

DEF_TRAVERSE_DECL(PragmaDetectMismatchDecl, {})

DEF_TRAVERSE_DECL(ExternCContextDecl, {})

DEF_TRAVERSE_DECL(NamespaceAliasDecl, {
TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));



ShouldVisitChildren = false;
})

DEF_TRAVERSE_DECL(LabelDecl, {
})

DEF_TRAVERSE_DECL(
NamespaceDecl,
{


})

DEF_TRAVERSE_DECL(ObjCCompatibleAliasDecl, {
})

DEF_TRAVERSE_DECL(ObjCCategoryDecl, {
if (ObjCTypeParamList *typeParamList = D->getTypeParamList()) {
for (auto typeParam : *typeParamList) {
TRY_TO(TraverseObjCTypeParamDecl(typeParam));
}
}
})

DEF_TRAVERSE_DECL(ObjCCategoryImplDecl, {
})

DEF_TRAVERSE_DECL(ObjCImplementationDecl, {
})

DEF_TRAVERSE_DECL(ObjCInterfaceDecl, {
if (ObjCTypeParamList *typeParamList = D->getTypeParamListAsWritten()) {
for (auto typeParam : *typeParamList) {
TRY_TO(TraverseObjCTypeParamDecl(typeParam));
}
}

if (TypeSourceInfo *superTInfo = D->getSuperClassTInfo()) {
TRY_TO(TraverseTypeLoc(superTInfo->getTypeLoc()));
}
})

DEF_TRAVERSE_DECL(ObjCProtocolDecl, {
})

DEF_TRAVERSE_DECL(ObjCMethodDecl, {
if (D->getReturnTypeSourceInfo()) {
TRY_TO(TraverseTypeLoc(D->getReturnTypeSourceInfo()->getTypeLoc()));
}
for (ParmVarDecl *Parameter : D->parameters()) {
TRY_TO(TraverseDecl(Parameter));
}
if (D->isThisDeclarationADefinition()) {
TRY_TO(TraverseStmt(D->getBody()));
}
ShouldVisitChildren = false;
})

DEF_TRAVERSE_DECL(ObjCTypeParamDecl, {
if (D->hasExplicitBound()) {
TRY_TO(TraverseTypeLoc(D->getTypeSourceInfo()->getTypeLoc()));



}
})

DEF_TRAVERSE_DECL(ObjCPropertyDecl, {
if (D->getTypeSourceInfo())
TRY_TO(TraverseTypeLoc(D->getTypeSourceInfo()->getTypeLoc()));
else
TRY_TO(TraverseType(D->getType()));
ShouldVisitChildren = false;
})

DEF_TRAVERSE_DECL(UsingDecl, {
TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(D->getNameInfo()));
})

DEF_TRAVERSE_DECL(UsingEnumDecl, {})

DEF_TRAVERSE_DECL(UsingPackDecl, {})

DEF_TRAVERSE_DECL(UsingDirectiveDecl, {
TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));
})

DEF_TRAVERSE_DECL(UsingShadowDecl, {})

DEF_TRAVERSE_DECL(ConstructorUsingShadowDecl, {})

DEF_TRAVERSE_DECL(OMPThreadPrivateDecl, {
for (auto *I : D->varlists()) {
TRY_TO(TraverseStmt(I));
}
})

DEF_TRAVERSE_DECL(OMPRequiresDecl, {
for (auto *C : D->clauselists()) {
TRY_TO(TraverseOMPClause(C));
}
})

DEF_TRAVERSE_DECL(OMPDeclareReductionDecl, {
TRY_TO(TraverseStmt(D->getCombiner()));
if (auto *Initializer = D->getInitializer())
TRY_TO(TraverseStmt(Initializer));
TRY_TO(TraverseType(D->getType()));
return true;
})

DEF_TRAVERSE_DECL(OMPDeclareMapperDecl, {
for (auto *C : D->clauselists())
TRY_TO(TraverseOMPClause(C));
TRY_TO(TraverseType(D->getType()));
return true;
})

DEF_TRAVERSE_DECL(OMPCapturedExprDecl, { TRY_TO(TraverseVarHelper(D)); })

DEF_TRAVERSE_DECL(OMPAllocateDecl, {
for (auto *I : D->varlists())
TRY_TO(TraverseStmt(I));
for (auto *C : D->clauselists())
TRY_TO(TraverseOMPClause(C));
})


template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateParameterListHelper(
TemplateParameterList *TPL) {
if (TPL) {
for (NamedDecl *D : *TPL) {
TRY_TO(TraverseDecl(D));
}
if (Expr *RequiresClause = TPL->getRequiresClause()) {
TRY_TO(TraverseStmt(RequiresClause));
}
}
return true;
}

template <typename Derived>
template <typename T>
bool RecursiveASTVisitor<Derived>::TraverseDeclTemplateParameterLists(T *D) {
for (unsigned i = 0; i < D->getNumTemplateParameterLists(); i++) {
TemplateParameterList *TPL = D->getTemplateParameterList(i);
TraverseTemplateParameterListHelper(TPL);
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateInstantiations(
ClassTemplateDecl *D) {
for (auto *SD : D->specializations()) {
for (auto *RD : SD->redecls()) {

if (cast<CXXRecordDecl>(RD)->isInjectedClassName())
continue;

switch (
cast<ClassTemplateSpecializationDecl>(RD)->getSpecializationKind()) {

case TSK_Undeclared:
case TSK_ImplicitInstantiation:
TRY_TO(TraverseDecl(RD));
break;




case TSK_ExplicitInstantiationDeclaration:
case TSK_ExplicitInstantiationDefinition:
case TSK_ExplicitSpecialization:
break;
}
}
}

return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateInstantiations(
VarTemplateDecl *D) {
for (auto *SD : D->specializations()) {
for (auto *RD : SD->redecls()) {
switch (
cast<VarTemplateSpecializationDecl>(RD)->getSpecializationKind()) {
case TSK_Undeclared:
case TSK_ImplicitInstantiation:
TRY_TO(TraverseDecl(RD));
break;

case TSK_ExplicitInstantiationDeclaration:
case TSK_ExplicitInstantiationDefinition:
case TSK_ExplicitSpecialization:
break;
}
}
}

return true;
}



template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateInstantiations(
FunctionTemplateDecl *D) {
for (auto *FD : D->specializations()) {
for (auto *RD : FD->redecls()) {
switch (RD->getTemplateSpecializationKind()) {
case TSK_Undeclared:
case TSK_ImplicitInstantiation:

TRY_TO(TraverseDecl(RD));
break;



case TSK_ExplicitInstantiationDeclaration:
case TSK_ExplicitInstantiationDefinition:
TRY_TO(TraverseDecl(RD));
break;

case TSK_ExplicitSpecialization:
break;
}
}
}

return true;
}



#define DEF_TRAVERSE_TMPL_DECL(TMPLDECLKIND) DEF_TRAVERSE_DECL(TMPLDECLKIND##TemplateDecl, { TRY_TO(TraverseTemplateParameterListHelper(D->getTemplateParameters())); TRY_TO(TraverseDecl(D->getTemplatedDecl())); if (getDerived().shouldVisitTemplateInstantiations() && D == D->getCanonicalDecl()) TRY_TO(TraverseTemplateInstantiations(D)); })



















DEF_TRAVERSE_TMPL_DECL(Class)
DEF_TRAVERSE_TMPL_DECL(Var)
DEF_TRAVERSE_TMPL_DECL(Function)

DEF_TRAVERSE_DECL(TemplateTemplateParmDecl, {


TRY_TO(TraverseDecl(D->getTemplatedDecl()));
if (D->hasDefaultArgument() && !D->defaultArgumentWasInherited())
TRY_TO(TraverseTemplateArgumentLoc(D->getDefaultArgument()));
TRY_TO(TraverseTemplateParameterListHelper(D->getTemplateParameters()));
})

DEF_TRAVERSE_DECL(BuiltinTemplateDecl, {
TRY_TO(TraverseTemplateParameterListHelper(D->getTemplateParameters()));
})

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateTypeParamDeclConstraints(
const TemplateTypeParmDecl *D) {
if (const auto *TC = D->getTypeConstraint()) {
if (Expr *IDC = TC->getImmediatelyDeclaredConstraint()) {
TRY_TO(TraverseStmt(IDC));
} else {




TRY_TO(TraverseConceptReference(*TC));
}
}
return true;
}

DEF_TRAVERSE_DECL(TemplateTypeParmDecl, {

if (D->getTypeForDecl())
TRY_TO(TraverseType(QualType(D->getTypeForDecl(), 0)));
TRY_TO(TraverseTemplateTypeParamDeclConstraints(D));
if (D->hasDefaultArgument() && !D->defaultArgumentWasInherited())
TRY_TO(TraverseTypeLoc(D->getDefaultArgumentInfo()->getTypeLoc()));
})

DEF_TRAVERSE_DECL(TypedefDecl, {
TRY_TO(TraverseTypeLoc(D->getTypeSourceInfo()->getTypeLoc()));



})

DEF_TRAVERSE_DECL(TypeAliasDecl, {
TRY_TO(TraverseTypeLoc(D->getTypeSourceInfo()->getTypeLoc()));



})

DEF_TRAVERSE_DECL(TypeAliasTemplateDecl, {
TRY_TO(TraverseDecl(D->getTemplatedDecl()));
TRY_TO(TraverseTemplateParameterListHelper(D->getTemplateParameters()));
})

DEF_TRAVERSE_DECL(ConceptDecl, {
TRY_TO(TraverseTemplateParameterListHelper(D->getTemplateParameters()));
TRY_TO(TraverseStmt(D->getConstraintExpr()));
})

DEF_TRAVERSE_DECL(UnresolvedUsingTypenameDecl, {


TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));



})

DEF_TRAVERSE_DECL(UnresolvedUsingIfExistsDecl, {})

DEF_TRAVERSE_DECL(EnumDecl, {
TRY_TO(TraverseDeclTemplateParameterLists(D));

if (D->getTypeForDecl())
TRY_TO(TraverseType(QualType(D->getTypeForDecl(), 0)));

TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));


})


template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseRecordHelper(RecordDecl *D) {



TRY_TO(TraverseDeclTemplateParameterLists(D));
TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseCXXBaseSpecifier(
const CXXBaseSpecifier &Base) {
TRY_TO(TraverseTypeLoc(Base.getTypeSourceInfo()->getTypeLoc()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseCXXRecordHelper(CXXRecordDecl *D) {
if (!TraverseRecordHelper(D))
return false;
if (D->isCompleteDefinition()) {
for (const auto &I : D->bases()) {
TRY_TO(TraverseCXXBaseSpecifier(I));
}


}
return true;
}

DEF_TRAVERSE_DECL(RecordDecl, { TRY_TO(TraverseRecordHelper(D)); })

DEF_TRAVERSE_DECL(CXXRecordDecl, { TRY_TO(TraverseCXXRecordHelper(D)); })

#define DEF_TRAVERSE_TMPL_SPEC_DECL(TMPLDECLKIND) DEF_TRAVERSE_DECL(TMPLDECLKIND##TemplateSpecializationDecl, { if (TypeSourceInfo *TSI = D->getTypeAsWritten()) TRY_TO(TraverseTypeLoc(TSI->getTypeLoc())); TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc())); if (!getDerived().shouldVisitTemplateInstantiations() && D->getTemplateSpecializationKind() != TSK_ExplicitSpecialization) return true; })






















DEF_TRAVERSE_TMPL_SPEC_DECL(Class)
DEF_TRAVERSE_TMPL_SPEC_DECL(Var)

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseTemplateArgumentLocsHelper(
const TemplateArgumentLoc *TAL, unsigned Count) {
for (unsigned I = 0; I < Count; ++I) {
TRY_TO(TraverseTemplateArgumentLoc(TAL[I]));
}
return true;
}

#define DEF_TRAVERSE_TMPL_PART_SPEC_DECL(TMPLDECLKIND, DECLKIND) DEF_TRAVERSE_DECL(TMPLDECLKIND##TemplatePartialSpecializationDecl, { if (TemplateParameterList *TPL = D->getTemplateParameters()) { for (TemplateParameterList::iterator I = TPL->begin(), E = TPL->end(); I != E; ++I) { TRY_TO(TraverseDecl(*I)); } } TRY_TO(TraverseTemplateArgumentLocsHelper( D->getTemplateArgsAsWritten()->getTemplateArgs(), D->getTemplateArgsAsWritten()->NumTemplateArgs)); TRY_TO(Traverse##DECLKIND##Helper(D)); })





















DEF_TRAVERSE_TMPL_PART_SPEC_DECL(Class, CXXRecord)
DEF_TRAVERSE_TMPL_PART_SPEC_DECL(Var, Var)

DEF_TRAVERSE_DECL(EnumConstantDecl, { TRY_TO(TraverseStmt(D->getInitExpr())); })

DEF_TRAVERSE_DECL(UnresolvedUsingValueDecl, {


TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(D->getNameInfo()));
})

DEF_TRAVERSE_DECL(IndirectFieldDecl, {})

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseDeclaratorHelper(DeclaratorDecl *D) {
TRY_TO(TraverseDeclTemplateParameterLists(D));
TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));
if (D->getTypeSourceInfo())
TRY_TO(TraverseTypeLoc(D->getTypeSourceInfo()->getTypeLoc()));
else
TRY_TO(TraverseType(D->getType()));
return true;
}

DEF_TRAVERSE_DECL(DecompositionDecl, {
TRY_TO(TraverseVarHelper(D));
for (auto *Binding : D->bindings()) {
TRY_TO(TraverseDecl(Binding));
}
})

DEF_TRAVERSE_DECL(BindingDecl, {
if (getDerived().shouldVisitImplicitCode())
TRY_TO(TraverseStmt(D->getBinding()));
})

DEF_TRAVERSE_DECL(MSPropertyDecl, { TRY_TO(TraverseDeclaratorHelper(D)); })

DEF_TRAVERSE_DECL(MSGuidDecl, {})

DEF_TRAVERSE_DECL(TemplateParamObjectDecl, {})

DEF_TRAVERSE_DECL(FieldDecl, {
TRY_TO(TraverseDeclaratorHelper(D));
if (D->isBitField())
TRY_TO(TraverseStmt(D->getBitWidth()));
else if (D->hasInClassInitializer())
TRY_TO(TraverseStmt(D->getInClassInitializer()));
})

DEF_TRAVERSE_DECL(ObjCAtDefsFieldDecl, {
TRY_TO(TraverseDeclaratorHelper(D));
if (D->isBitField())
TRY_TO(TraverseStmt(D->getBitWidth()));

})

DEF_TRAVERSE_DECL(ObjCIvarDecl, {
TRY_TO(TraverseDeclaratorHelper(D));
if (D->isBitField())
TRY_TO(TraverseStmt(D->getBitWidth()));

})

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseFunctionHelper(FunctionDecl *D) {
TRY_TO(TraverseDeclTemplateParameterLists(D));
TRY_TO(TraverseNestedNameSpecifierLoc(D->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(D->getNameInfo()));






if (const FunctionTemplateSpecializationInfo *FTSI =
D->getTemplateSpecializationInfo()) {
if (FTSI->getTemplateSpecializationKind() != TSK_Undeclared &&
FTSI->getTemplateSpecializationKind() != TSK_ImplicitInstantiation) {


if (const ASTTemplateArgumentListInfo *TALI =
FTSI->TemplateArgumentsAsWritten) {
TRY_TO(TraverseTemplateArgumentLocsHelper(TALI->getTemplateArgs(),
TALI->NumTemplateArgs));
}
}
}





if (TypeSourceInfo *TSI = D->getTypeSourceInfo()) {
TRY_TO(TraverseTypeLoc(TSI->getTypeLoc()));
} else if (getDerived().shouldVisitImplicitCode()) {




for (ParmVarDecl *Parameter : D->parameters()) {
TRY_TO(TraverseDecl(Parameter));
}
}


if (Expr *TrailingRequiresClause = D->getTrailingRequiresClause()) {
TRY_TO(TraverseStmt(TrailingRequiresClause));
}

if (CXXConstructorDecl *Ctor = dyn_cast<CXXConstructorDecl>(D)) {

for (auto *I : Ctor->inits()) {
if (I->isWritten() || getDerived().shouldVisitImplicitCode())
TRY_TO(TraverseConstructorInitializer(I));
}
}

bool VisitBody =
D->isThisDeclarationADefinition() &&


(!D->isDefaulted() || getDerived().shouldVisitImplicitCode());

if (const auto *MD = dyn_cast<CXXMethodDecl>(D)) {
if (const CXXRecordDecl *RD = MD->getParent()) {
if (RD->isLambda() &&
declaresSameEntity(RD->getLambdaCallOperator(), MD)) {
VisitBody = VisitBody && getDerived().shouldVisitLambdaBody();
}
}
}

if (VisitBody) {
TRY_TO(TraverseStmt(D->getBody()));
}
return true;
}

DEF_TRAVERSE_DECL(FunctionDecl, {


ShouldVisitChildren = false;
ReturnValue = TraverseFunctionHelper(D);
})

DEF_TRAVERSE_DECL(CXXDeductionGuideDecl, {


ShouldVisitChildren = false;
ReturnValue = TraverseFunctionHelper(D);
})

DEF_TRAVERSE_DECL(CXXMethodDecl, {


ShouldVisitChildren = false;
ReturnValue = TraverseFunctionHelper(D);
})

DEF_TRAVERSE_DECL(CXXConstructorDecl, {


ShouldVisitChildren = false;
ReturnValue = TraverseFunctionHelper(D);
})



DEF_TRAVERSE_DECL(CXXConversionDecl, {


ShouldVisitChildren = false;
ReturnValue = TraverseFunctionHelper(D);
})

DEF_TRAVERSE_DECL(CXXDestructorDecl, {


ShouldVisitChildren = false;
ReturnValue = TraverseFunctionHelper(D);
})

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseVarHelper(VarDecl *D) {
TRY_TO(TraverseDeclaratorHelper(D));

if (!isa<ParmVarDecl>(D) &&
(!D->isCXXForRangeDecl() || getDerived().shouldVisitImplicitCode()))
TRY_TO(TraverseStmt(D->getInit()));
return true;
}

DEF_TRAVERSE_DECL(VarDecl, { TRY_TO(TraverseVarHelper(D)); })

DEF_TRAVERSE_DECL(ImplicitParamDecl, { TRY_TO(TraverseVarHelper(D)); })

DEF_TRAVERSE_DECL(NonTypeTemplateParmDecl, {

TRY_TO(TraverseDeclaratorHelper(D));
if (D->hasDefaultArgument() && !D->defaultArgumentWasInherited())
TRY_TO(TraverseStmt(D->getDefaultArgument()));
})

DEF_TRAVERSE_DECL(ParmVarDecl, {
TRY_TO(TraverseVarHelper(D));

if (D->hasDefaultArg() && D->hasUninstantiatedDefaultArg() &&
!D->hasUnparsedDefaultArg())
TRY_TO(TraverseStmt(D->getUninstantiatedDefaultArg()));

if (D->hasDefaultArg() && !D->hasUninstantiatedDefaultArg() &&
!D->hasUnparsedDefaultArg())
TRY_TO(TraverseStmt(D->getDefaultArg()));
})

DEF_TRAVERSE_DECL(RequiresExprBodyDecl, {})

#undef DEF_TRAVERSE_DECL











#define DEF_TRAVERSE_STMT(STMT, CODE) template <typename Derived> bool RecursiveASTVisitor<Derived>::Traverse##STMT( STMT *S, DataRecursionQueue *Queue) { bool ShouldVisitChildren = true; bool ReturnValue = true; if (!getDerived().shouldTraversePostOrder()) TRY_TO(WalkUpFrom##STMT(S)); { CODE; } if (ShouldVisitChildren) { for (Stmt * SubStmt : getDerived().getStmtChildren(S)) { TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(SubStmt); } } if (!Queue && ReturnValue && getDerived().shouldTraversePostOrder()) { TRY_TO(WalkUpFrom##STMT(S)); } return ReturnValue; }























DEF_TRAVERSE_STMT(GCCAsmStmt, {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getAsmString());
for (unsigned I = 0, E = S->getNumInputs(); I < E; ++I) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getInputConstraintLiteral(I));
}
for (unsigned I = 0, E = S->getNumOutputs(); I < E; ++I) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getOutputConstraintLiteral(I));
}
for (unsigned I = 0, E = S->getNumClobbers(); I < E; ++I) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getClobberStringLiteral(I));
}

})

DEF_TRAVERSE_STMT(
MSAsmStmt,
{

})

DEF_TRAVERSE_STMT(CXXCatchStmt, {
TRY_TO(TraverseDecl(S->getExceptionDecl()));

})

DEF_TRAVERSE_STMT(DeclStmt, {
for (auto *I : S->decls()) {
TRY_TO(TraverseDecl(I));
}





ShouldVisitChildren = false;
})



DEF_TRAVERSE_STMT(BreakStmt, {})
DEF_TRAVERSE_STMT(CXXTryStmt, {})
DEF_TRAVERSE_STMT(CaseStmt, {})
DEF_TRAVERSE_STMT(CompoundStmt, {})
DEF_TRAVERSE_STMT(ContinueStmt, {})
DEF_TRAVERSE_STMT(DefaultStmt, {})
DEF_TRAVERSE_STMT(DoStmt, {})
DEF_TRAVERSE_STMT(ForStmt, {})
DEF_TRAVERSE_STMT(GotoStmt, {})
DEF_TRAVERSE_STMT(IfStmt, {})
DEF_TRAVERSE_STMT(IndirectGotoStmt, {})
DEF_TRAVERSE_STMT(LabelStmt, {})
DEF_TRAVERSE_STMT(AttributedStmt, {})
DEF_TRAVERSE_STMT(NullStmt, {})
DEF_TRAVERSE_STMT(ObjCAtCatchStmt, {})
DEF_TRAVERSE_STMT(ObjCAtFinallyStmt, {})
DEF_TRAVERSE_STMT(ObjCAtSynchronizedStmt, {})
DEF_TRAVERSE_STMT(ObjCAtThrowStmt, {})
DEF_TRAVERSE_STMT(ObjCAtTryStmt, {})
DEF_TRAVERSE_STMT(ObjCForCollectionStmt, {})
DEF_TRAVERSE_STMT(ObjCAutoreleasePoolStmt, {})

DEF_TRAVERSE_STMT(CXXForRangeStmt, {
if (!getDerived().shouldVisitImplicitCode()) {
if (S->getInit())
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getInit());
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getLoopVarStmt());
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getRangeInit());
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getBody());

ShouldVisitChildren = false;
}
})

DEF_TRAVERSE_STMT(MSDependentExistsStmt, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(S->getNameInfo()));
})

DEF_TRAVERSE_STMT(ReturnStmt, {})
DEF_TRAVERSE_STMT(SwitchStmt, {})
DEF_TRAVERSE_STMT(WhileStmt, {})

DEF_TRAVERSE_STMT(ConstantExpr, {})

DEF_TRAVERSE_STMT(CXXDependentScopeMemberExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(S->getMemberNameInfo()));
if (S->hasExplicitTemplateArgs()) {
TRY_TO(TraverseTemplateArgumentLocsHelper(S->getTemplateArgs(),
S->getNumTemplateArgs()));
}
})

DEF_TRAVERSE_STMT(DeclRefExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(S->getNameInfo()));
TRY_TO(TraverseTemplateArgumentLocsHelper(S->getTemplateArgs(),
S->getNumTemplateArgs()));
})

DEF_TRAVERSE_STMT(DependentScopeDeclRefExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(S->getNameInfo()));
if (S->hasExplicitTemplateArgs()) {
TRY_TO(TraverseTemplateArgumentLocsHelper(S->getTemplateArgs(),
S->getNumTemplateArgs()));
}
})

DEF_TRAVERSE_STMT(MemberExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(S->getMemberNameInfo()));
TRY_TO(TraverseTemplateArgumentLocsHelper(S->getTemplateArgs(),
S->getNumTemplateArgs()));
})

DEF_TRAVERSE_STMT(
ImplicitCastExpr,
{

})

DEF_TRAVERSE_STMT(CStyleCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXFunctionalCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXAddrspaceCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXConstCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXDynamicCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXReinterpretCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXStaticCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(BuiltinBitCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseSynOrSemInitListExpr(
InitListExpr *S, DataRecursionQueue *Queue) {
if (S) {


if (!getDerived().shouldTraversePostOrder())
TRY_TO(WalkUpFromInitListExpr(S));


for (Stmt *SubStmt : S->children()) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(SubStmt);
}

if (!Queue && getDerived().shouldTraversePostOrder())
TRY_TO(WalkUpFromInitListExpr(S));
}
return true;
}

template<typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseConceptReference(
const ConceptReference &C) {
TRY_TO(TraverseNestedNameSpecifierLoc(C.getNestedNameSpecifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(C.getConceptNameInfo()));
if (C.hasExplicitTemplateArgs())
TRY_TO(TraverseTemplateArgumentLocsHelper(
C.getTemplateArgsAsWritten()->getTemplateArgs(),
C.getTemplateArgsAsWritten()->NumTemplateArgs));
return true;
}









template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseInitListExpr(
InitListExpr *S, DataRecursionQueue *Queue) {
if (S->isSemanticForm() && S->isSyntacticForm()) {

TRY_TO(TraverseSynOrSemInitListExpr(S, Queue));
return true;
}
TRY_TO(TraverseSynOrSemInitListExpr(
S->isSemanticForm() ? S->getSyntacticForm() : S, Queue));
if (getDerived().shouldVisitImplicitCode()) {


TRY_TO(TraverseSynOrSemInitListExpr(
S->isSemanticForm() ? S : S->getSemanticForm(), Queue));
}
return true;
}




DEF_TRAVERSE_STMT(GenericSelectionExpr, {
TRY_TO(TraverseStmt(S->getControllingExpr()));
for (const GenericSelectionExpr::Association Assoc : S->associations()) {
if (TypeSourceInfo *TSI = Assoc.getTypeSourceInfo())
TRY_TO(TraverseTypeLoc(TSI->getTypeLoc()));
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(Assoc.getAssociationExpr());
}
ShouldVisitChildren = false;
})



DEF_TRAVERSE_STMT(PseudoObjectExpr, {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getSyntacticForm());
for (PseudoObjectExpr::semantics_iterator i = S->semantics_begin(),
e = S->semantics_end();
i != e; ++i) {
Expr *sub = *i;
if (OpaqueValueExpr *OVE = dyn_cast<OpaqueValueExpr>(sub))
sub = OVE->getSourceExpr();
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(sub);
}
ShouldVisitChildren = false;
})

DEF_TRAVERSE_STMT(CXXScalarValueInitExpr, {


TRY_TO(TraverseTypeLoc(S->getTypeSourceInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXNewExpr, {

TRY_TO(TraverseTypeLoc(S->getAllocatedTypeSourceInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(OffsetOfExpr, {




TRY_TO(TraverseTypeLoc(S->getTypeSourceInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(UnaryExprOrTypeTraitExpr, {


if (S->isArgumentType())
TRY_TO(TraverseTypeLoc(S->getArgumentTypeInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXTypeidExpr, {


if (S->isTypeOperand())
TRY_TO(TraverseTypeLoc(S->getTypeOperandSourceInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(MSPropertyRefExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
})

DEF_TRAVERSE_STMT(MSPropertySubscriptExpr, {})

DEF_TRAVERSE_STMT(CXXUuidofExpr, {


if (S->isTypeOperand())
TRY_TO(TraverseTypeLoc(S->getTypeOperandSourceInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(TypeTraitExpr, {
for (unsigned I = 0, N = S->getNumArgs(); I != N; ++I)
TRY_TO(TraverseTypeLoc(S->getArg(I)->getTypeLoc()));
})

DEF_TRAVERSE_STMT(ArrayTypeTraitExpr, {
TRY_TO(TraverseTypeLoc(S->getQueriedTypeSourceInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(ExpressionTraitExpr,
{ TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getQueriedExpression()); })

DEF_TRAVERSE_STMT(VAArgExpr, {

TRY_TO(TraverseTypeLoc(S->getWrittenTypeInfo()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXTemporaryObjectExpr, {

TRY_TO(TraverseTypeLoc(S->getTypeSourceInfo()->getTypeLoc()));
})


DEF_TRAVERSE_STMT(LambdaExpr, {

for (unsigned I = 0, N = S->capture_size(); I != N; ++I) {
const LambdaCapture *C = S->capture_begin() + I;
if (C->isExplicit() || getDerived().shouldVisitImplicitCode()) {
TRY_TO(TraverseLambdaCapture(S, C, S->capture_init_begin()[I]));
}
}

if (getDerived().shouldVisitImplicitCode()) {

TRY_TO(TraverseDecl(S->getLambdaClass()));
} else {

TypeLoc TL = S->getCallOperator()->getTypeSourceInfo()->getTypeLoc();
FunctionProtoTypeLoc Proto = TL.getAsAdjusted<FunctionProtoTypeLoc>();

TRY_TO(TraverseTemplateParameterListHelper(S->getTemplateParameterList()));
if (S->hasExplicitParameters()) {

for (unsigned I = 0, N = Proto.getNumParams(); I != N; ++I)
TRY_TO(TraverseDecl(Proto.getParam(I)));
}

auto *T = Proto.getTypePtr();
for (const auto &E : T->exceptions())
TRY_TO(TraverseType(E));

if (Expr *NE = T->getNoexceptExpr())
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(NE);

if (S->hasExplicitResultType())
TRY_TO(TraverseTypeLoc(Proto.getReturnLoc()));
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getTrailingRequiresClause());

TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getBody());
}
ShouldVisitChildren = false;
})

DEF_TRAVERSE_STMT(CXXUnresolvedConstructExpr, {

TRY_TO(TraverseTypeLoc(S->getTypeSourceInfo()->getTypeLoc()));
})



DEF_TRAVERSE_STMT(CXXConstructExpr, {})
DEF_TRAVERSE_STMT(CallExpr, {})
DEF_TRAVERSE_STMT(CXXMemberCallExpr, {})



DEF_TRAVERSE_STMT(AddrLabelExpr, {})
DEF_TRAVERSE_STMT(ArraySubscriptExpr, {})
DEF_TRAVERSE_STMT(MatrixSubscriptExpr, {})
DEF_TRAVERSE_STMT(OMPArraySectionExpr, {})
DEF_TRAVERSE_STMT(OMPArrayShapingExpr, {})
DEF_TRAVERSE_STMT(OMPIteratorExpr, {})

DEF_TRAVERSE_STMT(BlockExpr, {
TRY_TO(TraverseDecl(S->getBlockDecl()));
return true;
})

DEF_TRAVERSE_STMT(ChooseExpr, {})
DEF_TRAVERSE_STMT(CompoundLiteralExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeSourceInfo()->getTypeLoc()));
})
DEF_TRAVERSE_STMT(CXXBindTemporaryExpr, {})
DEF_TRAVERSE_STMT(CXXBoolLiteralExpr, {})

DEF_TRAVERSE_STMT(CXXDefaultArgExpr, {
if (getDerived().shouldVisitImplicitCode())
TRY_TO(TraverseStmt(S->getExpr()));
})

DEF_TRAVERSE_STMT(CXXDefaultInitExpr, {})
DEF_TRAVERSE_STMT(CXXDeleteExpr, {})
DEF_TRAVERSE_STMT(ExprWithCleanups, {})
DEF_TRAVERSE_STMT(CXXInheritedCtorInitExpr, {})
DEF_TRAVERSE_STMT(CXXNullPtrLiteralExpr, {})
DEF_TRAVERSE_STMT(CXXStdInitializerListExpr, {})

DEF_TRAVERSE_STMT(CXXPseudoDestructorExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
if (TypeSourceInfo *ScopeInfo = S->getScopeTypeInfo())
TRY_TO(TraverseTypeLoc(ScopeInfo->getTypeLoc()));
if (TypeSourceInfo *DestroyedTypeInfo = S->getDestroyedTypeInfo())
TRY_TO(TraverseTypeLoc(DestroyedTypeInfo->getTypeLoc()));
})

DEF_TRAVERSE_STMT(CXXThisExpr, {})
DEF_TRAVERSE_STMT(CXXThrowExpr, {})
DEF_TRAVERSE_STMT(UserDefinedLiteral, {})
DEF_TRAVERSE_STMT(DesignatedInitExpr, {})
DEF_TRAVERSE_STMT(DesignatedInitUpdateExpr, {})
DEF_TRAVERSE_STMT(ExtVectorElementExpr, {})
DEF_TRAVERSE_STMT(GNUNullExpr, {})
DEF_TRAVERSE_STMT(ImplicitValueInitExpr, {})
DEF_TRAVERSE_STMT(NoInitExpr, {})
DEF_TRAVERSE_STMT(ArrayInitLoopExpr, {


if (OpaqueValueExpr *OVE = S->getCommonExpr())
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(OVE->getSourceExpr());
})
DEF_TRAVERSE_STMT(ArrayInitIndexExpr, {})
DEF_TRAVERSE_STMT(ObjCBoolLiteralExpr, {})

DEF_TRAVERSE_STMT(ObjCEncodeExpr, {
if (TypeSourceInfo *TInfo = S->getEncodedTypeSourceInfo())
TRY_TO(TraverseTypeLoc(TInfo->getTypeLoc()));
})

DEF_TRAVERSE_STMT(ObjCIsaExpr, {})
DEF_TRAVERSE_STMT(ObjCIvarRefExpr, {})

DEF_TRAVERSE_STMT(ObjCMessageExpr, {
if (TypeSourceInfo *TInfo = S->getClassReceiverTypeInfo())
TRY_TO(TraverseTypeLoc(TInfo->getTypeLoc()));
})

DEF_TRAVERSE_STMT(ObjCPropertyRefExpr, {
if (S->isClassReceiver()) {
ObjCInterfaceDecl *IDecl = S->getClassReceiver();
QualType Type = IDecl->getASTContext().getObjCInterfaceType(IDecl);
ObjCInterfaceLocInfo Data;
Data.NameLoc = S->getReceiverLocation();
Data.NameEndLoc = Data.NameLoc;
TRY_TO(TraverseTypeLoc(TypeLoc(Type, &Data)));
}
})
DEF_TRAVERSE_STMT(ObjCSubscriptRefExpr, {})
DEF_TRAVERSE_STMT(ObjCProtocolExpr, {})
DEF_TRAVERSE_STMT(ObjCSelectorExpr, {})
DEF_TRAVERSE_STMT(ObjCIndirectCopyRestoreExpr, {})

DEF_TRAVERSE_STMT(ObjCBridgedCastExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeInfoAsWritten()->getTypeLoc()));
})

DEF_TRAVERSE_STMT(ObjCAvailabilityCheckExpr, {})
DEF_TRAVERSE_STMT(ParenExpr, {})
DEF_TRAVERSE_STMT(ParenListExpr, {})
DEF_TRAVERSE_STMT(SYCLUniqueStableNameExpr, {
TRY_TO(TraverseTypeLoc(S->getTypeSourceInfo()->getTypeLoc()));
})
DEF_TRAVERSE_STMT(PredefinedExpr, {})
DEF_TRAVERSE_STMT(ShuffleVectorExpr, {})
DEF_TRAVERSE_STMT(ConvertVectorExpr, {})
DEF_TRAVERSE_STMT(StmtExpr, {})
DEF_TRAVERSE_STMT(SourceLocExpr, {})

DEF_TRAVERSE_STMT(UnresolvedLookupExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
if (S->hasExplicitTemplateArgs()) {
TRY_TO(TraverseTemplateArgumentLocsHelper(S->getTemplateArgs(),
S->getNumTemplateArgs()));
}
})

DEF_TRAVERSE_STMT(UnresolvedMemberExpr, {
TRY_TO(TraverseNestedNameSpecifierLoc(S->getQualifierLoc()));
if (S->hasExplicitTemplateArgs()) {
TRY_TO(TraverseTemplateArgumentLocsHelper(S->getTemplateArgs(),
S->getNumTemplateArgs()));
}
})

DEF_TRAVERSE_STMT(SEHTryStmt, {})
DEF_TRAVERSE_STMT(SEHExceptStmt, {})
DEF_TRAVERSE_STMT(SEHFinallyStmt, {})
DEF_TRAVERSE_STMT(SEHLeaveStmt, {})
DEF_TRAVERSE_STMT(CapturedStmt, { TRY_TO(TraverseDecl(S->getCapturedDecl())); })

DEF_TRAVERSE_STMT(CXXOperatorCallExpr, {})
DEF_TRAVERSE_STMT(CXXRewrittenBinaryOperator, {
if (!getDerived().shouldVisitImplicitCode()) {
CXXRewrittenBinaryOperator::DecomposedForm Decomposed =
S->getDecomposedForm();
TRY_TO(TraverseStmt(const_cast<Expr*>(Decomposed.LHS)));
TRY_TO(TraverseStmt(const_cast<Expr*>(Decomposed.RHS)));
ShouldVisitChildren = false;
}
})
DEF_TRAVERSE_STMT(OpaqueValueExpr, {})
DEF_TRAVERSE_STMT(TypoExpr, {})
DEF_TRAVERSE_STMT(RecoveryExpr, {})
DEF_TRAVERSE_STMT(CUDAKernelCallExpr, {})



DEF_TRAVERSE_STMT(BinaryConditionalOperator, {})
DEF_TRAVERSE_STMT(ConditionalOperator, {})
DEF_TRAVERSE_STMT(UnaryOperator, {})
DEF_TRAVERSE_STMT(BinaryOperator, {})
DEF_TRAVERSE_STMT(CompoundAssignOperator, {})
DEF_TRAVERSE_STMT(CXXNoexceptExpr, {})
DEF_TRAVERSE_STMT(PackExpansionExpr, {})
DEF_TRAVERSE_STMT(SizeOfPackExpr, {})
DEF_TRAVERSE_STMT(SubstNonTypeTemplateParmPackExpr, {})
DEF_TRAVERSE_STMT(SubstNonTypeTemplateParmExpr, {})
DEF_TRAVERSE_STMT(FunctionParmPackExpr, {})
DEF_TRAVERSE_STMT(CXXFoldExpr, {})
DEF_TRAVERSE_STMT(AtomicExpr, {})

DEF_TRAVERSE_STMT(MaterializeTemporaryExpr, {
if (S->getLifetimeExtendedTemporaryDecl()) {
TRY_TO(TraverseLifetimeExtendedTemporaryDecl(
S->getLifetimeExtendedTemporaryDecl()));
ShouldVisitChildren = false;
}
})



DEF_TRAVERSE_STMT(CoroutineBodyStmt, {
if (!getDerived().shouldVisitImplicitCode()) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getBody());
ShouldVisitChildren = false;
}
})
DEF_TRAVERSE_STMT(CoreturnStmt, {
if (!getDerived().shouldVisitImplicitCode()) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getOperand());
ShouldVisitChildren = false;
}
})
DEF_TRAVERSE_STMT(CoawaitExpr, {
if (!getDerived().shouldVisitImplicitCode()) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getOperand());
ShouldVisitChildren = false;
}
})
DEF_TRAVERSE_STMT(DependentCoawaitExpr, {
if (!getDerived().shouldVisitImplicitCode()) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getOperand());
ShouldVisitChildren = false;
}
})
DEF_TRAVERSE_STMT(CoyieldExpr, {
if (!getDerived().shouldVisitImplicitCode()) {
TRY_TO_TRAVERSE_OR_ENQUEUE_STMT(S->getOperand());
ShouldVisitChildren = false;
}
})

DEF_TRAVERSE_STMT(ConceptSpecializationExpr, {
TRY_TO(TraverseConceptReference(*S));
})

DEF_TRAVERSE_STMT(RequiresExpr, {
TRY_TO(TraverseDecl(S->getBody()));
for (ParmVarDecl *Parm : S->getLocalParameters())
TRY_TO(TraverseDecl(Parm));
for (concepts::Requirement *Req : S->getRequirements())
if (auto *TypeReq = dyn_cast<concepts::TypeRequirement>(Req)) {
if (!TypeReq->isSubstitutionFailure())
TRY_TO(TraverseTypeLoc(TypeReq->getType()->getTypeLoc()));
} else if (auto *ExprReq = dyn_cast<concepts::ExprRequirement>(Req)) {
if (!ExprReq->isExprSubstitutionFailure())
TRY_TO(TraverseStmt(ExprReq->getExpr()));
auto &RetReq = ExprReq->getReturnTypeRequirement();
if (RetReq.isTypeConstraint())
TRY_TO(TraverseTemplateParameterListHelper(
RetReq.getTypeConstraintTemplateParameterList()));
} else {
auto *NestedReq = cast<concepts::NestedRequirement>(Req);
if (!NestedReq->isSubstitutionFailure())
TRY_TO(TraverseStmt(NestedReq->getConstraintExpr()));
}
})


DEF_TRAVERSE_STMT(IntegerLiteral, {})
DEF_TRAVERSE_STMT(FixedPointLiteral, {})
DEF_TRAVERSE_STMT(CharacterLiteral, {})
DEF_TRAVERSE_STMT(FloatingLiteral, {})
DEF_TRAVERSE_STMT(ImaginaryLiteral, {})
DEF_TRAVERSE_STMT(StringLiteral, {})
DEF_TRAVERSE_STMT(ObjCStringLiteral, {})
DEF_TRAVERSE_STMT(ObjCBoxedExpr, {})
DEF_TRAVERSE_STMT(ObjCArrayLiteral, {})
DEF_TRAVERSE_STMT(ObjCDictionaryLiteral, {})


DEF_TRAVERSE_STMT(AsTypeExpr, {})


template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseOMPExecutableDirective(
OMPExecutableDirective *S) {
for (auto *C : S->clauses()) {
TRY_TO(TraverseOMPClause(C));
}
return true;
}

DEF_TRAVERSE_STMT(OMPCanonicalLoop, {
if (!getDerived().shouldVisitImplicitCode()) {

TRY_TO(TraverseStmt(S->getLoopStmt()));
ShouldVisitChildren = false;
}
})

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::TraverseOMPLoopDirective(OMPLoopDirective *S) {
return TraverseOMPExecutableDirective(S);
}

DEF_TRAVERSE_STMT(OMPParallelDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTileDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPUnrollDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPForDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPForSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPSectionsDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPSectionDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPSingleDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPMasterDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPCriticalDirective, {
TRY_TO(TraverseDeclarationNameInfo(S->getDirectiveName()));
TRY_TO(TraverseOMPExecutableDirective(S));
})

DEF_TRAVERSE_STMT(OMPParallelForDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPParallelForSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPParallelMasterDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPParallelSectionsDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTaskDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTaskyieldDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPBarrierDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTaskwaitDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTaskgroupDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPCancellationPointDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPCancelDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPFlushDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPDepobjDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPScanDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPOrderedDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPAtomicDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetDataDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetEnterDataDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetExitDataDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetParallelDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetParallelForDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTeamsDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetUpdateDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTaskLoopDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTaskLoopSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPMasterTaskLoopDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPMasterTaskLoopSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPParallelMasterTaskLoopDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPParallelMasterTaskLoopSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPDistributeDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPDistributeParallelForDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPDistributeParallelForSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPDistributeSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetParallelForSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTeamsDistributeDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTeamsDistributeSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTeamsDistributeParallelForSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTeamsDistributeParallelForDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetTeamsDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetTeamsDistributeDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetTeamsDistributeParallelForDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetTeamsDistributeParallelForSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPTargetTeamsDistributeSimdDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPInteropDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPDispatchDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })

DEF_TRAVERSE_STMT(OMPMaskedDirective,
{ TRY_TO(TraverseOMPExecutableDirective(S)); })


template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseOMPClause(OMPClause *C) {
if (!C)
return true;
switch (C->getClauseKind()) {
#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) case llvm::omp::Clause::Enum: TRY_TO(Visit##Class(static_cast<Class *>(C))); break;



#define CLAUSE_NO_CLASS(Enum, Str) case llvm::omp::Clause::Enum: break;


#include "llvm/Frontend/OpenMP/OMP.inc"
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPClauseWithPreInit(
OMPClauseWithPreInit *Node) {
TRY_TO(TraverseStmt(Node->getPreInitStmt()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPClauseWithPostUpdate(
OMPClauseWithPostUpdate *Node) {
TRY_TO(VisitOMPClauseWithPreInit(Node));
TRY_TO(TraverseStmt(Node->getPostUpdateExpr()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPAllocatorClause(
OMPAllocatorClause *C) {
TRY_TO(TraverseStmt(C->getAllocator()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPAllocateClause(OMPAllocateClause *C) {
TRY_TO(TraverseStmt(C->getAllocator()));
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPIfClause(OMPIfClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getCondition()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPFinalClause(OMPFinalClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getCondition()));
return true;
}

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::VisitOMPNumThreadsClause(OMPNumThreadsClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getNumThreads()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPSafelenClause(OMPSafelenClause *C) {
TRY_TO(TraverseStmt(C->getSafelen()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPSimdlenClause(OMPSimdlenClause *C) {
TRY_TO(TraverseStmt(C->getSimdlen()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPSizesClause(OMPSizesClause *C) {
for (Expr *E : C->getSizesRefs())
TRY_TO(TraverseStmt(E));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPFullClause(OMPFullClause *C) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPPartialClause(OMPPartialClause *C) {
TRY_TO(TraverseStmt(C->getFactor()));
return true;
}

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::VisitOMPCollapseClause(OMPCollapseClause *C) {
TRY_TO(TraverseStmt(C->getNumForLoops()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDefaultClause(OMPDefaultClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPProcBindClause(OMPProcBindClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUnifiedAddressClause(
OMPUnifiedAddressClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUnifiedSharedMemoryClause(
OMPUnifiedSharedMemoryClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPReverseOffloadClause(
OMPReverseOffloadClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDynamicAllocatorsClause(
OMPDynamicAllocatorsClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPAtomicDefaultMemOrderClause(
OMPAtomicDefaultMemOrderClause *) {
return true;
}

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::VisitOMPScheduleClause(OMPScheduleClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getChunkSize()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPOrderedClause(OMPOrderedClause *C) {
TRY_TO(TraverseStmt(C->getNumForLoops()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPNowaitClause(OMPNowaitClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUntiedClause(OMPUntiedClause *) {
return true;
}

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::VisitOMPMergeableClause(OMPMergeableClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPReadClause(OMPReadClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPWriteClause(OMPWriteClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUpdateClause(OMPUpdateClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPCaptureClause(OMPCaptureClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPSeqCstClause(OMPSeqCstClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPAcqRelClause(OMPAcqRelClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPAcquireClause(OMPAcquireClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPReleaseClause(OMPReleaseClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPRelaxedClause(OMPRelaxedClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPThreadsClause(OMPThreadsClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPSIMDClause(OMPSIMDClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPNogroupClause(OMPNogroupClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPInitClause(OMPInitClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUseClause(OMPUseClause *C) {
TRY_TO(TraverseStmt(C->getInteropVar()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDestroyClause(OMPDestroyClause *C) {
TRY_TO(TraverseStmt(C->getInteropVar()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPNovariantsClause(
OMPNovariantsClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getCondition()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPNocontextClause(
OMPNocontextClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getCondition()));
return true;
}

template <typename Derived>
template <typename T>
bool RecursiveASTVisitor<Derived>::VisitOMPClauseList(T *Node) {
for (auto *E : Node->varlists()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPInclusiveClause(
OMPInclusiveClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPExclusiveClause(
OMPExclusiveClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPPrivateClause(OMPPrivateClause *C) {
TRY_TO(VisitOMPClauseList(C));
for (auto *E : C->private_copies()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPFirstprivateClause(
OMPFirstprivateClause *C) {
TRY_TO(VisitOMPClauseList(C));
TRY_TO(VisitOMPClauseWithPreInit(C));
for (auto *E : C->private_copies()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->inits()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPLastprivateClause(
OMPLastprivateClause *C) {
TRY_TO(VisitOMPClauseList(C));
TRY_TO(VisitOMPClauseWithPostUpdate(C));
for (auto *E : C->private_copies()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->source_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->destination_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->assignment_ops()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPSharedClause(OMPSharedClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPLinearClause(OMPLinearClause *C) {
TRY_TO(TraverseStmt(C->getStep()));
TRY_TO(TraverseStmt(C->getCalcStep()));
TRY_TO(VisitOMPClauseList(C));
TRY_TO(VisitOMPClauseWithPostUpdate(C));
for (auto *E : C->privates()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->inits()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->updates()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->finals()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPAlignedClause(OMPAlignedClause *C) {
TRY_TO(TraverseStmt(C->getAlignment()));
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPCopyinClause(OMPCopyinClause *C) {
TRY_TO(VisitOMPClauseList(C));
for (auto *E : C->source_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->destination_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->assignment_ops()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPCopyprivateClause(
OMPCopyprivateClause *C) {
TRY_TO(VisitOMPClauseList(C));
for (auto *E : C->source_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->destination_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->assignment_ops()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::VisitOMPReductionClause(OMPReductionClause *C) {
TRY_TO(TraverseNestedNameSpecifierLoc(C->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(C->getNameInfo()));
TRY_TO(VisitOMPClauseList(C));
TRY_TO(VisitOMPClauseWithPostUpdate(C));
for (auto *E : C->privates()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->lhs_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->rhs_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->reduction_ops()) {
TRY_TO(TraverseStmt(E));
}
if (C->getModifier() == OMPC_REDUCTION_inscan) {
for (auto *E : C->copy_ops()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->copy_array_temps()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->copy_array_elems()) {
TRY_TO(TraverseStmt(E));
}
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPTaskReductionClause(
OMPTaskReductionClause *C) {
TRY_TO(TraverseNestedNameSpecifierLoc(C->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(C->getNameInfo()));
TRY_TO(VisitOMPClauseList(C));
TRY_TO(VisitOMPClauseWithPostUpdate(C));
for (auto *E : C->privates()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->lhs_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->rhs_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->reduction_ops()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPInReductionClause(
OMPInReductionClause *C) {
TRY_TO(TraverseNestedNameSpecifierLoc(C->getQualifierLoc()));
TRY_TO(TraverseDeclarationNameInfo(C->getNameInfo()));
TRY_TO(VisitOMPClauseList(C));
TRY_TO(VisitOMPClauseWithPostUpdate(C));
for (auto *E : C->privates()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->lhs_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->rhs_exprs()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->reduction_ops()) {
TRY_TO(TraverseStmt(E));
}
for (auto *E : C->taskgroup_descriptors())
TRY_TO(TraverseStmt(E));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPFlushClause(OMPFlushClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDepobjClause(OMPDepobjClause *C) {
TRY_TO(TraverseStmt(C->getDepobj()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDependClause(OMPDependClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDeviceClause(OMPDeviceClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getDevice()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPMapClause(OMPMapClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPNumTeamsClause(
OMPNumTeamsClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getNumTeams()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPThreadLimitClause(
OMPThreadLimitClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getThreadLimit()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPPriorityClause(
OMPPriorityClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getPriority()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPGrainsizeClause(
OMPGrainsizeClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getGrainsize()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPNumTasksClause(
OMPNumTasksClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getNumTasks()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPHintClause(OMPHintClause *C) {
TRY_TO(TraverseStmt(C->getHint()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDistScheduleClause(
OMPDistScheduleClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getChunkSize()));
return true;
}

template <typename Derived>
bool
RecursiveASTVisitor<Derived>::VisitOMPDefaultmapClause(OMPDefaultmapClause *C) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPToClause(OMPToClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPFromClause(OMPFromClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUseDevicePtrClause(
OMPUseDevicePtrClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUseDeviceAddrClause(
OMPUseDeviceAddrClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPIsDevicePtrClause(
OMPIsDevicePtrClause *C) {
TRY_TO(VisitOMPClauseList(C));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPNontemporalClause(
OMPNontemporalClause *C) {
TRY_TO(VisitOMPClauseList(C));
for (auto *E : C->private_refs()) {
TRY_TO(TraverseStmt(E));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPOrderClause(OMPOrderClause *) {
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPDetachClause(OMPDetachClause *C) {
TRY_TO(TraverseStmt(C->getEventHandler()));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPUsesAllocatorsClause(
OMPUsesAllocatorsClause *C) {
for (unsigned I = 0, E = C->getNumberOfAllocators(); I < E; ++I) {
const OMPUsesAllocatorsClause::Data Data = C->getAllocatorData(I);
TRY_TO(TraverseStmt(Data.Allocator));
TRY_TO(TraverseStmt(Data.AllocatorTraits));
}
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPAffinityClause(
OMPAffinityClause *C) {
TRY_TO(TraverseStmt(C->getModifier()));
for (Expr *E : C->varlists())
TRY_TO(TraverseStmt(E));
return true;
}

template <typename Derived>
bool RecursiveASTVisitor<Derived>::VisitOMPFilterClause(OMPFilterClause *C) {
TRY_TO(VisitOMPClauseWithPreInit(C));
TRY_TO(TraverseStmt(C->getThreadID()));
return true;
}


















#undef DEF_TRAVERSE_STMT
#undef TRAVERSE_STMT
#undef TRAVERSE_STMT_BASE

#undef TRY_TO

}

#endif
