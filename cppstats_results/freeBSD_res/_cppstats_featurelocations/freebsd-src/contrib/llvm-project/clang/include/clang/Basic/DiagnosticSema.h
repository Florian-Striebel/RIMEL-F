







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICSEMA_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICSEMA_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define SEMASTART
#include "clang/Basic/DiagnosticSemaKinds.inc"
#undef DIAG
NUM_BUILTIN_SEMA_DIAGNOSTICS
};
}
}

#endif
