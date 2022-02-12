











#if !defined(LLVM_CLANG_AST_INTERP_BYTECODESTMTGEN_H)
#define LLVM_CLANG_AST_INTERP_BYTECODESTMTGEN_H

#include "ByteCodeEmitter.h"
#include "ByteCodeExprGen.h"
#include "EvalEmitter.h"
#include "Pointer.h"
#include "PrimType.h"
#include "Record.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/StmtVisitor.h"
#include "llvm/ADT/Optional.h"

namespace clang {
class QualType;

namespace interp {
class Function;
class State;

template <class Emitter> class LoopScope;
template <class Emitter> class SwitchScope;
template <class Emitter> class LabelScope;


template <class Emitter>
class ByteCodeStmtGen : public ByteCodeExprGen<Emitter> {
using LabelTy = typename Emitter::LabelTy;
using AddrTy = typename Emitter::AddrTy;
using OptLabelTy = llvm::Optional<LabelTy>;
using CaseMap = llvm::DenseMap<const SwitchCase *, LabelTy>;

public:
template<typename... Tys>
ByteCodeStmtGen(Tys&&... Args)
: ByteCodeExprGen<Emitter>(std::forward<Tys>(Args)...) {}

protected:
bool visitFunc(const FunctionDecl *F) override;

private:
friend class LabelScope<Emitter>;
friend class LoopScope<Emitter>;
friend class SwitchScope<Emitter>;


bool visitStmt(const Stmt *S);
bool visitCompoundStmt(const CompoundStmt *S);
bool visitDeclStmt(const DeclStmt *DS);
bool visitReturnStmt(const ReturnStmt *RS);
bool visitIfStmt(const IfStmt *IS);


bool visitVarDecl(const VarDecl *VD);

private:

llvm::Optional<PrimType> ReturnType;


CaseMap CaseLabels;


OptLabelTy BreakLabel;

OptLabelTy ContinueLabel;

OptLabelTy DefaultLabel;
};

extern template class ByteCodeExprGen<EvalEmitter>;

}
}

#endif
