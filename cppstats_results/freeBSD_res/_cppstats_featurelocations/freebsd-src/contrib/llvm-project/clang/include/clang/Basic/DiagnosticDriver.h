







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTICDRIVER_H)
#define LLVM_CLANG_BASIC_DIAGNOSTICDRIVER_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR, SHOWINSYSHEADER, DEFERRABLE, CATEGORY) ENUM,


#define DRIVERSTART
#include "clang/Basic/DiagnosticDriverKinds.inc"
#undef DIAG
NUM_BUILTIN_DRIVER_DIAGNOSTICS
};
}
}

#endif
