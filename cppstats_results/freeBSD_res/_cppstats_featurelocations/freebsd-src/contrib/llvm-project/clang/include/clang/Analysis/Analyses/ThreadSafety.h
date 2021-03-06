
















#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETY_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_THREADSAFETY_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/StringRef.h"

namespace clang {

class AnalysisDeclContext;
class FunctionDecl;
class NamedDecl;

namespace threadSafety {

class BeforeSet;



enum ProtectedOperationKind {

POK_VarDereference,


POK_VarAccess,


POK_FunctionCall,


POK_PassByRef,


POK_PtPassByRef
};




enum LockKind {

LK_Shared,


LK_Exclusive,


LK_Generic
};



enum AccessKind {

AK_Read,


AK_Written
};









enum LockErrorKind {
LEK_LockedSomeLoopIterations,
LEK_LockedSomePredecessors,
LEK_LockedAtEndOfFunction,
LEK_NotLockedAtEndOfFunction
};


class ThreadSafetyHandler {
public:
using Name = StringRef;

ThreadSafetyHandler() = default;
virtual ~ThreadSafetyHandler();




virtual void handleInvalidLockExp(StringRef Kind, SourceLocation Loc) {}








virtual void handleUnmatchedUnlock(StringRef Kind, Name LockName,
SourceLocation Loc,
SourceLocation LocPreviousUnlock) {}











virtual void handleIncorrectUnlockKind(StringRef Kind, Name LockName,
LockKind Expected, LockKind Received,
SourceLocation LocLocked,
SourceLocation LocUnlock) {}







virtual void handleDoubleLock(StringRef Kind, Name LockName,
SourceLocation LocLocked,
SourceLocation LocDoubleLock) {}














virtual void handleMutexHeldEndOfScope(StringRef Kind, Name LockName,
SourceLocation LocLocked,
SourceLocation LocEndOfScope,
LockErrorKind LEK) {}









virtual void handleExclusiveAndShared(StringRef Kind, Name LockName,
SourceLocation Loc1,
SourceLocation Loc2) {}







virtual void handleNoMutexHeld(StringRef Kind, const NamedDecl *D,
ProtectedOperationKind POK, AccessKind AK,
SourceLocation Loc) {}










virtual void handleMutexNotHeld(StringRef Kind, const NamedDecl *D,
ProtectedOperationKind POK, Name LockName,
LockKind LK, SourceLocation Loc,
Name *PossibleMatch = nullptr) {}








virtual void handleNegativeNotHeld(StringRef Kind, Name LockName, Name Neg,
SourceLocation Loc) {}






virtual void handleNegativeNotHeld(const NamedDecl *D, Name LockName,
SourceLocation Loc) {}








virtual void handleFunExcludesLock(StringRef Kind, Name FunName,
Name LockName, SourceLocation Loc) {}


virtual void handleLockAcquiredBefore(StringRef Kind, Name L1Name,
Name L2Name, SourceLocation Loc) {}


virtual void handleBeforeAfterCycle(Name L1Name, SourceLocation Loc) {}



virtual void enterFunction(const FunctionDecl *FD) {}


virtual void leaveFunction(const FunctionDecl *FD) {}

bool issueBetaWarnings() { return IssueBetaWarnings; }
void setIssueBetaWarnings(bool b) { IssueBetaWarnings = b; }

private:
bool IssueBetaWarnings = false;
};






void runThreadSafetyAnalysis(AnalysisDeclContext &AC,
ThreadSafetyHandler &Handler,
BeforeSet **Bset);

void threadSafetyCleanup(BeforeSet *Cache);



LockKind getLockKindFromAccessKind(AccessKind AK);

}
}

#endif
