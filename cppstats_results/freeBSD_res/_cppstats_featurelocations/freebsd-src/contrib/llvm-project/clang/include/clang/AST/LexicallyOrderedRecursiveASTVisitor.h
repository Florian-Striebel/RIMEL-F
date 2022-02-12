












#if !defined(LLVM_CLANG_AST_LEXICALLY_ORDERED_RECURSIVEASTVISITOR_H)
#define LLVM_CLANG_AST_LEXICALLY_ORDERED_RECURSIVEASTVISITOR_H

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/SaveAndRestore.h"

namespace clang {



































template <typename Derived>
class LexicallyOrderedRecursiveASTVisitor
: public RecursiveASTVisitor<Derived> {
using BaseType = RecursiveASTVisitor<Derived>;

public:
LexicallyOrderedRecursiveASTVisitor(const SourceManager &SM) : SM(SM) {}

bool TraverseObjCImplementationDecl(ObjCImplementationDecl *D) {



bool Result = BaseType::TraverseObjCImplementationDecl(D);
return TraverseAdditionalLexicallyNestedDeclarations() ? Result : false;
}

bool TraverseObjCCategoryImplDecl(ObjCCategoryImplDecl *D) {
bool Result = BaseType::TraverseObjCCategoryImplDecl(D);
return TraverseAdditionalLexicallyNestedDeclarations() ? Result : false;
}

bool TraverseDeclContextHelper(DeclContext *DC) {
if (!DC)
return true;

for (auto I = DC->decls_begin(), E = DC->decls_end(); I != E;) {
Decl *Child = *I;
if (BaseType::canIgnoreChildDeclWhileTraversingDeclContext(Child)) {
++I;
continue;
}
if (!isa<ObjCImplementationDecl>(Child) &&
!isa<ObjCCategoryImplDecl>(Child)) {
if (!BaseType::getDerived().TraverseDecl(Child))
return false;
++I;
continue;
}


LexicallyNestedDeclarations.clear();
for (++I; I != E; ++I) {
Decl *Sibling = *I;
if (!SM.isBeforeInTranslationUnit(Sibling->getBeginLoc(),
Child->getEndLoc()))
break;
if (!BaseType::canIgnoreChildDeclWhileTraversingDeclContext(Sibling))
LexicallyNestedDeclarations.push_back(Sibling);
}
if (!BaseType::getDerived().TraverseDecl(Child))
return false;
}
return true;
}

Stmt::child_range getStmtChildren(Stmt *S) { return S->children(); }

SmallVector<Stmt *, 8> getStmtChildren(CXXOperatorCallExpr *CE) {
SmallVector<Stmt *, 8> Children(CE->children());
bool Swap;


switch (CE->getOperator()) {
case OO_Arrow:
case OO_Call:
case OO_Subscript:
Swap = true;
break;
case OO_PlusPlus:
case OO_MinusMinus:

Swap = Children.size() != 2;
break;
default:
Swap = CE->isInfixBinaryOp();
break;
}
if (Swap && Children.size() > 1)
std::swap(Children[0], Children[1]);
return Children;
}

private:
bool TraverseAdditionalLexicallyNestedDeclarations() {





assert(!BaseType::getDerived().shouldTraversePostOrder() &&
"post-order traversal is not supported for lexically ordered "
"recursive ast visitor");
for (Decl *D : LexicallyNestedDeclarations) {
if (!BaseType::getDerived().TraverseDecl(D))
return false;
}
return true;
}

const SourceManager &SM;
llvm::SmallVector<Decl *, 8> LexicallyNestedDeclarations;
};

}

#endif
