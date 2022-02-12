










#if !defined(LLVM_CLANG_AST_STMTOBJC_H)
#define LLVM_CLANG_AST_STMTOBJC_H

#include "clang/AST/Stmt.h"
#include "llvm/Support/Compiler.h"

namespace clang {




class ObjCForCollectionStmt : public Stmt {
enum { ELEM, COLLECTION, BODY, END_EXPR };
Stmt* SubExprs[END_EXPR];
SourceLocation ForLoc;
SourceLocation RParenLoc;
public:
ObjCForCollectionStmt(Stmt *Elem, Expr *Collect, Stmt *Body,
SourceLocation FCL, SourceLocation RPL);
explicit ObjCForCollectionStmt(EmptyShell Empty) :
Stmt(ObjCForCollectionStmtClass, Empty) { }

Stmt *getElement() { return SubExprs[ELEM]; }
Expr *getCollection() {
return reinterpret_cast<Expr*>(SubExprs[COLLECTION]);
}
Stmt *getBody() { return SubExprs[BODY]; }

const Stmt *getElement() const { return SubExprs[ELEM]; }
const Expr *getCollection() const {
return reinterpret_cast<Expr*>(SubExprs[COLLECTION]);
}
const Stmt *getBody() const { return SubExprs[BODY]; }

void setElement(Stmt *S) { SubExprs[ELEM] = S; }
void setCollection(Expr *E) {
SubExprs[COLLECTION] = reinterpret_cast<Stmt*>(E);
}
void setBody(Stmt *S) { SubExprs[BODY] = S; }

SourceLocation getForLoc() const { return ForLoc; }
void setForLoc(SourceLocation Loc) { ForLoc = Loc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation Loc) { RParenLoc = Loc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return ForLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return SubExprs[BODY]->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCForCollectionStmtClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[END_EXPR]);
}

const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[END_EXPR]);
}
};


class ObjCAtCatchStmt : public Stmt {
private:
VarDecl *ExceptionDecl;
Stmt *Body;
SourceLocation AtCatchLoc, RParenLoc;

public:
ObjCAtCatchStmt(SourceLocation atCatchLoc, SourceLocation rparenloc,
VarDecl *catchVarDecl,
Stmt *atCatchStmt)
: Stmt(ObjCAtCatchStmtClass), ExceptionDecl(catchVarDecl),
Body(atCatchStmt), AtCatchLoc(atCatchLoc), RParenLoc(rparenloc) { }

explicit ObjCAtCatchStmt(EmptyShell Empty) :
Stmt(ObjCAtCatchStmtClass, Empty) { }

const Stmt *getCatchBody() const { return Body; }
Stmt *getCatchBody() { return Body; }
void setCatchBody(Stmt *S) { Body = S; }

const VarDecl *getCatchParamDecl() const {
return ExceptionDecl;
}
VarDecl *getCatchParamDecl() {
return ExceptionDecl;
}
void setCatchParamDecl(VarDecl *D) { ExceptionDecl = D; }

SourceLocation getAtCatchLoc() const { return AtCatchLoc; }
void setAtCatchLoc(SourceLocation Loc) { AtCatchLoc = Loc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation Loc) { RParenLoc = Loc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtCatchLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return Body->getEndLoc(); }

bool hasEllipsis() const { return getCatchParamDecl() == nullptr; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCAtCatchStmtClass;
}

child_range children() { return child_range(&Body, &Body + 1); }

const_child_range children() const {
return const_child_range(&Body, &Body + 1);
}
};


class ObjCAtFinallyStmt : public Stmt {
SourceLocation AtFinallyLoc;
Stmt *AtFinallyStmt;

public:
ObjCAtFinallyStmt(SourceLocation atFinallyLoc, Stmt *atFinallyStmt)
: Stmt(ObjCAtFinallyStmtClass), AtFinallyLoc(atFinallyLoc),
AtFinallyStmt(atFinallyStmt) {}

explicit ObjCAtFinallyStmt(EmptyShell Empty) :
Stmt(ObjCAtFinallyStmtClass, Empty) { }

const Stmt *getFinallyBody() const { return AtFinallyStmt; }
Stmt *getFinallyBody() { return AtFinallyStmt; }
void setFinallyBody(Stmt *S) { AtFinallyStmt = S; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtFinallyLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return AtFinallyStmt->getEndLoc();
}

SourceLocation getAtFinallyLoc() const { return AtFinallyLoc; }
void setAtFinallyLoc(SourceLocation Loc) { AtFinallyLoc = Loc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCAtFinallyStmtClass;
}

child_range children() {
return child_range(&AtFinallyStmt, &AtFinallyStmt+1);
}

const_child_range children() const {
return const_child_range(&AtFinallyStmt, &AtFinallyStmt + 1);
}
};


class ObjCAtTryStmt : public Stmt {
private:

SourceLocation AtTryLoc;


unsigned NumCatchStmts : 16;


bool HasFinally : 1;






Stmt **getStmts() { return reinterpret_cast<Stmt **> (this + 1); }
const Stmt* const *getStmts() const {
return reinterpret_cast<const Stmt * const*> (this + 1);
}

ObjCAtTryStmt(SourceLocation atTryLoc, Stmt *atTryStmt,
Stmt **CatchStmts, unsigned NumCatchStmts,
Stmt *atFinallyStmt);

explicit ObjCAtTryStmt(EmptyShell Empty, unsigned NumCatchStmts,
bool HasFinally)
: Stmt(ObjCAtTryStmtClass, Empty), NumCatchStmts(NumCatchStmts),
HasFinally(HasFinally) { }

public:
static ObjCAtTryStmt *Create(const ASTContext &Context,
SourceLocation atTryLoc, Stmt *atTryStmt,
Stmt **CatchStmts, unsigned NumCatchStmts,
Stmt *atFinallyStmt);
static ObjCAtTryStmt *CreateEmpty(const ASTContext &Context,
unsigned NumCatchStmts, bool HasFinally);


SourceLocation getAtTryLoc() const { return AtTryLoc; }
void setAtTryLoc(SourceLocation Loc) { AtTryLoc = Loc; }


const Stmt *getTryBody() const { return getStmts()[0]; }
Stmt *getTryBody() { return getStmts()[0]; }
void setTryBody(Stmt *S) { getStmts()[0] = S; }



unsigned getNumCatchStmts() const { return NumCatchStmts; }


const ObjCAtCatchStmt *getCatchStmt(unsigned I) const {
assert(I < NumCatchStmts && "Out-of-bounds @catch index");
return cast_or_null<ObjCAtCatchStmt>(getStmts()[I + 1]);
}


ObjCAtCatchStmt *getCatchStmt(unsigned I) {
assert(I < NumCatchStmts && "Out-of-bounds @catch index");
return cast_or_null<ObjCAtCatchStmt>(getStmts()[I + 1]);
}


void setCatchStmt(unsigned I, ObjCAtCatchStmt *S) {
assert(I < NumCatchStmts && "Out-of-bounds @catch index");
getStmts()[I + 1] = S;
}


const ObjCAtFinallyStmt *getFinallyStmt() const {
if (!HasFinally)
return nullptr;

return cast_or_null<ObjCAtFinallyStmt>(getStmts()[1 + NumCatchStmts]);
}
ObjCAtFinallyStmt *getFinallyStmt() {
if (!HasFinally)
return nullptr;

return cast_or_null<ObjCAtFinallyStmt>(getStmts()[1 + NumCatchStmts]);
}
void setFinallyStmt(Stmt *S) {
assert(HasFinally && "@try does not have a @finally slot!");
getStmts()[1 + NumCatchStmts] = S;
}

SourceLocation getBeginLoc() const LLVM_READONLY { return AtTryLoc; }
SourceLocation getEndLoc() const LLVM_READONLY;

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCAtTryStmtClass;
}

child_range children() {
return child_range(getStmts(),
getStmts() + 1 + NumCatchStmts + HasFinally);
}

const_child_range children() const {
return const_child_range(const_cast<ObjCAtTryStmt *>(this)->children());
}
};









class ObjCAtSynchronizedStmt : public Stmt {
private:
SourceLocation AtSynchronizedLoc;
enum { SYNC_EXPR, SYNC_BODY, END_EXPR };
Stmt* SubStmts[END_EXPR];

public:
ObjCAtSynchronizedStmt(SourceLocation atSynchronizedLoc, Stmt *synchExpr,
Stmt *synchBody)
: Stmt(ObjCAtSynchronizedStmtClass) {
SubStmts[SYNC_EXPR] = synchExpr;
SubStmts[SYNC_BODY] = synchBody;
AtSynchronizedLoc = atSynchronizedLoc;
}
explicit ObjCAtSynchronizedStmt(EmptyShell Empty) :
Stmt(ObjCAtSynchronizedStmtClass, Empty) { }

SourceLocation getAtSynchronizedLoc() const { return AtSynchronizedLoc; }
void setAtSynchronizedLoc(SourceLocation Loc) { AtSynchronizedLoc = Loc; }

const CompoundStmt *getSynchBody() const {
return reinterpret_cast<CompoundStmt*>(SubStmts[SYNC_BODY]);
}
CompoundStmt *getSynchBody() {
return reinterpret_cast<CompoundStmt*>(SubStmts[SYNC_BODY]);
}
void setSynchBody(Stmt *S) { SubStmts[SYNC_BODY] = S; }

const Expr *getSynchExpr() const {
return reinterpret_cast<Expr*>(SubStmts[SYNC_EXPR]);
}
Expr *getSynchExpr() {
return reinterpret_cast<Expr*>(SubStmts[SYNC_EXPR]);
}
void setSynchExpr(Stmt *S) { SubStmts[SYNC_EXPR] = S; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtSynchronizedLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return getSynchBody()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCAtSynchronizedStmtClass;
}

child_range children() {
return child_range(&SubStmts[0], &SubStmts[0]+END_EXPR);
}

const_child_range children() const {
return const_child_range(&SubStmts[0], &SubStmts[0] + END_EXPR);
}
};


class ObjCAtThrowStmt : public Stmt {
SourceLocation AtThrowLoc;
Stmt *Throw;

public:
ObjCAtThrowStmt(SourceLocation atThrowLoc, Stmt *throwExpr)
: Stmt(ObjCAtThrowStmtClass), Throw(throwExpr) {
AtThrowLoc = atThrowLoc;
}
explicit ObjCAtThrowStmt(EmptyShell Empty) :
Stmt(ObjCAtThrowStmtClass, Empty) { }

const Expr *getThrowExpr() const { return reinterpret_cast<Expr*>(Throw); }
Expr *getThrowExpr() { return reinterpret_cast<Expr*>(Throw); }
void setThrowExpr(Stmt *S) { Throw = S; }

SourceLocation getThrowLoc() const LLVM_READONLY { return AtThrowLoc; }
void setThrowLoc(SourceLocation Loc) { AtThrowLoc = Loc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtThrowLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return Throw ? Throw->getEndLoc() : AtThrowLoc;
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCAtThrowStmtClass;
}

child_range children() { return child_range(&Throw, &Throw+1); }

const_child_range children() const {
return const_child_range(&Throw, &Throw + 1);
}
};


class ObjCAutoreleasePoolStmt : public Stmt {
SourceLocation AtLoc;
Stmt *SubStmt;

public:
ObjCAutoreleasePoolStmt(SourceLocation atLoc, Stmt *subStmt)
: Stmt(ObjCAutoreleasePoolStmtClass), AtLoc(atLoc), SubStmt(subStmt) {}

explicit ObjCAutoreleasePoolStmt(EmptyShell Empty) :
Stmt(ObjCAutoreleasePoolStmtClass, Empty) { }

const Stmt *getSubStmt() const { return SubStmt; }
Stmt *getSubStmt() { return SubStmt; }
void setSubStmt(Stmt *S) { SubStmt = S; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return SubStmt->getEndLoc();
}

SourceLocation getAtLoc() const { return AtLoc; }
void setAtLoc(SourceLocation Loc) { AtLoc = Loc; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCAutoreleasePoolStmtClass;
}

child_range children() { return child_range(&SubStmt, &SubStmt + 1); }

const_child_range children() const {
return const_child_range(&SubStmt, &SubStmt + 1);
}
};

}

#endif
