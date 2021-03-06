







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICPARSE_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICPARSE_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define PARSESTART
#include "clang/Basic/DiagnosticParseKinds.inc"
#undef DIAG
NUM_BUILTIN_PARSE_DIAGNOSTICS
};
}
}

#endif
