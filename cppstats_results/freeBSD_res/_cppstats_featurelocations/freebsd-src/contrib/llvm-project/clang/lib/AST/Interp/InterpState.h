











#if !defined(LLVM_CLANG_AST_INTERP_INTERPSTATE_H)
#define LLVM_CLANG_AST_INTERP_INTERPSTATE_H

#include "Context.h"
#include "Function.h"
#include "InterpStack.h"
#include "State.h"
#include "clang/AST/APValue.h"
#include "clang/AST/ASTDiagnostic.h"
#include "clang/AST/Expr.h"
#include "clang/AST/OptionalDiagnostic.h"

namespace clang {
namespace interp {
class Context;
class Function;
class InterpStack;
class InterpFrame;
class SourceMapper;


class InterpState final : public State, public SourceMapper {
public:
InterpState(State &Parent, Program &P, InterpStack &Stk, Context &Ctx,
SourceMapper *M = nullptr);

~InterpState();


Frame *getSplitFrame() { return Parent.getCurrentFrame(); }
Frame *getCurrentFrame() override;
unsigned getCallStackDepth() override { return CallStackDepth; }
const Frame *getBottomFrame() const override {
return Parent.getBottomFrame();
}


Expr::EvalStatus &getEvalStatus() const override {
return Parent.getEvalStatus();
}
ASTContext &getCtx() const override { return Parent.getCtx(); }


bool checkingForUndefinedBehavior() const override {
return Parent.checkingForUndefinedBehavior();
}
bool keepEvaluatingAfterFailure() const override {
return Parent.keepEvaluatingAfterFailure();
}
bool checkingPotentialConstantExpression() const override {
return Parent.checkingPotentialConstantExpression();
}
bool noteUndefinedBehavior() override {
return Parent.noteUndefinedBehavior();
}
bool hasActiveDiagnostic() override { return Parent.hasActiveDiagnostic(); }
void setActiveDiagnostic(bool Flag) override {
Parent.setActiveDiagnostic(Flag);
}
void setFoldFailureDiagnostic(bool Flag) override {
Parent.setFoldFailureDiagnostic(Flag);
}
bool hasPriorDiagnostic() override { return Parent.hasPriorDiagnostic(); }


bool reportOverflow(const Expr *E, const llvm::APSInt &Value);


void deallocate(Block *B);


SourceInfo getSource(Function *F, CodePtr PC) const override {
return M ? M->getSource(F, PC) : F->getSource(PC);
}

private:

State &Parent;

DeadBlock *DeadBlocks = nullptr;

SourceMapper *M;

public:

Program &P;

InterpStack &Stk;

Context &Ctx;

InterpFrame *Current = nullptr;

unsigned CallStackDepth;
};

}
}

#endif
