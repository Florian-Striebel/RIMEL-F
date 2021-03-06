











#if !defined(LLVM_CLANG_AST_STMTCXX_H)
#define LLVM_CLANG_AST_STMTCXX_H

#include "clang/AST/DeclarationName.h"
#include "clang/AST/Expr.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/Stmt.h"
#include "llvm/Support/Compiler.h"

namespace clang {

class VarDecl;



class CXXCatchStmt : public Stmt {
SourceLocation CatchLoc;

VarDecl *ExceptionDecl;

Stmt *HandlerBlock;

public:
CXXCatchStmt(SourceLocation catchLoc, VarDecl *exDecl, Stmt *handlerBlock)
: Stmt(CXXCatchStmtClass), CatchLoc(catchLoc), ExceptionDecl(exDecl),
HandlerBlock(handlerBlock) {}

CXXCatchStmt(EmptyShell Empty)
: Stmt(CXXCatchStmtClass), ExceptionDecl(nullptr), HandlerBlock(nullptr) {}

SourceLocation getBeginLoc() const LLVM_READONLY { return CatchLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return HandlerBlock->getEndLoc();
}

SourceLocation getCatchLoc() const { return CatchLoc; }
VarDecl *getExceptionDecl() const { return ExceptionDecl; }
QualType getCaughtType() const;
Stmt *getHandlerBlock() const { return HandlerBlock; }

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXCatchStmtClass;
}

child_range children() { return child_range(&HandlerBlock, &HandlerBlock+1); }

const_child_range children() const {
return const_child_range(&HandlerBlock, &HandlerBlock + 1);
}

friend class ASTStmtReader;
};



class CXXTryStmt final : public Stmt,
private llvm::TrailingObjects<CXXTryStmt, Stmt *> {

friend TrailingObjects;
friend class ASTStmtReader;

SourceLocation TryLoc;
unsigned NumHandlers;
size_t numTrailingObjects(OverloadToken<Stmt *>) const { return NumHandlers; }

CXXTryStmt(SourceLocation tryLoc, Stmt *tryBlock, ArrayRef<Stmt*> handlers);
CXXTryStmt(EmptyShell Empty, unsigned numHandlers)
: Stmt(CXXTryStmtClass), NumHandlers(numHandlers) { }

Stmt *const *getStmts() const { return getTrailingObjects<Stmt *>(); }
Stmt **getStmts() { return getTrailingObjects<Stmt *>(); }

public:
static CXXTryStmt *Create(const ASTContext &C, SourceLocation tryLoc,
Stmt *tryBlock, ArrayRef<Stmt*> handlers);

static CXXTryStmt *Create(const ASTContext &C, EmptyShell Empty,
unsigned numHandlers);

SourceLocation getBeginLoc() const LLVM_READONLY { return getTryLoc(); }

SourceLocation getTryLoc() const { return TryLoc; }
SourceLocation getEndLoc() const {
return getStmts()[NumHandlers]->getEndLoc();
}

CompoundStmt *getTryBlock() {
return cast<CompoundStmt>(getStmts()[0]);
}
const CompoundStmt *getTryBlock() const {
return cast<CompoundStmt>(getStmts()[0]);
}

unsigned getNumHandlers() const { return NumHandlers; }
CXXCatchStmt *getHandler(unsigned i) {
return cast<CXXCatchStmt>(getStmts()[i + 1]);
}
const CXXCatchStmt *getHandler(unsigned i) const {
return cast<CXXCatchStmt>(getStmts()[i + 1]);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXTryStmtClass;
}

child_range children() {
return child_range(getStmts(), getStmts() + getNumHandlers() + 1);
}

const_child_range children() const {
return const_child_range(getStmts(), getStmts() + getNumHandlers() + 1);
}
};








class CXXForRangeStmt : public Stmt {
SourceLocation ForLoc;
enum { INIT, RANGE, BEGINSTMT, ENDSTMT, COND, INC, LOOPVAR, BODY, END };


Stmt *SubExprs[END];
SourceLocation CoawaitLoc;
SourceLocation ColonLoc;
SourceLocation RParenLoc;

friend class ASTStmtReader;
public:
CXXForRangeStmt(Stmt *InitStmt, DeclStmt *Range, DeclStmt *Begin,
DeclStmt *End, Expr *Cond, Expr *Inc, DeclStmt *LoopVar,
Stmt *Body, SourceLocation FL, SourceLocation CAL,
SourceLocation CL, SourceLocation RPL);
CXXForRangeStmt(EmptyShell Empty) : Stmt(CXXForRangeStmtClass, Empty) { }

Stmt *getInit() { return SubExprs[INIT]; }
VarDecl *getLoopVariable();
Expr *getRangeInit();

const Stmt *getInit() const { return SubExprs[INIT]; }
const VarDecl *getLoopVariable() const;
const Expr *getRangeInit() const;


DeclStmt *getRangeStmt() { return cast<DeclStmt>(SubExprs[RANGE]); }
DeclStmt *getBeginStmt() {
return cast_or_null<DeclStmt>(SubExprs[BEGINSTMT]);
}
DeclStmt *getEndStmt() { return cast_or_null<DeclStmt>(SubExprs[ENDSTMT]); }
Expr *getCond() { return cast_or_null<Expr>(SubExprs[COND]); }
Expr *getInc() { return cast_or_null<Expr>(SubExprs[INC]); }
DeclStmt *getLoopVarStmt() { return cast<DeclStmt>(SubExprs[LOOPVAR]); }
Stmt *getBody() { return SubExprs[BODY]; }

const DeclStmt *getRangeStmt() const {
return cast<DeclStmt>(SubExprs[RANGE]);
}
const DeclStmt *getBeginStmt() const {
return cast_or_null<DeclStmt>(SubExprs[BEGINSTMT]);
}
const DeclStmt *getEndStmt() const {
return cast_or_null<DeclStmt>(SubExprs[ENDSTMT]);
}
const Expr *getCond() const {
return cast_or_null<Expr>(SubExprs[COND]);
}
const Expr *getInc() const {
return cast_or_null<Expr>(SubExprs[INC]);
}
const DeclStmt *getLoopVarStmt() const {
return cast<DeclStmt>(SubExprs[LOOPVAR]);
}
const Stmt *getBody() const { return SubExprs[BODY]; }

void setInit(Stmt *S) { SubExprs[INIT] = S; }
void setRangeInit(Expr *E) { SubExprs[RANGE] = reinterpret_cast<Stmt*>(E); }
void setRangeStmt(Stmt *S) { SubExprs[RANGE] = S; }
void setBeginStmt(Stmt *S) { SubExprs[BEGINSTMT] = S; }
void setEndStmt(Stmt *S) { SubExprs[ENDSTMT] = S; }
void setCond(Expr *E) { SubExprs[COND] = reinterpret_cast<Stmt*>(E); }
void setInc(Expr *E) { SubExprs[INC] = reinterpret_cast<Stmt*>(E); }
void setLoopVarStmt(Stmt *S) { SubExprs[LOOPVAR] = S; }
void setBody(Stmt *S) { SubExprs[BODY] = S; }

SourceLocation getForLoc() const { return ForLoc; }
SourceLocation getCoawaitLoc() const { return CoawaitLoc; }
SourceLocation getColonLoc() const { return ColonLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return ForLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return SubExprs[BODY]->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CXXForRangeStmtClass;
}


child_range children() {
return child_range(&SubExprs[0], &SubExprs[END]);
}

const_child_range children() const {
return const_child_range(&SubExprs[0], &SubExprs[END]);
}
};




























class MSDependentExistsStmt : public Stmt {
SourceLocation KeywordLoc;
bool IsIfExists;
NestedNameSpecifierLoc QualifierLoc;
DeclarationNameInfo NameInfo;
Stmt *SubStmt;

friend class ASTReader;
friend class ASTStmtReader;

public:
MSDependentExistsStmt(SourceLocation KeywordLoc, bool IsIfExists,
NestedNameSpecifierLoc QualifierLoc,
DeclarationNameInfo NameInfo,
CompoundStmt *SubStmt)
: Stmt(MSDependentExistsStmtClass),
KeywordLoc(KeywordLoc), IsIfExists(IsIfExists),
QualifierLoc(QualifierLoc), NameInfo(NameInfo),
SubStmt(reinterpret_cast<Stmt *>(SubStmt)) { }



SourceLocation getKeywordLoc() const { return KeywordLoc; }


bool isIfExists() const { return IsIfExists; }


bool isIfNotExists() const { return !IsIfExists; }



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }



DeclarationNameInfo getNameInfo() const { return NameInfo; }



CompoundStmt *getSubStmt() const {
return reinterpret_cast<CompoundStmt *>(SubStmt);
}

SourceLocation getBeginLoc() const LLVM_READONLY { return KeywordLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return SubStmt->getEndLoc();
}

child_range children() {
return child_range(&SubStmt, &SubStmt+1);
}

const_child_range children() const {
return const_child_range(&SubStmt, &SubStmt + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == MSDependentExistsStmtClass;
}
};




class CoroutineBodyStmt final
: public Stmt,
private llvm::TrailingObjects<CoroutineBodyStmt, Stmt *> {
enum SubStmt {
Body,
Promise,
InitSuspend,
FinalSuspend,
OnException,
OnFallthrough,
Allocate,
Deallocate,
ReturnValue,
ResultDecl,
ReturnStmt,
ReturnStmtOnAllocFailure,
FirstParamMove
};
unsigned NumParams;

friend class ASTStmtReader;
friend class ASTReader;
friend TrailingObjects;

Stmt **getStoredStmts() { return getTrailingObjects<Stmt *>(); }

Stmt *const *getStoredStmts() const { return getTrailingObjects<Stmt *>(); }

public:

struct CtorArgs {
Stmt *Body = nullptr;
Stmt *Promise = nullptr;
Expr *InitialSuspend = nullptr;
Expr *FinalSuspend = nullptr;
Stmt *OnException = nullptr;
Stmt *OnFallthrough = nullptr;
Expr *Allocate = nullptr;
Expr *Deallocate = nullptr;
Expr *ReturnValue = nullptr;
Stmt *ResultDecl = nullptr;
Stmt *ReturnStmt = nullptr;
Stmt *ReturnStmtOnAllocFailure = nullptr;
ArrayRef<Stmt *> ParamMoves;
};

private:

CoroutineBodyStmt(CtorArgs const& Args);

public:
static CoroutineBodyStmt *Create(const ASTContext &C, CtorArgs const &Args);
static CoroutineBodyStmt *Create(const ASTContext &C, EmptyShell,
unsigned NumParams);

bool hasDependentPromiseType() const {
return getPromiseDecl()->getType()->isDependentType();
}



Stmt *getBody() const {
return getStoredStmts()[SubStmt::Body];
}

Stmt *getPromiseDeclStmt() const {
return getStoredStmts()[SubStmt::Promise];
}
VarDecl *getPromiseDecl() const {
return cast<VarDecl>(cast<DeclStmt>(getPromiseDeclStmt())->getSingleDecl());
}

Stmt *getInitSuspendStmt() const {
return getStoredStmts()[SubStmt::InitSuspend];
}
Stmt *getFinalSuspendStmt() const {
return getStoredStmts()[SubStmt::FinalSuspend];
}

Stmt *getExceptionHandler() const {
return getStoredStmts()[SubStmt::OnException];
}
Stmt *getFallthroughHandler() const {
return getStoredStmts()[SubStmt::OnFallthrough];
}

Expr *getAllocate() const {
return cast_or_null<Expr>(getStoredStmts()[SubStmt::Allocate]);
}
Expr *getDeallocate() const {
return cast_or_null<Expr>(getStoredStmts()[SubStmt::Deallocate]);
}
Expr *getReturnValueInit() const {
return cast<Expr>(getStoredStmts()[SubStmt::ReturnValue]);
}
Stmt *getResultDecl() const { return getStoredStmts()[SubStmt::ResultDecl]; }
Stmt *getReturnStmt() const { return getStoredStmts()[SubStmt::ReturnStmt]; }
Stmt *getReturnStmtOnAllocFailure() const {
return getStoredStmts()[SubStmt::ReturnStmtOnAllocFailure];
}
ArrayRef<Stmt const *> getParamMoves() const {
return {getStoredStmts() + SubStmt::FirstParamMove, NumParams};
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return getBody() ? getBody()->getBeginLoc()
: getPromiseDecl()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return getBody() ? getBody()->getEndLoc() : getPromiseDecl()->getEndLoc();
}

child_range children() {
return child_range(getStoredStmts(),
getStoredStmts() + SubStmt::FirstParamMove + NumParams);
}

const_child_range children() const {
return const_child_range(getStoredStmts(), getStoredStmts() +
SubStmt::FirstParamMove +
NumParams);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CoroutineBodyStmtClass;
}
};












class CoreturnStmt : public Stmt {
SourceLocation CoreturnLoc;

enum SubStmt { Operand, PromiseCall, Count };
Stmt *SubStmts[SubStmt::Count];

bool IsImplicit : 1;

friend class ASTStmtReader;
public:
CoreturnStmt(SourceLocation CoreturnLoc, Stmt *Operand, Stmt *PromiseCall,
bool IsImplicit = false)
: Stmt(CoreturnStmtClass), CoreturnLoc(CoreturnLoc),
IsImplicit(IsImplicit) {
SubStmts[SubStmt::Operand] = Operand;
SubStmts[SubStmt::PromiseCall] = PromiseCall;
}

CoreturnStmt(EmptyShell) : CoreturnStmt({}, {}, {}) {}

SourceLocation getKeywordLoc() const { return CoreturnLoc; }



Expr *getOperand() const { return static_cast<Expr*>(SubStmts[Operand]); }




Expr *getPromiseCall() const {
return static_cast<Expr*>(SubStmts[PromiseCall]);
}

bool isImplicit() const { return IsImplicit; }
void setIsImplicit(bool value = true) { IsImplicit = value; }

SourceLocation getBeginLoc() const LLVM_READONLY { return CoreturnLoc; }
SourceLocation getEndLoc() const LLVM_READONLY {
return getOperand() ? getOperand()->getEndLoc() : getBeginLoc();
}

child_range children() {
if (!getOperand())
return child_range(SubStmts + SubStmt::PromiseCall,
SubStmts + SubStmt::Count);
return child_range(SubStmts, SubStmts + SubStmt::Count);
}

const_child_range children() const {
if (!getOperand())
return const_child_range(SubStmts + SubStmt::PromiseCall,
SubStmts + SubStmt::Count);
return const_child_range(SubStmts, SubStmts + SubStmt::Count);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == CoreturnStmtClass;
}
};

}

#endif
