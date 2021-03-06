











#if !defined(LLVM_CLANG_LIB_SEMA_COROUTINESTMTBUILDER_H)
#define LLVM_CLANG_LIB_SEMA_COROUTINESTMTBUILDER_H

#include "clang/AST/Decl.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/StmtCXX.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/SemaInternal.h"

namespace clang {

class CoroutineStmtBuilder : public CoroutineBodyStmt::CtorArgs {
Sema &S;
FunctionDecl &FD;
sema::FunctionScopeInfo &Fn;
bool IsValid = true;
SourceLocation Loc;
SmallVector<Stmt *, 4> ParamMovesVector;
const bool IsPromiseDependentType;
CXXRecordDecl *PromiseRecordDecl = nullptr;

public:


CoroutineStmtBuilder(Sema &S, FunctionDecl &FD, sema::FunctionScopeInfo &Fn,
Stmt *Body);



bool buildStatements();








bool buildDependentStatements();

bool isInvalid() const { return !this->IsValid; }

private:
bool makePromiseStmt();
bool makeInitialAndFinalSuspend();
bool makeNewAndDeleteExpr();
bool makeOnFallthrough();
bool makeOnException();
bool makeReturnObject();
bool makeGroDeclAndReturnStmt();
bool makeReturnOnAllocFailure();
};

}

#endif
