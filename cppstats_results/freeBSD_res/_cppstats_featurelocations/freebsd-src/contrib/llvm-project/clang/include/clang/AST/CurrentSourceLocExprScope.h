













#if !defined(LLVM_CLANG_AST_CURRENT_SOURCE_LOC_EXPR_SCOPE_H)
#define LLVM_CLANG_AST_CURRENT_SOURCE_LOC_EXPR_SCOPE_H

#include <cassert>

namespace clang {
class Expr;




class CurrentSourceLocExprScope {

const Expr *DefaultExpr = nullptr;

public:



class SourceLocExprScopeGuard;

const Expr *getDefaultExpr() const { return DefaultExpr; }

explicit CurrentSourceLocExprScope() = default;

private:
explicit CurrentSourceLocExprScope(const Expr *DefaultExpr)
: DefaultExpr(DefaultExpr) {}

CurrentSourceLocExprScope(CurrentSourceLocExprScope const &) = default;
CurrentSourceLocExprScope &
operator=(CurrentSourceLocExprScope const &) = default;
};

class CurrentSourceLocExprScope::SourceLocExprScopeGuard {
public:
SourceLocExprScopeGuard(const Expr *DefaultExpr,
CurrentSourceLocExprScope &Current)
: Current(Current), OldVal(Current), Enable(false) {
assert(DefaultExpr && "the new scope should not be empty");
if ((Enable = (Current.getDefaultExpr() == nullptr)))
Current = CurrentSourceLocExprScope(DefaultExpr);
}

~SourceLocExprScopeGuard() {
if (Enable)
Current = OldVal;
}

private:
SourceLocExprScopeGuard(SourceLocExprScopeGuard const &) = delete;
SourceLocExprScopeGuard &operator=(SourceLocExprScopeGuard const &) = delete;

CurrentSourceLocExprScope &Current;
CurrentSourceLocExprScope OldVal;
bool Enable;
};

}

#endif
