












#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_SMARTPTR_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_SMARTPTR_H

#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"

namespace clang {
namespace ento {
namespace smartptr {


bool isStdSmartPtrCall(const CallEvent &Call);
bool isStdSmartPtr(const CXXRecordDecl *RD);
bool isStdSmartPtr(const Expr *E);

bool isStdSmartPtr(const CXXRecordDecl *RD);


bool isNullSmartPtr(const ProgramStateRef State, const MemRegion *ThisRegion);

const BugType *getNullDereferenceBugType();

}
}
}

#endif
