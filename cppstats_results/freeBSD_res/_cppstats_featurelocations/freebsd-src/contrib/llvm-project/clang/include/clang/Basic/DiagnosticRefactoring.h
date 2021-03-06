







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICREFACTORING_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICREFACTORING_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define REFACTORINGSTART
#include "clang/Basic/DiagnosticRefactoringKinds.inc"
#undef DIAG
NUM_BUILTIN_REFACTORING_DIAGNOSTICS
};
}
}

#endif
