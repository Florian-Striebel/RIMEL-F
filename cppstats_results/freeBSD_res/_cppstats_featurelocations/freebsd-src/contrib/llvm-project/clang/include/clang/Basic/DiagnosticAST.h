







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICAST_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICAST_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define ASTSTART
#include "clang/Basic/DiagnosticASTKinds.inc"
#undef DIAG
NUM_BUILTIN_AST_DIAGNOSTICS
};
}
}

#endif
