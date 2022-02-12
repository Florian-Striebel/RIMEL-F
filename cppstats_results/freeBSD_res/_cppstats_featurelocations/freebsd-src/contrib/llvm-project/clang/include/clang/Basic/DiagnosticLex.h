







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICLEX_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICLEX_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define LEXSTART
#include "clang/Basic/DiagnosticLexKinds.inc"
#undef DIAG
NUM_BUILTIN_LEX_DIAGNOSTICS
};
}
}

#endif
