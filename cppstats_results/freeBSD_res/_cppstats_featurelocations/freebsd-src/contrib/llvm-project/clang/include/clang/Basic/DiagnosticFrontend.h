







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICFRONTEND_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICFRONTEND_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define FRONTENDSTART
#include "clang/Basic/DiagnosticFrontendKinds.inc"
#undef DIAG
NUM_BUILTIN_FRONTEND_DIAGNOSTICS
};
}
}

#endif
