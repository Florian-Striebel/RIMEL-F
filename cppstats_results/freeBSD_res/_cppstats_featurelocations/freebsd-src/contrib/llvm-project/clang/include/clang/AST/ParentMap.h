











#if !defined(LLVM_CLANG_AST_PARENTMAP_H)
#define LLVM_CLANG_AST_PARENTMAP_H

namespace clang {
class Stmt;
class Expr;

class ParentMap {
void* Impl;
public:
ParentMap(Stmt* ASTRoot);
~ParentMap();




void addStmt(Stmt* S);




void setParent(const Stmt *S, const Stmt *Parent);

Stmt *getParent(Stmt*) const;
Stmt *getParentIgnoreParens(Stmt *) const;
Stmt *getParentIgnoreParenCasts(Stmt *) const;
Stmt *getParentIgnoreParenImpCasts(Stmt *) const;
Stmt *getOuterParenParent(Stmt *) const;

const Stmt *getParent(const Stmt* S) const {
return getParent(const_cast<Stmt*>(S));
}

const Stmt *getParentIgnoreParens(const Stmt *S) const {
return getParentIgnoreParens(const_cast<Stmt*>(S));
}

const Stmt *getParentIgnoreParenCasts(const Stmt *S) const {
return getParentIgnoreParenCasts(const_cast<Stmt*>(S));
}

bool hasParent(const Stmt *S) const { return getParent(S) != nullptr; }

bool isConsumedExpr(Expr *E) const;

bool isConsumedExpr(const Expr *E) const {
return isConsumedExpr(const_cast<Expr*>(E));
}
};

}
#endif
