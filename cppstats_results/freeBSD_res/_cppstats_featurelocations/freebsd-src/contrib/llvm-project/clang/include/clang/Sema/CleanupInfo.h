












#if !defined(LLVM_CLANG_SEMA_CLEANUP_INFO_H)
#define LLVM_CLANG_SEMA_CLEANUP_INFO_H

namespace clang {

class CleanupInfo {
bool ExprNeedsCleanups = false;
bool CleanupsHaveSideEffects = false;

public:
bool exprNeedsCleanups() const { return ExprNeedsCleanups; }

bool cleanupsHaveSideEffects() const { return CleanupsHaveSideEffects; }

void setExprNeedsCleanups(bool SideEffects) {
ExprNeedsCleanups = true;
CleanupsHaveSideEffects |= SideEffects;
}

void reset() {
ExprNeedsCleanups = false;
CleanupsHaveSideEffects = false;
}

void mergeFrom(CleanupInfo Rhs) {
ExprNeedsCleanups |= Rhs.ExprNeedsCleanups;
CleanupsHaveSideEffects |= Rhs.CleanupsHaveSideEffects;
}
};

}

#endif
